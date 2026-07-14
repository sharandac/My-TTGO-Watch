#ifndef _PARKINSENSE_BUILD_CONFIG_H
    #define _PARKINSENSE_BUILD_CONFIG_H

    /** True when firmware should start the accel pipeline without Play (cohort wear). */
    bool parkinsense_build_auto_start_on_boot(void);
    /** True when BLE hourly sync uses PKSD binary batches (matches native app parser). */
    bool parkinsense_build_ble_binary(void);
    /** True when accel uses BMA423 hardware FIFO (continuous sampling in light sleep). */
    bool parkinsense_build_bma_fifo(void);
    /** True for the blessed cohort production env (distinct boot banner). */
    bool parkinsense_build_is_cohort(void);
    /** Short build label for serial logs, e.g. "cohort-v1" or "dev". */
    const char* parkinsense_build_label(void);

#endif
