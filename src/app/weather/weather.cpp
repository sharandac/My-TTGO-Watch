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

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "images/resolve_owm_icon.h"
#include "hardware/motor.h"

#include "weather.h"
#include "weather_fetch.h"
#include "weather_setup.h"

EventGroupHandle_t weather_widget_event_handle = NULL;
TaskHandle_t _weather_widget_sync_Task;
void weather_widget_sync_Task( void * pvParameters );

weather_config_t weather_config;
weather_forcast_t weather_today;

lv_tile_number weather_widget_tile_num = NO_TILE;
lv_tile_number weather_widget_setup_tile_num = NO_TILE;
lv_obj_t *weather_widget_cont = NULL;
lv_obj_t *weather_widget_condition_img = NULL;
lv_obj_t *weather_widget_temperature_label = NULL;
lv_obj_t *weather_widget_wind_label = NULL;

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
LV_IMG_DECLARE(owm_01d_64px);
LV_FONT_DECLARE(Ubuntu_16px);

void weather_app_setup( void ) {

    weather_load_config();

    // get a free widget tile and a widget setup tile
    weather_widget_tile_num = mainbar_get_next_free_tile( TILE_TYPE_WIDGET_TILE );
    weather_widget_setup_tile_num = mainbar_get_next_free_tile( TILE_TYPE_WIDGET_SETUP );
    // register the widget setup function
    mainbar_set_tile_setup_cb( weather_widget_tile_num, weather_tile_setup );
    mainbar_set_tile_setup_cb( weather_widget_setup_tile_num, weather_setup_tile_setup );

    // get an widget container from main_tile
    weather_widget_cont = main_tile_register_widget();

    // create widget weather condition icon and temperature label
    weather_widget_condition_img = lv_imgbtn_create( weather_widget_cont, NULL );
    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_RELEASED, &owm_01d_64px);
    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_PRESSED, &owm_01d_64px);
    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_CHECKED_RELEASED, &owm_01d_64px);
    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_CHECKED_PRESSED, &owm_01d_64px);
    lv_obj_reset_style_list( weather_widget_condition_img, LV_OBJ_PART_MAIN );
    lv_obj_align( weather_widget_condition_img , weather_widget_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( weather_widget_condition_img, enter_weather_widget_event_cb );

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
        weather_widget_sync_request();
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP );

    weather_widget_event_handle = xEventGroupCreate();
    xEventGroupClearBits( weather_widget_event_handle, WEATHER_WIDGET_SYNC_REQUEST );

    xTaskCreate(
                        weather_widget_sync_Task,      /* Function to implement the task */
                        "weather sync Task",    /* Name of the task */
                        5000,              /* Stack size in words */
                        NULL,               /* Task input parameter */
                        1,                  /* Priority of the task */
                        &_weather_widget_sync_Task );  /* Task handle. */  
}

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       motor_vibe( 1 );
                                        mainbar_jump_to_tilenumber( weather_widget_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

void weather_jump_to_forecast( void ) {
    mainbar_jump_to_tilenumber( weather_widget_tile_num, LV_ANIM_ON );
}

void weather_jump_to_setup( void ) {
    mainbar_jump_to_tilenumber( weather_widget_setup_tile_num, LV_ANIM_ON );    
}

void weather_widget_sync_request( void ) {
    if ( xEventGroupGetBits( weather_widget_event_handle ) & WEATHER_WIDGET_SYNC_REQUEST ) {
        return;
    }
    else {
        xEventGroupSetBits( weather_widget_event_handle, WEATHER_WIDGET_SYNC_REQUEST );
        vTaskResume( _weather_widget_sync_Task );    
    }
}

weather_config_t *weather_get_config( void ) {
    return( &weather_config );
}

void weather_widget_sync_Task( void * pvParameters ) {
    while( true ) {
        vTaskDelay( 500 );
        if ( xEventGroupGetBits( weather_widget_event_handle ) & WEATHER_WIDGET_SYNC_REQUEST ) {   
            if ( weather_config.autosync ) {
                uint32_t retval = weather_fetch_today( &weather_config, &weather_today );
                if ( retval == 200 ) {
                    lv_label_set_text( weather_widget_temperature_label, weather_today.temp );
                    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_RELEASED, resolve_owm_icon( weather_today.icon ) );
                    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_PRESSED, resolve_owm_icon( weather_today.icon ) );
                    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_CHECKED_RELEASED, resolve_owm_icon( weather_today.icon ) );
                    lv_imgbtn_set_src( weather_widget_condition_img, LV_BTN_STATE_CHECKED_PRESSED, resolve_owm_icon( weather_today.icon ) );

                    if ( weather_config.showWind )
                    {
                        lv_label_set_text( weather_widget_wind_label, weather_today.wind );
                        lv_obj_align( weather_widget_temperature_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, -22);
                        lv_obj_align( weather_widget_wind_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
                    }
                    else
                    {
                        lv_label_set_text( weather_widget_wind_label, "" );
                        lv_obj_align( weather_widget_temperature_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
                        lv_obj_align( weather_widget_wind_label, weather_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
                    }
                }
            }
            xEventGroupClearBits( weather_widget_event_handle, WEATHER_WIDGET_SYNC_REQUEST );
        }
        vTaskSuspend( _weather_widget_sync_Task );
    }
}

/*
 *
 */
void weather_save_config( void ) {

    fs::File file = SPIFFS.open( WEATHER_CONFIG_FILE, FILE_WRITE );

    if ( !file ) {
        Serial.printf( __FILE__ "Can't save file: %s\r\n", WEATHER_CONFIG_FILE );
    }
    else {
        file.write( (uint8_t *)&weather_config, sizeof( weather_config ) );
        file.close();
    }
}

/*
 *
 */
void weather_load_config( void ) {

    fs::File file = SPIFFS.open( WEATHER_CONFIG_FILE, FILE_READ );

    if (!file) {
        Serial.printf( __FILE__ "Can't open file: %s\r\n", WEATHER_CONFIG_FILE );
    }
    else {
        size_t filesize = file.size();

        // Special case, convert V1 of the file to a versioned V2 format
        if (filesize == sizeof(weather_config_t_v1))
        {
            log_i("Reading weather config v1");
            weather_config_t_v1 v1Config;
            weather_config_t_v2 &v2Config = weather_config; // For now, V2 is current.
            file.read((uint8_t *)&v1Config, filesize);
            log_i("Converting weather config v1 to v2");
            v2Config.version = 2;
            memcpy(&v2Config.apikey[0], &v1Config, sizeof(weather_config_t_v1));
            v2Config.autosync = v1Config.autosync;
            v2Config.showWind = false;
        }
        else if ( filesize > 0)
        {
            // Read version number, then verify and catch up as needed
            uint8_t version = 0;
            file.read(&version, 1);
            file.seek(0, fs::SeekSet);
            log_v("Reading weather config version: %d", version);

            if ( version > currentConfigVersion)
            {
                log_e( "Unexpected weather config version. Expected at most %d found %d", currentConfigVersion, version);
            }
            if ( version == 2 )
            {
                if (filesize != sizeof(weather_config_t_v2))
                {
                    log_e( "Failed to read weather config file. Wrong filesize! Expected %d found %d", sizeof(weather_config_t_v2), filesize);
                }
                else
                {
                    file.read((uint8_t *)&weather_config, filesize);
                }
            }
        }
        else
        {
            log_e( "Failed to read weather config file. File size is %d", filesize);
        }

        file.close();
    }
}

