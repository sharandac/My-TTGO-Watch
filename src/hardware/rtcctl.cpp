/****************************************************************************
 *   Copyright  2020  Jakub Vesely
 *   Email: jakub_vesely@seznam.cz
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

#include "rtcctl.h"
#include "powermgm.h"
#include "callback.h"

volatile bool DRAM_ATTR rtc_irq_flag = false;
portMUX_TYPE DRAM_ATTR RTC_IRQ_Mux = portMUX_INITIALIZER_UNLOCKED;
static void IRAM_ATTR rtcctl_irq( void );

static bool alarm_enabled = false;
static int alarm_hour = 0;
static int alarm_minute = 0;

bool rtcctl_send_event_cb( EventBits_t event );
bool rtcctl_powermgm_event_cb( EventBits_t event, void *arg );
bool rtcctl_powermgm_loop_cb( EventBits_t event, void *arg );

callback_t *rtcctl_callback = NULL;

void rtcctl_setup( void ) {

    pinMode( RTC_INT, INPUT_PULLUP);
    attachInterrupt( RTC_INT, &rtcctl_irq, FALLING );

    //set values to be aligned with default variable values
    if (alarm_enabled){
        rtcctl_enable_alarm();
    }
    else{
        rtcctl_disable_alarm();
    }
    rtcctl_set_alarm_term( alarm_hour, alarm_minute );

    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, rtcctl_powermgm_event_cb, "rtcctl" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP, rtcctl_powermgm_loop_cb, "rtcctl loop" );
}

bool rtcctl_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_STANDBY:          log_i("go standby");
                                        gpio_wakeup_enable( (gpio_num_t)RTC_INT, GPIO_INTR_LOW_LEVEL );
                                        esp_sleep_enable_gpio_wakeup ();
                                        break;
        case POWERMGM_WAKEUP:           log_i("go wakeup");
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   log_i("go silence wakeup");
                                        break;
    }
    return( true );
}

bool rtcctl_powermgm_loop_cb( EventBits_t event, void *arg ) {
    rtcctl_loop();
    return( true );
}

static void IRAM_ATTR rtcctl_irq( void ) {
    portENTER_CRITICAL_ISR(&RTC_IRQ_Mux);
    rtc_irq_flag = true;
    portEXIT_CRITICAL_ISR(&RTC_IRQ_Mux);
    powermgm_set_event( POWERMGM_RTC_ALARM );
}

void rtcctl_loop( void ) {
    // fire callback
    if ( !powermgm_get_event( POWERMGM_STANDBY ) ) {
        portENTER_CRITICAL( &RTC_IRQ_Mux );
        bool temp_rtc_irq_flag = rtc_irq_flag;
        rtc_irq_flag = false;
        portEXIT_CRITICAL( &RTC_IRQ_Mux );
        if ( temp_rtc_irq_flag ) {
            rtcctl_send_event_cb( RTCCTL_ALARM_OCCURRED );
        }
    }
}

bool rtcctl_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( rtcctl_callback == NULL ) {
        rtcctl_callback = callback_init( "rtctl" );
        if ( rtcctl_callback == NULL ) {
            log_e("rtcctl callback alloc failed");
            while(true);
        }
    }    
    return( callback_register( rtcctl_callback, event, callback_func, id ) );
}

bool rtcctl_send_event_cb( EventBits_t event ) {
    return( callback_send( rtcctl_callback, event, (void*)NULL ) );
}

void rtcctl_set_alarm_term( uint8_t hour, uint8_t minute ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    if (alarm_enabled){
        ttgo->rtc->disableAlarm();
    }
    alarm_hour = hour;
    alarm_minute = minute;
    ttgo->rtc->setAlarm( hour, minute, PCF8563_NO_ALARM, PCF8563_NO_ALARM );
    if (alarm_enabled){
        ttgo->rtc->enableAlarm();
    }
    rtcctl_send_event_cb( RTCCTL_ALARM_TERM_SET );
}

void rtcctl_enable_alarm( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->rtc->enableAlarm();
    alarm_enabled = true;
    rtcctl_send_event_cb( RTCCTL_ALARM_ENABLED );
}

void rtcctl_disable_alarm( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->rtc->disableAlarm();
    alarm_enabled = false;
    rtcctl_send_event_cb( RTCCTL_ALARM_DISABLED );
}

bool rtcctl_is_alarm_enabled( void ) {
    return alarm_enabled;
}

bool rtcctl_is_alarm_time(){
    TTGOClass *ttgo = TTGOClass::getWatch();
    RTC_Date date_time = ttgo->rtc->getDateTime();
    return alarm_hour == date_time.hour && alarm_minute == date_time.minute;
}

uint8_t rtcctl_get_alarm_hour(){
    return alarm_hour;
}

uint8_t rtcctl_get_alarm_minute(){
    return alarm_minute;
}
