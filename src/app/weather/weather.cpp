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
#include <TTGO.h>
#include <WiFi.h>
#include "esp_task_wdt.h"

#include "weather.h"
#include "weather_fetch.h"
#include "weather_forecast.h"
#include "weather_setup.h"
#include "images/resolve_owm_icon.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "hardware/motor.h"
#include "hardware/powermgm.h"
#include "hardware/json_psram_allocator.h"

EventGroupHandle_t weather_widget_event_handle = NULL;
TaskHandle_t _weather_widget_sync_Task;
void weather_widget_sync_Task( void * pvParameters );

weather_config_t weather_config;
weather_forcast_t weather_today;

uint32_t weather_app_tile_num;
uint32_t weather_app_setup_tile_num;

lv_obj_t *weather_app_cont = NULL;
lv_obj_t *weather_widget_cont = NULL;
lv_obj_t *weather_widget_info_img = NULL;
lv_obj_t *weather_widget_condition_img = NULL;
lv_obj_t *weather_widget_temperature_label = NULL;
lv_obj_t *weather_widget_wind_label = NULL;

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
LV_IMG_DECLARE(owm_01d_64px);
LV_IMG_DECLARE(info_ok_16px);
LV_IMG_DECLARE(info_fail_16px);
LV_FONT_DECLARE(Ubuntu_16px);

void weather_app_setup( void ) {

    weather_load_config();

    // get an app tile and copy mainstyle
    weather_app_tile_num = mainbar_add_app_tile( 1, 2 );
    weather_app_setup_tile_num = weather_app_tile_num + 1;

    weather_forecast_tile_setup( weather_app_tile_num );
    weather_setup_tile_setup( weather_app_setup_tile_num );

    weather_app_cont = app_tile_register_app( "weather");
    lv_obj_t *weather_app_icon = lv_imgbtn_create( weather_app_cont, NULL );
    lv_imgbtn_set_src( weather_app_icon, LV_BTN_STATE_RELEASED, &owm_01d_64px);
    lv_imgbtn_set_src( weather_app_icon, LV_BTN_STATE_PRESSED, &owm_01d_64px);
    lv_imgbtn_set_src( weather_app_icon, LV_BTN_STATE_CHECKED_RELEASED, &owm_01d_64px);
    lv_imgbtn_set_src( weather_app_icon, LV_BTN_STATE_CHECKED_PRESSED, &owm_01d_64px);
    lv_obj_reset_style_list( weather_app_icon, LV_OBJ_PART_MAIN );
    lv_obj_align( weather_app_icon , weather_widget_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( weather_app_icon, enter_weather_widget_event_cb );
    
    // get an widget container from main_tile
    // create widget weather condition icon and temperature label
    weather_widget_cont = main_tile_register_widget();
    weather_widget_condition_img = lv_imgbtn_create( weather_widget_cont, NULL );
    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_RELEASED, &owm_01d_64px);
    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_PRESSED, &owm_01d_64px);
    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_CHECKED_RELEASED, &owm_01d_64px);
    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_CHECKED_PRESSED, &owm_01d_64px);
    lv_obj_reset_style_list( weather_widget_condition_img, LV_OBJ_PART_MAIN );
    lv_obj_align( weather_widget_condition_img , weather_widget_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( weather_widget_condition_img, enter_weather_widget_event_cb );

    weather_widget_info_img = lv_img_create( weather_widget_cont, NULL );
    lv_img_set_src( weather_widget_info_img, &info_ok_16px );
    lv_obj_align( weather_widget_info_img, weather_widget_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( weather_widget_info_img, true );

    weather_widget_temperature_label = lv_label_create( weather_widget_cont , NULL);
    lv_label_set_text( weather_widget_temperature_label, "n/a");
    lv_obj_reset_style_list( weather_widget_temperature_label, LV_OBJ_PART_MAIN );
    lv_obj_align( weather_widget_temperature_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    weather_widget_wind_label = lv_label_create( weather_widget_cont , NULL);
    lv_label_set_text( weather_widget_wind_label, "");
    lv_obj_reset_style_list( weather_widget_wind_label, LV_OBJ_PART_MAIN );
    lv_obj_align( weather_widget_wind_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    if( weather_config.showWind )
    {
        lv_obj_align( weather_widget_temperature_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, -20);
        lv_obj_align( weather_widget_wind_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, +5);
    }

    // regster callback for wifi sync
    WiFi.onEvent( [](WiFiEvent_t event, WiFiEventInfo_t info) {
        if ( weather_config.autosync ) {
            weather_widget_sync_request();
        }
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP );

    // regster callback for wifi sync
    WiFi.onEvent( [](WiFiEvent_t event, WiFiEventInfo_t info) {
        lv_obj_set_hidden( weather_widget_info_img, true );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_STOP );

    weather_widget_event_handle = xEventGroupCreate();
    xEventGroupClearBits( weather_widget_event_handle, WEATHER_WIDGET_SYNC_REQUEST );
}

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( weather_app_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

void weather_jump_to_forecast( void ) {
    statusbar_hide( true );
    mainbar_jump_to_tilenumber( weather_app_tile_num, LV_ANIM_ON );
}

void weather_jump_to_setup( void ) {
    statusbar_hide( true );
    mainbar_jump_to_tilenumber( weather_app_setup_tile_num, LV_ANIM_ON );    
}

void weather_widget_sync_request( void ) {
    if ( xEventGroupGetBits( weather_widget_event_handle ) & WEATHER_WIDGET_SYNC_REQUEST ) {
        return;
    }
    else {
        xEventGroupSetBits( weather_widget_event_handle, WEATHER_WIDGET_SYNC_REQUEST );
        lv_obj_set_hidden( weather_widget_info_img, true );
        xTaskCreate(    weather_widget_sync_Task,       /* Function to implement the task */
                        "weather widget sync Task",     /* Name of the task */
                        5000,                           /* Stack size in words */
                        NULL,                           /* Task input parameter */
                        1,                              /* Priority of the task */
                        &_weather_widget_sync_Task );   /* Task handle. */
    }
}

weather_config_t *weather_get_config( void ) {
    return( &weather_config );
}

void weather_widget_sync_Task( void * pvParameters ) {
    log_i("start weather widget task");

    vTaskDelay( 250 );

    if ( xEventGroupGetBits( weather_widget_event_handle ) & WEATHER_WIDGET_SYNC_REQUEST ) {       
        uint32_t retval = weather_fetch_today( &weather_config, &weather_today );
        if ( retval == 200 ) {
            lv_label_set_text( weather_widget_temperature_label, weather_today.temp );
            lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_RELEASED, resolve_owm_icon( weather_today.icon ) );
            lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_PRESSED, resolve_owm_icon( weather_today.icon ) );
            lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_CHECKED_RELEASED, resolve_owm_icon( weather_today.icon ) );
            lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_CHECKED_PRESSED, resolve_owm_icon( weather_today.icon ) );

            if ( weather_config.showWind ) {
                lv_label_set_text( weather_widget_wind_label, weather_today.wind );
                lv_obj_align( weather_widget_temperature_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, -22);
                lv_obj_align( weather_widget_wind_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
            }
            else {
                lv_label_set_text( weather_widget_wind_label, "" );
                lv_obj_align( weather_widget_temperature_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
                lv_obj_align( weather_widget_wind_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
            }

            lv_img_set_src( weather_widget_info_img, &info_ok_16px );
            lv_obj_set_hidden( weather_widget_info_img, false );
        }
        else {
            lv_img_set_src( weather_widget_info_img, &info_fail_16px );
            lv_obj_set_hidden( weather_widget_info_img, false );
        }
        lv_obj_invalidate( lv_scr_act() );
    }
    xEventGroupClearBits( weather_widget_event_handle, WEATHER_WIDGET_SYNC_REQUEST );
    vTaskDelete( NULL );
}

/*
 *
 */
void weather_save_config( void ) {
    if ( SPIFFS.exists( WEATHER_CONFIG_FILE ) ) {
        SPIFFS.remove( WEATHER_CONFIG_FILE );
        log_i("remove old binary weather config");
    }

    fs::File file = SPIFFS.open( WEATHER_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", WEATHER_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["apikey"] = weather_config.apikey;
        doc["lat"] = weather_config.lat;
        doc["lon"] = weather_config.lon;
        doc["autosync"] = weather_config.autosync;
        doc["showWind"] = weather_config.showWind;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

/*
 *
 */
void weather_load_config( void ) {
    if ( SPIFFS.exists( WEATHER_JSON_CONFIG_FILE ) ) {        
        fs::File file = SPIFFS.open( WEATHER_JSON_CONFIG_FILE, FILE_READ );
        if (!file) {
            log_e("Can't open file: %s!", WEATHER_JSON_CONFIG_FILE );
        }
        else {
            int filesize = file.size();
            SpiRamJsonDocument doc( filesize * 2 );

            DeserializationError error = deserializeJson( doc, file );
            if ( error ) {
                log_e("update check deserializeJson() failed: %s", error.c_str() );
            }
            else {
                strlcpy( weather_config.apikey, doc["apikey"], sizeof( weather_config.apikey ) );
                strlcpy( weather_config.lat, doc["lat"], sizeof( weather_config.lat ) );
                strlcpy( weather_config.lon, doc["lon"], sizeof( weather_config.lon ) );
                weather_config.autosync = doc["autosync"].as<bool>();
                weather_config.showWind = doc["showWind"].as<bool>();
            }        
            doc.clear();
        }
        file.close();
    }
    else {
        log_i("no json config exists, read from binary");
        fs::File file = SPIFFS.open( WEATHER_CONFIG_FILE, FILE_READ );

        if (!file) {
            log_e("Can't open file: %s!", WEATHER_CONFIG_FILE );
        }
        else {
            int filesize = file.size();
            if ( filesize > sizeof( weather_config ) ) {
                log_e("Failed to read configfile. Wrong filesize!" );
            }
            else {
                file.read( (uint8_t *)&weather_config, filesize );
                file.close();
                weather_save_config();
                return; 
            }
            file.close();
        }
    }
}

