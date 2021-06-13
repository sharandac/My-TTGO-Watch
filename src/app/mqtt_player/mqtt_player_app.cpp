/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#include <TTGO.h>

#include "mqtt_player_app.h"
#include "mqtt_player_app_main.h"
#include "mqtt_player_app_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

#include "utils/mqtt/mqtt.h"
#include "utils/json_psram_allocator.h"

mqtt_player_config_t mqtt_player_config;

uint32_t mqtt_player_main_tile_num;
uint32_t mqtt_player_setup_tile_num;

// app icon
icon_t *mqtt_player_app = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(mqtt_player_64px);

// declare callback functions for the app and widget icon to enter the app
static void enter_mqtt_player_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for mqtt player
 */
void mqtt_player_app_setup( void ) {
    mqtt_player_load_config();

    // register 2 vertical tiles and get the first tile number and save it for later use
    mqtt_player_main_tile_num = mainbar_add_app_tile( 1, 2, "mqtt player" );
    mqtt_player_setup_tile_num = mqtt_player_main_tile_num + 1;

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/ and declare it like LV_IMG_DECLARE( your_icon );
    mqtt_player_app = app_register( "MQTT\nPlayer", &mqtt_player_64px, enter_mqtt_player_event_cb );
    app_hide_indicator( mqtt_player_app );

    // init main and setup tile, see mqtt_player_main.cpp and mqtt_player_setup.cpp
    mqtt_player_main_setup( mqtt_player_main_tile_num );
    mqtt_player_setup_setup( mqtt_player_setup_tile_num );
}

/*
 *
 */
uint32_t mqtt_player_get_app_main_tile_num( void ) {
    return( mqtt_player_main_tile_num );
}

/*
 *
 */
uint32_t mqtt_player_get_app_setup_tile_num( void ) {
    return( mqtt_player_setup_tile_num );
}

/*
 *
 */
static void enter_mqtt_player_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( mqtt_player_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        app_hide_indicator( mqtt_player_app );
                                        mqtt_start();
                                        break;
    }    
}

void mqtt_player_app_set_indicator(icon_indicator_t indicator) {
    app_set_indicator( mqtt_player_app, indicator );
}

void mqtt_player_app_hide_indicator() {
    app_hide_indicator( mqtt_player_app );
}

mqtt_player_config_t *mqtt_player_get_config( void ) {
    return( &mqtt_player_config );
}

void mqtt_player_load_config( void ) {
    fs::File file = SPIFFS.open( MQTT_PLAYER_JSON_CONFIG_FILE, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", MQTT_PLAYER_JSON_CONFIG_FILE );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 4 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            strlcpy( mqtt_player_config.topic_base, doc["topic_base"], sizeof( mqtt_player_config.topic_base ) );
            strlcpy( mqtt_player_config.topic_state, doc["topic_state"], sizeof( mqtt_player_config.topic_state ) );
            strlcpy( mqtt_player_config.topic_artist, doc["topic_artist"], sizeof( mqtt_player_config.topic_artist ) );
            strlcpy( mqtt_player_config.topic_title, doc["topic_title"], sizeof( mqtt_player_config.topic_title ) );
            strlcpy( mqtt_player_config.topic_cmd_play, doc["topic_cmd_play"], sizeof( mqtt_player_config.topic_cmd_play ) );
            strlcpy( mqtt_player_config.topic_cmd_pause, doc["topic_cmd_pause"], sizeof( mqtt_player_config.topic_cmd_pause ) );
            strlcpy( mqtt_player_config.topic_cmd_next, doc["topic_cmd_next"], sizeof( mqtt_player_config.topic_cmd_next ) );
            strlcpy( mqtt_player_config.topic_cmd_prev, doc["topic_cmd_prev"], sizeof( mqtt_player_config.topic_cmd_prev ) );
            strlcpy( mqtt_player_config.topic_cmd_volumeup, doc["topic_cmd_volumeup"], sizeof( mqtt_player_config.topic_cmd_volumeup ) );
            strlcpy( mqtt_player_config.topic_cmd_volumedown, doc["topic_cmd_volumedown"], sizeof( mqtt_player_config.topic_cmd_volumedown ) );
        }
        doc.clear();
    }
    file.close();
}

void mqtt_player_save_config( void ) {
    fs::File file = SPIFFS.open( MQTT_PLAYER_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", MQTT_PLAYER_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["topic_base"] = mqtt_player_config.topic_base;
        doc["topic_state"] = mqtt_player_config.topic_state;
        doc["topic_artist"] = mqtt_player_config.topic_artist;
        doc["topic_title"] = mqtt_player_config.topic_title;
        doc["topic_cmd_play"] = mqtt_player_config.topic_cmd_play;
        doc["topic_cmd_pause"] = mqtt_player_config.topic_cmd_pause;
        doc["topic_cmd_next"] = mqtt_player_config.topic_cmd_next;
        doc["topic_cmd_prev"] = mqtt_player_config.topic_cmd_prev;
        doc["topic_cmd_volumeup"] = mqtt_player_config.topic_cmd_volumeup;
        doc["topic_cmd_volumedown"] = mqtt_player_config.topic_cmd_volumedown;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}