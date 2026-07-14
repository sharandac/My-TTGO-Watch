/*
 * Tier 1 Integration Tests — Data Pipeline
 *
 * Tests the handoffs between pipeline stages:
 *   BMA423 → Circular Buffer → PSRAM
 *
 * All tests are ESP32-only (require real accelerometer + PSRAM hardware).
 */
#include "unity.h"
#include "utils/circular_buff.h"
#include "utils/psram_accel_alloc.h"

#ifdef ESP32
#include <Arduino.h>
#include "hardware/motion.h"

/* Shared test state — reset by setUp() in test_setup.cpp via psram_init() */
static circular_buff_t s_pipe_buff;
static bma_accel_data_t s_pipe_data[BUFFER_SIZE];

/* ------------------------------------------------------------------ *
 * 1.1  Accel → Circular Buffer
 *
 * Read real BMA423 samples into the circular buffer, read them back,
 * verify: values are sane, timestamps are monotonically non-decreasing,
 * sample count matches.
 * ------------------------------------------------------------------ */
void test_integ_accel_to_buffer(void) {
    circular_buff_init(&s_pipe_buff);
    const int num_samples = 50;

    for (int i = 0; i < num_samples; i++) {
        bma_accel_data_t sample = bma_get_accel();
        circular_buff_write(sample, &s_pipe_buff);
        delay(20);
    }

    TEST_ASSERT_EQUAL_INT(num_samples, s_pipe_buff.count);

    uint32_t prev_ts = 0;
    bool any_nonzero = false;

    for (int i = 0; i < num_samples; i++) {
        bma_accel_data_t out;
        bool ok = circular_buff_read(&s_pipe_buff, &out);
        TEST_ASSERT_TRUE(ok);

        TEST_ASSERT_TRUE(out.x >= -2048 && out.x <= 2047);
        TEST_ASSERT_TRUE(out.y >= -2048 && out.y <= 2047);
        TEST_ASSERT_TRUE(out.z >= -2048 && out.z <= 2047);

        if (out.x != 0 || out.y != 0 || out.z != 0)
            any_nonzero = true;

        TEST_ASSERT_TRUE(out.timestamp_ms >= prev_ts);
        prev_ts = out.timestamp_ms;
    }

    TEST_ASSERT_TRUE_MESSAGE(any_nonzero, "All accel samples were zero — sensor may not be active");
    TEST_ASSERT_EQUAL_INT(0, s_pipe_buff.count);
}

/* ------------------------------------------------------------------ *
 * 1.2  Circular Buffer → PSRAM flush
 *
 * Fill buffer with real accel data, drain into a temp array, write
 * to PSRAM as a single block, verify header fields and checksum.
 * ------------------------------------------------------------------ */
void test_integ_buffer_to_psram(void) {
    circular_buff_init(&s_pipe_buff);
    psram_init();
    const int num_samples = 100;

    for (int i = 0; i < num_samples; i++) {
        bma_accel_data_t sample = bma_get_accel();
        circular_buff_write(sample, &s_pipe_buff);
        delay(20);
    }

    int drained = 0;
    while (drained < num_samples) {
        bool ok = circular_buff_read(&s_pipe_buff, &s_pipe_data[drained]);
        TEST_ASSERT_TRUE(ok);
        drained++;
    }
    TEST_ASSERT_EQUAL_INT(num_samples, drained);

    header_t* block = psram_write(drained, s_pipe_data);
    TEST_ASSERT_NOT_NULL(block);

    TEST_ASSERT_EQUAL_UINT8(PSRAM_ACCEL_HEADER_MAGIC, block->magic_number);
    TEST_ASSERT_EQUAL_UINT8(1, block->version);
    TEST_ASSERT_EQUAL_UINT32(num_samples, block->num_elements);
    TEST_ASSERT_EQUAL_UINT32(s_pipe_data[0].timestamp_ms, block->start_time_ms);
    TEST_ASSERT_EQUAL_UINT32(s_pipe_data[num_samples - 1].timestamp_ms, block->end_time_ms);
    TEST_ASSERT_NOT_EQUAL(0, block->checksum);

    header_t* readback = psram_read(num_samples);
    TEST_ASSERT_NOT_NULL_MESSAGE(readback, "psram_read failed — checksum or magic mismatch");
}

/* ------------------------------------------------------------------ *
 * 1.3  Full pipeline: Accel → Buffer → PSRAM → readback → verify
 *
 * End-to-end: real accelerometer data flows through the entire
 * pipeline, then is read back from PSRAM and compared sample-by-sample
 * against what went in.
 * ------------------------------------------------------------------ */
void test_integ_full_pipeline(void) {
    circular_buff_init(&s_pipe_buff);
    psram_init();
    const int num_samples = 64;

    bma_accel_data_t originals[64];

    for (int i = 0; i < num_samples; i++) {
        originals[i] = bma_get_accel();
        circular_buff_write(originals[i], &s_pipe_buff);
        delay(20);
    }

    bma_accel_data_t drained[64];
    for (int i = 0; i < num_samples; i++) {
        bool ok = circular_buff_read(&s_pipe_buff, &drained[i]);
        TEST_ASSERT_TRUE(ok);
    }

    for (int i = 0; i < num_samples; i++) {
        TEST_ASSERT_EQUAL_INT16(originals[i].x, drained[i].x);
        TEST_ASSERT_EQUAL_INT16(originals[i].y, drained[i].y);
        TEST_ASSERT_EQUAL_INT16(originals[i].z, drained[i].z);
        TEST_ASSERT_EQUAL_UINT32(originals[i].timestamp_ms, drained[i].timestamp_ms);
    }

    header_t* block = psram_write(num_samples, drained);
    TEST_ASSERT_NOT_NULL(block);

    header_t* readback = psram_read(num_samples);
    TEST_ASSERT_NOT_NULL(readback);

    bma_accel_data_t* payload = (bma_accel_data_t*)((char*)readback + sizeof(header_t));
    for (int i = 0; i < num_samples; i++) {
        TEST_ASSERT_EQUAL_INT16(originals[i].x, payload[i].x);
        TEST_ASSERT_EQUAL_INT16(originals[i].y, payload[i].y);
        TEST_ASSERT_EQUAL_INT16(originals[i].z, payload[i].z);
        TEST_ASSERT_EQUAL_UINT32(originals[i].timestamp_ms, payload[i].timestamp_ms);
    }
}

/* ------------------------------------------------------------------ *
 * 1.4  Multi-block PSRAM accumulation
 *
 * Simulate multiple flush cycles: fill buffer, drain to PSRAM,
 * repeat. Verify linked list is intact, all blocks have valid
 * headers, sequence numbers increment, and checksums pass.
 * ------------------------------------------------------------------ */
void test_integ_multi_block_accumulation(void) {
    circular_buff_init(&s_pipe_buff);
    psram_init();

    const int blocks_to_write = 5;
    const int samples_per_block = 32;

    for (int b = 0; b < blocks_to_write; b++) {
        for (int i = 0; i < samples_per_block; i++) {
            bma_accel_data_t sample = bma_get_accel();
            circular_buff_write(sample, &s_pipe_buff);
            delay(10);
        }

        bma_accel_data_t flush_buf[32];
        for (int i = 0; i < samples_per_block; i++) {
            circular_buff_read(&s_pipe_buff, &flush_buf[i]);
        }

        header_t* block = psram_write(samples_per_block, flush_buf);
        TEST_ASSERT_NOT_NULL(block);
    }

    TEST_ASSERT_NOT_NULL(first_accel_block_ptr);
    TEST_ASSERT_NOT_NULL(current_accel_block_ptr);

    header_t* cursor = first_accel_block_ptr;
    uint32_t prev_seq = 0;
    int block_count = 0;

    while (cursor != NULL) {
        TEST_ASSERT_EQUAL_UINT8(PSRAM_ACCEL_HEADER_MAGIC, cursor->magic_number);
        TEST_ASSERT_EQUAL_UINT32(samples_per_block, cursor->num_elements);
        TEST_ASSERT_NOT_EQUAL(0, cursor->checksum);

        if (block_count > 0) {
            TEST_ASSERT_TRUE_MESSAGE(cursor->seq_num > prev_seq,
                "Sequence numbers must be strictly increasing");
        }
        prev_seq = cursor->seq_num;

        TEST_ASSERT_TRUE(cursor->end_time_ms >= cursor->start_time_ms);

        block_count++;
        cursor = (header_t*)cursor->next;
    }

    TEST_ASSERT_EQUAL_INT(blocks_to_write, block_count);
}

/* ------------------------------------------------------------------ *
 * 1.5  Rapid fill-flush cycles under pressure
 *
 * Alternate between writing to the buffer and flushing to PSRAM
 * in small batches, simulating the real-world pattern where
 * new samples arrive while a flush is in progress.
 * ------------------------------------------------------------------ */
void test_integ_interleaved_fill_flush(void) {
    circular_buff_init(&s_pipe_buff);
    psram_init();

    const int cycles = 10;
    const int samples_per_cycle = 16;
    int total_psram_samples = 0;

    for (int c = 0; c < cycles; c++) {
        for (int i = 0; i < samples_per_cycle; i++) {
            bma_accel_data_t sample = bma_get_accel();
            circular_buff_write(sample, &s_pipe_buff);
            delay(5);
        }

        bma_accel_data_t batch[16];
        int read_count = 0;
        while (read_count < samples_per_cycle) {
            if (!circular_buff_read(&s_pipe_buff, &batch[read_count]))
                break;
            read_count++;
        }
        TEST_ASSERT_EQUAL_INT(samples_per_cycle, read_count);

        header_t* block = psram_write(read_count, batch);
        TEST_ASSERT_NOT_NULL(block);
        total_psram_samples += read_count;
    }

    TEST_ASSERT_EQUAL_INT(0, s_pipe_buff.count);

    header_t* cursor = first_accel_block_ptr;
    int verified_samples = 0;
    while (cursor != NULL) {
        TEST_ASSERT_EQUAL_UINT8(PSRAM_ACCEL_HEADER_MAGIC, cursor->magic_number);
        verified_samples += cursor->num_elements;
        cursor = (header_t*)cursor->next;
    }

    TEST_ASSERT_EQUAL_INT(total_psram_samples, verified_samples);
}

/* ------------------------------------------------------------------ *
 * 1.6  Buffer overwrite does not corrupt PSRAM blocks
 *
 * Let the circular buffer overwrite old samples (write more than
 * BUFFER_SIZE without draining), then flush whatever remains to
 * PSRAM, verify the block is still valid and self-consistent.
 * ------------------------------------------------------------------ */
void test_integ_buffer_overwrite_then_flush(void) {
    circular_buff_init(&s_pipe_buff);
    psram_init();

    const int overflow_count = BUFFER_SIZE + 200;

    for (int i = 0; i < overflow_count; i++) {
        bma_accel_data_t sample = bma_get_accel();
        circular_buff_write(sample, &s_pipe_buff);
        delay(1);
    }

    TEST_ASSERT_EQUAL_INT(BUFFER_SIZE, s_pipe_buff.count);

    int drained = 0;
    while (drained < BUFFER_SIZE) {
        bool ok = circular_buff_read(&s_pipe_buff, &s_pipe_data[drained]);
        if (!ok) break;
        drained++;
    }
    TEST_ASSERT_EQUAL_INT(BUFFER_SIZE, drained);

    header_t* block = psram_write(drained, s_pipe_data);
    TEST_ASSERT_NOT_NULL(block);

    TEST_ASSERT_EQUAL_UINT8(PSRAM_ACCEL_HEADER_MAGIC, block->magic_number);
    TEST_ASSERT_EQUAL_UINT32(BUFFER_SIZE, block->num_elements);
    TEST_ASSERT_TRUE(block->end_time_ms >= block->start_time_ms);

    header_t* readback = psram_read(BUFFER_SIZE);
    TEST_ASSERT_NOT_NULL_MESSAGE(readback, "Checksum failed after buffer overwrite flush");
}
#endif /* ESP32 */
