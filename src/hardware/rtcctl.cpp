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
#include <SPIFFS.h>
#include <time.h>

#include "rtcctl.h"
#include "powermgm.h"
#include "callback.h"
#include "timesync.h"

#include "hardware/powermgm.h"
#include "hardware/json_psram_allocator.h"

#define VERSION_KEY "version"
#define ENABLED_KEY "enabled"
#define HOUR_KEY "hour"
#define MINUTE_KEY "minute"
#define WEEK_DAYS_KEY "week_days" 

static rtcctl_alarm_t alarm_data = {
    .enabled = false,
    .hour = 0,
    .minute = 0,
    .week_days = { false, false, false, false, false, false, false }
}; 
static time_t alarm_time = 0;

volatile bool DRAM_ATTR rtc_irq_flag = false;
portMUX_TYPE DRAM_ATTR RTC_IRQ_Mux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR rtcctl_irq( void );

bool rtcctl_powermgm_event_cb( EventBits_t event, void *arg );
bool rtcctl_powermgm_loop_cb( EventBits_t event, void *arg );
bool rtcctl_timesync_event_cb( EventBits_t event, void *arg );
bool rtcctl_send_event_cb( EventBits_t event );
void rtcctl_load_data( void );
void rtcctl_store_data( void );

callback_t *rtcctl_callback = NULL;

void rtcctl_setup( void ) {

    pinMode( RTC_INT, INPUT_PULLUP);
    attachInterrupt( RTC_INT, &rtcctl_irq, FALLING );

    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, rtcctl_powermgm_event_cb, "rtcctl" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP, rtcctl_powermgm_loop_cb, "rtcctl loop" );
    timesync_register_cb( TIME_SYNC_OK, rtcctl_timesync_event_cb, "rtcctl timesync" );

    rtcctl_load_data();
}

bool rtcctl_send_event_cb( EventBits_t event ) {
    return( callback_send( rtcctl_callback, event, (void*)NULL ) );
}

bool is_any_day_enabled( void ) {
    for (int index = 0; index < DAYS_IN_WEEK; ++index){
        if (alarm_data.week_days[index])
            return true; 
    }
    return false;
}

time_t find_next_alarm_day( int day_of_week, time_t now ) {
    //it is expected that test if any day is enabled has been performed
    
    time_t ret_val = now;
    int wday_index = day_of_week;
    do {
        ret_val += 60 * 60 * 24; //number of seconds in day
        if (++wday_index == DAYS_IN_WEEK){
            wday_index = 0;
        } 
        if (alarm_data.week_days[wday_index]){
            return ret_val;
        }        
    } while (wday_index != day_of_week);
    
    return ret_val; //the same day of next week
}

void set_next_alarm( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    if ( !is_any_day_enabled() ) {
        ttgo->rtc->setAlarm( PCF8563_NO_ALARM, PCF8563_NO_ALARM, PCF8563_NO_ALARM, PCF8563_NO_ALARM );    
        rtcctl_send_event_cb( RTCCTL_ALARM_TERM_SET );
        return;
    } 

    //trc and system must be synchronized, it is important when alarm has been raised and we want to set next concurency
    //if the synchronisation is not done the time can be set to now again
    //ttgo->rtc->syncToSystem();

    //FIXME: for now would be better to synchronize to rtc because time zone setting is not finished yet (#124).
    //Unfortunately, each synchToRtc without a NTP synchronization inacurate internal clock
    //This issue is complex and should be solbed when timezone setting will be finished .
    // timesyncToRTC();

    time_t now;
    time( &now );
    alarm_time = now;
    struct tm alarm_tm;

    // get local time and set alarm time
    localtime_r(&alarm_time, &alarm_tm);
    log_i("local time: %02d:%02d day: %d", alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_mday );
    alarm_tm.tm_hour = alarm_data.hour;
    alarm_tm.tm_min = alarm_data.minute;
    alarm_time = mktime( &alarm_tm );

    if ( !alarm_data.week_days[ alarm_tm.tm_wday ] || alarm_time <= now ) {
        alarm_time = find_next_alarm_day( alarm_tm.tm_wday, alarm_time );
        localtime_r( &alarm_time, &alarm_tm );
    }

    // convert local alarm time into GMT0 alarm time, it is necessary sine rtc store time in GMT0
    log_i("next local alarm time: %02d:%02d day: %d", alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_mday );
    gmtime_r( &alarm_time, &alarm_tm );
    log_i("next GMT0 alarm time: %02d:%02d day %d", alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_mday );

    // it is better define alarm by day in month rather than weekday. This way will be work-around an error in pcf8563 source and will avoid eaising alarm when there is only one alarm in the week (today) and alarm time is set to now
    ttgo->rtc->setAlarm( alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_mday, PCF8563_NO_ALARM );
    rtcctl_send_event_cb( RTCCTL_ALARM_TERM_SET );
}

void rtcctl_set_next_alarm( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    if (alarm_data.enabled){
        ttgo->rtc->disableAlarm();
    }

    set_next_alarm();
    
    if (alarm_data.enabled){
        ttgo->rtc->enableAlarm();
    }
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

bool rtcctl_timesync_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case TIME_SYNC_OK:
            rtcctl_set_next_alarm();
            break;
    }
    return( true );
}

void IRAM_ATTR rtcctl_irq( void ) {
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

void rtcctl_load_data( void ) {
    if (! SPIFFS.exists( CONFIG_FILE_PATH ) ) {
        return; //wil be used default values set during theier creation
    }

    fs::File file = SPIFFS.open( CONFIG_FILE_PATH, FILE_READ );

    if (!file) {
        log_e("Can't open file: %s!", CONFIG_FILE_PATH );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 2 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("rtcctl config deserializeJson() failed: %s", error.c_str() );
        }
        else {
            rtcctl_alarm_t stored_data;
            stored_data.enabled = doc[ENABLED_KEY].as<bool>();
            stored_data.hour = doc[HOUR_KEY].as<uint8_t>();
            stored_data.minute =  doc[MINUTE_KEY].as<uint8_t>();
            uint8_t stored_week_days = doc[WEEK_DAYS_KEY].as<uint8_t>();
            for (int index = 0; index < DAYS_IN_WEEK; ++index){
                stored_data.week_days[index] = ((stored_week_days >> index) & 1) != 0;
            }
            rtcctl_set_alarm(&stored_data);
            doc.clear();
        }
    }
    file.close();
}

void rtcctl_store_data( void ) {
    fs::File file = SPIFFS.open( CONFIG_FILE_PATH, FILE_WRITE );
    if (!file) {
        log_e("Can't open file: %s!", CONFIG_FILE_PATH );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc[VERSION_KEY] = 1;
        doc[ENABLED_KEY] = alarm_data.enabled;
        doc[HOUR_KEY] = alarm_data.hour;
        doc[MINUTE_KEY] = alarm_data.minute;

        uint8_t week_days_to_store = 0;
        for (int index = 0; index < DAYS_IN_WEEK; ++index){
            week_days_to_store |= alarm_data.week_days[index] << index; 
        }
        doc[WEEK_DAYS_KEY] = week_days_to_store;
        
        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write rtcctl config file");
        }
        doc.clear();
    }
    file.close();
}

void rtcctl_set_alarm( rtcctl_alarm_t *data ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    bool was_enabled = alarm_data.enabled;
    if (was_enabled){
        ttgo->rtc->disableAlarm();
    }
    alarm_data = *data;
    rtcctl_store_data();

    set_next_alarm();

    if (was_enabled && !alarm_data.enabled){
        //already disabled
        rtcctl_send_event_cb( RTCCTL_ALARM_DISABLED );
    }
    else if (was_enabled && alarm_data.enabled){
        ttgo->rtc->enableAlarm();
        //nothing actually changed;
    }
    else if (!was_enabled && alarm_data.enabled){
        ttgo->rtc->enableAlarm();
        rtcctl_send_event_cb( RTCCTL_ALARM_ENABLED );   
    }    
}

rtcctl_alarm_t *rtcctl_get_alarm_data( void ) {
    return &alarm_data;
}

int rtcctl_get_next_alarm_week_day( void ) {
    if (!is_any_day_enabled()){
        return RTCCTL_ALARM_NOT_SET;
    }
    tm alarm_tm;
    localtime_r(&alarm_time, &alarm_tm);
    return alarm_tm.tm_wday;
}