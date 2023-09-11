/****************************************************************************
 *   Aug 11 17:13:51 2020
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

#include "tracker_app.h"
#include "tracker_app_main.h"
#include "tracker_app_view.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget_styles.h"
#include "gui/widget_factory.h"


#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
#endif

lv_obj_t *tracker_app_main_tile = NULL;
lv_obj_t *tracker_app_view_tile = NULL;
uint32_t tracker_app_main_tile_num;
uint32_t tracker_app_view_tile_num;
/*
 * app icon
 */
icon_t *tracker_app = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(tracker_64px);
LV_IMG_DECLARE(info_1_16px);
/*
 * declare callback functions for the app 
 */
static void tracker_enter_app_event_cb( lv_obj_t * obj, lv_event_t event );
/*
 * automatic register the app setup function with explicit call in main.cpp
 */
static int registed = app_autocall_function( &tracker_app_setup, 4 );           /** @brief app autocall function */
/*
 * setup routine for wifimon app
 */
void tracker_app_setup( void ) {
    /*
     * check if app already registered for autocall
     */
    if( !registed ) {
        return;
    }
    /*
     * allow the app depending on the hardware
     */
    #if defined( M5PAPER ) || defined( M5CORE2 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( NATIVE_64BIT )
        tracker_app_main_tile_num = mainbar_add_app_tile( 2, 1, "gps tracker" );
        tracker_app_view_tile_num = tracker_app_main_tile_num + 1;

        tracker_app_main_tile = mainbar_get_tile_obj( tracker_app_main_tile_num );
        tracker_app_view_tile = mainbar_get_tile_obj( tracker_app_view_tile_num );

        tracker_app = app_register( "gps tracker", &tracker_64px, tracker_enter_app_event_cb );

        tracker_app_main_setup( tracker_app_main_tile_num );
        tracker_app_view_setup( tracker_app_view_tile_num );
    #endif
}
/**
 * @brief get the app tile number
 * 
 * @return uint32_t 
 */
uint32_t tracker_app_get_app_main_tile_num( void ) {
    return( tracker_app_main_tile_num );
}
/**
 * @brief get the app stats view tile number
 * 
 * @return uint32_t 
 */
uint32_t tracker_app_get_app_view_tile_num( void ) {
    return( tracker_app_view_tile_num );
}
/**
 * @brief call back function when enter the app
 * 
 * @param obj           pointer to the object
 * @param event         the event
 */
static void tracker_enter_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       app_hide_indicator( tracker_app );
                                        mainbar_jump_to_tilenumber( tracker_app_main_tile_num, LV_ANIM_OFF, true );
                                        break;
    }    
}
