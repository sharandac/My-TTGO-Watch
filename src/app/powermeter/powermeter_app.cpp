/****************************************************************************
 *   Sep 3 23:05:42 2020
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

#include "powermeter_app.h"
#include "powermeter_main.h"
#include "powermeter_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

#include "hardware/json_psram_allocator.h"

powermeter_config_t powermeter_config;

// app and widget icon
icon_t *powermeter_app = NULL;
icon_t *powermeter_widget = NULL;

uint32_t powermeter_app_main_tile_num;
uint32_t powermeter_app_setup_tile_num;

// declare you images or fonts you need
LV_IMG_DECLARE(powermeter_64px);

// declare callback functions
static void enter_powermeter_app_event_cb( lv_obj_t * obj, lv_event_t event );

// setup routine for example app
void powermeter_app_setup( void ) {

    powermeter_load_config();

    // register 2 vertical tiles and get the first tile number and save it for later use
    powermeter_app_main_tile_num = mainbar_add_app_tile( 1, 3, "Powermeter App" );
    powermeter_app_setup_tile_num = powermeter_app_main_tile_num + 1;

    powermeter_app = app_register( "power-\nmeter", &powermeter_64px, enter_powermeter_app_event_cb );

    if ( powermeter_config.widget ) {
        powermeter_add_widget();
    }

    powermeter_main_tile_setup( powermeter_app_main_tile_num );
    powermeter_setup_tile_setup( powermeter_app_setup_tile_num );
}

uint32_t powermeter_get_app_main_tile_num( void ) {
    return( powermeter_app_main_tile_num );
}

uint32_t powermeter_get_app_setup_tile_num( void ) {
    return( powermeter_app_setup_tile_num );
}

icon_t *powermeter_get_app_icon( void ) {
    return( powermeter_app );
}

icon_t *powermeter_get_widget_icon( void ) {
    return( powermeter_widget );
}

static void enter_powermeter_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( powermeter_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}


powermeter_config_t *powermeter_get_config( void ) {
    return( &powermeter_config );
}

void powermeter_load_config( void ) {
    fs::File file = SPIFFS.open( POWERMETER_JSON_CONFIG_FILE, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", POWERMETER_JSON_CONFIG_FILE );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 4 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            strlcpy( powermeter_config.server, doc["powermeter"]["server"], sizeof( powermeter_config.server ) );
	    powermeter_config.port = doc["powermeter"]["port"] | 1883;
            powermeter_config.ssl = doc["powermeter"]["ssl"] | false;
            strlcpy( powermeter_config.user, doc["powermeter"]["user"], sizeof( powermeter_config.user ) );
            strlcpy( powermeter_config.password, doc["powermeter"]["password"], sizeof( powermeter_config.password ) );
            strlcpy( powermeter_config.topic, doc["powermeter"]["topic"], sizeof( powermeter_config.topic ) );
            powermeter_config.autoconnect = doc["powermeter"]["autoconnect"] | false;
            powermeter_config.widget = doc["powermeter"]["widget"] | false;
        }        
        doc.clear();
    }
    file.close();
}

void powermeter_save_config( void ) {
    fs::File file = SPIFFS.open( POWERMETER_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", POWERMETER_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["powermeter"]["server"] = powermeter_config.server;
        doc["powermeter"]["port"] = powermeter_config.port;
        doc["powermeter"]["ssl"] = powermeter_config.ssl;
        doc["powermeter"]["user"] = powermeter_config.user;
        doc["powermeter"]["password"] = powermeter_config.password;
        doc["powermeter"]["topic"] = powermeter_config.topic;
        doc["powermeter"]["port"] = powermeter_config.port;
        doc["powermeter"]["autoconnect"] = powermeter_config.autoconnect;
        doc["powermeter"]["widget"] = powermeter_config.widget;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}


bool powermeter_add_widget( void ) {
    if ( powermeter_widget == NULL ) {
        powermeter_widget = widget_register( "n/a", &powermeter_64px, enter_powermeter_app_event_cb );
        widget_hide_indicator( powermeter_widget );
        if ( powermeter_widget != NULL ) {
            return( true );
        }
        else {
            return( false );
        }
    }
    return( true );
}

bool powermeter_remove_widget( void ) {
    powermeter_widget = widget_remove( powermeter_widget );
    return( true );
}
