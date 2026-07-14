/*
 * Shared test entry point for both native (main) and device (setup/loop).
 * Runs all circular buffer tests, plus PSRAM tests on ESP32.
 */
#include "unity.h"
#include "utils/circular_buff.h"
#include "utils/psram_accel_alloc.h"
#include "hardware/hardware.h"

/* ---- Unity hooks ---- */
void setUp(void) {
    psram_init();
}

void tearDown(void) {
}


/* ---- Cohort build config (all platforms) ---- */
void test_cohort_config_auto_start_flag(void);
void test_cohort_config_binary_flag(void);
void test_cohort_config_cohort_build_flag(void);
void test_cohort_config_bma_fifo_flag(void);

void test_bma_fifo_sample_timestamp_ms_oldest_first(void);
void test_bma_fifo_sample_timestamp_ms_single(void);

/* ---- Declare test functions from test_circular_buffer.cpp ---- */
void test_circular_buffer_empty_after_init(void);
void test_circular_buffer_write(void);
void test_circular_buffer_read(void);
void test_circular_buffer_write_seq(void);
void test_circular_buff_read_seq(void);
void test_circular_buffer_sequential_readwrite(void);
void test_circular_buffer_overwrite(void);
void test_circular_buffer_read_empty(void);
void test_circular_buffer_multi_overwrite(void);
void test_circular_buffer_count_invariants(void);
void test_circular_buffer_wrap_around(void);

/* ---- Declare test functions from test_psram.cpp ---- */
void test_psram_free_first_advances_queue(void);
void test_psram_free_first_on_empty_list(void);
void test_psram_queue_cap_no_drop_below_max(void);
void test_psram_queue_cap_drop_when_at_max(void);
#ifdef ESP32
void test_psram_get_memory(void);
void test_psram_init(void);
void test_write_to_psram(void);
void test_read_from_psram(void);
void test_light_sleep_memory_survival(void);   /* test_power.cpp */
void test_light_sleep_bma_psram_survival(void); /* test_power.cpp */
#endif

/* ---- Declare test functions from test_integ_pipeline.cpp (ESP32 only) ---- */
#ifdef ESP32
void test_parkinsense_pipeline_auto_start_matches_build(void);
void test_integ_accel_to_buffer(void);
void test_integ_buffer_to_psram(void);
void test_integ_full_pipeline(void);
void test_integ_multi_block_accumulation(void);
void test_integ_interleaved_fill_flush(void);
void test_integ_buffer_overwrite_then_flush(void);
#endif

/* ---- Run all tests ---- */
static void run_all_tests(void) {
    UNITY_BEGIN();

    /* Cohort build config (compile-flag dependent) */
    RUN_TEST(test_cohort_config_auto_start_flag);
    RUN_TEST(test_cohort_config_binary_flag);
    RUN_TEST(test_cohort_config_cohort_build_flag);
    RUN_TEST(test_cohort_config_bma_fifo_flag);

    RUN_TEST(test_bma_fifo_sample_timestamp_ms_oldest_first);
    RUN_TEST(test_bma_fifo_sample_timestamp_ms_single);


    /* Circular buffer tests (all platforms) */
    RUN_TEST(test_circular_buffer_empty_after_init);
    RUN_TEST(test_circular_buffer_write);
    RUN_TEST(test_circular_buffer_read);
    RUN_TEST(test_circular_buffer_write_seq);
    RUN_TEST(test_circular_buff_read_seq);
    RUN_TEST(test_circular_buffer_sequential_readwrite);
    RUN_TEST(test_circular_buffer_overwrite);
    RUN_TEST(test_circular_buffer_read_empty);
    RUN_TEST(test_circular_buffer_multi_overwrite);
    RUN_TEST(test_circular_buffer_count_invariants);
    RUN_TEST(test_circular_buffer_wrap_around);

    RUN_TEST(test_psram_free_first_advances_queue);
    RUN_TEST(test_psram_free_first_on_empty_list);

    RUN_TEST(test_psram_queue_cap_no_drop_below_max);
    RUN_TEST(test_psram_queue_cap_drop_when_at_max);

    /* PSRAM tests (device only) */
#ifdef ESP32
    RUN_TEST(test_psram_get_memory);
    RUN_TEST(test_psram_init);
    RUN_TEST(test_write_to_psram);
    RUN_TEST(test_read_from_psram);
#endif

    /* Power / sleep tests (device only, test_power.cpp) */
#ifdef ESP32
    RUN_TEST(test_light_sleep_memory_survival);
    RUN_TEST(test_light_sleep_bma_psram_survival);
#endif

    /* Cohort pipeline auto-start (device only) */
#ifdef ESP32
    RUN_TEST(test_parkinsense_pipeline_auto_start_matches_build);
#endif

    /* Integration pipeline tests (device only) */
#ifdef ESP32
    RUN_TEST(test_integ_accel_to_buffer);
    RUN_TEST(test_integ_buffer_to_psram);
    RUN_TEST(test_integ_full_pipeline);
    RUN_TEST(test_integ_multi_block_accumulation);
    RUN_TEST(test_integ_interleaved_fill_flush);
    RUN_TEST(test_integ_buffer_overwrite_then_flush);
#endif

    UNITY_END();
}

/* ---- Entry points ---- */
#ifdef ESP32
#include <Arduino.h>
void setup(void) {
    hardware_test_init_bma();
    delay(2000);  /* let Serial connect before Unity output */
    run_all_tests();
}

void loop(void) {
}
#else
int main(int argc, char **argv) {
    run_all_tests();
    return 0;
}
#endif
