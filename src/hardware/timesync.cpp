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
#include <sys/time.h>
#include "timesync.h"
#include "powermgm.h"
#include "callback.h"
#include "hardware/config/timesyncconfig.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else  
    #include "wifictl.h"
    #include "blectl.h"
    #include "rtcctl.h"

    EventGroupHandle_t time_event_handle = NULL;
    TaskHandle_t _timesync_Task;
#endif

timesync_config_t timesync_config;
callback_t *timesync_callback = NULL;

void timesync_Task( void * pvParameters );
bool timesync_powermgm_event_cb( EventBits_t event, void *arg );
bool timesync_wifictl_event_cb( EventBits_t event, void *arg );
bool timesync_blectl_event_cb( EventBits_t event, void *arg );
bool timesync_send_event_cb( EventBits_t event, void *arg );

void timesync_setup( void ) {
    /*
     * load config from json
     */
    timesync_config.load();
    #ifndef NATIVE_64BIT
        /*
        * create timesync event group
        */
        time_event_handle = xEventGroupCreate();
        /*
        * register wigi, ble and powermgm callback function
        */
        wifictl_register_cb( WIFICTL_CONNECT, timesync_wifictl_event_cb, "wifictl timesync" );
        blectl_register_cb( BLECTL_MSG, timesync_blectl_event_cb, "blectl timesync" );
    #endif
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, timesync_powermgm_event_cb, "powermgm timesync" );
    /*
     * sync time from rtc to system
     */
    timesyncToSystem();
}

bool timesync_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
        /*
     * check if an callback table exist, if not allocate a callback table
     */
    if ( timesync_callback == NULL ) {
        timesync_callback = callback_init( "timesync" );
        if ( timesync_callback == NULL ) {
            log_e("timesync callback alloc failed");
            while(true);
        }
    }
    /*
     * register an callback entry and return them
     */
    return( callback_register( timesync_callback, event, callback_func, id ) );
}

bool timesync_send_event_cb( EventBits_t event, void *arg ) {
    /*
     * call all callbacks with her event mask
     */
    return( callback_send( timesync_callback, event, (void*)NULL ) );
}

bool timesync_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_STANDBY:          
#ifdef NATIVE_64BIT
            log_i("go standby");
#else
            /*
             * only update rtc time when an NTP timesync was success
             */
            if ( xEventGroupGetBits( time_event_handle ) & TIME_SYNC_OK ) {
                timesyncToRTC();
                xEventGroupClearBits( time_event_handle, TIME_SYNC_OK );
                log_i("go standby, timesync to RTC");
            }
            else {
                log_i("go standby");
            }
#endif
            break;
        case POWERMGM_WAKEUP:           
            /*
             * sync time from rtc to system after wakeup
             */
            log_i("go wakeup");
            timesyncToSystem();
            break;
        case POWERMGM_SILENCE_WAKEUP:   
            /*
             * sync time from rtc to system after silence wakeup
             */
            log_i("go silence wakeup");
            timesyncToSystem();
            break;
    }
    return( true );
}

bool timesync_wifictl_event_cb( EventBits_t event, void *arg ) {
#ifndef NATIVE_64BIT
    switch ( event ) {
        case WIFICTL_CONNECT:
            /*
             * sync time when autosync is enabled
             */ 
            if ( timesync_config.timesync ) {
                /*
                 * check if another sync request is running
                 */
                if ( xEventGroupGetBits( time_event_handle ) & TIME_SYNC_REQUEST ) {
                    break;
                }
                else {
                    /*
                     * start timesync task
                     */
                    xEventGroupSetBits( time_event_handle, TIME_SYNC_REQUEST );
                    xTaskCreate(    timesync_Task,      /* Function to implement the task */
                                    "timesync Task",    /* Name of the task */
                                    2000,              /* Stack size in words */
                                    NULL,               /* Task input parameter */
                                    1,                  /* Priority of the task */
                                    &_timesync_Task );  /* Task handle. */
                }
            }
            break;

    }
#endif
    return( true );
}

bool timesync_blectl_event_cb( EventBits_t event, void *arg ) {
    char *settime_str = NULL;
    time_t now;
    struct timeval new_now;

#ifndef NATIVE_64BIT
    switch( event ) {
        case BLECTL_MSG:
            settime_str = strstr( (const char*)arg, "setTime(" );
            if ( settime_str ) {
                settime_str = settime_str + 8;
                time( &now );
                log_i("old time: %d", now );
                new_now.tv_sec = atol( settime_str );
                new_now.tv_usec = 0;
                if ( settimeofday(&new_now, NULL) == 0 ) {
                    log_i("new time: %d", new_now.tv_sec );
                }
                else {
                    log_e("set new time failed, errno = %d", errno );
                }
                xEventGroupSetBits( time_event_handle, TIME_SYNC_OK );
            }
    }
#endif

    return( true );
}

void timesync_save_config( void ) {
    timesync_config.save();
}

void timesync_read_config( void ) {    
    timesync_config.load();
}

bool timesync_get_timesync( void ) {
    return( timesync_config.timesync );
}

void timesync_set_timesync( bool timesync ) {
    timesync_config.timesync = timesync;
    timesync_save_config();
}

bool timesync_get_daylightsave( void ) {
    return( timesync_config.daylightsave );
}

void timesync_set_daylightsave( bool daylightsave ) {
    timesync_config.daylightsave = daylightsave;
    timesync_save_config();
}

int32_t timesync_get_timezone( void ) {
    return( timesync_config.timezone );
}

char* timesync_get_timezone_name( void ) {
    return( timesync_config.timezone_name );
}

char* timesync_get_timezone_rule( void ) {
    return( timesync_config.timezone_rule );
}

bool timesync_get_24hr(void) {
    return (timesync_config.use_24hr_clock);
}

void timesync_set_timezone( int32_t timezone ) {
    timesync_config.timezone = timezone;
    timesyncToSystem();
    timesync_send_event_cb( TIME_SYNC_UPDATE, (void *)NULL );
    timesync_save_config();
}

void timesync_set_timezone_name( char * timezone_name ) {
    strncpy( timesync_config.timezone_name, timezone_name, sizeof( timesync_config.timezone_name ) );
    timesyncToSystem();
    timesync_send_event_cb( TIME_SYNC_UPDATE, (void *)NULL );
    timesync_save_config();
}

void timesync_set_timezone_rule( const char * timezone_rule ) {
    strncpy( timesync_config.timezone_rule, timezone_rule, sizeof( timesync_config.timezone_rule ) );
    timesyncToSystem();
    timesync_send_event_cb( TIME_SYNC_UPDATE, (void *)NULL );
    timesync_save_config();
}

void timesync_set_24hr( bool use24 ) {
    timesync_config.use_24hr_clock = use24;
    timesyncToSystem();
    timesync_send_event_cb( TIME_SYNC_UPDATE, (void *)NULL );
    timesync_save_config();
}

void timesyncToSystem( void ) {
    /**
     * set TZ tp GMT0 to get time from RTC
     */
    setenv("TZ", "GMT0", 1);
    tzset();
#ifdef NATIVE_64BIT

#else
    rtcctl_syncToSystem();
#endif
    /**
     * set back TZ to local settings
     */
    log_i("TZ rule: %s", timesync_config.timezone_rule );
    setenv("TZ", timesync_config.timezone_rule, 1);
    tzset();
}

void timesyncToRTC( void ) {
    /**
     * set TZ to GMT0 for storinf it into rtc
     */
    setenv("TZ", "GMT0", 1);
    tzset();
#ifdef NATIVE_64BIT

#else
    rtcctl_syncToRtc();
#endif
    /**
     * set back TZ to local settings
     */
    log_i("TZ rule: %s", timesync_config.timezone_rule );
    setenv("TZ", timesync_config.timezone_rule, 1);
    tzset();
    timesync_send_event_cb( TIME_SYNC_OK, (void *)NULL );
    timesync_send_event_cb( TIME_SYNC_UPDATE, (void *)NULL );
}

void timesync_Task( void * pvParameters ) {
#ifndef NATIVE_64BIT
  log_i("start time sync task, heap: %d", ESP.getFreeHeap() );

    if ( xEventGroupGetBits( time_event_handle ) & TIME_SYNC_REQUEST ) { 
        struct tm info;

        configTzTime( timesync_config.timezone_rule, "pool.ntp.org" );

        if( !getLocalTime( &info ) ) {
            log_e("Failed to obtain time" );
        }
        else {
            xEventGroupSetBits( time_event_handle, TIME_SYNC_OK );
        }
    }

    xEventGroupClearBits( time_event_handle, TIME_SYNC_REQUEST );
    log_i("finish time sync task, heap: %d", ESP.getFreeHeap() );
    vTaskDelete( NULL );
#endif
}

void timesync_get_current_timestring( char * buf, size_t buf_len ) {
    time_t now;
    struct tm info;
    /*
    * copy current time into now and convert it local time info
    */
    time( &now );
    localtime_r( &now, &info );

    int h = info.tm_hour;
    int m = info.tm_min;

    if ( timesync_get_24hr() ) {
        snprintf( buf, buf_len, "%02d:%02d", h, m );
    }
    else {
        if (h == 0) h = 12;
        if (h > 12) h -= 12;
        snprintf( buf, buf_len, "%d:%02d", h, m );
    }
}

void timesync_get_current_datestring( char * buf, size_t buf_len ) {
    time_t now;
    struct tm info;
    /*
    * copy current time into now and convert it local time info
    */
    time( &now );
    localtime_r( &now, &info );

    strftime( buf, sizeof( buf_len ), "%a %d.%b %Y", &info );
}

bool timesync_is_between( struct tm start, struct tm end ) {
    time_t now;
    struct tm info;
    /*
    * copy current time into now and convert it local time info
    */
    time( &now );
    localtime_r( &now, &info );

    // differentiate between silencing over the day or night
    if (start.tm_hour < end.tm_hour || (start.tm_hour == end.tm_hour && start.tm_min < end.tm_min)) {
        bool startPassed = info.tm_hour > start.tm_hour || (info.tm_hour == start.tm_hour && info.tm_min > start.tm_min);
        bool endPassed = info.tm_hour < end.tm_hour || (info.tm_hour == end.tm_hour && info.tm_min < end.tm_min);
        return startPassed && endPassed;
    } else {
        bool startPassed = info.tm_hour > start.tm_hour || (info.tm_hour == start.tm_hour && info.tm_min > start.tm_min);
        bool endPassed = info.tm_hour < end.tm_hour || (info.tm_hour == end.tm_hour && info.tm_min < end.tm_min);
        return startPassed || endPassed;
    }
}