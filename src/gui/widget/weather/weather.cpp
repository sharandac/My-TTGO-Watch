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
#include "weather.h"
#include "weather_fetch.h"
#include <WiFi.h>

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "images/resolve_owm_icon.h"

weather_config_t weather_config;
weather_forcast_t weather_today;
weather_forcast_t weather_forecast[ WEATHER_MAX_FORECAST ];

lv_tile_number weather_widget_tile_num = NO_TILE;
lv_tile_number weather_widget_setup_tile_num = NO_TILE;
lv_obj_t *widget_cont = NULL;
lv_obj_t *widget_weather_condition_img = NULL;
lv_obj_t *widget_weather_temperature_label = NULL;

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
LV_IMG_DECLARE(owm_01d_64px);
LV_FONT_DECLARE(Ubuntu_16px);

void weather_widget_setup( void ) {

    weather_load_config();

    // get a free widget tile and a widget setup tile
    weather_widget_tile_num = mainbar_get_next_free_tile( TILE_TYPE_WIDGET_TILE );
    weather_widget_setup_tile_num = mainbar_get_next_free_tile( TILE_TYPE_WIDGET_SETUP );
    // register the widget setup function
    mainbar_set_tile_setup_cb( weather_widget_tile_num, weather_widget_tile_setup );
    mainbar_set_tile_setup_cb( weather_widget_setup_tile_num, weather_widget_setup_tile_setup );

    // get an widget container from main_tile
    widget_cont = main_tile_register_widget();

    // create widget weather condition icon and temperature label
    widget_weather_condition_img = lv_imgbtn_create( widget_cont, NULL );
    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_RELEASED, &owm_01d_64px);
    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_PRESSED, &owm_01d_64px);
    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_CHECKED_RELEASED, &owm_01d_64px);
    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_CHECKED_PRESSED, &owm_01d_64px);
    lv_obj_reset_style_list( widget_weather_condition_img, LV_OBJ_PART_MAIN );
    lv_obj_align( widget_weather_condition_img , widget_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( widget_weather_condition_img, enter_weather_widget_event_cb );

    widget_weather_temperature_label = lv_label_create( widget_cont , NULL);
    lv_label_set_text(widget_weather_temperature_label, "n/a");
    lv_obj_reset_style_list( widget_weather_temperature_label, LV_OBJ_PART_MAIN );
    lv_obj_align(widget_weather_temperature_label, widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);    
}

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( weather_widget_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

/*
 *
 */
EventGroupHandle_t weather_event_handle = NULL;
TaskHandle_t _weather_sync_Task;
void weather_sync_Task( void * pvParameters );

lv_obj_t *weather_widget_setup_tile = NULL;
lv_obj_t *weather_apikey_textfield = NULL;
lv_obj_t *weather_lat_textfield = NULL;
lv_obj_t *weather_lon_textfield = NULL;
lv_obj_t *weather_autosync_onoff = NULL;
lv_style_t weather_widget_setup_style;

LV_IMG_DECLARE(exit_32px);

static void weather_apikey_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_weather_widget_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void weather_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event );

void weather_widget_setup_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {
    lv_style_init( &weather_widget_setup_style );
    lv_style_set_radius( &weather_widget_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &weather_widget_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    weather_widget_setup_tile = lv_obj_create( tile, NULL);
    lv_obj_set_size( weather_widget_setup_tile, hres , vres);
    lv_obj_align( weather_widget_setup_tile, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style( weather_widget_setup_tile, LV_OBJ_PART_MAIN, &weather_widget_setup_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( weather_widget_setup_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( exit_btn, weather_widget_setup_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_weather_widget_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( weather_widget_setup_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( exit_label, "open weather setup");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *weather_apikey_cont = lv_obj_create( weather_widget_setup_tile, NULL );
    lv_obj_set_size(weather_apikey_cont, hres , 40);
    lv_obj_add_style( weather_apikey_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( weather_apikey_cont, weather_widget_setup_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    lv_obj_t *weather_apikey_label = lv_label_create( weather_apikey_cont, NULL);
    lv_obj_add_style( weather_apikey_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( weather_apikey_label, "appid");
    lv_obj_align( weather_apikey_label, weather_apikey_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    weather_apikey_textfield = lv_textarea_create( weather_apikey_cont, NULL);
    lv_textarea_set_text( weather_apikey_textfield, weather_config.apikey );
    lv_textarea_set_pwd_mode( weather_apikey_textfield, false);
    lv_textarea_set_one_line( weather_apikey_textfield, true);
    lv_textarea_set_cursor_hidden( weather_apikey_textfield, true);
    lv_obj_set_width( weather_apikey_textfield, LV_HOR_RES /4 * 3 );
    lv_obj_align( weather_apikey_textfield, weather_apikey_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_apikey_textfield, weather_apikey_event_cb );

    lv_obj_t *weather_lat_cont = lv_obj_create( weather_widget_setup_tile, NULL );
    lv_obj_set_size(weather_lat_cont, hres / 2 , 40 );
    lv_obj_add_style( weather_lat_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( weather_lat_cont, weather_apikey_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );
    lv_obj_t *weather_lat_label = lv_label_create( weather_lat_cont, NULL);
    lv_obj_add_style( weather_lat_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( weather_lat_label, "lat");
    lv_obj_align( weather_lat_label, weather_lat_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    weather_lat_textfield = lv_textarea_create( weather_lat_cont, NULL);
    lv_textarea_set_text( weather_lat_textfield, weather_config.lat );
    lv_textarea_set_pwd_mode( weather_lat_textfield, false);
    lv_textarea_set_one_line( weather_lat_textfield, true);
    lv_textarea_set_cursor_hidden( weather_lat_textfield, true);
    lv_obj_set_width( weather_lat_textfield, LV_HOR_RES / 4 );
    lv_obj_align( weather_lat_textfield, weather_lat_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_lat_textfield, weather_apikey_event_cb );

    lv_obj_t *weather_lon_cont = lv_obj_create( weather_widget_setup_tile, NULL );
    lv_obj_set_size(weather_lon_cont, hres / 2 , 40 );
    lv_obj_add_style( weather_lon_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( weather_lon_cont, weather_apikey_cont, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0 );
    lv_obj_t *weather_lon_label = lv_label_create( weather_lon_cont, NULL);
    lv_obj_add_style( weather_lon_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( weather_lon_label, "lon");
    lv_obj_align( weather_lon_label, weather_lon_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    weather_lon_textfield = lv_textarea_create( weather_lon_cont, NULL);
    lv_textarea_set_text( weather_lon_textfield, weather_config.lon );
    lv_textarea_set_pwd_mode( weather_lon_textfield, false);
    lv_textarea_set_one_line( weather_lon_textfield, true);
    lv_textarea_set_cursor_hidden( weather_lon_textfield, true);
    lv_obj_set_width( weather_lon_textfield, LV_HOR_RES / 4 );
    lv_obj_align( weather_lon_textfield, weather_lon_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_lon_textfield, weather_apikey_event_cb );

    lv_obj_t *weather_autosync_cont = lv_obj_create( weather_widget_setup_tile, NULL );
    lv_obj_set_size( weather_autosync_cont, hres , 40);
    lv_obj_add_style( weather_autosync_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( weather_autosync_cont, weather_lat_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );
    weather_autosync_onoff = lv_switch_create( weather_autosync_cont, NULL );
    lv_switch_off( weather_autosync_onoff, LV_ANIM_ON );
    lv_obj_align( weather_autosync_onoff, weather_autosync_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_autosync_onoff, weather_autosync_onoff_event_handler );
    lv_obj_t *weather_autosync_label = lv_label_create( weather_autosync_cont, NULL);
    lv_obj_add_style( weather_autosync_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( weather_autosync_label, "sync if wifi connected");
    lv_obj_align( weather_autosync_label, weather_autosync_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    if ( weather_config.autosync )
        lv_switch_on( weather_autosync_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( weather_autosync_onoff, LV_ANIM_OFF );

    WiFi.onEvent( [](WiFiEvent_t event, WiFiEventInfo_t info) {
        xEventGroupSetBits( weather_event_handle, WEATHER_SYNC_REQUEST );
        vTaskResume( _weather_sync_Task );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP );

    weather_event_handle = xEventGroupCreate();
    xEventGroupClearBits( weather_event_handle, WEATHER_SYNC_REQUEST );

    xTaskCreate(
                        weather_sync_Task,      /* Function to implement the task */
                        "weather sync Task",    /* Name of the task */
                        10000,              /* Stack size in words */
                        NULL,               /* Task input parameter */
                        1,                  /* Priority of the task */
                        &_weather_sync_Task );  /* Task handle. */
}

static void weather_apikey_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
    else if ( event == LV_EVENT_DEFOCUSED ) {
        keyboard_hide();
    }
}

static void weather_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    if(event == LV_EVENT_VALUE_CHANGED) {
        if( lv_switch_get_state( obj ) ) {
            weather_config.autosync = true;
        }
        else {
            weather_config.autosync = false;
        }
    }
}

static void exit_weather_widget_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       keyboard_hide();
                                        weather_save_config();
                                        mainbar_jump_to_tilenumber( weather_widget_tile_num, LV_ANIM_ON );
                                        break;
    }
}

void weather_sync_Task( void * pvParameters ) {

    while( true ) {
        vTaskDelay( 250 );
        if ( xEventGroupGetBits( weather_event_handle ) & WEATHER_SYNC_REQUEST ) {   
            if ( weather_config.autosync ) {
                weather_fetch_today( &weather_config, &weather_today );
                if ( weather_today.valide ) {
                    Serial.printf("weather fetch ok\r\n");
                    lv_label_set_text( widget_weather_temperature_label, weather_today.temp );
                    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_RELEASED, resolve_owm_icon( weather_today.icon ) );
                    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_PRESSED, resolve_owm_icon( weather_today.icon ) );
                    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_CHECKED_RELEASED, resolve_owm_icon( weather_today.icon ) );
                    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_CHECKED_PRESSED, resolve_owm_icon( weather_today.icon ) );
                    lv_obj_align( widget_weather_temperature_label, widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
                }            
            }
            xEventGroupClearBits( weather_event_handle, WEATHER_SYNC_REQUEST );
        }
        vTaskSuspend( _weather_sync_Task );
    }
}

/*
 *
 */
lv_obj_t *weather_widget_tile = NULL;
lv_style_t weather_widget_style;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);

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

}

static void exit_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( MAIN_TILE, LV_ANIM_OFF );
                                        break;
    }
}

static void setup_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( weather_widget_setup_tile_num, LV_ANIM_ON );
                                        break;
    }
}

static void refresh_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       xEventGroupSetBits( weather_event_handle, WEATHER_SYNC_REQUEST );
                                        vTaskResume( _weather_sync_Task );
                                        break;
    }
}
/*
 *
 */
void weather_save_config( void ) {

    strcpy( weather_config.apikey, lv_textarea_get_text( weather_apikey_textfield ) );
    strcpy( weather_config.lat, lv_textarea_get_text( weather_lat_textfield ) );
    strcpy( weather_config.lon, lv_textarea_get_text( weather_lon_textfield ) );

    fs::File file = SPIFFS.open( WEATHER_CONFIG_FILE, FILE_WRITE );

    if ( !file ) {
        Serial.printf("Can't save file: %s\r\n", WEATHER_CONFIG_FILE );
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
        Serial.printf("Can't open file: %s\r\n", WEATHER_CONFIG_FILE );
    }
    else {
        int filesize = file.size();
        if ( filesize > sizeof( weather_config ) ) {
            Serial.printf("Failed to read configfile. Wrong filesize!\r\n" );
        }
        else {
            file.read( (uint8_t *)&weather_config, filesize );
        }
        file.close();
    }
}