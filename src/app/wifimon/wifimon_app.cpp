/******************************************************************************
 *   linuxthor 2020 
 *   ttgo watch wifimonitor toy based on information/code from Łukasz Podkalicki
 ******************************************************************************/
 
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

#include "wifimon_app.h"
#include "wifimon_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
#endif
/*
 * app tiles
 */
uint32_t wifimon_app_main_tile_num;
uint32_t wifimon_app_setup_tile_num;
/*
 * app icon
 */
icon_t *wifimon_app = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(wifimon_app_64px);
LV_IMG_DECLARE(info_1_16px);
/*
 * declare callback functions for the app and widget icon to enter the app
 */
static void enter_wifimon_app_event_cb( lv_obj_t * obj, lv_event_t event );
/*
 * automatic register the app setup function with explicit call in main.cpp
 */
static int registed = app_autocall_function( &wifimon_app_setup, 14 );           /** @brief app autocall function */
/*
 * setup routine for wifimon app
 */
void wifimon_app_setup( void ) {
    /*
     * check if app already registered for autocall
     */
    if( !registed ) {
        return;
    }
    wifimon_app_main_tile_num = mainbar_add_app_tile( 1, 1, "wifimon app" );
    wifimon_app = app_register( "wifi\nmon", &wifimon_app_64px, enter_wifimon_app_event_cb );
    wifimon_app_main_setup( wifimon_app_main_tile_num );
}
/**
 * @brief get the app main tile number
 * 
 * @return uint32_t 
 */
uint32_t wifimon_app_get_app_main_tile_num( void ) {
    return( wifimon_app_main_tile_num );
}
/**
 * @brief callback function to enter the app
 * 
 * @param obj           pointer to the object
 * @param event         the event
 */
static void enter_wifimon_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       app_hide_indicator( wifimon_app );
                                        mainbar_jump_to_tilenumber( wifimon_app_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        break;
    }    
}
