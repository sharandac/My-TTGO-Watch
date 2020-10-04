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
#include "TTGO.h"
#include <sys/time.h>

#include "wifictl.h"
#include "timesync.h"
#include "powermgm.h"
#include "blectl.h"
#include "json_psram_allocator.h"
#include "callback.h"

EventGroupHandle_t time_event_handle = NULL;
TaskHandle_t _timesync_Task;
timesync_config_t timesync_config;

callback_t *timesync_callback = NULL;

void timesync_Task( void * pvParameters );
bool timesync_powermgm_event_cb( EventBits_t event, void *arg );
bool timesync_wifictl_event_cb( EventBits_t event, void *arg );
bool timesync_blectl_event_cb( EventBits_t event, void *arg );
bool timesync_send_event_cb( EventBits_t event, void *arg );

void timesync_setup( void ) {

    timesync_read_config();
    time_event_handle = xEventGroupCreate();

    wifictl_register_cb( WIFICTL_CONNECT, timesync_wifictl_event_cb, "timesync" );
    blectl_register_cb( BLECTL_MSG, timesync_blectl_event_cb, "time sync ble" );
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, timesync_powermgm_event_cb, "timesync" );

    timesyncToSystem();
}

bool timesync_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( timesync_callback == NULL ) {
        timesync_callback = callback_init( "timesync" );
        if ( timesync_callback == NULL ) {
            log_e("timesync callback alloc failed");
            while(true);
        }
    }    
    return( callback_register( timesync_callback, event, callback_func, id ) );
}

bool timesync_send_event_cb( EventBits_t event, void *arg ) {
    return( callback_send( timesync_callback, event, (void*)NULL ) );
}

bool timesync_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_STANDBY:          
            // only update rtc time when an NTP timesync was success
            if ( xEventGroupGetBits( time_event_handle ) & TIME_SYNC_OK ) {
                timesyncToRTC();
                xEventGroupClearBits( time_event_handle, TIME_SYNC_OK );
                log_i("go standby, timesync to RTC");
            }
            else {
                log_i("go standby");
            }
            break;
        case POWERMGM_WAKEUP:           
            log_i("go wkaeup");
            timesyncToSystem();
            break;
        case POWERMGM_SILENCE_WAKEUP:   
            log_i("go silence wakeup");
            timesyncToSystem();
            break;
    }
    return( true );
}

bool timesync_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch ( event ) {
        case WIFICTL_CONNECT:       
            if ( timesync_config.timesync ) {
            if ( xEventGroupGetBits( time_event_handle ) & TIME_SYNC_REQUEST ) {
                break;
            }
            else {
                xEventGroupSetBits( time_event_handle, TIME_SYNC_REQUEST );
                xTaskCreate(  timesync_Task,      /* Function to implement the task */
                            "timesync Task",    /* Name of the task */
                            2000,              /* Stack size in words */
                            NULL,               /* Task input parameter */
                            1,                  /* Priority of the task */
                            &_timesync_Task );  /* Task handle. */
            }
        }
        break;
    }
    return( true );
}

bool timesync_blectl_event_cb( EventBits_t event, void *arg ) {
    char *settime_str = NULL;
    time_t now;
    struct timeval new_now;

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
    return( true );
}

void timesync_save_config( void ) {
    fs::File file = SPIFFS.open( TIMESYNC_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", TIMESYNC_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["daylightsave"] = timesync_config.daylightsave;
        doc["timesync"] = timesync_config.timesync;
        doc["timezone"] = timesync_config.timezone;
        doc["use_24hr_clock"] = timesync_config.use_24hr_clock;
        doc["timezone_name"] = timesync_config.timezone_name;
        doc["timezone_rule"] = timesync_config.timezone_rule;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void timesync_read_config( void ) {    
    fs::File file = SPIFFS.open( TIMESYNC_JSON_CONFIG_FILE, FILE_READ );

    if (!file) {
        log_e("Can't open file: %s!", TIMESYNC_JSON_CONFIG_FILE );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 2 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            timesync_config.daylightsave = doc["daylightsave"] | false;
            timesync_config.timesync = doc["timesync"] | true;
            timesync_config.timezone = doc["timezone"] | 0;
            timesync_config.use_24hr_clock = doc["use_24hr_clock"] | true;
            // todo: for upgrade, default name = Etc\GMTxxx based on timezone & daylightsave
            // todo: for upgrade, default rule = GMT0 or <-xx>xx based on timezone & daylightsave
            // todo: upgrade rtc clock to be in utc? (First sync will fix it.)
            strlcpy( timesync_config.timezone_name, doc["timezone_name"] | TIMEZONE_NAME_DEFAULT, sizeof( timesync_config.timezone_name ) );
            strlcpy( timesync_config.timezone_rule, doc["timezone_rule"] | TIMEZONE_RULE_DEFAULT, sizeof( timesync_config.timezone_rule ) );
            setenv("TZ", timesync_config.timezone_rule, 1);
            tzset();
        }
        doc.clear();
    }
    file.close();
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

void timesync_set_timezone( int32_t timezone ) {
    timesync_config.timezone = timezone;
    timesync_save_config();
}

char* timesync_get_timezone_name( void ) {
    return( timesync_config.timezone_name );
}

void timesync_set_timezone_name( char * timezone_name ) {
    strlcpy( timesync_config.timezone_name, timezone_name, sizeof( timesync_config.timezone_name ) );
    timesync_save_config();
}

char* timesync_get_timezone_rule( void ) {
    return( timesync_config.timezone_rule );
}

void timesync_set_timezone_rule( const char * timezone_rule ) {
    strlcpy( timesync_config.timezone_rule, timezone_rule, sizeof( timesync_config.timezone_rule ) );
    setenv("TZ", timesync_config.timezone_rule, 1);
    tzset();
    timesync_save_config();
    timesync_send_event_cb( TIME_SYNC_OK, (void *)NULL );
}

void timesync_set_24hr( bool use24 ) {
    timesync_config.use_24hr_clock = use24;
    timesync_save_config();
}

bool timesync_get_24hr(void) {
    return (timesync_config.use_24hr_clock);
}

void timesyncToSystem( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    setenv("TZ", "GMT0", 1);
    tzset();
    ttgo->rtc->syncToSystem();
    setenv("TZ", timesync_config.timezone_rule, 1);
    tzset();
}

void timesyncToRTC( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    setenv("TZ", "GMT0", 1);
    tzset();
    ttgo->rtc->syncToRtc();
    setenv("TZ", timesync_config.timezone_rule, 1);
    tzset();
    timesync_send_event_cb( TIME_SYNC_OK, (void *)NULL );
}

void timesync_Task( void * pvParameters ) {
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
}
