/****************************************************************************
 *   July 28 00:23:05 2020
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
#include "fakegps.h"
#include "hardware/gpsctl.h"
#include "hardware/wifictl.h"
#include "utils/uri_load/uri_load.h"
#include "utils/json_psram_allocator.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "hardware/callback.h"
    #include "utils/io.h"

    static EventBits_t fakegps_event = 0;
#else
    EventGroupHandle_t fakegps_event = NULL;
    TaskHandle_t _fakegps_get_location_Task = NULL;
#endif

static float lat = 0;
static float lon = 0;

static volatile bool fakegps_wifi_enable = false;

void fakegps_get_location_Task( void * pvParameters );
bool fakegps_wifictl_event_cb( EventBits_t event, void *arg );
bool fakegps_gpsctl_event_cb( EventBits_t event, void *arg );
void fakegps_start_task( void );

void fakegps_setup( void ) {
    #ifdef NATIVE_64BIT
        fakegps_event = 0;
    #else
        fakegps_event = xEventGroupCreate();
    #endif
    fakegps_wifi_enable = false;
    wifictl_register_cb( WIFICTL_CONNECT_IP | WIFICTL_DISCONNECT | WIFICTL_OFF, fakegps_wifictl_event_cb, "wifictl fakegps");
    gpsctl_register_cb( GPSCTL_ENABLE, fakegps_gpsctl_event_cb, "gpsctl fakegps");
}

double fakegps_get_last_lat( void ) {
    return( lat );
}

double fakegps_get_last_lon( void ) {
    return( lon );
}

bool fakegps_gpsctl_event_cb( EventBits_t event, void *arg ) {
    switch ( event ) {
        case GPSCTL_ENABLE:
            if( fakegps_wifi_enable ) {
                fakegps_start_task();
            }
            break;        
    }
    return( true );
}

bool fakegps_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_CONNECT_IP:
            fakegps_wifi_enable = true;
            fakegps_start_task();
            break;
        case WIFICTL_DISCONNECT:
            fakegps_wifi_enable = false;
            break;
        case WIFICTL_OFF:
            fakegps_wifi_enable = false;
            break;
    }
    return( true );
}

void fakegps_start_task( void ) {
    #ifdef NATIVE_64BIT
        if ( fakegps_event & FAKEGPS_SYNC_REQUEST ) {
            return;
        }
        else {
            if ( gpsctl_get_gps_over_ip() && gpsctl_get_autoon() ) {
                fakegps_event |= FAKEGPS_SYNC_REQUEST;
                fakegps_get_location_Task( NULL );
            }
        }
    #else
        if ( xEventGroupGetBits( fakegps_event ) & FAKEGPS_SYNC_REQUEST ) {
            return;
        }
        else {
            if ( gpsctl_get_gps_over_ip() && gpsctl_get_autoon() ) {
                xEventGroupSetBits( fakegps_event, FAKEGPS_SYNC_REQUEST );
                xTaskCreate(    fakegps_get_location_Task,
                                "fakegps update Task",
                                5000,
                                NULL,
                                1,
                                &_fakegps_get_location_Task );
            }
        }
    #endif
}

void fakegps_get_location_Task( void * pvParameters ) {
    #ifdef NATIVE_64BIT
        log_i("start fakegps task" );
        if ( fakegps_event & FAKEGPS_SYNC_REQUEST ) {
    #else
        log_i("start fakegps task, heap: %d", ESP.getFreeHeap() );
        if ( xEventGroupGetBits( fakegps_event ) & FAKEGPS_SYNC_REQUEST ) {
    #endif
            uri_load_dsc_t *uri_load_dsc = uri_load_to_ram( GEOIP_URL );
            if ( uri_load_dsc ) {

                SpiRamJsonDocument doc( uri_load_dsc->size * 4 );

                DeserializationError error = deserializeJson( doc, uri_load_dsc->data );
                if (error) {
                    log_e("fakegps deserializeJson() failed: %s", error.c_str() );
                }
                else {
                    if ( doc["lat"] && doc["lon"] ) {
                        lat = doc["lat"].as<float>();
                        lon = doc["lon"].as<float>();
                        log_i("lat: %f, lon:%f", lat, lon );
                        gpsctl_set_location( lat, lon, 0, GPS_SOURCE_IP, true );
                    }
                }
                doc.clear();
            }
            else {
                log_e("get location via fakegps failed");
            }
            uri_load_free_all( uri_load_dsc );
        }
    #ifdef NATIVE_64BIT
        fakegps_event &= ~FAKEGPS_SYNC_REQUEST;
        log_i("finish fakegps task");
    #else
        xEventGroupClearBits( fakegps_event, FAKEGPS_SYNC_REQUEST );
        log_i("finish fakegps task, heap: %d", ESP.getFreeHeap() );
        vTaskDelete( NULL );
    #endif
}
    