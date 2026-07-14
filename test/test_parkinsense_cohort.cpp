/*
 * Cohort build integration: pipeline auto-start after init (ESP32 only).
 */
#include "unity.h"

#ifdef ESP32
#include "hardware/parkinsense_data.h"
#include "utils/parkinsense_build_config.h"

void test_parkinsense_pipeline_auto_start_matches_build(void) {
    parkinsense_data_pipeline_init();
    if (parkinsense_build_auto_start_on_boot()) {
        TEST_ASSERT_TRUE_MESSAGE(
            parkinsense_data_pipeline_is_started(),
            "cohort build must auto-start pipeline on boot"
        );
    } else {
        TEST_ASSERT_FALSE_MESSAGE(
            parkinsense_data_pipeline_is_started(),
            "non-cohort build must not auto-start pipeline"
        );
    }
}
#endif
