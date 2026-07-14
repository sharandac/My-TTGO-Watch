#include "circular_buff.h"

#define NDEBUG
#include <assert.h>

void circular_buff_init(circular_buff_t* circ_buff){
    circ_buff->head = 0;
    circ_buff->tail = 0;
    circ_buff->count = 0;
}

bool circular_buff_empty(circular_buff_t* circ_buff){
    return circ_buff->count == 0;
}

void circular_buff_write(bma_accel_data_t input, circular_buff_t* circ_buff)
{
    assert(circ_buff != NULL);
    assert(circ_buff->count <= BUFFER_SIZE);

    if (circ_buff->count < BUFFER_SIZE) {
        // buffer not full, normal write
        circ_buff->buffer[circ_buff->head] = input;
        circ_buff->count++;
        circ_buff->head = (circ_buff->head + 1) % BUFFER_SIZE;
    } else {
        // buffer full, overwrite oldest
        circ_buff->buffer[circ_buff->head] = input;
        circ_buff->head = (circ_buff->head + 1) % BUFFER_SIZE;
        circ_buff->tail = (circ_buff->tail + 1) % BUFFER_SIZE;
    }
}


bool circular_buff_read(circular_buff_t* circ_buff,bma_accel_data_t* out){

    assert(circ_buff != NULL);
    assert(out != NULL);
    assert(circ_buff->count > 0);

    if(circ_buff->count == 0){
        return false;
    }
    *out = circ_buff->buffer[circ_buff->tail];
    circ_buff->tail = (circ_buff->tail+1) % BUFFER_SIZE;
    circ_buff->count--;
    return true;
}

int circular_buff_count(circular_buff_t* circ_buff) {
    if (!circ_buff) return 0;
    return circ_buff->count;
}
