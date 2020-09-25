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
#include "esp_task_wdt.h"

#include "weather.h"
#include "weather_fetch.h"
#include "weather_forecast.h"
#include "weather_setup.h"
#include "images/resolve_owm_icon.h"

#include "gui/app.h"
#include "gui/widget.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"

#include "hardware/json_psram_allocator.h"
#include "hardware/wifictl.h"

EventGroupHandle_t weather_widget_event_handle = NULL;
TaskHandle_t _weather_widget_sync_Task;
void weather_widget_sync_Task( void * pvParameters );

weather_config_t weather_config;
weather_forcast_t weather_today;

uint32_t weather_app_tile_num;
uint32_t weather_app_setup_tile_num;

icon_t *weather_app = NULL;
icon_t * weather_widget = NULL;

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
bool weather_widget_wifictl_event_cb( EventBits_t event, void *arg );

LV_IMG_DECLARE(owm_01d_64px);
LV_IMG_DECLARE(info_ok_16px);
LV_IMG_DECLARE(info_fail_16px);
LV_FONT_DECLARE(Ubuntu_16px);

void weather_app_setup( void ) {

    weather_load_config();

    // get an app tile and copy mainstyle
    weather_app_tile_num = mainbar_add_app_tile( 1, 2, "Weather App" );
    weather_app_setup_tile_num = weather_app_tile_num + 1;

    // init forecast and setup tile
    weather_forecast_tile_setup( weather_app_tile_num );
    weather_setup_tile_setup( weather_app_setup_tile_num );

    weather_app = app_register( "weather", &owm_01d_64px, enter_weather_widget_event_cb );    

    // register app and widget icon
    if ( weather_config.widget ) {
        weather_add_widget();
    }

    if( weather_config.showWind ) {
        widget_set_extended_label( weather_widget, "n/a" );
    }

    weather_widget_event_handle = xEventGroupCreate();

    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT, weather_widget_wifictl_event_cb, "weather" );
}

bool weather_widget_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_CONNECT:       if ( weather_config.autosync ) {
                                        weather_widget_sync_request();
                                    }
                                    break;
        case WIFICTL_OFF:           widget_hide_indicator( weather_widget );
                                    break;
    }
    return( true );
}

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( weather_app_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

void weather_add_widget( void ) {
    weather_widget = widget_register( "n/a", &owm_01d_64px, enter_weather_widget_event_cb );
}

void weather_remove_widget( void ) {
    weather_widget = widget_remove( weather_widget );
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
        widget_hide_indicator( weather_widget );
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
    log_i("start weather widget task, heap: %d", ESP.getFreeHeap() );

    vTaskDelay( 250 );

    if ( xEventGroupGetBits( weather_widget_event_handle ) & WEATHER_WIDGET_SYNC_REQUEST ) {       
        uint32_t retval = weather_fetch_today( &weather_config, &weather_today );
        if ( retval == 200 ) {
            widget_set_label( weather_widget, weather_today.temp );
            widget_set_icon( weather_widget, (lv_obj_t*)resolve_owm_icon( weather_today.icon ) );
            widget_set_indicator( weather_widget, ICON_INDICATOR_OK );

            if ( weather_config.showWind ) {
                widget_set_extended_label( weather_widget, weather_today.wind );
            }
            else {
                widget_set_extended_label( weather_widget, "" );
            }
        }
        else {
            widget_set_indicator( weather_widget, ICON_INDICATOR_FAIL );
        }
        lv_obj_invalidate( lv_scr_act() );
    }
    xEventGroupClearBits( weather_widget_event_handle, WEATHER_WIDGET_SYNC_REQUEST );
    log_i("finish weather widget task, heap: %d", ESP.getFreeHeap() );
    vTaskDelete( NULL );
}

void weather_save_config( void ) {
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
        doc["imperial"] = weather_config.imperial;
        doc["widget"] = weather_config.widget;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void weather_load_config( void ) {
    fs::File file = SPIFFS.open( WEATHER_JSON_CONFIG_FILE, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", WEATHER_JSON_CONFIG_FILE );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 4 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            strlcpy( weather_config.apikey, doc["apikey"], sizeof( weather_config.apikey ) );
            strlcpy( weather_config.lat, doc["lat"], sizeof( weather_config.lat ) );
            strlcpy( weather_config.lon, doc["lon"], sizeof( weather_config.lon ) );
            weather_config.autosync = doc["autosync"] | true;
            weather_config.showWind = doc["showWind"] | false;
            weather_config.imperial = doc["imperial"] | false;
            weather_config.widget = doc["widget"] | true;
        }        
        doc.clear();
    }
    file.close();
}

