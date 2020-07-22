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
#include "motor.h"
#include "powermgm.h"

volatile int DRAM_ATTR motor_run_time_counter=0;
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

bool motor_init = false;

/*
 *
 */
void IRAM_ATTR onTimer() {
    portENTER_CRITICAL_ISR(&timerMux);
    if ( motor_run_time_counter >0 ) {
        motor_run_time_counter--;
        digitalWrite(GPIO_NUM_4, HIGH );
    }
    else {
        digitalWrite(GPIO_NUM_4, LOW );
    }
    portEXIT_CRITICAL_ISR(&timerMux);
}

/*
 *
 */
void motor_setup( void ) {
    if ( motor_init == true )
        return;

    pinMode(GPIO_NUM_4, OUTPUT);
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, 10000, true);
    timerAlarmEnable(timer);
    motor_init = true;

    motor_vibe( 10 );
}

/*
 *
 */
void motor_vibe( int time ) {
    if ( motor_init == false )
        return;

    portENTER_CRITICAL(&timerMux);
    motor_run_time_counter = time;
    portEXIT_CRITICAL(&timerMux);
}