/****************************************************************************
 *   Tu May 22 21:23:51 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "config.h"
#include <TTGO.h>
#include <soc/rtc.h>

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
void pmu_write_log( const char * filename );

void pmu_setup( void ) {
    /*
     * read config from SPIFF
     */
    pmu_config.load();

    TTGOClass *ttgo = TTGOClass::getWatch();

    /*
     * Turn on the IRQ used
     */
    ttgo->power->adc1Enable( AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);
    ttgo->power->enableIRQ( AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ
                            | AXP202_CHARGING_FINISHED_IRQ | AXP202_CHARGING_IRQ
                            | AXP202_TIMER_TIMEOUT_IRQ
                            | AXP202_BATT_REMOVED_IRQ | AXP202_BATT_CONNECT_IRQ
                            , AXP202_ON );
    ttgo->power->clearIRQ();
    /*
     * delete old charge logfile
     */
    if ( ttgo->power->isVBUSPlug() ) {
        SPIFFS.remove( PMU_CHARGE_LOG_FILENAME );
    }
    /*
     * enable coulumb counter and set target voltage for charging
     */
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
    /*
     * Turn off unused power
     */
    ttgo->power->setPowerOutPut( AXP202_EXTEN, AXP202_OFF );
    ttgo->power->setPowerOutPut( AXP202_DCDC2, AXP202_OFF );
    ttgo->power->setPowerOutPut( AXP202_LDO4, AXP202_OFF );
    /*
     * Turn i2s DAC on
     */
    ttgo->power->setLDO3Mode( AXP202_LDO3_MODE_DCIN );
    ttgo->power->setPowerOutPut( AXP202_LDO3, AXP202_ON );
    /*
     * register IRQ function and GPIO pin
     */
    pinMode( AXP202_INT, INPUT );
    attachInterrupt( AXP202_INT, &pmu_irq, FALLING );
    /*
     * register all powermem callback functions
     */
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_ENABLE_INTERRUPTS | POWERMGM_DISABLE_INTERRUPTS , pmu_powermgm_event_cb, "powermgm pmu" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP , pmu_powermgm_loop_cb, "powermgm pmu loop" );
    /*
     * register blectl callback function
     */
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
        case POWERMGM_ENABLE_INTERRUPTS:
                                        attachInterrupt( AXP202_INT, &pmu_irq, FALLING );
                                        break;
        case POWERMGM_DISABLE_INTERRUPTS:
                                        detachInterrupt( AXP202_INT );
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
    TTGOClass *ttgo = TTGOClass::getWatch();

    static uint64_t nextmillis = 0;
    static int32_t percent = pmu_get_battery_percent();
    int32_t tmp_percent = 0;
    static bool plug = ttgo->power->isVBUSPlug();
    static bool charging = ttgo->power->isChargeing();
    static bool battery = ttgo->power->isBatteryConnect();

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
            /*
             * set an wakeup request and
             * delete old charging logfile when plug in and
             * set variable plug to true
             */
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            SPIFFS.remove( PMU_CHARGE_LOG_FILENAME );
            plug = true;
        }
        if ( ttgo->power->isVbusRemoveIRQ() ) {
            /*
             * set an wakeup request and
             * remove old discharging log file when unplug
             * set variable plug and chargingto false
             */
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            SPIFFS.remove( PMU_DISCHARGE_LOG_FILENAME );
            charging = false;
            plug = false;
        }
        if ( ttgo->power->isChargingIRQ() ) {
            /*
             * set an wakeup request and
             * set variable charging to true
             */
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            charging = true;
        }
        if ( ttgo->power->isChargingDoneIRQ() ) {
            /*
             * set an wakeup request and
             * set variable charging to false
             */
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            charging = false;
        }
        if ( ttgo->power->isBattPlugInIRQ() ) {
            /*
             * set an wakeup request and
             * set variable charging to false
             */
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            battery = true;
        }
        if ( ttgo->power->isBattRemoveIRQ() ) {
            /*
             * set an wakeup request and
             * set variable charging to false
             */
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            battery = false;
        }
        if ( ttgo->power->isPEKShortPressIRQ() ) {
            /*
             * set an wakeup request
             * clear IRQ state
             * send PMUCTL_SHORT_PRESS event
             * fast return for faster wakeup
             */
            powermgm_set_event( POWERMGM_POWER_BUTTON );
            ttgo->power->clearIRQ();
            pmu_send_cb( PMUCTL_SHORT_PRESS, NULL );
            return;
        }
        if ( ttgo->power->isPEKLongtPressIRQ() ) {
            /*
             * clear IRQ state
             * set an wakeup request
             * send PMUCTL_LONG_PRESS event
             * fast return for faster wakeup
             */
            ttgo->power->clearIRQ();
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            pmu_send_cb( PMUCTL_LONG_PRESS, NULL );
            return;
        }
        if ( ttgo->power->isTimerTimeoutIRQ() ) {
            /*
             * clear pmu timer and IRQ state
             * set an silence wakeup request
             * send PMUCTL_LONG_PRESS event
             * fast return for faster wakeup
             */
            ttgo->power->clearTimerStatus();
            ttgo->power->offTimer();
            ttgo->power->clearIRQ();
            powermgm_set_event( POWERMGM_SILENCE_WAKEUP_REQUEST );
            pmu_send_cb( PMUCTL_TIMER_TIMEOUT, NULL );
            return;
        }
        /*
         * clear IRQ
         * set update flag
         */
        ttgo->power->clearIRQ();
        pmu_update = true;
    }

    /*
     *  check if an update necessary and set percent variable if change
     */
    if ( nextmillis < millis() ) {
        /*
         * reduce battery update interval to 60s if we are in standby
         */
        if ( powermgm_get_event( POWERMGM_STANDBY ) ) {
            nextmillis = millis() + 60000L;
        }
        else {
            nextmillis = millis() + 10000L;
        }

        /*
         * only update if an change is detected
         */
        tmp_percent = pmu_get_battery_percent();
        if ( tmp_percent != percent ) {
            pmu_update = true;
            percent = tmp_percent;
        }

        /* 
         * log pmu data if enabled
         */
        if ( pmu_config.pmu_logging ) {
            if ( plug && charging ) {
                pmu_write_log( PMU_CHARGE_LOG_FILENAME );
            }
            else {
                pmu_write_log( PMU_DISCHARGE_LOG_FILENAME );
            }
        }

    }
    /*
     * check if update flag is set
     */
    if ( pmu_update ) {
        /*
         * Encode values on a single
         * As percent is supposed to be <=100% it is encoded on a single byte
         * We can use other bits to code the 2 booleans
         */
        int32_t msg = percent < 0 ? 0 : percent;
        msg |= plug ? PMUCTL_STATUS_PLUG : 0;
        msg |= charging ? PMUCTL_STATUS_CHARGING : 0;
        msg |= battery ? PMUCTL_STATUS_BATTERY : 0;
        /*
         * send updates via pmu event
         */
        pmu_send_cb( PMUCTL_STATUS, (void*)&msg );
        log_d("battery state: %d%%, %s, %s, %s (0x%04x)", percent, plug ? "connected" : "unconnected", charging ? "charging" : "discharge", battery ? "battery ok" : "no battery", msg );
        /*
         * clear update flag
         */
        pmu_update = false;
    }
}

bool pmu_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    /*
     * check if an callback table exist, if not allocate a callback table
     */
    if ( pmu_callback == NULL ) {
        pmu_callback = callback_init( "pmu" );
        if ( pmu_callback == NULL ) {
            log_e("pmu_callback alloc failed");
            while( true );
        }
    }
    /*
     * register an callback entry and return them
     */
    return( callback_register( pmu_callback, event, callback_func, id ) );
}

bool pmu_send_cb( EventBits_t event, void *arg ) {
    /*
     * call all callbacks with her event mask
     */
    return( callback_send( pmu_callback, event, arg ) );
}

void pmu_shutdown( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->power->shutdown();
}

void pmu_standby( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    ttgo->power->clearTimerStatus();
    /*
     * if silence wakeup enabled set the wakeup timer, depending on vplug
     */
    if ( pmu_get_silence_wakeup() ) {
        if ( ttgo->power->isChargeing() || ttgo->power->isVBUSPlug() ) {
            ttgo->power->setTimer( pmu_config.silence_wakeup_interval_vbplug );
            log_d("enable silence wakeup timer, %dmin", pmu_config.silence_wakeup_interval_vbplug );
        }
        else {
            ttgo->power->setTimer( pmu_config.silence_wakeup_interval );
            log_d("enable silence wakeup timer, %dmin", pmu_config.silence_wakeup_interval );
        }
    }

    /*
     * set powersave voltage depending on settings
     */
    if ( pmu_get_experimental_power_save() ) {
        ttgo->power->setDCDC3Voltage( pmu_config.experimental_power_save_voltage );
        log_d("go standby, enable %dmV standby voltage", pmu_config.experimental_power_save_voltage );
    } 
    else {
        ttgo->power->setDCDC3Voltage( pmu_config.normal_power_save_voltage );
        log_d("go standby, enable %dmV standby voltage", pmu_config.normal_power_save_voltage );
    }
    /*
     * disable LD02, sound?
     */
    ttgo->power->setPowerOutPut( AXP202_LDO2, AXP202_OFF );
    /*
     * enable GPIO in lightsleep for wakeup
     */
    gpio_wakeup_enable( (gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL );
    esp_sleep_enable_gpio_wakeup ();
}

void pmu_wakeup( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    /*
     * set normal voltage depending on settings
     */
    if ( pmu_get_experimental_power_save() ) {
        ttgo->power->setDCDC3Voltage( pmu_config.experimental_normal_voltage );
        log_d("go wakeup, enable %dmV voltage", pmu_config.experimental_normal_voltage );
    } 
    else {
        ttgo->power->setDCDC3Voltage( pmu_config.normal_voltage );
        log_d("go wakeup, enable %dmV voltage", pmu_config.normal_voltage );
    }
    /*
     * clear timer
     */
    ttgo->power->clearTimerStatus();
    ttgo->power->offTimer();
    /*
     * enable LDO2, backlight?
     */
    ttgo->power->setPowerOutPut( AXP202_LDO2, AXP202_ON );
    /*
     * set update to force update the screen and so on
     */
    pmu_update = true;
}

void pmu_save_config( void ) {
    pmu_config.save();
}

void pmu_read_config( void ) {
    pmu_config.load();
}

bool pmu_get_logging( void ) {
    return( pmu_config.pmu_logging );
}

void pmu_set_logging( bool logging ) {
    pmu_config.pmu_logging = logging;
    pmu_save_config();
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
        log_d("set target voltage to 4.36V");
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

void pmu_set_safe_voltage_for_update( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    
    ttgo->power->setDCDC3Voltage( NORMALVOLTAGE + 100 );
    log_d("set %dmV voltage", NORMALVOLTAGE );

    vTaskDelay(250);
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

void pmu_write_log( const char * filename ) {
    time_t now;
    struct tm info;

    time( &now );
    localtime_r( &now, &info );

    bool write_header = !( SPIFFS.exists( filename ) );

    fs::File file = SPIFFS.open( filename, FILE_APPEND );

    if (!file) {
        log_e("Can't open file: %s!", filename );
        return;
    }

    if ( write_header ) {
        file.println("Date\tTime\tFirmware\tUptime_ms\tBatt_V\tBatt_mAh\tCharge_C\tDischarge_C\tBatt_%\tBatt_c_%\tCharging_mA\tDischarging_mA\tAXP_Temp_degC" );
    }

    if ( !file.print( &info, "%F%t%T%t" ) ) {
        log_e("Failed to append to event log file: %s!", filename );
    }
    else {
        AXP20X_Class *power = TTGOClass::getWatch()->power;

        char log_line[256]="";
        snprintf( log_line, sizeof( log_line ), "%s\t%lu\t%0.3f\t%0.1f\t%u\t%u\t%d\t%0.1f\t%0.1f\t%0.1f\t%0.1f",
                                                __FIRMWARE__,
                                                millis() / 1000, 
                                                power->getBattVoltage() / 1000.0,
                                                pmu_get_coulumb_data(),
                                                power->getBattChargeCoulomb(),
                                                power->getBattDischargeCoulomb(),
                                                power->getBattPercentage(),
                                                ( power->getCoulombData() / pmu_config.designed_battery_cap ) * 100,
                                                power->getBattChargeCurrent(),
                                                power->getBattDischargeCurrent(),
                                                power->getTemp()
        );

        log_i("Firmware\tUptime\tBatt_V\tBat_mAh\tCharge\tDischar\tBatt_%\tBatt_%\tCharg\tDischar\tAXP_degC" );
        log_i("%s", log_line );

        if ( !file.println( log_line ) ) {
            log_e("Failed to append to event log file: %s!", filename );
        }
    }
    file.close();
}