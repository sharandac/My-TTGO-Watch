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
#include "wlan_settings.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"

#include "hardware/wifictl.h"
#include "hardware/motor.h"

#include <WiFi.h>

LV_IMG_DECLARE(exit_32px);

lv_obj_t *wifi_settings_tile=NULL;
lv_style_t wifi_settings_style;
lv_style_t wifi_password_style;
lv_style_t wifi_list_style;
uint32_t wifi_settings_tile_num;

lv_obj_t *wifi_password_tile=NULL;
lv_obj_t *wifi_password_name_label=NULL;
lv_obj_t *wifi_password_pass_textfield=NULL;
uint32_t wifi_password_tile_num;

lv_obj_t *wifi_onoff=NULL;
lv_obj_t *wifiname_list=NULL;

static void enter_wifi_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_wifi_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void wifi_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
void wifi_settings_enter_pass_event_cb( lv_obj_t * obj, lv_event_t event );
void WiFiScanDone(WiFiEvent_t event, WiFiEventInfo_t info);

LV_IMG_DECLARE(lock_16px);
LV_IMG_DECLARE(unlock_16px);
LV_IMG_DECLARE(check_32px);
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(trash_32px);
LV_IMG_DECLARE(wifi_64px);

void wlan_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    wifi_settings_tile_num = mainbar_add_app_tile( 2, 1 );
    wifi_password_tile_num = wifi_settings_tile_num + 1;

    wifi_settings_tile = mainbar_get_tile_obj( wifi_settings_tile_num );
    lv_style_copy( &wifi_settings_style, mainbar_get_style() );
    lv_style_set_bg_color( &wifi_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY );
    lv_style_set_bg_opa( &wifi_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &wifi_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( wifi_settings_tile, LV_OBJ_PART_MAIN, &wifi_settings_style );

    // register an setup icon an set an callback
    lv_obj_t *wifi_setup = lv_imgbtn_create ( setup_tile_register_setup(), NULL);
    lv_imgbtn_set_src( wifi_setup, LV_BTN_STATE_RELEASED, &wifi_64px);
    lv_imgbtn_set_src( wifi_setup, LV_BTN_STATE_PRESSED, &wifi_64px);
    lv_imgbtn_set_src( wifi_setup, LV_BTN_STATE_CHECKED_RELEASED, &wifi_64px);
    lv_imgbtn_set_src( wifi_setup, LV_BTN_STATE_CHECKED_PRESSED, &wifi_64px);
    lv_obj_add_style( wifi_setup, LV_IMGBTN_PART_MAIN,  mainbar_get_style() );
    lv_obj_align( wifi_setup, NULL, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_event_cb( wifi_setup, enter_wifi_setup_event_cb );

    lv_obj_t *exit_btn = lv_imgbtn_create( wifi_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &wifi_settings_style );
    lv_obj_align( exit_btn, wifi_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_wifi_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( wifi_settings_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &wifi_settings_style );
    lv_label_set_text( exit_label, "wlan settings");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    /*Copy the first switch and turn it ON*/
    wifi_onoff = lv_switch_create( wifi_settings_tile, NULL );
    lv_switch_off( wifi_onoff, LV_ANIM_ON );
    lv_obj_align( wifi_onoff, exit_label, LV_ALIGN_OUT_RIGHT_MID, 30, 0 );
    lv_obj_set_event_cb( wifi_onoff, wifi_onoff_event_handler);

    wifiname_list = lv_list_create( wifi_settings_tile, NULL);
    lv_obj_set_size( wifiname_list, LV_HOR_RES_MAX, 160);
    lv_style_init( &wifi_list_style  );
    lv_style_set_border_width( &wifi_list_style , LV_OBJ_PART_MAIN, 0);
    lv_style_set_radius( &wifi_list_style , LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( wifiname_list, LV_OBJ_PART_MAIN, &wifi_list_style  );
    lv_obj_align( wifiname_list, wifi_settings_tile, LV_ALIGN_IN_TOP_MID, 0, 80);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        lv_switch_on( wifi_onoff, LV_ANIM_OFF );
    }, WiFiEvent_t::SYSTEM_EVENT_WIFI_READY );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        lv_switch_off( wifi_onoff, LV_ANIM_OFF );
        while ( lv_list_remove( wifiname_list, 0 ) );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_STOP );

    WiFi.onEvent( WiFiScanDone, WiFiEvent_t::SYSTEM_EVENT_SCAN_DONE );

    wlan_password_tile_setup( wifi_password_tile_num );
}

static void enter_wifi_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
                                        mainbar_jump_to_tilenumber( wifi_settings_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_wifi_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
                                        mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
                                        break;
    }
}

void WiFiScanDone(WiFiEvent_t event, WiFiEventInfo_t info) {

    while ( lv_list_remove( wifiname_list, 0 ) );

    int len = WiFi.scanComplete();
    for( int i = 0 ; i < len ; i++ ) {
        if ( wifictl_is_known( WiFi.SSID(i).c_str() ) ) {
             lv_obj_t * wifiname_list_btn = lv_list_add_btn( wifiname_list, &unlock_16px, WiFi.SSID(i).c_str() );
             lv_obj_set_event_cb( wifiname_list_btn, wifi_settings_enter_pass_event_cb);
        }
        else {
             lv_obj_t * wifiname_list_btn = lv_list_add_btn( wifiname_list, &lock_16px, WiFi.SSID(i).c_str() );
             lv_obj_set_event_cb( wifiname_list_btn, wifi_settings_enter_pass_event_cb);
        }
    }
}

static void wifi_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_VALUE_CHANGED) {
        motor_vibe( 1 );
        if( lv_switch_get_state( obj ) ) {
            wifictl_on();
        }
        else {
            wifictl_off();
        }
    }
}

void wifi_settings_enter_pass_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if(event == LV_EVENT_CLICKED) {
        motor_vibe( 1 );
        lv_label_set_text( wifi_password_name_label, lv_list_get_btn_text(obj) );
        lv_textarea_set_text( wifi_password_pass_textfield, "");
        mainbar_jump_to_tilenumber( wifi_password_tile_num, LV_ANIM_ON );
    }
}

static void exit_wifi_password_event_cb( lv_obj_t * obj, lv_event_t event );
static void wlan_password_event_cb(lv_obj_t * obj, lv_event_t event);
static void apply_wifi_password_event_cb(  lv_obj_t * obj, lv_event_t event );
static void delete_wifi_password_event_cb(  lv_obj_t * obj, lv_event_t event );

void wlan_password_tile_setup( uint32_t wifi_password_tile_num ) {
    // get an app tile and copy mainstyle
    wifi_password_tile = mainbar_get_tile_obj( wifi_password_tile_num );
    lv_style_copy( &wifi_password_style, mainbar_get_style() );
    lv_style_set_bg_color( &wifi_password_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &wifi_password_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &wifi_password_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( wifi_password_tile, LV_OBJ_PART_MAIN, &wifi_password_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( wifi_password_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &wifi_password_style );
    lv_obj_align( exit_btn, wifi_password_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_wifi_password_event_cb );
    
    wifi_password_name_label = lv_label_create( wifi_password_tile, NULL);
    lv_obj_add_style( wifi_password_name_label, LV_OBJ_PART_MAIN, &wifi_password_style  );
    lv_label_set_text( wifi_password_name_label, "wlan setting");
    lv_obj_align( wifi_password_name_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    wifi_password_pass_textfield = lv_textarea_create( wifi_password_tile, NULL);
    lv_textarea_set_text( wifi_password_pass_textfield, "");
    lv_textarea_set_pwd_mode( wifi_password_pass_textfield, false);
    lv_textarea_set_one_line( wifi_password_pass_textfield, true);
    lv_textarea_set_cursor_hidden( wifi_password_pass_textfield, true);
    lv_obj_set_width( wifi_password_pass_textfield, LV_HOR_RES );
    lv_obj_align( wifi_password_pass_textfield, wifi_password_tile, LV_ALIGN_IN_TOP_LEFT, 0, 75);
    lv_obj_set_event_cb( wifi_password_pass_textfield, wlan_password_event_cb );

    lv_obj_t *mac_label = lv_label_create( wifi_password_tile, NULL);
    lv_obj_add_style( mac_label, LV_IMGBTN_PART_MAIN, &wifi_password_style );
    lv_obj_set_width( mac_label, LV_HOR_RES);
    lv_obj_align( mac_label, wifi_password_tile, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_label_set_text_fmt( mac_label, "MAC: %s", WiFi.macAddress().c_str());

    lv_obj_t *apply_btn = lv_imgbtn_create( wifi_password_tile, NULL);
    lv_imgbtn_set_src( apply_btn, LV_BTN_STATE_RELEASED, &check_32px);
    lv_imgbtn_set_src( apply_btn, LV_BTN_STATE_PRESSED, &check_32px);
    lv_imgbtn_set_src( apply_btn, LV_BTN_STATE_CHECKED_RELEASED, &check_32px);
    lv_imgbtn_set_src( apply_btn, LV_BTN_STATE_CHECKED_PRESSED, &check_32px);
    lv_obj_add_style( apply_btn, LV_IMGBTN_PART_MAIN, &wifi_password_style );
    lv_obj_align( apply_btn, wifi_password_pass_textfield, LV_ALIGN_OUT_BOTTOM_RIGHT, -10, 10 );
    lv_obj_set_event_cb( apply_btn, apply_wifi_password_event_cb );

    lv_obj_t *delete_btn = lv_imgbtn_create( wifi_password_tile, NULL);
    lv_imgbtn_set_src( delete_btn, LV_BTN_STATE_RELEASED, &trash_32px);
    lv_imgbtn_set_src( delete_btn, LV_BTN_STATE_PRESSED, &trash_32px);
    lv_imgbtn_set_src( delete_btn, LV_BTN_STATE_CHECKED_RELEASED, &trash_32px);
    lv_imgbtn_set_src( delete_btn, LV_BTN_STATE_CHECKED_PRESSED, &trash_32px);
    lv_obj_add_style( delete_btn, LV_IMGBTN_PART_MAIN, &wifi_password_style );
    lv_obj_align( delete_btn, wifi_password_pass_textfield, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 10 );
    lv_obj_set_event_cb( delete_btn, delete_wifi_password_event_cb );
}


static void apply_wifi_password_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
                                        wifictl_insert_network( lv_label_get_text( wifi_password_name_label ), lv_textarea_get_text( wifi_password_pass_textfield ) );
                                        keyboard_hide();
                                        mainbar_jump_to_tilenumber( wifi_settings_tile_num, LV_ANIM_ON );
                                        break;
    }
}

static void delete_wifi_password_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
                                        wifictl_delete_network( lv_label_get_text( wifi_password_name_label ) );
                                        keyboard_hide();
                                        mainbar_jump_to_tilenumber( wifi_settings_tile_num, LV_ANIM_ON );
                                        break;
    }
}

static void wlan_password_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
                                        keyboard_set_textarea( obj );
                                        break;
    }
}

static void exit_wifi_password_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       keyboard_hide();
                                        motor_vibe( 1 );
                                        mainbar_jump_to_tilenumber( wifi_settings_tile_num, LV_ANIM_ON );
                                        break;
    }
}
