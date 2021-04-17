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
#include "HTTPClient.h"
#include "fakegps.h"

#include "hardware/gpsctl.h"
#include "hardware/wifictl.h"
#include "utils/json_psram_allocator.h"

static float lat = 0;
static float lon = 0;

TaskHandle_t _fakegps_get_location_Task;
void fakegps_get_location_Task( void * pvParameters );
bool fakegps_powermgm_event_cb( EventBits_t event, void *arg );

void fakegps_setup( void ) {
    wifictl_register_cb( WIFICTL_CONNECT_IP, fakegps_powermgm_event_cb, "powermgm fakegps");
}

double fakegps_get_last_lat( void ) {
    return( lat );
}

double fakegps_get_last_lon( void ) {
    return( lon );
}

bool fakegps_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_CONNECT_IP:
            if ( gpsctl_get_gps_over_ip() && gpsctl_get_autoon() ) {
                xTaskCreate(    fakegps_get_location_Task,
                                "fakegps update Task",
                                5000,
                                NULL,
                                1,
                                &_fakegps_get_location_Task );
            }
            break;
    }
    return( true );
}

void fakegps_get_location_Task( void * pvParameters ) {
    int httpcode = -1;

    HTTPClient fakegps_client;

    fakegps_client.setUserAgent( "ESP32-" __FIRMWARE__ );
    fakegps_client.begin( GEOIP_URL );
    httpcode = fakegps_client.GET();

    if ( httpcode != 200 ) {
        log_e("HTTPClient error %d", httpcode );
        fakegps_client.end();
        vTaskDelete( NULL );
    }

    SpiRamJsonDocument doc( fakegps_client.getSize() * 4 );

    DeserializationError error = deserializeJson( doc, fakegps_client.getStream() );
    if (error) {
        log_e("fakegps deserializeJson() failed: %s", error.c_str() );
    }
    else {
        if ( doc["lat"] && doc["lon"] ) {
            lat = doc["lat"].as<float>();
            lon = doc["lon"].as<float>();
            log_i("lat: %f, lon:%f", lat, lon );
            gpsctl_set_location( lat, lon, GPS_SOURCE_IP );
        }
    }

    doc.clear();
    fakegps_client.end();
    vTaskDelete( NULL );
}
    