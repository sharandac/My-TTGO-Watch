#ifndef _PARKINSENSE_DATA_H
    #define _PARKINSENSE_DATA_H

    #include "callback.h"

    #ifdef NATIVE_64BIT
        #include "utils/io.h"
    #else

    #endif

    #define PARKINSENSE_DATA_UPDATE_INTERVAL 1000
    /** Minimum time between hourly PSRAM sync attempts (ms). */
    #define PARKINSENSE_HOURLY_SEND_INTERVAL_MS   (60UL * 60UL * 1000UL)
    /** How long each hour we advertise and try to drain PSRAM (ms). */
    #define PARKINSENSE_HOURLY_SYNC_WINDOW_MS       (5UL * 60UL * 1000UL)

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
    unsigned parkinsense_data_get_send_attempts(void);
    unsigned parkinsense_data_get_early_returns(void);

#endif // _PARKINSENSE_DATA_H
