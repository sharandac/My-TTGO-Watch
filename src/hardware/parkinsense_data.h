#ifndef _PARKINSENSE_DATA_H
    #define _PARKINSENSE_DATA_H

    #include "callback.h"
    #include "motion.h"

    #ifdef NATIVE_64BIT
        #include "utils/io.h"
    #else

    #endif

    #define PARKINSENSE_DATA_UPDATE_INTERVAL 1000
    /** Minimum wall-clock time between hourly PSRAM sync attempts (ms). */
    #define PARKINSENSE_HOURLY_SEND_INTERVAL_MS   (60UL * 60UL * 1000UL)
    /** How long each sync window advertises and tries to drain PSRAM (ms). */
    #define PARKINSENSE_HOURLY_SYNC_WINDOW_MS       (5UL * 60UL * 1000UL)

    /** Max PSRAM blocks (~1 h at 50 Hz). Oldest blocks drop when full. */
    #ifndef PARKINSENSE_MAX_PSRAM_QUEUE_BLOCKS
    #define PARKINSENSE_MAX_PSRAM_QUEUE_BLOCKS     5625U
    #endif

#ifdef PARKINSENSE_TEST_HOURLY_INTERVAL_MS
#undef PARKINSENSE_HOURLY_SEND_INTERVAL_MS
#define PARKINSENSE_HOURLY_SEND_INTERVAL_MS  PARKINSENSE_TEST_HOURLY_INTERVAL_MS
#endif
#ifdef PARKINSENSE_TEST_SYNC_WINDOW_MS
#undef PARKINSENSE_HOURLY_SYNC_WINDOW_MS
#define PARKINSENSE_HOURLY_SYNC_WINDOW_MS  PARKINSENSE_TEST_SYNC_WINDOW_MS
#endif

    /** Poll hourly sync / BLE advertise window (also called from powermgm on wake). */
    void parkinsense_data_hourly_sync_poll(void);

    /** True while an hourly PSRAM sync / BLE advertise window is active. */
    bool parkinsense_data_is_hourly_sync_window_active(void);

    /** True when a BLE central is connected (NimBLE server peer count > 0). */
    bool parkinsense_data_ble_client_connected(void);

    #ifndef _BV
    #define _BV(b) (1UL << (b))
    #endif
    #define PARKINSENSE_SEND_DONE   _BV(0)
    #define PARKINSENSE_SEND_ABORT  _BV(1)

    void parkinsense_data_pipeline_init(void);
    void parkinsense_data_pipeline_start(void);
    void parkinsense_data_pipeline_stop(void);
    void parkinsense_data_pipeline_reset(void);
    void parkinsense_data_pipeline_get_data(void);
    /** Push one accel sample (FIFO drain or legacy poll). */
    void parkinsense_data_pipeline_push_sample(bma_accel_data_t data);
    void parkinsense_data_pipeline_write_to_psram(void);
    void parkinsense_data_pipeline_drain_if_full(void);
    /** Send the oldest PSRAM block over BLE.  Returns true if the block was
     *  sent successfully (and freed); false if BLE is unavailable or send
     *  failed, in which case the block is **kept** in PSRAM for retry. */
    bool parkinsense_data_pipeline_send_data(void);
    int parkinsense_data_pipeline_buff_count(void);
    bool parkinsense_data_pipeline_is_started(void);
    /** Counters for on-screen diagnostics (no mutex needed, written only from pipeline task). */
    unsigned parkinsense_data_get_blocks_sent(void);
    unsigned parkinsense_data_get_send_fails(void);
    unsigned parkinsense_data_get_psram_queued(void);
    /** Oldest blocks dropped after queue cap (FIFO). */
    unsigned parkinsense_data_get_psram_blocks_dropped(void);
    unsigned parkinsense_data_get_send_attempts(void);
    unsigned parkinsense_data_get_early_returns(void);

#endif // _PARKINSENSE_DATA_H
