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