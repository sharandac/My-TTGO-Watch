#include "unity.h"
#include "utils/parkinsense_build_config.h"

void test_cohort_config_auto_start_flag(void) {
#ifdef PARKINSENSE_AUTO_START_ON_BOOT
    TEST_ASSERT_TRUE(parkinsense_build_auto_start_on_boot());
#else
    TEST_ASSERT_FALSE(parkinsense_build_auto_start_on_boot());
#endif
}

void test_cohort_config_binary_flag(void) {
#ifdef PARKINSENSE_BLE_BINARY
    TEST_ASSERT_TRUE(parkinsense_build_ble_binary());
#else
    TEST_ASSERT_FALSE(parkinsense_build_ble_binary());
#endif
}

void test_cohort_config_bma_fifo_flag(void) {
#ifdef PARKINSENSE_BMA_FIFO
    TEST_ASSERT_TRUE(parkinsense_build_bma_fifo());
#else
    TEST_ASSERT_FALSE(parkinsense_build_bma_fifo());
#endif
}

void test_cohort_config_cohort_build_flag(void) {
#ifdef PARKINSENSE_COHORT_BUILD
    TEST_ASSERT_TRUE(parkinsense_build_is_cohort());
    TEST_ASSERT_EQUAL_STRING("cohort-v1", parkinsense_build_label());
#else
    TEST_ASSERT_FALSE(parkinsense_build_is_cohort());
#endif
}
