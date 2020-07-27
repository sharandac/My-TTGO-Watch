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
#include <config.h>
#include <WiFi.h>

#include "weather.h"
#include "weather_fetch.h"
#include "weather_forecast.h"
#include "images/resolve_owm_icon.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"

#include "hardware/motor.h"

EventGroupHandle_t weather_forecast_event_handle = NULL;
TaskHandle_t _weather_forecast_sync_Task;
void weather_forecast_sync_Task( void * pvParameters );

lv_obj_t *weather_widget_tile = NULL;
lv_obj_t *weather_forecast_location_label = NULL;
lv_obj_t *weather_forecast_update_label = NULL;
lv_obj_t *weather_forecast_icon_imgbtn[ WEATHER_MAX_FORECAST ];
lv_obj_t *weather_forecast_temperature_label[ WEATHER_MAX_FORECAST ];
lv_style_t weather_widget_style;

weather_forcast_t weather_forecast[ WEATHER_MAX_FORECAST ];

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_IMG_DECLARE(owm_01d_64px);

static void exit_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
static void setup_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
static void refresh_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );

void weather_widget_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {
    lv_obj_t * exit_btn = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align(exit_btn, tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_weather_widget_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align(setup_btn, tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, setup_weather_widget_event_cb );

    lv_obj_t * reload_btn = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_PRESSED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_PRESSED, &refresh_32px);
    lv_obj_add_style(reload_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align(reload_btn, tile, LV_ALIGN_IN_TOP_RIGHT, -10 , STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( reload_btn, refresh_weather_widget_event_cb );

    weather_forecast_location_label = lv_label_create( tile , NULL);
    lv_label_set_text( weather_forecast_location_label, "n/a");
    lv_obj_reset_style_list( weather_forecast_location_label, LV_OBJ_PART_MAIN );
    lv_obj_align( weather_forecast_location_label, tile, LV_ALIGN_IN_TOP_LEFT, 15, STATUSBAR_HEIGHT + 15 );

    weather_forecast_update_label = lv_label_create( tile , NULL);
    lv_label_set_text( weather_forecast_update_label, "");
    lv_obj_reset_style_list( weather_forecast_update_label, LV_OBJ_PART_MAIN );
    lv_obj_align( weather_forecast_update_label, weather_forecast_location_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );

    lv_obj_t * weater_forecast_cont = lv_obj_create( tile, NULL );
    lv_obj_set_size( weater_forecast_cont, hres , 80 );
    lv_obj_add_style( weater_forecast_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( weater_forecast_cont, tile, LV_ALIGN_CENTER, 0, 10 );

    for ( int i = 0 ; i < WEATHER_MAX_FORECAST / 4 ; i++ ) {
        weather_forecast_icon_imgbtn[ i ] = lv_imgbtn_create( weater_forecast_cont, NULL);
        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_RELEASED, &owm_01d_64px);
        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_PRESSED, &owm_01d_64px);
        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_CHECKED_RELEASED, &owm_01d_64px);
        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_CHECKED_PRESSED, &owm_01d_64px);
        lv_obj_add_style( weather_forecast_icon_imgbtn[ i ], LV_IMGBTN_PART_MAIN, style);
        lv_obj_align( weather_forecast_icon_imgbtn[ i ], weater_forecast_cont, LV_ALIGN_IN_TOP_LEFT, i*60, 0 );

        weather_forecast_temperature_label[ i ] = lv_label_create( weater_forecast_cont , NULL);
        lv_label_set_text( weather_forecast_temperature_label[ i ], "n/a");
        lv_obj_reset_style_list( weather_forecast_temperature_label[ i ], LV_OBJ_PART_MAIN );
        lv_obj_align( weather_forecast_temperature_label[ i ], weather_forecast_icon_imgbtn[ i ], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    }

    // regster callback for wifi sync
    WiFi.onEvent( [](WiFiEvent_t event, WiFiEventInfo_t info) {
        xEventGroupSetBits( weather_forecast_event_handle, WEATHER_FORECAST_SYNC_REQUEST );
        vTaskResume( _weather_forecast_sync_Task );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP );

    weather_forecast_event_handle = xEventGroupCreate();
    xEventGroupClearBits( weather_forecast_event_handle, WEATHER_FORECAST_SYNC_REQUEST );

    xTaskCreate(
                        weather_forecast_sync_Task,      /* Function to implement the task */
                        "weather sync Task",    /* Name of the task */
                        5000,              /* Stack size in words */
                        NULL,               /* Task input parameter */
                        1,                  /* Priority of the task */
                        &_weather_forecast_sync_Task );  /* Task handle. */ 

}

static void exit_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
                                        mainbar_jump_to_tilenumber( MAIN_TILE, LV_ANIM_OFF );
                                        break;
    }
}

static void setup_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
                                        weather_jump_to_setup();
                                        break;
    }
}

static void refresh_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
                                        weather_forecast_sync_request();
                                        weather_widget_sync_request();
                                        break;
    }
}

void weather_forecast_sync_request( void ) {
    if ( xEventGroupGetBits( weather_forecast_event_handle ) & WEATHER_FORECAST_SYNC_REQUEST ) {
        return;
    }
    else {
        xEventGroupSetBits( weather_forecast_event_handle, WEATHER_FORECAST_SYNC_REQUEST );
        vTaskResume( _weather_forecast_sync_Task );    
    }
}

void weather_forecast_sync_Task( void * pvParameters ) {
    weather_config_t *weather_config = weather_get_config();
    uint32_t retval = -1;

    while( true ) {
        vTaskDelay( 500 );
        if ( xEventGroupGetBits( weather_forecast_event_handle ) & WEATHER_FORECAST_SYNC_REQUEST ) {   
            if ( weather_config->autosync ) {
                retval = weather_fetch_forecast( weather_get_config() , &weather_forecast[ 0 ] );
                if ( retval == 200 ) {
                    for( int i = 0 ; i < WEATHER_MAX_FORECAST / 4 ; i++ ) {
                        lv_label_set_text( weather_forecast_location_label, weather_forecast[ i * 4 ].name );
                        lv_label_set_text( weather_forecast_temperature_label[ i ], weather_forecast[ i * 4 ].temp );
                        lv_obj_align( weather_forecast_temperature_label[ i ], weather_forecast_icon_imgbtn[ i ], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
                        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_RELEASED, resolve_owm_icon( weather_forecast[ i * 4 ].icon ) );
                        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_PRESSED, resolve_owm_icon( weather_forecast[ i * 4 ].icon ) );
                        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_CHECKED_RELEASED, resolve_owm_icon( weather_forecast[ i * 4 ].icon ) );
                        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_CHECKED_PRESSED, resolve_owm_icon( weather_forecast[ i * 4 ].icon ) );

                        time_t now;
                        struct tm info;
                        char buf[64];
                        time( &now );
                        localtime_r( &now, &info );
                        strftime( buf, sizeof(buf), "updated: %d.%b %H:%M", &info );
                        lv_label_set_text( weather_forecast_update_label, buf );
                    }
                    motor_vibe( 1 );        
                }
                else {
                    char buf[64];
                    snprintf( buf, sizeof(buf), "Error: %d", retval );
                    lv_label_set_text( weather_forecast_update_label, buf );
                }
            }
            xEventGroupClearBits( weather_forecast_event_handle, WEATHER_FORECAST_SYNC_REQUEST );
        }
        vTaskSuspend( _weather_forecast_sync_Task );
    }
}
