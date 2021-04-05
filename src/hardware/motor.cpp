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

volatile int DRAM_ATTR motor_run_time_counter=0;
hw_timer_t * timer = NULL;
portMUX_TYPE DRAM_ATTR timerMux = portMUX_INITIALIZER_UNLOCKED;

bool motor_init = false;

motor_config_t motor_config;

bool motor_powermgm_event_cb( EventBits_t event, void *arg );

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
        digitalWrite(GPIO_NUM_4, HIGH );
    }
    else {
        /*
         * disable motor
         */
        digitalWrite(GPIO_NUM_4, LOW );
    }
    /*
     * leave critical section
     */
    portEXIT_CRITICAL_ISR(&timerMux);
}

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
    pinMode(GPIO_NUM_4, OUTPUT);
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 10000, true);
    timerAlarmEnable(timer);
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
    switch( event ) {
        case POWERMGM_SILENCE_WAKEUP:   portENTER_CRITICAL(&timerMux);
                                        motor_run_time_counter = 0;
                                        digitalWrite( GPIO_NUM_4 , LOW );
                                        portEXIT_CRITICAL(&timerMux);
                                        break;
        case POWERMGM_STANDBY:          portENTER_CRITICAL(&timerMux);
                                        motor_run_time_counter = 0;
                                        digitalWrite( GPIO_NUM_4 , LOW );
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
        /*
         * set critical section
         */
        portENTER_CRITICAL(&timerMux);
        motor_run_time_counter = time;
        /*
         * leave critical section
         */
        portEXIT_CRITICAL(&timerMux);
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