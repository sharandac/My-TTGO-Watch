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
#include "weather.h"
#include "weather_fetch.h"
#include "weather_setup.h"
#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"
#include "hardware/blectl.h"
#include "hardware/motor.h"
#include "hardware/gpsctl.h"
#include "utils/bluejsonrequest.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
#endif

lv_obj_t *weather_setup_tile = NULL;
uint32_t weather_setup_tile_num;

lv_obj_t *weather_geolocation_onoff = NULL;
lv_obj_t *weather_apikey_textfield = NULL;
lv_obj_t *weather_lat_textfield = NULL;
lv_obj_t *weather_lon_textfield = NULL;
lv_obj_t *weather_autosync_onoff = NULL;
lv_obj_t *weather_wind_onoff = NULL;
lv_obj_t *weather_imperial_onoff = NULL;
lv_obj_t *weather_widget_onoff = NULL;
lv_style_t weather_widget_setup_style;

static void weather_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void weather_num_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_weather_widget_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void weather_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event );
static void weather_wind_onoff_event_handler( lv_obj_t *obj, lv_event_t event );
static void weather_imperial_onoff_event_handler( lv_obj_t *obj, lv_event_t event );
static void weather_widget_onoff_event_handler(lv_obj_t *obj, lv_event_t event);

bool weather_gpsctl_app_use_location_event_cb( EventBits_t event, void *arg );
bool weather_bluetooth_message_event_cb( EventBits_t event, void *arg );
static void weather_bluetooth_message_msg_pharse( BluetoothJsonRequest &doc );

void weather_setup_tile_setup( uint32_t tile_num ) {

    weather_config_t *weather_config = weather_get_config();

    weather_setup_tile_num = tile_num;
    weather_setup_tile = mainbar_get_tile_obj( weather_setup_tile_num );

    lv_obj_add_style( weather_setup_tile, LV_OBJ_PART_MAIN, SETUP_STYLE );

    lv_obj_t *header = wf_add_settings_header( weather_setup_tile, "open weather setup", exit_weather_widget_setup_event_cb );
    lv_obj_align( header, weather_setup_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );

    lv_obj_t *weather_apikey_cont = lv_obj_create( weather_setup_tile, NULL );
    lv_obj_set_size(weather_apikey_cont, lv_disp_get_hor_res( NULL ) , 35);
    lv_obj_add_style( weather_apikey_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( weather_apikey_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *weather_apikey_label = lv_label_create( weather_apikey_cont, NULL);
    lv_obj_add_style( weather_apikey_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( weather_apikey_label, "appid");
    lv_obj_align( weather_apikey_label, weather_apikey_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    weather_apikey_textfield = lv_textarea_create( weather_apikey_cont, NULL);
    lv_textarea_set_text( weather_apikey_textfield, weather_config->apikey );
    lv_textarea_set_pwd_mode( weather_apikey_textfield, false);
    lv_textarea_set_one_line( weather_apikey_textfield, true);
    lv_textarea_set_cursor_hidden( weather_apikey_textfield, true);
    lv_obj_set_width( weather_apikey_textfield, LV_HOR_RES /4 * 3 );
    lv_obj_align( weather_apikey_textfield, weather_apikey_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_apikey_textfield, weather_textarea_event_cb );

    lv_obj_t *weather_lat_cont = lv_obj_create( weather_setup_tile, NULL );
    lv_obj_set_size(weather_lat_cont, lv_disp_get_hor_res( NULL ) / 2 , 35 );
    lv_obj_add_style( weather_lat_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( weather_lat_cont, weather_apikey_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );
    lv_obj_t *weather_lat_label = lv_label_create( weather_lat_cont, NULL);
    lv_obj_add_style( weather_lat_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( weather_lat_label, "lat");
    lv_obj_align( weather_lat_label, weather_lat_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    weather_lat_textfield = lv_textarea_create( weather_lat_cont, NULL);
    lv_textarea_set_text( weather_lat_textfield, weather_config->lat );
    lv_textarea_set_pwd_mode( weather_lat_textfield, false);
    lv_textarea_set_accepted_chars( weather_lat_textfield, "-.0123456789.");
    lv_textarea_set_one_line( weather_lat_textfield, true);
    lv_textarea_set_cursor_hidden( weather_lat_textfield, true);
    lv_obj_set_width( weather_lat_textfield, lv_disp_get_hor_res( NULL ) / 4 );
    lv_obj_align( weather_lat_textfield, weather_lat_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_lat_textfield, weather_num_textarea_event_cb );

    lv_obj_t *weather_lon_cont = lv_obj_create( weather_setup_tile, NULL );
    lv_obj_set_size(weather_lon_cont, lv_disp_get_hor_res( NULL ) / 2 , 35 );
    lv_obj_add_style( weather_lon_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( weather_lon_cont, weather_apikey_cont, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0 );
    lv_obj_t *weather_lon_label = lv_label_create( weather_lon_cont, NULL);
    lv_obj_add_style( weather_lon_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( weather_lon_label, "lon");
    lv_obj_align( weather_lon_label, weather_lon_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    weather_lon_textfield = lv_textarea_create( weather_lon_cont, NULL);
    lv_textarea_set_text( weather_lon_textfield, weather_config->lon );
    lv_textarea_set_pwd_mode( weather_lon_textfield, false);
    lv_textarea_set_accepted_chars( weather_lon_textfield, "-.0123456789.");
    lv_textarea_set_one_line( weather_lon_textfield, true);
    lv_textarea_set_cursor_hidden( weather_lon_textfield, true);
    lv_obj_set_width( weather_lon_textfield, lv_disp_get_hor_res( NULL ) / 4 );
    lv_obj_align( weather_lon_textfield, weather_lon_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_lon_textfield, weather_num_textarea_event_cb );

    lv_obj_t *weather_autosync_cont = lv_obj_create( weather_setup_tile, NULL );
    lv_obj_set_size( weather_autosync_cont, lv_disp_get_hor_res( NULL ) , 30);
    lv_obj_add_style( weather_autosync_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( weather_autosync_cont, weather_lat_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5 );
    weather_autosync_onoff = wf_add_switch( weather_autosync_cont, false );
    lv_obj_align( weather_autosync_onoff, weather_autosync_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( weather_autosync_onoff, weather_autosync_onoff_event_handler );
    lv_obj_t *weather_autosync_label = lv_label_create( weather_autosync_cont, NULL);
    lv_obj_add_style( weather_autosync_label, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_label_set_text( weather_autosync_label, "Sync if wifi connected");
    lv_obj_align( weather_autosync_label, weather_autosync_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *weather_wind_cont = lv_obj_create( weather_setup_tile, NULL);
    lv_obj_set_size( weather_wind_cont, lv_disp_get_hor_res( NULL ), 30);
    lv_obj_add_style( weather_wind_cont, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_obj_align( weather_wind_cont, weather_autosync_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    weather_wind_onoff = wf_add_switch( weather_wind_cont, false );
    lv_obj_align( weather_wind_onoff, weather_wind_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_set_event_cb( weather_wind_onoff, weather_wind_onoff_event_handler);
    lv_obj_t *weather_wind_label = lv_label_create(weather_wind_cont, NULL);
    lv_obj_add_style( weather_wind_label, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_label_set_text( weather_wind_label, "Display wind");
    lv_obj_align( weather_wind_label, weather_wind_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);

    lv_obj_t *weather_imperial_cont = lv_obj_create( weather_setup_tile, NULL);
    lv_obj_set_size( weather_imperial_cont, lv_disp_get_hor_res( NULL ), 30);
    lv_obj_add_style( weather_imperial_cont, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_obj_align( weather_imperial_cont, weather_wind_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    weather_imperial_onoff = wf_add_switch( weather_imperial_cont, false );
    lv_obj_align( weather_imperial_onoff, weather_imperial_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_set_event_cb( weather_imperial_onoff, weather_imperial_onoff_event_handler);
    lv_obj_t *weather_imperial_label = lv_label_create(weather_imperial_cont, NULL);
    lv_obj_add_style( weather_imperial_label, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_label_set_text( weather_imperial_label, "Use Imperial");
    lv_obj_align( weather_imperial_label, weather_imperial_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);

    lv_obj_t *weather_widget_cont = lv_obj_create( weather_setup_tile, NULL);
    lv_obj_set_size( weather_widget_cont, lv_disp_get_hor_res( NULL ), 30);
    lv_obj_add_style( weather_widget_cont, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_obj_align( weather_widget_cont, weather_imperial_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    weather_widget_onoff = wf_add_switch( weather_widget_cont, false );
    lv_obj_align( weather_widget_onoff, weather_widget_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_set_event_cb( weather_widget_onoff, weather_widget_onoff_event_handler);
    lv_obj_t *weather_widget_label = lv_label_create( weather_widget_cont, NULL);
    lv_obj_add_style( weather_widget_label, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_label_set_text( weather_widget_label, "widget");
    lv_obj_align( weather_widget_label, weather_widget_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);

    if ( weather_config->autosync)
        lv_switch_on(weather_autosync_onoff, LV_ANIM_OFF);
    else
        lv_switch_off(weather_autosync_onoff, LV_ANIM_OFF);

    if ( weather_config->showWind )
        lv_switch_on( weather_wind_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( weather_wind_onoff, LV_ANIM_OFF );

    if ( weather_config->imperial )
        lv_switch_on( weather_imperial_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( weather_imperial_onoff, LV_ANIM_OFF );

    if ( weather_config->widget )
        lv_switch_on( weather_widget_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( weather_widget_onoff, LV_ANIM_OFF );

    blectl_register_cb( BLECTL_MSG_JSON, weather_bluetooth_message_event_cb, "weather setup" );
    gpsctl_register_cb( GPSCTL_SET_APP_LOCATION, weather_gpsctl_app_use_location_event_cb, "gpsctl weather");
}

static void weather_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void weather_num_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        num_keyboard_set_textarea( obj );
    }
}

static void weather_autosync_onoff_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch (event) {
        case (LV_EVENT_VALUE_CHANGED):      weather_config_t *weather_config = weather_get_config();
                                            weather_config->autosync = lv_switch_get_state( obj );
                                            break;
    }
}

static void weather_widget_onoff_event_handler(lv_obj_t *obj, lv_event_t event) {
    switch (event) {
        case ( LV_EVENT_VALUE_CHANGED ):    weather_config_t *weather_config = weather_get_config();
                                            weather_config->widget = lv_switch_get_state( obj );
                                            if ( weather_config->widget ) {
                                                weather_add_widget();
                                            }
                                            else {
                                                weather_remove_widget();
                                            }
                                            break;
    }
}

static void weather_wind_onoff_event_handler(lv_obj_t *obj, lv_event_t event) {
    switch (event) {
        case ( LV_EVENT_VALUE_CHANGED ):    weather_config_t *weather_config = weather_get_config();
                                            weather_config->showWind = lv_switch_get_state( obj );
                                            break;
    }
}

static void weather_imperial_onoff_event_handler(lv_obj_t *obj, lv_event_t event) {
    switch (event) {
        case ( LV_EVENT_VALUE_CHANGED ):    weather_config_t *weather_config = weather_get_config();
                                            weather_config->imperial = lv_switch_get_state( obj );
                                            break;
    }
}

static void exit_weather_widget_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):           keyboard_hide();
                                            weather_config_t *weather_config = weather_get_config();
                                            strncpy( weather_config->apikey, lv_textarea_get_text( weather_apikey_textfield ), sizeof( weather_config->apikey ) );
                                            strncpy( weather_config->lat, lv_textarea_get_text( weather_lat_textfield ), sizeof( weather_config->lat ) );
                                            strncpy( weather_config->lon, lv_textarea_get_text( weather_lon_textfield ), sizeof( weather_config->lon ) );
                                            weather_save_config();
                                            mainbar_jump_back();
                                            break;
    }
}

bool weather_bluetooth_message_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_MSG_JSON:       weather_bluetooth_message_msg_pharse( *(BluetoothJsonRequest*)arg );
                                    break;
    }
    return( true );
}

void weather_bluetooth_message_msg_pharse( BluetoothJsonRequest &doc ) {
    if( !strcmp( doc["t"], "conf" ) ) {
        if ( !strcmp( doc["app"], "weather" ) ) {

            weather_config_t *weather_config = weather_get_config();
            strncpy( weather_config->apikey, doc["apikey"] |"", sizeof( weather_config->apikey ) );
            strncpy( weather_config->lat, doc["lat"] | "", sizeof( weather_config->lat ) );
            strncpy( weather_config->lon, doc["lon"] | "", sizeof( weather_config->lon ) );
            weather_save_config();

            lv_textarea_set_text( weather_apikey_textfield, weather_config->apikey );
            lv_textarea_set_text( weather_lat_textfield, weather_config->lat );
            lv_textarea_set_text( weather_lon_textfield, weather_config->lon );

            motor_vibe(100);
        }

    }
}

bool weather_gpsctl_app_use_location_event_cb( EventBits_t event, void *arg ) {
    gps_data_t *gps_data = (gps_data_t*)arg;
    weather_config_t *weather_config = weather_get_config();

    switch ( event ) {
        case GPSCTL_SET_APP_LOCATION:
                snprintf( weather_config->lat, sizeof( weather_config->lat ), "%.2f", gps_data->lat );
                snprintf( weather_config->lon, sizeof( weather_config->lon ), "%.2f", gps_data->lon );
                weather_save_config();
                lv_textarea_set_text( weather_lat_textfield, weather_config->lat );
                lv_textarea_set_text( weather_lon_textfield, weather_config->lon );
            break;
    }
    return( true );
 }