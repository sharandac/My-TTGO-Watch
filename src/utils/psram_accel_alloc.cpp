#include "psram_accel_alloc.h"
#include "utils/alloc.h"
#include <sys/time.h>
#include <stddef.h>
#include <string.h>
#if defined(ESP32)
#include <esp_attr.h>
#endif

/* Single definition of globals for block list.
 * On ESP32 these use __NOINIT_ATTR so they survive soft reset (PSRAM content intact).
 * On native they're regular globals. */
#ifdef NATIVE_64BIT
header_t* current_accel_block_ptr = nullptr;
header_t* first_accel_block_ptr = nullptr;
uint32_t accel_block_valid_magic = 0;
uint32_t accel_seq_counter = 0;
#else
__NOINIT_ATTR header_t* current_accel_block_ptr;
__NOINIT_ATTR header_t* first_accel_block_ptr;
__NOINIT_ATTR uint32_t accel_block_valid_magic;
__NOINIT_ATTR uint32_t accel_seq_counter;
#endif

/* Pipeline: circular buffer batches are written here as single-linked blocks in PSRAM.
 * first_accel_block_ptr = oldest block; current_accel_block_ptr = latest; each
 * header_t.next points to the next (newer) block; latest block has next == nullptr. */


void psram_init(void){
    first_accel_block_ptr = nullptr;
    current_accel_block_ptr = nullptr;
    accel_block_valid_magic = 0;
    accel_seq_counter = 0;
}

header_t* psram_write(int num_elements, bma_accel_data_t* buf){
    if (num_elements < 0 || (unsigned)num_elements > PSRAM_ACCEL_MAX_ELEMENTS)
        return nullptr;
    size_t payload_size = (size_t)num_elements * sizeof(bma_accel_data_t);
    void* ptr = MALLOC(sizeof(header_t) + payload_size);

    if (!ptr) {
        log_w("No memory available for psram write");
        return nullptr;
    }

    if (!first_accel_block_ptr) {
        first_accel_block_ptr = (header_t*)ptr;
    }


    header_t* h = create_header(num_elements, buf, ptr);
    /* Copy payload into block (after header). */
    if (num_elements > 0 && buf) {
        memcpy((char*)ptr + sizeof(header_t), buf, num_elements * sizeof(bma_accel_data_t));
    }

    /* Checksum last: over whole block with checksum field treated as zero. */
    h->checksum = checksum(ptr, sizeof(header_t) + num_elements * sizeof(bma_accel_data_t));
    

    if (current_accel_block_ptr) {
        header_t* old = (header_t*)current_accel_block_ptr;
        old->next = ptr;
    }

    current_accel_block_ptr = (header_t*)ptr;

    return (header_t*)ptr;

}

uint32_t checksum(const void* ptr, size_t len){
    size_t cs_off = offsetof(header_t, checksum);
    size_t next_off = offsetof(header_t, next);
    uint32_t sum = 0;
    const uint8_t* p = (const uint8_t*)ptr;
    for (size_t i = 0; i < len; i++) {
        if (i >= cs_off && i < cs_off + sizeof(uint32_t)) continue;
        if (i >= next_off && i < next_off + sizeof(void*)) continue;
        sum += p[i];
    }
    return sum;
}

/** Compute checksum over block excluding the checksum and next fields (both mutate after write). */
static uint32_t checksum_block_for_validate(const void* ptr, size_t block_len){
    size_t cs_off = offsetof(header_t, checksum);
    size_t next_off = offsetof(header_t, next);
    uint32_t sum = 0;
    const uint8_t* p = (const uint8_t*)ptr;
    for (size_t i = 0; i < block_len; i++) {
        if (i >= cs_off && i < cs_off + sizeof(uint32_t)) continue;
        if (i >= next_off && i < next_off + sizeof(void*)) continue;
        sum += p[i];
    }
    return sum;
}

header_t* create_header(int num_elements, bma_accel_data_t* buf, void* ptr){
    header_t* h = (header_t*)ptr;
    h->magic_number = PSRAM_ACCEL_HEADER_MAGIC;
    h->version = PSRAM_ACCEL_HEADER_VERSION;
    h->header_size = sizeof(header_t);
    h->num_elements = num_elements;
    h->sync = false;
    h->next = nullptr;  /* this block is the latest */
    /* Block start/end from first/last sample so date is stored once per block. */
    if (num_elements > 0) {
        h->start_time_ms = buf[0].timestamp_ms;
        h->end_time_ms = buf[num_elements - 1].timestamp_ms;
    } else {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        uint32_t now_ms = (uint32_t)(tv.tv_sec * 1000UL + tv.tv_usec / 1000);
        h->start_time_ms = now_ms;
        h->end_time_ms = now_ms;
    }
    h->seq_num = accel_seq_counter++;
    h->checksum = 0;
    return h;

}
header_t* get_latest_block(void){
    if (!current_accel_block_ptr) return nullptr;
    header_t* current = (header_t*)current_accel_block_ptr;
    unsigned n = 0;
    while (current->next && n < PSRAM_ACCEL_MAX_ITERATIONS) {
        current = (header_t*)current->next;
        n++;
    }
    return current;
}
    



header_t *psram_read(int num_elements){

    //returned pointer is the block start, header then payload, head is at (bma_accel_data_t*)latest + sizeof(header_t)
    header_t* latest = get_latest_block();
    if (!latest) {
        log_w("No latest block found");
        return nullptr;
    }
    if (latest->magic_number != PSRAM_ACCEL_HEADER_MAGIC) {
        log_w("Invalid magic number in latest block");
        return nullptr;
    }
    if (latest->num_elements > PSRAM_ACCEL_MAX_ELEMENTS)
        return nullptr;
    size_t block_len = sizeof(header_t) + (size_t)latest->num_elements * sizeof(bma_accel_data_t);
    if (latest->checksum != checksum_block_for_validate(latest, block_len)) {
        log_w("Invalid checksum in latest block");
        return nullptr;
    }
    if (latest->num_elements != num_elements) {
        log_w("Invalid number of elements in latest block");
        return nullptr;
    }
    return latest;
}

header_t* psram_free(header_t* block) {
    if (!block) return nullptr;
    free(block);
    return nullptr;
}

/** Unlink and free the oldest block (first_accel_block_ptr). Call after sending its data. */
void psram_free_first_block(void) {
    if (!first_accel_block_ptr) return;
    header_t* old = first_accel_block_ptr;
    first_accel_block_ptr = (header_t*)old->next;
    if (!first_accel_block_ptr)
        current_accel_block_ptr = nullptr;
    free(old);
}

/** Return true if block has valid magic, sane num_elements, and checksum. */
bool psram_validate_block(header_t* block) {
    if (!block || block->magic_number != PSRAM_ACCEL_HEADER_MAGIC) return false;
    if (block->num_elements > PSRAM_ACCEL_MAX_ELEMENTS) return false;
    size_t block_len = sizeof(header_t) + (size_t)block->num_elements * sizeof(bma_accel_data_t);
    return block->checksum == checksum_block_for_validate(block, block_len);
}
