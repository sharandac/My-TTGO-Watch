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
#include "ArduinoJson.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/app.h"
#include "gui/widget.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"
#include "gui/statusbar.h"
#include "gui/sound/piep.h"

#include "hardware/blectl.h"
#include "hardware/ble/gadgetbridge.h"
#include "hardware/powermgm.h"
#include "hardware/motor.h"
#include "hardware/sound.h"
#include "hardware/button.h"

#include "utils/alloc.h"
#include "utils/msg_chain.h"
#include "utils/bluejsonrequest.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
    #include <string>

    using namespace std;
    #define String string
#else
    #include <Arduino.h>
#endif

// messages app and widget
icon_t *messages_app = NULL;
icon_t *messages_widget = NULL;

lv_obj_t *bluetooth_message_tile=NULL;
lv_style_t bluetooth_message_style;
lv_style_t bluetooth_message_sender_style;
uint32_t bluetooth_message_tile_num;

lv_obj_t *bluetooth_message_img = NULL;
lv_obj_t *bluetooth_message_notify_source_label = NULL;
lv_obj_t *bluetooth_message_time_label = NULL;
lv_obj_t *bluetooth_message_sender_label = NULL;
lv_obj_t *bluetooth_message_msg_label = NULL;
lv_obj_t *bluetooth_message_page = NULL;
lv_obj_t *bluetooth_message_entrys_label = NULL;
lv_obj_t *bluetooth_message_exit_btn = NULL;
lv_obj_t *bluetooth_message_prev_msg_btn = NULL;
lv_obj_t *bluetooth_message_next_msg_btn = NULL;
lv_obj_t *bluetooth_message_trash_msg_btn = NULL;
lv_obj_t *bluetooth_message_msg_entrys_label = NULL;

msg_chain_t *bluetooth_msg_chain = NULL;
static bool bluetooth_message_active = true;
static bool bluetooth_message_tile_active = false;
int32_t bluetooth_current_msg = -1;

LV_FONT_DECLARE(Ubuntu_12px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);
LV_FONT_DECLARE(Ubuntu_72px);

LV_IMG_DECLARE(message_32px);
LV_IMG_DECLARE(message_48px);
LV_IMG_DECLARE(message_64px);
LV_IMG_DECLARE(message_96px);

#if defined( BIG_THEME )
    #define default_msg_icon        &message_96px

    LV_IMG_DECLARE(telegram_96px);
    LV_IMG_DECLARE(signal_96px);
    LV_IMG_DECLARE(whatsapp_96px);
    LV_IMG_DECLARE(k9mail_96px);
    LV_IMG_DECLARE(email_96px);
    LV_IMG_DECLARE(osmand_96px);
    LV_IMG_DECLARE(youtube_96px);
    LV_IMG_DECLARE(instagram_96px);
    LV_IMG_DECLARE(tinder_96px);
    LV_IMG_DECLARE(joyce_96px);
    LV_IMG_DECLARE(update_96px);

    src_icon_t src_icon[] = {
        { "Telegram", true, &telegram_96px },
        { "Signal", true, &signal_96px },
        { "WhatsApp", true, &whatsapp_96px },
        { "K-9 Mail", true, &k9mail_96px },
        { "Gmail", true, &email_96px },
        { "E-Mail", true, &message_96px },
        { "OsmAnd", false, &osmand_96px },
        { "YouTube", false, &youtube_96px },
        { "Instagram", false, &instagram_96px },
        { "Tinder", false, &tinder_96px },
        { "JOYCE", false, &tinder_96px },
        { "Update", false, &update_96px },
        { "", NULL }
    };

    lv_font_t *message_title_font = &Ubuntu_72px;
    lv_font_t *message_font = &Ubuntu_32px;
#elif defined( MID_THEME )
    #define default_msg_icon        &message_64px

    LV_IMG_DECLARE(telegram_64px);
    LV_IMG_DECLARE(signal_64px);
    LV_IMG_DECLARE(whatsapp_64px);
    LV_IMG_DECLARE(k9mail_64px);
    LV_IMG_DECLARE(email_64px);
    LV_IMG_DECLARE(osmand_64px);
    LV_IMG_DECLARE(youtube_64px);
    LV_IMG_DECLARE(instagram_64px);
    LV_IMG_DECLARE(tinder_64px);
    LV_IMG_DECLARE(joyce_64px);
    LV_IMG_DECLARE(update_64px);

    src_icon_t src_icon[] = {
        { "Telegram", true, &telegram_64px },
        { "Signal", true, &signal_64px },
        { "WhatsApp", true, &whatsapp_64px },
        { "K-9 Mail", true, &k9mail_64px },
        { "Gmail", true, &email_64px },
        { "E-Mail", true, &message_64px },
        { "OsmAnd", false, &osmand_64px },
        { "YouTube", false, &youtube_64px },
        { "Instagram", false, &instagram_64px },
        { "Tinder", false, &tinder_64px },
        { "JOYCE", false, &tinder_64px },
        { "Update", false, &update_64px },
        { "", NULL }
    };

    lv_font_t *message_title_font = &Ubuntu_32px;
    lv_font_t *message_font = &Ubuntu_16px;
#else
    #define default_msg_icon        &message_32px

    LV_IMG_DECLARE(telegram_32px);
    LV_IMG_DECLARE(signal_32px);
    LV_IMG_DECLARE(whatsapp_32px);
    LV_IMG_DECLARE(k9mail_32px);
    LV_IMG_DECLARE(email_32px);
    LV_IMG_DECLARE(osmand_32px);
    LV_IMG_DECLARE(youtube_32px);
    LV_IMG_DECLARE(instagram_32px);
    LV_IMG_DECLARE(tinder_32px);    
    LV_IMG_DECLARE(joyce_32px);
    LV_IMG_DECLARE(update_32px);

    src_icon_t src_icon[] = {
        { "Telegram", true, &telegram_32px },
        { "Signal", true, &signal_32px },
        { "WhatsApp", true, &whatsapp_32px },
        { "K-9 Mail", true, &k9mail_32px },
        { "Gmail", true, &email_32px },
        { "E-Mail", true, &message_32px },
        { "OsmAnd", false, &osmand_32px },
        { "YouTube", false, &youtube_32px },
        { "Instagram", false, &instagram_32px },
        { "Tinder", false, &tinder_32px },
        { "JOYCE", false, &tinder_32px },
        { "Update", false, &update_32px },
        { "", NULL }
    };

    lv_font_t *message_title_font = &Ubuntu_32px;
    lv_font_t *message_font = &Ubuntu_16px;
#endif

static void bluetooth_message_set_indicator( void );
static bool bluetooth_message_button_event_cb( EventBits_t event, void *arg );
static void bluetooth_message_activate_cb( void );
static void bluetooth_message_hibernate_cb( void );
static bool bluetooth_message_style_change_event_cb( EventBits_t event, void *arg );
static void bluetooth_prev_message_event_cb( lv_obj_t * obj, lv_event_t event );
static void bluetooth_next_message_event_cb( lv_obj_t * obj, lv_event_t event );
static void bluetooth_del_message_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_message_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_bluetooth_messages_cb( lv_obj_t * obj, lv_event_t event );
bool bluetooth_message_event_cb( EventBits_t event, void *arg );
const lv_img_dsc_t *bluetooth_message_find_img( const char * src_name );

static void bluetooth_message_send_del_json( int32_t entry );
void bluetooth_add_msg_to_chain( const char *msg );
static bool bluetooth_message_queue_msg( BluetoothJsonRequest &doc );
bool bluetooth_message_queue_msg( const char *msg );
bool bluetooth_delete_msg_from_chain( int32_t entry );
int32_t bluetooth_get_msg_entrys( void );
int32_t bluetooth_get_number_of_msg( void );
const char* bluetooth_get_msg_entry( int32_t entry );
void bluetooth_print_msg_chain( void );
void bluetooth_message_show_msg( int32_t entry );
void bluetooth_message_play_audio( int32_t entry );

void bluetooth_message_tile_setup( void ) {
    /*
     * get an app tile and copy mainstyle
     */
    bluetooth_message_tile_num = mainbar_add_app_tile( 1, 1, "bluetooth message" );
    bluetooth_message_tile = mainbar_get_tile_obj( bluetooth_message_tile_num );

    lv_style_copy( &bluetooth_message_style, APP_STYLE );
    lv_style_set_text_font( &bluetooth_message_style, LV_STATE_DEFAULT, message_font );
    lv_obj_add_style( bluetooth_message_tile, LV_OBJ_PART_MAIN, &bluetooth_message_style );

    lv_style_copy( &bluetooth_message_sender_style, &bluetooth_message_style );
    lv_style_set_text_font( &bluetooth_message_sender_style, LV_STATE_DEFAULT, message_title_font );

    lv_obj_t *bluettoth_message_img_cont = lv_cont_create( bluetooth_message_tile, NULL );
    lv_obj_set_size( bluettoth_message_img_cont, THEME_ICON_SIZE, THEME_ICON_SIZE );
    lv_obj_add_style( bluettoth_message_img_cont, LV_OBJ_PART_MAIN, APP_STYLE  );
    lv_obj_align( bluettoth_message_img_cont, bluetooth_message_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );

    bluetooth_message_img = lv_img_create( bluettoth_message_img_cont, NULL );
    lv_img_set_src( bluetooth_message_img, default_msg_icon );
    lv_obj_align( bluetooth_message_img, bluettoth_message_img_cont, LV_ALIGN_CENTER, 0, 0 );

    bluetooth_message_notify_source_label = lv_label_create( bluetooth_message_tile, NULL);
    lv_obj_add_style( bluetooth_message_notify_source_label, LV_OBJ_PART_MAIN, &bluetooth_message_sender_style  );
    lv_label_set_text( bluetooth_message_notify_source_label, "E-Mail" );
    lv_obj_align( bluetooth_message_notify_source_label, bluettoth_message_img_cont, LV_ALIGN_OUT_RIGHT_MID, THEME_PADDING, 0 );

    bluetooth_message_time_label = lv_label_create( bluetooth_message_tile, NULL);
    lv_obj_add_style( bluetooth_message_time_label, LV_OBJ_PART_MAIN, &bluetooth_message_style  );
    lv_label_set_text( bluetooth_message_time_label, "23:42");
    lv_obj_align( bluetooth_message_time_label, bluettoth_message_img_cont, LV_ALIGN_OUT_BOTTOM_LEFT, THEME_PADDING, THEME_PADDING );

    bluetooth_message_sender_label = lv_label_create( bluetooth_message_tile, NULL);
    lv_obj_add_style( bluetooth_message_sender_label, LV_OBJ_PART_MAIN, &bluetooth_message_style  );
    lv_label_set_text( bluetooth_message_sender_label, "foo");
    lv_obj_align( bluetooth_message_sender_label, bluetooth_message_time_label, LV_ALIGN_OUT_RIGHT_MID, THEME_PADDING, 0 );

    bluetooth_message_page = lv_page_create( bluetooth_message_tile, NULL);
    lv_obj_set_size( bluetooth_message_page, lv_disp_get_hor_res( NULL ) - ( 2 + THEME_PADDING), lv_disp_get_ver_res( NULL ) - ( 3 * THEME_ICON_SIZE ) );
    lv_obj_add_style( bluetooth_message_page, LV_OBJ_PART_MAIN, &bluetooth_message_style );
    lv_page_set_scrlbar_mode( bluetooth_message_page, LV_SCRLBAR_MODE_DRAG );
    lv_obj_align( bluetooth_message_page, bluetooth_message_time_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, THEME_PADDING );

    bluetooth_message_msg_label = lv_label_create( bluetooth_message_page, NULL );
    lv_label_set_long_mode( bluetooth_message_msg_label, LV_LABEL_LONG_BREAK );
    lv_obj_set_width( bluetooth_message_msg_label, lv_page_get_width_fit ( bluetooth_message_page ) );
    lv_obj_add_style( bluetooth_message_msg_label, LV_OBJ_PART_MAIN, &bluetooth_message_style );
    lv_label_set_text( bluetooth_message_msg_label, "Test message from bar.");


    bluetooth_message_exit_btn = wf_add_close_button( bluetooth_message_tile, exit_bluetooth_message_event_cb );
    lv_obj_align( bluetooth_message_exit_btn, bluetooth_message_tile, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING, THEME_PADDING );

    bluetooth_message_prev_msg_btn = wf_add_left_button( bluetooth_message_tile, bluetooth_prev_message_event_cb );
    lv_obj_align( bluetooth_message_prev_msg_btn, bluetooth_message_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_PADDING, -THEME_PADDING );

    bluetooth_message_next_msg_btn = wf_add_right_button( bluetooth_message_tile, bluetooth_next_message_event_cb );
    lv_obj_align( bluetooth_message_next_msg_btn, bluetooth_message_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING, -THEME_PADDING );

    bluetooth_message_trash_msg_btn = wf_add_trash_button( bluetooth_message_tile, bluetooth_del_message_event_cb );
    lv_obj_align( bluetooth_message_trash_msg_btn, bluetooth_message_prev_msg_btn, LV_ALIGN_OUT_RIGHT_MID, 0, 0 );

    bluetooth_message_entrys_label = lv_label_create( bluetooth_message_tile, NULL );
    lv_obj_add_style( bluetooth_message_entrys_label, LV_OBJ_PART_MAIN, &bluetooth_message_sender_style );
    lv_label_set_text( bluetooth_message_entrys_label, "1/1");
    lv_obj_align( bluetooth_message_entrys_label, bluetooth_message_next_msg_btn, LV_ALIGN_OUT_LEFT_MID, 0, 0 );

    gadgetbridge_register_cb( GADGETBRIDGE_JSON_MSG, bluetooth_message_event_cb, "bluetooth_message" );
    styles_register_cb( STYLE_CHANGE, bluetooth_message_style_change_event_cb, "bluetooth message style" );
    button_register_cb( BUTTON_NOTIFY_TEST | BUTTON_NOTIFY_DEL_TEST, bluetooth_message_button_event_cb, "bluetooth message button cb" );
    mainbar_add_tile_button_cb( bluetooth_message_tile_num, bluetooth_message_button_event_cb );
    mainbar_add_tile_activate_cb( bluetooth_message_tile_num, bluetooth_message_activate_cb );
    mainbar_add_tile_hibernate_cb( bluetooth_message_tile_num, bluetooth_message_hibernate_cb );
    messages_app = app_register( "messages", &message_64px, enter_bluetooth_messages_cb );
}

static bool bluetooth_message_button_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_EXIT:           
            mainbar_jump_back();
            break;
        case BUTTON_NOTIFY_TEST:
            gadgetbridge_send_loop_msg( "{\"t\":\"notify\",\"id\":1654906064,\"src\":\"K-9 Mail\",\"title\":\"foo\",\"body\":\"bar 23\"}" );
            break;
        case BUTTON_NOTIFY_DEL_TEST:
            gadgetbridge_send_loop_msg( "{\"t\":\"notify-\",\"id\":1654906064}" );
            break;
    }

    return( true );
}

static void bluetooth_message_activate_cb( void ) {
    wf_image_button_fade_in( bluetooth_message_exit_btn, 300, 0 );
    if( !lv_obj_get_hidden( bluetooth_message_prev_msg_btn ) )
        wf_image_button_fade_in( bluetooth_message_prev_msg_btn, 300, 100 );
    if( !lv_obj_get_hidden( bluetooth_message_next_msg_btn ) )
        wf_image_button_fade_in( bluetooth_message_next_msg_btn, 300, 100 );
    if( !lv_obj_get_hidden( bluetooth_message_trash_msg_btn ) )
        wf_image_button_fade_in( bluetooth_message_trash_msg_btn, 300, 1000 );

    bluetooth_message_tile_active = true;
}

static void bluetooth_message_hibernate_cb( void ) {
    bluetooth_message_tile_active = false;
}

static bool bluetooth_message_style_change_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_CHANGE:  lv_style_copy( &bluetooth_message_style, APP_STYLE );
                            lv_style_set_text_font( &bluetooth_message_style, LV_STATE_DEFAULT, message_font );
                            lv_style_copy( &bluetooth_message_sender_style, &bluetooth_message_style );
                            lv_style_set_text_font( &bluetooth_message_sender_style, LV_STATE_DEFAULT, message_title_font );
                            break;
    }
    return( true );
}

static void enter_bluetooth_messages_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
            if ( msg_chain_get_entrys( bluetooth_msg_chain ) > 0 ) {
                bluetooth_message_show_msg( msg_chain_get_entrys( bluetooth_msg_chain ) - 1 );
                mainbar_jump_to_tilenumber( bluetooth_message_tile_num, LV_ANIM_OFF, true );
            }
            break;
        case ( LV_EVENT_LONG_PRESSED ):             
            break;
    }    
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
            }
            break;
        
    }
}

static void bluetooth_del_message_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            bluetooth_delete_msg_from_chain( bluetooth_current_msg );
            break;
    }
}

bool bluetooth_delete_msg_from_chain( int32_t entry ) {
    if( msg_chain_get_entrys( bluetooth_msg_chain ) == 0 ) {
        return( false );
    }
    else if ( msg_chain_get_entrys( bluetooth_msg_chain ) == 1 ) {
        bluetooth_message_send_del_json( entry );
        msg_chain_delete_msg_entry( bluetooth_msg_chain, entry );
        bluetooth_current_msg--;

        if( bluetooth_message_tile_active )
            mainbar_jump_back();
    }
    else {
        if ( bluetooth_current_msg == ( msg_chain_get_entrys( bluetooth_msg_chain ) - 1 ) ) {
            bluetooth_message_send_del_json( entry );
            msg_chain_delete_msg_entry( bluetooth_msg_chain, entry );
            bluetooth_current_msg--;
            bluetooth_message_show_msg( bluetooth_current_msg );
        }
        else {
            bluetooth_message_send_del_json( entry );
            msg_chain_delete_msg_entry( bluetooth_msg_chain, entry );
            bluetooth_message_show_msg( bluetooth_current_msg );
        }
    }

    bluetooth_message_set_indicator();

    return( true );
}

static void bluetooth_message_send_del_json( int32_t entry ) {
    const char *msg = bluetooth_get_msg_entry( entry );

    BluetoothJsonRequest request( msg, strlen( msg ) * 4 );
    if ( request.isValid() ) {
        if( request.containsKey("id") ) {
            gadgetbridge_send_msg("\r\n{\"t\":\"notify-\",\"id\":%ld}\r\n", request["id"].as<long>() );
        }
    }
    request.clear();   
}

static void bluetooth_message_set_indicator( void ) {
    switch ( msg_chain_get_entrys( bluetooth_msg_chain ) ) {
        case 0:
            app_hide_indicator( messages_app );
            messages_widget = widget_remove( messages_widget );
            break;
        case 1:
            widget_set_indicator( messages_widget, ICON_INDICATOR_1 );
            app_set_indicator( messages_app, ICON_INDICATOR_1 );
            break;
        case 2:
            widget_set_indicator( messages_widget, ICON_INDICATOR_2 );
            app_set_indicator( messages_app, ICON_INDICATOR_2 );
            break;
        case 3:
            widget_set_indicator( messages_widget, ICON_INDICATOR_3 );
            app_set_indicator( messages_app, ICON_INDICATOR_3 );
            break;
        default:
            widget_set_indicator( messages_widget, ICON_INDICATOR_N );
            app_set_indicator( messages_app, ICON_INDICATOR_N );
    } 
}

const char* bluetooth_get_msg_entry( int32_t entry ) {
    if( msg_chain_get_entrys( bluetooth_msg_chain ) == 0 ) {
        return( "" );
    }
    else {
        return( msg_chain_get_msg_entry( bluetooth_msg_chain, entry ) );
    }
}

bool bluetooth_message_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case GADGETBRIDGE_JSON_MSG:    
            bluetooth_message_queue_msg( *(BluetoothJsonRequest*)arg );
            break;
    }
    return( true );
}

static void exit_bluetooth_message_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
            bluetooth_message_set_indicator();
            mainbar_jump_back();
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
    /*
     * search for the right src icon
     */
    for ( int i = 0; src_icon[ i ].img != NULL; i++ ) {
        if ( strstr( src_name, src_icon[ i ].src_name ) ) {
            return( src_icon[ i ].img );
        }
    }
    return( default_msg_icon );
}

bool bluetooth_message_queue_msg( BluetoothJsonRequest &doc ) {
    bool retval = false;
    /*
     * check if showing messages allowed
     */
    if ( bluetooth_message_active == false ) {
        return( retval );
    }
    if( doc.containsKey("t") ) {
        /**
         * block osm msg
         */
        if( doc.containsKey("src" ) ) {
            if( !strcmp( doc["src"], "OsmAnd" ) ) {
                return( true );
            }
        }
        /*
        * if msg an notify or weather msg?
        */
        if( !strcmp( doc["t"], "notify" ) || !strcmp( doc["t"], "weather" ) ) {
            int len = doc.memoryUsage();
            char *msg = (char *)MALLOC_ASSERT( len, "bluetooth message alloc failed" );

            serializeJson( doc, msg, len );
            retval = bluetooth_message_queue_msg( msg );
            free( msg );
        }
        else if( !strcmp( doc["t"], "notify-" ) && doc.containsKey("id" ) ) {
            for( int i = 0 ; i < bluetooth_get_number_of_msg() ; i++ ) {
                const char *msg = bluetooth_get_msg_entry( i );

                BluetoothJsonRequest request( msg, strlen( msg ) * 4 );
                if ( request.isValid() ) {
                    if( request.containsKey("id") ) {
                        if( request["id"].as<long>() == doc["id"].as<long>() ) {
                            bluetooth_delete_msg_from_chain( i );
                            break;
                        }
                    }
                }
                request.clear();                
            }
        }
    }
    return( retval );
}

bool bluetooth_message_queue_msg( const char *msg ) {
    if ( bluetooth_message_active == false ) {
        return( false );
    }
    /*
     * check if msg a json before add it to msg queue
     */
    BluetoothJsonRequest request( msg, strlen( msg ) * 4 );
    if ( !request.isValid() ) {
        request.clear();
        return( false );
    }
    request.clear();
    /*
     * add msg to the msg chain
     */
    bluetooth_msg_chain = msg_chain_add_msg( bluetooth_msg_chain, msg );
    /*
     * only alert or alret and showing msg
     */
    int32_t entry = bluetooth_current_msg = msg_chain_get_entrys( bluetooth_msg_chain ) - 1;
    bluetooth_message_show_msg( entry );
    /**
     * show on notification if enabled
     */
    if ( blectl_get_show_notification() ) {
        mainbar_jump_to_tilenumber( bluetooth_message_tile_num, LV_ANIM_OFF, true );
    }
    /**
     * wakeup on nitification if enabled
     */
    if( blectl_get_wakeup_on_notification() ) {
        powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
        lv_disp_trig_activity( NULL );
    }
    /**
     * sound on notification if enabled
     */
    if( blectl_get_sound_notification() ) {
        bluetooth_message_play_audio( entry );
    }
    /**
     * vibe on notification if enabled
     */
    if( blectl_get_vibe_notification() ) {
        motor_vibe(20);
    }
    /*
     * allocate an widget if nor allocated
     */
    if ( messages_widget == NULL ) {
        messages_widget = widget_register( "message", &message_64px, enter_bluetooth_messages_cb );
    }
    /*
     * set widget icon indicator
     */
    bluetooth_message_set_indicator();
    return( true );
}

int32_t bluetooth_get_number_of_msg( void ) {
    return( bluetooth_current_msg < 0 ? 0 : bluetooth_current_msg + 1 );
}

void bluetooth_message_show_msg( int32_t entry ) {
    /*
     * if an msg set?
     */
    const char *msg = msg_chain_get_msg_entry( bluetooth_msg_chain, entry );
    if ( msg == NULL ) {
        return;
    }
    /*
     * check msg number to print pre/next arrow
     */
    if ( entry > 0 ) {
        if( lv_obj_get_hidden( bluetooth_message_prev_msg_btn ) )
            wf_image_button_fade_in( bluetooth_message_prev_msg_btn, 300, 0 );
    }
    else {
        if( !lv_obj_get_hidden( bluetooth_message_prev_msg_btn ) )
            wf_image_button_fade_out( bluetooth_message_prev_msg_btn, 300, 0 );        
    }
    if ( entry < ( msg_chain_get_entrys( bluetooth_msg_chain ) -1 ) ) {
        if( lv_obj_get_hidden( bluetooth_message_next_msg_btn ) )
            wf_image_button_fade_in( bluetooth_message_next_msg_btn, 300, 0 );
    }
    else {
        if( !lv_obj_get_hidden( bluetooth_message_next_msg_btn ) )
            wf_image_button_fade_out( bluetooth_message_next_msg_btn, 300, 0 );
    }
    /*
     * allocate json memory and serialize msg
     */
    SpiRamJsonDocument doc( strlen( msg ) * 4 );
    DeserializationError error = deserializeJson( doc, msg );
    if ( error ) {
        log_e("bluetooth message deserializeJson() failed: %s", error.c_str() );
    }
    else {
        /*
         * if msg an notify msg?
         */
        if( doc.containsKey("t" ) ) {
            if( !strcmp( doc["t"], "notify" ) || !strcmp( doc["t"], "weather" ) ) {
                /*
                * set the receive time string
                */
                struct tm info;
                localtime_r( msg_chain_get_msg_timestamp_entry( bluetooth_msg_chain, entry ), &info );
                int h = info.tm_hour;
                int m = info.tm_min;
                wf_label_printf( bluetooth_message_time_label, "%02d:%02d", h, m );
                /*
                * set the numbers of msg string
                */
                wf_label_printf( bluetooth_message_entrys_label, bluetooth_message_next_msg_btn, LV_ALIGN_OUT_LEFT_MID, -5, 0, "%d/%d", entry + 1, msg_chain_get_entrys( bluetooth_msg_chain ) );
                /*
                * set notify source icon if msg src known
                */
                if ( doc.containsKey("src") ) {
                    lv_img_set_src( bluetooth_message_img, bluetooth_message_find_img( doc["src"] ) ); 
                    lv_label_set_text( bluetooth_message_notify_source_label, doc["src"] );
                }
                else {
                    lv_img_set_src( bluetooth_message_img, default_msg_icon );
                    lv_label_set_text( bluetooth_message_notify_source_label, "Message" );
                }
                /*
                * set message if body known or set title and if no other information
                * available set an emty msg
                */
                if ( doc.containsKey("body") ) {
                    lv_label_set_text( bluetooth_message_msg_label, doc["body"] );
                }
                else if ( doc.containsKey("title") ) {
                    lv_label_set_text( bluetooth_message_msg_label, doc["title"] );
                }
                else if ( doc.containsKey("temp") && doc.containsKey("loc") && doc.containsKey("txt") ) {
                    /*
                    * add special case when a weather information is set
                    */
                    int temperature = doc["temp"];
                    wf_label_printf( bluetooth_message_msg_label, "%s / %d °C / %s", doc["loc"].as<String>().c_str(), temperature - 273, doc["txt"].as<String>().c_str() );
                }
                else {
                    lv_label_set_text( bluetooth_message_msg_label, "" );
                }
                /*
                * scroll back to the top of the msg
                */
                if ( lv_page_get_scrl_height( bluetooth_message_page ) > 160 ) {
                    lv_page_scroll_ver( bluetooth_message_page, lv_page_get_scrl_height( bluetooth_message_page ) ); 
                }
                /*
                * set sender label from available source
                */
                if ( doc.containsKey("title") ) {
                    lv_label_set_text( bluetooth_message_sender_label, doc["title"] );
                }
                else if ( doc.containsKey("sender") ) {
                    lv_label_set_text( bluetooth_message_sender_label, doc["sender"] );
                }
                else if( doc.containsKey("tel") ) {
                    lv_label_set_text( bluetooth_message_sender_label, doc["tel"] );
                }
                else {
                    lv_label_set_text( bluetooth_message_sender_label, "n/a" );
                }
                /*
                * trigger invalidate to redraw all information
                */
                lv_obj_invalidate( lv_scr_act() );
            }
        }
    }        
    doc.clear();
}

void bluetooth_message_play_audio( int32_t entry ) {
    bool found = false;
    static uint64_t nextmillis = 0;

    /*
     * check if audio played recently
     */
    if ( nextmillis >= millis() ) {
        nextmillis += 5000L;
        return;
    }
    nextmillis = millis() + 10000L;
    /*
     * if an msg set?
     */
    const char *msg = msg_chain_get_msg_entry( bluetooth_msg_chain, entry );
    if ( msg == NULL ) {
        return;
    }
    /*
     * check msg number to print pre/next arrow
     */
    if ( entry > 0 ) {
        lv_obj_set_hidden( bluetooth_message_prev_msg_btn, false );
    }
    else {
        lv_obj_set_hidden( bluetooth_message_prev_msg_btn, true );
    }
    if ( entry < ( msg_chain_get_entrys( bluetooth_msg_chain ) -1 ) ) {
        lv_obj_set_hidden( bluetooth_message_next_msg_btn, false );
    }
    else {
        lv_obj_set_hidden( bluetooth_message_next_msg_btn, true );
    }
    /*
     * allocate json memory and serialize msg
     */
    SpiRamJsonDocument doc( strlen( msg ) * 4 );
    DeserializationError error = deserializeJson( doc, msg );
    if ( error ) {
        log_e("bluetooth message deserializeJson() failed: %s", error.c_str() );
    }
    else {
        const char *message = "";

        if ( doc.containsKey("body") ) {
            message = doc["body"];
        }
        else if ( doc.containsKey("title") ) {
            message = doc["title"];
        }

        blectl_custom_audio* custom_audio_notifications = blectl_get_custom_audio_notifications();
        /**
         * check for custom audio notifications
         */
        for ( int entry = 0; entry < CUSTOM_AUDIO_ENTRYS; entry++) {
            blectl_custom_audio custom_audio_notification = custom_audio_notifications[ entry ];

            if ( strlen( custom_audio_notification.text ) <= 0 )
                continue;
            if ( strstr( message, custom_audio_notification.text ) == NULL )
                continue;

            found = true;

            if ( strlen( custom_audio_notification.value ) <= 1 ) {
                char tts[128];
                const char *text = message;
                if ( !strncmp( message, "/", 1 ) ) text = strchr( message, ' ' );
                snprintf( tts, sizeof( tts ), "%s.", text );
                
                sound_speak( tts );
            }
            else {
                sound_play_spiffs_mp3( custom_audio_notification.value );
            }
        }
        /**
         * if not custom audio notification found, play default piep
         */
        if ( !found ) {
            sound_play_progmem_wav( piep_wav, piep_wav_len );
        }
    }
    doc.clear();
}