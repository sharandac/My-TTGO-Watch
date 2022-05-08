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
#include "hardware/display.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
#endif

lv_obj_t *touch_calibration_tile = NULL;                    /** @brief touch calibration tile lv_obj_t pointer */
lv_obj_t *touch_location_icon = NULL;                       /** @brief touch calibration icon lv_obj_t pointer */
lv_obj_t *touch_location_done_arc = NULL;                   /** @brief touch calibration done arv lv_obj_t pointer*/
lv_obj_t *touch_location_done_icon = NULL;                  /** @brief touch calibration done icon lv_obj_t pointer*/
lv_style_t touch_calibration_style;                         /** @brief touch calibration lv_style_t pointer */
lv_style_t touch_calibration_arc_style;                     /** @brief touch calibration arv lv_style_t pointer */
uint32_t touch_calibration_tile_num;                        /** @brief touch calibration tile number */

bool touch_calibration_active = false;                      /** @brief touch calibration active flag */
float touch_calibration_x_scale = 0.0;                      /** @brief touch x scale */
float touch_calibration_y_scale = 0.0;                      /** @brief touch y scale */

uint32_t touch_calibration_state = CALIBRATION_STATE_NONE;  /** @brief calibration sequence state*/
uint32_t touch_calibration_press_counter = 0;               /** @brief calibration presse counter */
uint32_t display_rotation = 0;                              /** @brief old display rotation for restoration */
float touch_calibration_min_x_value = 0.0;                  /** @brief touch min x */
float touch_calibration_min_y_value = 0.0;                  /** @brief touch min y */
float touch_calibration_max_x_value = 0.0;                  /** @brief touch max x */
float touch_calibration_max_y_value = 0.0;                  /** @brief touch max y */

LV_IMG_DECLARE(touch_64px);
LV_IMG_DECLARE(location_32px);
LV_IMG_DECLARE(check_32px);

void touch_calibration_activate_cb( void );
void touch_calibration_hibernate_cb( void );
bool touch_calibration_event_cb( EventBits_t event, void *arg );

void touch_calibration_tile_setup( void ) {
    touch_calibration_tile_num = mainbar_add_app_tile( 1, 1, "touch calibration" );
    touch_calibration_tile = mainbar_get_tile_obj( touch_calibration_tile_num );

    lv_style_copy( &touch_calibration_style, ws_get_app_opa_style() );
    lv_style_set_bg_color( &touch_calibration_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_obj_add_style( touch_calibration_tile, LV_OBJ_PART_MAIN, &touch_calibration_style );

    touch_location_done_arc = lv_arc_create( touch_calibration_tile, NULL);
    lv_arc_set_bg_angles( touch_location_done_arc, 0, 0 );
    lv_arc_set_angles( touch_location_done_arc, 0, 360 );
    lv_arc_set_rotation( touch_location_done_arc, 90 );
    lv_arc_set_end_angle( touch_location_done_arc, 360 );
    lv_obj_align( touch_location_done_arc, NULL, LV_ALIGN_CENTER, 0, 0 );
    lv_style_copy( &touch_calibration_arc_style, ws_get_app_opa_style() );
    lv_style_set_bg_opa( &touch_calibration_style, LV_ARC_PART_INDIC, LV_OPA_TRANSP );
    lv_obj_add_style( touch_location_done_arc, LV_ARC_PART_BG, &touch_calibration_arc_style );

    touch_location_done_icon = wf_add_image( touch_calibration_tile, check_32px );
    lv_obj_align( touch_location_done_icon, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_hidden(touch_location_done_icon, true );

    touch_location_icon = wf_add_image( touch_calibration_tile, location_32px );
    lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_hidden(touch_location_icon, true );

    mainbar_add_tile_activate_cb( touch_calibration_tile_num, touch_calibration_activate_cb );
    mainbar_add_tile_hibernate_cb( touch_calibration_tile_num, touch_calibration_hibernate_cb );

    touch_register_cb( TOUCH_CONFIG_CHANGE | TOUCH_UPDATE, touch_calibration_event_cb, "touch config change");
}

uint32_t touch_calibration_get_tile_num( void ) {
    return( touch_calibration_tile_num );
}

void touch_calibration_activate_cb( void ) {
    display_rotation = display_get_rotation();
    display_set_rotation( 180 );
    touch_set_x_scale( 1.0 );
    touch_set_y_scale( 1.0 );
    touch_calibration_state = CALIBRATION_STATE_START;
    touch_calibration_press_counter = 2;
    lv_arc_set_end_angle( touch_location_done_arc, 360 );
    lv_obj_set_hidden( touch_location_icon, true );
    lv_obj_set_hidden(touch_location_done_icon, true );
    lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 );
    touch_calibration_active = true;
}

void touch_calibration_hibernate_cb( void ) {
    display_set_rotation( display_rotation );
    touch_calibration_state = CALIBRATION_STATE_NONE;    
    touch_calibration_press_counter = 2;
    lv_arc_set_end_angle( touch_location_done_arc, 360 );
    lv_obj_set_hidden( touch_location_icon, true );
    lv_obj_set_hidden(touch_location_done_icon, true );
    lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_CENTER, 0, 0 );
    touch_calibration_active = false;
}

bool touch_calibration_event_cb( EventBits_t event, void *arg ) {
    static bool touch_pressed = false;      /** @brief touch pressed flag */

    if( touch_calibration_active == false ) {
        return( false );
    }

    switch( event ) {
        case TOUCH_CONFIG_CHANGE:
            break;
        case TOUCH_UPDATE:
            touch_t *touch = (touch_t *)arg;

            if( touch_pressed != touch->touched ) {
                if( touch->touched ) {
                    touch_calibration_press_counter++;

                    lv_arc_set_end_angle( touch_location_done_arc, touch_calibration_press_counter * 30 + ( touch_calibration_state - 2 ) * 90 );

                    switch( touch_calibration_state ) {
                        case CALIBRATION_STATE_START:
                            touch_calibration_min_x_value = 0.0;
                            touch_calibration_min_y_value = 0.0;
                            touch_calibration_max_x_value = 0.0;
                            touch_calibration_max_y_value = 0.0;
                            break;
                        case CALIBRATION_STATE_BOTTOM_LEFT:
                            touch_calibration_min_x_value = touch_calibration_min_x_value + touch->x_coor;
                            touch_calibration_max_y_value = touch_calibration_max_y_value + touch->y_coor;
                            break;
                        case CALIBRATION_STATE_TOP_LEFT:
                            touch_calibration_min_x_value = touch_calibration_min_x_value + touch->x_coor;
                            touch_calibration_min_y_value = touch_calibration_min_y_value + touch->y_coor;
                            break;
                        case CALIBRATION_STATE_TOP_RIGHT:
                            touch_calibration_max_x_value = touch_calibration_max_x_value + touch->x_coor;
                            touch_calibration_min_y_value = touch_calibration_min_y_value + touch->y_coor;
                            break;
                        case CALIBRATION_STATE_BOTTOM_RIGHT:
                            touch_calibration_max_x_value = touch_calibration_max_x_value + touch->x_coor;
                            touch_calibration_max_y_value = touch_calibration_max_y_value + touch->y_coor;
                            break;
                        case CALIBRATION_STATE_DONE:
                            touch_calibration_min_x_value = touch_calibration_min_x_value / 6.0;
                            touch_calibration_min_y_value = touch_calibration_min_y_value / 6.0;
                            touch_calibration_max_x_value = touch_calibration_max_x_value / 6.0;
                            touch_calibration_max_y_value = touch_calibration_max_y_value / 6.0;

                            log_i("measured min x = %.1f", touch_calibration_min_x_value - ( lv_disp_get_hor_res( NULL ) / 2 ) );
                            log_i("measured max x = %.1f", touch_calibration_max_x_value - ( lv_disp_get_hor_res( NULL ) / 2 ) );
                            log_i("measured min y = %.1f", touch_calibration_min_y_value - ( lv_disp_get_ver_res( NULL ) / 2 ) );
                            log_i("measured max y = %.1f", touch_calibration_max_y_value - ( lv_disp_get_ver_res( NULL ) / 2 ) );
                            
                            log_i("calculated min x = %d", - ( lv_disp_get_hor_res( NULL ) / 2 ) + ( LOCATION_ICON_SIZE / 2 ) );
                            log_i("calculated max x = %d", ( lv_disp_get_hor_res( NULL ) / 2 ) - ( LOCATION_ICON_SIZE / 2 ) );
                            log_i("calculated min y = %d", - ( lv_disp_get_ver_res( NULL ) / 2 ) + ( LOCATION_ICON_SIZE / 2 ) );
                            log_i("calculated max y = %d", ( lv_disp_get_ver_res( NULL ) / 2 ) - ( LOCATION_ICON_SIZE / 2 ) );

                            touch_calibration_x_scale = ( - ( lv_disp_get_hor_res( NULL ) / 2 ) + ( LOCATION_ICON_SIZE / 2 ) ) / ( touch_calibration_min_x_value - ( lv_disp_get_hor_res( NULL ) / 2 ) );
                            touch_calibration_y_scale = ( - ( lv_disp_get_ver_res( NULL ) / 2 ) + ( LOCATION_ICON_SIZE / 2 ) ) / ( touch_calibration_min_y_value - ( lv_disp_get_ver_res( NULL ) / 2 ) );

                            touch_calibration_x_scale = touch_calibration_x_scale + ( ( lv_disp_get_hor_res( NULL ) / 2 ) - ( LOCATION_ICON_SIZE / 2 ) ) / ( touch_calibration_max_x_value - ( lv_disp_get_hor_res( NULL ) / 2 ) );
                            touch_calibration_y_scale = touch_calibration_y_scale + ( ( lv_disp_get_ver_res( NULL ) / 2 ) - ( LOCATION_ICON_SIZE / 2 ) ) / ( touch_calibration_max_y_value - ( lv_disp_get_ver_res( NULL ) / 2 ) );

                            touch_calibration_x_scale = touch_calibration_x_scale / 2.0;
                            touch_calibration_y_scale = touch_calibration_y_scale / 2.0;

                            log_i("x scale = %.2f", touch_calibration_x_scale );
                            log_i("y scale = %.2f", touch_calibration_y_scale );

                            if( touch_calibration_x_scale < 0.0 )
                                touch_calibration_x_scale = touch_calibration_x_scale * -1.0;

                            if( touch_calibration_y_scale < 0.0 )
                                touch_calibration_y_scale = touch_calibration_y_scale * -1.0;

                            if( touch_calibration_x_scale > 1.2 || touch_calibration_x_scale < 0.8) {
                                touch_calibration_x_scale = 1.0;
                            }

                            if( touch_calibration_y_scale > 1.2 || touch_calibration_y_scale < 0.8) {
                                touch_calibration_y_scale = 1.0;
                            }

                            break;
                    }

                    if( touch_calibration_press_counter >= 3 ) {
                        touch_calibration_press_counter = 0;
                        touch_calibration_state++;
                    }

                    switch( touch_calibration_state ) {
                        case CALIBRATION_STATE_START:
                            display_set_rotation( 180 );
                            lv_obj_set_hidden( touch_location_icon, true ); 
                            break;
                        case CALIBRATION_STATE_BOTTOM_LEFT:
                            lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0 );
                            lv_obj_set_hidden(touch_location_done_icon, true );
                            lv_obj_set_hidden( touch_location_icon, false ); 
                            break;
                        case CALIBRATION_STATE_TOP_LEFT:
                            lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
                            lv_obj_set_hidden(touch_location_done_icon, true );
                            lv_obj_set_hidden( touch_location_icon, false ); 
                            break;
                        case CALIBRATION_STATE_TOP_RIGHT:
                            lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
                            lv_obj_set_hidden(touch_location_done_icon, true );
                            lv_obj_set_hidden( touch_location_icon, false ); 
                            break;
                        case CALIBRATION_STATE_BOTTOM_RIGHT:
                            lv_obj_align( touch_location_icon, touch_calibration_tile, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0 );
                            lv_obj_set_hidden(touch_location_done_icon, true );
                            lv_obj_set_hidden( touch_location_icon, false ); 
                            break;
                        case CALIBRATION_STATE_DONE:
                            display_set_rotation( display_rotation );
                            lv_obj_set_hidden(touch_location_done_icon, false );
                            lv_obj_set_hidden( touch_location_icon, true );
                            touch_calibration_press_counter = 2;
                            break;
                        case CALIBRATION_STATE_EXIT:
                            touch_set_x_scale( touch_calibration_x_scale );
                            touch_set_y_scale( touch_calibration_y_scale );
                            log_i("x scale = %.2f", touch_calibration_x_scale );
                            log_i("y scale = %.2f", touch_calibration_y_scale );
                            break;
                        default:
                            touch_calibration_state = CALIBRATION_STATE_START;
                            break;
                    }
                }
                else {
                    if( touch_calibration_state == CALIBRATION_STATE_EXIT )
                        mainbar_jump_back();
                }
                touch_pressed = touch->touched;
            }
            break;
    }
    return( false );
}
