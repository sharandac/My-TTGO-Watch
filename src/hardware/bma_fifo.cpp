#include "hardware/bma_fifo.h"
#include "utils/bma_fifo_timestamps.h"
#include "hardware/parkinsense_data.h"
#include "utils/logging.h"
#include "config.h"

#ifdef NATIVE_64BIT

bool bma_fifo_is_enabled(void) { return false; }
bool bma_fifo_is_active(void) { return false; }
bool bma_fifo_setup(void) { return false; }
void bma_fifo_teardown(void) {}
void bma_fifo_on_motion(void) {}
void bma_fifo_idle_tick(void) {}
unsigned bma_fifo_service(void) { return 0; }
void bma_fifo_prepare_light_sleep(void) {}
unsigned bma_fifo_get_samples_drained(void) { return 0; }

#else

#if defined(LILYGO_WATCH_2020_V1) || defined(LILYGO_WATCH_2020_V2) || defined(LILYGO_WATCH_2020_V3)

#ifdef PARKINSENSE_BMA_FIFO

#include <Arduino.h>
#include <TTGO.h>
#include <sys/time.h>
#include "drive/bma423/bma4.h"
#include "drive/bma423/bma423.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define BMA_FIFO_MAX_READ_BYTES 126U
#define BMA_FIFO_WM_BYTES 126U
#define BMA_FIFO_SAMPLE_INTERVAL_MS 20U
#define BMA_FIFO_MAX_SAMPLES 48U
#define BMA_FIFO_IDLE_TIMEOUT_MS 30000U

static struct bma4_dev s_bma_dev;
static struct bma4_fifo_frame s_fifo_frame;
static uint8_t s_fifo_buffer[BMA_FIFO_MAX_READ_BYTES];
static bool s_fifo_configured = false;
static unsigned s_samples_drained = 0;
static uint32_t s_last_motion_ms = 0;
static SemaphoreHandle_t s_service_mutex = NULL;

static void bma_fifo_drain_hw_fifo_locked(TTGOClass *ttgo);

static TTGOClass *bma_fifo_ttgo(void)
{
    return TTGOClass::getWatch();
}

static uint16_t bma_fifo_bus_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    TTGOClass *ttgo = bma_fifo_ttgo();
    if (!ttgo || !reg_data || len == 0) {
        return BMA4_E_NULL_PTR;
    }

    if (reg_addr == BMA4_FIFO_DATA_ADDR) {
        if (len > BMA_FIFO_MAX_READ_BYTES) {
            len = BMA_FIFO_MAX_READ_BYTES;
        }
        ttgo->readBytes(dev_addr, reg_addr, reg_data, len);
        return BMA4_OK;
    }

    uint16_t offset = 0;
    while (offset < len) {
        uint16_t chunk = len - offset;
        if (chunk > 32U) {
            chunk = 32U;
        }
        ttgo->readBytes(dev_addr, (uint8_t)(reg_addr + offset), reg_data + offset, chunk);
        offset = (uint16_t)(offset + chunk);
    }
    return BMA4_OK;
}

static uint16_t bma_fifo_bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    TTGOClass *ttgo = bma_fifo_ttgo();
    if (!ttgo || !reg_data || len == 0) {
        return BMA4_E_NULL_PTR;
    }
    ttgo->writeBytes(dev_addr, reg_addr, reg_data, len);
    return BMA4_OK;
}

static void bma_fifo_init_dev_struct(void)
{
    s_bma_dev.dev_addr = BMA4_I2C_ADDR_SECONDARY;
    s_bma_dev.interface = BMA4_I2C_INTERFACE;
    s_bma_dev.bus_read = bma_fifo_bus_read;
    s_bma_dev.bus_write = bma_fifo_bus_write;
    s_bma_dev.delay = delay;
    s_bma_dev.read_write_len = 32;
    s_bma_dev.resolution = 12;
    s_bma_dev.feature_len = BMA423_FEATURE_SIZE;
    s_bma_dev.fifo = &s_fifo_frame;
    s_fifo_frame.data = s_fifo_buffer;
    s_fifo_frame.length = BMA_FIFO_MAX_READ_BYTES;
    s_fifo_frame.fifo_header_enable = 0;
    s_fifo_frame.fifo_data_enable = BMA4_FIFO_A_ENABLE;
    s_fifo_frame.accel_byte_start_idx = 0;
    s_fifo_frame.mag_byte_start_idx = 0;
}

static uint32_t bma_fifo_now_ms(void)
{
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) == 0 && tv.tv_sec >= 1577836800L) {
        return (uint32_t)((uint64_t)tv.tv_sec * 1000ULL + (uint64_t)(tv.tv_usec / 1000));
    }
    return (uint32_t)(millis());
}

static unsigned bma_fifo_push_extracted(const struct bma4_accel *accel_data, unsigned count, uint32_t drain_end_ms)
{
    if (!parkinsense_data_pipeline_is_started() || count == 0) {
        return 0;
    }
    for (unsigned i = 0; i < count; i++) {
        bma_accel_data_t sample;
        sample.x = (int16_t)accel_data[i].x;
        sample.y = (int16_t)accel_data[i].y;
        sample.z = (int16_t)accel_data[i].z;
        sample.timestamp_ms = bma_fifo_sample_timestamp_ms(
            drain_end_ms, i, count, BMA_FIFO_SAMPLE_INTERVAL_MS);
        parkinsense_data_pipeline_push_sample(sample);
    }
    s_samples_drained += count;
    s_last_motion_ms = drain_end_ms;
    return count;
}

bool bma_fifo_is_enabled(void)
{
    return true;
}

bool bma_fifo_is_active(void)
{
    return s_fifo_configured;
}

void bma_fifo_on_motion(void)
{
    if (!parkinsense_data_pipeline_is_started()) {
        return;
    }

    s_last_motion_ms = bma_fifo_now_ms();
    if (s_fifo_configured) {
        return;
    }

    if (bma_fifo_setup()) {
        log_i("bma_fifo: motion detected, 50 Hz recording started");
    }
}

void bma_fifo_idle_tick(void)
{
    if (!s_fifo_configured || s_last_motion_ms == 0U) {
        return;
    }

    const uint32_t now = bma_fifo_now_ms();
    if ((now - s_last_motion_ms) >= BMA_FIFO_IDLE_TIMEOUT_MS) {
        bma_fifo_teardown();
        log_i("bma_fifo: idle timeout, recording stopped");
    }
}

bool bma_fifo_setup(void)
{
    if (s_fifo_configured) {
        return true;
    }

    if (!s_service_mutex) {
        s_service_mutex = xSemaphoreCreateMutex();
        if (!s_service_mutex) {
            log_e("bma_fifo: mutex create failed");
            return false;
        }
    }

    bma_fifo_init_dev_struct();

    struct bma4_accel_config cfg;
    cfg.odr = BMA4_OUTPUT_DATA_RATE_50HZ;
    cfg.range = BMA4_ACCEL_RANGE_2G;
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;

    uint16_t rslt = bma4_set_accel_enable(BMA4_ENABLE, &s_bma_dev);
    if (rslt != BMA4_OK) {
        log_e("bma_fifo: enable accel failed (%u)", (unsigned)rslt);
        return false;
    }
    rslt = bma4_set_accel_config(&cfg, &s_bma_dev);
    if (rslt != BMA4_OK) {
        log_e("bma_fifo: accel config failed (%u)", (unsigned)rslt);
        return false;
    }

    rslt = bma4_set_fifo_config(BMA4_FIFO_ACCEL, BMA4_ENABLE, &s_bma_dev);
    if (rslt != BMA4_OK) {
        log_e("bma_fifo: fifo config failed (%u)", (unsigned)rslt);
        return false;
    }

    rslt = bma4_set_fifo_wm(BMA_FIFO_WM_BYTES, &s_bma_dev);
    if (rslt != BMA4_OK) {
        log_e("bma_fifo: fifo wm failed (%u)", (unsigned)rslt);
        return false;
    }

    rslt = bma4_map_interrupt(
        BMA4_INTR1_MAP,
        BMA4_FIFO_WM_INT | BMA4_FIFO_FULL_INT,
        BMA4_ENABLE,
        &s_bma_dev);
    if (rslt != BMA4_OK) {
        log_e("bma_fifo: map fifo int failed (%u)", (unsigned)rslt);
        return false;
    }

    s_fifo_configured = true;
    if (s_last_motion_ms == 0U) {
        s_last_motion_ms = bma_fifo_now_ms();
    }
    log_i("bma_fifo: 50 Hz FIFO wm=%u bytes (motion-gated)", (unsigned)BMA_FIFO_WM_BYTES);
    return true;
}

void bma_fifo_teardown(void)
{
    if (!s_fifo_configured) {
        return;
    }
    (void)bma4_map_interrupt(
        BMA4_INTR1_MAP,
        BMA4_FIFO_WM_INT | BMA4_FIFO_FULL_INT,
        BMA4_DISABLE,
        &s_bma_dev);
    (void)bma4_set_fifo_config(BMA4_FIFO_ACCEL, BMA4_DISABLE, &s_bma_dev);
    s_fifo_configured = false;
    s_last_motion_ms = 0;
    log_i("bma_fifo: disabled");
}

unsigned bma_fifo_service(void)
{
    if (!s_fifo_configured || !parkinsense_data_pipeline_is_started() || !s_service_mutex) {
        return 0;
    }
    if (xSemaphoreTake(s_service_mutex, 0) != pdTRUE) {
        return 0;
    }

    TTGOClass *ttgo = bma_fifo_ttgo();
    if (!ttgo) {
        xSemaphoreGive(s_service_mutex);
        return 0;
    }

    const unsigned pushed_before = s_samples_drained;
    bma_fifo_drain_hw_fifo_locked(ttgo);
    const unsigned total_pushed = s_samples_drained - pushed_before;

    xSemaphoreGive(s_service_mutex);
    return total_pushed;
}

static void bma_fifo_drain_hw_fifo_locked(TTGOClass *ttgo)
{
    for (unsigned pass = 0; pass < 8U; pass++) {
        uint16_t fifo_length = 0;
        if (bma4_get_fifo_length(&fifo_length, &s_bma_dev) != BMA4_OK || fifo_length == 0) {
            break;
        }
        if (fifo_length > BMA_FIFO_MAX_READ_BYTES) {
            fifo_length = BMA_FIFO_MAX_READ_BYTES;
        }

        s_fifo_frame.accel_byte_start_idx = 0;
        s_fifo_frame.mag_byte_start_idx = 0;
        s_fifo_frame.length = fifo_length;
        ttgo->readBytes(s_bma_dev.dev_addr, BMA4_FIFO_DATA_ADDR, s_fifo_buffer, fifo_length);

        uint8_t cfg1 = 0;
        if (bma4_read_regs(BMA4_FIFO_CONFIG_1_ADDR, &cfg1, 1, &s_bma_dev) == BMA4_OK) {
            s_fifo_frame.fifo_header_enable = cfg1 & BMA4_FIFO_HEADER;
            s_fifo_frame.fifo_data_enable = cfg1 & BMA4_FIFO_M_A_ENABLE;
        }

        struct bma4_accel accel_data[BMA_FIFO_MAX_SAMPLES];
        uint16_t accel_length = BMA_FIFO_MAX_SAMPLES;
        const uint16_t extract_rslt = bma4_extract_accel(accel_data, &accel_length, &s_bma_dev);
        if (extract_rslt == BMA4_OK && accel_length > 0) {
            const uint32_t drain_end_ms = bma_fifo_now_ms();
            const unsigned pushed = bma_fifo_push_extracted(accel_data, accel_length, drain_end_ms);
            if (pushed > 0) {
                parkinsense_data_pipeline_drain_if_full();
            }
        }
    }
}

void bma_fifo_prepare_light_sleep(void)
{
    if (!s_fifo_configured || !s_service_mutex) {
        return;
    }
    if (xSemaphoreTake(s_service_mutex, pdMS_TO_TICKS(250)) != pdTRUE) {
        return;
    }

    TTGOClass *ttgo = bma_fifo_ttgo();
    if (ttgo) {
        bma_fifo_drain_hw_fifo_locked(ttgo);
        uint16_t int_status = 0;
        (void)bma4_read_int_status(&int_status, &s_bma_dev);
        for (unsigned i = 0; i < 4U && digitalRead(BMA423_INT1); i++) {
            (void)bma4_read_int_status(&int_status, &s_bma_dev);
            bma_fifo_drain_hw_fifo_locked(ttgo);
        }
    }

    xSemaphoreGive(s_service_mutex);
}

unsigned bma_fifo_get_samples_drained(void)
{
    return s_samples_drained;
}

#else /* !PARKINSENSE_BMA_FIFO */

bool bma_fifo_is_enabled(void) { return false; }
bool bma_fifo_is_active(void) { return false; }
bool bma_fifo_setup(void) { return false; }
void bma_fifo_teardown(void) {}
void bma_fifo_on_motion(void) {}
void bma_fifo_idle_tick(void) {}
unsigned bma_fifo_service(void) { return 0; }
void bma_fifo_prepare_light_sleep(void) {}
unsigned bma_fifo_get_samples_drained(void) { return 0; }

#endif /* PARKINSENSE_BMA_FIFO */

#else /* !LILYGO_WATCH_2020 */

bool bma_fifo_is_enabled(void) { return false; }
bool bma_fifo_is_active(void) { return false; }
bool bma_fifo_setup(void) { return false; }
void bma_fifo_teardown(void) {}
void bma_fifo_on_motion(void) {}
void bma_fifo_idle_tick(void) {}
unsigned bma_fifo_service(void) { return 0; }
void bma_fifo_prepare_light_sleep(void) {}
unsigned bma_fifo_get_samples_drained(void) { return 0; }

#endif /* LILYGO_WATCH_2020 */

#endif /* NATIVE_64BIT */
