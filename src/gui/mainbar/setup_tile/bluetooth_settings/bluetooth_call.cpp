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
#include "bluetooth_call.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "hardware/blectl.h"
#include "hardware/powermgm.h"
#include "hardware/motor.h"
#include "hardware/json_psram_allocator.h"

lv_obj_t *bluetooth_call_tile=NULL;
lv_style_t bluetooth_call_style;
uint32_t bluetooth_call_tile_num;

lv_obj_t *bluetooth_call_img = NULL;
lv_obj_t *bluetooth_call_number_label = NULL;
lv_obj_t *bluetooth_call_name_label = NULL;

LV_IMG_DECLARE(cancel_32px);
LV_IMG_DECLARE(call_ok_128px);
LV_FONT_DECLARE(Ubuntu_16px);

static void exit_bluetooth_call_event_cb( lv_obj_t * obj, lv_event_t event );
bool bluetooth_call_event_cb( EventBits_t event, void *arg );
static void bluetooth_call_msg_pharse( const char* msg );

void bluetooth_call_tile_setup( void ) {
    // get an app tile and copy mainstyle
    bluetooth_call_tile_num = mainbar_add_app_tile( 1, 1, "bluetooth call" );
    bluetooth_call_tile = mainbar_get_tile_obj( bluetooth_call_tile_num );

    lv_style_copy( &bluetooth_call_style, mainbar_get_style() );
    lv_style_set_bg_color( &bluetooth_call_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &bluetooth_call_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &bluetooth_call_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font( &bluetooth_call_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_obj_add_style( bluetooth_call_tile, LV_OBJ_PART_MAIN, &bluetooth_call_style );

    bluetooth_call_img = lv_img_create( bluetooth_call_tile, NULL );
    lv_img_set_src( bluetooth_call_img, &call_ok_128px );
    lv_obj_align( bluetooth_call_img, bluetooth_call_tile, LV_ALIGN_CENTER, 0, 0 );

    bluetooth_call_number_label = lv_label_create( bluetooth_call_tile, NULL);
    lv_obj_add_style( bluetooth_call_number_label, LV_OBJ_PART_MAIN, &bluetooth_call_style  );
    lv_label_set_text( bluetooth_call_number_label, "");
    lv_obj_align( bluetooth_call_number_label, bluetooth_call_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    lv_obj_t *exit_btn = lv_imgbtn_create( bluetooth_call_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &cancel_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &bluetooth_call_style );
    lv_obj_align( exit_btn, bluetooth_call_tile, LV_ALIGN_IN_TOP_RIGHT, -10, 10 );
    lv_obj_set_event_cb( exit_btn, exit_bluetooth_call_event_cb );

    blectl_register_cb( BLECTL_MSG, bluetooth_call_event_cb, "bluetooth_call" );
}

bool bluetooth_call_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_MSG:            bluetooth_call_msg_pharse( (const char*)arg );
                                    break;
    }
    return( true );
}

static void exit_bluetooth_call_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

void bluetooth_call_msg_pharse( const char* msg ) {
    static bool standby = false;

    SpiRamJsonDocument doc( strlen( msg ) * 4 );

    DeserializationError error = deserializeJson( doc, msg );
    if ( error ) {
        log_e("bluetooth call deserializeJson() failed: %s", error.c_str() );
    }
    else {
        if ( doc["t"] && doc["cmd"] ) {
            if( !strcmp( doc["t"], "call" ) && !strcmp( doc["cmd"], "accept" ) ) {
                statusbar_hide( true );
                if ( powermgm_get_event( POWERMGM_STANDBY ) ) {
                    standby = true;
                }
                else {
                    standby = false;
                }
                
                powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                mainbar_jump_to_tilenumber( bluetooth_call_tile_num, LV_ANIM_OFF );
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
                lv_obj_invalidate( lv_scr_act() );
                motor_vibe(100);            
            }
        }

        if ( doc["t"] && doc["cmd"] ) {
            if( !strcmp( doc["t"], "call" ) && !strcmp( doc["cmd"], "start" ) ) {
                if ( standby == true ) {
                    powermgm_set_event( POWERMGM_STANDBY_REQUEST );
                }
                mainbar_jump_to_maintile( LV_ANIM_OFF );
                lv_obj_invalidate( lv_scr_act() );
            }
        }
    }        
    doc.clear();
}
