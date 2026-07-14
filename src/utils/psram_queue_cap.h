#ifndef _PSRAM_QUEUE_CAP_H
    #define _PSRAM_QUEUE_CAP_H

    #include <stddef.h>

    /** Blocks to drop before accepting one new block when at cap (0 = none). */
    size_t psram_queue_cap_drops_needed(size_t queued, size_t max_blocks);

#endif
