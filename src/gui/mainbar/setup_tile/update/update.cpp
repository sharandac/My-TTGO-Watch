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

#include "update.h"
#include "update_setup.h"
#include "update_check_version.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/display.h"
#include "hardware/powermgm.h"
#include "hardware/wifictl.h"
#include "hardware/motor.h"
#include "hardware/callback.h"

#ifdef NATIVE_64BIT
    #include <iostream>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include "utils/logging.h"

    EventBits_t update_event = 0;
#else
    #include <Arduino.h>
    #include <SPIFFS.h>
    #include <HTTPClient.h>
    #include <HTTPUpdate.h>

    #include "utils/http_ota/http_ota.h"

    EventGroupHandle_t update_event = NULL;
    TaskHandle_t _update_Task;
#endif

lv_task_t *_update_progress_task;
void update_Task( void * pvParameters );

icon_t *update_setup_icon = NULL;

lv_obj_t *update_settings_tile=NULL;
uint32_t update_tile_num;
static float progress = 0;
static int64_t last_firmware_version = 0;

lv_obj_t *update_btn = NULL;
lv_obj_t *update_status_label = NULL;
lv_obj_t *update_btn_label = NULL;
lv_obj_t *update_progressbar = NULL;

static bool reset = false;

static void enter_update_setup_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_update_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void update_event_handler(lv_obj_t * obj, lv_event_t event );

bool update_http_ota_event_cb( EventBits_t event, void *arg );
bool update_wifictl_event_cb( EventBits_t event, void *arg );

void update_update_activate_cb( void );
void update_update_hibernate_cb( void );
void update_progress_task( lv_task_t *task );

LV_IMG_DECLARE(update_64px);
LV_IMG_DECLARE(info_1_16px);

void update_tile_setup( void ) {
    last_firmware_version = atoll( __FIRMWARE__ );
    // get an app tile and copy mainstyle
    update_tile_num = mainbar_add_setup_tile( 1, 2, "update setup" );
    update_settings_tile = mainbar_get_tile_obj( update_tile_num );

    update_setup_tile_setup( update_tile_num + 1 );

    lv_obj_add_style( update_settings_tile, LV_OBJ_PART_MAIN, SETUP_STYLE );

    update_setup_icon = setup_register( "update", &update_64px, enter_update_setup_event_cb );
    setup_hide_indicator( update_setup_icon );

    lv_obj_t *header = wf_add_settings_header( update_settings_tile, "update" );
    lv_obj_align( header, update_settings_tile, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, STATUSBAR_HEIGHT + THEME_PADDING );

    lv_obj_t *setup_btn = wf_add_setup_button( update_settings_tile, enter_update_setup_setup_event_cb );
    lv_obj_align( setup_btn, header, LV_ALIGN_IN_RIGHT_MID, 0, 0 );

    lv_obj_t *update_version_cont = wf_add_label( update_settings_tile, "firmware version", SETUP_STYLE );
    lv_obj_align( update_version_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, 8 );

    lv_obj_t *update_firmware_version_cont = wf_add_label( update_settings_tile, __FIRMWARE__ , SETUP_STYLE );
    lv_obj_align( update_firmware_version_cont, update_version_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 8 );

    update_btn = lv_btn_create( update_settings_tile, NULL);
    lv_obj_set_event_cb( update_btn, update_event_handler );
    lv_obj_add_style( update_btn, LV_BTN_PART_MAIN, ws_get_button_style() );
    lv_obj_align( update_btn, update_firmware_version_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    update_btn_label = lv_label_create( update_btn, NULL );
    lv_label_set_text( update_btn_label, "update");

    update_status_label = lv_label_create( update_settings_tile, NULL);
    lv_obj_add_style( update_status_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( update_status_label, "" );
    lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    update_progressbar = lv_bar_create( update_settings_tile, NULL );
    lv_obj_set_size( update_progressbar, lv_disp_get_hor_res( NULL ) - 80, 20 );
    lv_obj_add_style( update_progressbar, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_obj_align( update_progressbar, update_status_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
    lv_bar_set_anim_time( update_progressbar, 2000 );
    lv_bar_set_value( update_progressbar, 0, LV_ANIM_ON );

    wifictl_register_cb( WIFICTL_CONNECT, update_wifictl_event_cb, "update" );
#ifdef NATIVE_64BIT
#else
    http_ota_register_cb( HTTP_OTA_PROGRESS | HTTP_OTA_START | HTTP_OTA_FINISH | HTTP_OTA_ERROR, update_http_ota_event_cb, "http updater");
#endif
    mainbar_add_tile_activate_cb( update_tile_num, update_update_activate_cb );
    mainbar_add_tile_hibernate_cb( update_tile_num, update_update_hibernate_cb );

#ifdef NATIVE_64BIT
    update_event = 0;
#else
    update_event = xEventGroupCreate();
    xEventGroupClearBits( update_event, UPDATE_REQUEST );
#endif
}

void update_update_activate_cb( void ) {
    _update_progress_task = lv_task_create( update_progress_task, 250,  LV_TASK_PRIO_LOWEST, NULL );
}

void update_update_hibernate_cb( void ) {
    lv_task_del( _update_progress_task );
}

void update_progress_task( lv_task_t *task ) {
    if ( progress > 0 ) {
        char msg[16]="";
        lv_bar_set_value( update_progressbar, progress, LV_ANIM_ON );
        snprintf( msg, sizeof( msg ), "%.0f%%", progress );
        lv_label_set_text( update_status_label, msg );
        lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
    }
}


bool update_http_ota_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
#ifdef NATIVE_64BIT
#else
        case HTTP_OTA_PROGRESS:
            progress = *(float *)arg;
            break;
        case HTTP_OTA_START:        
            statusbar_show_icon( STATUSBAR_WARNING );
            statusbar_style_icon( STATUSBAR_WARNING, STATUSBAR_STYLE_YELLOW );   
            lv_label_set_text( update_status_label, (char *)arg );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            break;
        case HTTP_OTA_FINISH:        
            statusbar_show_icon( STATUSBAR_WARNING );
            statusbar_style_icon( STATUSBAR_WARNING, STATUSBAR_STYLE_GREEN );   
            lv_label_set_text( update_status_label, (char *)arg );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            break;
        case HTTP_OTA_ERROR:
            statusbar_show_icon( STATUSBAR_WARNING );
            statusbar_style_icon( STATUSBAR_WARNING, STATUSBAR_STYLE_RED );   
            lv_label_set_text( update_status_label, (char *)arg );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            break;
#endif
    }
    return( true );
}

bool update_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_CONNECT:
            if ( update_setup_get_autosync() && reset == false ) {
                update_check_version();
                break;
            }
    }
    return( true );
}

static void enter_update_setup_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mainbar_jump_to_tilenumber( update_tile_num + 1, LV_ANIM_OFF );
            break;
    }
}

static void enter_update_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
            mainbar_jump_to_tilenumber( update_tile_num, LV_ANIM_OFF );
            break;
    }
}

static void update_event_handler(lv_obj_t * obj, lv_event_t event) {
    if( event == LV_EVENT_CLICKED ) {

    #ifdef NATIVE_64BIT
        if ( update_event & ( UPDATE_GET_VERSION_REQUEST | UPDATE_REQUEST ) ) {
            return;
        }
        else {
            update_event |= UPDATE_REQUEST;
            update_Task( NULL );
        }
    #else
            if ( reset ) {
                log_i("System reboot by user");
                motor_vibe(20);
                delay(20);
                display_standby();
                SPIFFS.end();
                log_i("SPIFFS unmounted!");
                delay(500);
                ESP.restart();
            }
            else if ( xEventGroupGetBits( update_event) & ( UPDATE_GET_VERSION_REQUEST | UPDATE_REQUEST ) )  {
                log_i("update blocked");
                return;
            }
            else {
                xEventGroupSetBits( update_event, UPDATE_REQUEST );
                xTaskCreate(    update_Task,
                                "update Task",
                                5000,
                                NULL,
                                1,
                                &_update_Task );
            }
    #endif

    }
}

void update_check_version( void ) {
#ifdef NATIVE_64BIT
    if( update_event & ( UPDATE_GET_VERSION_REQUEST | UPDATE_REQUEST ) ) {
        return;
    }
    else {
        update_event |= UPDATE_GET_VERSION_REQUEST;
        update_Task( NULL );
    }
#else
    if ( xEventGroupGetBits( update_event ) & ( UPDATE_GET_VERSION_REQUEST | UPDATE_REQUEST ) ) {
        log_i("update blocked");
        return;
    }
    else {
        xEventGroupSetBits( update_event, UPDATE_GET_VERSION_REQUEST );
        xTaskCreate(    update_Task,
                        "update Task",
                        5000,
                        NULL,
                        1,
                        &_update_Task );
    }
#endif
}

void update_Task( void * pvParameters ) {
#ifdef NATIVE_64BIT
    if ( update_event & UPDATE_GET_VERSION_REQUEST ) {
        int64_t firmware_version = update_check_new_version( update_setup_get_url() );
        if ( firmware_version > atoll( __FIRMWARE__ ) && firmware_version > 0 ) {
            char version_msg[48] = "";
            snprintf( version_msg, sizeof( version_msg ), "new version: %lld", firmware_version );
            lv_label_set_text( update_status_label, (const char*)version_msg );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            setup_set_indicator( update_setup_icon, ICON_INDICATOR_1 );
            if ( last_firmware_version < firmware_version ) {
                bluetooth_message_queue_msg("{\"t\":\"notify\",\"id\":1575479849,\"src\":\"Update\",\"title\":\"update\",\"body\":\"new firmware version available\"}");
                last_firmware_version = firmware_version;
            }
        }
        else if ( firmware_version == atoll( __FIRMWARE__ ) ) {
            lv_label_set_text( update_status_label, "yeah! up to date ..." );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );  
            setup_hide_indicator( update_setup_icon );
        }
        else {
            lv_label_set_text( update_status_label, "get update info failed" );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );  
            setup_hide_indicator( update_setup_icon );
        }
        lv_obj_invalidate( lv_scr_act() );
    }

    update_event = update_event & ~( UPDATE_REQUEST | UPDATE_GET_VERSION_REQUEST );
    lv_disp_trig_activity(NULL);
    lv_obj_invalidate( lv_scr_act() );
#else
    log_i("start update task, heap: %d", ESP.getFreeHeap() );

    if ( xEventGroupGetBits( update_event) & UPDATE_GET_VERSION_REQUEST ) {
        int64_t firmware_version = update_check_new_version( update_setup_get_url() );
        if ( firmware_version > atoll( __FIRMWARE__ ) && firmware_version > 0 ) {
            char version_msg[48] = "";
            snprintf( version_msg, sizeof( version_msg ), "new version: %lld", firmware_version );
            lv_label_set_text( update_status_label, (const char*)version_msg );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            setup_set_indicator( update_setup_icon, ICON_INDICATOR_1 );
            if ( last_firmware_version < firmware_version ) {
                bluetooth_message_queue_msg("{\"t\":\"notify\",\"id\":1575479849,\"src\":\"Update\",\"title\":\"update\",\"body\":\"new firmware version available\"}");
                last_firmware_version = firmware_version;
            }
        }
        else if ( firmware_version == atoll( __FIRMWARE__ ) ) {
            lv_label_set_text( update_status_label, "yeah! up to date ..." );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );  
            setup_hide_indicator( update_setup_icon );
        }
        else {
            lv_label_set_text( update_status_label, "get update info failed" );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );  
            setup_hide_indicator( update_setup_icon );
        }
        lv_obj_invalidate( lv_scr_act() );
    }
    if ( ( xEventGroupGetBits( update_event) & UPDATE_REQUEST ) && ( update_get_url() != NULL ) ) {
        if( ( WiFi.status() == WL_CONNECTED ) ) {

            uint32_t display_timeout = display_get_timeout();
            display_set_timeout( DISPLAY_MAX_TIMEOUT );

            if ( http_ota_start( update_get_url(), update_get_md5(), update_get_size() ) ) {
                reset = true;
                progress = 0;
                lv_label_set_text( update_status_label, "update ok, turn off and on!" );
                lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
                lv_label_set_text( update_btn_label, "restart");
                if( update_setup_get_autorestart() ) {
                    log_i("System reboot by user");
                    motor_vibe(20);
                    delay(20);
                    display_standby();
                    SPIFFS.end();
                    log_i("SPIFFS unmounted!");
                    delay(500);
                    ESP.restart();
                }
            }
            else {
                reset = false;
                lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
            }
            progress = 0;
            lv_bar_set_value( update_progressbar, 0 , LV_ANIM_ON );
            display_set_timeout( display_timeout );
            powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
        }
        else {
            lv_label_set_text( update_status_label, "turn wifi on!" );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );  
        }
    }
    xEventGroupClearBits( update_event, UPDATE_REQUEST | UPDATE_GET_VERSION_REQUEST );
    lv_disp_trig_activity(NULL);
    lv_obj_invalidate( lv_scr_act() );
    log_i("finish update task, heap: %d", ESP.getFreeHeap() );
    vTaskDelete( NULL );
#endif
}