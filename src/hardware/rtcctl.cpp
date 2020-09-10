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

volatile bool DRAM_ATTR rtc_irq_flag = false;
portMUX_TYPE RTC_IRQ_Mux = portMUX_INITIALIZER_UNLOCKED;

static bool alarm_enable = false;

void rtcctl_send_event_cb( EventBits_t event );
void rtcctl_powermgm_event_cb( EventBits_t event );

rtcctl_event_cb_t *rtcctl_event_cb_table = NULL;
uint32_t rtcctl_event_cb_entrys = 0;

static void IRAM_ATTR rtcctl_irq( void );

void rtcctl_setup( void ) {
    pinMode( RTC_INT, INPUT_PULLUP);
    attachInterrupt( RTC_INT, &rtcctl_irq, FALLING );
    rtcctl_disable_alarm();
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, rtcctl_powermgm_event_cb, "rtcctl" );
}

void rtcctl_powermgm_event_cb( EventBits_t event ) {
    switch( event ) {
        case POWERMGM_STANDBY:          break;
        case POWERMGM_WAKEUP:           break;
        case POWERMGM_SILENCE_WAKEUP:   break;
    }
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
        portENTER_CRITICAL_ISR(&RTC_IRQ_Mux);
        bool temp_rtc_irq_flag = rtc_irq_flag;
        rtc_irq_flag = false;
        portEXIT_CRITICAL_ISR(&RTC_IRQ_Mux);
        if ( temp_rtc_irq_flag ) {
            rtcctl_send_event_cb( RTCCTL_ALARM );
        }
    }
}

void rtcctl_register_cb( EventBits_t event, RTCCTL_CALLBACK_FUNC rtcctl_event_cb, const char *id ) {
    rtcctl_event_cb_entrys++;

    if ( rtcctl_event_cb_table == NULL ) {
        rtcctl_event_cb_table = ( rtcctl_event_cb_t * )ps_malloc( sizeof( rtcctl_event_cb_t ) * rtcctl_event_cb_entrys );
        if ( rtcctl_event_cb_table == NULL ) {
            log_e("rtc_event_cb_table malloc faild");
            while(true);
        }
    }
    else {
        rtcctl_event_cb_t *new_rtcctl_event_cb_table = NULL;

        new_rtcctl_event_cb_table = ( rtcctl_event_cb_t * )ps_realloc( rtcctl_event_cb_table, sizeof( rtcctl_event_cb_t ) * rtcctl_event_cb_entrys );
        if ( new_rtcctl_event_cb_table == NULL ) {
            log_e("rtc_event_cb_table realloc faild");
            while(true);
        }
        rtcctl_event_cb_table = new_rtcctl_event_cb_table;
    }

    rtcctl_event_cb_table[ rtcctl_event_cb_entrys - 1 ].event = event;
    rtcctl_event_cb_table[ rtcctl_event_cb_entrys - 1 ].event_cb = rtcctl_event_cb;
    rtcctl_event_cb_table[ rtcctl_event_cb_entrys - 1 ].id = id;
    log_i("register rtc_event_cb success (%p:%s)", rtcctl_event_cb_table[ rtcctl_event_cb_entrys - 1 ].event_cb, rtcctl_event_cb_table[ rtcctl_event_cb_entrys - 1 ].id );
}

void rtcctl_send_event_cb( EventBits_t event ) {
    if ( rtcctl_event_cb_entrys == 0 ) {
      return;
    }
      
    for ( int entry = 0 ; entry < rtcctl_event_cb_entrys ; entry++ ) {
        yield();
        if ( event & rtcctl_event_cb_table[ entry ].event ) {
            log_i("call rtc_event_cb (%p:04x:%s)", rtcctl_event_cb_table[ entry ].event_cb, event, rtcctl_event_cb_table[ entry ].id );
            rtcctl_event_cb_table[ entry ].event_cb( event );
        }
    }
}

void rtcctl_set_alarm( uint8_t hour, uint8_t minute ){
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->rtc->setAlarm( hour, minute, PCF8563_NO_ALARM, PCF8563_NO_ALARM );
    rtcctl_send_event_cb( RTCCTL_ALARM_SET );
}

void rtcctl_enable_alarm( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->rtc->enableAlarm();
    alarm_enable = true;
    rtcctl_send_event_cb( RTCCTL_ALARM_ENABLE );
}

void rtcctl_disable_alarm( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->rtc->disableAlarm();
    alarm_enable = false;
    rtcctl_send_event_cb( RTCCTL_ALARM_DISABLE );
}

bool rtcctl_get_alarmstate( void ) {
    return( alarm_enable );
}

bool rtcctl_is_time( uint8_t hour, uint8_t minute ){
    TTGOClass *ttgo = TTGOClass::getWatch();
    RTC_Date date_time = ttgo->rtc->getDateTime();
    return hour == date_time.hour && minute == date_time.minute;
}