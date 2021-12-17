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
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"
#include "hardware/wifictl.h"
#include "hardware/motor.h"
#include "hardware/blectl.h"
#include "utils/bluejsonrequest.h"
#include "utils/webserver/webserver.h"
#include "utils/ftpserver/ftpserver.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #include <WiFi.h>
#endif

lv_obj_t *wifi_settings_tile=NULL;
lv_style_t wifi_list_style;
uint32_t wifi_settings_tile_num;
icon_t *wifi_setup_icon = NULL;

lv_obj_t *wifi_password_tile=NULL;
lv_style_t wifi_password_style;
lv_obj_t *wifi_password_name_label=NULL;
lv_obj_t *wifi_password_pass_textfield=NULL;
uint32_t wifi_password_tile_num;

lv_obj_t *wifi_setup_tile=NULL;
lv_style_t wifi_setup_style;
uint32_t wifi_setup_tile_num;

lv_obj_t *wifi_onoff=NULL;
lv_obj_t *wifiname_list=NULL;

static void enter_wifi_settings_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_wifi_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void wifi_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
void wifi_settings_enter_pass_event_cb( lv_obj_t * obj, lv_event_t event );
bool wifi_setup_wifictl_event_cb( EventBits_t event, void *arg );

bool wifi_setup_bluetooth_message_event_cb( EventBits_t event, void *arg );
static void wifi_setup_bluetooth_message_msg_pharse( BluetoothJsonRequest &doc );

LV_IMG_DECLARE(lock_16px);
LV_IMG_DECLARE(unlock_16px);
LV_IMG_DECLARE(wifi_64px);

void wlan_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    wifi_settings_tile_num = mainbar_add_setup_tile( 2, 2, "wifi setup" );
    wifi_password_tile_num = wifi_settings_tile_num + 1;
    wifi_setup_tile_num = wifi_settings_tile_num + 2;

    wifi_settings_tile = mainbar_get_tile_obj( wifi_settings_tile_num );

    wifi_setup_icon = setup_register( "wifi", &wifi_64px, enter_wifi_settings_event_cb );
    setup_hide_indicator( wifi_setup_icon );

    lv_obj_t *header = wf_add_settings_header( wifi_settings_tile, "wlan" );
    lv_obj_align( header, wifi_settings_tile, LV_ALIGN_IN_TOP_LEFT, THEME_ICON_PADDING, STATUSBAR_HEIGHT + THEME_ICON_PADDING );

    lv_obj_t *setup_btn = wf_add_setup_button( wifi_settings_tile, enter_wifi_setup_event_cb, SETUP_STYLE );
    lv_obj_align( setup_btn, wifi_settings_tile, LV_ALIGN_IN_TOP_RIGHT, -THEME_ICON_PADDING, STATUSBAR_HEIGHT + THEME_ICON_PADDING );

    /*Copy the first switch and turn it ON*/    
    wifi_onoff = wf_add_switch( wifi_settings_tile, false );
    lv_obj_align( wifi_onoff, setup_btn, LV_ALIGN_OUT_LEFT_MID, -THEME_ICON_PADDING, 0 );
    lv_obj_set_event_cb( wifi_onoff, wifi_onoff_event_handler);

    wifiname_list = lv_list_create( wifi_settings_tile, NULL);
    lv_obj_set_size( wifiname_list, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) - STATUSBAR_HEIGHT - THEME_ICON_SIZE );
    lv_style_init( &wifi_list_style  );
    lv_style_set_border_width( &wifi_list_style , LV_OBJ_PART_MAIN, 0);
    lv_style_set_radius( &wifi_list_style , LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( wifiname_list, LV_OBJ_PART_MAIN, &wifi_list_style  );
    lv_obj_align( wifiname_list, wifi_settings_tile, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );

    wlan_password_tile_setup( wifi_password_tile_num );
    wlan_setup_tile_setup( wifi_setup_tile_num );

    wifictl_register_cb( WIFICTL_ON | WIFICTL_OFF | WIFICTL_SCAN_DONE | WIFICTL_SCAN_ENTRY, wifi_setup_wifictl_event_cb, "wifi network scan" );
}

uint32_t wifi_get_setup_tile_num( void ) {
    return ( wifi_setup_tile_num );
}

bool wifi_setup_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case    WIFICTL_ON:
                    lv_switch_on( wifi_onoff, LV_ANIM_OFF );
                    break;
        case    WIFICTL_OFF:
                    lv_switch_off( wifi_onoff, LV_ANIM_OFF );
                    while ( lv_list_remove( wifiname_list, 0 ) );
                    break;
        case    WIFICTL_SCAN_DONE:
                    while ( lv_list_remove( wifiname_list, 0 ) );
                    break;
        case    WIFICTL_SCAN_ENTRY:
                    lv_obj_t * wifiname_list_btn = lv_list_add_btn( wifiname_list, wifictl_is_known( (const char*)arg )?&unlock_16px:&lock_16px , (const char*)arg );
                    lv_obj_set_event_cb( wifiname_list_btn, wifi_settings_enter_pass_event_cb);
                    break;
    }
    return( true );
}

static void enter_wifi_settings_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( wifi_settings_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void wifi_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): if( lv_switch_get_state( obj ) ) {
                                            wifictl_on();
                                        }
                                        else {
                                            wifictl_off();
                                        }
    }
}

void wifi_settings_enter_pass_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   lv_label_set_text( wifi_password_name_label, lv_list_get_btn_text(obj) );
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

    lv_obj_t *header = wf_add_settings_header( wifi_password_tile, "wlan setting", exit_wifi_password_event_cb );
    lv_obj_align( header, wifi_password_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + THEME_ICON_PADDING );
    
    wifi_password_name_label = wf_get_settings_header_title(header);

    wifi_password_pass_textfield = lv_textarea_create( wifi_password_tile, NULL);
    lv_textarea_set_text( wifi_password_pass_textfield, "");
    lv_textarea_set_pwd_mode( wifi_password_pass_textfield, false);
    lv_textarea_set_one_line( wifi_password_pass_textfield, true);
    lv_textarea_set_cursor_hidden( wifi_password_pass_textfield, true);
    lv_obj_set_width( wifi_password_pass_textfield, lv_disp_get_hor_res( NULL ) - THEME_ICON_PADDING * 2 );
    lv_obj_align( wifi_password_pass_textfield, header, LV_ALIGN_OUT_BOTTOM_MID, THEME_ICON_PADDING, THEME_ICON_PADDING );
    lv_obj_set_event_cb( wifi_password_pass_textfield, wlan_password_event_cb );

    lv_obj_t *mac_label = lv_label_create( wifi_password_tile, NULL);
    lv_obj_add_style( mac_label, LV_IMGBTN_PART_MAIN, &wifi_password_style );
    lv_obj_set_width( mac_label, lv_disp_get_hor_res( NULL ) );
    lv_obj_align( mac_label, wifi_password_tile, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
#ifdef NATIVE_64BIT
    lv_label_set_text_fmt( mac_label, "MAC:" );
#else
    lv_label_set_text_fmt( mac_label, "MAC: %s", WiFi.macAddress().c_str());
#endif

    lv_obj_t *apply_btn = wf_add_check_button( wifi_password_tile, apply_wifi_password_event_cb, &wifi_password_style );
    lv_obj_align( apply_btn, wifi_password_pass_textfield, LV_ALIGN_OUT_BOTTOM_RIGHT, -10, 10 );

    lv_obj_t *delete_btn = wf_add_trash_button( wifi_password_tile, delete_wifi_password_event_cb, &wifi_password_style );
    lv_obj_align( delete_btn, wifi_password_pass_textfield, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 10 );
}

static void apply_wifi_password_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       wifictl_insert_network( lv_label_get_text( wifi_password_name_label ), lv_textarea_get_text( wifi_password_pass_textfield ) );
                                        keyboard_hide();
                                        mainbar_jump_to_tilenumber( wifi_settings_tile_num, LV_ANIM_ON );
                                        break;
    }
}

static void delete_wifi_password_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       wifictl_delete_network( lv_label_get_text( wifi_password_name_label ) );
                                        keyboard_hide();
                                        mainbar_jump_to_tilenumber( wifi_settings_tile_num, LV_ANIM_ON );
                                        break;
    }
}

static void wlan_password_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       keyboard_set_textarea( obj );
                                        break;
    }
}

static void exit_wifi_password_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       keyboard_hide();
                                        mainbar_jump_back();
                                        break;
    }
}

lv_obj_t *wifi_autoon_onoff = NULL;
lv_obj_t *wifi_webserver_onoff = NULL;
lv_obj_t *wifi_ftpserver_onoff = NULL;
lv_obj_t *wifi_enabled_on_standby_onoff = NULL;

static void wps_start_event_handler( lv_obj_t * obj, lv_event_t event );
static void wifi_autoon_onoff_event_handler( lv_obj_t * obj, lv_event_t event );
static void wifi_webserver_onoff_event_handler( lv_obj_t * obj, lv_event_t event );
static void wifi_ftpserver_onoff_event_handler( lv_obj_t * obj, lv_event_t event );
static void wifi_enabled_on_standby_onoff_event_handler( lv_obj_t * obj, lv_event_t event );
bool wifi_setup_autoon_event_cb( EventBits_t event, void *arg );

void wlan_setup_tile_setup( uint32_t wifi_setup_tile_num ) {
    // get an app tile and copy mainstyle
    wifi_setup_tile = mainbar_get_tile_obj( wifi_setup_tile_num );

    lv_obj_t *header = wf_add_settings_header( wifi_setup_tile, "wlan settings" );
    lv_obj_align( header, wifi_setup_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + THEME_ICON_PADDING );

    lv_obj_t *wifi_autoon_onoff_cont = wf_add_labeled_switch( wifi_setup_tile, "enable on wakeup", &wifi_autoon_onoff, wifictl_get_autoon(), wifi_autoon_onoff_event_handler,SETUP_STYLE );
    lv_obj_align( wifi_autoon_onoff_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_PADDING );

    lv_obj_t *wifi_enabled_on_standby_onoff_cont = wf_add_labeled_switch( wifi_setup_tile, "enable on standby", &wifi_enabled_on_standby_onoff, wifictl_get_enable_on_standby(), wifi_enabled_on_standby_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( wifi_enabled_on_standby_onoff_cont, wifi_autoon_onoff_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_PADDING );

    lv_obj_t *wifi_webserver_onoff_cont = wf_add_labeled_switch( wifi_setup_tile, "enable webserver", &wifi_webserver_onoff, wifictl_get_webserver(), wifi_webserver_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( wifi_webserver_onoff_cont, wifi_enabled_on_standby_onoff_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_PADDING );

    lv_obj_t *wifi_ftpserver_onoff_cont = wf_add_labeled_switch( wifi_setup_tile, "enable ftpserver", &wifi_ftpserver_onoff, wifictl_get_ftpserver(), wifi_ftpserver_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( wifi_ftpserver_onoff_cont, wifi_webserver_onoff_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_PADDING );

    lv_obj_t *wps_btn = lv_btn_create( wifi_setup_tile, NULL);
    lv_obj_set_event_cb( wps_btn, wps_start_event_handler );
    lv_obj_align( wps_btn, wifi_ftpserver_onoff_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *wps_btn_label = lv_label_create( wps_btn, NULL );
    lv_label_set_text( wps_btn_label, "start WPS");

    #ifndef ENABLE_WEBSERVER
        lv_obj_set_hidden( wifi_webserver_onoff_cont, true );
    #endif
    #ifndef ENABLE_FTPSERVER
        lv_obj_set_hidden( wifi_ftpserver_onoff_cont, true );
    #endif

    blectl_register_cb( BLECTL_MSG_JSON, wifi_setup_bluetooth_message_event_cb, "wifi settings" );
    wifictl_register_cb( WIFICTL_AUTOON, wifi_setup_autoon_event_cb, "wifi setup");
}

static void wps_start_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       wifictl_start_wps();
                                        break;
    }
}

static void enter_wifi_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( wifi_setup_tile_num, LV_ANIM_ON );
                                        break;
    }
}

static void wifi_autoon_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch (event) {
        case (LV_EVENT_VALUE_CHANGED):  wifictl_set_autoon( lv_switch_get_state( obj ) );
                                        break;
    }
}

static void wifi_webserver_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch (event) {
        case (LV_EVENT_VALUE_CHANGED):  wifictl_set_webserver( lv_switch_get_state( obj ) );
                                            if ( lv_switch_get_state( obj ) ) {
                                            asyncwebserver_start();
                                        }
                                        else {
                                            asyncwebserver_end();
                                        }
                                        break;
    }
}

static void wifi_ftpserver_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch (event) {
        case (LV_EVENT_VALUE_CHANGED):  wifictl_set_ftpserver( lv_switch_get_state( obj ) );
                                        break;
    }
}

static void wifi_enabled_on_standby_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch (event) {
        case (LV_EVENT_VALUE_CHANGED):  wifictl_set_enable_on_standby( lv_switch_get_state( obj ) );
                                        if ( lv_switch_get_state( obj ) ) {
                                            setup_set_indicator( wifi_setup_icon, ICON_INDICATOR_FAIL );
                                        }
                                        else {
                                            setup_hide_indicator( wifi_setup_icon );
                                        }
                                        break;
    }
}

bool wifi_setup_autoon_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_AUTOON:
            if ( *(bool*)arg ) {
                lv_switch_on( wifi_autoon_onoff, LV_ANIM_OFF);
            }
            else {
                lv_switch_off( wifi_autoon_onoff, LV_ANIM_OFF);
            }
            break;
    }
    return( true );
}

bool wifi_setup_bluetooth_message_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_MSG_JSON:       wifi_setup_bluetooth_message_msg_pharse( *(BluetoothJsonRequest*)arg );
                                    break;
    }
    return( true );
}

void wifi_setup_bluetooth_message_msg_pharse( BluetoothJsonRequest &doc ) {
    if( !strcmp( doc["t"], "conf" ) ) {
        if ( !strcmp( doc["app"], "settings" ) ) {
            if ( !strcmp( doc["settings"], "wlan" ) ) {
                motor_vibe(100);
                wifictl_insert_network(  doc["ssid"] |"" , doc["key"] |"" );
            }
        }
    }
}
