#ifndef _BMA_FIFO_TIMESTAMPS_H
#define _BMA_FIFO_TIMESTAMPS_H

#include <stdint.h>

/** Assign wall-clock ms to sample i of n drained at once (oldest first). */
uint32_t bma_fifo_sample_timestamp_ms(
    uint32_t drain_end_ms,
    unsigned sample_index,
    unsigned sample_count,
    unsigned interval_ms);

#endif
