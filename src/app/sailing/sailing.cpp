/****************************************************************************
 *   Apr 17 00:28:11 2021
 *   Copyright  2021  Federico Liuzzi
 *   Email: f.liuzzi02@gmail.com
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

#include "sailing.h"
#include "sailing_main.h"
#include "sailing_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
#endif

uint32_t sailing_main_tile_num;
uint32_t sailing_setup_tile_num;

// app icon
icon_t *sailing = NULL;


// declare you images or fonts you need
LV_IMG_DECLARE(sailing_64px);
LV_IMG_DECLARE(info_1_16px);

// declare callback functions for the app and widget icon to enter the app
static void enter_sailing_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for example app
 */
void sailing_setup( void ) {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif
    // register 2 vertical tiles and get the first tile number and save it for later use
    sailing_main_tile_num = mainbar_add_app_tile( 1, 1, "Sailing" );
    sailing_setup_tile_num = mainbar_add_setup_tile( 1, 1, "Sailing" );

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/ and declare it like LV_IMG_DECLARE( your_icon );
    sailing = app_register( "Sailing", &sailing_64px, enter_sailing_event_cb );
    app_set_indicator( sailing, ICON_INDICATOR_OK );

    // init main and setup tile, see sailing_main.cpp and sailing_setup.cpp
    sailing_main_setup( sailing_main_tile_num );
    sailing_setup_setup( sailing_setup_tile_num );
}

/*
 *
 */
uint32_t sailing_get_app_main_tile_num( void ) {
    return( sailing_main_tile_num );
}

/*
 *
 */
uint32_t sailing_get_app_setup_tile_num( void ) {
    return( sailing_setup_tile_num );
}

/*
 *
 */
static void enter_sailing_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( false );
                                        app_hide_indicator( sailing );
                                        mainbar_jump_to_tilenumber( sailing_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}
