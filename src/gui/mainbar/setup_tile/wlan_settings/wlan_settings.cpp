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
#include "gui/statusbar.h"
#include "gui/keyboard.h"

#include "hardware/wifictl.h"
#include "hardware/motor.h"

#include <WiFi.h>

LV_IMG_DECLARE(exit_32px);

lv_obj_t *wlan_settings_tile1=NULL;
lv_obj_t *wifi_onoff=NULL;
lv_obj_t *wifiname_list=NULL;
lv_style_t wlan_settings_style;
lv_style_t wlan_list_style;

static void exit_wifi_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_wifi_password_event_cb( lv_obj_t * obj, lv_event_t event );
static void wifi_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
void wifi_settings_enter_pass_event_cb( lv_obj_t * obj, lv_event_t event );
void WiFiScanDone(WiFiEvent_t event, WiFiEventInfo_t info);

LV_IMG_DECLARE(lock_16px);
LV_IMG_DECLARE(unlock_16px);
LV_IMG_DECLARE(check_32px);
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(trash_32px);

void wlan_settings_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {
    lv_style_init( &wlan_settings_style );
    lv_style_set_radius( &wlan_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color( &wlan_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &wlan_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &wlan_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &wlan_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor( &wlan_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    wlan_settings_tile1 = lv_obj_create( tile, NULL);
    lv_obj_set_size(wlan_settings_tile1, hres , vres);
    lv_obj_align(wlan_settings_tile1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style( wlan_settings_tile1, LV_OBJ_PART_MAIN, &wlan_settings_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( wlan_settings_tile1, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( exit_btn, wlan_settings_tile1, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_wifi_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( wlan_settings_tile1, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( exit_label, "wlan settings");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    /*Copy the first switch and turn it ON*/
    wifi_onoff = lv_switch_create( wlan_settings_tile1, NULL );
    lv_switch_off( wifi_onoff, LV_ANIM_ON );
    lv_obj_align( wifi_onoff, exit_label, LV_ALIGN_OUT_RIGHT_MID, 30, 0 );
    lv_obj_set_event_cb( wifi_onoff, wifi_onoff_event_handler);

    wifiname_list = lv_list_create( wlan_settings_tile1, NULL);
    lv_obj_set_size( wifiname_list, hres, 160);
    lv_style_init( &wlan_list_style );
    lv_style_set_border_width( &wlan_list_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_radius( &wlan_list_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( wifiname_list, LV_OBJ_PART_MAIN, &wlan_list_style );
    lv_obj_align( wifiname_list, wlan_settings_tile1, LV_ALIGN_IN_TOP_MID, 0, 80);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        lv_switch_on( wifi_onoff, LV_ANIM_OFF );
    }, WiFiEvent_t::SYSTEM_EVENT_WIFI_READY );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        lv_switch_off( wifi_onoff, LV_ANIM_OFF );
        while ( lv_list_remove( wifiname_list, 0 ) );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_STOP );

    WiFi.onEvent( WiFiScanDone, WiFiEvent_t::SYSTEM_EVENT_SCAN_DONE );
}

lv_obj_t *wlan_password_tile1=NULL;
lv_obj_t *wlan_password_name_label=NULL;
lv_obj_t *wlan_password_pass_textfield=NULL;

static void wlan_password_event_cb(lv_obj_t * obj, lv_event_t event);
static void apply_wifi_password_event_cb(  lv_obj_t * obj, lv_event_t event );
static void delete_wifi_password_event_cb(  lv_obj_t * obj, lv_event_t event );

void wlan_password_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {
    lv_style_init( &wlan_settings_style );
    lv_style_set_radius( &wlan_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color( &wlan_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &wlan_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &wlan_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &wlan_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor( &wlan_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    wlan_password_tile1 = lv_obj_create( tile, NULL);
    lv_obj_set_size (wlan_password_tile1, hres , vres);
    lv_obj_align( wlan_password_tile1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style( wlan_password_tile1, LV_OBJ_PART_MAIN, &wlan_settings_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( wlan_password_tile1, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( exit_btn, wlan_password_tile1, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_wifi_password_event_cb );
    
    wlan_password_name_label = lv_label_create( wlan_password_tile1, NULL);
    lv_obj_add_style( wlan_password_name_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( wlan_password_name_label, "wlan setting");
    lv_obj_align( wlan_password_name_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    wlan_password_pass_textfield = lv_textarea_create( wlan_password_tile1, NULL);
    lv_textarea_set_text( wlan_password_pass_textfield, "");
    lv_textarea_set_pwd_mode( wlan_password_pass_textfield, false);
    lv_textarea_set_one_line( wlan_password_pass_textfield, true);
    lv_textarea_set_cursor_hidden( wlan_password_pass_textfield, true);
    lv_obj_set_width( wlan_password_pass_textfield, LV_HOR_RES );
    lv_obj_align( wlan_password_pass_textfield, wlan_password_tile1, LV_ALIGN_IN_TOP_LEFT, 0, 75);
    lv_obj_set_event_cb( wlan_password_pass_textfield, wlan_password_event_cb );

    lv_obj_t *mac_label = lv_label_create( wlan_password_tile1, NULL);
    lv_obj_add_style( mac_label, LV_IMGBTN_PART_MAIN, style);
    lv_obj_set_width( mac_label, LV_HOR_RES);
    lv_obj_align( mac_label, wlan_password_tile1, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_label_set_text_fmt( mac_label, "MAC: %s", WiFi.macAddress().c_str());

    lv_obj_t *apply_btn = lv_imgbtn_create( wlan_password_tile1, NULL);
    lv_imgbtn_set_src( apply_btn, LV_BTN_STATE_RELEASED, &check_32px);
    lv_imgbtn_set_src( apply_btn, LV_BTN_STATE_PRESSED, &check_32px);
    lv_imgbtn_set_src( apply_btn, LV_BTN_STATE_CHECKED_RELEASED, &check_32px);
    lv_imgbtn_set_src( apply_btn, LV_BTN_STATE_CHECKED_PRESSED, &check_32px);
    lv_obj_add_style( apply_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( apply_btn, wlan_password_pass_textfield, LV_ALIGN_OUT_BOTTOM_RIGHT, -10, 10 );
    lv_obj_set_event_cb( apply_btn, apply_wifi_password_event_cb );

    lv_obj_t *delete_btn = lv_imgbtn_create( wlan_password_tile1, NULL);
    lv_imgbtn_set_src( delete_btn, LV_BTN_STATE_RELEASED, &trash_32px);
    lv_imgbtn_set_src( delete_btn, LV_BTN_STATE_PRESSED, &trash_32px);
    lv_imgbtn_set_src( delete_btn, LV_BTN_STATE_CHECKED_RELEASED, &trash_32px);
    lv_imgbtn_set_src( delete_btn, LV_BTN_STATE_CHECKED_PRESSED, &trash_32px);
    lv_obj_add_style( delete_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( delete_btn, wlan_password_pass_textfield, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 10 );
    lv_obj_set_event_cb( delete_btn, delete_wifi_password_event_cb );
}


static void apply_wifi_password_event_cb(  lv_obj_t * obj, lv_event_t event ) {

    if(event == LV_EVENT_CLICKED) {
        motor_vibe( 1 );
        wifictl_insert_network( lv_label_get_text( wlan_password_name_label ), lv_textarea_get_text( wlan_password_pass_textfield ) );
        keyboard_hide();
        mainbar_jump_to_tilenumber( WLAN_SETTINGS_TILE, LV_ANIM_ON );
    }

}

static void delete_wifi_password_event_cb(  lv_obj_t * obj, lv_event_t event ) {

    if(event == LV_EVENT_CLICKED) {
        motor_vibe( 1 );
        wifictl_delete_network( lv_label_get_text( wlan_password_name_label ) );
        keyboard_hide();
        mainbar_jump_to_tilenumber( WLAN_SETTINGS_TILE, LV_ANIM_ON );
    }

}

static void wlan_password_event_cb( lv_obj_t * obj, lv_event_t event )
{
    if( event == LV_EVENT_CLICKED ) {
        motor_vibe( 1 );
        keyboard_set_textarea( obj );
    }
}

static void exit_wifi_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
        mainbar_jump_to_tilenumber( SETUP_TILE, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_wifi_password_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       keyboard_hide();
                                        motor_vibe( 1 );
                                        mainbar_jump_to_tilenumber( WLAN_SETTINGS_TILE, LV_ANIM_ON );
                                        break;
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
        lv_label_set_text( wlan_password_name_label, lv_list_get_btn_text(obj) );
        lv_textarea_set_text( wlan_password_pass_textfield, "");
        mainbar_jump_to_tilenumber( WLAN_PASSWORD_TILE, LV_ANIM_ON );
    }
}

void WiFiScanDone(WiFiEvent_t event, WiFiEventInfo_t info) {

    while (lv_list_remove( wifiname_list, 0 ) );

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