#include "unity.h"
#include "utils/circular_buff.h"
#include <stdint.h>

#define MAX_G 2047
#define MIN_G -2048

// STRUCT FOR PSEUDORANDOM NUMBER GENERATOR //
typedef struct {
    uint32_t state;
} rng_t;

/* Shared static buffers -- reused by every test (each calls circular_buff_init).
 * Keeps BSS usage to one circular_buff_t + one data array instead of 11+. */
static circular_buff_t s_buff;
static bma_accel_data_t s_data[2048];   /* largest test needs 2048 */


/* setUp/tearDown in test_setup.cpp */

// HELPER FUNCTIONS //

static inline void rng_seed(rng_t *rng, uint32_t seed)
{
    rng->state = seed ? seed : 0xDEADBEEF;
}

static inline uint32_t rng_next(rng_t *rng)
{
    uint32_t x = rng->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng->state = x;
    return x;
}

static int16_t clamp_12bit(int32_t v)
{
    if (v > MAX_G) return MAX_G;
    if (v < MIN_G) return MIN_G;
    return (int16_t)v;
}

bma_accel_data_t gen_mock_val(void)
{
    static rng_t rng;
    static uint32_t ts = 0;
    static int seeded = 0;

    if (!seeded) {
        rng_seed(&rng, 1234); // deterministic seed
        ts = 0;
        seeded = 1;
    }

    bma_accel_data_t v;

    v.x = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));
    v.y = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));
    v.z = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));

    v.timestamp_ms = ts;
    ts += 40; // assuming 50 Hz sample rate

    return v;
}


void gen_mock_array(bma_accel_data_t* arr, int n) {
    for (int i=0;i<n;i++)
        arr[i] = gen_mock_val();
}

//BASIC LOGIC AND FUNCTIONAL TESTING//
void test_circular_buffer_empty_after_init(){

    circular_buff_init(&s_buff);

    TEST_ASSERT_TRUE(circular_buff_empty(&s_buff));
    


}

void test_circular_buffer_write(){

    circular_buff_init(&s_buff);

    bma_accel_data_t mock_val = {1,2,3,1000};
    circular_buff_write(mock_val,&s_buff);

    TEST_ASSERT_EQUAL_INT(1,s_buff.head);
    TEST_ASSERT_EQUAL_INT(1,s_buff.count);
    TEST_ASSERT_EQUAL_INT(0,s_buff.tail);

}


void test_circular_buffer_read(){

    bma_accel_data_t mock_val;

    circular_buff_init(&s_buff);

    // CREATE MOCK VALUES //
    mock_val = {1,2,3,1000};

    circular_buff_write(mock_val,&s_buff);
    int count_exp = s_buff.count-1;
    int tail_exp = s_buff.tail+1;

    bma_accel_data_t read_val;
    circular_buff_read(&s_buff,&read_val);

    // check if tail is incrementing //
    TEST_ASSERT_EQUAL(1,read_val.x);
    TEST_ASSERT_EQUAL(2,read_val.y);
    TEST_ASSERT_EQUAL(3,read_val.z);
    TEST_ASSERT_EQUAL(1000,read_val.timestamp_ms);
    TEST_ASSERT_EQUAL(count_exp,s_buff.count);
    TEST_ASSERT_EQUAL(tail_exp, s_buff.tail);

}


void test_circular_buffer_write_seq(){

    circular_buff_init(&s_buff);
    const int num_samples = 100;

    gen_mock_array(s_data, num_samples);

    for(int s = 0; s<num_samples;s++){
        circular_buff_write(s_data[s],&s_buff);
    }

    TEST_ASSERT_EQUAL_INT(num_samples,s_buff.head);
    TEST_ASSERT_EQUAL_INT(num_samples,s_buff.count);
    TEST_ASSERT_EQUAL_INT(0,s_buff.tail);

}

void test_circular_buff_read_seq(){

    circular_buff_init(&s_buff);
    const int num_samples = 100;

    gen_mock_array(s_data, num_samples);

    for(int s = 0; s<num_samples; s++){
        circular_buff_write(s_data[s],&s_buff);
    }

    for(int s = 0; s<num_samples; s++){
        bma_accel_data_t stored_samp;
        circular_buff_read(&s_buff,&stored_samp);
        TEST_ASSERT_EQUAL_INT16(s_data[s].x,stored_samp.x);
        TEST_ASSERT_EQUAL_INT16(s_data[s].y,stored_samp.y);
        TEST_ASSERT_EQUAL_INT16(s_data[s].z,stored_samp.z);
        TEST_ASSERT_EQUAL_INT32(s_data[s].timestamp_ms,stored_samp.timestamp_ms);
    }

    TEST_ASSERT_EQUAL_INT(num_samples,s_buff.tail);
    TEST_ASSERT_EQUAL_INT(0,s_buff.count);
    TEST_ASSERT_EQUAL_INT(num_samples,s_buff.head);

}

void test_circular_buffer_sequential_readwrite(void)
{
    circular_buff_init(&s_buff);

    const int n = 100;

    /* Use first half of s_data for A, second half for B */
    bma_accel_data_t *a = &s_data[0];
    bma_accel_data_t *b = &s_data[100];

    gen_mock_array(a, n);
    gen_mock_array(b, n);

    /* -------------------------------------------------
     * Phase 1: Write A
     * ------------------------------------------------- */
    for (int i = 0; i < n; i++) {
        circular_buff_write(a[i], &s_buff);
    }

    /* -------------------------------------------------
     * Phase 2: Read half of A
     * ------------------------------------------------- */
    for (int i = 0; i < n / 2; i++) {
        bma_accel_data_t out;
        circular_buff_read(&s_buff,&out);

        TEST_ASSERT_EQUAL_MEMORY(
            &a[i],
            &out,
            sizeof(bma_accel_data_t)
        );
    }

    /* -------------------------------------------------
     * Phase 3: Write B
     * ------------------------------------------------- */
    for (int i = 0; i < n; i++) {
        circular_buff_write(b[i], &s_buff);
    }

    /* -------------------------------------------------
     * Phase 4: Read remainder of A
     * ------------------------------------------------- */
    for (int i = n / 2; i < n; i++) {
        bma_accel_data_t out;
        circular_buff_read(&s_buff,&out);

        TEST_ASSERT_EQUAL_MEMORY(
            &a[i],
            &out,
            sizeof(bma_accel_data_t)
        );
    }

    /* -------------------------------------------------
     * Phase 5: Read all of B
     * ------------------------------------------------- */
    for (int i = 0; i < n; i++) {
        bma_accel_data_t out;
        circular_buff_read(&s_buff,&out);

        TEST_ASSERT_EQUAL_MEMORY(
            &b[i],
            &out,
            sizeof(bma_accel_data_t)
        );
    }
}


void test_circular_buffer_overwrite(void)
{
    circular_buff_init(&s_buff);

    const int capacity = 1024;
    const int total_writes = capacity + 1; // 1 overflow

    gen_mock_array(s_data, total_writes);

    for (int i = 0; i < total_writes; i++) {
        circular_buff_write(s_data[i], &s_buff);
    }

    bma_accel_data_t out;

    // The first element should have been overwritten
    // Buffer should now contain s_data[1] ... s_data[1024]
    for (int i = 1; i < total_writes; i++) {
        bool ok = circular_buff_read(&s_buff, &out);
        TEST_ASSERT_TRUE(ok);
        TEST_ASSERT_EQUAL_MEMORY(&s_data[i], &out, sizeof(bma_accel_data_t));
    }
}

void test_circular_buffer_read_empty(void) {
    circular_buff_init(&s_buff);

    bma_accel_data_t out;
    bool ok = circular_buff_read(&s_buff, &out);
    TEST_ASSERT_FALSE(ok);
    TEST_ASSERT_EQUAL_INT(0, s_buff.count);
    TEST_ASSERT_EQUAL_INT(0, s_buff.head);
    TEST_ASSERT_EQUAL_INT(0, s_buff.tail);
}

void test_circular_buffer_multi_overwrite(void) {
    circular_buff_init(&s_buff);

    const int capacity = 1024;
    const int total_writes = capacity + 10; // multiple overwrites

    gen_mock_array(s_data, total_writes);

    for (int i = 0; i < total_writes; i++) {
        circular_buff_write(s_data[i], &s_buff);
    }

    bma_accel_data_t out;
    // Only the last 'capacity' elements should remain
    for (int i = total_writes - capacity; i < total_writes; i++) {
        bool ok = circular_buff_read(&s_buff, &out);
        TEST_ASSERT_TRUE(ok);
        TEST_ASSERT_EQUAL_MEMORY(&s_data[i], &out, sizeof(bma_accel_data_t));
    }

    // Buffer should now be empty
    bool ok = circular_buff_read(&s_buff, &out);
    TEST_ASSERT_FALSE(ok);
}

void test_circular_buffer_count_invariants(void) {
    circular_buff_init(&s_buff);

    const int capacity = 1024;

    // Write more than capacity
    for (int i = 0; i < capacity + 5; i++) {
        bma_accel_data_t val = gen_mock_val();
        circular_buff_write(val, &s_buff);
        TEST_ASSERT_TRUE(s_buff.count <= capacity);
        TEST_ASSERT_TRUE(s_buff.count >= 0);
    }

    // Read until empty
    bma_accel_data_t out;
    while (circular_buff_read(&s_buff, &out)) {
        TEST_ASSERT_TRUE(s_buff.count <= capacity);
        TEST_ASSERT_TRUE(s_buff.count >= 0);
    }
}

void test_circular_buffer_wrap_around(void) {
    circular_buff_init(&s_buff);

    const int capacity = 1024;           // small buffer to force wrap-around
    const int total_writes = capacity * 2; // write twice the capacity

    // Deterministic array generation
    rng_t rng;
    rng_seed(&rng, 4321);  // fixed seed for repeatability
    uint32_t ts = 0;
    for (int i = 0; i < total_writes; i++) {
        s_data[i].x = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));
        s_data[i].y = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));
        s_data[i].z = clamp_12bit((int32_t)((rng_next(&rng) & 0x0FFF) - 2048));
        s_data[i].timestamp_ms = ts;
        ts += 40;
    }

    // Write all elements into the buffer
    for (int i = 0; i < total_writes; i++) {
        circular_buff_write(s_data[i], &s_buff);
    }

    // Only the last 'capacity' elements should remain
    bma_accel_data_t out;
    for (int i = total_writes - capacity; i < total_writes; i++) {
        bool ok = circular_buff_read(&s_buff, &out);
        TEST_ASSERT_TRUE(ok);

        // Field-by-field comparison avoids padding issues
        TEST_ASSERT_EQUAL_INT16(s_data[i].x, out.x);
        TEST_ASSERT_EQUAL_INT16(s_data[i].y, out.y);
        TEST_ASSERT_EQUAL_INT16(s_data[i].z, out.z);
        TEST_ASSERT_EQUAL_INT32(s_data[i].timestamp_ms, out.timestamp_ms);
    }

    // After reading everything, buffer should be empty and head == tail
    TEST_ASSERT_EQUAL_INT(0, s_buff.count);
    TEST_ASSERT_EQUAL_INT(s_buff.head, s_buff.tail);
}


//----------------------------- TEST WRITES TO THE PSRAM ----------------------------// 









/* test entry is in test_setup.cpp */