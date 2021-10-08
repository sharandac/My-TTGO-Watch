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
#include "bluetooth_media.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/blectl.h"

icon_t *bluettoth_setup_icon = NULL;

lv_obj_t *bluetooth_settings_tile_1=NULL;
lv_obj_t *bluetooth_settings_tile_2=NULL;
uint32_t bluetooth_tile_num_1;
uint32_t bluetooth_tile_num_2;

lv_obj_t *bluetooth_enable_onoff = NULL;
lv_obj_t *bluetooth_standby_onoff = NULL;
lv_obj_t *bluetooth_stayon_onoff = NULL;
lv_obj_t *bluetooth_advertising_onoff = NULL;
lv_obj_t *bluetooth_show_notifications_onoff = NULL;
lv_obj_t *txpower_list = NULL;

LV_IMG_DECLARE(bluetooth_64px);
LV_IMG_DECLARE(info_fail_16px);
LV_IMG_DECLARE(up_32px);
LV_IMG_DECLARE(down_32px);

static void enter_bluetooth_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void down_bluetooth_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void up_bluetooth_setup_event_cb( lv_obj_t * obj, lv_event_t event );
bool blectl_onoff_event_cb( EventBits_t event, void *arg );
static void bluetooth_enable_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void bluetooth_standby_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void bluetooth_stayon_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void bluetooth_advertising_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void bluetooth_show_notifications_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void bluetooth_txpower_event_handler(lv_obj_t * obj, lv_event_t event);

void bluetooth_settings_tile_setup( void ) {
    lv_obj_t *header_2 = NULL;

    #if RES_Y_MAX < 340
        bluetooth_tile_num_1 = mainbar_add_setup_tile( 1, 2, "bluetooth setup" );
        bluetooth_tile_num_2 = bluetooth_tile_num_1 + 1;
        bluetooth_settings_tile_1 = mainbar_get_tile_obj( bluetooth_tile_num_1 );
        bluetooth_settings_tile_2 = mainbar_get_tile_obj( bluetooth_tile_num_2 );
        lv_obj_add_style( bluetooth_settings_tile_1, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );
        lv_obj_add_style( bluetooth_settings_tile_2, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );
    #else
        bluetooth_tile_num_1 = mainbar_add_setup_tile( 1, 1, "bluetooth setup" );
        bluetooth_settings_tile_1 = mainbar_get_tile_obj( bluetooth_tile_num_1 );
        lv_obj_add_style( bluetooth_settings_tile_1, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );
    #endif

    bluettoth_setup_icon = setup_register( "bluetooth", &bluetooth_64px, enter_bluetooth_setup_event_cb );
    setup_hide_indicator( bluettoth_setup_icon );

    lv_obj_t *header = wf_add_settings_header( bluetooth_settings_tile_1, "bluetooth settings" );
    lv_obj_align( header, bluetooth_settings_tile_1, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, STATUSBAR_HEIGHT + THEME_PADDING );
    
    if ( bluetooth_settings_tile_2 ) {
        header_2 = wf_add_settings_header( bluetooth_settings_tile_2, "bluetooth settings" );
        lv_obj_align( header_2, bluetooth_settings_tile_2, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, STATUSBAR_HEIGHT + THEME_PADDING );

        lv_obj_t *up_btn_1 = wf_add_image_button( bluetooth_settings_tile_2, up_32px, up_bluetooth_setup_event_cb, ws_get_setup_tile_style() );
        lv_obj_align( up_btn_1, bluetooth_settings_tile_2, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING, STATUSBAR_HEIGHT + THEME_PADDING );

        lv_obj_t *down_btn_1 = wf_add_image_button( bluetooth_settings_tile_1, down_32px, down_bluetooth_setup_event_cb, ws_get_setup_tile_style() );
        lv_obj_align( down_btn_1, bluetooth_settings_tile_1, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING, STATUSBAR_HEIGHT + THEME_PADDING );
    }
    
    lv_obj_t *bluetooth_enable_cont = wf_add_labeled_switch( bluetooth_settings_tile_1, "enable on wakeup", &bluetooth_enable_onoff, blectl_get_autoon(), bluetooth_enable_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( bluetooth_enable_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    lv_obj_t *bluetooth_advertising_cont = wf_add_labeled_switch( bluetooth_settings_tile_1, "visibility", &bluetooth_advertising_onoff, blectl_get_advertising(), bluetooth_advertising_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( bluetooth_advertising_cont, bluetooth_enable_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );
    
    lv_obj_t *bluetooth_standby_cont = wf_add_labeled_switch( bluetooth_settings_tile_1, "always on", &bluetooth_standby_onoff, blectl_get_enable_on_standby(), bluetooth_standby_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( bluetooth_standby_cont, bluetooth_advertising_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    lv_obj_t *bluetooth_stayon_cont = wf_add_labeled_switch( bluetooth_settings_tile_1, "stay on", &bluetooth_stayon_onoff, blectl_get_disable_only_disconnected(), bluetooth_stayon_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( bluetooth_stayon_cont, bluetooth_standby_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    lv_obj_t *bluetooth_show_notifications_cont = wf_add_labeled_switch( bluetooth_settings_tile_1, "show notifications", &bluetooth_show_notifications_onoff, blectl_get_show_notification(), bluetooth_show_notifications_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( bluetooth_show_notifications_cont, bluetooth_stayon_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    if ( bluetooth_settings_tile_2 ) {
        lv_obj_t *txpower_cont = wf_add_labeled_list( bluetooth_settings_tile_2, "tx power", &txpower_list, "-12db\n-9db\n-6db\n-3db\n0db", bluetooth_txpower_event_handler, ws_get_setup_tile_style() );
        lv_obj_align( txpower_cont, header_2, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );
    }
    else {
        lv_obj_t *txpower_cont = wf_add_labeled_list( bluetooth_settings_tile_1, "tx power", &txpower_list, "-12db\n-9db\n-6db\n-3db\n0db", bluetooth_txpower_event_handler, SETUP_STYLE );
        lv_obj_align( txpower_cont, bluetooth_show_notifications_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );
    }

    lv_dropdown_set_selected( txpower_list, blectl_get_txpower() );

    bluetooth_pairing_tile_setup();
    bluetooth_call_tile_setup();
    bluetooth_message_tile_setup();
    bluetooth_media_tile_setup();

    blectl_register_cb( BLECTL_ON | BLECTL_OFF, blectl_onoff_event_cb, "bluetooth settings");
}

uint32_t bluetooth_get_setup_tile_num() {
    return ( bluetooth_tile_num_1 );
}

bool blectl_onoff_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_ON:
            lv_switch_on( bluetooth_enable_onoff, LV_ANIM_OFF );
            break;
        case BLECTL_OFF:
            lv_switch_off( bluetooth_enable_onoff, LV_ANIM_OFF );
            break;
    }
    return( true );
}

static void enter_bluetooth_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( bluetooth_tile_num_1, LV_ANIM_OFF );
                                        break;
    }
}

static void down_bluetooth_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       lv_dropdown_close( txpower_list );
                                        mainbar_jump_to_tilenumber( bluetooth_tile_num_2, LV_ANIM_ON );
                                        break;
    }

}

static void up_bluetooth_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       lv_dropdown_close( txpower_list );
                                        mainbar_jump_back();
                                        break;
    }

}

static void bluetooth_enable_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED): blectl_set_autoon( lv_switch_get_state( obj ) );
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
                                        break;
    }
}

static void bluetooth_stayon_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED): blectl_set_disable_only_disconnected( lv_switch_get_state( obj ) );
    }
}

static void bluetooth_show_notifications_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED): blectl_set_show_notification( lv_switch_get_state( obj ) );
                                        break;
    }
}

static void bluetooth_txpower_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED): blectl_set_txpower( lv_dropdown_get_selected( obj ) );
                                        break;
    }
}