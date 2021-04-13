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

#include "motor.h"
#include "powermgm.h"

#include "hardware/config/motorconfig.h"

#if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
    volatile int DRAM_ATTR motor_run_time_counter=0;
    hw_timer_t * timer = NULL;
    portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED;
    bool motor_powermgm_event_cb( EventBits_t event, void *arg );
#elif defined( LILYGO_WATCH_2020_V2 )
    static Adafruit_DRV2605 *drv = NULL;
    #define DRV2605_ADDRESS 0x5A
#endif

bool motor_init = false;
motor_config_t motor_config;

#if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
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
#endif 

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
    /*
     * setup motor gpio, timer interrupt and interrupt function
     */
    #if defined( LILYGO_WATCH_2020_V2 )
        Wire.beginTransmission(DRV2605_ADDRESS);
        uint8_t err = Wire.endTransmission();
        if (err == 0) {
            log_i("Motor init: I2C device found at address %p", DRV2605_ADDRESS);
            TTGOClass * ttgo = TTGOClass::getWatch(); 
            ttgo->enableDrv2650(true);
            drv = ttgo->drv;            
            drv->selectLibrary(1);
            drv->setMode(DRV2605_MODE_INTTRIG);            
        }
        else
        {
            log_e("Motor init: I2C device not found, error %d", err);
            drv = NULL;    
        }        
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
        pinMode(MOTOR_PIN, OUTPUT);
        timer = timerBegin(0, 80, true);
        timerAttachInterrupt(timer, &onTimer, true);
        timerAlarmWrite(timer, 10000, true);
        timerAlarmEnable(timer);
    #endif

    motor_init = true;
    /*
     * register powermgm callback function
     */
    #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
        owermgm_register_cb( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP | POWERMGM_ENABLE_INTERRUPTS | POWERMGM_DISABLE_INTERRUPTS, &motor_powermgm_event_cb, "powermgm motor");
    #endif
    /*
     * vibe for success
     */
    motor_vibe( 10 );
}

#if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
bool motor_powermgm_event_cb( EventBits_t event, void *arg ) {
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
    return( true );
}
#endif 

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
        #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
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
                // set the effect to play
                drv->setWaveform(0, 75);  // play effect
                drv->setWaveform(1, 0);       // end waveform
                // play the effect!
                drv->go();
            }
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