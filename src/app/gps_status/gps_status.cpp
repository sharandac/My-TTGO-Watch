/****************************************************************************
 *   Apr 13 14:17:11 2021
 *   Copyright  2021  Cornelius Wild
 *   Email: tt-watch-code@dervomsee.de
 *   Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  Example_App"
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

#include "gps_status.h"
#include "gps_status_main.h"
#include "gps_status_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else

#endif

uint32_t gps_status_main_tile_num;
uint32_t gps_status_setup_tile_num;

// app icon
icon_t *gps_status = NULL;

// widget icon
icon_t *gps_status_widget = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(gps_status_64px);
LV_IMG_DECLARE(info_1_16px);

// declare callback functions for the app and widget icon to enter the app
static void enter_gps_status_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for example app
 */
void gps_status_setup( void ) {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif
    // register 2 vertical tiles and get the first tile number and save it for later use
    gps_status_main_tile_num = mainbar_add_app_tile( 1, 1, "gps status" );
    gps_status_setup_tile_num = mainbar_add_setup_tile( 1, 1, "gps status" );

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/ and declare it like LV_IMG_DECLARE( your_icon );
    gps_status = app_register( "gps status", &gps_status_64px, enter_gps_status_event_cb );

    // init main and setup tile, see gps_status_main.cpp and gps_status_setup.cpp
    gps_status_main_setup( gps_status_main_tile_num );
    gps_status_setup_setup( gps_status_setup_tile_num );
}

/*
 *
 */
uint32_t gps_status_get_app_main_tile_num( void ) {
    return( gps_status_main_tile_num );
}

/*
 *
 */
uint32_t gps_status_get_app_setup_tile_num( void ) {
    return( gps_status_setup_tile_num );
}

/*
 *
 */
static void enter_gps_status_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( gps_status_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

