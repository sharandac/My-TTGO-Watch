#include "config.h"
#include <TTGO.h>
#include <soc/rtc.h>

#include "pmu.h"
#include "powermgm.h"
#include "statusbar.h"
#include "motor.h"

EventGroupHandle_t pmu_event_handle = NULL;

void IRAM_ATTR pmu_irq( void );

/*
 * init the pmu: AXP202 
 */
void pmu_setup( TTGOClass *ttgo ) {
    pmu_event_handle = xEventGroupCreate();

    // Turn on the IRQ used
    ttgo->power->adc1Enable(AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);
    ttgo->power->enableIRQ(AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_FINISHED_IRQ, AXP202_ON);
    ttgo->power->clearIRQ();

    // Turn off unused power
    ttgo->power->setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
    ttgo->power->setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
    ttgo->power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
    ttgo->power->setPowerOutPut(AXP202_LDO4, AXP202_OFF);

    pinMode(AXP202_INT, INPUT);

    attachInterrupt(AXP202_INT, &pmu_irq , FALLING);
}

/*
 * IRQ routine AXP202
 */
void IRAM_ATTR  pmu_irq( void ) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    /*
     * setup an PMU event
     */
    xEventGroupSetBitsFromISR( pmu_event_handle, PMU_EVENT_AXP_INT, &xHigherPriorityTaskWoken );
    if ( xHigherPriorityTaskWoken ) {
        portYIELD_FROM_ISR();
    }
    /*
     * fast wake up from IRQ
     */
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);
    // setCpuFrequencyMhz(240);
}

/*
 * loop routine for handling IRQ in main loop
 */
void pmu_loop( TTGOClass *ttgo ) {
    static uint64_t nextmillis = 0;
    bool updatetrigger = false;

    /*
     * handle IRQ event
     */
    if ( xEventGroupGetBitsFromISR( pmu_event_handle ) & PMU_EVENT_AXP_INT ) {
        ttgo->power->readIRQ();
        if (ttgo->power->isVbusPlugInIRQ()) {
            powermgm_set_event( POWERMGM_PMU_BATTERY );
            motor_vibe( 1 );
            updatetrigger = true;
        }
        if (ttgo->power->isVbusRemoveIRQ()) {
            powermgm_set_event( POWERMGM_PMU_BATTERY );
            motor_vibe( 1 );
            updatetrigger = true;
        }
        if (ttgo->power->isChargingDoneIRQ()) {
            powermgm_set_event( POWERMGM_PMU_BATTERY );
            motor_vibe( 1 );
            updatetrigger = true;
        }
        if (ttgo->power->isPEKShortPressIRQ()) {
            updatetrigger = true;
            powermgm_set_event( POWERMGM_PMU_BUTTON );
            ttgo->power->clearIRQ();
            return;
        }
        ttgo->power->clearIRQ();
        xEventGroupClearBits( pmu_event_handle, PMU_EVENT_AXP_INT );
    }

    if ( !powermgm_get_event( POWERMGM_STANDBY ) ) {
        if ( nextmillis < millis() || updatetrigger == true ) {
            nextmillis = millis() + 1000;
            statusbar_update_battery( ttgo->power->getBattPercentage(), ttgo->power->isChargeing(), ttgo->power->isVBUSPlug() );
        }
    }
}