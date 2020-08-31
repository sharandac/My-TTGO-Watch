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
#include "hardware/powermgm.h"

EventGroupHandle_t rtcctl_status = NULL;
portMUX_TYPE rtcctlMux = portMUX_INITIALIZER_UNLOCKED;

static bool alarm_enable = false;

static void IRAM_ATTR rtcctl_irq( void );
void rtcctl_send_event_cb( EventBits_t event );
void rtcctl_test_cb( EventBits_t event );
void rtcctl_set_event( EventBits_t bits );
void rtcctl_clear_event( EventBits_t bits );
bool rtcctl_get_event( EventBits_t bits );

rtcctl_event_cb_t *rtcctl_event_cb_table = NULL;
uint32_t rtcctl_event_cb_entrys = 0;

void rtcctl_setup( void ){
    rtcctl_status = xEventGroupCreate();

    pinMode( RTC_INT, INPUT_PULLUP);
    attachInterrupt( RTC_INT, &rtcctl_irq, FALLING );
    rtcctl_disable_alarm();
}

void rtcctl_loop( void ) {
    // fire callback
    if ( !powermgm_get_event( POWERMGM_STANDBY ) ) {
        if ( rtcctl_get_event( RTCCTL_ALARM ) ) {
            rtcctl_send_event_cb( RTCCTL_ALARM );
            rtcctl_clear_event( RTCCTL_ALARM );
        }
    }
}

static void IRAM_ATTR rtcctl_irq( void ) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    /*
     * setup an RTC event
     */
    xEventGroupSetBitsFromISR( rtcctl_status, RTCCTL_ALARM, &xHigherPriorityTaskWoken );
    if ( xHigherPriorityTaskWoken ) {
        portYIELD_FROM_ISR();
    }
    powermgm_set_event( POWERMGM_RTC_ALARM );
}

void rtcctl_register_cb( EventBits_t event, RTCCTL_CALLBACK_FUNC rtcctl_event_cb ) {
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
    log_i("register rtc_event_cb success (%p)", rtcctl_event_cb_table[ rtcctl_event_cb_entrys - 1 ].event_cb );
}

void rtcctl_send_event_cb( EventBits_t event ) {
    if ( rtcctl_event_cb_entrys == 0 ) {
      return;
    }
      
    for ( int entry = 0 ; entry < rtcctl_event_cb_entrys ; entry++ ) {
        yield();
        if ( event & rtcctl_event_cb_table[ entry ].event ) {
            log_i("call rtc_event_cb (%p)", rtcctl_event_cb_table[ entry ].event_cb );
            rtcctl_event_cb_table[ entry ].event_cb( event );
        }
    }
}

void rtcctl_set_event( EventBits_t bits ) {
    portENTER_CRITICAL(&rtcctlMux);
    xEventGroupSetBits( rtcctl_status, bits );
    portEXIT_CRITICAL(&rtcctlMux);
}

void rtcctl_clear_event( EventBits_t bits ) {
    portENTER_CRITICAL(&rtcctlMux);
    xEventGroupClearBits( rtcctl_status, bits );
    portEXIT_CRITICAL(&rtcctlMux);
}

bool rtcctl_get_event( EventBits_t bits ) {
    portENTER_CRITICAL(&rtcctlMux);
    EventBits_t temp = xEventGroupGetBits( rtcctl_status ) & bits;
    portEXIT_CRITICAL(&rtcctlMux);
    if ( temp )
        return( true );

    return( false );
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
