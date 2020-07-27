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
#include "timesync.h"
#include <WiFi.h>

EventGroupHandle_t time_event_handle = NULL;
TaskHandle_t _timesync_Task;
void timesync_Task( void * pvParameters );

timesync_config_t timesync_config;

void timesync_setup( TTGOClass *ttgo ) {

    timesync_read_config();

    WiFi.onEvent( [](WiFiEvent_t event, WiFiEventInfo_t info) {
        if ( timesync_config.timesync ) {
          if ( xEventGroupGetBits( time_event_handle ) & TIME_SYNC_REQUEST ) {
              return;
          }
          else {
              xEventGroupSetBits( time_event_handle, TIME_SYNC_REQUEST );
              vTaskResume( _timesync_Task );
          }
        }
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP );

    time_event_handle = xEventGroupCreate();
    xEventGroupClearBits( time_event_handle, TIME_SYNC_REQUEST );

    xTaskCreate(
                        timesync_Task,      /* Function to implement the task */
                        "timesync Task",    /* Name of the task */
                        2000,              /* Stack size in words */
                        NULL,               /* Task input parameter */
                        1,                  /* Priority of the task */
                        &_timesync_Task );  /* Task handle. */

}

void timesync_save_config( void ) {
  fs::File file = SPIFFS.open( TIMESYNC_CONFIG_FILE, FILE_WRITE );

  if ( !file ) {
    Serial.printf("Can't save file: %s\r\n", TIMESYNC_CONFIG_FILE );
  }
  else {
    file.write( (uint8_t *)&timesync_config, sizeof( timesync_config ) );
    file.close();
  }
}

void timesync_read_config( void ) {
  fs::File file = SPIFFS.open( TIMESYNC_CONFIG_FILE, FILE_READ );

  if (!file) {
    Serial.printf("Can't open file: %s!\r\n", TIMESYNC_CONFIG_FILE );
  }
  else {
    int filesize = file.size();
    if ( filesize > sizeof( timesync_config ) ) {
      Serial.printf("Failed to read configfile. Wrong filesize!\r\n" );
    }
    else {
      file.read( (uint8_t *)&timesync_config, filesize );
    }
    file.close();
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

void timesync_Task( void * pvParameters ) {

    while( true ) {
        vTaskDelay( 500 );
        if ( xEventGroupGetBits( time_event_handle ) & TIME_SYNC_REQUEST ) {   
            struct tm info;
            TTGOClass *ttgo = TTGOClass::getWatch();

            long gmtOffset_sec = timesync_config.timezone * 3600;
            int daylightOffset_sec = 0;
            
            if ( timesync_config.daylightsave )
              daylightOffset_sec = 3600;
                    
            configTime( gmtOffset_sec, daylightOffset_sec, "pool.ntp.org" );

            if( !getLocalTime( &info ) ) {
                Serial.println( "Failed to obtain time\r\n" );
            }
            ttgo->rtc->syncToRtc();

            xEventGroupClearBits( time_event_handle, TIME_SYNC_REQUEST );
        }
        vTaskSuspend( _timesync_Task );
    }
}