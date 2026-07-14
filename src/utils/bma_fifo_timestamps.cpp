#include "utils/bma_fifo_timestamps.h"

uint32_t bma_fifo_sample_timestamp_ms(
    uint32_t drain_end_ms,
    unsigned sample_index,
    unsigned sample_count,
    unsigned interval_ms)
{
    if (sample_count == 0 || sample_index >= sample_count) {
        return drain_end_ms;
    }
    const unsigned age_ms = (sample_count - 1U - sample_index) * interval_ms;
    return (age_ms >= drain_end_ms) ? 0U : (drain_end_ms - age_ms);
}
