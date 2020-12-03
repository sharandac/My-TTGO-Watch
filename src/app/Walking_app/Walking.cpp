/****************************************************************************
* Walking 
* 11/2020 by Neuroplant
*
*  Based on Example App 
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

#include "Walking.h"
#include "Walking_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"

uint32_t Walking_main_tile_num;
uint32_t Walking_setup_tile_num;

// app icon
icon_t *Walking = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(Walking_app_64px);
LV_IMG_DECLARE(info_1_16px);

// declare callback functions for the app and widget icon to enter the app
static void enter_Walking_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for Walking app
 */
void Walking_setup( void ) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    Walking_main_tile_num = mainbar_add_app_tile( 1, 1, "Walking app" );

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/Walkings/images/ and declare it like LV_IMG_DECLARE( your_icon );
    Walking = app_register( "Walking", &Walking_app_64px, enter_Walking_event_cb );

    // init main and setup tile, see Walking_main.cpp and Walking_setup.cpp
    Walking_main_setup( Walking_main_tile_num );
}

/*
 *
 */
uint32_t Walking_get_app_main_tile_num( void ) {
    return( Walking_main_tile_num );
}

/*
 *
 */
static void enter_Walking_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
										walking =true;
                                        mainbar_jump_to_tilenumber( Walking_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

