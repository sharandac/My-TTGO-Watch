/****************************************************************************
 *   Aug 18 12:37:31 2020
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
#include "bluetooth_message.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/sound/piep.h"
#include "hardware/blectl.h"
#include "hardware/powermgm.h"
#include "hardware/motor.h"
#include "hardware/json_psram_allocator.h"
#include "hardware/sound.h"
#include "hardware/alloc.h"
#include "hardware/msg_chain.h"

lv_obj_t *bluetooth_message_tile=NULL;
lv_style_t bluetooth_message_style;
lv_style_t bluetooth_message_sender_style;
uint32_t bluetooth_message_tile_num;

lv_obj_t *bluetooth_message_img = NULL;
lv_obj_t *bluetooth_message_notify_source_label = NULL;
lv_obj_t *bluetooth_message_sender_label = NULL;
lv_obj_t *bluetooth_message_msg_label = NULL;
lv_obj_t *bluetooth_message_page = NULL;
lv_obj_t *bluetooth_prev_msg_btn = NULL;
lv_obj_t *bluetooth_next_msg_btn = NULL;
lv_obj_t *bluetooth_trash_msg_btn = NULL;
lv_obj_t *bluetooth_msg_entrys_label = NULL;

msg_chain_t *bluetooth_msg_chain = NULL;
static bool bluetooth_message_active = true;
int32_t bluetooth_current_msg = -1;

LV_IMG_DECLARE(cancel_32px);
LV_IMG_DECLARE(telegram_32px);
LV_IMG_DECLARE(whatsapp_32px);
LV_IMG_DECLARE(k9mail_32px);
LV_IMG_DECLARE(email_32px);
LV_IMG_DECLARE(message_32px);
LV_IMG_DECLARE(osmand_32px);
LV_IMG_DECLARE(youtube_32px);
LV_IMG_DECLARE(instagram_32px);
LV_IMG_DECLARE(tinder_32px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);
LV_FONT_DECLARE(trash_32px);
LV_FONT_DECLARE(left_32px);
LV_FONT_DECLARE(right_32px);

src_icon_t src_icon[] = {
    { "Telegram", 50, &telegram_32px },
    { "WhatsApp", 50, &whatsapp_32px },
    { "K-9 Mail", 50, &k9mail_32px },
    { "Gmail", 50, &email_32px },
    { "E-Mail", 50, &message_32px },
    { "OsmAnd", 0, &osmand_32px },
    { "YouTube", 50, &youtube_32px },
    { "Instagram", 50, &instagram_32px },
    { "Tinder", 50, &tinder_32px },
    { "", 0, NULL }
};

static void bluetooth_prev_message_event_cb( lv_obj_t * obj, lv_event_t event );
static void bluetooth_next_message_event_cb( lv_obj_t * obj, lv_event_t event );
static void bluetooth_del_message_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_message_event_cb( lv_obj_t * obj, lv_event_t event );
bool bluetooth_message_event_cb( EventBits_t event, void *arg );
const lv_img_dsc_t *bluetooth_message_find_img( const char * src_name );

void bluetooth_add_msg_to_chain( const char *msg );
bool bluetooth_message_queue_msg( const char *msg );
bool bluetooth_delete_msg_from_chain( int32_t entry );
int32_t bluetooth_get_msg_entrys( void );
const char* bluetooth_get_msg_entry( int32_t entry );
void bluetooth_print_msg_chain( void );
void bluetooth_message_show_msg( int32_t entry );

void bluetooth_message_tile_setup( void ) {
    // get an app tile and copy mainstyle
    bluetooth_message_tile_num = mainbar_add_app_tile( 1, 1, "bluetooth message" );
    bluetooth_message_tile = mainbar_get_tile_obj( bluetooth_message_tile_num );

    lv_style_copy( &bluetooth_message_style, mainbar_get_style() );
    lv_style_set_bg_color( &bluetooth_message_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &bluetooth_message_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &bluetooth_message_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font( &bluetooth_message_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_obj_add_style( bluetooth_message_tile, LV_OBJ_PART_MAIN, &bluetooth_message_style );

    lv_style_copy( &bluetooth_message_sender_style, &bluetooth_message_style );
    lv_style_set_text_font( &bluetooth_message_sender_style, LV_STATE_DEFAULT, &Ubuntu_32px);

    bluetooth_message_img = lv_img_create( bluetooth_message_tile, NULL );
    lv_img_set_src( bluetooth_message_img, &cancel_32px );
    lv_obj_align( bluetooth_message_img, bluetooth_message_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );

    bluetooth_message_notify_source_label = lv_label_create( bluetooth_message_tile, NULL);
    lv_obj_add_style( bluetooth_message_notify_source_label, LV_OBJ_PART_MAIN, &bluetooth_message_sender_style  );
    lv_label_set_text( bluetooth_message_notify_source_label, "");
    lv_obj_align( bluetooth_message_notify_source_label, bluetooth_message_img, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    bluetooth_message_sender_label = lv_label_create( bluetooth_message_tile, NULL);
    lv_obj_add_style( bluetooth_message_sender_label, LV_OBJ_PART_MAIN, &bluetooth_message_style  );
    lv_label_set_text( bluetooth_message_sender_label, "");
    lv_obj_align( bluetooth_message_sender_label, bluetooth_message_img, LV_ALIGN_OUT_BOTTOM_LEFT, 5, 5 );

    bluetooth_message_page = lv_page_create( bluetooth_message_tile, NULL);
    lv_obj_set_size( bluetooth_message_page, lv_disp_get_hor_res( NULL ) - 20, 120 );
    lv_obj_add_style( bluetooth_message_page, LV_OBJ_PART_MAIN, &bluetooth_message_style );
    lv_page_set_scrlbar_mode( bluetooth_message_page, LV_SCRLBAR_MODE_DRAG );
    lv_obj_align( bluetooth_message_page, bluetooth_message_sender_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5 );

    bluetooth_message_msg_label = lv_label_create( bluetooth_message_page, NULL );
    lv_label_set_long_mode( bluetooth_message_msg_label, LV_LABEL_LONG_BREAK );
    lv_obj_set_width( bluetooth_message_msg_label, lv_page_get_width_fit ( bluetooth_message_page ) );
    lv_obj_add_style( bluetooth_message_msg_label, LV_OBJ_PART_MAIN, &bluetooth_message_style );
    lv_label_set_text( bluetooth_message_msg_label, "");

    lv_obj_t *exit_btn = lv_imgbtn_create( bluetooth_message_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &cancel_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &bluetooth_message_style );
    lv_obj_align( exit_btn, bluetooth_message_tile, LV_ALIGN_IN_TOP_RIGHT, -10, 10 );
    lv_obj_set_event_cb( exit_btn, exit_bluetooth_message_event_cb );

    bluetooth_prev_msg_btn = lv_imgbtn_create( bluetooth_message_tile, NULL);
    lv_imgbtn_set_src( bluetooth_prev_msg_btn, LV_BTN_STATE_RELEASED, &left_32px);
    lv_imgbtn_set_src( bluetooth_prev_msg_btn, LV_BTN_STATE_PRESSED, &left_32px);
    lv_imgbtn_set_src( bluetooth_prev_msg_btn, LV_BTN_STATE_CHECKED_RELEASED, &left_32px);
    lv_imgbtn_set_src( bluetooth_prev_msg_btn, LV_BTN_STATE_CHECKED_PRESSED, &left_32px);
    lv_obj_add_style( bluetooth_prev_msg_btn, LV_IMGBTN_PART_MAIN, &bluetooth_message_style );
    lv_obj_align( bluetooth_prev_msg_btn, bluetooth_message_tile, LV_ALIGN_IN_BOTTOM_LEFT, 5, -5 );
    lv_obj_set_event_cb( bluetooth_prev_msg_btn, bluetooth_prev_message_event_cb );

    bluetooth_next_msg_btn = lv_imgbtn_create( bluetooth_message_tile, NULL);
    lv_imgbtn_set_src( bluetooth_next_msg_btn, LV_BTN_STATE_RELEASED, &right_32px);
    lv_imgbtn_set_src( bluetooth_next_msg_btn, LV_BTN_STATE_PRESSED, &right_32px);
    lv_imgbtn_set_src( bluetooth_next_msg_btn, LV_BTN_STATE_CHECKED_RELEASED, &right_32px);
    lv_imgbtn_set_src( bluetooth_next_msg_btn, LV_BTN_STATE_CHECKED_PRESSED, &right_32px);
    lv_obj_add_style( bluetooth_next_msg_btn, LV_IMGBTN_PART_MAIN, &bluetooth_message_style );
    lv_obj_align( bluetooth_next_msg_btn, bluetooth_message_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -5, -5 );
    lv_obj_set_event_cb( bluetooth_next_msg_btn, bluetooth_next_message_event_cb );

    bluetooth_trash_msg_btn = lv_imgbtn_create( bluetooth_message_tile, NULL);
    lv_imgbtn_set_src( bluetooth_trash_msg_btn, LV_BTN_STATE_RELEASED, &trash_32px);
    lv_imgbtn_set_src( bluetooth_trash_msg_btn, LV_BTN_STATE_PRESSED, &trash_32px);
    lv_imgbtn_set_src( bluetooth_trash_msg_btn, LV_BTN_STATE_CHECKED_RELEASED, &trash_32px);
    lv_imgbtn_set_src( bluetooth_trash_msg_btn, LV_BTN_STATE_CHECKED_PRESSED, &trash_32px);
    lv_obj_add_style( bluetooth_trash_msg_btn, LV_IMGBTN_PART_MAIN, &bluetooth_message_style );
    lv_obj_align( bluetooth_trash_msg_btn, bluetooth_prev_msg_btn, LV_ALIGN_OUT_RIGHT_MID, 15, 0 );
    lv_obj_set_event_cb( bluetooth_trash_msg_btn, bluetooth_del_message_event_cb );

    bluetooth_msg_entrys_label = lv_label_create( bluetooth_message_tile, NULL );
    lv_obj_add_style( bluetooth_msg_entrys_label, LV_OBJ_PART_MAIN, &bluetooth_message_sender_style );
    lv_label_set_text( bluetooth_msg_entrys_label, "1/1");
    lv_obj_align( bluetooth_msg_entrys_label, bluetooth_next_msg_btn, LV_ALIGN_OUT_LEFT_MID, -5, 0 );

    blectl_register_cb( BLECTL_MSG, bluetooth_message_event_cb, "bluetooth_message" );
}

static void bluetooth_prev_message_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            if ( bluetooth_current_msg > 0 ) {
                bluetooth_current_msg--;
                bluetooth_message_show_msg( bluetooth_current_msg );
            }
            break;
    }
}

static void bluetooth_next_message_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            if ( bluetooth_current_msg < ( msg_chain_get_entrys( bluetooth_msg_chain ) - 1 ) ) {
                bluetooth_current_msg++;
                bluetooth_message_show_msg( bluetooth_current_msg );
//                msg_chain_printf_msg_chain( bluetooth_msg_chain );
            }
            break;
        
    }
}

static void bluetooth_del_message_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            if ( msg_chain_get_entrys( bluetooth_msg_chain ) == 1 ) {
                msg_chain_delete_msg_entry( bluetooth_msg_chain, bluetooth_current_msg );
                bluetooth_current_msg--;
                mainbar_jump_to_maintile( LV_ANIM_OFF );
            }
            else {
                if ( bluetooth_current_msg == ( msg_chain_get_entrys( bluetooth_msg_chain ) - 1 ) ) {
                    msg_chain_delete_msg_entry( bluetooth_msg_chain, bluetooth_current_msg );
                    bluetooth_current_msg--;
                    bluetooth_message_show_msg( bluetooth_current_msg );
                }
                else {
                    msg_chain_delete_msg_entry( bluetooth_msg_chain, bluetooth_current_msg );
                    bluetooth_message_show_msg( bluetooth_current_msg );
                }
            }
    }
}

bool bluetooth_message_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_MSG:            
            if ( bluetooth_message_queue_msg( (const char*)arg ) ) {
                bluetooth_message_show_msg( msg_chain_get_entrys( bluetooth_msg_chain ) - 1 );
                bluetooth_current_msg = msg_chain_get_entrys( bluetooth_msg_chain ) - 1;
            }
            break;
    }
    return( true );
}

static void exit_bluetooth_message_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

void bluetooth_message_disable( void ) {
    bluetooth_message_active = false;
}

void bluetooth_message_enable( void ) {
    bluetooth_message_active = true;    
}

const lv_img_dsc_t *bluetooth_message_find_img( const char * src_name ) {
    for ( int i = 0; src_icon[ i ].img != NULL; i++ ) {
        if ( strstr( src_name, src_icon[ i ].src_name ) ) {
            log_i("hit: %s -> %s", src_name, src_icon[ i ].src_name );
            return( src_icon[ i ].img );
        }
    }
    return( &message_32px );
}

bool bluetooth_message_queue_msg( const char *msg ) {
    bool retval = false;

    if ( bluetooth_message_active == false ) {
        return( false );
    }
    
    SpiRamJsonDocument doc( strlen( msg ) * 4 );

    DeserializationError error = deserializeJson( doc, msg );
    if ( error ) {
        log_e("bluetooth message deserializeJson() failed: %s", error.c_str() );
    }
    else {
        if( !strcmp( doc["t"], "notify" ) ) {
            bluetooth_msg_chain = msg_chain_add_msg( bluetooth_msg_chain, msg );
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
            mainbar_jump_to_tilenumber( bluetooth_message_tile_num, LV_ANIM_OFF );
            sound_play_progmem_wav( piep_wav, piep_wav_len );
            motor_vibe(10);
            retval = true;
        }
    }
    return( retval );
}

void bluetooth_message_show_msg( int32_t entry ) {
    char msg_num[16] = "";

    const char *msg = msg_chain_get_msg_entry( bluetooth_msg_chain, entry );
    if ( msg == NULL ) {
        return;
    }

    if ( entry > 0 ) {
        lv_obj_set_hidden( bluetooth_prev_msg_btn, false );
    }
    else {
        lv_obj_set_hidden( bluetooth_prev_msg_btn, true );
    }

    if ( entry < ( msg_chain_get_entrys( bluetooth_msg_chain ) -1 ) ) {
        lv_obj_set_hidden( bluetooth_next_msg_btn, false );
    }
    else {
        lv_obj_set_hidden( bluetooth_next_msg_btn, true );
    }

    SpiRamJsonDocument doc( strlen( msg ) * 4 );

    DeserializationError error = deserializeJson( doc, msg );
    if ( error ) {
        log_e("bluetooth message deserializeJson() failed: %s", error.c_str() );
    }
    else {
        if( !strcmp( doc["t"], "notify" ) ) {

            snprintf( msg_num, sizeof( msg_num ), "%d/%d", entry + 1, msg_chain_get_entrys( bluetooth_msg_chain ) );
            lv_label_set_text( bluetooth_msg_entrys_label, msg_num );
            lv_obj_align( bluetooth_msg_entrys_label, bluetooth_next_msg_btn, LV_ALIGN_OUT_LEFT_MID, -5, 0 );

            statusbar_hide( true );

            // set notify source icon
            if ( doc["src"] ) {
                lv_img_set_src( bluetooth_message_img, bluetooth_message_find_img( doc["src"] ) ); 
                lv_label_set_text( bluetooth_message_notify_source_label, doc["src"] );
            }
            else {
                lv_img_set_src( bluetooth_message_img, &message_32px );
                lv_label_set_text( bluetooth_message_notify_source_label, "Message" );
            }
            
            // set message
            if ( doc["body"] )
                lv_label_set_text( bluetooth_message_msg_label, doc["body"] );
            else if ( doc["title"] )
                lv_label_set_text( bluetooth_message_msg_label, doc["title"] );
            else 
                lv_label_set_text( bluetooth_message_msg_label, "" );

            // scroll back to the top
            if ( lv_page_get_scrl_height( bluetooth_message_page ) > 160 )
                lv_page_scroll_ver( bluetooth_message_page, lv_page_get_scrl_height( bluetooth_message_page ) );
            
            // set sender label
            if ( doc["title"] )
                lv_label_set_text( bluetooth_message_sender_label, doc["title"] );
            else if ( doc["sender"] )
                lv_label_set_text( bluetooth_message_sender_label, doc["sender"] );
            else if( doc["tel"] ) 
                lv_label_set_text( bluetooth_message_sender_label, doc["tel"] );
            else
                lv_label_set_text( bluetooth_message_sender_label, "n/a" );

            lv_obj_invalidate( lv_scr_act() );
        }
    }        
    doc.clear();
}
