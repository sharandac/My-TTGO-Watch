/*
 * Power / sleep tests — light sleep, memory and PSRAM survival.
 * ESP32-only (device).
 */
#ifdef ESP32
#include "unity.h"
#include "utils/circular_buff.h"
#include "utils/psram_accel_alloc.h"
#include <esp_sleep.h>
#include <Arduino.h>
#include "hardware/motion.h"
#endif
/* setUp/tearDown in test_setup.cpp */

#ifdef ESP32
/* Light sleep with mock: the 4 allocator variables (and PSRAM) survive. Single run, no reboot. */
void test_light_sleep_memory_survival(void) {
    psram_init();
    bma_accel_data_t mock = {1, 2, 3, 1000};
    TEST_ASSERT_NOT_NULL(psram_write(1, &mock));
    esp_sleep_enable_timer_wakeup(500000);   /* 500 ms */
    esp_light_sleep_start();
    TEST_ASSERT_EQUAL_UINT32(ESP_SLEEP_WAKEUP_TIMER, esp_sleep_get_wakeup_cause());
    TEST_ASSERT_NOT_NULL(first_accel_block_ptr);
    TEST_ASSERT_NOT_NULL(current_accel_block_ptr);
    TEST_ASSERT_TRUE(first_accel_block_ptr == current_accel_block_ptr);
    TEST_ASSERT_EQUAL_UINT32(0, accel_block_valid_magic);
    TEST_ASSERT_EQUAL_UINT32(1, accel_seq_counter);
    header_t* read_header = psram_read(1);
    TEST_ASSERT_NOT_NULL(read_header);
    TEST_ASSERT_EQUAL_INT(PSRAM_ACCEL_HEADER_MAGIC, read_header->magic_number);
    TEST_ASSERT_EQUAL_INT(1, read_header->num_elements);
    TEST_ASSERT_EQUAL_INT(1000, read_header->start_time_ms);
}

/* Light sleep with real BMA data: fill from BMA423, write block to PSRAM, light sleep, verify 4 vars and block. */
#define LIGHT_SLEEP_BMA_SAMPLES  32
static bma_accel_data_t s_sleep_bma_buf[LIGHT_SLEEP_BMA_SAMPLES];

void test_light_sleep_bma_psram_survival(void) {
    psram_init();
    for (int i = 0; i < LIGHT_SLEEP_BMA_SAMPLES; i++) {
        s_sleep_bma_buf[i] = bma_get_accel();
        delay(20);
    }
    header_t* block = psram_write(LIGHT_SLEEP_BMA_SAMPLES, s_sleep_bma_buf);
    TEST_ASSERT_NOT_NULL(block);
    uint32_t start_ms = s_sleep_bma_buf[0].timestamp_ms;
    uint32_t end_ms   = s_sleep_bma_buf[LIGHT_SLEEP_BMA_SAMPLES - 1].timestamp_ms;

    esp_sleep_enable_timer_wakeup(500000);   /* 500 ms */
    esp_light_sleep_start();
    TEST_ASSERT_EQUAL_UINT32(ESP_SLEEP_WAKEUP_TIMER, esp_sleep_get_wakeup_cause());

    TEST_ASSERT_NOT_NULL(first_accel_block_ptr);
    TEST_ASSERT_NOT_NULL(current_accel_block_ptr);
    TEST_ASSERT_TRUE(first_accel_block_ptr == current_accel_block_ptr);
    TEST_ASSERT_EQUAL_UINT32(0, accel_block_valid_magic);
    TEST_ASSERT_EQUAL_UINT32(1, accel_seq_counter);

    header_t* read_header = psram_read(LIGHT_SLEEP_BMA_SAMPLES);
    TEST_ASSERT_NOT_NULL_MESSAGE(read_header, "psram_read failed after light sleep");
    TEST_ASSERT_EQUAL_UINT8(PSRAM_ACCEL_HEADER_MAGIC, read_header->magic_number);
    TEST_ASSERT_EQUAL_UINT32(LIGHT_SLEEP_BMA_SAMPLES, read_header->num_elements);
    TEST_ASSERT_EQUAL_UINT32(start_ms, read_header->start_time_ms);
    TEST_ASSERT_EQUAL_UINT32(end_ms, read_header->end_time_ms);
    TEST_ASSERT_NOT_EQUAL(0, read_header->checksum);
}
#endif /* ESP32 */
