/****************************************************************************
 *   June 14 02:01:00 2021
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

#include "kodi_remote_app.h"
#include "kodi_remote_app_main.h"
#include "kodi_remote_app_setup.h"
#include "kodi_remote_config.h"

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
/*
 * kodi remoate app config
 */
kodi_remote_config_t kodi_remote_config;
/*
 * app tiles
 */
uint32_t kodi_remote_app_main_tile_num;
uint32_t kodi_remote_app_setup_tile_num;
/*
 * app icon
 */
icon_t *kodi_remote_app = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(kodi_remote_app_64px);
/*
 * declare callback functions for the app and widget icon to enter the app
 */
static void enter_kodi_remote_app_event_cb( lv_obj_t * obj, lv_event_t event );
/*
 * automatic register the app setup function with explicit call in main.cpp
 */
static int registed = app_autocall_function( &kodi_remote_app_setup, 16 );           /** @brief app autocall function */
/*
 * setup routine for example app
 */
void kodi_remote_app_setup( void ) {
    /*
     * check if app already registered for autocall
     */
    if( !registed ) {
        return;
    }
    kodi_remote_load_config();

    /*
     * register 2 vertical tiles and get the first tile number and save it for later use
     */
    kodi_remote_app_main_tile_num = mainbar_add_app_tile( 2, 1, "kodi remote" );
    kodi_remote_app_setup_tile_num = mainbar_add_setup_tile( 1, 1, "kodi remote" );
    /*
     * register app icon on the app tile
     * set your own icon and register her callback to activate by an click
     * remember, an app icon must have an size of 64x64 pixel with an alpha channel
     * use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
     * the resulting c-file can put in /app/examples/images/ and declare it like LV_IMG_DECLARE( your_icon );
     */
    kodi_remote_app = app_register( "Kodi\nRemote", &kodi_remote_app_64px, enter_kodi_remote_app_event_cb );
    app_hide_indicator( kodi_remote_app );
    /*
     * init main and setup tile, see kodi_remote_app_main.cpp and kodi_remote_app_setup.cpp
     */
    kodi_remote_app_main_setup( kodi_remote_app_main_tile_num );
    kodi_remote_app_setup_setup( kodi_remote_app_setup_tile_num );
}
/**
 * @brief get the app main tile number
 * 
 * @return uint32_t 
 */
uint32_t kodi_remote_app_get_app_main_tile_num( void ) {
    return( kodi_remote_app_main_tile_num );
}
/**
 * @brief get the app setup tile number
 * 
 * @return uint32_t 
 */
uint32_t kodi_remote_app_get_app_setup_tile_num( void ) {
    return( kodi_remote_app_setup_tile_num );
}
/**
 * @brief call back function when enter the app
 * 
 * @param obj           app icon object
 * @param event         event type
 */
static void enter_kodi_remote_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( kodi_remote_app_main_tile_num, LV_ANIM_OFF, true );
                                        app_hide_indicator( kodi_remote_app );
                                        break;
    }    
}
/**
 * @brief set the app indicator
 * 
 * @param indicator 
 */
void kodi_remote_app_set_indicator(icon_indicator_t indicator) {
    app_set_indicator( kodi_remote_app, indicator );
}
/**
 * @brief hide the app indicator
 */
void kodi_remote_app_hide_indicator() {
    app_hide_indicator( kodi_remote_app );
}
/**
 * @brief get the config structure
 * 
 * @return kodi_remote_config_t* 
 */
kodi_remote_config_t *kodi_remote_get_config( void ) {
    return( &kodi_remote_config );
}
/**
 * @brief load the config
 */
void kodi_remote_load_config( void ) {
    kodi_remote_config.load();
}
/**
 * @brief save the config
 */
void kodi_remote_save_config( void ) {
    kodi_remote_config.save();
}