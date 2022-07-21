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
#include "motor.h"
#include "powermgm.h"
#include "hardware/config/motorconfig.h"

#ifdef NATIVE_64BIT

#else
    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <TTGO.h>

        #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
            volatile int DRAM_ATTR motor_run_time_counter=0;
            hw_timer_t * timer = NULL;
            portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED;

            void IRAM_ATTR onTimer();
            void IRAM_ATTR onTimer() {
                /*
                * set critical section
                */
                portENTER_CRITICAL_ISR(&timerMux);
                /*
                * check if timer counter > zero
                */
                if ( motor_run_time_counter >0 ) {
                    /*
                    * decrement timer counter and enable motor
                    */
                    motor_run_time_counter--;       
                    digitalWrite(MOTOR_PIN, HIGH );
                }
                else {
                    /*
                    * disable motor
                    */
                    digitalWrite(MOTOR_PIN, LOW );              
                }
                /*
                * leave critical section
                */
                portEXIT_CRITICAL_ISR(&timerMux);
            }
        #elif defined( LILYGO_WATCH_2020_V2 )
            static Adafruit_DRV2605 *drv = NULL;
            #define DRV2605_ADDRESS 0x5A
        #endif
    #elif defined( LILYGO_WATCH_2021 )
        #include <twatch2021_config.h>  
        
        volatile int DRAM_ATTR motor_run_time_counter=0;
        hw_timer_t * timer = NULL;
        portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED;

        void IRAM_ATTR onTimer();
        void IRAM_ATTR onTimer() {
            /*
            * set critical section
            */
            portENTER_CRITICAL_ISR(&timerMux);
            /*
            * check if timer counter > zero
            */
            if ( motor_run_time_counter >0 ) {
                /*
                * decrement timer counter and enable motor
                */
                motor_run_time_counter--;       
                digitalWrite(MOTOR_PIN, HIGH );
            }
            else {
                /*
                * disable motor
                */
                digitalWrite(MOTOR_PIN, LOW );              
            }
            /*
            * leave critical section
            */
            portEXIT_CRITICAL_ISR(&timerMux);
        }
    #elif defined( WT32_SC01 )

    #else
        #warning "no hardware driver for motor/vibe"
    #endif
#endif

bool motor_init = false;
motor_config_t motor_config;

bool motor_powermgm_event_cb( EventBits_t event, void *arg );

void motor_setup( void ) {
    /*
     * check if motor already init
     */
    if ( motor_init == true ) {
        return;
    }
    /*
     * load config from json
     */
    motor_config.load();
    #ifdef NATIVE_64BIT

    #else
        #ifdef M5PAPER
        
        #elif defined( LILYGO_WATCH_2020_V2 )
            /**
             * check if an DRV2605 connected
             */
            Wire.beginTransmission( DRV2605_ADDRESS );
            uint8_t err = Wire.endTransmission();
            if ( err == 0 ) {
                log_d("Motor init: I2C device found at address %p", DRV2605_ADDRESS);
                TTGOClass * ttgo = TTGOClass::getWatch(); 
                ttgo->enableDrv2650( true );
                drv = ttgo->drv;
                drv->setMode( DRV2605_MODE_INTTRIG );
                /**
                 * default, internal trigger when sending GO command
                 */
                drv->selectLibrary( 1 );
                drv->setWaveform( 0, 75 ); //Transition Ramp Down Short Smooth 2 – 100 to 0%
                drv->setWaveform( 2, 0 );  //end of waveforms        
            }
            else {
                log_e("Motor init: I2C device not found, error %d", err);
                drv = NULL;    
            }     
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 ) || defined( LILYGO_WATCH_2021 ) 
            pinMode(MOTOR_PIN, OUTPUT);
            timer = timerBegin(0, 80, true);
            timerAttachInterrupt(timer, &onTimer, true);
            timerAlarmWrite(timer, 10000, true);
            timerAlarmEnable(timer);
        #elif defined( WT32_SC01 )

        #endif
    #endif
    /*
     * setup motor gpio, timer interrupt and interrupt function
     */
    motor_init = true;
    /*
     * register powermgm callback function
     */
    powermgm_register_cb( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP | POWERMGM_ENABLE_INTERRUPTS | POWERMGM_DISABLE_INTERRUPTS, &motor_powermgm_event_cb, "powermgm motor");
    /*
     * vibe for success
     */
    motor_vibe( 10 );
}

bool motor_powermgm_event_cb( EventBits_t event, void *arg ) {
    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )
        #elif defined( LILYGO_WATCH_2020_V2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 ) || defined( LILYGO_WATCH_2021 )
            switch( event ) {
                case POWERMGM_SILENCE_WAKEUP:   portENTER_CRITICAL(&timerMux);
                                                motor_run_time_counter = 0;
                                                digitalWrite(MOTOR_PIN, LOW );   
                                                portEXIT_CRITICAL(&timerMux);
                                                break;
                case POWERMGM_STANDBY:          portENTER_CRITICAL(&timerMux);
                                                motor_run_time_counter = 0;
                                                digitalWrite(MOTOR_PIN, LOW );
                                                portEXIT_CRITICAL(&timerMux);
                                                break;
                case POWERMGM_ENABLE_INTERRUPTS:
                                                timerAttachInterrupt(timer, &onTimer, true);
                                                break;
                case POWERMGM_DISABLE_INTERRUPTS:
                                                timerDetachInterrupt(timer);
                                                break;
            }
        #elif defined( WT32_SC01 )
        #endif
    #endif
    return( true );
}

void motor_vibe( int time, bool enforced ) {
    /*
     * check if motor already init
     */
    if ( motor_init == false ) {
        return;
    }
    /*
     * if motor disabled or forced?
     */
    if ( motor_get_vibe_config() || enforced ) {
    #ifdef NATIVE_64BIT

    #else
        #if defined( M5PAPER )

        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 ) || defined( LILYGO_WATCH_2021 )
            /*
            * set critical section
            */        
            portENTER_CRITICAL(&timerMux);
            motor_run_time_counter = time;
            /*
            * leave critical section
            */
            portEXIT_CRITICAL(&timerMux);
        #elif defined( LILYGO_WATCH_2020_V2 )
            if (drv!=NULL) {
                /**
                 * play the effect!
                 */
                drv->go();
            }
        #endif
    #endif
    }
}

bool motor_get_vibe_config( void ) {
    return( motor_config.vibe );
}

void motor_set_vibe_config( bool enable ) {
    motor_config.vibe = enable;
    motor_config.save();
}

void motor_save_config( void ) {
    motor_config.save();
}

void motor_read_config( void ) {
    motor_config.load();
}