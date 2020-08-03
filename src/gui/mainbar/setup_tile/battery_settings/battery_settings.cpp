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
#include "battery_settings.h"
#include "battery_view.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup.h"
#include "gui/statusbar.h"
#include "hardware/display.h"
#include "hardware/motor.h"
#include "hardware/pmu.h"

lv_obj_t *battery_settings_tile=NULL;
lv_style_t battery_settings_style;
uint32_t battery_settings_tile_num;

lv_obj_t *battery_setup_icon_cont = NULL;
lv_obj_t *battery_setup_info_img = NULL;

lv_obj_t *battery_percent_switch = NULL;
lv_obj_t *battery_experimental_switch = NULL;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(battery_icon_64px);
LV_IMG_DECLARE(info_update_16px);

static void enter_battery_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_battery_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void battery_percent_switch_event_handler( lv_obj_t * obj, lv_event_t event );
static void battery_experimental_switch_event_handler( lv_obj_t * obj, lv_event_t event );

void battery_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    battery_settings_tile_num = mainbar_add_app_tile( 1, 2 );
    battery_settings_tile = mainbar_get_tile_obj( battery_settings_tile_num + 1 );

    battery_view_tile_setup( battery_settings_tile_num );

    lv_style_copy( &battery_settings_style, mainbar_get_style() );
    lv_style_set_bg_color( &battery_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &battery_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &battery_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( battery_settings_tile, LV_OBJ_PART_MAIN, &battery_settings_style );

    // register an setup icon an set an callback
    battery_setup_icon_cont = setup_tile_register_setup();
    lv_obj_t *battery_setup = lv_imgbtn_create ( battery_setup_icon_cont, NULL);
    lv_imgbtn_set_src( battery_setup, LV_BTN_STATE_RELEASED, &battery_icon_64px);
    lv_imgbtn_set_src( battery_setup, LV_BTN_STATE_PRESSED, &battery_icon_64px);
    lv_imgbtn_set_src( battery_setup, LV_BTN_STATE_CHECKED_RELEASED, &battery_icon_64px);
    lv_imgbtn_set_src( battery_setup, LV_BTN_STATE_CHECKED_PRESSED, &battery_icon_64px);
    lv_obj_add_style( battery_setup, LV_IMGBTN_PART_MAIN,  mainbar_get_style() );
    lv_obj_align( battery_setup, NULL, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_event_cb( battery_setup, enter_battery_setup_event_cb );

    battery_setup_info_img = lv_img_create( battery_setup_icon_cont, NULL );
    lv_img_set_src( battery_setup_info_img, &info_update_16px );
    lv_obj_align( battery_setup_info_img, battery_setup_icon_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( battery_setup_info_img, true );

    lv_obj_t *exit_btn = lv_imgbtn_create( battery_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &battery_settings_style );
    lv_obj_align( exit_btn, battery_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_battery_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( battery_settings_tile, NULL );
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &battery_settings_style  );
    lv_label_set_text( exit_label, "battery settings");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *battery_setup_label_cont = lv_obj_create( battery_settings_tile, NULL );
    lv_obj_set_size(battery_setup_label_cont, LV_HOR_RES_MAX , 40);
    lv_obj_add_style( battery_setup_label_cont, LV_OBJ_PART_MAIN, &battery_settings_style  );
    lv_obj_align( battery_setup_label_cont, battery_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    lv_obj_t *battery_setup_label = lv_label_create( battery_setup_label_cont, NULL);
    lv_obj_add_style( battery_setup_label, LV_OBJ_PART_MAIN, &battery_settings_style  );
    lv_label_set_text( battery_setup_label, "experimental functions");
    lv_obj_align( battery_setup_label, battery_setup_label_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *battery_percent_switch_cont = lv_obj_create( battery_settings_tile, NULL );
    lv_obj_set_size(battery_percent_switch_cont, LV_HOR_RES_MAX , 40);
    lv_obj_add_style( battery_percent_switch_cont, LV_OBJ_PART_MAIN, &battery_settings_style  );
    lv_obj_align( battery_percent_switch_cont, battery_setup_label_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    battery_percent_switch = lv_switch_create( battery_percent_switch_cont, NULL );
    lv_obj_add_protect( battery_percent_switch, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( battery_percent_switch, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( battery_percent_switch, LV_ANIM_ON );
    lv_obj_align( battery_percent_switch, battery_percent_switch_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( battery_percent_switch, battery_percent_switch_event_handler );
    lv_obj_t *stepcounter_label = lv_label_create( battery_percent_switch_cont, NULL);
    lv_obj_add_style( stepcounter_label, LV_OBJ_PART_MAIN, &battery_settings_style  );
    lv_label_set_text( stepcounter_label, "calculated percent");
    lv_obj_align( stepcounter_label, battery_percent_switch_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *battery_experimental_switch_cont = lv_obj_create( battery_settings_tile, NULL );
    lv_obj_set_size(battery_experimental_switch_cont, LV_HOR_RES_MAX , 40);
    lv_obj_add_style( battery_experimental_switch_cont, LV_OBJ_PART_MAIN, &battery_settings_style  );
    lv_obj_align( battery_experimental_switch_cont, battery_percent_switch_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    battery_experimental_switch = lv_switch_create( battery_experimental_switch_cont, NULL );
    lv_obj_add_protect( battery_experimental_switch, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( battery_experimental_switch, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( battery_experimental_switch, LV_ANIM_ON );
    lv_obj_align( battery_experimental_switch, battery_experimental_switch_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( battery_experimental_switch, battery_experimental_switch_event_handler );
    lv_obj_t *doubleclick_label = lv_label_create( battery_experimental_switch_cont, NULL);
    lv_obj_add_style( doubleclick_label, LV_OBJ_PART_MAIN, &battery_settings_style  );
    lv_label_set_text( doubleclick_label, "power save");
    lv_obj_align( doubleclick_label, battery_experimental_switch_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    if ( pmu_get_calculated_percent() )
        lv_switch_on( battery_percent_switch, LV_ANIM_OFF);
    else
        lv_switch_off( battery_percent_switch, LV_ANIM_OFF);

    if ( pmu_get_experimental_power_save() ) {
        lv_switch_on( battery_experimental_switch, LV_ANIM_OFF);
        lv_obj_set_hidden( battery_setup_info_img, false );
    }
    else
        lv_switch_off( battery_experimental_switch, LV_ANIM_OFF);
}

static void battery_percent_switch_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): pmu_set_calculated_percent( lv_switch_get_state( obj ) );
                                        break;
    }
}

static void battery_experimental_switch_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): pmu_set_experimental_power_save( lv_switch_get_state( obj ) );
                                        if ( pmu_get_experimental_power_save() )
                                            lv_obj_set_hidden( battery_setup_info_img, false );
                                        else
                                            lv_obj_set_hidden( battery_setup_info_img, true );
                                        break;
    }
}

static void enter_battery_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( battery_settings_tile_num, LV_ANIM_OFF );
                                        break;
    }

}

static void exit_battery_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( battery_settings_tile_num,  LV_ANIM_OFF );
                                        break;
    }
}
