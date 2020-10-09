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

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/sound/piep.h"

#include "hardware/blectl.h"
#include "hardware/json_psram_allocator.h"
#include "hardware/powermgm.h"

static bool bluetooth_media_play_state = false;

lv_obj_t *bluetooth_media_tile = NULL;
lv_style_t bluetooth_media_style;
uint32_t bluetooth_media_tile_num;

lv_obj_t *bluetooth_media_play = NULL;
lv_obj_t *bluetooth_media_prev = NULL;
lv_obj_t *bluetooth_media_next = NULL;
lv_obj_t *bluetooth_media_title = NULL;
lv_obj_t *bluetooth_media_album = NULL;

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

bool bluetooth_media_event_cb( EventBits_t event, void *arg );
static void exit_bluetooth_media_volume_up_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_media_volume_down_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_media_play_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_media_next_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_media_prev_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_media_event_cb( lv_obj_t * obj, lv_event_t event );
bool bluetooth_media_queue_msg( const char *msg );

void bluetooth_media_tile_setup( void ) {
    // get an app tile and copy mainstyle
    bluetooth_media_tile_num = mainbar_add_app_tile( 1, 1, "bluetooth media" );
    bluetooth_media_tile = mainbar_get_tile_obj( bluetooth_media_tile_num );

    lv_style_copy( &bluetooth_media_style, mainbar_get_style() );
    lv_style_set_bg_color( &bluetooth_media_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &bluetooth_media_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &bluetooth_media_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font( &bluetooth_media_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_obj_add_style( bluetooth_media_tile, LV_OBJ_PART_MAIN, &bluetooth_media_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( bluetooth_media_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &cancel_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &bluetooth_media_style );
    lv_obj_align( exit_btn, bluetooth_media_tile, LV_ALIGN_IN_TOP_RIGHT, -10, 10 );
    lv_obj_set_event_cb( exit_btn, exit_bluetooth_media_event_cb );

    bluetooth_media_play = lv_imgbtn_create( bluetooth_media_tile, NULL);
    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_RELEASED, &play_64px);
    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_PRESSED, &play_64px);
    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_CHECKED_RELEASED, &play_64px);
    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_CHECKED_PRESSED, &play_64px);
    lv_obj_add_style( bluetooth_media_play, LV_IMGBTN_PART_MAIN, &bluetooth_media_style );
    lv_obj_align( bluetooth_media_play, bluetooth_media_tile, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_event_cb( bluetooth_media_play, exit_bluetooth_media_play_event_cb );

    bluetooth_media_next = lv_imgbtn_create( bluetooth_media_tile, NULL);
    lv_imgbtn_set_src( bluetooth_media_next, LV_BTN_STATE_RELEASED, &next_32px);
    lv_imgbtn_set_src( bluetooth_media_next, LV_BTN_STATE_PRESSED, &next_32px);
    lv_imgbtn_set_src( bluetooth_media_next, LV_BTN_STATE_CHECKED_RELEASED, &next_32px);
    lv_imgbtn_set_src( bluetooth_media_next, LV_BTN_STATE_CHECKED_PRESSED, &next_32px);
    lv_obj_add_style( bluetooth_media_next, LV_IMGBTN_PART_MAIN, &bluetooth_media_style );
    lv_obj_align( bluetooth_media_next, bluetooth_media_play, LV_ALIGN_OUT_RIGHT_MID, 32, 0 );
    lv_obj_set_event_cb( bluetooth_media_next, exit_bluetooth_media_next_event_cb );

    bluetooth_media_prev = lv_imgbtn_create( bluetooth_media_tile, NULL);
    lv_imgbtn_set_src( bluetooth_media_prev, LV_BTN_STATE_RELEASED, &prev_32px);
    lv_imgbtn_set_src( bluetooth_media_prev, LV_BTN_STATE_PRESSED, &prev_32px);
    lv_imgbtn_set_src( bluetooth_media_prev, LV_BTN_STATE_CHECKED_RELEASED, &prev_32px);
    lv_imgbtn_set_src( bluetooth_media_prev, LV_BTN_STATE_CHECKED_PRESSED, &prev_32px);
    lv_obj_add_style( bluetooth_media_prev, LV_IMGBTN_PART_MAIN, &bluetooth_media_style );
    lv_obj_align( bluetooth_media_prev, bluetooth_media_play, LV_ALIGN_OUT_LEFT_MID, -32, 0 );
    lv_obj_set_event_cb( bluetooth_media_prev, exit_bluetooth_media_prev_event_cb );

    bluetooth_media_album = lv_label_create( bluetooth_media_tile, NULL);
    lv_obj_add_style( bluetooth_media_album, LV_OBJ_PART_MAIN, &bluetooth_media_style  );
    lv_label_set_text( bluetooth_media_album, "");
    lv_label_set_long_mode( bluetooth_media_album, LV_LABEL_LONG_SROLL_CIRC );
    lv_obj_set_width( bluetooth_media_album, lv_disp_get_hor_res( NULL ) - 60 );
    lv_obj_align( bluetooth_media_album, bluetooth_media_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );

    bluetooth_media_title = lv_label_create( bluetooth_media_tile, NULL);
    lv_obj_add_style( bluetooth_media_title, LV_OBJ_PART_MAIN, &bluetooth_media_style  );
    lv_label_set_text( bluetooth_media_title, "");
    lv_label_set_long_mode( bluetooth_media_title, LV_LABEL_LONG_SROLL_CIRC );
    lv_obj_set_width( bluetooth_media_title, lv_disp_get_hor_res( NULL ) - 20 );
    lv_obj_align( bluetooth_media_title, bluetooth_media_play, LV_ALIGN_OUT_TOP_MID, 0, -16 );

    lv_obj_t *bluetooth_media_speaker = lv_imgbtn_create( bluetooth_media_tile, NULL);
    lv_imgbtn_set_src( bluetooth_media_speaker, LV_BTN_STATE_RELEASED, &sound_32px);
    lv_imgbtn_set_src( bluetooth_media_speaker, LV_BTN_STATE_PRESSED, &sound_32px);
    lv_imgbtn_set_src( bluetooth_media_speaker, LV_BTN_STATE_CHECKED_RELEASED, &sound_32px);
    lv_imgbtn_set_src( bluetooth_media_speaker, LV_BTN_STATE_CHECKED_PRESSED, &sound_32px);
    lv_obj_add_style( bluetooth_media_speaker, LV_IMGBTN_PART_MAIN, &bluetooth_media_style );
    lv_obj_align( bluetooth_media_speaker, bluetooth_media_play, LV_ALIGN_OUT_BOTTOM_MID, 0, 32 );

    lv_obj_t *bluetooth_media_volume_down = lv_imgbtn_create( bluetooth_media_tile, NULL);
    lv_imgbtn_set_src( bluetooth_media_volume_down, LV_BTN_STATE_RELEASED, &down_32px);
    lv_imgbtn_set_src( bluetooth_media_volume_down, LV_BTN_STATE_PRESSED, &down_32px);
    lv_imgbtn_set_src( bluetooth_media_volume_down, LV_BTN_STATE_CHECKED_RELEASED, &down_32px);
    lv_imgbtn_set_src( bluetooth_media_volume_down, LV_BTN_STATE_CHECKED_PRESSED, &down_32px);
    lv_obj_add_style( bluetooth_media_volume_down, LV_IMGBTN_PART_MAIN, &bluetooth_media_style );
    lv_obj_align( bluetooth_media_volume_down, bluetooth_media_speaker, LV_ALIGN_OUT_LEFT_MID, -32, 0 );
    lv_obj_set_event_cb( bluetooth_media_volume_down, exit_bluetooth_media_volume_down_event_cb );

    lv_obj_t *bluetooth_media_volume_up = lv_imgbtn_create( bluetooth_media_tile, NULL);
    lv_imgbtn_set_src( bluetooth_media_volume_up, LV_BTN_STATE_RELEASED, &up_32px);
    lv_imgbtn_set_src( bluetooth_media_volume_up, LV_BTN_STATE_PRESSED, &up_32px);
    lv_imgbtn_set_src( bluetooth_media_volume_up, LV_BTN_STATE_CHECKED_RELEASED, &up_32px);
    lv_imgbtn_set_src( bluetooth_media_volume_up, LV_BTN_STATE_CHECKED_PRESSED, &up_32px);
    lv_obj_add_style( bluetooth_media_volume_up, LV_IMGBTN_PART_MAIN, &bluetooth_media_style );
    lv_obj_align( bluetooth_media_volume_up, bluetooth_media_speaker, LV_ALIGN_OUT_RIGHT_MID, 32, 0 );
    lv_obj_set_event_cb( bluetooth_media_volume_up, exit_bluetooth_media_volume_up_event_cb );

    blectl_register_cb( BLECTL_MSG, bluetooth_media_event_cb, "bluetooth media" );
}


static void exit_bluetooth_media_play_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            if( bluetooth_media_play_state == true ) {
                lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_RELEASED, &play_64px);
                lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_PRESSED, &play_64px);
                lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_CHECKED_RELEASED, &play_64px);
                lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_CHECKED_PRESSED, &play_64px);
                bluetooth_media_play_state = false;
                blectl_send_msg( (char*)"\r\n{t:\"music\", n:\"pause\"}\r\n" );
            }
            else {
                lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_RELEASED, &pause_64px);
                lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_PRESSED, &pause_64px);
                lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_CHECKED_RELEASED, &pause_64px);
                lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_CHECKED_PRESSED, &pause_64px);
                bluetooth_media_play_state = true;
                blectl_send_msg( (char*)"\r\n{t:\"music\", n:\"play\"}\r\n" );
            }
            break;
    }
}

static void exit_bluetooth_media_volume_up_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            blectl_send_msg( (char*)"\r\n{t:\"music\", n:\"volumeup\"}\r\n" );
            break;
    }
}

static void exit_bluetooth_media_volume_down_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            blectl_send_msg( (char*)"\r\n{t:\"music\", n:\"volumedown\"}\r\n" );
            break;
    }
}

static void exit_bluetooth_media_next_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            blectl_send_msg( (char*)"\r\n{t:\"music\", n:\"next\"}\r\n" );
            break;
    }
}

static void exit_bluetooth_media_prev_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            blectl_send_msg( (char*)"\r\n{t:\"music\", n:\"previous\"}\r\n" );
            break;
    }
}

static void exit_bluetooth_media_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

bool bluetooth_media_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_MSG:            
            bluetooth_media_queue_msg( (const char*)arg );
            break;
    }
    return( true );
}

bool bluetooth_media_queue_msg( const char *msg ) {
    bool retval = false;
    
    SpiRamJsonDocument doc( strlen( msg ) * 4 );

    DeserializationError error = deserializeJson( doc, msg );
    if ( error ) {
        log_e("bluetooth message deserializeJson() failed: %s", error.c_str() );
    }
    else {
        if( !strcmp( doc["t"], "musicstate" ) ) {
            if( doc["state"] ) {
                if( !strcmp( doc["state"], "pause" ) ) {
                    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_RELEASED, &play_64px);
                    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_PRESSED, &play_64px);
                    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_CHECKED_RELEASED, &play_64px);
                    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_CHECKED_PRESSED, &play_64px);
                    bluetooth_media_play_state = false;
                }
                if( !strcmp( doc["state"], "play" ) ) {
                    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_RELEASED, &pause_64px);
                    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_PRESSED, &pause_64px);
                    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_CHECKED_RELEASED, &pause_64px);
                    lv_imgbtn_set_src( bluetooth_media_play, LV_BTN_STATE_CHECKED_PRESSED, &pause_64px);                    
                    bluetooth_media_play_state = true;
                }
            }
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            statusbar_hide( true );
            mainbar_jump_to_tilenumber( bluetooth_media_tile_num, LV_ANIM_OFF );
            retval = true;
        }
        if( !strcmp( doc["t"], "musicinfo" ) ) {
            if ( doc["track"] ) {
                lv_label_set_text( bluetooth_media_title, doc["track"] );
                lv_obj_align( bluetooth_media_title, bluetooth_media_play, LV_ALIGN_OUT_TOP_MID, 0, -16 );
            }
            
            if ( doc["album"] ) {
                lv_label_set_text( bluetooth_media_album, doc["album"] );
                lv_obj_align( bluetooth_media_album, bluetooth_media_tile, LV_ALIGN_OUT_TOP_MID, 0, -16 );
            }

            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            statusbar_hide( true );
            mainbar_jump_to_tilenumber( bluetooth_media_tile_num, LV_ANIM_OFF );
            retval = true;
        }
    }
    return( retval );
}