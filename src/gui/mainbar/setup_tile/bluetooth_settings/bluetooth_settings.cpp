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
#include "bluetooth_settings.h"
#include "bluetooth_pairing.h"
#include "bluetooth_call.h"
#include "bluetooth_message.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"

#include "hardware/blectl.h"

icon_t *bluettoth_setup_icon = NULL;

lv_obj_t *bluetooth_settings_tile=NULL;
lv_style_t bluetooth_settings_style;
uint32_t bluetooth_tile_num;

lv_obj_t *bluetooth_standby_onoff = NULL;
lv_obj_t *bluetooth_advertising_onoff = NULL;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(bluetooth_64px);
LV_IMG_DECLARE(info_fail_16px);

static void enter_bluetooth_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void bluetooth_standby_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void bluetooth_advertising_onoff_event_handler(lv_obj_t * obj, lv_event_t event);

void bluetooth_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    bluetooth_tile_num = mainbar_add_app_tile( 1, 1 );
    bluetooth_settings_tile = mainbar_get_tile_obj( bluetooth_tile_num );

    lv_style_copy( &bluetooth_settings_style, mainbar_get_style() );
    lv_style_set_bg_color( &bluetooth_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &bluetooth_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &bluetooth_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( bluetooth_settings_tile, LV_OBJ_PART_MAIN, &bluetooth_settings_style );

    bluettoth_setup_icon = setup_register( "bluetooth", &bluetooth_64px, enter_bluetooth_setup_event_cb );
    setup_hide_indicator( bluettoth_setup_icon );

    lv_obj_t *exit_btn = lv_imgbtn_create( bluetooth_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &bluetooth_settings_style );
    lv_obj_align( exit_btn, bluetooth_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_bluetooth_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( bluetooth_settings_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &bluetooth_settings_style  );
    lv_label_set_text( exit_label, "bluetooth settings");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *bluetooth_advertising_cont = lv_obj_create( bluetooth_settings_tile, NULL );
    lv_obj_set_size( bluetooth_advertising_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( bluetooth_advertising_cont, LV_OBJ_PART_MAIN, &bluetooth_settings_style  );
    lv_obj_align( bluetooth_advertising_cont, bluetooth_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    bluetooth_advertising_onoff = lv_switch_create( bluetooth_advertising_cont, NULL );
    lv_obj_add_protect( bluetooth_advertising_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( bluetooth_advertising_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( bluetooth_advertising_onoff, LV_ANIM_ON );
    lv_obj_align( bluetooth_advertising_onoff, bluetooth_advertising_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( bluetooth_advertising_onoff, bluetooth_advertising_onoff_event_handler );
    lv_obj_t *bluetooth_advertising_label = lv_label_create( bluetooth_advertising_cont, NULL);
    lv_obj_add_style( bluetooth_advertising_label, LV_OBJ_PART_MAIN, &bluetooth_settings_style  );
    lv_label_set_text( bluetooth_advertising_label, "visibility");
    lv_obj_align( bluetooth_advertising_label, bluetooth_advertising_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *bluettoth_info_label_cont = lv_obj_create( bluetooth_settings_tile, NULL );
    lv_obj_set_size(bluettoth_info_label_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( bluettoth_info_label_cont, LV_OBJ_PART_MAIN, &bluetooth_settings_style  );
    lv_obj_align( bluettoth_info_label_cont, bluetooth_advertising_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *bluetooth_info_label = lv_label_create( bluettoth_info_label_cont, NULL);
    lv_obj_add_style( bluetooth_info_label, LV_OBJ_PART_MAIN, &bluetooth_settings_style  );
    lv_label_set_text( bluetooth_info_label, "increases power consumption");
    lv_obj_align( bluetooth_info_label, bluettoth_info_label_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    
    lv_obj_t *bluetooth_standby_cont = lv_obj_create( bluetooth_settings_tile, NULL );
    lv_obj_set_size( bluetooth_standby_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( bluetooth_standby_cont, LV_OBJ_PART_MAIN, &bluetooth_settings_style  );
    lv_obj_align( bluetooth_standby_cont, bluettoth_info_label_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    bluetooth_standby_onoff = lv_switch_create( bluetooth_standby_cont, NULL );
    lv_obj_add_protect( bluetooth_standby_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( bluetooth_standby_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( bluetooth_standby_onoff, LV_ANIM_ON );
    lv_obj_align( bluetooth_standby_onoff, bluetooth_standby_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( bluetooth_standby_onoff, bluetooth_standby_onoff_event_handler );
    lv_obj_t *bluetooth_standby_label = lv_label_create( bluetooth_standby_cont, NULL);
    lv_obj_add_style( bluetooth_standby_label, LV_OBJ_PART_MAIN, &bluetooth_settings_style  );
    lv_label_set_text( bluetooth_standby_label, "always on");
    lv_obj_align( bluetooth_standby_label, bluetooth_standby_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    if ( blectl_get_advertising() ) {
        lv_switch_on( bluetooth_advertising_onoff, LV_ANIM_OFF );
    }
    else {
        lv_switch_off( bluetooth_advertising_onoff, LV_ANIM_OFF );
    }

    if ( blectl_get_enable_on_standby() ) {
        setup_set_indicator( bluettoth_setup_icon, ICON_INDICATOR_FAIL );
        lv_switch_on( bluetooth_standby_onoff, LV_ANIM_OFF );
    }
    else {
        lv_switch_off( bluetooth_standby_onoff, LV_ANIM_OFF );
    }

    bluetooth_pairing_tile_setup();
    bluetooth_call_tile_setup();
    bluetooth_message_tile_setup();
}

static void enter_bluetooth_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( bluetooth_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_bluetooth_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
                                        break;
    }
}

static void bluetooth_advertising_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED): blectl_set_advertising( lv_switch_get_state( obj ) );
    }
}

static void bluetooth_standby_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED): blectl_set_enable_on_standby( lv_switch_get_state( obj ) );
                                        if( lv_switch_get_state( obj ) ) {
                                            setup_set_indicator( bluettoth_setup_icon, ICON_INDICATOR_FAIL );        
                                        }
                                        else {
                                            setup_hide_indicator( bluettoth_setup_icon );
                                        }
    }
}
