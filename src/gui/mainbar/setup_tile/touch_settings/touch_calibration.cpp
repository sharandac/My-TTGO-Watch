/****************************************************************************
 *   Tu May 22 21:23:51 2020
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
#include "touch_calibration.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/touch.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
#endif

lv_obj_t *touch_calibration_tile = NULL;
lv_obj_t *touch_location_icon = NULL;
lv_obj_t *touch_location_label = NULL;
lv_style_t touch_calibration_style;
uint32_t touch_calibration_tile_num;

float touch_calibration_x_scale = 0.0;
float touch_calibration_y_scale = 0.0;

uint32_t touch_calibration_state = CALIBRATION_STATE_NONE;
uint32_t touch_calibration_press_counter = 0;
float touch_calibration_min_x_value = 0.0;
float touch_calibration_min_y_value = 0.0;
float touch_calibration_max_x_value = 0.0;
float touch_calibration_max_y_value = 0.0;

LV_IMG_DECLARE(touch_64px);
LV_IMG_DECLARE(location_32px);

void touch_calibration_activate_cb( void );
void touch_calibration_hibernate_cb( void );
bool touch_calibration_event_cb( EventBits_t event, void *arg );

void touch_calibration_tile_setup( void ) {
    touch_calibration_tile_num = mainbar_add_app_tile( 1, 1, "touch calibration" );
    touch_calibration_tile = mainbar_get_tile_obj( touch_calibration_tile_num );

    lv_style_copy( &touch_calibration_style, ws_get_app_opa_style() );
    lv_style_set_bg_color( &touch_calibration_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_obj_add_style( touch_calibration_tile, LV_OBJ_PART_MAIN, &touch_calibration_style );

    touch_location_icon = wf_add_image( touch_calibration_tile, location_32px );
    lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0 );
    lv_obj_set_hidden(touch_location_icon, true );

    touch_location_label = lv_label_create( touch_calibration_tile, NULL);
    lv_label_set_text( touch_location_label, "calibration starts" );
    lv_obj_align( touch_location_label, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 );

    mainbar_add_tile_activate_cb( touch_calibration_tile_num, touch_calibration_activate_cb );
    mainbar_add_tile_hibernate_cb( touch_calibration_tile_num, touch_calibration_hibernate_cb );

    touch_register_cb( TOUCH_CONFIG_CHANGE | TOUCH_UPDATE, touch_calibration_event_cb, "touch config change");
}

uint32_t touch_calibration_get_tile_num( void ) {
    return( touch_calibration_tile_num );
}

void touch_calibration_activate_cb( void ) {
    touch_calibration_state = CALIBRATION_STATE_START;
    touch_calibration_press_counter = 2;
    lv_label_set_text( touch_location_label, "calibration starts" );
    lv_obj_align( touch_location_label, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 ); 
    lv_obj_set_hidden(touch_location_icon, true );
}

void touch_calibration_hibernate_cb( void ) {
    touch_calibration_state = CALIBRATION_STATE_NONE;    
    touch_calibration_press_counter = 2;
    lv_label_set_text( touch_location_label, "calibration starts" );
    lv_obj_align( touch_location_label, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 ); 
    lv_obj_set_hidden(touch_location_icon, true );
}

bool touch_calibration_event_cb( EventBits_t event, void *arg ) {
    static bool touch_pressed = false;

    switch( event ) {
        case TOUCH_CONFIG_CHANGE:
            break;
        case TOUCH_UPDATE:
            touch_t *touch = (touch_t *)arg;

            if( touch_pressed != touch->touched ) {
                if( touch->touched ) {
                    touch_calibration_press_counter++;

                    switch( touch_calibration_state ) {
                        case CALIBRATION_STATE_START:
                            touch_calibration_min_x_value = 0.0;
                            touch_calibration_min_y_value = 0.0;
                            touch_calibration_max_x_value = 0.0;
                            touch_calibration_max_y_value = 0.0;
                            break;
                        case CALIBRATION_STATE_BOTTOM_LEFT:
                            touch_calibration_min_x_value =+ touch->x_coor;
                            touch_calibration_max_y_value =+ touch->y_coor;
                            break;
                        case CALIBRATION_STATE_TOP_LEFT:
                            touch_calibration_min_x_value =+ touch->x_coor;
                            touch_calibration_min_y_value =+ touch->y_coor;
                            break;
                        case CALIBRATION_STATE_TOP_RIGHT:
                            touch_calibration_max_x_value =+ touch->x_coor;
                            touch_calibration_min_y_value =+ touch->y_coor;
                            break;
                        case CALIBRATION_STATE_BOTTOM_RIGHT:
                            touch_calibration_max_x_value =+ touch->x_coor;
                            touch_calibration_max_y_value =+ touch->y_coor;
                            break;
                        case CALIBRATION_STATE_DONE:
                            log_i("min x = %.1f", touch_calibration_min_x_value );
                            log_i("max x = %.1f", touch_calibration_max_x_value );
                            log_i("min y = %.1f", touch_calibration_min_y_value );
                            log_i("max y = %.1f", touch_calibration_max_y_value );
                            break;
                    }


                    if( touch_calibration_press_counter >= 3 ) {
                        touch_calibration_press_counter = 0;
                        touch_calibration_state++;
                    }

                    log_i("calibrate state %d\r\n", touch_calibration_state );

                    switch( touch_calibration_state ) {
                        case CALIBRATION_STATE_START:
                            lv_label_set_text( touch_location_label, "calibration starts" );
                            lv_obj_align( touch_location_label, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 ); 
                            lv_obj_set_hidden(touch_location_icon, true ); 
                            break;
                        case CALIBRATION_STATE_BOTTOM_LEFT:
                            lv_label_set_text( touch_location_label, "touch bottom left" );
                            lv_obj_align( touch_location_label, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 );
                            lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0 );
                            lv_obj_set_hidden(touch_location_icon, false ); 
                            break;
                        case CALIBRATION_STATE_TOP_LEFT:
                            lv_label_set_text( touch_location_label, "touch top left" );
                            lv_obj_align( touch_location_label, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 );
                            lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
                            lv_obj_set_hidden(touch_location_icon, false ); 
                            break;
                        case CALIBRATION_STATE_TOP_RIGHT:
                            lv_label_set_text( touch_location_label, "touch top left" );
                            lv_obj_align( touch_location_label, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 );
                            lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
                            lv_obj_set_hidden(touch_location_icon, false ); 
                            break;
                        case CALIBRATION_STATE_BOTTOM_RIGHT:
                            lv_label_set_text( touch_location_label, "touch bottom right" );
                            lv_obj_align( touch_location_label, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 );
                            lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0 );
                            lv_obj_set_hidden(touch_location_icon, false ); 
                            break;
                        case CALIBRATION_STATE_DONE:
                            lv_label_set_text( touch_location_label, "Done!" );
                            lv_obj_align( touch_location_label, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 );
                            lv_obj_set_hidden(touch_location_icon, true );
                            touch_calibration_press_counter = 2;
                            break;
                        case CALIBRATION_STATE_EXIT:
                            mainbar_jump_back();
                            break;
                        default:
                            touch_calibration_state = CALIBRATION_STATE_START;
                            break;
                    }
                }
                touch_pressed = touch->touched;
            }
            break;
    }
    return( false );
}
