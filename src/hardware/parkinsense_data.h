#ifndef _PARKINSENSE_DATA_H
    #define _PARKINSENSE_DATA_H

    #include "callback.h"

    #ifdef NATIVE_64BIT
        #include "utils/io.h"
    #else

    #endif

    #define PARKINSENSE_DATA_UPDATE_INTERVAL 1000
    /** How often (ms) the pipeline auto-sends PSRAM blocks over BLE.
     *  1 s keeps latency low now that blocks are small (32 samples ≈ 1 KB). */
    #define PARKINSENSE_AUTO_SEND_INTERVAL_MS 1000

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
