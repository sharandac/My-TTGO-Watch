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

#include "prayer_app.h"
#include "prayer_app_main.h"
#include "prayer_app_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t prayer_app_main_tile_num;
uint32_t prayer_app_setup_tile_num;

// app icon
icon_t *prayer_app = NULL;

// widget icon
icon_t *prayer_widget = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(mosconi_64px);
LV_IMG_DECLARE(info_1_16px);

// declare callback functions for the app and widget icon to enter the app
static void enter_prayer_app_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_prayer_widget_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for example app
 */
void prayer_app_setup( void ) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    prayer_app_main_tile_num = mainbar_add_app_tile( 1, 2, "Preghiere" );
    prayer_app_setup_tile_num = prayer_app_main_tile_num + 1;

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/ and declare it like LV_IMG_DECLARE( your_icon );
    prayer_app = app_register( "prega", &mosconi_64px, enter_prayer_app_event_cb );
    app_set_indicator( prayer_app, ICON_INDICATOR_OK );

#ifdef prayer_widget
    // register widget icon on the main tile
    // set your own icon and register her callback to activate by an click
    // remember, an widget icon must have an max size of 64x64 pixel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/ and declare it like LV_IMG_DECLARE( your_icon );
    prayer_widget = widget_register( "myapp", &prayer_app_64px, enter_prayer_widget_event_cb );
    widget_set_indicator( prayer_widget, ICON_INDICATOR_UPDATE );
#endif // prayer_widget

    // init main and setup tile, see prayer_app_main.cpp and prayer_app_setup.cpp
    prayer_app_main_setup( prayer_app_main_tile_num );
    prayer_app_setup_setup( prayer_app_setup_tile_num );
}

/*
 *
 */
uint32_t prayer_app_get_app_main_tile_num( void ) {
    return( prayer_app_main_tile_num );
}

/*
 *
 */
uint32_t prayer_app_get_app_setup_tile_num( void ) {
    return( prayer_app_setup_tile_num );
}

/*
 *
 */
static void enter_prayer_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        app_hide_indicator( prayer_app );
                                        porcodio();
                                        mainbar_jump_to_tilenumber( prayer_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

/*
 *
 */
static void enter_prayer_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        widget_hide_indicator( prayer_widget );
                                        mainbar_jump_to_tilenumber( prayer_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}
