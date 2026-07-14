#include "parkinsense_data.h"
#include "config.h"
#include "callback.h"
#include "utils/logging.h"
#include "utils/circular_buff.h"
#include "utils/psram_accel_alloc.h"
#include "utils/psram_queue_cap.h"
#include "utils/parkinsense_build_config.h"
#include <stdio.h>
#ifndef NATIVE_64BIT
#include "hardware/ble/parkinsense_ble.h"
#include "hardware/blectl.h"
#include "hardware/motion.h"
#include "hardware/bma_fifo.h"
#include "hardware/powermgm.h"
#include "NimBLEDevice.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <sys/time.h>
#endif

#define PARKINSENSE_DATA_PIPELINE_BUFFER_SIZE 1024
/** Number of samples per PSRAM block (contiguous chunk we copy from circular buffer).
 *  32 keeps each JSON message ~1 KB → only 3-5 BLE notifications instead of 30+,
 *  which drastically reduces the chance of data loss from a dropped notification. */
#define PARKINSENSE_PSRAM_BLOCK_SAMPLES 32
/** Max formatted JSON size per block (32 samples * ~30 chars + header ≈ 1.2 KB). */
#ifdef PARKINSENSE_BLE_BINARY
#define PARKINSENSE_SEND_BUF_SIZE (8 * 1024)
#define PARKINSENSE_BINARY_MAX_BLOCKS 8
#define PKSD_HEADER_SIZE 16
#define PKSD_BLOCK_HDR 14
#define PKSD_SAMPLE_SIZE 12
#else
#define PARKINSENSE_SEND_BUF_SIZE (2 * 1024)
#endif
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
static uint8_t parkinsense_send_buf[PARKINSENSE_SEND_BUF_SIZE];
static callback_t* parkinsense_data_callback = NULL;
/** Single block buffer for drain; static to avoid large stack use in pipeline task. */
static bma_accel_data_t parkinsense_block_buf[PARKINSENSE_PSRAM_BLOCK_SAMPLES];
#ifndef NATIVE_64BIT
static SemaphoreHandle_t parkinsense_psram_mutex = NULL;
/** Wall-clock ms of last hourly window open (RTC-backed after time sync). */
static uint64_t parkinsense_last_hourly_wall_ms = 0;
static uint64_t parkinsense_sync_window_end_wall_ms = 0;
static bool parkinsense_sync_window_active = false;
static unsigned parkinsense_blocks_sent = 0;
#ifdef PARKINSENSE_BLE_BINARY
static uint32_t parkinsense_binary_batch_seq = 0;
#endif
static unsigned parkinsense_send_fails = 0;
static unsigned parkinsense_send_attempts = 0;
static unsigned parkinsense_send_early_returns = 0;
static unsigned parkinsense_psram_blocks_dropped = 0;
#ifdef PARKINSENSE_BLE_UNSECURED
/** Timer-based BLE send cadence; interval scales with negotiated MTU. */
static uint32_t parkinsense_last_auto_send_ms = 0;
#endif
static void parkinsense_task_loop(void* pv);
static bool parkinsense_ble_client_connected(void);
static void parkinsense_drain_psram_queue(void);
static void parkinsense_hourly_sync_tick(uint64_t now_wall_ms);
static uint64_t parkinsense_wall_clock_ms(void);
static bool parkinsense_powermgm_event_cb(EventBits_t event, void* arg);
static bool parkinsense_ble_sync_standby_cb(EventBits_t event, void* arg);
static bool parkinsense_blectl_event_cb(EventBits_t event, void *arg);
static void parkinsense_enforce_psram_queue_cap(void);
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
    if (ok != pdPASS || !parkinsense_data_task_handle)
        log_e("Parkinsense pipeline task create failed");
    parkinsense_last_hourly_wall_ms = parkinsense_wall_clock_ms();
    powermgm_register_cb(
        POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP,
        parkinsense_powermgm_event_cb,
        "parkinsense hourly"
    );
    powermgm_register_cb(
        POWERMGM_STANDBY,
        parkinsense_ble_sync_standby_cb,
        "parkinsense ble sync"
    );
    blectl_register_cb( BLECTL_CONNECT | BLECTL_DISCONNECT, parkinsense_blectl_event_cb, "parkinsense blectl" );
    if (parkinsense_build_is_cohort()) {
        log_i(
            "Parkinsense COHORT build label=%s binary=%d auto_start=%d psram=%d bma_fifo=%d",
            parkinsense_build_label(),
            parkinsense_build_ble_binary() ? 1 : 0,
            parkinsense_build_auto_start_on_boot() ? 1 : 0,
            parkinsense_build_bma_fifo() ? 1 : 0,
#if defined(BOARD_HAS_PSRAM)
            1
#else
            0
#endif
        );
    }
#ifdef PARKINSENSE_TEST_HOURLY_INTERVAL_MS
    log_i(
        "Parkinsense TEST hourly: interval=%lu ms, window=%lu ms",
        (unsigned long)PARKINSENSE_HOURLY_SEND_INTERVAL_MS,
        (unsigned long)PARKINSENSE_HOURLY_SYNC_WINDOW_MS
    );
#endif
    if (parkinsense_build_auto_start_on_boot()) {
        parkinsense_data_pipeline_start();
    }
#endif
}

void parkinsense_data_pipeline_start(void) {
    log_i("Parkinsense data pipeline started (motion-gated FIFO)");
    parkinsense_data_pipeline_started = true;
    /* Do not reset parkinsense_last_hourly_wall_ms — Play must not delay the hourly window. */
    /* FIFO setup deferred until BMA423 wakeup/tilt — see bma_fifo_on_motion(). */
}

void parkinsense_data_pipeline_stop(void) {
#ifndef NATIVE_64BIT
    if (bma_fifo_is_enabled()) {
        bma_fifo_teardown();
    }
#endif
    log_i("Parkinsense data pipeline stopped");
    parkinsense_data_pipeline_started = false;
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


#ifdef PARKINSENSE_BLE_BINARY
static void pksd_write_u16(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xff);
    p[1] = (uint8_t)((v >> 8) & 0xff);
}

static void pksd_write_u32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xff);
    p[1] = (uint8_t)((v >> 8) & 0xff);
    p[2] = (uint8_t)((v >> 16) & 0xff);
    p[3] = (uint8_t)((v >> 24) & 0xff);
}

static size_t pksd_block_wire_size(header_t *block) {
    return PKSD_BLOCK_HDR + (size_t)block->num_elements * PKSD_SAMPLE_SIZE;
}

static bool parkinsense_data_pipeline_send_binary_batch(void) {
#ifndef NATIVE_64BIT
    parkinsense_send_attempts++;
    if (!parkinsense_psram_mutex) {
        parkinsense_send_early_returns++;
        return false;
    }
    if (xSemaphoreTake(parkinsense_psram_mutex, pdMS_TO_TICKS(500)) != pdTRUE) {
        parkinsense_send_early_returns++;
        return false;
    }

    header_t *batch_blocks[PARKINSENSE_BINARY_MAX_BLOCKS];
    unsigned batch_count = 0;
    size_t payload = 0;
    header_t *walk = first_accel_block_ptr;
    while (walk && batch_count < PARKINSENSE_BINARY_MAX_BLOCKS) {
        if (!psram_validate_block(walk)) {
            header_t *bad = walk;
            walk = (header_t *)walk->next;
            if (bad == first_accel_block_ptr)
                psram_free_first_block();
            continue;
        }
        size_t need = pksd_block_wire_size(walk);
        if (PKSD_HEADER_SIZE + payload + need > sizeof(parkinsense_send_buf))
            break;
        batch_blocks[batch_count++] = walk;
        payload += need;
        walk = (header_t *)walk->next;
    }

    if (batch_count == 0) {
        xSemaphoreGive(parkinsense_psram_mutex);
        parkinsense_send_early_returns++;
        return false;
    }

    const uint32_t frame_len = (uint32_t)(PKSD_HEADER_SIZE + payload);
    uint8_t *out = parkinsense_send_buf;
    out[0] = 'P';
    out[1] = 'K';
    out[2] = 'S';
    out[3] = 'D';
    out[4] = 1;
    out[5] = 0;
    pksd_write_u32(out + 6, parkinsense_binary_batch_seq++);
    pksd_write_u32(out + 10, frame_len);
    pksd_write_u16(out + 14, (uint16_t)batch_count);

    size_t pos = PKSD_HEADER_SIZE;
    for (unsigned i = 0; i < batch_count; i++) {
        header_t *block = batch_blocks[i];
        bma_accel_data_t *samples = (bma_accel_data_t *)((char *)block + sizeof(header_t));
        pksd_write_u32(out + pos, block->seq_num);
        pos += 4;
        pksd_write_u32(out + pos, block->start_time_ms);
        pos += 4;
        pksd_write_u32(out + pos, block->end_time_ms);
        pos += 4;
        pksd_write_u16(out + pos, (uint16_t)block->num_elements);
        pos += 2;
        for (uint32_t s = 0; s < block->num_elements; s++) {
            pksd_write_u16(out + pos, (uint16_t)samples[s].x);
            pos += 2;
            pksd_write_u16(out + pos, (uint16_t)samples[s].y);
            pos += 2;
            pksd_write_u16(out + pos, (uint16_t)samples[s].z);
            pos += 2;
            pksd_write_u32(out + pos, samples[s].timestamp_ms);
            pos += 4;
        }
    }

    xSemaphoreGive(parkinsense_psram_mutex);

    log_i("send: binary batch=%lu blocks=%u len=%lu",
        (unsigned long)(parkinsense_binary_batch_seq - 1), (unsigned)batch_count, (unsigned long)frame_len);
    bool sent_ok = parkinsense_ble_send_binary(out, frame_len);
    if (!sent_ok) {
        parkinsense_send_fails++;
        return false;
    }

    if (xSemaphoreTake(parkinsense_psram_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
        for (unsigned i = 0; i < batch_count; i++) {
            (void)batch_blocks[i];
            if (first_accel_block_ptr) {
                first_accel_block_ptr->sync = true;
                psram_free_first_block();
            }
        }
        xSemaphoreGive(parkinsense_psram_mutex);
    }
    parkinsense_blocks_sent += batch_count;
    if (parkinsense_data_callback)
        callback_send(parkinsense_data_callback, PARKINSENSE_SEND_DONE, NULL);
    return true;
#else
    return true;
#endif
}
#endif

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
                int r = snprintf((char *)parkinsense_send_buf, buf_size,
                    "{\"t\":\"parkinsense\",\"seq\":%lu,\"start\":%lu,\"end\":%lu,\"n\":%lu,\"s\":[",
                    (unsigned long)block->seq_num, (unsigned long)block->start_time_ms,
                    (unsigned long)block->end_time_ms, (unsigned long)n);
                if (r > 0 && (size_t)r < buf_size) {
                    len = r;
                    for (uint32_t i = 0; i < n; i++) {
                        size_t rem = buf_size - (size_t)len;
                        if (rem < 32) break;
                        r = snprintf((char *)parkinsense_send_buf + len, rem,
                            "%s[%d,%d,%d,%lu]", i ? "," : "",
                            (int)samples[i].x, (int)samples[i].y, (int)samples[i].z,
                            (unsigned long)samples[i].timestamp_ms);
                        if (r < 0 || (size_t)r >= rem) break;
                        len += r;
                    }
                    if (len > 0 && (size_t)len < buf_size - 4) {
                        r = snprintf((char *)parkinsense_send_buf + len, buf_size - (size_t)len, "]}");
                        if (r > 0) len += r;
                    }
                }
                xSemaphoreGive(parkinsense_psram_mutex);
                log_i("send: seq=%lu len=%d calling ble_send", (unsigned long)block->seq_num, len);
                if (len > 0 && (size_t)len < buf_size) {
                    sent_ok = parkinsense_ble_send((const char *)parkinsense_send_buf, (size_t)len);
                    if (!sent_ok) parkinsense_send_fails++;
                }
                log_i("send: seq=%lu ble_send=%s R=%d", (unsigned long)block->seq_num,
                    sent_ok ? "OK" : "FAIL", parkinsense_ble_last_fail_reason());
                if (sent_ok) {
                    if (xSemaphoreTake(parkinsense_psram_mutex, pdMS_TO_TICKS(500)) == pdTRUE) {
                        if (first_accel_block_ptr == block) {
                            block->sync = true;
                            psram_free_first_block();
                        }
                        xSemaphoreGive(parkinsense_psram_mutex);
                    }
                    parkinsense_blocks_sent++;
                }
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
void parkinsense_data_pipeline_push_sample(bma_accel_data_t data) {
    circular_buff_write(data, &parkinsense_data_pipeline_buff);
}

void parkinsense_data_pipeline_get_data(){
    parkinsense_data_pipeline_push_sample(bma_get_accel());
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
unsigned parkinsense_data_get_psram_blocks_dropped(void) { return parkinsense_psram_blocks_dropped; }

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

static void parkinsense_enforce_psram_queue_cap(void) {
#ifndef NATIVE_64BIT
    if (!parkinsense_psram_mutex) {
        return;
    }
    if (xSemaphoreTake(parkinsense_psram_mutex, pdMS_TO_TICKS(500)) != pdTRUE) {
        return;
    }
    unsigned queued = 0;
    header_t* p = first_accel_block_ptr;
    while (p && queued < 9999) {
        queued++;
        p = (header_t*)p->next;
    }
    const size_t drops = psram_queue_cap_drops_needed(
        queued,
        (size_t)PARKINSENSE_MAX_PSRAM_QUEUE_BLOCKS
    );
    for (size_t i = 0; i < drops && first_accel_block_ptr; i++) {
        psram_free_first_block();
        parkinsense_psram_blocks_dropped++;
    }
    xSemaphoreGive(parkinsense_psram_mutex);
#endif
}


void parkinsense_data_pipeline_write_to_psram() {
    int count = 0;
    while (count < PARKINSENSE_PSRAM_BLOCK_SAMPLES && circular_buff_read(&parkinsense_data_pipeline_buff, &parkinsense_block_buf[count])) {
        count++;
    }
    if (count == 0) {
        return;
    }
#ifndef NATIVE_64BIT
    parkinsense_enforce_psram_queue_cap();
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
/** Epoch ms; uses RTC when set (survives light sleep), else FreeRTOS tick ms. */
static uint64_t parkinsense_wall_clock_ms(void) {
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) != 0) {
        return (uint64_t)xTaskGetTickCount() * (uint64_t)portTICK_PERIOD_MS;
    }
    /* Before first time sync, tv_sec is often near boot — fall back to ticks. */
    if (tv.tv_sec < 1577836800L) {
        return (uint64_t)xTaskGetTickCount() * (uint64_t)portTICK_PERIOD_MS;
    }
    return (uint64_t)tv.tv_sec * 1000ULL + (uint64_t)(tv.tv_usec / 1000);
}

static bool parkinsense_powermgm_event_cb(EventBits_t event, void* arg) {
    (void)arg;
    if (event == POWERMGM_SILENCE_WAKEUP || event == POWERMGM_WAKEUP) {
        parkinsense_data_hourly_sync_poll();
    }
    return true;
}


/** Block standby during hourly sync window or while phone is connected. */
static bool parkinsense_ble_sync_standby_cb(EventBits_t event, void *arg) {
    (void)arg;
    if (event != POWERMGM_STANDBY) {
        return true;
    }
    if (!parkinsense_data_pipeline_is_started()) {
        return true;
    }
    if (parkinsense_data_ble_client_connected()) {
        log_i("[PWR_BLE] parkinsense: standby blocked — phone connected");
        return false;
    }
#ifndef PARKINSENSE_STREAM_WHILE_CONNECTED
    if (parkinsense_data_is_hourly_sync_window_active()) {
        log_i("[PWR_BLE] parkinsense: standby blocked — hourly sync window");
        return false;
    }
#endif
    return true;
}

static bool parkinsense_blectl_event_cb(EventBits_t event, void *arg) {
    (void)arg;
#ifdef PARKINSENSE_BLE_UNSECURED
    if (event == BLECTL_CONNECT) {
        parkinsense_last_auto_send_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
        log_i("[PWR_BLE] parkinsense: BLE connect — send cadence reset");
    }
#else
    (void)event;
#endif
    return true;
}

void parkinsense_data_hourly_sync_poll(void) {
    parkinsense_hourly_sync_tick(parkinsense_wall_clock_ms());
}

bool parkinsense_data_is_hourly_sync_window_active(void) {
#ifdef NATIVE_64BIT
    return false;
#else
#ifdef PARKINSENSE_STREAM_WHILE_CONNECTED
    return false;
#else
    return parkinsense_sync_window_active;
#endif
#endif
}

bool parkinsense_data_ble_client_connected(void) {
#ifndef NATIVE_64BIT
    NimBLEServer *pServer = blectl_get_ble_server();
    return pServer && pServer->getConnectedCount() > 0;
#else
    return false;
#endif
}

static bool parkinsense_ble_client_connected(void) {
    return parkinsense_data_ble_client_connected();
}

static void parkinsense_drain_psram_queue(void) {
#ifdef PARKINSENSE_BLE_BINARY
    if (!parkinsense_ble_ready_to_send())
        return;
    if (first_accel_block_ptr && parkinsense_ble_client_connected()) {
        if (parkinsense_data_pipeline_send_binary_batch())
            vTaskDelay(pdMS_TO_TICKS(20));
    }
#else
    if (!parkinsense_ble_ready_to_send())
        return;
    const unsigned max_blocks_per_tick = 8;
    unsigned sent = 0;
    while (first_accel_block_ptr && parkinsense_ble_client_connected() && sent < max_blocks_per_tick) {
        if (!parkinsense_data_pipeline_send_data())
            break;
        sent++;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
#endif
}

static void parkinsense_hourly_sync_tick(uint64_t now_wall_ms) {
    const bool has_queue = first_accel_block_ptr != nullptr;

#ifdef PARKINSENSE_STREAM_WHILE_CONNECTED
#ifdef PARKINSENSE_BLE_UNSECURED
    /*
     * Unsecured dev: send one queued block per second while connected (matches
     * integration003 + ble-refactor). Gives the phone time to subscribe before data flows.
     */
    (void)now_wall_ms;
    if (has_queue && parkinsense_ble_client_connected()) {
        if (!parkinsense_ble_ready_to_send()) {
            parkinsense_ble_log_wait_state_if_stuck();
            return;
        }
        uint32_t now_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
        const uint32_t interval_ms = parkinsense_ble_send_interval_ms();
        if ((now_ms - parkinsense_last_auto_send_ms) >= interval_ms) {
            parkinsense_last_auto_send_ms = now_ms;
            parkinsense_data_pipeline_send_data();
        }
    }
#else
    /*
     * Secured dev: drain the PSRAM queue whenever a BLE client is connected.
     */
    (void)now_wall_ms;
    if (has_queue && parkinsense_ble_client_connected())
        parkinsense_drain_psram_queue();
#endif
    return;
#else

    if (!parkinsense_sync_window_active) {
        if ((now_wall_ms - parkinsense_last_hourly_wall_ms) < PARKINSENSE_HOURLY_SEND_INTERVAL_MS)
            return;
        parkinsense_last_hourly_wall_ms = now_wall_ms;
        parkinsense_sync_window_active = true;
        parkinsense_sync_window_end_wall_ms = now_wall_ms + PARKINSENSE_HOURLY_SYNC_WINDOW_MS;
        blectl_on_hourly_advertise_window();
        log_i("hourly sync: window opened, queued=%u", parkinsense_data_get_psram_queued());
        return;
    }

    if (now_wall_ms >= parkinsense_sync_window_end_wall_ms) {
        parkinsense_sync_window_active = false;
        log_i("hourly sync: window closed (timeout), queued=%u", parkinsense_data_get_psram_queued());
        return;
    }

    if (!has_queue) {
        if (parkinsense_ble_client_connected()) {
            parkinsense_sync_window_active = false;
            log_i("hourly sync: window closed (queue drained)");
        } else {
            blectl_on_hourly_advertise_window();
        }
        return;
    }

    if ( !parkinsense_ble_client_connected() )
        blectl_on_hourly_advertise_window();

    if (parkinsense_ble_client_connected())
        parkinsense_drain_psram_queue();
#endif
}

static void parkinsense_task_loop(void* pv) {
    (void)pv;
    for (;;) {
        if (parkinsense_data_pipeline_started) {
#ifdef PARKINSENSE_BMA_FIFO
            if (bma_fifo_is_enabled()) {
                bma_fifo_idle_tick();
                if (bma_fifo_is_active()) {
                    bma_fifo_service();
                }
            } else
#endif
            {
                parkinsense_data_pipeline_get_data();
                parkinsense_data_pipeline_drain_if_full();
            }
        }

        parkinsense_hourly_sync_tick(parkinsense_wall_clock_ms());

        vTaskDelay(pdMS_TO_TICKS(PARKINSENSE_TASK_DELAY_MS));
    }
}
#endif

