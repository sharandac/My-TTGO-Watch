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
#include "weather_setup.h"
#include "images/resolve_owm_icon.h"
#include "gui/app.h"
#include "gui/widget.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "hardware/wifictl.h"
#include "utils/json_psram_allocator.h"

#ifdef NATIVE_64BIT
    #include <iostream>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include "utils/logging.h"

    using namespace std;
    #define String string
#else
    #include <Arduino.h>

    EventGroupHandle_t weather_sync_event_handle = NULL;
    TaskHandle_t _weather_sync_Task;
#endif

void weather_widget_sync( void );
void weather_sync_Task( void * pvParameters );

weather_config_t weather_config;
weather_forcast_t weather_today;

uint32_t weather_app_tile_num;
uint32_t weather_app_setup_tile_num;

icon_t *weather_app = NULL;
icon_t * weather_widget = NULL;

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
bool weather_wifictl_event_cb( EventBits_t event, void *arg );

LV_IMG_DECLARE(owm01d_64px);
LV_IMG_DECLARE(info_ok_16px);
LV_IMG_DECLARE(info_fail_16px);
LV_FONT_DECLARE(Ubuntu_16px);

void weather_app_setup( void ) {

    weather_config.load();

    // get an app tile and copy mainstyle
    weather_app_tile_num = mainbar_add_app_tile( 1, 2, "Weather App" );
    weather_app_setup_tile_num = weather_app_tile_num + 1;

    // init forecast and setup tile
    weather_forecast_tile_setup( weather_app_tile_num );
    weather_setup_tile_setup( weather_app_setup_tile_num );

    weather_app = app_register( "weather", &owm01d_64px, enter_weather_widget_event_cb );    

    // register app and widget icon
    if ( weather_config.widget ) {
        weather_add_widget();
    }

    if( weather_config.showWind ) {
        widget_set_extended_label( weather_widget, "n/a" );
    }

#ifdef NATIVE_64BIT

#else
    weather_sync_event_handle = xEventGroupCreate();
#endif

    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT, weather_wifictl_event_cb, "weather" );
}

bool weather_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_CONNECT:       if ( weather_config.autosync ) {
                                        weather_sync_request();
                                    }
                                    break;
        case WIFICTL_OFF:           widget_hide_indicator( weather_widget );
                                    break;
    }
    return( true );
}

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( weather_app_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        break;
    }    
}

void weather_add_widget( void ) {
    weather_widget = widget_register( "n/a", &owm01d_64px, enter_weather_widget_event_cb );
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

void weather_sync_request( void ) {
#ifdef NATIVE_64BIT
    weather_sync_Task( NULL );
#else
    if ( xEventGroupGetBits( weather_sync_event_handle ) & WEATHER_SYNC_REQUEST ) {
        return;
    }
    else {
        xEventGroupSetBits( weather_sync_event_handle, WEATHER_SYNC_REQUEST );
        widget_hide_indicator( weather_widget );
        xTaskCreate(    weather_sync_Task,              /* Function to implement the task */
                        "weather sync Task",            /* Name of the task */
                        5000,                           /* Stack size in words */
                        NULL,                           /* Task input parameter */
                        1,                              /* Priority of the task */
                        &_weather_sync_Task );          /* Task handle. */
    }
#endif
}

weather_config_t *weather_get_config( void ) {
    return( &weather_config );
}

void weather_sync_Task( void * pvParameters ) {
#ifndef NATIVE_64BIT
    log_i("start weather widget task, heap: %d", ESP.getFreeHeap() );
    vTaskDelay( 250 );
    if ( xEventGroupGetBits( weather_sync_event_handle ) & WEATHER_SYNC_REQUEST ) {       
#endif

    weather_widget_sync();
    weather_forecast_sync();

#ifndef NATIVE_64BIT
    }
    xEventGroupClearBits( weather_sync_event_handle, WEATHER_SYNC_REQUEST );
    log_i("finish weather widget task, heap: %d", ESP.getFreeHeap() );
    vTaskDelete( NULL );
#endif
}

void weather_widget_sync( void ) {
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

void weather_save_config( void ) {
    weather_config.save();
}

void weather_load_config( void ) {
    weather_config.load();
}

