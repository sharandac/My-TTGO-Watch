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
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>

#include "update.h"
#include "update_setup.h"
#include "update_check_version.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup.h"
#include "gui/statusbar.h"
#include "hardware/display.h"

EventGroupHandle_t update_event_handle = NULL;
TaskHandle_t _update_Task;
void update_Task( void * pvParameters );

lv_obj_t *update_settings_tile=NULL;
lv_style_t update_settings_style;
uint32_t update_tile_num;

lv_obj_t *update_btn = NULL;
lv_obj_t *update_status_label = NULL;
lv_obj_t *update_setup_icon_cont = NULL;
lv_obj_t *update_info_img = NULL;

static void enter_update_setup_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_update_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_update_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void update_event_handler(lv_obj_t * obj, lv_event_t event );

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(update_64px);
LV_IMG_DECLARE(info_1_16px);

void update_tile_setup( void ) {
    // get an app tile and copy mainstyle
    update_tile_num = mainbar_add_app_tile( 1, 2 );
    update_settings_tile = mainbar_get_tile_obj( update_tile_num );

    update_setup_tile_setup( update_tile_num + 1 );

    lv_style_copy( &update_settings_style, mainbar_get_style() );
    lv_style_set_bg_color( &update_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &update_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &update_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( update_settings_tile, LV_OBJ_PART_MAIN, &update_settings_style );

    // register an setup icon an set an callback
    update_setup_icon_cont = setup_tile_register_setup();
    lv_obj_t *update_setup = lv_imgbtn_create ( update_setup_icon_cont, NULL);
    lv_imgbtn_set_src( update_setup, LV_BTN_STATE_RELEASED, &update_64px);
    lv_imgbtn_set_src( update_setup, LV_BTN_STATE_PRESSED, &update_64px);
    lv_imgbtn_set_src( update_setup, LV_BTN_STATE_CHECKED_RELEASED, &update_64px);
    lv_imgbtn_set_src( update_setup, LV_BTN_STATE_CHECKED_PRESSED, &update_64px);
    lv_obj_add_style( update_setup, LV_IMGBTN_PART_MAIN, mainbar_get_style() );
    lv_obj_align( update_setup, update_setup_icon_cont, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_event_cb( update_setup, enter_update_setup_event_cb );

    update_info_img = lv_img_create( update_setup_icon_cont, NULL );
    lv_img_set_src( update_info_img, &info_1_16px );
    lv_obj_align( update_info_img, update_setup_icon_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( update_info_img, true );

    lv_obj_t *setup_btn = lv_imgbtn_create( update_settings_tile, NULL);
    lv_imgbtn_set_src( setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src( setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src( setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src( setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style( setup_btn, LV_IMGBTN_PART_MAIN, &update_settings_style );
    lv_obj_align( setup_btn, update_settings_tile, LV_ALIGN_IN_TOP_RIGHT, -10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( setup_btn, enter_update_setup_setup_event_cb );

    lv_obj_t *exit_btn = lv_imgbtn_create( update_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &update_settings_style );
    lv_obj_align( exit_btn, update_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_update_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( update_settings_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &update_settings_style  );
    lv_label_set_text( exit_label, "update");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *update_version_cont = lv_obj_create( update_settings_tile, NULL );
    lv_obj_set_size(update_version_cont, LV_HOR_RES_MAX , 40);
    lv_obj_add_style( update_version_cont, LV_OBJ_PART_MAIN, &update_settings_style  );
    lv_obj_align( update_version_cont, update_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    lv_obj_t *update_version_label = lv_label_create( update_version_cont, NULL);
    lv_obj_add_style( update_version_label, LV_OBJ_PART_MAIN, &update_settings_style  );
    lv_label_set_text( update_version_label, "firmware version" );
    lv_obj_align( update_version_label, update_version_cont, LV_ALIGN_IN_TOP_MID, 0, 0 );
    lv_obj_t *update_firmware_version_label = lv_label_create( update_version_cont, NULL);
    lv_obj_add_style( update_firmware_version_label, LV_OBJ_PART_MAIN, &update_settings_style  );
    lv_label_set_text( update_firmware_version_label, __FIRMWARE__ );
    lv_obj_align( update_firmware_version_label, update_version_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );

    update_btn = lv_btn_create( update_settings_tile, NULL);
    lv_obj_set_event_cb( update_btn, update_event_handler );
    lv_obj_align( update_btn, NULL, LV_ALIGN_CENTER, 0, 40);
    lv_obj_t *update_btn_label = lv_label_create( update_btn, NULL );
    lv_label_set_text( update_btn_label, "update");

    update_status_label = lv_label_create( update_settings_tile, NULL);
    lv_obj_add_style( update_status_label, LV_OBJ_PART_MAIN, &update_settings_style  );
    lv_label_set_text( update_status_label, "" );
    lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    // regster callback
    WiFi.onEvent( [](WiFiEvent_t event, WiFiEventInfo_t info) {
        if ( update_setup_get_autosync() ) {
            update_check_version();
        }
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP );

    update_event_handle = xEventGroupCreate();
    xEventGroupClearBits( update_event_handle, UPDATE_REQUEST );
}

static void enter_update_setup_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( update_tile_num + 1, LV_ANIM_OFF );
                                        break;
    }
}

static void enter_update_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( update_tile_num, LV_ANIM_OFF );
                                        break;
    }
}
static void exit_update_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
                                        break;
    }
}

static void update_event_handler(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) {
        if ( xEventGroupGetBits( update_event_handle) & ( UPDATE_GET_VERSION_REQUEST | UPDATE_REQUEST ) )  {
            return;
        }
        else {
            xEventGroupSetBits( update_event_handle, UPDATE_REQUEST );
            xTaskCreate(    update_Task,        /* Function to implement the task */
                            "update Task",      /* Name of the task */
                            10000,               /* Stack size in words */
                            NULL,               /* Task input parameter */
                            0,                  /* Priority of the task */
                            &_update_Task );    /* Task handle. */
        }
    }
}

void update_check_version( void ) {
    if ( xEventGroupGetBits( update_event_handle ) & ( UPDATE_GET_VERSION_REQUEST | UPDATE_REQUEST ) ) {
        return;
    }
    else {
        xEventGroupSetBits( update_event_handle, UPDATE_GET_VERSION_REQUEST );
        xTaskCreate(    update_Task,        /* Function to implement the task */
                        "update Task",      /* Name of the task */
                        5000,               /* Stack size in words */
                        NULL,               /* Task input parameter */
                        1,                  /* Priority of the task */
                        &_update_Task );    /* Task handle. */
    }
}

void update_Task( void * pvParameters ) {
    log_i("start update task");

    if ( xEventGroupGetBits( update_event_handle) & UPDATE_GET_VERSION_REQUEST ) {
        int64_t firmware_version = update_check_new_version();
        if ( firmware_version > atol( __FIRMWARE__ ) && firmware_version > 0 ) {
            char version_msg[48] = "";
            snprintf( version_msg, sizeof( version_msg ), "new version: %lld", firmware_version );
            lv_label_set_text( update_status_label, (const char*)version_msg );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );
            lv_obj_set_hidden( update_info_img, false );
        }
        else if ( firmware_version == atol( __FIRMWARE__ ) ) {
            lv_label_set_text( update_status_label, "yeah! up to date ..." );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );  
            lv_obj_set_hidden( update_info_img, true );
        }
        lv_obj_invalidate( lv_scr_act() );
    }
    if ( xEventGroupGetBits( update_event_handle) & UPDATE_REQUEST ) {
        if( WiFi.status() == WL_CONNECTED ) {

            uint32_t display_timeout = display_get_timeout();
            display_set_timeout( DISPLAY_MAX_TIMEOUT );

            WiFiClient client;

            lv_label_set_text( update_status_label, "start update ..." );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );  

            t_httpUpdate_return ret = httpUpdate.update( client, "http://www.neo-guerillaz.de/ttgo-t-watch2020_v1.ino.bin" );

            switch(ret) {
                case HTTP_UPDATE_FAILED:
                    lv_label_set_text( update_status_label, "update failed" );
                    lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );  
                    break;

                case HTTP_UPDATE_NO_UPDATES:
                    lv_label_set_text( update_status_label, "no update" );
                    lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );  
                    break;

                case HTTP_UPDATE_OK:
                    lv_label_set_text( update_status_label, "update ok" );
                    lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );  
                    break;
            }
            display_set_timeout( display_timeout );
        }
        else {
            lv_label_set_text( update_status_label, "turn wifi on!" );
            lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );  
        }
    }
    xEventGroupClearBits( update_event_handle, UPDATE_REQUEST | UPDATE_GET_VERSION_REQUEST );
    lv_disp_trig_activity(NULL);
    vTaskDelete( NULL );
}