/****************************************************************************
 *   Aug 14 12:37:31 2020
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
#include "ArduinoJson.h"
#include "bluetooth_call.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"
#include "hardware/blectl.h"
#include "hardware/powermgm.h"
#include "hardware/motor.h"

#include "utils/bluejsonrequest.h"

lv_obj_t *bluetooth_call_tile=NULL;
lv_style_t bluetooth_call_style;
uint32_t bluetooth_call_tile_num;

lv_obj_t *bluetooth_call_img = NULL;
lv_obj_t *bluetooth_call_number_label = NULL;
lv_obj_t *bluetooth_call_name_label = NULL;

LV_IMG_DECLARE(cancel_32px);
LV_IMG_DECLARE(call_ok_128px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);
LV_FONT_DECLARE(Ubuntu_72px);

#if defined( BIG_THEME )
    lv_font_t *caller_font = &Ubuntu_72px;
#elif defined( MID_THEME )
    lv_font_t *caller_font = &Ubuntu_32px;
#else
    lv_font_t *caller_font = &Ubuntu_16px;
#endif


bool bluetooth_call_style_change_event_cb( EventBits_t event, void *arg );
static void exit_bluetooth_call_event_cb( lv_obj_t * obj, lv_event_t event );
bool bluetooth_call_event_cb( EventBits_t event, void *arg );
static void bluetooth_call_msg_pharse( BluetoothJsonRequest &doc );

void bluetooth_call_tile_setup( void ) {
    // get an app tile and copy mainstyle
    bluetooth_call_tile_num = mainbar_add_app_tile( 1, 1, "bluetooth call" );
    bluetooth_call_tile = mainbar_get_tile_obj( bluetooth_call_tile_num );

    lv_style_copy( &bluetooth_call_style, APP_STYLE );
    lv_style_set_text_font( &bluetooth_call_style, LV_STATE_DEFAULT, caller_font );
    lv_obj_add_style( bluetooth_call_tile, LV_OBJ_PART_MAIN, &bluetooth_call_style );

    bluetooth_call_img = lv_img_create( bluetooth_call_tile, NULL );
    lv_img_set_src( bluetooth_call_img, &call_ok_128px );
    lv_obj_align( bluetooth_call_img, bluetooth_call_tile, LV_ALIGN_CENTER, 0, 0 );

    bluetooth_call_number_label = lv_label_create( bluetooth_call_tile, NULL);
    lv_obj_add_style( bluetooth_call_number_label, LV_OBJ_PART_MAIN, &bluetooth_call_style  );
    lv_label_set_text( bluetooth_call_number_label, "foo bar");
    lv_obj_align( bluetooth_call_number_label, bluetooth_call_img, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    lv_obj_t * exit_btn = wf_add_exit_button( bluetooth_call_tile, exit_bluetooth_call_event_cb );
    lv_obj_align( exit_btn, bluetooth_call_tile, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING, THEME_PADDING );

    blectl_register_cb( BLECTL_MSG_JSON, bluetooth_call_event_cb, "bluetooth_call" );
    styles_register_cb( STYLE_CHANGE, bluetooth_call_style_change_event_cb, "bluetooth call style" );
}

bool bluetooth_call_style_change_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_CHANGE:  lv_style_copy( &bluetooth_call_style, APP_STYLE );
                            lv_style_set_text_font( &bluetooth_call_style, LV_STATE_DEFAULT, caller_font );
                            break;
    }
    return( true );
}

bool bluetooth_call_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_MSG_JSON:            
            bluetooth_call_msg_pharse( *(BluetoothJsonRequest*)arg );
            break;
    }
    return( true );
}

static void exit_bluetooth_call_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mainbar_jump_to_maintile( LV_ANIM_OFF );
            break;
    }
}

void bluetooth_call_msg_pharse( BluetoothJsonRequest &doc ) {
    static bool standby = false;

    /*
     * check if type and cmd available
     */
    if ( doc["t"] && doc["cmd"] ) {
        /*
         * check for an incoming call
         */
        if( !strcmp( doc["t"], "call" ) && !strcmp( doc["cmd"], "accept" ) ) {
            /*
             * hide statusbar and save current powerstate for later use after a call
             */
            statusbar_hide( true );
            if ( powermgm_get_event( POWERMGM_STANDBY ) ) {
                standby = true;
            }
            else {
                standby = false;
            }
            /*
             * wake up and jup to call tile
             */
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            mainbar_jump_to_tilenumber( bluetooth_call_tile_num, LV_ANIM_OFF );
            /*
             * set caller information
             */
            if ( doc["number"] ) {
                if ( doc["name"] ) {
                    lv_label_set_text( bluetooth_call_number_label, doc["name"] );
                }
                else {
                    lv_label_set_text( bluetooth_call_number_label, doc["number"] );
                }
            }
            else {
                lv_label_set_text( bluetooth_call_number_label, "n/a" );
            }
            lv_obj_align( bluetooth_call_number_label, bluetooth_call_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );                
            motor_vibe(100);            
        }
        else if( !strcmp( doc["t"], "call" ) && !strcmp( doc["cmd"], "start" ) ) {
            /*
                * restore last powerstate after call
                */
            if ( standby == true ) {
                powermgm_set_event( POWERMGM_STANDBY_REQUEST );
            }
            mainbar_jump_to_maintile( LV_ANIM_OFF );
        }
        lv_obj_invalidate( lv_scr_act() );
    }
}
