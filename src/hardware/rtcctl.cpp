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
#include <time.h>

#include "rtcctl.h"
#include "powermgm.h"
#include "callback.h"
#include "timesync.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"

    volatile bool rtc_irq_flag = false;
#else
    #include <sys/time.h>

    volatile bool rtc_irq_flag = false;
    portMUX_TYPE RTC_IRQ_Mux = portMUX_INITIALIZER_UNLOCKED;

    #if defined( M5PAPER )
        #include <M5EPD.h>
        #include <SPIFFS.h>
    #elif defined( M5CORE2 )
        #include <M5Core2.h>
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include "TTGO.h"
    #elif defined( LILYGO_WATCH_2021 )
        #include <PCF8563/pcf8563.h>
        #include <Wire.h>

        PCF8563_Class rtc;
    #elif defined( WT32_SC01 )

    #else
        #warning "no hardware driver for rtcctl"
    #endif

    void IRAM_ATTR rtcctl_irq( void );

    void IRAM_ATTR rtcctl_irq( void ) {
        portENTER_CRITICAL_ISR(&RTC_IRQ_Mux);
        rtc_irq_flag = true;
        portEXIT_CRITICAL_ISR(&RTC_IRQ_Mux);
        powermgm_resume_from_ISR();
    }
#endif

static rtcctl_alarm_t alarm_data; 
static time_t alarm_time = 0;

bool rtcctl_powermgm_event_cb( EventBits_t event, void *arg );
bool rtcctl_powermgm_loop_cb( EventBits_t event, void *arg );
bool rtcctl_timesync_event_cb( EventBits_t event, void *arg );
bool rtcctl_send_event_cb( EventBits_t event );
void rtcctl_load_data( void );
void rtcctl_store_data( void );

callback_t *rtcctl_callback = NULL;

void rtcctl_setup( void ) {
#ifdef NATIVE_64BIT

#else
    #if defined( M5PAPER )
        M5.RTC.begin();
    #elif defined( M5CORE2 )
        M5.Rtc.begin();
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        /**
         * fix issue #276
         * disable timer and clk if enabled from older projects
         */
        TTGOClass *ttgo = TTGOClass::getWatch();
        if ( ttgo->rtc->isTimerActive() || ttgo->rtc->isTimerEnable() ) {
            log_d("clear/disable rtc timer");
            ttgo->rtc->clearTimer();
            ttgo->rtc->disableTimer();
        }
        ttgo->rtc->disableCLK();

        pinMode( RTC_INT_PIN, INPUT_PULLUP);
        attachInterrupt( RTC_INT_PIN, &rtcctl_irq, FALLING );
    #elif defined( LILYGO_WATCH_2021 )
        #include <twatch2021_config.h>

        rtc.begin();
        if ( rtc.isTimerActive() || rtc.isTimerEnable() ) {
            log_d("clear/disable rtc timer");
            rtc.clearTimer();
            rtc.disableTimer();
        }
        rtc.disableCLK();

        #if defined( VERSION_2 )
//            pinMode( RTC_Int, INPUT);
//            attachInterrupt( RTC_Int, &rtcctl_irq, GPIO_INTR_POSEDGE );
        #endif
    #elif defined( WT32_SC01 )

    #endif
#endif

    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_ENABLE_INTERRUPTS | POWERMGM_DISABLE_INTERRUPTS , rtcctl_powermgm_event_cb, "powermgm rtcctl" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, rtcctl_powermgm_loop_cb, "powermgm rtcctl loop" );
    timesync_register_cb( TIME_SYNC_OK, rtcctl_timesync_event_cb, "timesync rtcctl" );

    rtcctl_load_data();
}

bool rtcctl_send_event_cb( EventBits_t event ) {
    return( callback_send( rtcctl_callback, event, (void*)NULL ) );
}

static bool is_enabled( void ) {
    return alarm_data.enabled;
}

bool is_any_day_enabled( void ) {
    for (int index = 0; index < DAYS_IN_WEEK; ++index){
        if (alarm_data.week_days[index])
            return true; 
    }
    return false;
}

static bool is_day_checked( int wday ) {
    // No day checked mean ALL days
    return alarm_data.week_days[wday] || !is_any_day_enabled();
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
        if (is_day_checked( wday_index )){
            return ret_val;
        }        
    } while (wday_index != day_of_week);
    
    return ret_val; //the same day of next week
}

void set_next_alarm( void ) {


    if ( !is_enabled() ) {
#ifdef NATIVE_64BIT

#else
    #if defined( M5PAPER )
//        M5.RTC.begin();
    #elif defined( M5CORE2 )

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        TTGOClass *ttgo = TTGOClass::getWatch();
        ttgo->rtc->setAlarm( PCF8563_NO_ALARM, PCF8563_NO_ALARM, PCF8563_NO_ALARM, PCF8563_NO_ALARM );    
    #elif defined( LILYGO_WATCH_2021 )
        rtc.setAlarm( PCF8563_NO_ALARM, PCF8563_NO_ALARM, PCF8563_NO_ALARM, PCF8563_NO_ALARM );        
    #elif defined( WT32_SC01 )

    #else
        #warning "no alarm rtcctl function"
    #endif
#endif
        rtcctl_send_event_cb( RTCCTL_ALARM_TERM_SET );
        return;
    } 

    time_t now;
    time( &now );
    alarm_time = now;
    struct tm alarm_tm;

    // get local time and set alarm time
    localtime_r(&alarm_time, &alarm_tm);
    log_d("local time: %02d:%02d day: %d", alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_mday );
    alarm_tm.tm_hour = alarm_data.hour;
    alarm_tm.tm_min = alarm_data.minute;
    alarm_time = mktime( &alarm_tm );

    if ( alarm_time <= now  || !is_day_checked( alarm_tm.tm_wday ) ) {
        alarm_time = find_next_alarm_day( alarm_tm.tm_wday, alarm_time );
        localtime_r( &alarm_time, &alarm_tm );
    }

    /*
     * convert local alarm time into GMT0 alarm time, it is necessary sine rtc store time in GMT0
     */
    log_d("next local alarm time: %02d:%02d day: %d", alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_mday );
    gmtime_r( &alarm_time, &alarm_tm );
    log_d("next GMT0 alarm time: %02d:%02d day %d", alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_mday );

#ifdef NATIVE_64BIT

#else
    #if defined( M5PAPER )

    #elif defined( M5CORE2 )

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        /*
         * it is better define alarm by day in month rather than weekday.
         * This way will be work-around an error in pcf8563 source and 
         * will avoid eaising alarm when there is only one alarm in the week (today) and alarm time is set to now
         */
        TTGOClass *ttgo = TTGOClass::getWatch();
        ttgo->rtc->setAlarm( alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_mday, PCF8563_NO_ALARM );
    #elif defined( LILYGO_WATCH_2021 )
        rtc.setAlarm( alarm_tm.tm_hour, alarm_tm.tm_min, alarm_tm.tm_mday, PCF8563_NO_ALARM );
    #elif defined( WT32_SC01 )

    #else
        #warning "no alarm rtcctl function"
    #endif
#endif
    rtcctl_send_event_cb( RTCCTL_ALARM_TERM_SET );
}

void rtcctl_set_next_alarm( void ) {

    if (alarm_data.enabled){
#ifdef NATIVE_64BIT

#else
    #if defined( M5PAPER )
//        M5.RTC.clearIRQ();
    #elif defined( M5CORE2 )

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        TTGOClass *ttgo = TTGOClass::getWatch();
        ttgo->rtc->disableAlarm();
    #elif defined( LILYGO_WATCH_2021 )
        rtc.disableAlarm();
    #elif defined( WT32_SC01 )

    #else
        #warning "no alarm rtcctl function"
    #endif
#endif
    }

    set_next_alarm();
    
    if (alarm_data.enabled){
#ifdef NATIVE_64BIT

#else
    #if defined( M5PAPER )
//        M5.RTC.setAlarmIRQ();
    #elif defined( M5CORE2 )

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        TTGOClass *ttgo = TTGOClass::getWatch();
        ttgo->rtc->enableAlarm();
    #elif defined( LILYGO_WATCH_2021 )
        rtc.enableAlarm();
    #elif defined( WT32_SC01 )

    #else
        #warning "no alarm rtcctl function"
    #endif
#endif
    }
}

bool rtcctl_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_STANDBY:          log_d("go standby");
                                        #ifdef NATIVE_64BIT

                                        #else
                                            #if defined( M5PAPER )
                                            #elif defined( M5CORE2 )
                                            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                                                gpio_wakeup_enable( (gpio_num_t)RTC_INT_PIN, GPIO_INTR_LOW_LEVEL );
                                                esp_sleep_enable_gpio_wakeup ();
                                            #elif defined( LILYGO_WATCH_2021 ) && defined( VERSION_2 )
                                                // gpio_wakeup_enable( (gpio_num_t)RTC_Int, GPIO_INTR_POSEDGE );
                                                // esp_sleep_enable_gpio_wakeup ();
                                            #elif defined( WT32_SC01 )
                                            #else
                                                #warning "no rtcctl powermgm standby event"
                                            #endif
                                        #endif
                                        break;
        case POWERMGM_WAKEUP:           log_d("go wakeup");
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   log_d("go silence wakeup");
                                        break;
        case POWERMGM_ENABLE_INTERRUPTS:
                                        #ifdef NATIVE_64BIT

                                        #else
                                            #if defined( M5PAPER )
                                            #elif defined( M5CORE2 )
                                            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                                                attachInterrupt( RTC_INT_PIN, &rtcctl_irq, FALLING );
                                            #elif defined( LILYGO_WATCH_2021 )
                                            #elif defined( WT32_SC01 )
                                            #else
                                                #warning "no rtcctl powermgm enable interrupts event"
                                            #endif
                                        #endif
                                        break;
        case POWERMGM_DISABLE_INTERRUPTS:
                                        #ifdef NATIVE_64BIT

                                        #else
                                            #if defined( M5PAPER )
                                            #elif defined( M5CORE2 )
                                            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                                                detachInterrupt( RTC_INT_PIN );
                                            #elif defined( LILYGO_WATCH_2021 )
                                            #elif defined( WT32_SC01 )
                                            #else
                                                #warning "no rtcctl powermgm disable interrupts event"
                                            #endif
                                        #endif
                                        break;
    }
    return( true );
}

bool rtcctl_powermgm_loop_cb( EventBits_t event, void *arg ) {
    bool temp_rtc_irq_flag = false;

#ifndef NATIVE_64BIT
    portENTER_CRITICAL( &RTC_IRQ_Mux );
    temp_rtc_irq_flag = rtc_irq_flag;
    rtc_irq_flag = false;
    portEXIT_CRITICAL( &RTC_IRQ_Mux );
#endif
    if ( temp_rtc_irq_flag ) {
        #if defined( LILYGO_WATCH_2021 ) && defined( VERSION_2 )
            if( rtc.status2() & PCF8563_ALARM_AF )
                rtcctl_send_event_cb( RTCCTL_ALARM_OCCURRED );
        #else
                rtcctl_send_event_cb( RTCCTL_ALARM_OCCURRED );
        #endif
    }
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
    rtcctl_alarm_t stored_data;
    stored_data.load();
    rtcctl_set_alarm(&stored_data);
}

void rtcctl_store_data( void ) {
    alarm_data.save();
}

void rtcctl_set_alarm( rtcctl_alarm_t *data ) {
    bool was_enabled = alarm_data.enabled;
    if (was_enabled){
        #ifdef NATIVE_64BIT

        #else
            #if defined( M5PAPER )

            #elif defined( M5CORE2 )

            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                TTGOClass *ttgo = TTGOClass::getWatch();
                ttgo->rtc->disableAlarm();
            #elif defined( LILYGO_WATCH_2021 )
                rtc.disableAlarm();
            #elif defined( WT32_SC01 )
            #else
                #warning "no rtcctl alarm function"
            #endif
        #endif
    }
    alarm_data = *data;
    rtcctl_store_data();

    set_next_alarm();

    if (was_enabled && !alarm_data.enabled){
        /*
         * already disabled
         */
        rtcctl_send_event_cb( RTCCTL_ALARM_DISABLED );
    }
    else if (was_enabled && alarm_data.enabled){
        /*
         * nothing actually changed;
         */
        #ifdef NATIVE_64BIT

        #else
            #if defined( M5PAPER )

            #elif defined( M5CORE2 )

            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                TTGOClass *ttgo = TTGOClass::getWatch();
                ttgo->rtc->enableAlarm();
            #elif defined( LILYGO_WATCH_2021 )
                rtc.enableAlarm();
            #elif defined( WT32_SC01 )

            #else
                #warning "no rtcctl alarm function"
            #endif
        #endif
    }
    else if (!was_enabled && alarm_data.enabled){
        #ifdef NATIVE_64BIT

        #else
            #if defined( M5PAPER )

            #elif defined( M5CORE2 )

            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                TTGOClass *ttgo = TTGOClass::getWatch();
                ttgo->rtc->enableAlarm();
            #elif defined( LILYGO_WATCH_2021 )
                rtc.enableAlarm();
            #elif defined( WT32_SC01 )

            #else
                #warning "no rtcctl alarm function"
            #endif
        #endif
        rtcctl_send_event_cb( RTCCTL_ALARM_ENABLED );   
    }    
}

rtcctl_alarm_t *rtcctl_get_alarm_data( void ) {
    return &alarm_data;
}

int rtcctl_get_next_alarm_week_day( void ) {
    if (!is_enabled()){
        return RTCCTL_ALARM_NOT_SET;
    }
    tm alarm_tm;
    localtime_r(&alarm_time, &alarm_tm);
    return alarm_tm.tm_wday;
}

void rtcctl_syncToSystem( void ) {
    #ifdef NATIVE_64BIT
    
    #else
        #if defined( M5PAPER )
            struct tm t_tm;
            struct timeval val;
            /**
             * get GMT0 RTC time
             */
            rtc_time_t RTCtime;
            rtc_date_t RTCDate;

            M5.RTC.getTime(&RTCtime);
            M5.RTC.getDate(&RTCDate);

            t_tm.tm_hour = RTCtime.hour;
            t_tm.tm_min = RTCtime.min;
            t_tm.tm_sec = RTCtime.sec;
            t_tm.tm_year = RTCDate.year - 1900;    //Year, whose value starts from 1900
            t_tm.tm_mon = RTCDate.mon - 1;       //Month (starting from January, 0 for January) - Value range is [0,11]
            t_tm.tm_mday = RTCDate.day;
            val.tv_sec = mktime(&t_tm);
            val.tv_usec = 0;
            settimeofday(&val, NULL);
        #elif defined( M5CORE2 )
            struct tm t_tm;
            struct timeval val;
            /**
             * get GMT0 RTC time
             */
            
            RTC_TimeTypeDef RTCtime;
            RTC_DateTypeDef RTCDate;

            M5.Rtc.GetTime( &RTCtime );
            M5.Rtc.GetDate( &RTCDate );

            t_tm.tm_hour = RTCtime.Hours;
            t_tm.tm_min = RTCtime.Minutes;
            t_tm.tm_sec = RTCtime.Seconds;
            t_tm.tm_year = RTCDate.Year - 1900;    //Year, whose value starts from 1900
            t_tm.tm_mon = RTCDate.Month - 1;       //Month (starting from January, 0 for January) - Value range is [0,11]
            t_tm.tm_mday = RTCDate.Date;
            val.tv_sec = mktime(&t_tm);
            val.tv_usec = 0;
            settimeofday(&val, NULL);
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            ttgo->rtc->syncToSystem();
        #elif defined( LILYGO_WATCH_2021 )
            rtc.syncToSystem();
        #elif defined( WT32_SC01 )
        #endif
    #endif
}

void rtcctl_syncToRtc( void ) {
    #ifdef NATIVE_64BIT
    
    #else
        #if defined( M5PAPER )
            time_t now;
            struct tm  t_tm;
            /**
             * get GMT0 system time
             */
            time(&now);
            localtime_r(&now, &t_tm);
            /**
             * store GMT0 System time to RTC
             */
            rtc_time_t RTCtime;
            rtc_date_t RTCDate;

            RTCtime.hour = t_tm.tm_hour;
            RTCtime.min = t_tm.tm_min;
            RTCtime.sec = t_tm.tm_sec;
            M5.RTC.setTime(&RTCtime);

            RTCDate.year = t_tm.tm_year + 1900;
            RTCDate.mon = t_tm.tm_mon + 1;
            RTCDate.day = t_tm.tm_mday;
            M5.RTC.setDate(&RTCDate);
        #elif defined( M5CORE2 )
            time_t now;
            struct tm  t_tm;
            /**
             * get GMT0 system time
             */
            time(&now);
            localtime_r(&now, &t_tm);
            /**
             * store GMT0 System time to RTC
             */
            RTC_TimeTypeDef RTCtime;
            RTC_DateTypeDef RTCDate;

            RTCtime.Hours = t_tm.tm_hour;
            RTCtime.Minutes = t_tm.tm_min;
            RTCtime.Seconds = t_tm.tm_sec;
            M5.Rtc.SetTime( &RTCtime );

            RTCDate.Year = t_tm.tm_year + 1900;
            RTCDate.Month = t_tm.tm_mon + 1;
            RTCDate.Date = t_tm.tm_mday;
            M5.Rtc.SetDate( &RTCDate );
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            ttgo->rtc->syncToRtc();
        #elif defined( LILYGO_WATCH_2021 )
            rtc.syncToRtc();
        #elif defined( WT32_SC01 )
        #endif
    #endif
}