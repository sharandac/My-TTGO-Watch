#ifndef _BMA_FIFO_H
#define _BMA_FIFO_H

#include "motion.h"
#include <stdint.h>

/** True when this firmware build supports BMA423 hardware FIFO sampling. */
bool bma_fifo_is_enabled(void);

/** True while FIFO is armed and draining after a motion event. */
bool bma_fifo_is_active(void);

/** Configure BMA423 accel FIFO (50 Hz). Called on motion, not at pipeline start. */
bool bma_fifo_setup(void);

/** Disable FIFO watermark/full interrupts (idle timeout or pipeline stop). */
void bma_fifo_teardown(void);

/** BMA423 wakeup/tilt detected — start or extend a recording window. */
void bma_fifo_on_motion(void);

/** Call from pipeline task; stops FIFO after idle timeout with no new samples. */
void bma_fifo_idle_tick(void);

/** Drain any pending FIFO bytes into the Parkinsense pipeline. Safe from task or powermgm loop. */
unsigned bma_fifo_service(void);

/**
 * Drain FIFO + clear latched INT before esp_light_sleep.
 * Avoids instant GPIO-39 wake when INT1 is still high (HIGH_LEVEL trigger).
 */
void bma_fifo_prepare_light_sleep(void);

/** Samples drained since boot (diagnostics). */
unsigned bma_fifo_get_samples_drained(void);

#endif
