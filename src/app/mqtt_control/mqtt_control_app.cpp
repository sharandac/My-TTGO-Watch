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

#include "mqtt_control_app.h"
#include "mqtt_control_app_main.h"
#include "mqtt_control_app_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t mqtt_control_main_tile_num;
uint32_t mqtt_control_setup_tile_num;

// app icon
icon_t *mqtt_control_app = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(mqtt_control_64px);

// declare callback functions for the app and widget icon to enter the app
static void enter_mqtt_control_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for example app
 */
void mqtt_control_app_setup( void ) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    mqtt_control_main_tile_num = mainbar_add_app_tile( 1, 2, "mqtt control" );
    mqtt_control_setup_tile_num = mqtt_control_main_tile_num + 1;

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/ and declare it like LV_IMG_DECLARE( your_icon );
    mqtt_control_app = app_register( "MQTT\nControl", &mqtt_control_64px, enter_mqtt_control_event_cb );
    app_hide_indicator( mqtt_control_app );

    // init main and setup tile, see mqtt_control_main.cpp and mqtt_control_setup.cpp
    mqtt_control_main_setup( mqtt_control_main_tile_num );
    mqtt_control_setup_setup( mqtt_control_setup_tile_num );
}

/*
 *
 */
uint32_t mqtt_control_get_app_main_tile_num( void ) {
    return( mqtt_control_main_tile_num );
}

/*
 *
 */
uint32_t mqtt_control_get_app_setup_tile_num( void ) {
    return( mqtt_control_setup_tile_num );
}

/*
 *
 */
static void enter_mqtt_control_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( mqtt_control_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        app_hide_indicator( mqtt_control_app );
                                        break;
    }    
}

void mqtt_control_app_set_indicator(icon_indicator_t indicator) {
    app_set_indicator( mqtt_control_app, indicator );
}

void mqtt_control_app_hide_indicator() {
    app_hide_indicator( mqtt_control_app );
}
