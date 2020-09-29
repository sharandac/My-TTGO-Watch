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
#include "esp_task_wdt.h"

#include "weather.h"
#include "weather_fetch.h"
#include "weather_forecast.h"
#include "images/resolve_owm_icon.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"

#include "hardware/powermgm.h"
#include "hardware/wifictl.h"
#include "hardware/alloc.h"

EventGroupHandle_t weather_forecast_event_handle = NULL;
TaskHandle_t _weather_forecast_sync_Task;

lv_obj_t *weather_forecast_tile = NULL;
lv_style_t weather_forecast_style;
uint32_t weather_forecast_tile_num;

lv_obj_t *weather_forecast_location_label = NULL;
lv_obj_t *weather_forecast_update_label = NULL;
lv_obj_t *weather_forecast_time_label[ WEATHER_MAX_FORECAST ];
lv_obj_t *weather_forecast_icon_imgbtn[ WEATHER_MAX_FORECAST ];
lv_obj_t *weather_forecast_temperature_label[ WEATHER_MAX_FORECAST ];
lv_obj_t *weather_forecast_wind_label[ WEATHER_MAX_FORECAST ];

static weather_forcast_t *weather_forecast = NULL;

void weather_forecast_sync_Task( void * pvParameters );
bool weather_forecast_wifictl_event_cb( EventBits_t event, void *arg );

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_IMG_DECLARE(owm_01d_64px);

static void exit_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
static void setup_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
static void refresh_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );

void weather_forecast_tile_setup( uint32_t tile_num ) {

    weather_forecast = (weather_forcast_t*)CALLOC( sizeof( weather_forcast_t ) * WEATHER_MAX_FORECAST , 1 );
    if( !weather_forecast ) {
      log_e("weather forecast calloc faild");
      while(true);
    }

    weather_forecast_tile_num = tile_num;
    weather_forecast_tile = mainbar_get_tile_obj( weather_forecast_tile_num );
    lv_style_copy( &weather_forecast_style, mainbar_get_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( weather_forecast_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &weather_forecast_style );
    lv_obj_align(exit_btn, weather_forecast_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_weather_widget_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( weather_forecast_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &weather_forecast_style );
    lv_obj_align(setup_btn, weather_forecast_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, setup_weather_widget_event_cb );

    lv_obj_t * reload_btn = lv_imgbtn_create( weather_forecast_tile, NULL);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_PRESSED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_PRESSED, &refresh_32px);
    lv_obj_add_style(reload_btn, LV_IMGBTN_PART_MAIN, &weather_forecast_style );
    lv_obj_align(reload_btn, weather_forecast_tile, LV_ALIGN_IN_TOP_RIGHT, -10 , 10 );
    lv_obj_set_event_cb( reload_btn, refresh_weather_widget_event_cb );

    weather_forecast_location_label = lv_label_create( weather_forecast_tile , NULL);
    lv_label_set_text( weather_forecast_location_label, "n/a");
    lv_obj_reset_style_list( weather_forecast_location_label, LV_OBJ_PART_MAIN );
    lv_obj_align( weather_forecast_location_label, weather_forecast_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );

    weather_forecast_update_label = lv_label_create( weather_forecast_tile , NULL);
    lv_label_set_text( weather_forecast_update_label, "");
    lv_obj_reset_style_list( weather_forecast_update_label, LV_OBJ_PART_MAIN );
    lv_obj_align( weather_forecast_update_label, weather_forecast_location_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );

    lv_obj_t * weater_forecast_cont = lv_obj_create( weather_forecast_tile, NULL );
    lv_obj_set_size( weater_forecast_cont, lv_disp_get_hor_res( NULL ) , 96 );
    lv_obj_add_style( weater_forecast_cont, LV_OBJ_PART_MAIN, &weather_forecast_style  );
    lv_obj_align( weater_forecast_cont, weather_forecast_tile, LV_ALIGN_CENTER, 0, 0 );

    for ( int i = 0 ; i < WEATHER_MAX_FORECAST / 4 ; i++ ) {
        weather_forecast_icon_imgbtn[ i ] = lv_imgbtn_create( weater_forecast_cont, NULL);
        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_RELEASED, &owm_01d_64px);
        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_PRESSED, &owm_01d_64px);
        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_CHECKED_RELEASED, &owm_01d_64px);
        lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_CHECKED_PRESSED, &owm_01d_64px);
        lv_obj_add_style( weather_forecast_icon_imgbtn[ i ], LV_IMGBTN_PART_MAIN, &weather_forecast_style );
        lv_obj_align( weather_forecast_icon_imgbtn[ i ], weater_forecast_cont, LV_ALIGN_IN_LEFT_MID, i*58, 0 );

        weather_forecast_temperature_label[ i ] = lv_label_create( weater_forecast_cont , NULL);
        lv_label_set_text( weather_forecast_temperature_label[ i ], "n/a");
        lv_obj_reset_style_list( weather_forecast_temperature_label[ i ], LV_OBJ_PART_MAIN );
        lv_obj_align( weather_forecast_temperature_label[ i ], weather_forecast_icon_imgbtn[ i ], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

        weather_forecast_wind_label[ i ] = lv_label_create( weater_forecast_cont , NULL);
        lv_label_set_text( weather_forecast_wind_label[i], "");
        lv_obj_reset_style_list( weather_forecast_wind_label[i], LV_OBJ_PART_MAIN);
        lv_obj_align( weather_forecast_wind_label[i], weather_forecast_icon_imgbtn[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

        weather_forecast_time_label[ i ] = lv_label_create( weater_forecast_cont , NULL);
        lv_label_set_text( weather_forecast_time_label[ i ], "n/a");
        lv_obj_reset_style_list( weather_forecast_time_label[ i ], LV_OBJ_PART_MAIN );
        lv_obj_align( weather_forecast_time_label[ i ], weather_forecast_icon_imgbtn[ i ], LV_ALIGN_OUT_TOP_MID, 0, 0);
    }

    weather_forecast_event_handle = xEventGroupCreate();

    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT, weather_forecast_wifictl_event_cb, "weather forcecast" );
}

bool weather_forecast_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_CONNECT:       weather_config_t *tmp_weather_config = weather_get_config();
                                    if ( tmp_weather_config->autosync ) {
                                        weather_forecast_sync_request();
                                    }
                                    break;
    }
    return( true );
}

static void exit_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

static void setup_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       weather_jump_to_setup();
                                        break;
    }
}

static void refresh_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       weather_forecast_sync_request();
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
        xTaskCreate(    weather_forecast_sync_Task,      /* Function to implement the task */
                        "weather forecast sync Task",    /* Name of the task */
                        5000,                            /* Stack size in words */
                        NULL,                            /* Task input parameter */
                        1,                               /* Priority of the task */
                        &_weather_forecast_sync_Task );  /* Task handle. */ 
    }
}

void weather_forecast_sync_Task( void * pvParameters ) {
    weather_config_t *weather_config = weather_get_config();
    int32_t retval = -1;

    log_i("start weather forecast task, heap: %d", ESP.getFreeHeap() );

    vTaskDelay( 250 );

    if ( xEventGroupGetBits( weather_forecast_event_handle ) & WEATHER_FORECAST_SYNC_REQUEST ) {   
        if ( weather_config->autosync ) {
            retval = weather_fetch_forecast( weather_get_config() , &weather_forecast[ 0 ] );
            if ( retval == 200 ) {
                time_t now;
                struct tm info;
                char buf[64];

                lv_label_set_text( weather_forecast_location_label, weather_forecast[ 0 ].name );

                for( int i = 0 ; i < WEATHER_MAX_FORECAST / 4 ; i++ ) {
                    lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_RELEASED, resolve_owm_icon( weather_forecast[ i * 2 ].icon ) );
                    lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_PRESSED, resolve_owm_icon( weather_forecast[ i * 2 ].icon ) );
                    lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_CHECKED_RELEASED, resolve_owm_icon( weather_forecast[ i * 2 ].icon ) );
                    lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_CHECKED_PRESSED, resolve_owm_icon( weather_forecast[ i * 2 ].icon ) );

                    lv_label_set_text( weather_forecast_temperature_label[ i ], weather_forecast[ i * 2 ].temp );

                    if(weather_config->showWind)
                    {
                        lv_obj_align(weather_forecast_temperature_label[i], weather_forecast_icon_imgbtn[i], LV_ALIGN_OUT_BOTTOM_MID, 0, -22);
                        lv_label_set_text(weather_forecast_wind_label[i], weather_forecast[i * 2].wind);
                        lv_obj_align(weather_forecast_wind_label[i], weather_forecast_icon_imgbtn[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
                    }
                    else
                    {
                        lv_obj_align(weather_forecast_temperature_label[i], weather_forecast_icon_imgbtn[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
                        lv_label_set_text(weather_forecast_wind_label[i], "");
                    }

                    localtime_r( &weather_forecast[ i * 2 ].timestamp, &info );
                    strftime( buf, sizeof(buf), "%H:%M", &info );
                    lv_label_set_text( weather_forecast_time_label[ i ], buf );
                    lv_obj_align( weather_forecast_time_label[ i ], weather_forecast_icon_imgbtn[ i ], LV_ALIGN_OUT_TOP_MID, 0, 0);
                }

                time( &now );
                localtime_r( &now, &info );
                strftime( buf, sizeof(buf), "updated: %d.%b %H:%M", &info );
                lv_label_set_text( weather_forecast_update_label, buf );
                lv_obj_invalidate( lv_scr_act() );
            }
        }
    }
    xEventGroupClearBits( weather_forecast_event_handle, WEATHER_FORECAST_SYNC_REQUEST );
    log_i("finsh weather forecast task, heap: %d", ESP.getFreeHeap() );
    vTaskDelete( NULL );
}
