/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#include <TTGO.h>

#include "gui/mainbar/setup_tile/watchface/config/watchface_config.h"
#include "gui/mainbar/setup_tile/watchface/watchface_manager.h"
#include "gui/mainbar/setup_tile/watchface/watchface_setup.h"
#include "gui/mainbar/setup_tile/watchface/watchface_tile.h"
#include "watchface_manager_app.h"

#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_styles.h"
#include "gui/widget_factory.h"

#include "hardware/display.h"

#include "utils/json_psram_allocator.h"
#include "utils/uri_load/uri_load.h"
#include "utils/alloc.h"

EventGroupHandle_t watchface_manager_app_event_handle = NULL;
TaskHandle_t _watchface_manager_app_task;
lv_task_t *_watchface_manager_theme_install_task;   
static volatile uint32_t watchface_manager_display_timeout = 15;              
 /*
 * watchface manager app tile container
 */
lv_obj_t *watchface_manager_app_tile = NULL;
/**
 * watchface manager app container
 */
lv_obj_t *watchface_manager_app_cont = NULL;
lv_obj_t *watchface_manager_app_preview_cont = NULL;
lv_obj_t *watchface_manager_prev_btn = NULL;
lv_obj_t *watchface_manager_next_btn = NULL;
lv_obj_t *watchface_manager_preview_img = NULL;
lv_obj_t *watchface_manager_download_btn = NULL;
lv_obj_t *watchface_manager_app_info_cont = NULL;
lv_obj_t *watchface_manager_app_info_label = NULL;
lv_obj_t *watchface_manager_app_progress_label = NULL;
lv_obj_t *watchface_manager_app_info_progressbar = NULL;
lv_style_t watchface_manager_style;
lv_style_t watchface_manager_trans_button_style;

/**
 * font we need
 */
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_48px);
/**
 * images we need
 */
LV_IMG_DECLARE(right_32px);
LV_IMG_DECLARE(left_32px);
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_IMG_DECLARE(downloading_32px);

watchface_theme_t watchface_theme;

void watchface_manager_theme_install_task( lv_task_t * task );
void watchface_manager_app_Task( void * pvParameters );
static void exit_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event );
static void setup_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event );
static void prev_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event );
static void next_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event );
void watchface_manager_app_set_progressbar( int16_t percent );
void watchface_manager_app_set_progressbar_label( const char *label );
void watchface_manager_app_set_info_label( const char *label );
void watchface_manager_get_theme_json_cb( int32_t percent );
void watchface_manager_app_activate_cb ( void );
void watchface_manager_app_hibernate_cb ( void );
void watchface_manager_update_theme_list( watchface_theme_t *watchface_theme );
void watchface_manager_next_theme_entry( watchface_theme_t *watchface_theme );
void watchface_manager_prev_theme_entry( watchface_theme_t *watchface_theme );
bool watchface_manager_update_theme_prev( watchface_theme_t *watchface_theme );
static void download_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event );
bool watchface_manager_download_theme( watchface_theme_t *watchface_theme );

void watchface_manager_app_setup( uint32_t tile_num ) {
    /**
     * init watchface theme structure
     */
    watchface_theme.watchface_theme_json_list = NULL;
    watchface_theme.watchface_manager_theme_entrys = 0;
    watchface_theme.watchface_manager_current_theme_entry = 0;
    watchface_theme.watchface_manager_theme_name = "- / -";
    watchface_theme.watchface_manager_theme_url = "";
    watchface_theme.watchface_manager_theme_prev_url = "";
    watchface_theme.watchface_theme_prev.header.always_zero = 0;
    watchface_theme.watchface_theme_prev.header.cf = LV_IMG_CF_RAW_ALPHA;
    watchface_theme.watchface_theme_prev.header.w = 120;
    watchface_theme.watchface_theme_prev.header.h = 120;
    watchface_theme.watchface_theme_prev.data = NULL;
    watchface_theme.watchface_theme_prev.data_size = 0;
    /**
     * geht app tile
     */
    watchface_manager_app_tile = mainbar_get_tile_obj( tile_num );
    /**
     * get and set main style
     */
    lv_style_copy( &watchface_manager_style, ws_get_app_opa_style() );
    lv_style_set_text_font( &watchface_manager_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_style_set_bg_color( &watchface_manager_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_obj_add_style( watchface_manager_app_tile, LV_OBJ_PART_MAIN, &watchface_manager_style );

    lv_style_copy( &watchface_manager_trans_button_style, ws_get_mainbar_style() );
    lv_style_set_text_font( &watchface_manager_trans_button_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_style_set_bg_color( &watchface_manager_trans_button_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    /**
     * create global watchface manager container
     */
    lv_obj_t *watchface_manager_app_cont = lv_obj_create( watchface_manager_app_tile, NULL );
    lv_obj_set_size( watchface_manager_app_cont, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) );
    lv_obj_add_style( watchface_manager_app_cont, LV_OBJ_PART_MAIN, &watchface_manager_style );
    lv_obj_align( watchface_manager_app_cont, watchface_manager_app_tile, LV_ALIGN_CENTER, 0, 0 );
    /**
     * create watchface preview container
     */
    lv_obj_t *watchface_manager_app_preview_cont = lv_obj_create( watchface_manager_app_cont, NULL );
    lv_obj_set_size( watchface_manager_app_preview_cont, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) / 2 );
    lv_obj_add_style( watchface_manager_app_preview_cont, LV_OBJ_PART_MAIN, &watchface_manager_style );
    lv_obj_align( watchface_manager_app_preview_cont, watchface_manager_app_cont, LV_ALIGN_IN_TOP_MID, 0, 0 );
    watchface_manager_prev_btn = wf_add_image_button( watchface_manager_app_preview_cont, left_32px, prev_watchface_manager_app_event_cb, NULL );
    lv_obj_align( watchface_manager_prev_btn, watchface_manager_app_preview_cont, LV_ALIGN_IN_LEFT_MID, 10, 0 );
    watchface_manager_next_btn = wf_add_image_button( watchface_manager_app_preview_cont, right_32px, next_watchface_manager_app_event_cb, NULL );
    lv_obj_align( watchface_manager_next_btn, watchface_manager_app_preview_cont, LV_ALIGN_IN_RIGHT_MID, -10, 0 );
    watchface_manager_preview_img = lv_img_create( watchface_manager_app_preview_cont, NULL );
    lv_img_set_src( watchface_manager_preview_img, &downloading_32px );
    lv_obj_align( watchface_manager_preview_img, watchface_manager_app_preview_cont, LV_ALIGN_CENTER, 0, 0 );
    watchface_manager_download_btn = lv_btn_create( watchface_manager_app_preview_cont, NULL );
    lv_obj_set_width( watchface_manager_download_btn, lv_disp_get_hor_res( NULL ) / 2 );
    lv_obj_set_height( watchface_manager_download_btn, lv_disp_get_ver_res( NULL ) / 2 );
    lv_obj_add_protect( watchface_manager_download_btn, LV_PROTECT_CLICK_FOCUS );
    lv_obj_add_style( watchface_manager_download_btn, LV_OBJ_PART_MAIN, &watchface_manager_trans_button_style );
    lv_obj_align( watchface_manager_download_btn, watchface_manager_app_preview_cont, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_event_cb( watchface_manager_download_btn, download_watchface_manager_app_event_cb );
    /**
     * create watchface info container
     * + info label and progress bar
     */
    watchface_manager_app_info_cont = lv_obj_create( watchface_manager_app_cont, NULL );
    lv_obj_set_size( watchface_manager_app_info_cont, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) / 4 );
    lv_obj_add_style( watchface_manager_app_info_cont, LV_OBJ_PART_MAIN, &watchface_manager_style );
    lv_obj_align( watchface_manager_app_info_cont, watchface_manager_app_preview_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    watchface_manager_app_info_progressbar = lv_bar_create( watchface_manager_app_info_cont, NULL);
    lv_obj_set_size( watchface_manager_app_info_progressbar, ( lv_disp_get_hor_res( NULL ) / 4 ) * 3, 24 );
    lv_obj_align( watchface_manager_app_info_progressbar, watchface_manager_app_info_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_bar_set_anim_time( watchface_manager_app_info_progressbar, 1000 );
    lv_bar_set_value( watchface_manager_app_info_progressbar, 0, LV_ANIM_OFF );
    lv_obj_add_style( watchface_manager_app_info_progressbar, LV_BAR_PART_BG, &watchface_manager_style );
    watchface_manager_app_progress_label = wf_add_label( watchface_manager_app_info_cont, "" );
    lv_obj_align( watchface_manager_app_progress_label, watchface_manager_app_info_cont, LV_ALIGN_IN_BOTTOM_MID, 0, -2 );
    watchface_manager_app_info_label = wf_add_label( watchface_manager_app_info_cont, watchface_theme.watchface_manager_theme_name.c_str() );
    lv_obj_align( watchface_manager_app_info_label, watchface_manager_app_info_cont, LV_ALIGN_IN_TOP_MID, 0, 4 );
    /**
     * add exit button
     */
    lv_obj_t *watchface_manager_exit_btn = wf_add_image_button( watchface_manager_app_cont, exit_32px, exit_watchface_manager_app_event_cb, NULL );
    lv_obj_align( watchface_manager_exit_btn, watchface_manager_app_cont, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    /**
     * add setup button
     */
    lv_obj_t *watchface_manager_setup_btn = wf_add_image_button( watchface_manager_app_cont, setup_32px, setup_watchface_manager_app_event_cb, NULL );
    lv_obj_align( watchface_manager_setup_btn, watchface_manager_app_cont, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    /**
     * set activate and hibernate call back
     */
    mainbar_add_tile_activate_cb( tile_num, watchface_manager_app_activate_cb );
    mainbar_add_tile_hibernate_cb( tile_num, watchface_manager_app_hibernate_cb );
    /**
     * create event group
     */
    watchface_manager_app_event_handle = xEventGroupCreate();
    _watchface_manager_theme_install_task = lv_task_create( watchface_manager_theme_install_task, 3000, LV_TASK_PRIO_MID, NULL );
}

void watchface_manager_theme_install_task( lv_task_t * task ) {
    /**
     * install watchface theme from tar.gz work only from main task/lv_task
     */
    if ( xEventGroupGetBits( watchface_manager_app_event_handle ) & WATCHFACE_MANAGER_APP_INSTALL_THEME ) {
            watchface_decompress_theme();
            xEventGroupClearBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_INSTALL_THEME );
    }
}

static void exit_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case LV_EVENT_CLICKED:
            mainbar_jump_back( LV_ANIM_OFF );
            break;
    }
}

static void setup_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case LV_EVENT_CLICKED:
            mainbar_jump_to_tilenumber( watchface_manager_get_setup_tile_num(), LV_ANIM_OFF );
            break;
    }
}

static void prev_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case LV_EVENT_CLICKED:
            if ( !(xEventGroupGetBits( watchface_manager_app_event_handle ) & WATCHFACE_MANAGER_APP_GET_PREV_THEME) ) {
                xEventGroupSetBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_GET_PREV_THEME );
            }
            break;
    }
}

static void download_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case LV_EVENT_CLICKED:
            if ( !(xEventGroupGetBits( watchface_manager_app_event_handle ) & WATCHFACE_MANAGER_APP_DOWNLOAD_THEME) ) {
                xEventGroupSetBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_DOWNLOAD_THEME );
            }
            break;
    }
}

static void next_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case LV_EVENT_CLICKED:
            if ( !(xEventGroupGetBits( watchface_manager_app_event_handle ) & WATCHFACE_MANAGER_APP_GET_NEXT_THEME) ) {
                xEventGroupSetBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_GET_NEXT_THEME );
            }
            break;
    }
}

void watchface_manager_app_set_progressbar( int16_t percent ) {
    WATCHFACE_MANAGER_APP_INFO_LOG("download percent: %d%%", percent );
    lv_bar_set_value( watchface_manager_app_info_progressbar, percent, LV_ANIM_ON );
}

void watchface_manager_app_set_progressbar_label( const char *label ) {
    lv_label_set_text( watchface_manager_app_progress_label, label );
    lv_obj_align( watchface_manager_app_progress_label, watchface_manager_app_info_cont, LV_ALIGN_IN_BOTTOM_MID, 0, -2 );
}

void watchface_manager_get_theme_json_cb( int32_t percent ) {
    watchface_manager_app_set_progressbar( percent );
}

void watchface_manager_app_set_info_label( const char *label ) {
    lv_label_set_text( watchface_manager_app_info_label, label );
    lv_obj_align( watchface_manager_app_info_label, watchface_manager_app_info_cont, LV_ALIGN_IN_TOP_MID, 0, 4 );
}

void watchface_manager_app_Task( void * pvParameters ) {
    WATCHFACE_MANAGER_APP_INFO_LOG("start watchface manager background task, heap: %d", ESP.getFreeHeap() );
    while( true ) {
        /**
         * check if a tile image update is requested
         */
        if ( xEventGroupGetBits( watchface_manager_app_event_handle ) & WATCHFACE_MANAGER_APP_GET_THEME_JSON_REQUEST ) {
            /**
             * get theme json
             */
            watchface_manager_update_theme_list( &watchface_theme );
            if ( watchface_manager_update_theme_prev( &watchface_theme ) ) {
                lv_img_set_src( watchface_manager_preview_img, &watchface_theme.watchface_theme_prev );
                lv_obj_align( watchface_manager_preview_img, watchface_manager_app_preview_cont, LV_ALIGN_CENTER, 0, 0 );
            }
            /**
             * clear update request flag
             */
            xEventGroupClearBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_GET_THEME_JSON_REQUEST );
        }
        else if ( xEventGroupGetBits( watchface_manager_app_event_handle ) & WATCHFACE_MANAGER_APP_GET_NEXT_THEME ) {
            /**
             * get next theme entry
             */
            watchface_manager_next_theme_entry( &watchface_theme );
            if ( watchface_manager_update_theme_prev( &watchface_theme ) ) {
                lv_img_set_src( watchface_manager_preview_img, &watchface_theme.watchface_theme_prev );
                lv_obj_align( watchface_manager_preview_img, watchface_manager_app_preview_cont, LV_ALIGN_CENTER, 0, 0 );
            }
            /**
             * clear update request flag
             */
            xEventGroupClearBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_GET_NEXT_THEME );
        }
        else if ( xEventGroupGetBits( watchface_manager_app_event_handle ) & WATCHFACE_MANAGER_APP_GET_PREV_THEME ) {
            /**
             * get next theme entry
             */
            watchface_manager_prev_theme_entry( &watchface_theme );
            if ( watchface_manager_update_theme_prev( &watchface_theme ) ) {
                lv_img_set_src( watchface_manager_preview_img, &watchface_theme.watchface_theme_prev );
                lv_obj_align( watchface_manager_preview_img, watchface_manager_app_preview_cont, LV_ALIGN_CENTER, 0, 0 );
            }
            /**
             * clear update request flag
             */
            xEventGroupClearBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_GET_PREV_THEME );
        }
        else if ( xEventGroupGetBits( watchface_manager_app_event_handle ) & WATCHFACE_MANAGER_APP_DOWNLOAD_THEME ) {
            /**
             * get next theme entry
             */
            watchface_manager_download_theme( &watchface_theme );
            /**
             * clear update request flag
             */
            xEventGroupClearBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_DOWNLOAD_THEME );
        }        /**
         * check if for a task exit request
         */
        if ( xEventGroupGetBits( watchface_manager_app_event_handle ) & WATCHFACE_MANAGER_APP_TASK_EXIT_REQUEST ) {
            WATCHFACE_MANAGER_APP_INFO_LOG("stop watchface manager background task");
            break;
        }
        /**
         * block this task for 125ms
         */
        vTaskDelay( 25 );
    }
    WATCHFACE_MANAGER_APP_INFO_LOG("finsh watchface manager background task, heap: %d", ESP.getFreeHeap() );
    vTaskDelete( NULL );    
}

void watchface_manager_app_activate_cb ( void ) {
    /**
     * set progressbar to default
     */
    watchface_manager_app_set_progressbar( 0 );
    watchface_manager_app_set_progressbar_label( "" );
    /**
     * block display timeout
     */
    watchface_manager_display_timeout = display_get_timeout();
    display_set_timeout( DISPLAY_MAX_TIMEOUT );
    /**
     * start watchface manager background task
     */
    xEventGroupClearBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_TASK_EXIT_REQUEST );
    xTaskCreate(    watchface_manager_app_Task,      /* Function to implement the task */
                    "watchface manager Task",    /* Name of the task */
                    5000,                            /* Stack size in words */
                    NULL,                            /* Task input parameter */
                    1,                               /* Priority of the task */
                    &_watchface_manager_app_task );  /* Task handle. */

    xEventGroupSetBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_GET_THEME_JSON_REQUEST );
}

void watchface_manager_app_hibernate_cb ( void ) {
    /**
     * restore display timeout
     */
    display_set_timeout( watchface_manager_display_timeout );
    /**
     * trigger background task to finish
     */
    xEventGroupSetBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_TASK_EXIT_REQUEST );
}

void watchface_manager_update_theme_list( watchface_theme_t *watchface_theme ) {    
    if ( watchface_theme->watchface_theme_json_list ) {
        return;
    }
    /**
     * reinit watchface theme structure
     */
    if ( watchface_theme->watchface_theme_json_list )
        free( (void *)watchface_theme->watchface_theme_json_list );
    watchface_theme->watchface_theme_json_list = NULL;
    watchface_theme->watchface_manager_theme_entrys = 0;
    watchface_theme->watchface_manager_current_theme_entry = 0;
    watchface_theme->watchface_manager_theme_name = "- / -";
    watchface_theme->watchface_manager_theme_url = "";
    watchface_theme->watchface_manager_theme_prev_url = "";
    watchface_theme->watchface_theme_prev.header.always_zero = 0;
    watchface_theme->watchface_theme_prev.header.cf = LV_IMG_CF_RAW_ALPHA;
    watchface_theme->watchface_theme_prev.header.w = 120;
    watchface_theme->watchface_theme_prev.header.h = 120;
    if ( watchface_theme->watchface_theme_prev.data )
        free( (void*)watchface_theme->watchface_theme_prev.data );
    watchface_theme->watchface_theme_prev.data = NULL;
    watchface_theme->watchface_theme_prev.data_size = 0;
    /**
     * build theme list url
     */
    String theme_url = watchface_setup_get_theme_url() + WATCHFACE_THEME_LIST_FILE;
    /**
     * set prograssbar and progress label
     */
    watchface_manager_app_set_progressbar( 0 );
    watchface_manager_app_set_progressbar_label( "get theme list" );
    /**
     * get theme json
     */
    uri_load_dsc_t *theme_list = uri_load_to_ram( theme_url.c_str() , watchface_manager_get_theme_json_cb );
    watchface_manager_app_set_progressbar( 0 );
    if ( theme_list ) {
        watchface_manager_app_set_progressbar_label( "success" );
    }
    else {
        watchface_manager_app_set_progressbar_label( "failed" );
        return;
    }
    /**
     * make string from binary data
     */
    watchface_theme->watchface_theme_json_list = (char*)CALLOC( 1, theme_list->size + 1 );
    if ( watchface_theme->watchface_theme_json_list ) {
        memcpy( watchface_theme->watchface_theme_json_list, theme_list->data, theme_list->size );
        WATCHFACE_MANAGER_APP_INFO_LOG("theme list = %s", watchface_theme->watchface_theme_json_list );
    }
    else {
        WATCHFACE_MANAGER_APP_ERROR_LOG("theme list calloc failed");
    }
    uri_load_free_all( theme_list );
    /**
     * get theme entrys
     */
    SpiRamJsonDocument doc( strlen( (const char*)watchface_theme->watchface_theme_json_list ) * 2 );
    DeserializationError error = deserializeJson( doc, (const char *)watchface_theme->watchface_theme_json_list );

    if ( error ) {
        WATCHFACE_MANAGER_APP_ERROR_LOG("watchface theme list deserializeJson() failed: %s", error.c_str() );
    }
    else {
        for( int i = 0; i < WATCHFACE_MAX_ENTRYS; i++ ) {
            if ( doc[ i ]["name"] ) {
                String watchface_manager_theme_name = doc[ i ][ "name" ];
                String watchface_theme_url = doc[ i ][ "url" ];
                String watchface_theme_download = doc[ i ][ "download" ];
                WATCHFACE_MANAGER_APP_INFO_LOG("theme name / url: %s / %s", watchface_manager_theme_name.c_str(), watchface_theme_url.c_str() );
                if ( watchface_theme->watchface_manager_theme_entrys == 0 ) {
                    watchface_theme->watchface_manager_theme_name = watchface_manager_theme_name;
                    watchface_theme->watchface_manager_theme_url = watchface_theme_download;
                    watchface_theme->watchface_manager_theme_prev_url = watchface_theme_url + WATCHFACE_THEME_PREV;
                }
                watchface_theme->watchface_manager_theme_entrys++;
            }
            else {
                break;
            }
        }

/*
        JsonObject obj = doc.as<JsonObject>();
        for ( JsonPair p : obj ) {
            String watchface_manager_theme_name = p.key().c_str();
            String watchface_theme_url = doc[ p.key().c_str() ];
            WATCHFACE_MANAGER_APP_INFO_LOG("theme name / url: %s / %s", watchface_manager_theme_name.c_str(), watchface_theme_url.c_str() );
            if ( watchface_theme->watchface_manager_theme_entrys == 0 ) {
                watchface_theme->watchface_manager_theme_name = watchface_manager_theme_name;
                watchface_theme->watchface_manager_theme_url = watchface_theme_url + WATCHFACE_THEME_FILE;
                watchface_theme->watchface_manager_theme_prev_url = watchface_theme_url + WATCHFACE_THEME_PREV;
            }
            watchface_theme->watchface_manager_theme_entrys++;
        }
*/
        watchface_manager_app_set_info_label( watchface_theme->watchface_manager_theme_name.c_str() );
        WATCHFACE_MANAGER_APP_INFO_LOG("theme entrys: %d", watchface_theme->watchface_manager_theme_entrys );
        WATCHFACE_MANAGER_APP_INFO_LOG("current theme entry: %d", watchface_theme->watchface_manager_current_theme_entry );
        WATCHFACE_MANAGER_APP_INFO_LOG("current theme name: %s", watchface_theme->watchface_manager_theme_name.c_str() );
        WATCHFACE_MANAGER_APP_INFO_LOG("current theme url: %s", watchface_theme->watchface_manager_theme_url.c_str() );
        WATCHFACE_MANAGER_APP_INFO_LOG("current theme prev url: %s", watchface_theme->watchface_manager_theme_prev_url.c_str() );
    }
    doc.clear();
}

void watchface_manager_next_theme_entry( watchface_theme_t *watchface_theme ) {
    /**
     * check if watchface set
     */
    if ( !watchface_theme && !watchface_theme->watchface_theme_json_list) {
        return;
    }
    /**
     * check if the current entry not the last one
     */
    if ( watchface_theme->watchface_manager_current_theme_entry < watchface_theme->watchface_manager_theme_entrys - 1 ) {
        /**
         * get theme entrys
         */
        SpiRamJsonDocument doc( strlen( (const char*)watchface_theme->watchface_theme_json_list ) * 2 );
        DeserializationError error = deserializeJson( doc, (const char *)watchface_theme->watchface_theme_json_list );

        if ( error ) {
            WATCHFACE_MANAGER_APP_ERROR_LOG("watchface theme list deserializeJson() failed: %s", error.c_str() );
        }
        else {
            watchface_theme->watchface_manager_current_theme_entry++;
            watchface_theme->watchface_manager_theme_entrys = 0;

            for( int i = 0; i < WATCHFACE_MAX_ENTRYS; i++ ) {
                if ( doc[ i ]["name"] ) {
                    String watchface_manager_theme_name = doc[ i ][ "name" ];
                    String watchface_theme_url = doc[ i ][ "url" ];
                    String watchface_theme_download = doc[ i ][ "download" ];
                    WATCHFACE_MANAGER_APP_INFO_LOG("theme name / url: %s / %s", watchface_manager_theme_name.c_str(), watchface_theme_url.c_str() );
                    if ( watchface_theme->watchface_manager_theme_entrys == watchface_theme->watchface_manager_current_theme_entry ) {
                        watchface_theme->watchface_manager_theme_name = watchface_manager_theme_name;
                        watchface_theme->watchface_manager_theme_url = watchface_theme_download;
                        watchface_theme->watchface_manager_theme_prev_url = watchface_theme_url + WATCHFACE_THEME_PREV;
                    }
                    watchface_theme->watchface_manager_theme_entrys++;
                }
                else {
                    break;
                }
            }
/*
            JsonObject obj = doc.as<JsonObject>();
            for ( JsonPair p : obj ) {
                String watchface_manager_theme_name = p.key().c_str();
                String watchface_theme_url = doc[ p.key().c_str() ];
                WATCHFACE_MANAGER_APP_INFO_LOG("theme name / url: %s / %s", watchface_manager_theme_name.c_str(), watchface_theme_url.c_str() );
                if ( watchface_theme->watchface_manager_theme_entrys == watchface_theme->watchface_manager_current_theme_entry ) {
                    watchface_theme->watchface_manager_theme_name = watchface_manager_theme_name;
                    watchface_theme->watchface_manager_theme_url = watchface_theme_url + WATCHFACE_THEME_FILE;
                    watchface_theme->watchface_manager_theme_prev_url = watchface_theme_url + WATCHFACE_THEME_PREV;
                }
                watchface_theme->watchface_manager_theme_entrys++;
            }
*/
            watchface_manager_app_set_info_label( watchface_theme->watchface_manager_theme_name.c_str() );
            WATCHFACE_MANAGER_APP_INFO_LOG("theme entrys: %d", watchface_theme->watchface_manager_theme_entrys );
            WATCHFACE_MANAGER_APP_INFO_LOG("current theme entry: %d", watchface_theme->watchface_manager_current_theme_entry );
            WATCHFACE_MANAGER_APP_INFO_LOG("current theme name: %s", watchface_theme->watchface_manager_theme_name.c_str() );
            WATCHFACE_MANAGER_APP_INFO_LOG("current theme url: %s", watchface_theme->watchface_manager_theme_url.c_str() );
            WATCHFACE_MANAGER_APP_INFO_LOG("current theme prev url: %s", watchface_theme->watchface_manager_theme_prev_url.c_str() );
        }
        doc.clear();
    }
}

void watchface_manager_prev_theme_entry( watchface_theme_t *watchface_theme ) {
    /**
     * check if watchface set
     */
    if ( !watchface_theme && !watchface_theme->watchface_theme_json_list) {
        return;
    }
    /**
     * check if the current entry not the last one
     */
    if ( watchface_theme->watchface_manager_current_theme_entry > 0 ) {
        /**
         * get theme entrys
         */
        SpiRamJsonDocument doc( strlen( (const char*)watchface_theme->watchface_theme_json_list ) * 2 );
        DeserializationError error = deserializeJson( doc, (const char *)watchface_theme->watchface_theme_json_list );

        if ( error ) {
            WATCHFACE_MANAGER_APP_ERROR_LOG("watchface theme list deserializeJson() failed: %s", error.c_str() );
        }
        else {
            watchface_theme->watchface_manager_current_theme_entry--;
            watchface_theme->watchface_manager_theme_entrys = 0;
            for( int i = 0; i < WATCHFACE_MAX_ENTRYS; i++ ) {
                if ( doc[ i ]["name"] ) {
                    String watchface_manager_theme_name = doc[ i ][ "name" ];
                    String watchface_theme_url = doc[ i ][ "url" ];
                    String watchface_theme_download = doc[ i ][ "download" ];
                    WATCHFACE_MANAGER_APP_INFO_LOG("theme name / url: %s / %s", watchface_manager_theme_name.c_str(), watchface_theme_url.c_str() );
                    if ( watchface_theme->watchface_manager_theme_entrys == watchface_theme->watchface_manager_current_theme_entry ) {
                        watchface_theme->watchface_manager_theme_name = watchface_manager_theme_name;
                        watchface_theme->watchface_manager_theme_url = watchface_theme_download;
                        watchface_theme->watchface_manager_theme_prev_url = watchface_theme_url + WATCHFACE_THEME_PREV;
                    }
                    watchface_theme->watchface_manager_theme_entrys++;
                }
                else {
                    break;
                }
            }
/*
            JsonObject obj = doc.as<JsonObject>();
            for ( JsonPair p : obj ) {
                String watchface_manager_theme_name = p.key().c_str();
                String watchface_theme_url = doc[ p.key().c_str() ];
                WATCHFACE_MANAGER_APP_INFO_LOG("theme name / url: %s / %s", watchface_manager_theme_name.c_str(), watchface_theme_url.c_str() );
                if ( watchface_theme->watchface_manager_theme_entrys == watchface_theme->watchface_manager_current_theme_entry ) {
                    watchface_theme->watchface_manager_theme_name = watchface_manager_theme_name;
                    watchface_theme->watchface_manager_theme_url = watchface_theme_url + WATCHFACE_THEME_FILE;
                    watchface_theme->watchface_manager_theme_prev_url = watchface_theme_url + WATCHFACE_THEME_PREV;
                }
                watchface_theme->watchface_manager_theme_entrys++;
            }
*/
            watchface_manager_app_set_info_label( watchface_theme->watchface_manager_theme_name.c_str() );
            WATCHFACE_MANAGER_APP_INFO_LOG("theme entrys: %d", watchface_theme->watchface_manager_theme_entrys );
            WATCHFACE_MANAGER_APP_INFO_LOG("current theme entry: %d", watchface_theme->watchface_manager_current_theme_entry );
            WATCHFACE_MANAGER_APP_INFO_LOG("current theme name: %s", watchface_theme->watchface_manager_theme_name.c_str() );
            WATCHFACE_MANAGER_APP_INFO_LOG("current theme url: %s", watchface_theme->watchface_manager_theme_url.c_str() );
            WATCHFACE_MANAGER_APP_INFO_LOG("current theme prev url: %s", watchface_theme->watchface_manager_theme_prev_url.c_str() );
        }
        doc.clear();
    }
}

bool watchface_manager_update_theme_prev( watchface_theme_t *watchface_theme ) {
    bool retval = false;
    /**
     * check if watchface set
     */
    if ( !watchface_theme && !watchface_theme->watchface_theme_json_list) {
        return( retval );
    }
    /**
     * set download info img
     */
    lv_img_set_src( watchface_manager_preview_img, &downloading_32px );
    lv_obj_align( watchface_manager_preview_img, watchface_manager_app_preview_cont, LV_ALIGN_CENTER, 0, 0 );
    /**
     * get the preview image
     */
    watchface_manager_app_set_progressbar_label( "download preview" );
    watchface_manager_app_set_progressbar( 0 );
    uri_load_dsc_t *uri_load_dsc = uri_load_to_ram( watchface_theme->watchface_manager_theme_prev_url.c_str(), watchface_manager_get_theme_json_cb );
    /**
     * show preview image if download was success or abort
     */
    if ( uri_load_dsc ) {
        /**
         * clear old image data
         */
        if ( watchface_theme->watchface_theme_prev.data )
            free( (void*) watchface_theme->watchface_theme_prev.data );
        watchface_theme->watchface_theme_prev.data = uri_load_dsc->data;
        watchface_theme->watchface_theme_prev.data_size = uri_load_dsc->size;
        /**
         * clear uri_load_dsc and leave data in memory
         */
        uri_load_free_without_data( uri_load_dsc );
        /**
         * clear image cache
         */
        lv_img_cache_invalidate_src( &watchface_theme->watchface_theme_prev );
        /**
         * clear progressbar label
         */
        watchface_manager_app_set_progressbar_label( "" );
        retval = true;
    }
    else {
        watchface_manager_app_set_progressbar_label( "download preview failed" );
    }
    watchface_manager_app_set_progressbar( 0 );
    return( retval );
}

bool watchface_manager_download_theme( watchface_theme_t *watchface_theme ) {
    bool retval = false;
    /**
     * check if watchface set
     */
    if ( !watchface_theme && !watchface_theme->watchface_theme_json_list) {
        return( retval );
    }
    /**
     * set progressbar and progressbar label
     */
    watchface_manager_app_set_progressbar_label( "download theme" );
    watchface_manager_app_set_progressbar( 0 );
    /**
     * start download tar.gz theme file
     */
    if ( uri_load_to_file( watchface_theme->watchface_manager_theme_url.c_str(), "/spiffs", watchface_manager_get_theme_json_cb ) ) {
        watchface_manager_app_set_progressbar_label( "install theme" );
        watchface_manager_app_set_progressbar( 0 );
        /**
         * trigger install routine in lv_task
         */
        xEventGroupSetBits( watchface_manager_app_event_handle, WATCHFACE_MANAGER_APP_INSTALL_THEME );
        retval = true;
    }
    else {
        watchface_manager_app_set_progressbar_label( "download theme failed" );
        watchface_manager_app_set_progressbar( 0 );
    }
    return( retval );
}