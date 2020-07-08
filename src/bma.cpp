#include "config.h"
#include <TTGO.h>
#include <soc/rtc.h>

#include "bma.h"
#include "powermgm.h"
#include "statusbar.h"

EventGroupHandle_t bma_event_handle = NULL;

void IRAM_ATTR  bma_irq( void );

/*
 *
 */
void bma_setup( TTGOClass *ttgo ) {

    bma_event_handle = xEventGroupCreate();

    ttgo->bma->begin();
    ttgo->bma->attachInterrupt();
    ttgo->bma->direction();

    pinMode(BMA423_INT1, INPUT);
    attachInterrupt(BMA423_INT1, bma_irq, RISING );

    ttgo->bma->enableWakeupInterrupt( true );
}

/*
 *
 */
void IRAM_ATTR  bma_irq( void ) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xEventGroupSetBitsFromISR(bma_event_handle, BMA_EVENT_INT, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR ();
    }
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);
    // setCpuFrequencyMhz(240);
}

/*
 * loop routine for handling IRQ in main loop
 */
void bma_loop( TTGOClass *ttgo ) {
    /*
     * handle IRQ event
     */
    if ( xEventGroupGetBitsFromISR( bma_event_handle ) & BMA_EVENT_INT ) {
        while( !ttgo->bma->readInterrupt() );
        if ( ttgo->bma->isDoubleClick() ) {
            powermgm_set_event( POWERMGM_BMA_WAKEUP );
            xEventGroupClearBitsFromISR( bma_event_handle, BMA_EVENT_INT );
            return;
        }
    }

    if ( !powermgm_get_event( POWERMGM_STANDBY ) && xEventGroupGetBitsFromISR( bma_event_handle ) & BMA_EVENT_INT ) {
        statusbar_update_stepcounter( ttgo->bma->getCounter() );
        xEventGroupClearBitsFromISR( bma_event_handle, BMA_EVENT_INT );
    }
}