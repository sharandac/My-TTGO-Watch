#include "parkinsense_data.h"
#include "config.h"
#include "callback.h"
#include "utils/logging.h"
#include "utils/circular_buff.h"
#include "utils/psram_accel_alloc.h"
#include <stdio.h>
#ifndef NATIVE_64BIT
#include "hardware/ble/parkinsense_ble.h"
#include "hardware/blectl.h"
#include "hardware/motion.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#endif

#define PARKINSENSE_DATA_PIPELINE_BUFFER_SIZE 1024
/** Number of samples per PSRAM block (contiguous chunk we copy from circular buffer).
 *  32 keeps each JSON message ~1 KB → only 3-5 BLE notifications instead of 30+,
 *  which drastically reduces the chance of data loss from a dropped notification. */
#define PARKINSENSE_PSRAM_BLOCK_SAMPLES 32
/** Max formatted JSON size per block (32 samples * ~30 chars + header ≈ 1.2 KB). */
#define PARKINSENSE_SEND_BUF_SIZE (2 * 1024)
/** Task stack size and priority for the pipeline task. */
#define PARKINSENSE_TASK_STACK 4096
#define PARKINSENSE_TASK_PRIO 1
/** Delay between pipeline iterations (ms) – ~50 Hz sample rate. */
#define PARKINSENSE_TASK_DELAY_MS 20

static circular_buff_t parkinsense_data_pipeline_buff;
static bool parkinsense_data_pipeline_initialized = false;
static bool parkinsense_data_pipeline_started = false;
#ifndef NATIVE_64BIT
static TaskHandle_t parkinsense_data_task_handle = NULL;
#endif
static char parkinsense_send_buf[PARKINSENSE_SEND_BUF_SIZE];
static callback_t* parkinsense_data_callback = NULL;
/** Single block buffer for drain; static to avoid large stack use in pipeline task. */
static bma_accel_data_t parkinsense_block_buf[PARKINSENSE_PSRAM_BLOCK_SAMPLES];
#ifndef NATIVE_64BIT
static SemaphoreHandle_t parkinsense_psram_mutex = NULL;
static uint32_t parkinsense_last_auto_send_ms = 0;
static unsigned parkinsense_blocks_sent = 0;
static unsigned parkinsense_send_fails = 0;
static unsigned parkinsense_send_attempts = 0;
static unsigned parkinsense_send_early_returns = 0;
static void parkinsense_task_loop(void* pv);
#endif

void parkinsense_data_pipeline_init(void) {
    if (parkinsense_data_pipeline_initialized) return;
    log_i("Parkinsense data pipeline initialized");
    circular_buff_init(&parkinsense_data_pipeline_buff);
    psram_init();
    parkinsense_data_pipeline_initialized = true;
    parkinsense_data_pipeline_started = false;
#ifndef NATIVE_64BIT
    BaseType_t ok = xTaskCreate(
        parkinsense_task_loop,
        "parkinsense",
        PARKINSENSE_TASK_STACK,
        NULL,
        PARKINSENSE_TASK_PRIO,
        &parkinsense_data_task_handle
    );
    parkinsense_psram_mutex = xSemaphoreCreateMutex();
    if (!parkinsense_psram_mutex)
        log_e("Parkinsense pipeline mutex create failed");
    if (ok == pdPASS && parkinsense_data_task_handle)
        vTaskSuspend(parkinsense_data_task_handle);
    else
        log_e("Parkinsense pipeline task create failed");
#endif
}

void parkinsense_data_pipeline_start(void) {
    log_i("Parkinsense data pipeline started");
    parkinsense_data_pipeline_started = true;
#ifndef NATIVE_64BIT
    parkinsense_last_auto_send_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
    if (parkinsense_data_task_handle)
        vTaskResume(parkinsense_data_task_handle);
#endif
}

void parkinsense_data_pipeline_stop(void) {
    log_i("Parkinsense data pipeline stopped");
    parkinsense_data_pipeline_started = false;
#ifndef NATIVE_64BIT
    if (parkinsense_data_task_handle)
        vTaskSuspend(parkinsense_data_task_handle);
#endif
}

void parkinsense_data_pipeline_reset(void) {
    log_i("Parkinsense data pipeline reset");
#ifndef NATIVE_64BIT
    if (parkinsense_psram_mutex && xSemaphoreTake(parkinsense_psram_mutex, pdMS_TO_TICKS(1000)) == pdTRUE) {
        unsigned freed = 0;
        while (first_accel_block_ptr && freed < PSRAM_ACCEL_MAX_ITERATIONS) {
            psram_free_first_block();
            freed++;
        }
        if (first_accel_block_ptr)
            log_e("Parkinsense reset: hit max iterations, list may be corrupt");
        psram_init();
        xSemaphoreGive(parkinsense_psram_mutex);
    } else if (parkinsense_psram_mutex) {
        log_w("Parkinsense reset: mutex take timeout, clearing buffer only");
    } else {
        log_w("Parkinsense reset: no mutex, clearing buffer only");
    }
#else
    {
        unsigned freed = 0;
        while (first_accel_block_ptr && freed < PSRAM_ACCEL_MAX_ITERATIONS) {
            psram_free_first_block();
            freed++;
        }
        if (first_accel_block_ptr)
            log_e("Parkinsense reset: hit max iterations, list may be corrupt");
        psram_init();
    }
#endif
    circular_buff_init(&parkinsense_data_pipeline_buff);
}

bool parkinsense_data_pipeline_send_data(void) {
#ifndef NATIVE_64BIT
    parkinsense_send_attempts++;
    if (!parkinsense_psram_mutex) { log_w("send: no mutex"); parkinsense_send_early_returns++; return false; }
    if (xSemaphoreTake(parkinsense_psram_mutex, pdMS_TO_TICKS(500)) != pdTRUE) {
        log_w("send: mutex timeout");
        parkinsense_send_early_returns++;
        return false;
    }
    bool sent_ok = false;
    if (first_accel_block_ptr) {
        header_t* block = first_accel_block_ptr;
        if (!psram_validate_block(block)) {
            log_w("send: validate fail seq=%lu, freeing", (unsigned long)block->seq_num);
            psram_free_first_block();
        } else {
            bma_accel_data_t* samples = (bma_accel_data_t*)((char*)block + sizeof(header_t));
            uint32_t n = block->num_elements;
            if (n > PSRAM_ACCEL_MAX_ELEMENTS) {
                log_w("send: n=%lu > max, freeing", (unsigned long)n);
                psram_free_first_block();
            } else {
                size_t buf_size = sizeof(parkinsense_send_buf);
                int len = 0;
                int r = snprintf(parkinsense_send_buf, buf_size,
                    "{\"t\":\"parkinsense\",\"seq\":%lu,\"start\":%lu,\"end\":%lu,\"n\":%lu,\"s\":[",
                    (unsigned long)block->seq_num, (unsigned long)block->start_time_ms,
                    (unsigned long)block->end_time_ms, (unsigned long)n);
                if (r > 0 && (size_t)r < buf_size) {
                    len = r;
                    for (uint32_t i = 0; i < n; i++) {
                        size_t rem = buf_size - (size_t)len;
                        if (rem < 32) break;
                        r = snprintf(parkinsense_send_buf + len, rem,
                            "%s[%d,%d,%d,%lu]", i ? "," : "",
                            (int)samples[i].x, (int)samples[i].y, (int)samples[i].z,
                            (unsigned long)samples[i].timestamp_ms);
                        if (r < 0 || (size_t)r >= rem) break;
                        len += r;
                    }
                    if (len > 0 && (size_t)len < buf_size - 4) {
                        r = snprintf(parkinsense_send_buf + len, buf_size - (size_t)len, "]}");
                        if (r > 0) len += r;
                    }
                }
                xSemaphoreGive(parkinsense_psram_mutex);
                log_i("send: seq=%lu len=%d calling ble_send", (unsigned long)block->seq_num, len);
                if (len > 0 && (size_t)len < buf_size) {
                    sent_ok = parkinsense_ble_send(parkinsense_send_buf, (size_t)len);
                    if (!sent_ok) parkinsense_send_fails++;
                }
                log_i("send: seq=%lu ble_send=%s R=%d", (unsigned long)block->seq_num,
                    sent_ok ? "OK" : "FAIL", parkinsense_ble_last_fail_reason());
                if (xSemaphoreTake(parkinsense_psram_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
                    if (first_accel_block_ptr) {
                        first_accel_block_ptr->sync = true;
                        psram_free_first_block();
                    }
                    xSemaphoreGive(parkinsense_psram_mutex);
                }
                if (sent_ok) parkinsense_blocks_sent++;
                if (parkinsense_data_callback)
                    callback_send(parkinsense_data_callback, PARKINSENSE_SEND_DONE, NULL);
                return sent_ok;
            }
        }
    } else {
        parkinsense_send_early_returns++;
    }
    xSemaphoreGive(parkinsense_psram_mutex);
    return false;
#else
    log_i("Parkinsense data pipeline send (native stub)");
    return true;
#endif
}

/** Get one BMA sample and push into the circular buffer. Call from pipeline task at sample rate. */
void parkinsense_data_pipeline_get_data(){
    bma_accel_data_t data = bma_get_accel();
    circular_buff_write(data, &parkinsense_data_pipeline_buff);
}

/** Current number of samples in the pipeline circular buffer (0..PARKINSENSE_DATA_PIPELINE_BUFFER_SIZE). */
int parkinsense_data_pipeline_buff_count(void) {
    return circular_buff_count(&parkinsense_data_pipeline_buff);
}

bool parkinsense_data_pipeline_is_started(void) {
    return parkinsense_data_pipeline_started;
}

unsigned parkinsense_data_get_blocks_sent(void) { return parkinsense_blocks_sent; }
unsigned parkinsense_data_get_send_fails(void) { return parkinsense_send_fails; }
unsigned parkinsense_data_get_send_attempts(void) { return parkinsense_send_attempts; }
unsigned parkinsense_data_get_early_returns(void) { return parkinsense_send_early_returns; }
unsigned parkinsense_data_get_psram_queued(void) {
#ifndef NATIVE_64BIT
    unsigned count = 0;
    if (parkinsense_psram_mutex && xSemaphoreTake(parkinsense_psram_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        header_t* p = first_accel_block_ptr;
        while (p && count < 9999) {
            count++;
            p = (header_t*)p->next;
        }
        xSemaphoreGive(parkinsense_psram_mutex);
    }
    return count;
#else
    return 0;
#endif
}

/** When buffer has reached 1024 (or any time there is at least one full block of 256), drain full blocks to PSRAM. Call from pipeline task after get_data or on a timer. */
void parkinsense_data_pipeline_drain_if_full(void) {
    unsigned drained = 0;
    while (circular_buff_count(&parkinsense_data_pipeline_buff) >= PARKINSENSE_PSRAM_BLOCK_SAMPLES
           && drained < 4) {
        parkinsense_data_pipeline_write_to_psram();
        drained++;
    }
}

/** Drain one block from the circular buffer into a contiguous buffer, then write that block to PSRAM. Call when the circular buffer has at least PARKINSENSE_PSRAM_BLOCK_SAMPLES (or drain what you have). Uses static buffer to avoid large stack in pipeline task. */
void parkinsense_data_pipeline_write_to_psram() {
    int count = 0;
    while (count < PARKINSENSE_PSRAM_BLOCK_SAMPLES && circular_buff_read(&parkinsense_data_pipeline_buff, &parkinsense_block_buf[count])) {
        count++;
    }
    if (count == 0) {
        return;
    }
#ifndef NATIVE_64BIT
    if (parkinsense_psram_mutex && xSemaphoreTake(parkinsense_psram_mutex, pdMS_TO_TICKS(500)) != pdTRUE)
        return;
#endif
    header_t* block = psram_write(count, parkinsense_block_buf);
#ifndef NATIVE_64BIT
    if (parkinsense_psram_mutex) xSemaphoreGive(parkinsense_psram_mutex);
#endif
    if (block) {
        log_i("Parkinsense data pipeline wrote block to PSRAM: %u samples, seq=%lu", (unsigned)count, (unsigned long)block->seq_num);
    } else {
        log_e("Parkinsense data pipeline write to PSRAM failed");
    }
}

void parkinsense_data_pipeline_register_cb(EventBits_t event, CALLBACK_FUNC callback_func, const char* id) {
    if (!parkinsense_data_callback) {
        parkinsense_data_callback = callback_init("parkinsense_data");
        if (!parkinsense_data_callback) return;
    }
    callback_register(parkinsense_data_callback, event, callback_func, id);
}

void parkinsense_data_pipeline_unregister_cb(EventBits_t event, const char* id) {
    (void)event;
    (void)id;
    /* callback_unregister not available in this codebase; no-op */
}

#ifndef NATIVE_64BIT
static void parkinsense_task_loop(void* pv) {
    (void)pv;
    for (;;) {
        if (parkinsense_data_pipeline_started) {
            parkinsense_data_pipeline_get_data();
            parkinsense_data_pipeline_drain_if_full();

            uint32_t now = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
            if (first_accel_block_ptr &&
                (now - parkinsense_last_auto_send_ms) >= PARKINSENSE_AUTO_SEND_INTERVAL_MS) {
                parkinsense_last_auto_send_ms = now;
                parkinsense_data_pipeline_send_data();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(PARKINSENSE_TASK_DELAY_MS));
    }
}
#endif

