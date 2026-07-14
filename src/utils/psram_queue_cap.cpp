#include "psram_queue_cap.h"

size_t psram_queue_cap_drops_needed(size_t queued, size_t max_blocks) {
    if (max_blocks == 0 || queued < max_blocks) {
        return 0;
    }
    return (queued - max_blocks) + 1;
}
