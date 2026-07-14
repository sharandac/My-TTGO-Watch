#include "parkinsense_build_config.h"

bool parkinsense_build_auto_start_on_boot(void) {
#ifdef PARKINSENSE_AUTO_START_ON_BOOT
    return true;
#else
    return false;
#endif
}

bool parkinsense_build_ble_binary(void) {
#ifdef PARKINSENSE_BLE_BINARY
    return true;
#else
    return false;
#endif
}

bool parkinsense_build_is_cohort(void) {
#ifdef PARKINSENSE_COHORT_BUILD
    return true;
#else
    return false;
#endif
}

const char* parkinsense_build_label(void) {
#ifdef PARKINSENSE_BUILD_LABEL
    return PARKINSENSE_BUILD_LABEL;
#elif defined(PARKINSENSE_COHORT_BUILD)
    return "cohort";
#else
    return "default";
#endif
}


bool parkinsense_build_bma_fifo(void) {
#ifdef PARKINSENSE_BMA_FIFO
    return true;
#else
    return false;
#endif
}
