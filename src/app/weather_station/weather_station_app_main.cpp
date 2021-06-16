/****************************************************************************
 *   June 16 23:15:00 2021
 *   Copyright  2021  Dirk Sarodnick
 *   Email: programmer@dirk-sarodnick.de
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
#include <TTGO.h>

#include "weather_station_app.h"
#include "weather_station_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/wifictl.h"
#include "utils/json_psram_allocator.h"

bool weather_station_state = false;
static uint64_t nextmillis = 0;

lv_obj_t *weather_station_app_main_tile = NULL;
lv_style_t weather_station_app_main_style;

lv_task_t * _weather_station_app_task;

LV_IMG_DECLARE(refresh_32px);

static void exit_weather_station_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_weather_station_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static bool weather_station_main_wifictl_event_cb( EventBits_t event, void *arg );
void weather_station_app_task( lv_task_t * task );
void weather_station_refresh();

void weather_station_app_main_setup( uint32_t tile_num ) {

    weather_station_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &weather_station_app_main_style, ws_get_mainbar_style() );

    lv_obj_t * exit_btn = wf_add_exit_button( weather_station_app_main_tile, exit_weather_station_app_main_event_cb, &weather_station_app_main_style );
    lv_obj_align(exit_btn, weather_station_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    lv_obj_t * setup_btn = wf_add_setup_button( weather_station_app_main_tile, enter_weather_station_app_setup_event_cb, &weather_station_app_main_style );
    lv_obj_align(setup_btn, weather_station_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );

    // callbacks
    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT, weather_station_main_wifictl_event_cb, "weather station main" );

    // create an task that runs every second
    _weather_station_app_task = lv_task_create( weather_station_app_task, 1000, LV_TASK_PRIO_MID, NULL );
}

static bool weather_station_main_wifictl_event_cb( EventBits_t event, void *arg ) {    
    switch( event ) {
        case WIFICTL_CONNECT:       weather_station_state = true;
                                    weather_station_app_hide_indicator();
                                    break;
        case WIFICTL_DISCONNECT:    weather_station_state = false;
                                    weather_station_app_set_indicator( ICON_INDICATOR_FAIL );
                                    break;
        case WIFICTL_OFF:           weather_station_state = false;
                                    weather_station_app_hide_indicator();
                                    break;
    }
    return( true );
}

static void enter_weather_station_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( weather_station_app_get_app_setup_tile_num(), LV_ANIM_ON );
                                        nextmillis = 0;
                                        break;
    }
}

static void exit_weather_station_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

void weather_station_app_task( lv_task_t * task ) {
    if (!weather_station_state) return;

    if ( nextmillis < millis() ) {
        nextmillis = millis() + 300000L;

        weather_station_refresh();
    }
}

void weather_station_refresh() {
    if (!weather_station_state) return;
    
    weather_station_config_t *weather_station_config = weather_station_get_config();

    HTTPClient http_client;
    http_client.useHTTP10( true );
    http_client.begin( weather_station_config->url );
    int httpcode = http_client.GET();

    if (httpcode > 200 && httpcode < 300) {
        SpiRamJsonDocument doc( 1000 );

        DeserializationError error = deserializeJson( doc, http_client.getStream() );
        if (error) {
            log_e("weather_station deserializeJson() failed: %s", error.c_str() );
            http_client.end();
            return;
        }

        //TODO: parse the values and fill them into the GUI

        doc.clear();
    }

    http_client.end();
}