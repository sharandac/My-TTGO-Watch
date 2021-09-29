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
#include "weather.h"
#include "weather_fetch.h"
#include "weather_forecast.h"
#include "images/resolve_owm_icon.h"
#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"
#include "hardware/powermgm.h"
#include "hardware/wifictl.h"
#include "utils/alloc.h"
#include "gui/mainbar/mainbar.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #include "esp_task_wdt.h"

    EventGroupHandle_t weather_forecast_event_handle = NULL;
    TaskHandle_t _weather_forecast_sync_Task;
#endif

lv_obj_t *weather_forecast_tile = NULL;
uint32_t weather_forecast_tile_num;

lv_obj_t *weather_forecast_location_label = NULL;
lv_obj_t *weather_forecast_update_label = NULL;
lv_obj_t *weather_forecast_time_label[ WEATHER_MAX_FORECAST ];
lv_obj_t *weather_forecast_icon_imgbtn[ WEATHER_MAX_FORECAST ];
lv_obj_t *weather_forecast_temperature_label[ WEATHER_MAX_FORECAST ];
lv_obj_t *weather_forecast_wind_label[ WEATHER_MAX_FORECAST ];

static weather_forcast_t *weather_forecast = NULL;

LV_IMG_DECLARE(refresh_32px);
LV_IMG_DECLARE(owm01d_64px);

bool weather_button_event_cb( EventBits_t event, void *arg );
void weather_forecast_sync_Task( void * pvParameters );
bool weather_forecast_wifictl_event_cb( EventBits_t event, void *arg );
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

    lv_obj_t * exit_btn = wf_add_exit_button( weather_forecast_tile, exit_weather_widget_event_cb );
    lv_obj_align(exit_btn, weather_forecast_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    lv_obj_t * setup_btn = wf_add_setup_button( weather_forecast_tile, setup_weather_widget_event_cb );
    lv_obj_align(setup_btn, weather_forecast_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );

    lv_obj_t * reload_btn = wf_add_refresh_button( weather_forecast_tile, refresh_weather_widget_event_cb );
    lv_obj_align(reload_btn, weather_forecast_tile, LV_ALIGN_IN_TOP_RIGHT, -10 , 10 );

    weather_forecast_location_label = lv_label_create( weather_forecast_tile , NULL);
    lv_label_set_text( weather_forecast_location_label, "n/a");
    lv_obj_reset_style_list( weather_forecast_location_label, LV_OBJ_PART_MAIN );
    #if defined( ROUND_DISPLAY )
        lv_obj_align( weather_forecast_location_label, weather_forecast_tile, LV_ALIGN_IN_TOP_MID, 0, 10 );
    #else
        lv_obj_align( weather_forecast_location_label, weather_forecast_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );
    #endif

    weather_forecast_update_label = lv_label_create( weather_forecast_tile , NULL);
    lv_label_set_text( weather_forecast_update_label, "");
    lv_obj_reset_style_list( weather_forecast_update_label, LV_OBJ_PART_MAIN );
    #if defined( ROUND_DISPLAY )
        lv_obj_align( weather_forecast_update_label, weather_forecast_location_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    #else
        lv_obj_align( weather_forecast_update_label, weather_forecast_location_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );
    #endif

    lv_obj_t * weater_forecast_cont = lv_obj_create( weather_forecast_tile, NULL );
    lv_obj_set_size( weater_forecast_cont, lv_disp_get_hor_res( NULL ) , ( lv_disp_get_ver_res( NULL ) / 4 ) * 3 );
    lv_obj_add_style( weater_forecast_cont, LV_OBJ_PART_MAIN, ws_get_mainbar_style()  );
    lv_obj_align( weater_forecast_cont, weather_forecast_tile, LV_ALIGN_CENTER, 0, 0 );

    for ( int i = 0 ; i < WEATHER_MAX_FORECAST && i < WEATHER_MAX_FORECAST_ICON; i++ ) {
        weather_forecast_icon_imgbtn[ i ] = wf_add_image_button( weater_forecast_cont, owm01d_64px, NULL, APP_STYLE );
        lv_obj_align( weather_forecast_icon_imgbtn[ i ], weater_forecast_cont, LV_ALIGN_IN_LEFT_MID, ( WEATHER_FORCAST_ICON_SPACE / 2 ) + ( i * WEATHER_ICON_SIZE + WEATHER_FORCAST_ICON_SPACE ) , 0 );

        weather_forecast_temperature_label[ i ] = lv_label_create( weater_forecast_cont , NULL);
        lv_label_set_text( weather_forecast_temperature_label[ i ], "n/a");
        lv_obj_reset_style_list( weather_forecast_temperature_label[ i ], LV_OBJ_PART_MAIN );
        lv_obj_align( weather_forecast_temperature_label[ i ], weather_forecast_icon_imgbtn[ i ], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

        weather_forecast_wind_label[ i ] = lv_label_create( weater_forecast_cont , NULL);
        lv_label_set_text( weather_forecast_wind_label[i], "");
        lv_obj_reset_style_list( weather_forecast_wind_label[i], LV_OBJ_PART_MAIN);
        lv_obj_align( weather_forecast_wind_label[i], weather_forecast_temperature_label[ i ], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

        weather_forecast_time_label[ i ] = lv_label_create( weater_forecast_cont , NULL);
        lv_label_set_text( weather_forecast_time_label[ i ], "n/a");
        lv_obj_reset_style_list( weather_forecast_time_label[ i ], LV_OBJ_PART_MAIN );
        lv_obj_align( weather_forecast_time_label[ i ], weather_forecast_icon_imgbtn[ i ], LV_ALIGN_OUT_TOP_MID, 0, 0);
    }
    mainbar_add_tile_button_cb( weather_forecast_tile_num, weather_button_event_cb );
}

bool weather_button_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_EXIT:           mainbar_jump_back();
                                    break;
        case BUTTON_SETUP:          weather_jump_to_setup();
                                    break;
        case BUTTON_REFRESH:        weather_sync_request();
                                    break;
    }
    return( true );
}

static void exit_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
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
        case( LV_EVENT_CLICKED ):       weather_sync_request();
                                        break;
    }
}

void weather_forecast_sync( void  ) {
    weather_config_t *weather_config = weather_get_config();
    int32_t retval = -1;

    retval = weather_fetch_forecast( weather_get_config() , &weather_forecast[ 0 ] );
    
    if ( retval == 200 ) {
        time_t now;
        struct tm info;
        char buf[64];

        lv_label_set_text( weather_forecast_location_label, weather_forecast[ 0 ].name );
        #if defined( ROUND_DISPLAY )
            lv_obj_align( weather_forecast_location_label, weather_forecast_tile, LV_ALIGN_IN_TOP_MID, 0, 10 );
        #else
            lv_obj_align( weather_forecast_location_label, weather_forecast_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );
        #endif

        for ( int i = 0 ; i < WEATHER_MAX_FORECAST && i < WEATHER_MAX_FORECAST_ICON ; i++ ) {
            lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_RELEASED, resolve_owm_icon( weather_forecast[ i * 2 ].icon ) );
            lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_PRESSED, resolve_owm_icon( weather_forecast[ i * 2 ].icon ) );
            lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_CHECKED_RELEASED, resolve_owm_icon( weather_forecast[ i * 2 ].icon ) );
            lv_imgbtn_set_src( weather_forecast_icon_imgbtn[ i ], LV_BTN_STATE_CHECKED_PRESSED, resolve_owm_icon( weather_forecast[ i * 2 ].icon ) );
            lv_label_set_text( weather_forecast_temperature_label[ i ], weather_forecast[ i * 2 ].temp );

            if(weather_config->showWind) {
                lv_label_set_text(weather_forecast_wind_label[i], weather_forecast[i * 2].wind);
            }
            else {
                lv_label_set_text(weather_forecast_wind_label[i], "");
            }
            
            lv_obj_align(weather_forecast_temperature_label[i], weather_forecast_icon_imgbtn[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
            lv_obj_align(weather_forecast_wind_label[i], weather_forecast_temperature_label[i], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

            localtime_r( &weather_forecast[ i * 2 ].timestamp, &info );
            strftime( buf, sizeof(buf), "%H:%M", &info );
            lv_label_set_text( weather_forecast_time_label[ i ], buf );
            lv_obj_align( weather_forecast_time_label[ i ], weather_forecast_icon_imgbtn[ i ], LV_ALIGN_OUT_TOP_MID, 0, 0);
        }

        time( &now );
        localtime_r( &now, &info );
        strftime( buf, sizeof(buf), "updated: %d.%b %H:%M", &info );
        lv_label_set_text( weather_forecast_update_label, buf );
        #if defined( ROUND_DISPLAY )
            lv_obj_align( weather_forecast_update_label, weather_forecast_location_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
        #else
            lv_obj_align( weather_forecast_update_label, weather_forecast_location_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );
        #endif
        lv_obj_invalidate( lv_scr_act() );
    }
}
