#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef NATIVE_64BIT
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
    uint32_t timestamp_ms;
} bma_accel_data_t;
#else
#include "../hardware/motion.h"
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE (1024)
#endif

typedef struct circular_buff_s {
    bma_accel_data_t buffer[BUFFER_SIZE];
    int head;
    int tail;
    int count;
} circular_buff_t;

void circular_buff_init(circular_buff_t* circ_buff);
void circular_buff_write(bma_accel_data_t input, circular_buff_t* circ_buff);
bool circular_buff_empty(circular_buff_t* circ_buff);
bool circular_buff_read(circular_buff_t* circ_buff, bma_accel_data_t* out);
/** Number of samples currently in the buffer (0..BUFFER_SIZE). */
int circular_buff_count(circular_buff_t* circ_buff);

#endif
