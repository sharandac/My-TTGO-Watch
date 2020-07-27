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
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "hardware/display.h"
#include "hardware/motor.h"

EventGroupHandle_t update_event_handle = NULL;
TaskHandle_t _update_Task;
void update_Task( void * pvParameters );

lv_obj_t *update_settings_tile = NULL;
lv_obj_t *update_btn = NULL;
lv_obj_t *update_status_label = NULL;
lv_style_t update_settings_style;

static void exit_update_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void update_event_handler(lv_obj_t * obj, lv_event_t event );

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(time_32px);

void update_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {
    lv_style_init( &update_settings_style );
    lv_style_set_radius( &update_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color( &update_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &update_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &update_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &update_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor( &update_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    update_settings_tile = lv_obj_create( tile, NULL);
    lv_obj_set_size( update_settings_tile, hres , vres);
    lv_obj_align( update_settings_tile, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style( update_settings_tile, LV_OBJ_PART_MAIN, &update_settings_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( update_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( exit_btn, update_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_update_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( update_settings_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( exit_label, "update");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *update_version_cont = lv_obj_create( update_settings_tile, NULL );
    lv_obj_set_size(update_version_cont, hres , 40);
    lv_obj_add_style( update_version_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( update_version_cont, update_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    lv_obj_t *update_version_label = lv_label_create( update_version_cont, NULL);
    lv_obj_add_style( update_version_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( update_version_label, "firmware version" );
    lv_obj_align( update_version_label, update_version_cont, LV_ALIGN_IN_TOP_MID, 0, 0 );
    lv_obj_t *update_firmware_version_label = lv_label_create( update_version_cont, NULL);
    lv_obj_add_style( update_firmware_version_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( update_firmware_version_label, __FIRMWARE__ );
    lv_obj_align( update_firmware_version_label, update_version_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );

    update_btn = lv_btn_create( update_settings_tile, NULL);
    lv_obj_set_event_cb( update_btn, update_event_handler );
    lv_obj_align( update_btn, NULL, LV_ALIGN_CENTER, 0, 40);
    lv_obj_t *update_btn_label = lv_label_create( update_btn, NULL );
    lv_label_set_text( update_btn_label, "update");

    update_status_label = lv_label_create( update_settings_tile, NULL);
    lv_obj_add_style( update_status_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( update_status_label, "" );
    lv_obj_align( update_status_label, update_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
    
    update_event_handle = xEventGroupCreate();
    xEventGroupClearBits( update_event_handle, UPDATE_REQUEST );

    xTaskCreate(
                        update_Task,      /* Function to implement the task */
                        "update Task",    /* Name of the task */
                        5000,              /* Stack size in words */
                        NULL,               /* Task input parameter */
                        1,                  /* Priority of the task */
                        &_update_Task );  /* Task handle. */
}

static void exit_update_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
                                        mainbar_jump_to_tilenumber( SETUP_TILE, LV_ANIM_OFF );
                                        break;
    }
}

static void update_event_handler(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) {
        motor_vibe( 1 );
        if ( xEventGroupGetBits( update_event_handle) & UPDATE_REQUEST )  {
            return;
        }
        else {
            xEventGroupSetBits( update_event_handle, UPDATE_REQUEST );
            vTaskResume( _update_Task );
        }
    }
}

void update_Task( void * pvParameters ) {
    while( true ) {
        vTaskDelay( 500 );
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
            xEventGroupClearBits( update_event_handle, UPDATE_REQUEST );
        }
        lv_disp_trig_activity(NULL);
        vTaskSuspend( _update_Task );
    }
}