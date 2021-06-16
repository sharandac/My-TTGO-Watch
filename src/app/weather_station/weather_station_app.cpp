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
#include <TTGO.h>

#include "weather_station_app.h"
#include "weather_station_app_main.h"
#include "weather_station_app_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

#include "utils/json_psram_allocator.h"

weather_station_config_t weather_station_config;

uint32_t weather_station_app_main_tile_num;
uint32_t weather_station_app_setup_tile_num;

// app icon
icon_t *weather_station_app = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(weather_station_app_64px);

// declare callback functions for the app and widget icon to enter the app
static void enter_weather_station_app_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for example app
 */
void weather_station_app_setup( void ) {
    weather_station_load_config();

    // register 2 vertical tiles and get the first tile number and save it for later use
    weather_station_app_main_tile_num = mainbar_add_app_tile( 1, 2, "weather station" );
    weather_station_app_setup_tile_num = weather_station_app_main_tile_num + 1;

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/ and declare it like LV_IMG_DECLARE( your_icon );
    weather_station_app = app_register( "Weather\nStation", &weather_station_app_64px, enter_weather_station_app_event_cb );
    app_set_indicator( weather_station_app, ICON_INDICATOR_OK );

    // init main and setup tile, see weather_station_app_main.cpp and weather_station_app_setup.cpp
    weather_station_app_main_setup( weather_station_app_main_tile_num );
    weather_station_app_setup_setup( weather_station_app_setup_tile_num );
}

/*
 *
 */
uint32_t weather_station_app_get_app_main_tile_num( void ) {
    return( weather_station_app_main_tile_num );
}

/*
 *
 */
uint32_t weather_station_app_get_app_setup_tile_num( void ) {
    return( weather_station_app_setup_tile_num );
}

/*
 *
 */
static void enter_weather_station_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( weather_station_app_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        app_hide_indicator( weather_station_app );
                                        break;
    }    
}

void weather_station_app_set_indicator(icon_indicator_t indicator) {
    app_set_indicator( weather_station_app, indicator );
}

void weather_station_app_hide_indicator() {
    app_hide_indicator( weather_station_app );
}

weather_station_config_t *weather_station_get_config( void ) {
    return( &weather_station_config );
}

void weather_station_load_config( void ) {
    fs::File file = SPIFFS.open( KODI_REMOTE_JSON_CONFIG_FILE, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", KODI_REMOTE_JSON_CONFIG_FILE );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 4 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            strlcpy( weather_station_config.url, doc["url"], sizeof( weather_station_config.url ) );
        }
        doc.clear();
    }
    file.close();
}

void weather_station_save_config( void ) {
    fs::File file = SPIFFS.open( KODI_REMOTE_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", KODI_REMOTE_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["url"] = weather_station_config.url;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}