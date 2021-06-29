/****************************************************************************
 *   June 16 23:15:00 2021
 *   Copyright  2021  Dirk Sarodnick
 *   Email: programmer@dirk-sarodnick.de
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
#include "HTTPClient.h"
#include <TTGO.h>

#include "weather_station_app.h"
#include "weather_station_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/wifictl.h"
#include "utils/json_psram_allocator.h"

bool weather_station_state = false;
static uint64_t nextmillis = 0;

lv_obj_t *weather_station_app_main_tile = NULL;
lv_style_t weather_station_app_main_style;
lv_style_t weather_station_header_style;
lv_style_t weather_station_title_style;
lv_style_t weather_station_label_style;

lv_obj_t *weather_station_temp_low_label = NULL;
lv_obj_t *weather_station_temp_high_label = NULL;
lv_obj_t *weather_station_temp_house_label = NULL;
lv_obj_t *weather_station_humidity_label = NULL;
lv_obj_t *weather_station_garden_bed_temp_label = NULL;
lv_obj_t *weather_station_garden_bed_soil_label = NULL;
lv_obj_t *weather_station_garden_house_temp_label = NULL;
lv_obj_t *weather_station_garden_house_soil_label = NULL;
lv_obj_t *weather_station_tank1_label = NULL;
lv_obj_t *weather_station_tank2_label = NULL;
lv_obj_t *weather_station_wind_label = NULL;
lv_obj_t *weather_station_rain_label = NULL;

lv_task_t * _weather_station_app_task;

LV_FONT_DECLARE(Ubuntu_12px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_IMG_DECLARE(refresh_32px);

static void exit_weather_station_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_weather_station_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void refresh_weather_station_event_cb( lv_obj_t * obj, lv_event_t event );
static bool weather_station_main_wifictl_event_cb( EventBits_t event, void *arg );
void weather_station_app_task( lv_task_t * task );
void weather_station_refresh();

void weather_station_app_main_setup( uint32_t tile_num ) {

    weather_station_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &weather_station_app_main_style, ws_get_mainbar_style() );

    lv_obj_t * exit_btn = wf_add_exit_button( weather_station_app_main_tile, exit_weather_station_app_main_event_cb, &weather_station_app_main_style );
    lv_obj_align(exit_btn, weather_station_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    lv_obj_t * setup_btn = wf_add_setup_button( weather_station_app_main_tile, enter_weather_station_app_setup_event_cb, &weather_station_app_main_style );
    lv_obj_align(setup_btn, weather_station_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );

    lv_obj_t * reload_btn = wf_add_image_button( weather_station_app_main_tile, refresh_32px, refresh_weather_station_event_cb, &weather_station_app_main_style );
    lv_obj_align(reload_btn, weather_station_app_main_tile, LV_ALIGN_IN_TOP_RIGHT, -10 , 10 );

    lv_style_copy(&weather_station_title_style, ws_get_label_style());
    lv_style_set_text_font(&weather_station_title_style, LV_STATE_DEFAULT, &Ubuntu_12px);

    lv_style_copy(&weather_station_label_style, ws_get_button_style());
    lv_style_set_text_font(&weather_station_label_style, LV_STATE_DEFAULT, &Ubuntu_12px);
	lv_style_set_pad_top(&weather_station_label_style, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&weather_station_label_style, LV_STATE_DEFAULT, 6);
	lv_style_set_pad_right(&weather_station_label_style, LV_STATE_DEFAULT, 6);
    lv_style_set_bg_color( &weather_station_label_style, LV_STATE_DEFAULT, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &weather_station_label_style, LV_STATE_DEFAULT, LV_OPA_50 );

    lv_style_copy(&weather_station_header_style, ws_get_label_style());
    lv_style_set_text_font(&weather_station_header_style, LV_STATE_DEFAULT, &Ubuntu_16px);

    lv_obj_t * label_header = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(label_header, LV_OBJ_PART_MAIN, &weather_station_header_style);
	lv_label_set_text(label_header, "Weather Station");
	lv_obj_set_pos(label_header, 10, 10);
	lv_obj_set_size(label_header, 60, 40);

    lv_obj_t * label_row1 = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(label_row1, LV_OBJ_PART_MAIN, &weather_station_title_style);
	lv_label_set_text(label_row1, "Air Temp.:");
	lv_label_set_long_mode(label_row1, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label_row1, 10, 50);
	lv_obj_set_size(label_row1, 60, 20);

    weather_station_temp_low_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_temp_low_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_temp_low_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_temp_low_label, "");
	lv_obj_set_pos(weather_station_temp_low_label, 80, 50);
	lv_obj_set_size(weather_station_temp_low_label, 75, 20);

    weather_station_temp_high_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_temp_high_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_temp_high_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_temp_high_label, "");
	lv_obj_set_pos(weather_station_temp_high_label, 160, 50);
	lv_obj_set_size(weather_station_temp_high_label, 75, 20);

    lv_obj_t * label_row2 = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(label_row2, LV_OBJ_PART_MAIN, &weather_station_title_style);
	lv_label_set_text(label_row2, "Shed / Humidity:");
	lv_label_set_long_mode(label_row2, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label_row2, 10, 75);
	lv_obj_set_size(label_row2, 60, 20);

    weather_station_temp_house_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_temp_house_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_temp_house_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_temp_house_label, "");
	lv_obj_set_pos(weather_station_temp_house_label, 80, 75);
	lv_obj_set_size(weather_station_temp_house_label, 75, 20);

    weather_station_humidity_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_humidity_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_humidity_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_humidity_label, "");
	lv_obj_set_pos(weather_station_humidity_label, 160, 75);
	lv_obj_set_size(weather_station_humidity_label, 75, 20);

    lv_obj_t * label_row3 = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(label_row3, LV_OBJ_PART_MAIN, &weather_station_title_style);
	lv_label_set_text(label_row3, "Garden Bed:");
	lv_label_set_long_mode(label_row3, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label_row3, 10, 100);
	lv_obj_set_size(label_row3, 60, 20);

    weather_station_garden_bed_temp_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_garden_bed_temp_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_garden_bed_temp_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_garden_bed_temp_label, "");
	lv_obj_set_pos(weather_station_garden_bed_temp_label, 80, 100);
	lv_obj_set_size(weather_station_garden_bed_temp_label, 75, 20);

    weather_station_garden_bed_soil_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_garden_bed_soil_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_garden_bed_soil_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_garden_bed_soil_label, "");
	lv_obj_set_pos(weather_station_garden_bed_soil_label, 160, 100);
	lv_obj_set_size(weather_station_garden_bed_soil_label, 75, 20);

    lv_obj_t * label_row4 = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(label_row4, LV_OBJ_PART_MAIN, &weather_station_title_style);
	lv_label_set_text(label_row4, "Garden House:");
	lv_label_set_long_mode(label_row4, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label_row4, 10, 125);
	lv_obj_set_size(label_row4, 60, 20);

    weather_station_garden_house_temp_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_garden_house_temp_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_garden_house_temp_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_garden_house_temp_label, "");
	lv_obj_set_pos(weather_station_garden_house_temp_label, 80, 125);
	lv_obj_set_size(weather_station_garden_house_temp_label, 75, 20);

    weather_station_garden_house_soil_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_garden_house_soil_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_garden_house_soil_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_garden_house_soil_label, "");
	lv_obj_set_pos(weather_station_garden_house_soil_label, 160, 125);
	lv_obj_set_size(weather_station_garden_house_soil_label, 75, 20);

    lv_obj_t * label_row5 = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(label_row5, LV_OBJ_PART_MAIN, &weather_station_title_style);
	lv_label_set_text(label_row5, "Wind / Rain:");
	lv_label_set_long_mode(label_row5, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label_row5, 10, 150);
	lv_obj_set_size(label_row5, 60, 20);

    weather_station_wind_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_wind_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_wind_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_wind_label, "");
	lv_obj_set_pos(weather_station_wind_label, 80, 150);
	lv_obj_set_size(weather_station_wind_label, 75, 20);

    weather_station_rain_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_rain_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_rain_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_rain_label, "");
	lv_obj_set_pos(weather_station_rain_label, 160, 150);
	lv_obj_set_size(weather_station_rain_label, 75, 20);

    lv_obj_t * label_row6 = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(label_row6, LV_OBJ_PART_MAIN, &weather_station_title_style);
	lv_label_set_text(label_row6, "Tank Level:");
	lv_label_set_long_mode(label_row6, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label_row6, 10, 175);
	lv_obj_set_size(label_row6, 60, 20);

    weather_station_tank1_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_tank1_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_tank1_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_tank1_label, "");
	lv_obj_set_pos(weather_station_tank1_label, 80, 175);
	lv_obj_set_size(weather_station_tank1_label, 75, 20);

    weather_station_tank2_label = lv_label_create(weather_station_app_main_tile, NULL);
	lv_obj_add_style(weather_station_tank2_label, LV_OBJ_PART_MAIN, &weather_station_label_style);
	lv_label_set_long_mode(weather_station_tank2_label, LV_LABEL_LONG_SROLL_CIRC);
	lv_label_set_text(weather_station_tank2_label, "");
	lv_obj_set_pos(weather_station_tank2_label, 160, 175);
	lv_obj_set_size(weather_station_tank2_label, 75, 20);

    // callbacks
    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT, weather_station_main_wifictl_event_cb, "weather station main" );

    // create an task that runs every second
    _weather_station_app_task = lv_task_create( weather_station_app_task, 1000, LV_TASK_PRIO_MID, NULL );
}

static bool weather_station_main_wifictl_event_cb( EventBits_t event, void *arg ) {    
    switch( event ) {
        case WIFICTL_CONNECT:       weather_station_state = true;
                                    weather_station_app_hide_indicator();
                                    break;
        case WIFICTL_DISCONNECT:    weather_station_state = false;
                                    weather_station_app_set_indicator( ICON_INDICATOR_FAIL );
                                    break;
        case WIFICTL_OFF:           weather_station_state = false;
                                    weather_station_app_hide_indicator();
                                    break;
    }
    return( true );
}

static void enter_weather_station_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( weather_station_app_get_app_setup_tile_num(), LV_ANIM_ON );
                                        nextmillis = 0;
                                        break;
    }
}

static void exit_weather_station_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}

static void refresh_weather_station_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       weather_station_refresh();
                                        break;
    }
}

void weather_station_app_task( lv_task_t * task ) {
    if (!weather_station_state) return;

    if ( nextmillis < millis() ) {
        nextmillis = millis() + 300000L;

        weather_station_refresh();
    }
}

void weather_station_refresh() {
    if (!weather_station_state) return;
    
    weather_station_config_t *weather_station_config = weather_station_get_config();

    HTTPClient http_client;
    http_client.useHTTP10( true );
    http_client.begin( weather_station_config->url );
    int httpcode = http_client.GET();

    if (httpcode >= 200 && httpcode < 300) {
        SpiRamJsonDocument doc( 1000 );

        DeserializationError error = deserializeJson( doc, http_client.getStream() );
        if (error) {
            log_e("weather station deserializeJson() failed: %s", error.c_str() );
            http_client.end();
            return;
        }

        char val[32];

        snprintf( val, sizeof(val), "%.1f °C", doc["Temp1"].as<float>() );
        lv_label_set_text(weather_station_temp_high_label, val);

        snprintf( val, sizeof(val), "%.1f °C", doc["Temp2"].as<float>() );
        lv_label_set_text(weather_station_temp_low_label, val);
        
        snprintf( val, sizeof(val), "%.1f °C", doc["ShedTemp"].as<float>() );
        lv_label_set_text(weather_station_temp_house_label, val);
        
        snprintf( val, sizeof(val), "%.1f %%", doc["Humidity"].as<float>() );
        lv_label_set_text(weather_station_humidity_label, val);
        
        snprintf( val, sizeof(val), "%.1f °C", doc["GroundTemp"].as<float>() );
        lv_label_set_text(weather_station_garden_bed_temp_label, val);
        
        snprintf( val, sizeof(val), "%.1f °C", doc["HouseTemp"].as<float>() );
        lv_label_set_text(weather_station_garden_house_temp_label, val);
        
        snprintf( val, sizeof(val), "%.1f %%", doc["SoilMoisture"].as<float>() );
        lv_label_set_text(weather_station_garden_bed_soil_label, val);
        
        snprintf( val, sizeof(val), "%.1f %%", doc["SoilMoisture2"].as<float>() );
        lv_label_set_text(weather_station_garden_house_soil_label, val);

        snprintf( val, sizeof(val), "%.1f m/s", doc["WindSpeed"].as<float>() );
        lv_label_set_text(weather_station_wind_label, val);
        
        snprintf( val, sizeof(val), "%.1f mm/3h", doc["Precipitation"].as<float>() );
        lv_label_set_text(weather_station_rain_label, val);

        snprintf( val, sizeof(val), "%.1f %%", doc["Tank"].as<float>() );
        lv_label_set_text(weather_station_tank1_label, val);
        
        snprintf( val, sizeof(val), "%.1f %%", doc["Tank2"].as<float>() );
        lv_label_set_text(weather_station_tank2_label, val);

        doc.clear();
    } else {
        log_e("weather station got http status code %d", httpcode);
    }

    http_client.end();
}