/****************************************************************************
 *   Sep 29 12:17:11 2021
 *   Copyright  2021  Dirk Brosswick
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
#include "mail_app.h"
#include "mail_app_main.h"
#include "gui/mainbar/mainbar.h"
#include "gui/widget_factory.h"
#include "utils/msg_chain.h"

#if defined( NATIVE_64BIT )
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #include <ESP_Mail_Client.h>
#endif

msg_chain_t *mail_uid = NULL;
uint32_t mail_main_tile_num = 0;
lv_obj_t *mail_main_tile = NULL;
lv_obj_t *mail_main_overview_page = NULL;
lv_obj_t *mail_main_overview = NULL;
lv_obj_t *mail_main_header_label = NULL;
lv_style_t mail_main_cell_style;

bool mail_main_style_event_cb( EventBits_t event, void *arg );
static void mail_main_selected_mail_event_cb( lv_obj_t * obj, lv_event_t event );
static void mail_main_refresh_event_cb( lv_obj_t *obj, lv_event_t event );
static void mail_main_setup_event_cb( lv_obj_t *obj, lv_event_t event );
bool mail_main_button_event_cb( EventBits_t event, void *arg );
void mail_sync_Task( void * pvParameters );
void mail_sync_request( void );
void mail_main_refresh( void );
void mail_main_clear_overview( void );
void mail_main_add_mail_entry( const char *from, const char *date );

#if defined( NATIVE_64BIT )

#else
    EventGroupHandle_t mail_sync_event_handle = NULL;
    TaskHandle_t _mail_sync_Task;
    IMAPSession imap;

    void mail_main_imapCallback( IMAP_Status status );
    void mail_main_imapCallback( IMAP_Status status ) {
        /* Show the result when reading finished */
        if (status.success())
        {
            /* Print the result */
            /* Get the message list from the message list data */
            IMAP_MSG_List msgList = imap.data();

            for (size_t i = 0; i < msgList.msgItems.size(); i++) {
                /**
                 * set mail table text entry
                 */
                IMAP_MSG_Item msg = msgList.msgItems[i];
                mail_main_add_mail_entry( msg.from, msg.date );
                /**
                 * store mail uid in a extra list
                 */
                char tmp_str[32] = "";
                snprintf( tmp_str, sizeof( tmp_str ), "%d", msg.UID );
                mail_uid = msg_chain_add_msg( mail_uid, tmp_str );
            }
            /* Clear all stored data in IMAPSession object */
            imap.empty();
        }    
    }
#endif

void mail_app_main_setup( uint32_t tile_num ) {
    /**
     * get tile num and obj
     */
    mail_main_tile_num = tile_num;
    mail_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &mail_main_cell_style, APP_STYLE );
    lv_style_set_pad_all( &mail_main_cell_style, LV_TABLE_PART_BG, 0 );
    /**
     * set button callback for this tile
     * set style change event
     */
    mainbar_add_tile_button_cb( mail_main_tile_num, mail_main_button_event_cb );
    styles_register_cb( STYLE_CHANGE, mail_main_style_event_cb, "mail style" );
    /**
     * build header
     */
    lv_obj_t *mail_main_exit_btn = wf_add_exit_button( mail_main_tile );
    lv_obj_align( mail_main_exit_btn, mail_main_tile, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, THEME_PADDING );
    mail_main_header_label = lv_label_create( mail_main_tile, NULL );
    lv_label_set_text( mail_main_header_label, "mail" );
    lv_obj_add_style( mail_main_header_label, LV_OBJ_PART_MAIN, APP_STYLE );
    lv_obj_align( mail_main_header_label, mail_main_exit_btn, LV_ALIGN_OUT_RIGHT_MID, THEME_PADDING, 0 );
    lv_obj_t *mail_main_refresh_btn = wf_add_refresh_button( mail_main_tile, mail_main_refresh_event_cb );
    lv_obj_align( mail_main_refresh_btn, mail_main_tile, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING, THEME_PADDING );
    lv_obj_t *mail_main_setup_btn = wf_add_setup_button( mail_main_tile, mail_main_setup_event_cb );
    lv_obj_align( mail_main_setup_btn, mail_main_refresh_btn, LV_ALIGN_OUT_LEFT_MID, -THEME_PADDING, 0 );
    /**
     * create mail main overview
     */
    mail_main_overview_page = lv_page_create( mail_main_tile, NULL);
    lv_obj_set_size( mail_main_overview_page, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) - THEME_ICON_SIZE );
    lv_obj_add_style( mail_main_overview_page, LV_OBJ_PART_MAIN, &mail_main_cell_style );
    lv_page_set_scrlbar_mode( mail_main_overview_page, LV_SCRLBAR_MODE_AUTO );
    lv_obj_align( mail_main_overview_page, mail_main_tile, LV_ALIGN_IN_TOP_MID, 0, THEME_ICON_SIZE );
    mail_main_overview = lv_table_create( mail_main_overview_page, NULL );
    lv_obj_add_style( mail_main_overview, LV_OBJ_PART_MAIN, &mail_main_cell_style );
    lv_page_glue_obj(mail_main_overview, true);
    lv_obj_set_event_cb( mail_main_overview, mail_main_selected_mail_event_cb );
    mail_main_clear_overview();

    #ifdef NATIVE_64BIT

    #else
        mail_sync_event_handle = xEventGroupCreate();
        imap.callback( mail_main_imapCallback );
    #endif

    msg_chain_delete( mail_uid );
}

bool mail_main_style_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_CHANGE:
            lv_style_copy( &mail_main_cell_style, APP_STYLE );
            lv_style_set_pad_all( &mail_main_cell_style, LV_TABLE_PART_BG, 0 );
            break;
    }
    return( true );
}

static void mail_main_selected_mail_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case LV_EVENT_CLICKED:
                uint16_t row, col;
                lv_table_get_pressed_cell( obj, &row, &col );
                MAIL_APP_INFO_LOG("row %d clicked, uid = %s", row, msg_chain_get_msg_entry( mail_uid, row - 1 )?msg_chain_get_msg_entry( mail_uid, row - 1 ):"n/a" );
            break;
        default:
            break;
    }
}

bool mail_main_button_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_EXIT:
            mainbar_jump_back();
            break;
        case BUTTON_REFRESH:
            mail_main_refresh();
            break;
        case BUTTON_SETUP:
            // mail_main_setup();
        case BUTTON_LEFT:
            lv_page_scroll_ver(mail_main_overview_page, lv_disp_get_ver_res( NULL ) / 8 );
            break;
        case BUTTON_RIGHT:
            lv_page_scroll_ver(mail_main_overview_page, -( lv_disp_get_ver_res( NULL ) / 8 ) );
            break;
    }
    return( true );
}

static void mail_main_refresh_event_cb( lv_obj_t *obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_CLICKED:
            mail_sync_request();
            break;
    }
}

static void mail_main_setup_event_cb( lv_obj_t *obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_CLICKED:
            mainbar_jump_to_tilenumber( mail_app_get_app_setup_tile_num(), LV_ANIM_OFF );
            break;
    }    
}

void mail_sync_request( void ) {
#ifdef NATIVE_64BIT
    mail_sync_Task( NULL );
#else
    #if defined( M5PAPER )
        mail_main_refresh();
    #else
        if ( xEventGroupGetBits( mail_sync_event_handle ) & MAIL_SYNC_REQUEST ) {
            return;
        }
        else {
            xEventGroupSetBits( mail_sync_event_handle, MAIL_SYNC_REQUEST );
            xTaskCreate(    mail_sync_Task,              /* Function to implement the task */
                            "mail sync Task",            /* Name of the task */
                            10000,                           /* Stack size in words */
                            NULL,                           /* Task input parameter */
                            1,                              /* Priority of the task */
                            &_mail_sync_Task );          /* Task handle. */
        }
    #endif
#endif
}

void mail_sync_Task( void * pvParameters ) {
    #ifndef NATIVE_64BIT
        log_i("start mail sync task, heap: %d", ESP.getFreeHeap() );
        vTaskDelay( 250 );
        if ( xEventGroupGetBits( mail_sync_event_handle ) & MAIL_SYNC_REQUEST ) {       
    #endif

    mail_main_refresh();

    #ifndef NATIVE_64BIT
        }
        xEventGroupClearBits( mail_sync_event_handle, MAIL_SYNC_REQUEST );
        log_i("finish mail sync task, heap: %d", ESP.getFreeHeap() );
        vTaskDelete( NULL );
    #endif
}

void mail_main_refresh( void ) {
#if defined( NATIVE_64BIT )
    mail_main_clear_overview();
    mail_main_add_mail_entry( "no native imap supported", "23:42" );
#else
    char tmp_str[128] = "";
    ESP_Mail_Session session;
    IMAP_Config config;
    mail_config_t *mail_config = mail_app_get_config();
    /**
     * connectio/session settings
     */
    session.server.host_name = mail_config->imap_server;
    session.server.port = mail_config->imap_port;
    session.login.email = mail_config->username;
    session.login.password = mail_config->password;
    /**
     * imap config
     */
    config.search.criteria = "UID SEARCH ALL";
    config.search.unseen_msg = true;
    config.fetch.uid = "";
    config.enable.html = true;
    config.enable.text = true;
    config.enable.recent_sort = true;
    config.enable.download_status = true;
    config.limit.search = mail_config->max_msg;
    config.limit.msg_size = mail_config->max_msg_size;
    config.storage.saved_path = "/mail";
    config.storage.type = esp_mail_file_storage_type_flash;
    imap.headerOnly();
    /**
     * start connections
     */
    snprintf( tmp_str, sizeof( tmp_str ), "connect to %s", mail_config->imap_server );
    lv_label_set_text( mail_main_header_label, tmp_str );
    if ( !imap.connect( &session, &config ) ) {
        snprintf( tmp_str, sizeof( tmp_str ), "imap connect abort: %s", imap.errorReason().c_str() );
        MAIL_APP_ERROR_LOG("%s", tmp_str );
        lv_label_set_text( mail_main_header_label, tmp_str );
        goto mail_refresh_exit;
    }
    MAIL_APP_DEBUG_LOG("imap connected");
    /**
     * select folder folders
     */
    snprintf( tmp_str, sizeof( tmp_str ), "set imap folder %s", mail_config->inbox_folder );
    lv_label_set_text( mail_main_header_label, tmp_str );
    if ( !imap.selectFolder( mail_config->inbox_folder ) ) {
        snprintf( tmp_str, sizeof( tmp_str ), "imap select folder abort; %s", imap.errorReason().c_str() );
        MAIL_APP_ERROR_LOG("%s", tmp_str );
        lv_label_set_text( mail_main_header_label, tmp_str );
        goto mail_refresh_exit;
    }
    MAIL_APP_DEBUG_LOG("imap folder selected, max msg %d (%d bytes)", config.limit.search, config.limit.msg_size );
    /**
     * clear mail list
     */
    mail_main_clear_overview();
    mail_uid = msg_chain_delete( mail_uid );
    /**
     * get all email headers
     */
    MAIL_APP_INFO_LOG("mail sync task, heap: %d", ESP.getFreeHeap() );
    lv_label_set_text( mail_main_header_label, "get mail header" );
    if ( MailClient.readMail( &imap ) ) {
        snprintf( tmp_str, sizeof( tmp_str ), "mail %d/%d", imap.selectedFolder().availableMessages(), imap.selectedFolder().msgCount() );
    }
    else {
        snprintf( tmp_str, sizeof( tmp_str ), "imap read mail header abort: %s", imap.errorReason().c_str() );
    }
    MAIL_APP_DEBUG_LOG("%s", tmp_str );
    lv_label_set_text( mail_main_header_label, tmp_str );

mail_refresh_exit:

    imap.empty();
    imap.closeSession();
#endif
}

void mail_main_clear_overview( void ) {
    lv_table_set_col_cnt( mail_main_overview, 2 );
    lv_table_set_row_cnt( mail_main_overview, 1 );
    lv_table_set_cell_type( mail_main_overview, 0, 0, 1 );
    lv_table_set_cell_type( mail_main_overview, 0, 1, 1 );
    lv_table_set_cell_value( mail_main_overview, 0, 0, "FROM");
    lv_table_set_cell_value( mail_main_overview, 0, 1, "TIME");
    lv_table_set_col_width( mail_main_overview, 0, ( lv_page_get_width_fit(mail_main_overview_page) / 16 ) * 11 - THEME_PADDING );
    lv_table_set_col_width( mail_main_overview, 1, ( lv_page_get_width_fit(mail_main_overview_page) / 16 ) * 5 );
    lv_obj_align( mail_main_overview, mail_main_overview_page, LV_ALIGN_IN_TOP_MID, 0, 0 );
}

void mail_main_add_mail_entry( const char *from, const char *date ) {
    lv_table_set_row_cnt( mail_main_overview, lv_table_get_row_cnt( mail_main_overview ) + 1 );
    lv_table_set_cell_value( mail_main_overview, lv_table_get_row_cnt( mail_main_overview ) - 1, 0, from );
    lv_table_set_cell_value( mail_main_overview, lv_table_get_row_cnt( mail_main_overview ) - 1, 1, date );
    lv_table_set_cell_type( mail_main_overview, lv_table_get_row_cnt( mail_main_overview ) - 1, 0, 3 );
    lv_table_set_cell_type( mail_main_overview, lv_table_get_row_cnt( mail_main_overview ) - 1, 1, 3 );
}