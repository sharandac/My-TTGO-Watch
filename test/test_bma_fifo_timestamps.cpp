#include <unity.h>
#include "hardware/bma_fifo.h"

void test_bma_fifo_sample_timestamp_ms_oldest_first(void) {
    TEST_ASSERT_EQUAL_UINT32(980U, bma_fifo_sample_timestamp_ms(1000U, 0U, 2U, 20U));
    TEST_ASSERT_EQUAL_UINT32(1000U, bma_fifo_sample_timestamp_ms(1000U, 1U, 2U, 20U));
}

void test_bma_fifo_sample_timestamp_ms_single(void) {
    TEST_ASSERT_EQUAL_UINT32(500U, bma_fifo_sample_timestamp_ms(500U, 0U, 1U, 20U));
}

void test_bma_fifo_is_enabled_cohort_flag(void) {
#ifdef PARKINSENSE_BMA_FIFO
    TEST_ASSERT_TRUE(bma_fifo_is_enabled());
#else
    TEST_ASSERT_FALSE(bma_fifo_is_enabled());
#endif
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    UNITY_BEGIN();
    RUN_TEST(test_bma_fifo_sample_timestamp_ms_oldest_first);
    RUN_TEST(test_bma_fifo_sample_timestamp_ms_single);
    RUN_TEST(test_bma_fifo_is_enabled_cohort_flag);
    return UNITY_END();
}
