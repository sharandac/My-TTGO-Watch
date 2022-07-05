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

#include "compass_app.h"
#include "compass_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget_styles.h"
#include "gui/widget_factory.h"

#include "hardware/compass.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
    #include "gui/mainbar/setup_tile/watchface/watchface_tile.h"
#endif
/**
 * local lv obj 
 */
lv_obj_t *compass_exit_btn = NULL;
lv_obj_t * compass_needle_img = NULL;
/**
 * call back functions
 */
static bool compass_app_main_update_event_cb( EventBits_t event, void *arg );
static void compass_app_main_activate_cb( void );
static void compass_app_main_hibernate_cb( void );
static bool compass_app_main_button_cb( EventBits_t event, void *arg );
static bool compass_app_main_compass_event_cb( EventBits_t event, void *arg );
/*
 *
 */
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);
LV_IMG_DECLARE(compass_needle_200px);
/*
 * setup routine for wifimon app
 */
void compass_app_main_setup( uint32_t tile ) {
    /**
     * add exit, menu and setup button to the main app tile
     */
    compass_exit_btn = wf_add_exit_button( mainbar_get_tile_obj( tile ) );
    lv_obj_align( compass_exit_btn, mainbar_get_tile_obj( tile ), LV_ALIGN_IN_BOTTOM_LEFT, THEME_PADDING, -THEME_PADDING );

    compass_needle_img = lv_img_create( mainbar_get_tile_obj( tile ), NULL);
    lv_img_set_src( compass_needle_img, &compass_needle_200px);
    lv_obj_align( compass_needle_img, NULL, LV_ALIGN_CENTER, 0, 0 );

    wf_label_printf( wf_add_label( mainbar_get_tile_obj( tile ), "", APP_ICON_LABEL_STYLE ), mainbar_get_tile_obj( tile ), LV_ALIGN_IN_TOP_MID, 0, THEME_PADDING, "N" );
    wf_label_printf( wf_add_label( mainbar_get_tile_obj( tile ), "", APP_ICON_LABEL_STYLE ), mainbar_get_tile_obj( tile ), LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0, "E" );
    wf_label_printf( wf_add_label( mainbar_get_tile_obj( tile ), "", APP_ICON_LABEL_STYLE ), mainbar_get_tile_obj( tile ), LV_ALIGN_IN_BOTTOM_MID, 0, -THEME_PADDING, "S" );
    wf_label_printf( wf_add_label( mainbar_get_tile_obj( tile ), "", APP_ICON_LABEL_STYLE ), mainbar_get_tile_obj( tile ), LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0, "W" );
    
    compass_register_cb( COMPASS_UPDATE, compass_app_main_update_event_cb, "compass updates");
    mainbar_add_tile_activate_cb( tile, compass_app_main_activate_cb );
    mainbar_add_tile_hibernate_cb( tile, compass_app_main_hibernate_cb );
    mainbar_add_tile_button_cb( tile, compass_app_main_button_cb );
}

static bool compass_app_main_update_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case COMPASS_UPDATE: {
            compass_data_t *compass_data = (compass_data_t*)arg;
            lv_img_set_angle( compass_needle_img, compass_data->azimuth * 10 );
            break;
        }
    }
    return( true );
}

/**
 * @brief call back function for button if the current tile active
 * 
 * @param event         event like BUTTON_LEFT, BUTTON_RIGHT, ...
 * @param arg           here like NULL
 * @return true 
 * @return false 
 */
static bool compass_app_main_button_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_EXIT:
            mainbar_jump_back();
            break;
    }
    return( true );
}
/**
 * @brief call back function if the current tile activate
 * 
 */
static void compass_app_main_activate_cb( void ) {
    compass_on();
    compass_start_calibration();
}
/**
 * @brief call back function if the current tile hibernate
 * 
 */
static void compass_app_main_hibernate_cb( void ) {
    compass_off();
}
