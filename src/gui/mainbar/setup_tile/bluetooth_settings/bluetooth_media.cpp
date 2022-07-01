/****************************************************************************
 *   Oct 05 23:05:42 2020
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
#include "bluetooth_media.h"

#include "gui/app.h"
#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/sound/piep.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/blectl.h"
#include "hardware/powermgm.h"

#include "hardware/ble/gadgetbridge.h"

#include "utils/bluejsonrequest.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
#endif

static bool bluetooth_media_play_state = false;

icon_t *bluetooth_media_app = NULL;
lv_obj_t *bluetooth_media_tile = NULL;
uint32_t bluetooth_media_tile_num;

lv_obj_t *bluetooth_exit_btn = NULL;
lv_obj_t *bluetooth_media_play = NULL;
lv_obj_t *bluetooth_media_stop = NULL;
lv_obj_t *bluetooth_media_prev = NULL;
lv_obj_t *bluetooth_media_next = NULL;
lv_obj_t *bluetooth_media_speaker = NULL;
lv_obj_t *bluetooth_media_volume_down = NULL;
lv_obj_t *bluetooth_media_volume_up = NULL;
lv_obj_t *bluetooth_media_title = NULL;
lv_obj_t *bluetooth_media_artist = NULL;

LV_IMG_DECLARE(cancel_32px);
LV_IMG_DECLARE(play_64px);
LV_IMG_DECLARE(pause_64px);
LV_IMG_DECLARE(next_32px);
LV_IMG_DECLARE(prev_32px);
LV_IMG_DECLARE(sound_32px);
LV_IMG_DECLARE(up_32px);
LV_IMG_DECLARE(down_32px);

LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

static void bluetooth_media_activate_cb( void );
static bool bluetooth_media_event_cb( EventBits_t event, void *arg );
static void enter_bluetooth_media_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_media_cb( lv_obj_t * obj, lv_event_t event );
static void bluetooth_media_volume_up_event_cb( lv_obj_t * obj, lv_event_t event );
static void bluetooth_media_volume_down_event_cb( lv_obj_t * obj, lv_event_t event );
static void bluetooth_media_play_event_cb( lv_obj_t * obj, lv_event_t event );
static void bluetooth_media_next_event_cb( lv_obj_t * obj, lv_event_t event );
static void bluetooth_media_prev_event_cb( lv_obj_t * obj, lv_event_t event );
static bool bluetooth_media_queue_msg( BluetoothJsonRequest &doc );

void bluetooth_media_tile_setup( void ) {
    // get an app tile and copy mainstyle
    bluetooth_media_tile_num = mainbar_add_app_tile( 1, 1, "bluetooth media" );
    // bluetooth_media_tile_num = mainbar_add_tile( 0, 1, "bluetooth media", ws_get_app_style() );
    bluetooth_media_tile = mainbar_get_tile_obj( bluetooth_media_tile_num );

    bluetooth_exit_btn = wf_add_image_button( bluetooth_media_tile, cancel_32px, exit_bluetooth_media_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( bluetooth_exit_btn, bluetooth_media_tile, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING, THEME_PADDING );

    bluetooth_media_play = wf_add_image_button( bluetooth_media_tile, play_64px, bluetooth_media_play_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( bluetooth_media_play, bluetooth_media_tile, LV_ALIGN_CENTER, 0, 0 );

    bluetooth_media_stop = wf_add_image_button( bluetooth_media_tile, pause_64px, bluetooth_media_play_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( bluetooth_media_stop, bluetooth_media_tile, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_hidden( bluetooth_media_stop, true );

    bluetooth_media_next = wf_add_image_button( bluetooth_media_tile, next_32px, bluetooth_media_next_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( bluetooth_media_next, bluetooth_media_play, LV_ALIGN_OUT_RIGHT_MID, THEME_ICON_SIZE, 0 );

    bluetooth_media_prev = wf_add_image_button( bluetooth_media_tile, prev_32px, bluetooth_media_prev_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( bluetooth_media_prev, bluetooth_media_play, LV_ALIGN_OUT_LEFT_MID, -THEME_ICON_SIZE, 0 );

    bluetooth_media_artist = wf_add_label( bluetooth_media_tile, "artist", APP_STYLE );
    lv_obj_set_style_local_text_font( bluetooth_media_artist, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &Ubuntu_32px );
    lv_label_set_long_mode( bluetooth_media_artist, LV_LABEL_LONG_SROLL_CIRC );
    lv_obj_set_width( bluetooth_media_artist, lv_disp_get_hor_res( NULL ) - THEME_ICON_SIZE - THEME_PADDING * 2 );
    lv_obj_align( bluetooth_media_artist, bluetooth_media_tile, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, THEME_PADDING );

    bluetooth_media_title = wf_add_label( bluetooth_media_tile, "title", APP_STYLE );
    lv_label_set_long_mode( bluetooth_media_title, LV_LABEL_LONG_SROLL_CIRC );
    lv_obj_set_width( bluetooth_media_title, lv_disp_get_hor_res( NULL ) - THEME_ICON_SIZE - THEME_PADDING * 2 );
    lv_obj_align( bluetooth_media_title, bluetooth_media_play, LV_ALIGN_OUT_TOP_MID, 0, -THEME_PADDING );

    bluetooth_media_speaker = wf_add_image_button( bluetooth_media_tile, sound_32px, NULL, SYSTEM_ICON_STYLE );
    lv_obj_align( bluetooth_media_speaker, bluetooth_media_play, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_SIZE );

    bluetooth_media_volume_down = wf_add_image_button( bluetooth_media_tile, down_32px, bluetooth_media_volume_down_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( bluetooth_media_volume_down, bluetooth_media_speaker, LV_ALIGN_OUT_LEFT_MID, -THEME_ICON_SIZE, 0 );

    bluetooth_media_volume_up = wf_add_image_button( bluetooth_media_tile, up_32px, bluetooth_media_volume_up_event_cb, SYSTEM_ICON_STYLE );
    lv_obj_align( bluetooth_media_volume_up, bluetooth_media_speaker, LV_ALIGN_OUT_RIGHT_MID, THEME_ICON_SIZE, 0 );

    gadgetbridge_register_cb( GADGETBRIDGE_JSON_MSG, bluetooth_media_event_cb, "bluetooth media" );
    mainbar_add_tile_activate_cb( bluetooth_media_tile_num, bluetooth_media_activate_cb );
    bluetooth_media_app = app_register( "media\nplayer", &play_64px, enter_bluetooth_media_cb );
}

static void bluetooth_media_activate_cb( void ) {
    wf_image_button_fade_in( bluetooth_exit_btn, 300, 0 );
    if( !lv_obj_get_hidden( bluetooth_media_play ) )
        wf_image_button_fade_in( bluetooth_media_play, 300, 0 );
    else
        wf_image_button_fade_in( bluetooth_media_stop, 300, 0 );
    wf_image_button_fade_in( bluetooth_media_next, 300, 200 );
    wf_image_button_fade_in( bluetooth_media_prev, 300, 200 );
    wf_image_button_fade_in( bluetooth_media_speaker, 300, 0 );
    wf_image_button_fade_in( bluetooth_media_volume_down, 300, 200 );
    wf_image_button_fade_in( bluetooth_media_volume_up, 300, 200 );
}

static void enter_bluetooth_media_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
            mainbar_jump_to_tilenumber( bluetooth_media_tile_num, LV_ANIM_OFF, true );
             break;
        case ( LV_EVENT_LONG_PRESSED ):             
            log_e("long press not implement!\r\n");
            break;
    }    
}

static void exit_bluetooth_media_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
            mainbar_jump_back();
            break;
    }    
}

static void bluetooth_media_play_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            if( bluetooth_media_play_state == true ) {
                lv_obj_set_hidden( bluetooth_media_play, true );
                lv_obj_set_hidden( bluetooth_media_stop, false );
                bluetooth_media_play_state = false;
                gadgetbridge_send_msg( (char*)"\r\n{t:\"music\", n:\"pause\"}\r\n" );
            }
            else {
                lv_obj_set_hidden( bluetooth_media_play, false );
                lv_obj_set_hidden( bluetooth_media_stop, true );
                bluetooth_media_play_state = true;
                gadgetbridge_send_msg( (char*)"\r\n{t:\"music\", n:\"play\"}\r\n" );
            }
            break;
    }
}

static void bluetooth_media_volume_up_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            gadgetbridge_send_msg( (char*)"\r\n{t:\"music\", n:\"volumeup\"}\r\n" );
            break;
    }
}

static void bluetooth_media_volume_down_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            gadgetbridge_send_msg( (char*)"\r\n{t:\"music\", n:\"volumedown\"}\r\n" );
            break;
    }
}

static void bluetooth_media_next_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            gadgetbridge_send_msg( (char*)"\r\n{t:\"music\", n:\"next\"}\r\n" );
            break;
    }
}

static void bluetooth_media_prev_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            gadgetbridge_send_msg( (char*)"\r\n{t:\"music\", n:\"previous\"}\r\n" );
            break;
    }
}

bool bluetooth_media_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case GADGETBRIDGE_JSON_MSG:            
            bluetooth_media_queue_msg( *(BluetoothJsonRequest*)arg );
            break;
    }
    return( true );
}

static bool bluetooth_media_queue_msg( BluetoothJsonRequest &doc ) {
    bool retval = false;
    
    if( doc.containsKey("t") ) {
        /**
         * check if we have a music state
         */
        if( !strcmp( doc["t"], "musicstate" ) && doc.containsKey("state") ) {
            /**
             * check for play state
             */
            if( !strcmp( doc["state"], "pause" ) ) {
                lv_obj_set_hidden( bluetooth_media_play, false );
                lv_obj_set_hidden( bluetooth_media_stop, true );
                bluetooth_media_play_state = false;
            }
            /**
             * check for play stop
             */
            else if( !strcmp( doc["state"], "play" ) ) {
                lv_obj_set_hidden( bluetooth_media_play, true );
                lv_obj_set_hidden( bluetooth_media_stop, false );
                bluetooth_media_play_state = true;
            }
            retval = true;
        }
        /**
         * check if we have a music information
         */
        else if( !strcmp( doc["t"], "musicinfo" ) ) {
            /**
             * check for track info update
             */
            if ( doc.containsKey("track") ) {
                lv_label_set_text( bluetooth_media_title, doc["track"] );
                lv_obj_align( bluetooth_media_title, bluetooth_media_play, LV_ALIGN_OUT_TOP_MID, 0, -THEME_PADDING );
            }
            /**
             * check for track artist update
             */            
            if ( doc.containsKey("artist") ) {
                lv_label_set_text( bluetooth_media_artist, doc["artist"] );
                lv_obj_align( bluetooth_media_artist, bluetooth_media_tile, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, THEME_PADDING );
            }
            retval = true;
        }
    }

    if( retval && bluetooth_media_play_state ) {
        if( blectl_get_media_notification() ) {
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            lv_disp_trig_activity( NULL );
            mainbar_jump_to_tilenumber( bluetooth_media_tile_num, LV_ANIM_OFF, true );
        }        
    }

    return( retval );
}