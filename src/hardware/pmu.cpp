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
#include "pmu.h"
#include "powermgm.h"
#include "blectl.h"
#include "callback.h"
#include "utils/alloc.h"
#include "utils/filepath_convert.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"

    volatile bool pmu_irq_flag = false;
    static bool pmu_update = true;

    #define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#else
    #include <Arduino.h>

    #if defined( M5PAPER )
        #include <M5EPD.h>
        static uint64_t next_wakeup = 0;
    #elif defined( M5CORE2 )
        #include <M5Core2.h>
        static uint64_t next_wakeup = 0;
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include "TTGO.h"
    #elif defined( LILYGO_WATCH_2021 )
        #include <twatch2021_config.h>
    #elif defined( WT32_SC01 )

    #else
        #warning "no hardware driver for pmu"
    #endif
    #include <soc/rtc.h>

    static bool pmu_update = true;
    volatile bool DRAM_ATTR pmu_irq_flag = false;
    portMUX_TYPE DRAM_ATTR PMU_IRQ_Mux = portMUX_INITIALIZER_UNLOCKED;

    void IRAM_ATTR pmu_irq( void );
    /**
     * pmu IRQ
     */
    void IRAM_ATTR pmu_irq( void ) {
        portENTER_CRITICAL_ISR(&PMU_IRQ_Mux);
        pmu_irq_flag = true;
        portEXIT_CRITICAL_ISR(&PMU_IRQ_Mux);
        powermgm_resume_from_ISR();
    }
#endif

callback_t *pmu_callback = NULL;
pmu_config_t pmu_config;

static int32_t pmu_get_voltage2percent( float mV );
bool pmu_powermgm_event_cb( EventBits_t event, void *arg );
bool pmu_powermgm_loop_cb( EventBits_t event, void *arg );
bool pmu_blectl_event_cb( EventBits_t event, void *arg );
bool pmu_send_cb( EventBits_t event, void *arg );

void pmu_setup( void ) {
    /*
     * read config from SPIFF
     */
    pmu_config.load();
    /**
     * setup battery adc
     */
#ifdef NATIVE_64BIT

#else
    #if defined( M5PAPER )
        M5.BatteryADCBegin();
    #elif defined( M5CORE2 )
        M5.Axp.SetSpkEnable( true );
        M5.Axp.SetCHGCurrent( true );
        M5.Axp.EnableCoulombcounter();
        M5.Axp.SetAdcState( true );
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        TTGOClass *ttgo = TTGOClass::getWatch();
        /**
         * if ADC sampling rate != 200, init charging current, samplingrate and coulomcounter
         */
        log_i("init AXP202 pmu controller");
        if( ttgo->power->getAdcSamplingRate() != 200 ) {
            int failCounter = 0;
            log_i("init AXP charging settings and control to 200Hz, 300mA, Coulomcounter");

            if ( ttgo->power->setChargeControlCur( 300 ) != AXP_PASS ) {
                log_e("charge current set failed!");
                failCounter++;
            }

            if ( ttgo->power->EnableCoulombcounter() != AXP_PASS ) {
                log_e("enable coulumb counter failed!");
                failCounter++;
            }

            if ( ttgo->power->setAdcSamplingRate( AXP_ADC_SAMPLING_RATE_200HZ ) != AXP_PASS ) {
                log_e("adc sample set failed!");
                failCounter++;
            }

            if ( failCounter ) {
                log_e("AXP202 setup failed, shutdown");
                ttgo->power->shutdown();
            }
        }
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
        * enable coulumb counter and set target voltage for charging
        */
        if ( pmu_config.high_charging_target_voltage ) {
            log_d("set target voltage to 4.36V");
            if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_36V ) )
                log_e("target voltage 4.36V set failed!");
        }
        else {
            log_d("set target voltage to 4.2V");
            if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_2V ) )
                log_e("target voltage 4.2V set failed!");
        }
        /*
        * Turn off unused power
        */
        ttgo->power->setPowerOutPut( AXP202_EXTEN, AXP202_OFF );
        ttgo->power->setPowerOutPut( AXP202_DCDC2, AXP202_OFF );
        ttgo->power->setPowerOutPut( AXP202_LDO4, AXP202_OFF );
        /*
        * register IRQ function and GPIO pin
        */
        pinMode( AXP202_INT, INPUT );
        attachInterrupt( AXP202_INT, &pmu_irq, FALLING );
    #elif defined( LILYGO_WATCH_2021 )    
        pinMode( PWR_ON, OUTPUT );
        digitalWrite( PWR_ON, HIGH );
        pinMode( CHARGE, INPUT_PULLUP );
        pinMode( BAT_ADC, INPUT );
        attachInterrupt( CHARGE, &pmu_irq, CHANGE );
    #elif defined( WT32_SC01 )

    #endif
#endif
    /*
     * register all powermem callback functions
     */
    powermgm_register_cb_with_prio( POWERMGM_STANDBY , pmu_powermgm_event_cb, "powermgm pmu", CALL_CB_LAST );
    powermgm_register_cb_with_prio( POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP | POWERMGM_ENABLE_INTERRUPTS | POWERMGM_DISABLE_INTERRUPTS , pmu_powermgm_event_cb, "powermgm pmu", CALL_CB_FIRST );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP , pmu_powermgm_loop_cb, "powermgm pmu loop" );
    // powermgm_set_resume_interval( pmu_config.powermgm_resume_interval );
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
    bool retval = false;
    
    switch( event ) {
        case POWERMGM_STANDBY:          pmu_standby();
                                        retval = true;
                                        break;
        case POWERMGM_WAKEUP:           pmu_wakeup();
                                        retval = true;
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   pmu_wakeup();
                                        retval = true;
                                        break;
        #ifdef NATIVE_64BIT

        #else
            #if defined( M5PAPER )
                case POWERMGM_ENABLE_INTERRUPTS:
                                                attachInterrupt( M5EPD_KEY_PUSH_PIN, &pmu_irq, FALLING );
                                                retval = true;
                                                break;
                case POWERMGM_DISABLE_INTERRUPTS:
                                                detachInterrupt( M5EPD_KEY_PUSH_PIN );
                                                retval = true;
                                                break;
            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                case POWERMGM_ENABLE_INTERRUPTS:
                                                attachInterrupt( AXP202_INT, &pmu_irq, FALLING );
                                                retval = true;
                                                break;
                case POWERMGM_DISABLE_INTERRUPTS:
                                                detachInterrupt( AXP202_INT );
                                                retval = true;
                                                break;
            #endif
        #endif
    }
    return( retval );
}

bool pmu_blectl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_CONNECT:
            pmu_update = true;
    }
    return( true );
}

void pmu_loop( void ) {
    static uint64_t nextmillis = 0;
    static int32_t percent = pmu_get_battery_percent();
    int32_t tmp_percent = 0;

#ifdef NATIVE_64BIT
    static bool plug = false;
    static bool charging = false;
    static bool battery = false; 
#else
    /*
     * handle IRQ event
     */
    portENTER_CRITICAL(&PMU_IRQ_Mux);
    bool temp_pmu_irq_flag = pmu_irq_flag;
    pmu_irq_flag = false;
    portEXIT_CRITICAL(&PMU_IRQ_Mux);
        
    #if defined( M5PAPER )
        static bool plug = false;
        static bool charging = false;
        static bool battery = false;   

        if ( next_wakeup != 0 ) {
            if ( next_wakeup < millis() ) {
                next_wakeup = 0;
                powermgm_set_event( POWERMGM_WAKEUP_REQUEST ); 
            }
        }

        if ( temp_pmu_irq_flag ) {
            log_e("hello Mc Fly, witch PMU irq on M5Paper?");
        }

    #elif defined( M5CORE2 )
        static bool plug = false;
        static bool charging = false;
        static bool battery = false;   

        charging = M5.Axp.isCharging();
        plug = M5.Axp.isACIN();
        battery = M5.Axp.isVBUS();

        if ( next_wakeup != 0 ) {
            if ( next_wakeup < millis() ) {
                next_wakeup = 0;
                powermgm_set_event( POWERMGM_SILENCE_WAKEUP_REQUEST ); 
            }
        }
        
        if ( powermgm_get_event( POWERMGM_STANDBY ) ) {
            if ( ( millis() / 1000 ) % 5 )
                M5.Axp.SetLed( 0 );
            else
                M5.Axp.SetLed( 1 );
        }

        if ( temp_pmu_irq_flag ) {
            log_e("hello Mc Fly, witch AXP irq on M5Core2?");
        }
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        TTGOClass *ttgo = TTGOClass::getWatch();

        static bool plug = ttgo->power->isVBUSPlug();
        static bool charging = ttgo->power->isChargeing();
        static bool battery = ttgo->power->isBatteryConnect();

        if ( temp_pmu_irq_flag ) {        
            ttgo->power->readIRQ();
            if ( ttgo->power->isVbusPlugInIRQ() ) {
                /*
                * set an wakeup request and
                * delete old charging logfile when plug in and
                * set variable plug to true
                */
                log_d("AXP202: VBusPlugInIRQ");
                powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                if ( pmu_config.high_charging_target_voltage ) {
                    log_w("set target voltage to 4.36V for high target charging");
                    if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_36V ) )
                        log_e("target voltage 4.36V set failed!");
                }
                else {
                    log_d("set target voltage to 4.20V for charging");
                    if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_2V ) )
                        log_e("target voltage 4.20V set failed!");
                }
                plug = true;
            }
            if ( ttgo->power->isVbusRemoveIRQ() ) {
                /*
                * set an wakeup request and
                * remove old discharging log file when unplug
                * set variable plug and chargingto false
                */
                log_d("AXP202: VBusRemoteInIRQ");
                powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                charging = false;
                plug = false;
            }
            if ( ttgo->power->isChargingIRQ() ) {
                /*
                * set an wakeup request and
                * set variable charging to true
                */
                log_d("AXP202: ChargingIRQ");
                powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                charging = true;
            }
            if ( ttgo->power->isChargingDoneIRQ() ) {
                /*
                * set an wakeup request and
                * set variable charging to false
                */
                log_d("AXP202: ChargingDoneIRQ");
                powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                log_w("set target voltage to 4.20V after high target charging");
                if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_2V ) )
                    log_e("target voltage 4.20V set failed!");
                charging = false;
            }
            if ( ttgo->power->isBattPlugInIRQ() ) {
                /*
                * set an wakeup request and
                * set variable charging to false
                */
                log_d("AXP202: BattPlugInIRQ");
                powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                battery = true;
            }
            if ( ttgo->power->isBattRemoveIRQ() ) {
                /*
                * set an wakeup request and
                * set variable charging to false
                */
                log_d("AXP202: BattRemoveIRQ");
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
                ttgo->power->clearIRQ();
                log_d("AXP202: PEKShortPressIRQ");
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
                log_d("AXP202: PEKLongtPressIRQ");
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
                log_d("AXP202: TimerTimeoutIRQ");
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
    #elif  defined( LILYGO_WATCH_2021 ) 
        static bool plug = false;
        static bool charging = pmu_is_charging();
        static bool battery = percent > 0 ? true : false;

        if ( temp_pmu_irq_flag ) {
            if ( charging != pmu_is_charging() ) {
                charging = pmu_is_charging();
                pmu_update = true;
            }
            battery = percent > 0 ? true : false;
            log_e("hello Mc Fly, witch PMU irq on T-Watch2021?");
        }
    #elif defined( WT32_SC01 )
        static bool plug = false;
        static bool charging = false;
        static bool battery = percent > 0 ? true : false;
    #endif
#endif
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
        /**
         * if battery voltage read failed, force read again
         */
        if( pmu_get_battery_voltage() == 0.0 && powermgm_get_event( POWERMGM_WAKEUP ) )
            nextmillis = millis();
        /*
         * only update if an change is detected
         */
        tmp_percent = pmu_get_battery_percent();
        if ( tmp_percent != percent ) {
            pmu_update = true;
            percent = tmp_percent;
        }
        pmu_battery_calibration_loop( false, false );
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
        log_d("battery state: %d%%, %s, %s, %s (0x%04x), voltage=%.2f", percent, plug ? "connected" : "unconnected", charging ? "charging" : "discharge", battery ? "battery ok" : "no battery", msg, pmu_get_battery_voltage() );
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
    /**
     * disable all power
     */
#ifdef NATIVE_64BIT

#else
    #if defined( M5PAPER )
        M5.disableEPDPower();
        M5.disableEXTPower();
        M5.disableMainPower();
    #elif defined( M5CORE2 )
        M5.shutdown();
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        TTGOClass *ttgo = TTGOClass::getWatch();
        ttgo->power->shutdown();
    #endif
#endif
}

void pmu_standby( void ) {
#ifdef NATIVE_64BIT

#else
    #if defined( M5PAPER )
        /**
         * disable external power
         */
        log_d("disable epd/ext power");
        while( M5.EPD.CheckAFSR() ){};
        M5.disableEXTPower();
        M5.disableEPDPower();
        /**
         * set wakeup timer
         */
        if ( pmu_get_silence_wakeup() ) {
            next_wakeup = millis() + pmu_config.silence_wakeup_interval * 60 * 1000;
            esp_sleep_enable_timer_wakeup( pmu_config.silence_wakeup_interval * 60 * 1000000 );
            log_d("enable wakeup timer (%d sec)", pmu_config.silence_wakeup_interval * 60 );
        }
    #elif defined( M5CORE2 )
        M5.Axp.PrepareToSleep();
        /**
         * set wakeup timer
         */
        if ( pmu_get_silence_wakeup() ) {
            next_wakeup = millis() + pmu_config.silence_wakeup_interval * 60 * 1000;
            esp_sleep_enable_timer_wakeup( pmu_config.silence_wakeup_interval * 60 * 1000000 );
            log_d("enable wakeup timer (%d sec)", pmu_config.silence_wakeup_interval * 60 );
        }
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
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
        #if defined( LILYGO_WATCH_2020_V2 ) 
            ttgo->power->setPowerOutPut(AXP202_LDO3, false);
        #else
            /*
                * disable LD02, sound?
                */
            ttgo->power->setPowerOutPut( AXP202_LDO2, AXP202_OFF );
        #endif
        /*
         * enable GPIO in lightsleep for wakeup
         */
        gpio_wakeup_enable( (gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL );
        esp_sleep_enable_gpio_wakeup ();
    #elif  defined( LILYGO_WATCH_2021 )
        digitalWrite( PWR_ON, LOW );
    #endif
#endif
}

void pmu_wakeup( void ) {
#ifdef NATIVE_64BIT

#else
    #if defined( M5PAPER )
        /**
         * enable external power
         */
        log_d("enable epd/ext power");
        M5.enableEPDPower();
        M5.enableEXTPower();
        delay(50);
        M5.EPD.SetRotation( 90 );
//        M5.EPD.Clear( true );
        /**
         * hard shut down if battery voltage under 3.5V to
         * prevent RTC time lost
         */
        if ( pmu_get_battery_voltage() < 3.2f ) {
            M5.shutdown();
        }
    #elif defined( M5CORE2 )
        M5.Axp.RestoreFromLightSleep();
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
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
        #if defined( LILYGO_WATCH_2020_V2 ) 
            ttgo->power->setLDO3Voltage(3300);
            ttgo->power->setPowerOutPut(AXP202_LDO3, true);
        #else
            /*
             * disable LD02, sound?
             */
            ttgo->power->setPowerOutPut( AXP202_LDO2, AXP202_ON );    
        #endif
    #elif  defined( LILYGO_WATCH_2021 )
        digitalWrite( PWR_ON, HIGH );
    #endif
#endif
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
#ifdef NATIVE_64BIT

#else
    #if defined( M5PAPER )

    #elif defined( M5CORE2 )

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        TTGOClass *ttgo = TTGOClass::getWatch();

        pmu_config.high_charging_target_voltage = value;

        if ( pmu_config.high_charging_target_voltage ) {
            log_d("set target voltage to 4.36V");
            if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_36V ) )
                log_e("target voltage 4.36V set failed!");
        }
        else {
            log_d("set target voltage to 4.2V");
            if ( ttgo->power->setChargingTargetVoltage( AXP202_TARGET_VOL_4_2V ) )
                log_e("target voltage 4.2V set failed!");
        }
    #endif
#endif
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
    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )

        #elif defined( M5CORE2 )
        
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            
            ttgo->power->setDCDC3Voltage( NORMALVOLTAGE + 100 );
            log_d("set %dmV voltage", NORMALVOLTAGE );
        #endif

        vTaskDelay(250);
    #endif
}

int32_t pmu_get_battery_percent( void ) {
    static int32_t percent = 23;
    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
            float mV = pmu_get_battery_voltage();
            int32_t tmp_percent = pmu_get_voltage2percent( mV );
            if( tmp_percent )
                percent = tmp_percent;
                
        #elif defined( M5CORE2 )
            if ( pmu_get_calculated_percent() ) {
                percent = ( pmu_get_coulumb_data() / pmu_config.designed_battery_cap ) * 100;
            }
            else {
                float mV = pmu_get_battery_voltage();
                int32_t tmp_percent = pmu_get_voltage2percent( mV );
                if( tmp_percent )
                    percent = tmp_percent;
            }
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();

            if ( ttgo->power->getBattChargeCoulomb() < ttgo->power->getBattDischargeCoulomb() || ttgo->power->getBattVoltage() < 3200 ) {
                ttgo->power->ClearCoulombcounter();
            }

            if ( pmu_get_calculated_percent() ) {
                percent = ( ttgo->power->getCoulombData() / pmu_config.designed_battery_cap ) * 100;
            }
            else {
                percent = ttgo->power->getBattPercentage();
            }
        #elif defined( LILYGO_WATCH_2021 )
            float mV = pmu_get_battery_voltage();
            if( pmu_is_charging() )
                mV = mV - pmu_config.battery_chargingoffset;
            int32_t tmp_percent = pmu_get_voltage2percent( mV );
            if( tmp_percent )
                percent = tmp_percent;
        #elif defined( WT32_SC01 )
            percent = 100;
        #endif
    #endif
    return( percent );
}

static int32_t pmu_get_voltage2percent( float mV ) {
    int32_t percent;
    /**
     * mV to percent main conversation table
     *                             0%   10%    20%   30%  40%    50%   60%   70%   80%   90%  100%
     */
    const float QcmMain[] =     { 3000, 3490, 3680, 3745, 3780, 3810, 3845, 3890, 3950, 4050, 4220 };
    size_t size = ( sizeof( QcmMain ) / sizeof( float ) ) - 1;
    float Qcm[ size ];
    /**
     * calc the table scale factor
     */
    if( pmu_config.battery_voltage_highest < pmu_config.battery_voltage_lowest ) {
        pmu_config.battery_voltage_highest = QcmMain[ size ];
        pmu_config.battery_voltage_lowest = QcmMain[ 0 ];
        log_w("lowest and highest battery voltage not valid, reset");
    }
    float scale = ( pmu_config.battery_voltage_highest - pmu_config.battery_voltage_lowest ) / ( QcmMain[ size ] - QcmMain[ 0 ] );
    /**
     * scale the table into a new table
     */
    for( int i = size ; i >= 0; i-- ) {
        Qcm[ i ] = ( QcmMain[ i ] - QcmMain[ 0 ] ) * scale + pmu_config.battery_voltage_lowest;
    }

    int i = size;
    while ( mV <= Qcm[ i ] ) {
        if (i == 0)
            break;
        i--;
    }

    float vol_section = ( Qcm[ i + 1 ] - Qcm[ i ] ) / ( 100.0 / size );
    float div = i * ( 100.0 / size );
    percent = constrain( div + ( ( mV - Qcm[ i ] ) / vol_section ), 0.0, 100.0 );
    log_i("voltage: %.0fmV, percent: %d%%", mV, percent );
    return( percent );
}

float pmu_get_battery_voltage( void ) {
    float voltage = 3.5f;

    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
            voltage = M5.getBatteryVoltage();
        #elif defined( M5CORE2 )
            voltage = M5.Axp.GetBatVoltage() * 1000.0;
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            voltage = ttgo->power->getBattVoltage();
        #elif defined( LILYGO_WATCH_2021 )
            uint16_t battery = 0;
            uint16_t count = 10;
            /**
             * dummy read
             */
            analogRead( BAT_ADC );
            /**
             * collect count measurements
             */
            for( int i = 0 ; i < count ; i++ )
                battery += analogRead( BAT_ADC );
            battery /= count;
            /**
             * calc voltage
             */
            voltage = ( ( battery * 3300 * 2 ) / 4096 ) + 200;
        #elif defined( WT32_SC01 )
            voltage = 3700.0;
        #endif
    #endif
    return( voltage );
}

calibration_data_t *pmu_battery_calibration_loop( bool start_calibration, bool store ) {
    calibration_data_t *retval = NULL;
    static calibration_data_t calibration_data;

    if( start_calibration ) {
        /**
         * set min/max start values in mV
         */
        calibration_data.maxVoltage = pmu_config.battery_voltage_highest;
        calibration_data.minVoltage = pmu_config.battery_voltage_lowest;
        calibration_data.maxVoltageCharge = 0.0;
        /**
         * set calibration time
         */
        calibration_data.nextmillis = millis() + 60 * 1000;
        calibration_data.run = false;
        calibration_data.store = store;
        retval = &calibration_data;
    }
    else if( calibration_data.nextmillis != 0 ) {
        /**
         * check if next data is coming AND we are not in standby
         */
        if( calibration_data.nextmillis < millis() && powermgm_get_event( POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP ) ) {
            /**
             * set next watchpoint
             */
            calibration_data.nextmillis = millis() + 60 * 1000;
            /**
             * collect pmu data
             */
            calibration_data.batteryVoltage = pmu_get_battery_voltage();
            calibration_data.charging = pmu_is_charging();
            calibration_data.VBUS = pmu_is_charging() || pmu_is_vbus_plug();
            /**
             * only measure if not charging and no VBUS connected
             */
            if( !calibration_data.charging && !calibration_data.VBUS ) {
                if( !calibration_data.run ) {
                    calibration_data.run = true;
                    calibration_data.maxVoltage = 0;
                    calibration_data.minVoltage = 10000;
                    pmu_send_cb( PMUCTL_CALIBRATION_START, (void*)&calibration_data );
                }

                if( calibration_data.batteryVoltage < calibration_data.minVoltage ) {
                    calibration_data.minVoltage = calibration_data.batteryVoltage;
                }
                if( calibration_data.batteryVoltage > calibration_data.maxVoltage ) {
                    calibration_data.maxVoltage = calibration_data.batteryVoltage;
                }
                calibration_data.chargingVoltageOffset = calibration_data.maxVoltageCharge - calibration_data.maxVoltage;
                /**
                 * store new min/max while calibration if enabled
                 */
                if( calibration_data.store ) {
                    pmu_config.battery_voltage_lowest = calibration_data.minVoltage;                
                    pmu_config.battery_voltage_highest = calibration_data.maxVoltage;
                    pmu_config.battery_chargingoffset = calibration_data.chargingVoltageOffset;
                    pmu_config.save();
                }
            }
            else {
                if( calibration_data.batteryVoltage > calibration_data.maxVoltageCharge )
                    calibration_data.maxVoltageCharge = calibration_data.batteryVoltage;
            }
            pmu_send_cb( PMUCTL_CALIBRATION_UPDATE, (void*)&calibration_data );
            retval = &calibration_data;
        }
    }
    else {
        calibration_data.maxVoltage = pmu_config.battery_voltage_highest;
        calibration_data.minVoltage = pmu_config.battery_voltage_lowest;
        calibration_data.maxVoltageCharge = pmu_config.battery_voltage_highest + pmu_config.battery_chargingoffset;
        calibration_data.chargingVoltageOffset = pmu_config.battery_chargingoffset;
        calibration_data.batteryVoltage = pmu_get_battery_voltage();
        calibration_data.charging = pmu_is_charging();
        calibration_data.VBUS = pmu_is_charging() || pmu_is_vbus_plug();
        pmu_send_cb( PMUCTL_CALIBRATION_UPDATE, (void*)&calibration_data );
        retval = &calibration_data;
    }

    return( retval );
}

float pmu_get_battery_charge_current( void ) {
    float current = 0.0f;

    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
            current = M5.Axp.GetBatChargeCurrent();
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            current = ttgo->power->getBattChargeCurrent();
        #elif defined( LILYGO_WATCH_2021 )
        #elif defined( WT32_SC01 )
        #endif
    #endif

    return( current );
}

float pmu_get_battery_discharge_current( void ) {
    float current = 0.0f;

    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
            current = M5.Axp.GetBatCurrent();
            if( current < 0.0f )
                current = current * -1.0f;
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            current = ttgo->power->getBattDischargeCurrent();
        #elif defined( LILYGO_WATCH_2021 )
        #elif defined( WT32_SC01 )
        #endif
    #endif

    return( current );
}

float pmu_get_vbus_voltage( void ) {
    float voltage = 0.0f;

    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
            voltage = 0.0;
            while( voltage == 0.0 ) {
                voltage = M5.Axp.GetVBusVoltage() * 1000.0;
            }
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            voltage = ttgo->power->getVbusVoltage();
        #elif defined( LILYGO_WATCH_2021 )
        #elif defined( WT32_SC01 )
            voltage = 5.0f;
        #endif
    #endif

    return( voltage );
}

float pmu_get_coulumb_data( void ) {
    float coulumb_data = 0.0f;

    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
            coulumb_data = M5.Axp.GetCoulombData();
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            coulumb_data = ttgo->power->getCoulombData();
        #elif defined( LILYGO_WATCH_2021 )
        #elif defined( WT32_SC01 )
        #endif
    #endif

    return( coulumb_data );
}

bool pmu_is_charging( void ) {
    bool charging = false;

    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
            charging = M5.Axp.isCharging();
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            charging = ttgo->power->isChargeing();
        #elif defined( LILYGO_WATCH_2021 )
            charging = digitalRead( CHARGE ) ? false : true;
        #elif defined( WT32_SC01 )
            charging = true;
        #endif
    #endif

    return( charging );
}

bool pmu_is_vbus_plug( void ) {
    bool plug = false;

    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
            plug = M5.Axp.isACIN();
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            plug = ttgo->power->isVBUSPlug();
        #elif defined( LILYGO_WATCH_2021 )
            if( pmu_get_battery_voltage() > 4300 )
                plug = true;
            else
                plug = false;
        #elif defined( WT32_SC01 )
            plug = true;
        #endif
    #endif

    return( plug );
}
