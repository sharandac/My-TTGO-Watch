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
#include "touch_settings.h"
#include "touch_calibration.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/touch.h"

lv_obj_t *touch_settings_tile = NULL;
lv_obj_t *calibrate_btn = NULL;
lv_obj_t *touch_scale_label = NULL;
lv_obj_t *touch_coor_label = NULL;

uint32_t touch_tile_num;

bool touch_active = false;
float touch_x_scale = 0.0;
float touch_y_scale = 0.0;

LV_IMG_DECLARE(touch_64px);
LV_IMG_DECLARE(location_32px);

void touch_settings_activate_cb( void );
void touch_settings_hibernate_cb( void );
bool touch_settings_event_cb( EventBits_t event, void *arg );
static void touch_settings_calibration_btn_cb( lv_obj_t * obj, lv_event_t event );
static void enter_touch_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_touch_setup_event_cb( lv_obj_t * obj, lv_event_t event );

void touch_settings_tile_setup( void ) {
    /**
     * create settings tile, all widgets, styles and cb
     */
    touch_tile_num = mainbar_add_setup_tile( 1, 1, "touch settings" );
    touch_settings_tile = mainbar_get_tile_obj( touch_tile_num );

    lv_obj_add_style( touch_settings_tile, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );

    icon_t *touch_setup_icon = setup_register( "touch", &touch_64px, enter_touch_setup_event_cb );
    setup_hide_indicator( touch_setup_icon );

    lv_obj_t *header = wf_add_settings_header( touch_settings_tile, "touch settings", exit_touch_setup_event_cb );
    lv_obj_align( header, touch_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );

    calibrate_btn = lv_btn_create( touch_settings_tile, NULL);
    lv_obj_set_event_cb( calibrate_btn, touch_settings_calibration_btn_cb );
    lv_obj_add_style( calibrate_btn, LV_BTN_PART_MAIN, ws_get_button_style() );
    lv_obj_align( calibrate_btn, header, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_obj_t *calibrate_btn_label = lv_label_create( calibrate_btn, NULL );
    lv_label_set_text( calibrate_btn_label, "calibrate");

    touch_scale_label = lv_label_create( touch_settings_tile, NULL);
    lv_obj_add_style( touch_scale_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( touch_scale_label, "" );
    lv_obj_align( touch_scale_label, calibrate_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    touch_coor_label = lv_label_create( touch_settings_tile, NULL);
    lv_obj_add_style( touch_coor_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( touch_coor_label, "" );
    lv_obj_align( touch_coor_label, touch_settings_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -5 );

    mainbar_add_tile_activate_cb( touch_tile_num, touch_settings_activate_cb );
    mainbar_add_tile_hibernate_cb( touch_tile_num, touch_settings_hibernate_cb );

    touch_register_cb( TOUCH_CONFIG_CHANGE | TOUCH_UPDATE, touch_settings_event_cb, "touch config change");
    /**
     * create touch calibration tile and call calibration tile setup
     */
    touch_calibration_tile_setup();
}

void touch_settings_activate_cb( void ) {
    touch_x_scale = touch_get_x_scale();
    touch_y_scale = touch_get_y_scale();

    char scale_label[64]="";
    snprintf( scale_label, sizeof( scale_label ), "x/y scale: %.2f/%.2f", touch_x_scale, touch_y_scale );
    lv_label_set_text( touch_scale_label, scale_label );
    lv_obj_align( touch_scale_label, calibrate_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    touch_active = true;
}

void touch_settings_hibernate_cb( void ) {
    touch_x_scale = touch_get_x_scale();
    touch_y_scale = touch_get_y_scale();

    char scale_label[64]="";
    snprintf( scale_label, sizeof( scale_label ), "x/y scale: %.2f/%.2f", touch_x_scale, touch_y_scale );
    lv_label_set_text( touch_scale_label, scale_label );
    lv_obj_align( touch_scale_label, calibrate_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    touch_active = false;
}

bool touch_settings_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case TOUCH_CONFIG_CHANGE:
            touch_x_scale = touch_get_x_scale();
            touch_y_scale = touch_get_y_scale();
            break;
        case TOUCH_UPDATE:
            if( touch_active ) {
                touch_t *touch = (touch_t*)arg;
                char touch_label[64]="";
                snprintf( touch_label, sizeof( touch_label ), "x/y coor: %d/%d/%s", touch->x_coor, touch->y_coor, touch->touched ? "pressed":"release" );
                lv_label_set_text( touch_coor_label, touch_label );
                lv_obj_align( touch_coor_label, touch_settings_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -5 );
            }
            break;
    }
    return( false );
}

static void touch_settings_calibration_btn_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mainbar_jump_to_tilenumber( touch_calibration_get_tile_num(), LV_ANIM_OFF );
            statusbar_hide( true );
            break;
    }
}

static void enter_touch_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
            mainbar_jump_to_tilenumber( touch_tile_num, LV_ANIM_OFF );
            touch_x_scale = touch_get_x_scale();
            touch_y_scale = touch_get_y_scale();
            char scale_label[64]="";
            snprintf( scale_label, sizeof( scale_label ), "x/y scale: %.2f/%.2f", touch_x_scale, touch_y_scale );
            lv_label_set_text( touch_scale_label, scale_label );
            lv_obj_align( touch_scale_label, calibrate_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            touch_active = true;
            break;
    }
}

static void exit_touch_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
            mainbar_jump_back();
            touch_set_x_scale( touch_x_scale );
            touch_set_y_scale( touch_y_scale );
            touch_active = false;
            break;
    }
}
