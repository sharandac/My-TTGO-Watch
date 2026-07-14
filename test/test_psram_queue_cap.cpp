#include "unity.h"
#include "utils/psram_queue_cap.h"

void test_psram_queue_cap_no_drop_below_max(void) {
    TEST_ASSERT_EQUAL_size_t(0, psram_queue_cap_drops_needed(100, 5625));
    TEST_ASSERT_EQUAL_size_t(0, psram_queue_cap_drops_needed(5624, 5625));
}

void test_psram_queue_cap_drop_when_at_max(void) {
    TEST_ASSERT_EQUAL_size_t(1, psram_queue_cap_drops_needed(5625, 5625));
    TEST_ASSERT_EQUAL_size_t(2, psram_queue_cap_drops_needed(5626, 5625));
}
