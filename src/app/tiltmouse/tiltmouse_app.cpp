/****************************************************************************
 *   June 04 02:01:00 2021
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
#include <TTGO.h>

#include "tiltmouse_app.h"
#include "tiltmouse_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t tiltmouse_app_main_tile_num;

// app icon
icon_t *tiltmouse_app = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(tiltmouse_app_64px);

/*
 * setup routine for tiltmouse app
 */
void tiltmouse_app_setup( void ) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    tiltmouse_app_main_tile_num = mainbar_add_app_tile( 1, 1, "tiltmouse app" );

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/tiltmouse/images/ and declare it like LV_IMG_DECLARE( your_icon );
    tiltmouse_app = app_register( "Tilt Mouse", &tiltmouse_app_64px, enter_tiltmouse_app_event_cb );

    // init main and setup tile, see tiltmouse_app_main.cpp and tiltmouse_app_setup.cpp
    tiltmouse_app_main_setup( tiltmouse_app_main_tile_num );
}

/*
 *
 */
uint32_t tiltmouse_app_get_app_main_tile_num( void ) {
    return( tiltmouse_app_main_tile_num );
}

void enter_tiltmouse_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       tiltmouse_activate();
                                        mainbar_jump_to_tilenumber( tiltmouse_app_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        app_hide_indicator( tiltmouse_app );
                                        break;
    }    
}

void exit_tiltmouse_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       tiltmouse_deactivate();
                                        mainbar_jump_back();
                                        break;
    }
}
