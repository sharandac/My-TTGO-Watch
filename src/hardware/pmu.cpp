#include "config.h"
#include <TTGO.h>
#include <soc/rtc.h>
#include "json_psram_allocator.h"

#include "display.h"
#include "pmu.h"
#include "powermgm.h"
#include "motor.h"
#include "blectl.h"
#include "callback.h"

#include "gui/statusbar.h"

static bool pmu_update = true;
volatile bool DRAM_ATTR pmu_irq_flag = false;
portMUX_TYPE DRAM_ATTR PMU_IRQ_Mux = portMUX_INITIALIZER_UNLOCKED;

callback_t *pmu_callback = NULL;
pmu_config_t pmu_config;

void IRAM_ATTR pmu_irq( void );
bool pmu_powermgm_event_cb( EventBits_t event, void *arg );
bool pmu_powermgm_loop_cb( EventBits_t event, void *arg );
bool pmu_blectl_event_cb( EventBits_t event, void *arg );
bool pmu_send_cb( EventBits_t event, void *arg );

void pmu_setup( void ) {

    pmu_read_config();

    TTGOClass *ttgo = TTGOClass::getWatch();

    // Turn on the IRQ used
    ttgo->power->adc1Enable( AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);
    ttgo->power->enableIRQ( AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_FINISHED_IRQ | AXP202_CHARGING_IRQ | AXP202_TIMER_TIMEOUT_IRQ, AXP202_ON );
    ttgo->power->clearIRQ();

    // enable coulumb counter
    if ( ttgo->power->EnableCoulombcounter() ) 
        log_e("enable coulumb counter failed!");    
    if ( pmu_config.high_charging_target_voltage ) {
        log_i("set target voltage to 4.36V");
        if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_36V ) )
            log_e("target voltage 4.36V set failed!");
    }
    else {
        log_i("set target voltage to 4.2V");
        if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_2V ) )
            log_e("target voltage 4.2V set failed!");
    }
    if ( ttgo->power->setChargeControlCur( 300 ) )
        log_e("charge current set failed!");
    if ( ttgo->power->setAdcSamplingRate( AXP_ADC_SAMPLING_RATE_200HZ ) )
        log_e("adc sample set failed!");

    // Turn off unused power
    ttgo->power->setPowerOutPut( AXP202_EXTEN, AXP202_OFF );
    ttgo->power->setPowerOutPut( AXP202_DCDC2, AXP202_OFF );
    ttgo->power->setPowerOutPut( AXP202_LDO4, AXP202_OFF );

    // Turn i2s DAC on
    ttgo->power->setLDO3Mode( AXP202_LDO3_MODE_DCIN );
    ttgo->power->setPowerOutPut( AXP202_LDO3, AXP202_ON );

    pinMode( AXP202_INT, INPUT );
    attachInterrupt( AXP202_INT, &pmu_irq, FALLING );

    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, pmu_powermgm_event_cb, "pmu" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP , pmu_powermgm_loop_cb, "pmu loop" );
    blectl_register_cb( BLECTL_CONNECT, pmu_blectl_event_cb, "pmu blectl" );
}

bool pmu_powermgm_loop_cb( EventBits_t event, void *arg ) {
    pmu_loop();
    return( true );
}

bool pmu_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_STANDBY:          pmu_standby();
                                        break;
        case POWERMGM_WAKEUP:           pmu_wakeup();
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   pmu_wakeup();
                                        break;
    }
    return( true );
}

bool pmu_blectl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_CONNECT:
            pmu_update = true;
    }
    return( true );
}

void IRAM_ATTR  pmu_irq( void ) {
    portENTER_CRITICAL_ISR(&PMU_IRQ_Mux);
    pmu_irq_flag = true;
    portEXIT_CRITICAL_ISR(&PMU_IRQ_Mux);
}

void pmu_loop( void ) {
    static uint64_t nextmillis = 0;
    static int32_t percent = 0;
    TTGOClass *ttgo = TTGOClass::getWatch();
    /*
     * handle IRQ event
     */
    portENTER_CRITICAL(&PMU_IRQ_Mux);
    bool temp_pmu_irq_flag = pmu_irq_flag;
    pmu_irq_flag = false;
    portEXIT_CRITICAL(&PMU_IRQ_Mux);

    if ( temp_pmu_irq_flag ) {        
        ttgo->power->readIRQ();
        if ( ttgo->power->isVbusPlugInIRQ() ) {
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
        }
        if ( ttgo->power->isVbusRemoveIRQ() ) {
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
        }
        if ( ttgo->power->isChargingIRQ() ) {
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
        }
        if ( ttgo->power->isChargingDoneIRQ() ) {
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
        }
        if ( ttgo->power->isPEKShortPressIRQ() ) {
            powermgm_set_event( POWERMGM_PMU_BUTTON );
            ttgo->power->clearIRQ();
            return;
        }
        if ( ttgo->power->isTimerTimeoutIRQ() ) {
            powermgm_set_event( POWERMGM_SILENCE_WAKEUP_REQUEST );
            ttgo->power->clearTimerStatus();
            ttgo->power->offTimer();
            ttgo->power->clearIRQ();
            return;
        }
        ttgo->power->clearIRQ();
        pmu_update = true;
    }

    if ( nextmillis < millis() ) {
        nextmillis = millis() + 30000L;
        if ( pmu_get_battery_percent() != percent ) {
            pmu_update = true;
        }
    }

    if ( pmu_update ) {

        char msg[64]="";
        percent = pmu_get_battery_percent();
        snprintf( msg, sizeof(msg), "\r\n{t:\"status\", bat:%d}\r\n", percent );
        blectl_send_msg( msg );

        bool plug = ttgo->power->isVBUSPlug();
        bool charging = ttgo->power->isChargeing();
        pmu_send_cb( PMUCTL_CHARGING, (void*)&charging );
        pmu_send_cb( PMUCTL_VBUS_PLUG, (void*)&plug );
        pmu_send_cb( PMUCTL_BATTERY_PERCENT, (void*)&percent );
        pmu_update = false;
    }
}

bool pmu_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( pmu_callback == NULL ) {
        pmu_callback = callback_init( "pmu" );
        if ( pmu_callback == NULL ) {
            log_e("pmu_callback alloc failed");
            while(true);
        }
    }
    return( callback_register( pmu_callback, event, callback_func, id ) );
}

bool pmu_send_cb( EventBits_t event, void *arg ) {
    return( callback_send( pmu_callback, event, arg ) );
}

void pmu_shutdown( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->power->shutdown();
}

void pmu_standby( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    ttgo->power->clearTimerStatus();
    if ( pmu_get_silence_wakeup() ) {
        if ( ttgo->power->isChargeing() || ttgo->power->isVBUSPlug() ) {
            ttgo->power->setTimer( pmu_config.silence_wakeup_interval_vbplug );
            log_i("enable silence wakeup timer, %dmin", pmu_config.silence_wakeup_interval_vbplug );
        }
        else {
            ttgo->power->setTimer( pmu_config.silence_wakeup_interval );
            log_i("enable silence wakeup timer, %dmin", pmu_config.silence_wakeup_interval );
        }
    }

    if ( pmu_get_experimental_power_save() ) {
        ttgo->power->setDCDC3Voltage( pmu_config.experimental_power_save_voltage );
        log_i("go standby, enable %dmV standby voltage", pmu_config.experimental_power_save_voltage );
    } 
    else {
        ttgo->power->setDCDC3Voltage( pmu_config.normal_power_save_voltage );
        log_i("go standby, enable %dmV standby voltage", pmu_config.normal_power_save_voltage );
    }
    ttgo->power->setPowerOutPut( AXP202_LDO2, AXP202_OFF );

    gpio_wakeup_enable( (gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL );
    esp_sleep_enable_gpio_wakeup ();
}

void pmu_wakeup( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    if ( pmu_get_experimental_power_save() ) {
        ttgo->power->setDCDC3Voltage( pmu_config.experimental_normal_voltage );
        log_i("go wakeup, enable %dmV voltage", pmu_config.experimental_normal_voltage );
    } 
    else {
        ttgo->power->setDCDC3Voltage( pmu_config.normal_voltage );
        log_i("go wakeup, enable %dmV voltage", pmu_config.normal_voltage );
    }

    ttgo->power->clearTimerStatus();
    ttgo->power->offTimer();

    ttgo->power->setPowerOutPut( AXP202_LDO2, AXP202_ON );

    pmu_update = true;
}

void pmu_save_config( void ) {
    fs::File file = SPIFFS.open( PMU_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", PMU_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 3000 );

        doc["silence_wakeup"] = pmu_config.silence_wakeup;
        doc["silence_wakeup_interval"] = pmu_config.silence_wakeup_interval;
        doc["silence_wakeup_interval_vbplug"] = pmu_config.silence_wakeup_interval_vbplug;
        doc["experimental_power_save"] = pmu_config.experimental_power_save;
        doc["normal_voltage"] = pmu_config.normal_voltage;
        doc["normal_power_save_voltage"] = pmu_config.normal_power_save_voltage;
        doc["experimental_normal_voltage"] = pmu_config.experimental_normal_voltage;
        doc["experimental_power_save_voltage"] = pmu_config.experimental_power_save_voltage;
        doc["compute_percent"] = pmu_config.compute_percent;
        doc["high_charging_target_voltage"] = pmu_config.high_charging_target_voltage;
        doc["designed_battery_cap"] = pmu_config.designed_battery_cap;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void pmu_read_config( void ) {
    fs::File file = SPIFFS.open( PMU_JSON_CONFIG_FILE, FILE_READ );

    if (!file) {
        log_e("Can't open file: %s!", PMU_JSON_CONFIG_FILE );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 2 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            pmu_config.silence_wakeup = doc["silence_wakeup"] | false;
            pmu_config.silence_wakeup_interval = doc["silence_wakeup_interval"] | SILENCEWAKEINTERVAL;
            pmu_config.silence_wakeup_interval_vbplug = doc["silence_wakeup_interval_vbplug"] | SILENCEWAKEINTERVAL_PLUG;
            pmu_config.experimental_power_save = doc["experimental_power_save"] | false;
            pmu_config.compute_percent = doc["compute_percent"] | false;
            pmu_config.high_charging_target_voltage = doc["high_charging_target_voltage"] | false;
            pmu_config.designed_battery_cap = doc["designed_battery_cap"] | 300;
            pmu_config.normal_voltage = doc["normal_voltage"] | NORMALVOLTAGE;
            pmu_config.normal_power_save_voltage = doc["normal_power_save_voltage"] | NORMALPOWERSAVEVOLTAGE;
            pmu_config.experimental_normal_voltage = doc["experimental_normal_voltage"] | EXPERIMENTALNORMALVOLTAGE;
            pmu_config.experimental_power_save_voltage = doc["experimental_power_save_voltage"] | EXPERIMENTALPOWERSAVEVOLTAGE;
        }        
        doc.clear();
    }
    file.close();
}

bool pmu_get_silence_wakeup( void ) {
    return( pmu_config.silence_wakeup );
}

bool pmu_get_high_charging_target_voltage( void ) {
    return( pmu_config.high_charging_target_voltage );
}

void pmu_set_high_charging_target_voltage( bool value ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    pmu_config.high_charging_target_voltage = value;

    if ( pmu_config.high_charging_target_voltage ) {
        log_i("set target voltage to 4.36V");
        if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_36V ) )
            log_e("target voltage 4.36V set failed!");
    }
    else {
        log_i("set target voltage to 4.2V");
        if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_2V ) )
            log_e("target voltage 4.2V set failed!");
    }

    pmu_save_config();
}

int32_t pmu_get_designed_battery_cap( void ) {
    return( pmu_config.designed_battery_cap );
}

void pmu_set_silence_wakeup( bool value ) {
    pmu_config.silence_wakeup = value;
    pmu_save_config();
}

bool pmu_get_calculated_percent( void ) {
    return( pmu_config.compute_percent );
}

bool pmu_get_experimental_power_save( void ) {
    return( pmu_config.experimental_power_save );
}

void pmu_set_calculated_percent( bool value ) {
    pmu_config.compute_percent = value;
    pmu_save_config();
}

void pmu_set_experimental_power_save( bool value ) {
    pmu_config.experimental_power_save = value;
    pmu_save_config();
}

int32_t pmu_get_battery_percent( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    if ( ttgo->power->getBattChargeCoulomb() < ttgo->power->getBattDischargeCoulomb() || ttgo->power->getBattVoltage() < 3200 ) {
        ttgo->power->ClearCoulombcounter();
    }

    if ( pmu_get_calculated_percent() ) {
        return( ( ttgo->power->getCoulombData() / pmu_config.designed_battery_cap ) * 100 );
    }
    else {
        return( ttgo->power->getBattPercentage() );
    }
}

float pmu_get_battery_voltage( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    return( ttgo->power->getBattVoltage() );
}

float pmu_get_battery_charge_current( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    return( ttgo->power->getBattChargeCurrent() );
}

float pmu_get_battery_discharge_current( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    return( ttgo->power->getBattDischargeCurrent() );
}

float pmu_get_vbus_voltage( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    return( ttgo->power->getVbusVoltage() );
}

float pmu_get_coulumb_data( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    return( ttgo->power->getCoulombData() );
}

bool pmu_is_charging( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    return( ttgo->power->isChargeing() );
}

bool pmu_is_vbus_plug( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    return( ttgo->power->isVBUSPlug() );
}