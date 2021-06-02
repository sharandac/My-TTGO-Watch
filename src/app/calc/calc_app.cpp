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

#include "calc_app.h"
#include "calc_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t calc_app_main_tile_num;

// app icon
icon_t *calc_app = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(calc_app_64px);

/*
 * setup routine for example app
 */
void calc_app_setup( void ) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    calc_app_main_tile_num = mainbar_add_app_tile( 1, 2, "calc app" );

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/ and declare it like LV_IMG_DECLARE( your_icon );
    calc_app = app_register( "Calculator", &calc_app_64px, enter_calc_app_event_cb );

    // init main and setup tile, see calc_app_main.cpp and calc_app_setup.cpp
    calc_app_main_setup( calc_app_main_tile_num );
}

/*
 *
 */
uint32_t calc_app_get_app_main_tile_num( void ) {
    return( calc_app_main_tile_num );
}

/*
 *
 */
void enter_calc_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( calc_app_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        app_hide_indicator( calc_app );
                                        break;
    }
}

/*
 *
 */
void exit_calc_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}