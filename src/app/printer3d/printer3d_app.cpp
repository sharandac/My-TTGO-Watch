/****************************************************************************
 *   January 04 19:00:00 2022
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

#include "printer3d_app.h"
#include "printer3d_app_main.h"
#include "printer3d_app_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

#include "utils/json_psram_allocator.h"

#ifdef NATIVE_64BIT
    #include <iostream>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include "utils/logging.h"
    #include "utils/millis.h"
    #include <string>

    using namespace std;
    #define String string
#else
    #include <Arduino.h>
    #include <FS.h>
    #include <SPIFFS.h>
#endif

printer3d_config_t printer3d_config;

uint32_t printer3d_app_main_tile_num;
uint32_t printer3d_app_setup_tile_num;

// app icon
icon_t *printer3d_app = NULL;

// widget icon
icon_t *printer3d_widget = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(printer3d_app_64px);

// declare callback functions for the app and widget icon to enter the app
static void enter_printer3d_app_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_printer3d_widget_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for printer3d app
 */
void printer3d_app_setup( void ) {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif
    printer3d_load_config();

    // register 2 vertical tiles and get the first tile number and save it for later use
    printer3d_app_main_tile_num = mainbar_add_app_tile( 1, 1, "3d printer app" );
    printer3d_app_setup_tile_num = mainbar_add_setup_tile( 1, 1, "3d printer app setup" );

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/printer3ds/images/ and declare it like LV_IMG_DECLARE( your_icon );
    printer3d_app = app_register( "3d printer", &printer3d_app_64px, enter_printer3d_app_event_cb );
    app_hide_indicator( printer3d_app );

#ifdef PRINTER3D_WIDGET
    // register widget icon on the main tile
    // set your own icon and register her callback to activate by an click
    // remember, an widget icon must have an max size of 64x64 pixel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/printer3ds/images/ and declare it like LV_IMG_DECLARE( your_icon );
    printer3d_widget = widget_register( "3d printer", &printer3d_app_64px, enter_printer3d_widget_event_cb );
    widget_set_indicator( printer3d_widget, ICON_INDICATOR_UPDATE );
#endif // PRINTER3D_WIDGET

    // init main and setup tile, see printer3d_app_main.cpp and printer3d_app_setup.cpp
    printer3d_app_main_setup( printer3d_app_main_tile_num );
    printer3d_app_setup_setup( printer3d_app_setup_tile_num );
}

/*
 *
 */
uint32_t printer3d_app_get_app_main_tile_num( void ) {
    return( printer3d_app_main_tile_num );
}

/*
 *
 */
uint32_t printer3d_app_get_app_setup_tile_num( void ) {
    return( printer3d_app_setup_tile_num );
}

/*
 *
 */
static void enter_printer3d_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( printer3d_app_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        app_hide_indicator( printer3d_app );
                                        break;
    }    
}

/*
 *
 */
static void enter_printer3d_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        widget_hide_indicator( printer3d_widget );
                                        mainbar_jump_to_tilenumber( printer3d_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

void printer3d_app_set_indicator(icon_indicator_t indicator) {
    app_set_indicator( printer3d_app, indicator );
}

void printer3d_app_hide_indicator() {
    app_hide_indicator( printer3d_app );
}

printer3d_config_t *printer3d_get_config( void ) {
    return( &printer3d_config );
}

void printer3d_load_config( void ) {
#ifdef NATIVE_64BIT
    std::fstream file;
    const char *homedir;
    char fileName[256] = "";
    char configFileName[] = PRINTER3D_JSON_CONFIG_FILE;
    /**
     * get home dir and build full path to file
     */
    if ( ( homedir = getenv("HOME") ) == NULL ) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    if ( configFileName[0] == '/')
        snprintf( fileName, sizeof( fileName ), "%s/.hedge/spiffs%s", homedir, configFileName );
    else
    {
        fileName[0] = '/';
        snprintf( fileName, sizeof( fileName ), "%s/.hedge/spiffs/%s", homedir, configFileName );
    }

    file.open( fileName, std::fstream::in );
#else
    fs::File file = SPIFFS.open( PRINTER3D_JSON_CONFIG_FILE, FILE_READ );
#endif
    if (!file) {
        log_e("Can't open file: %s!", PRINTER3D_JSON_CONFIG_FILE );
    }
    else {
#ifdef NATIVE_64BIT
        file.seekg( 0, file.end );
        int filesize = file.tellg();
        file.seekg( 0, file.beg );
#else
        int filesize = file.size();
#endif
        SpiRamJsonDocument doc( filesize * 4 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            strncpy( printer3d_config.host, doc["host"], sizeof( printer3d_config.host ) );
            printer3d_config.port = (uint16_t)doc["port"];
        }
        doc.clear();
    }
    file.close();
}

void printer3d_save_config( void ) {
#ifdef NATIVE_64BIT
    std::fstream file;
    const char *homedir;
    char fileName[256] = "";
    char configFileName[] = PRINTER3D_JSON_CONFIG_FILE;
    /**
     * get home dir and build full path to file
     */
    if ( ( homedir = getenv("HOME") ) == NULL ) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    if ( configFileName[0] == '/')
        snprintf( fileName, sizeof( fileName ), "%s/.hedge/spiffs%s", homedir, configFileName );
    else
    {
        fileName[0] = '/';
        snprintf( fileName, sizeof( fileName ), "%s/.hedge/spiffs/%s", homedir, configFileName );
    }

    file.open( fileName, std::fstream::out );
#else
    fs::File file = SPIFFS.open( PRINTER3D_JSON_CONFIG_FILE, FILE_WRITE );
#endif
    if (!file) {
        log_e("Can't open file: %s!", PRINTER3D_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["host"] = printer3d_config.host;
        doc["port"] = printer3d_config.port;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}