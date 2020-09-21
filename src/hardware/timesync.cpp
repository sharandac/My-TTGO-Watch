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

#include "time.h"
#include "wifictl.h"
#include "config.h"
#include "timesync.h"
#include "powermgm.h"
#include "json_psram_allocator.h"

EventGroupHandle_t time_event_handle = NULL;
TaskHandle_t _timesync_Task;
timesync_config_t timesync_config;

void timesync_Task( void * pvParameters );
bool timesync_powermgm_event_cb( EventBits_t event, void *arg );
bool timesync_wifictl_event_cb( EventBits_t event, void *arg );

void timesync_setup( void ) {

    timesync_read_config();
    time_event_handle = xEventGroupCreate();

    wifictl_register_cb( WIFICTL_CONNECT, timesync_wifictl_event_cb, "timesync" );
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, timesync_powermgm_event_cb, "timesync" );
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

void timesync_save_config( void ) {
    if ( SPIFFS.exists( TIMESYNC_CONFIG_FILE ) ) {
        SPIFFS.remove( TIMESYNC_CONFIG_FILE );
        log_i("remove old binary timesync config");
    }

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

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void timesync_read_config( void ) {
    if ( SPIFFS.exists( TIMESYNC_JSON_CONFIG_FILE ) ) {        
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
            }        
            doc.clear();
        }
        file.close();
    }
    else {
        log_i("no json config exists, read from binary");
        fs::File file = SPIFFS.open( TIMESYNC_CONFIG_FILE, FILE_READ );

        if (!file) {
            log_e("Can't open file: %s!", TIMESYNC_CONFIG_FILE );
        }
        else {
            int filesize = file.size();
            if ( filesize > sizeof( timesync_config ) ) {
                log_e("Failed to read configfile. Wrong filesize!" );
            }
            else {
                file.read( (uint8_t *)&timesync_config, filesize );
                file.close();
                timesync_save_config();
                return;
            }
        file.close();
        }
    }
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

void timesync_set_24hr( bool use24 ) {
    timesync_config.use_24hr_clock = use24;
    timesync_save_config();
}

bool timesync_get_24hr(void) {
    return (timesync_config.use_24hr_clock);
}

void timesyncToSystem( void ) {
  TTGOClass *ttgo = TTGOClass::getWatch();
  ttgo->rtc->syncToSystem();
}

void timesyncToRTC( void ) {
  TTGOClass *ttgo = TTGOClass::getWatch();
  ttgo->rtc->syncToRtc();
}

void timesync_Task( void * pvParameters ) {
  log_i("start time sync task, heap: %d", ESP.getFreeHeap() );

  if ( xEventGroupGetBits( time_event_handle ) & TIME_SYNC_REQUEST ) {   
    struct tm info;

    long gmtOffset_sec = timesync_config.timezone * 3600;
    int daylightOffset_sec = 0;
    
    if ( timesync_config.daylightsave )
      daylightOffset_sec = 3600;
            
    configTime( gmtOffset_sec, daylightOffset_sec, "pool.ntp.org" );

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
