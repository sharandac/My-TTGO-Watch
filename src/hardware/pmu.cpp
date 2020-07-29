#include "config.h"
#include <TTGO.h>
#include <soc/rtc.h>

#include "pmu.h"
#include "powermgm.h"
#include "motor.h"

#include "gui/statusbar.h"

EventGroupHandle_t pmu_event_handle = NULL;

void IRAM_ATTR pmu_irq( void );

/*
 * init the pmu: AXP202 
 */
void pmu_setup( TTGOClass *ttgo ) {
    pmu_event_handle = xEventGroupCreate();

    // Turn on the IRQ used
    ttgo->power->adc1Enable( AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);
    ttgo->power->enableIRQ( AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_FINISHED_IRQ | AXP202_TIMER_TIMEOUT_IRQ, AXP202_ON );
    ttgo->power->clearIRQ();

    // enable coulumb counter
    if ( ttgo->power->EnableCoulombcounter() ) 
        log_e("enable coulumb counter failed!");    
    if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_2V ) )
        log_e("target voltage set failed!");
    if ( ttgo->power->setChargeControlCur( 300 ) )
        log_e("charge current set failed!");
    if ( ttgo->power->setAdcSamplingRate( AXP_ADC_SAMPLING_RATE_200HZ ) )
        log_e("adc sample set failed!");

    // Turn off unused power
    ttgo->power->setPowerOutPut( AXP202_EXTEN, AXP202_OFF );
    ttgo->power->setPowerOutPut( AXP202_DCDC2, AXP202_OFF );
    ttgo->power->setPowerOutPut( AXP202_LDO3, AXP202_OFF );
    ttgo->power->setPowerOutPut( AXP202_LDO4, AXP202_OFF );

    pinMode( AXP202_INT, INPUT );
    attachInterrupt( AXP202_INT, &pmu_irq, FALLING );
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
    // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_240M);
    setCpuFrequencyMhz(240);
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
        if ( ttgo->power->isTimerTimeoutIRQ() ) {
            updatetrigger = true;
            powermgm_set_event( POWERMGM_SILENCE_WAKEUP_REQUEST );
            ttgo->power->clearTimerStatus();
            ttgo->power->offTimer();
            ttgo->power->clearIRQ();
            return;
        }
        ttgo->power->clearIRQ();
        xEventGroupClearBits( pmu_event_handle, PMU_EVENT_AXP_INT );
    }

    if ( !powermgm_get_event( POWERMGM_STANDBY ) ) {
        if ( nextmillis < millis() || updatetrigger == true ) {
            nextmillis = millis() + 1000;
            statusbar_update_battery( pmu_get_byttery_percent( ttgo ), ttgo->power->isChargeing(), ttgo->power->isVBUSPlug() );
        }
    }
}

uint32_t pmu_get_byttery_percent( TTGOClass *ttgo ) {
    // discharg coulumb higher then charge coulumb, battery state unknow and set to zero
    if ( ttgo->power->getBattChargeCoulomb() < ttgo->power->getBattDischargeCoulomb() || ttgo->power->getBattVoltage() < 3200 ) {
        ttgo->power->ClearCoulombcounter();
        return( -1 );
    }
    return( ( ttgo->power->getCoulombData() / PMU_BATTERY_CAP ) * 100 );
}