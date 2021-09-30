/****************************************************************************
 *   Aug 3 12:17:11 2020
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

#include "osmmap_app.h"
#include "osmmap_app_main.h"
#include "app/osmmap/config/osmmap_config.h"

#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/display.h"
#include "hardware/gpsctl.h"
#include "hardware/blectl.h"
#include "hardware/wifictl.h"
#include "hardware/touch.h"
#include "hardware/powermgm.h"

#include "utils/osm_map/osm_map.h"
#include "utils/json_psram_allocator.h"

#ifdef NATIVE_64BIT
    #include <iostream>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include "utils/logging.h"
    #include "utils/millis.h"
    #include <string>
    #include "utils/osm_map/osmtileserver.h"

    using namespace std;
    #define String string

    uint32_t eventmask = 0;
    const uint8_t * osm_server_json_start = osmtileserver_json;
#else
    #include <Arduino.h>
    #include <FS.h>
    #include <SPIFFS.h>
    #include "gui/mainbar/setup_tile/watchface/watchface_tile.h"

    EventGroupHandle_t osmmap_event_handle = NULL;                  /** @brief osm tile image update event queue */
    TaskHandle_t _osmmap_update_Task;                               /** @brief osm tile image update Task */
    TaskHandle_t _osmmap_load_ahead_Task;                           /** @brief osm tile image update Task */

    extern const uint8_t osm_server_json_start[] asm("_binary_src_utils_osm_map_osmtileserver_json_start");
    extern const uint8_t osm_server_json_end[] asm("_binary_src_utils_osm_map_osmtileserver_json_end");
#endif

lv_task_t *osmmap_main_tile_task;                               /** @brief osm active/inactive task for show/hide user interface */

lv_obj_t *osmmap_app_main_tile = NULL;                          /** @brief osm main tile obj */
lv_obj_t *osmmap_app_tile_img = NULL;                           /** @brief osm tile image obj */
lv_obj_t *osmmap_app_pos_img = NULL;                            /** @brief osm position point obj */
lv_obj_t *osmmap_lonlat_label = NULL;                           /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_north_btn = NULL;                              /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_south_btn = NULL;                              /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_west_btn = NULL;                               /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_zoom_northwest_btn = NULL;                     /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_zoom_northeast_btn = NULL;                     /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_zoom_southwest_btn = NULL;                     /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_zoom_southeast_btn = NULL;                     /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_east_btn = NULL;                               /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_exit_btn = NULL;                               /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_zoom_in_btl = NULL;                            /** @brief osm zoom in icon/button obj */
lv_obj_t *osmmap_zoom_out_btl = NULL;                           /** @brief osm zoom out icon/button obj */

lv_obj_t *osmmap_layers_btn = NULL;                             /** @brief osm exit icon/button obj */
lv_obj_t *osmmap_top_menu = NULL;
lv_obj_t *osmmap_sub_menu_layers = NULL;
lv_obj_t *osmmap_sub_menu_setting = NULL;                       /** @brief osm style list box */

lv_style_t osmmap_app_main_style;                               /** @brief osm main styte obj */
lv_style_t osmmap_app_btn_style;                                /** @brief osm main styte obj */
lv_style_t osmmap_app_label_style;                              /** @brief osm main styte obj */
lv_style_t osmmap_app_nav_style;                                /** @brief osm main styte obj */

static volatile bool osmmap_app_active = false;                 /** @brief osm app active/inactive flag, true means active */
static volatile bool osmmap_block_return_maintile = false;      /** @brief osm block to maintile state store */
static volatile bool osmmap_block_show_messages = false;        /** @brief osm show messages state store */
static volatile bool osmmap_block_watchface = false;            /** @brief osm statusbar force dark mode state store */
static volatile bool osmmap_gps_state = false;                  /** @brief osm gps state on enter osmmap */
static volatile bool osmmap_gps_on_standby_state = false;       /** @brief osm gps on standby on enter osmmap */
static volatile bool osmmap_wifi_state = false;                 /** @brief osm wifi state on enter osmmap */
static volatile uint64_t last_touch = 0;
osm_location_t *osmmap_location = NULL;             /** @brief osm location obj */
osmmap_config_t osmmap_config;

LV_IMG_DECLARE(layers_dark_48px);
LV_IMG_DECLARE(exit_dark_48px);
LV_IMG_DECLARE(zoom_in_dark_48px);
LV_IMG_DECLARE(zoom_out_dark_48px);
LV_IMG_DECLARE(osm_64px);
LV_IMG_DECLARE(info_fail_16px);
LV_IMG_DECLARE(checked_dark_16px);
LV_IMG_DECLARE(unchecked_dark_16px);
LV_FONT_DECLARE(Ubuntu_12px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

void osmmap_main_tile_update_task( lv_task_t * task );
void osmmap_update_request( void );
void osmmap_update_Task( void * pvParameters );
void osmmap_load_ahead_Task( void * pvParameters );
static void osmmap_app_get_setting_menu_cb( lv_obj_t * obj, lv_event_t event );
void osmmap_app_set_setting_menu( lv_obj_t *menu );
bool osmmap_app_touch_event_cb( EventBits_t event, void *arg );
void osmmap_app_set_left_right_hand( bool left_right_hand );
static void nav_direction_osmmap_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void nav_center_osmmap_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void zoom_in_osmmap_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void zoom_out_osmmap_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_osmmap_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void osmmap_tile_server_event_cb( lv_obj_t * obj, lv_event_t event );
static void layers_btn_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
void osmmap_update_map( osm_location_t *osmmap_location, double lon, double lat, uint32_t zoom );
bool osmmap_gpsctl_event_cb( EventBits_t event, void *arg );
void osmmap_add_tile_server_list( lv_obj_t *layers_list );
void osmmap_activate_cb( void );
void osmmap_hibernate_cb( void );
bool osmmap_button_cb( EventBits_t event, void *arg );

void osmmap_app_main_setup( uint32_t tile_num ) {
    /**
     * load config
     */
    osmmap_config.load();
    osmmap_location = osm_map_create_location_obj();
    osmmap_location->load_ahead = osmmap_config.load_ahead;
#if defined( M5PAPER )
    osmmap_location->tilex_dest_px_res = 540;
    osmmap_location->tiley_dest_px_res = 540;
#endif
    /**
     * geht app tile
     */
    osmmap_app_main_tile = mainbar_get_tile_obj( tile_num );

    lv_style_copy( &osmmap_app_main_style, ws_get_mainbar_style() );
    lv_obj_add_style( osmmap_app_main_tile, LV_OBJ_PART_MAIN, &osmmap_app_main_style );

    lv_style_copy( &osmmap_app_btn_style, ws_get_mainbar_style() );
    lv_style_set_image_recolor( &osmmap_app_btn_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_image_recolor_opa( &osmmap_app_btn_style, LV_OBJ_PART_MAIN, LV_OPA_100 );

    lv_style_copy( &osmmap_app_nav_style, ws_get_mainbar_style() );
    lv_style_set_radius( &osmmap_app_nav_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &osmmap_app_nav_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );

    lv_style_copy( &osmmap_app_label_style, ws_get_mainbar_style() );
    lv_style_set_text_font( &osmmap_app_label_style, LV_OBJ_PART_MAIN, &Ubuntu_12px );
    lv_style_set_text_color(&osmmap_app_label_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );

    lv_obj_t *osmmap_cont = lv_obj_create( osmmap_app_main_tile, NULL );
    lv_obj_set_size(osmmap_cont, lv_disp_get_hor_res( NULL )>512?lv_disp_get_hor_res( NULL ):240, lv_disp_get_hor_res( NULL )>512?lv_disp_get_hor_res( NULL ):240 );
    lv_obj_add_style( osmmap_cont, LV_OBJ_PART_MAIN, &osmmap_app_main_style );
    lv_obj_align( osmmap_cont, osmmap_app_main_tile, LV_ALIGN_IN_TOP_MID, 0, 0 );

    osmmap_app_tile_img = lv_img_create( osmmap_cont, NULL );
    lv_obj_set_width( osmmap_app_tile_img, lv_disp_get_hor_res( NULL )>512?lv_disp_get_hor_res( NULL ):240 );
    lv_obj_set_height( osmmap_app_tile_img, lv_disp_get_hor_res( NULL )>512?lv_disp_get_hor_res( NULL ):240 );
    lv_img_set_src( osmmap_app_tile_img, osm_map_get_no_data_image() );
#ifdef M5PAPER
    lv_img_set_zoom( osmmap_app_tile_img, 540 );
#endif
    lv_obj_align( osmmap_app_tile_img, osmmap_cont, LV_ALIGN_CENTER, 0, 0 );

    osmmap_app_pos_img = lv_img_create( osmmap_cont, NULL );
    lv_img_set_src( osmmap_app_pos_img, &info_fail_16px );
    lv_obj_align( osmmap_app_pos_img, osmmap_cont, LV_ALIGN_IN_TOP_LEFT, 120, 120 );
    lv_obj_set_hidden( osmmap_app_pos_img, true );

    osmmap_lonlat_label = lv_label_create( osmmap_cont, NULL );
    lv_obj_add_style( osmmap_lonlat_label, LV_OBJ_PART_MAIN, &osmmap_app_label_style );
    lv_obj_align( osmmap_lonlat_label, osmmap_cont, LV_ALIGN_IN_TOP_LEFT, 3, 0 );
    lv_label_set_text( osmmap_lonlat_label, "0 / 0" );

    osmmap_layers_btn = wf_add_menu_button( osmmap_cont, layers_btn_app_main_event_cb, &osmmap_app_btn_style );
    lv_obj_align( osmmap_layers_btn, osmmap_cont, LV_ALIGN_IN_TOP_LEFT, 10, 10 );

    osmmap_exit_btn = wf_add_exit_button( osmmap_cont, exit_osmmap_app_main_event_cb, &osmmap_app_btn_style );
    lv_obj_align( osmmap_exit_btn, osmmap_cont, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    osmmap_zoom_in_btl = wf_add_zoom_in_button( osmmap_cont, zoom_in_osmmap_app_main_event_cb, &osmmap_app_btn_style );
    lv_obj_align( osmmap_zoom_in_btl, osmmap_cont, LV_ALIGN_IN_TOP_RIGHT, -10, 10 );

    osmmap_zoom_out_btl = wf_add_zoom_out_button( osmmap_cont, zoom_out_osmmap_app_main_event_cb, &osmmap_app_btn_style );
    lv_obj_align( osmmap_zoom_out_btl, osmmap_cont, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );

    osmmap_north_btn = lv_btn_create( osmmap_cont, NULL );
    lv_obj_set_width( osmmap_north_btn, 80 );
    lv_obj_set_height( osmmap_north_btn, 48 );
    lv_obj_add_protect( osmmap_north_btn, LV_PROTECT_CLICK_FOCUS );
    lv_obj_add_style( osmmap_north_btn, LV_BTN_PART_MAIN, &osmmap_app_nav_style );
    lv_obj_align( osmmap_north_btn, osmmap_cont, LV_ALIGN_IN_TOP_MID, 0, 0 );
    lv_obj_set_event_cb( osmmap_north_btn, nav_direction_osmmap_app_main_event_cb );

    osmmap_south_btn = lv_btn_create( osmmap_cont, osmmap_north_btn );
    lv_obj_align( osmmap_south_btn, osmmap_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_obj_set_event_cb( osmmap_south_btn, nav_direction_osmmap_app_main_event_cb );

    osmmap_west_btn = lv_btn_create( osmmap_cont, NULL );
    lv_obj_set_width( osmmap_west_btn, 48 );
    lv_obj_set_height( osmmap_west_btn, 80 );
    lv_obj_add_protect( osmmap_west_btn, LV_PROTECT_CLICK_FOCUS );
    lv_obj_add_style( osmmap_west_btn, LV_BTN_PART_MAIN, &osmmap_app_nav_style );
    lv_obj_align( osmmap_west_btn, osmmap_cont, LV_ALIGN_IN_LEFT_MID, 0, 0 );
    lv_obj_set_event_cb( osmmap_west_btn, nav_direction_osmmap_app_main_event_cb );

    osmmap_east_btn = lv_btn_create( osmmap_cont, osmmap_west_btn );
    lv_obj_align( osmmap_east_btn, osmmap_cont, LV_ALIGN_IN_RIGHT_MID, 0, 0 );
    lv_obj_set_event_cb( osmmap_east_btn, nav_direction_osmmap_app_main_event_cb );

    osmmap_zoom_northwest_btn = lv_btn_create( osmmap_cont, NULL );
    lv_obj_set_width( osmmap_zoom_northwest_btn, 72 );
    lv_obj_set_height( osmmap_zoom_northwest_btn, 72 );
    lv_obj_add_protect( osmmap_zoom_northwest_btn, LV_PROTECT_CLICK_FOCUS );
    lv_imgbtn_set_checkable( osmmap_zoom_northwest_btn, true );
    lv_obj_add_style( osmmap_zoom_northwest_btn, LV_BTN_PART_MAIN, &osmmap_app_nav_style );
    lv_obj_align( osmmap_zoom_northwest_btn, osmmap_cont, LV_ALIGN_CENTER, -36, -36 );
    lv_obj_set_event_cb( osmmap_zoom_northwest_btn, nav_center_osmmap_app_main_event_cb );

    osmmap_zoom_northeast_btn = lv_btn_create( osmmap_cont, osmmap_zoom_northwest_btn );
    lv_obj_align( osmmap_zoom_northeast_btn, osmmap_cont, LV_ALIGN_CENTER, 36, -36 );
    lv_obj_set_event_cb( osmmap_zoom_northeast_btn, nav_center_osmmap_app_main_event_cb );

    osmmap_zoom_southwest_btn = lv_btn_create( osmmap_cont, osmmap_zoom_northwest_btn );
    lv_obj_align( osmmap_zoom_southwest_btn, osmmap_cont, LV_ALIGN_CENTER, -36, 36 );
    lv_obj_set_event_cb( osmmap_zoom_southwest_btn, nav_center_osmmap_app_main_event_cb );

    osmmap_zoom_southeast_btn = lv_btn_create( osmmap_cont, osmmap_zoom_northwest_btn );
    lv_obj_align( osmmap_zoom_southeast_btn, osmmap_cont, LV_ALIGN_CENTER, 36, 36 );
    lv_obj_set_event_cb( osmmap_zoom_southeast_btn, nav_center_osmmap_app_main_event_cb );
    /**
     * setup menu
     */
    osmmap_sub_menu_layers = lv_list_create( osmmap_cont, NULL );
    lv_obj_set_size( osmmap_sub_menu_layers, 160, 200 );
    lv_obj_align( osmmap_sub_menu_layers, osmmap_cont, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    osmmap_add_tile_server_list( osmmap_sub_menu_layers );
    lv_obj_set_hidden( osmmap_sub_menu_layers, true );

    osmmap_sub_menu_setting = lv_list_create( osmmap_cont, NULL );
    lv_obj_set_size( osmmap_sub_menu_setting, 160, 200 );
    lv_obj_align( osmmap_sub_menu_setting, osmmap_cont, LV_ALIGN_IN_RIGHT_MID, 0, 0);
    osmmap_app_set_setting_menu( osmmap_sub_menu_setting );
    lv_obj_set_hidden( osmmap_sub_menu_setting, true );
    /**
     * set left/right hand mode
     */
    osmmap_app_set_left_right_hand( osmmap_config.left_right_hand );
    /**
     * setup event callback and background Task
     */
    mainbar_add_tile_activate_cb( tile_num, osmmap_activate_cb );
    mainbar_add_tile_hibernate_cb( tile_num, osmmap_hibernate_cb );
    mainbar_add_tile_button_cb( tile_num, osmmap_button_cb );
    gpsctl_register_cb( GPSCTL_SET_APP_LOCATION | GPSCTL_UPDATE_LOCATION, osmmap_gpsctl_event_cb, "osm" );
    touch_register_cb( TOUCH_UPDATE , osmmap_app_touch_event_cb, "osm touch" );
#ifdef NATIVE_64BIT
    eventmask = 0;
#else
    osmmap_event_handle = xEventGroupCreate();
#endif
    osmmap_main_tile_task = lv_task_create( osmmap_main_tile_update_task, 250, LV_TASK_PRIO_MID, NULL );
}

bool osmmap_app_touch_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case( TOUCH_UPDATE ):
            if ( osmmap_app_active ) {
                last_touch = millis();
            }
            break;
    }
    return( false );
}

bool osmmap_button_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_LEFT:   osm_map_zoom_in( osmmap_location );
                            if ( osmmap_app_active )
                                osmmap_update_request();
                            break;
        case BUTTON_RIGHT:  osm_map_zoom_out( osmmap_location );
                            if ( osmmap_app_active )
                                osmmap_update_request();
                            break;
    }
    return( true );
}

void osmmap_app_set_left_right_hand( bool left_right_hand ) {
    if ( left_right_hand ) {
        lv_obj_align( osmmap_layers_btn, lv_obj_get_parent( osmmap_layers_btn ), LV_ALIGN_IN_TOP_RIGHT, -10, 10 );
        lv_obj_align( osmmap_exit_btn, lv_obj_get_parent( osmmap_exit_btn ), LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
        lv_obj_align( osmmap_zoom_in_btl, lv_obj_get_parent( osmmap_zoom_in_btl ), LV_ALIGN_IN_TOP_LEFT, 10, 10 );
        lv_obj_align( osmmap_zoom_out_btl, lv_obj_get_parent( osmmap_zoom_out_btl ), LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
        lv_obj_align( osmmap_sub_menu_layers, lv_obj_get_parent( osmmap_sub_menu_layers ), LV_ALIGN_IN_LEFT_MID, 0, 0);
        lv_obj_align( osmmap_sub_menu_setting, lv_obj_get_parent( osmmap_sub_menu_setting ), LV_ALIGN_IN_LEFT_MID, 0, 0);
    }
    else {
        lv_obj_align( osmmap_layers_btn, lv_obj_get_parent( osmmap_layers_btn ), LV_ALIGN_IN_TOP_LEFT, 10, 10 );
        lv_obj_align( osmmap_exit_btn, lv_obj_get_parent( osmmap_exit_btn ), LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
        lv_obj_align( osmmap_zoom_in_btl, lv_obj_get_parent( osmmap_zoom_in_btl ), LV_ALIGN_IN_TOP_RIGHT, -10, 10 );
        lv_obj_align( osmmap_zoom_out_btl, lv_obj_get_parent( osmmap_zoom_out_btl ), LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
        lv_obj_align( osmmap_sub_menu_layers, lv_obj_get_parent( osmmap_sub_menu_layers ), LV_ALIGN_IN_RIGHT_MID, 0, 0);
        lv_obj_align( osmmap_sub_menu_setting, lv_obj_get_parent( osmmap_sub_menu_setting ), LV_ALIGN_IN_RIGHT_MID, 0, 0);
    }
}

void osmmap_app_set_setting_menu( lv_obj_t *menu ) {
    lv_obj_t * menu_entry;
    char cachestring[32] = "";

    if ( menu ) {
        /**
         * clear all menu entrys
         */
        while ( lv_list_remove( menu, 0 ) );
        /**
         * add menu entry
         */
        menu_entry = lv_list_add_btn( menu, NULL, "OSM maps" );
        lv_obj_set_event_cb( menu_entry, osmmap_app_get_setting_menu_cb );
        menu_entry = lv_list_add_btn( menu, NULL, "left/right hand" );
        lv_obj_set_event_cb( menu_entry, osmmap_app_get_setting_menu_cb );
        menu_entry = lv_list_add_btn( menu, osmmap_config.gps_autoon ? &checked_dark_16px : &unchecked_dark_16px, "autostart gps" );
        lv_obj_set_event_cb( menu_entry, osmmap_app_get_setting_menu_cb );
        menu_entry = lv_list_add_btn( menu, osmmap_config.gps_on_standby ? &checked_dark_16px : &unchecked_dark_16px, "gps on standby" );
        lv_obj_set_event_cb( menu_entry, osmmap_app_get_setting_menu_cb );
        menu_entry = lv_list_add_btn( menu, osmmap_config.wifi_autoon ? &checked_dark_16px : &unchecked_dark_16px, "autostart wifi" );
        lv_obj_set_event_cb( menu_entry, osmmap_app_get_setting_menu_cb );
        menu_entry = lv_list_add_btn( menu, osmmap_config.load_ahead ? &checked_dark_16px : &unchecked_dark_16px, "load ahead" );
        lv_obj_set_event_cb( menu_entry, osmmap_app_get_setting_menu_cb );
        snprintf( cachestring, sizeof( cachestring ), "%dkB cached", osm_map_get_used_cache_size( osmmap_location ) / 1024 );
        menu_entry = lv_list_add_btn( menu, NULL, cachestring );
        lv_obj_set_event_cb( menu_entry, osmmap_app_get_setting_menu_cb );
    }
}

static void osmmap_app_get_setting_menu_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            if ( !strcmp( lv_list_get_btn_text( obj ), "OSM maps") ) {
                lv_obj_set_hidden( osmmap_sub_menu_setting, true );
                lv_obj_set_hidden( osmmap_sub_menu_layers, false );
            }
            else if ( !strcmp( lv_list_get_btn_text( obj ), "load ahead" ) ) {
                osmmap_config.load_ahead = !osmmap_config.load_ahead;
                osmmap_location->load_ahead = osmmap_config.load_ahead;
                osmmap_config.save();
            }
            else if ( !strcmp( lv_list_get_btn_text( obj ), "autostart gps" ) ) {
                osmmap_config.gps_autoon = !osmmap_config.gps_autoon;
                gpsctl_set_autoon( osmmap_config.gps_autoon );
                osmmap_config.save();
            }
            else if ( !strcmp( lv_list_get_btn_text( obj ), "gps on standby" ) ) {
                osmmap_config.gps_on_standby = !osmmap_config.gps_on_standby;
                gpsctl_set_enable_on_standby( osmmap_config.gps_on_standby );
                osmmap_config.save();
            }
            else if ( !strcmp( lv_list_get_btn_text( obj ), "autostart wifi" ) ) {
                osmmap_config.wifi_autoon = !osmmap_config.wifi_autoon;
                wifictl_set_autoon( osmmap_config.load_ahead );
                osmmap_config.save();
            }
            else if ( !strcmp( lv_list_get_btn_text( obj ), "left/right hand" ) ) {
                osmmap_config.left_right_hand = !osmmap_config.left_right_hand;
                osmmap_app_set_left_right_hand( osmmap_config.left_right_hand );
                osmmap_config.save();
            }
            osmmap_app_set_setting_menu( osmmap_sub_menu_setting );
            break;
    }
}

/**
 * @brief when osm is active, this task get the use inactive time and hide
 * the statusbar and icon.
 */
void osmmap_main_tile_update_task( lv_task_t * task ) {
    /*
     * check if maintile alread initialized
     */
/*
    if ( osmmap_app_active ) {
        if ( last_touch + 5000 < millis() ) {
            lv_obj_set_hidden( osmmap_layers_btn, true );
            lv_obj_set_hidden( osmmap_exit_btn, true );
            lv_obj_set_hidden( osmmap_zoom_in_btl, true );
            lv_obj_set_hidden( osmmap_zoom_out_btl, true );
            lv_obj_set_hidden( osmmap_zoom_northwest_btn, true );
            lv_obj_set_hidden( osmmap_zoom_northeast_btn, true );
            lv_obj_set_hidden( osmmap_zoom_southwest_btn, true );
            lv_obj_set_hidden( osmmap_zoom_southeast_btn, true );
            lv_obj_set_hidden( osmmap_north_btn, true );
            lv_obj_set_hidden( osmmap_south_btn, true );
            lv_obj_set_hidden( osmmap_west_btn, true );
            lv_obj_set_hidden( osmmap_east_btn, true );
            lv_obj_set_hidden( osmmap_sub_menu_layers, true );
            lv_obj_set_hidden( osmmap_sub_menu_setting, true );
        }
        else {
            lv_obj_set_hidden( osmmap_layers_btn, false );
            lv_obj_set_hidden( osmmap_exit_btn, false );
            lv_obj_set_hidden( osmmap_zoom_in_btl, false );
            lv_obj_set_hidden( osmmap_zoom_out_btl, false );
            lv_obj_set_hidden( osmmap_zoom_northwest_btn, false );
            lv_obj_set_hidden( osmmap_zoom_northeast_btn, false );
            lv_obj_set_hidden( osmmap_zoom_southwest_btn, false );
            lv_obj_set_hidden( osmmap_zoom_southeast_btn, false );
            lv_obj_set_hidden( osmmap_north_btn, false );
            lv_obj_set_hidden( osmmap_south_btn, false );
            lv_obj_set_hidden( osmmap_west_btn, false );
            lv_obj_set_hidden( osmmap_east_btn, false );
        }
    }
*/
#ifdef NATIVE_64BIT
    osmmap_load_ahead_Task( NULL );
    osmmap_update_Task( NULL );
#endif
}

bool osmmap_gpsctl_event_cb( EventBits_t event, void *arg ) {
    gps_data_t *gps_data = NULL;
    char lonlat[64] = "";
    
    switch ( event ) {
        case GPSCTL_SET_APP_LOCATION:
            /**
             * update location and tile map image on new location
             */
            OSMMAP_APP_LOG("get new gps coor.");
            gps_data = ( gps_data_t *)arg;
            osm_map_set_lon_lat( osmmap_location, gps_data->lon, gps_data->lat );
            snprintf( lonlat, sizeof( lonlat ), "%f° / %f°", gps_data->lat, gps_data->lon );
            lv_label_set_text( osmmap_lonlat_label, (const char*)lonlat );
            if ( osmmap_app_active )
                osmmap_update_request();
            break;
        case GPSCTL_UPDATE_LOCATION:
            /**
             * update location and tile map image on new location
             */
            OSMMAP_APP_LOG("get new gps coor.");
            gps_data = ( gps_data_t *)arg;
            osm_map_set_lon_lat( osmmap_location, gps_data->lon, gps_data->lat );
            snprintf( lonlat, sizeof( lonlat ), "%f° / %f°", gps_data->lat, gps_data->lon );
            lv_label_set_text( osmmap_lonlat_label, (const char*)lonlat );
            if ( osmmap_app_active )
                osmmap_update_request();
            break;
    }
    return( true );
}


void osmmap_update_request( void ) {
    /**
     * check if another osm tile image update is running
     */
#ifdef NATIVE_64BIT
    if ( eventmask & OSM_APP_UPDATE_REQUEST ) {
        return;
    }
    else {
        eventmask |= OSM_APP_UPDATE_REQUEST;
    }
#else
    if ( xEventGroupGetBits( osmmap_event_handle ) & OSM_APP_UPDATE_REQUEST ) {
        return;
    }
    else {
        xEventGroupSetBits( osmmap_event_handle, OSM_APP_UPDATE_REQUEST );
    }
#endif
}

void osmmap_load_ahead_Task( void * pvParameters ) {
#ifdef NATIVE_64BIT
    /**
     * check for  load ahead request
     */
    if ( eventmask & OSM_APP_LOAD_AHEAD_REQUEST ) {
        /**
         * check if load ahead need or finsh
         */
        OSMMAP_APP_LOG("start load ahead update handler");
        eventmask &= ~OSM_APP_LOAD_AHEAD_REQUEST ;
        while ( osm_map_load_tiles_ahead( osmmap_location ) ) {}
    }
#else
    OSMMAP_APP_INFO_LOG("start osm map load ahead background task, heap: %d", ESP.getFreeHeap() );
    while( true ) {
        /**
         * check for  load ahead request
         */
        if ( xEventGroupGetBits( osmmap_event_handle ) & OSM_APP_LOAD_AHEAD_REQUEST ) {
            /**
             * check if load ahead need or finsh
             */
            OSMMAP_APP_LOG("start load ahead update handler");
            xEventGroupClearBits( osmmap_event_handle, OSM_APP_LOAD_AHEAD_REQUEST );
            while ( osm_map_load_tiles_ahead( osmmap_location ) ) {
                /**
                 * block this task for 125ms
                 */
                vTaskDelay( 25 );
            }
        }
        /**
         * check if for a task exit request
         */
        if ( xEventGroupGetBits( osmmap_event_handle ) & OSM_APP_TASK_EXIT_REQUEST ) {
            break;
        }
        /**
         * block this task for 125ms
         */
        vTaskDelay( 25 );
    }
    OSMMAP_APP_INFO_LOG("finsh osm map load ahead background task, heap: %d", ESP.getFreeHeap() );
    vTaskDelete( NULL );    
#endif
}

void osmmap_update_Task( void * pvParameters ) {
#ifdef NATIVE_64BIT
    /**
     * check if a tile image update is requested
     */
    if ( eventmask & OSM_APP_UPDATE_REQUEST ) {
        /**
         * check if a tile image update is required and update them
         */
        OSMMAP_APP_LOG("start osm map update");
        if( osm_map_update( osmmap_location ) ) {
            if ( osm_map_get_tile_image( osmmap_location ) ) {
                lv_img_set_src( osmmap_app_tile_img, osm_map_get_tile_image( osmmap_location ) );
            }
            lv_obj_align( osmmap_app_tile_img, lv_obj_get_parent( osmmap_app_tile_img ), LV_ALIGN_CENTER, 0 , 0 );
            eventmask |= OSM_APP_LOAD_AHEAD_REQUEST;
        }
        /**
         * update postion point on the tile image when is valid
         */
        if ( osmmap_location->tilexy_pos_valid ) {
            lv_obj_align( osmmap_app_pos_img, lv_obj_get_parent( osmmap_app_pos_img ), LV_ALIGN_IN_TOP_LEFT, osmmap_location->tilex_pos - 8 , osmmap_location->tiley_pos - 8 );
            lv_obj_set_hidden( osmmap_app_pos_img, false );
        }
        else {
            lv_obj_set_hidden( osmmap_app_pos_img, true );
        }
        /**
         * clear update request flag
         */
        eventmask &= ~OSM_APP_UPDATE_REQUEST;
    }
#else
    OSMMAP_APP_INFO_LOG("start osm map tile background update task, heap: %d", ESP.getFreeHeap() );
    while( true ) {
        /**
         * check if a tile image update is requested
         */
        if ( xEventGroupGetBits( osmmap_event_handle ) & OSM_APP_UPDATE_REQUEST ) {
            /**
             * check if a tile image update is required and update them
             */
            OSMMAP_APP_LOG("start osm map update");
            if( osm_map_update( osmmap_location ) ) {
                if ( osm_map_get_tile_image( osmmap_location ) ) {
                    lv_img_set_src( osmmap_app_tile_img, osm_map_get_tile_image( osmmap_location ) );
                }
                lv_obj_align( osmmap_app_tile_img, lv_obj_get_parent( osmmap_app_tile_img ), LV_ALIGN_CENTER, 0 , 0 );
                xEventGroupSetBits( osmmap_event_handle, OSM_APP_LOAD_AHEAD_REQUEST );
            }
            /**
             * update postion point on the tile image when is valid
             */
            if ( osmmap_location->tilexy_pos_valid ) {
                lv_obj_align( osmmap_app_pos_img, lv_obj_get_parent( osmmap_app_pos_img ), LV_ALIGN_IN_TOP_LEFT, osmmap_location->tilex_pos - 8 , osmmap_location->tiley_pos - 8 );
                lv_obj_set_hidden( osmmap_app_pos_img, false );
            }
            else {
                lv_obj_set_hidden( osmmap_app_pos_img, true );
            }
            /**
             * clear update request flag
             */
            xEventGroupClearBits( osmmap_event_handle, OSM_APP_UPDATE_REQUEST );
        }
        /**
         * check if for a task exit request
         */
        if ( xEventGroupGetBits( osmmap_event_handle ) & OSM_APP_TASK_EXIT_REQUEST ) {
            OSMMAP_APP_INFO_LOG("stop osm map update task");
            break;
        }
        /**
         * block this task for 125ms
         */
        vTaskDelay( 25 );
    }
    OSMMAP_APP_INFO_LOG("finsh osm map tile background update task, heap: %d", ESP.getFreeHeap() );
    vTaskDelete( NULL );    
#endif
}

static void exit_osmmap_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            /**
             * exit to mainbar
             */
            mainbar_jump_back();
            break;
    }
}

static void nav_direction_osmmap_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        /**
         * long press event for center
         */
        case( LV_EVENT_LONG_PRESSED ):
            OSMMAP_APP_LOG("center map to pos");
            osm_map_center_location( osmmap_location );
            osmmap_update_request();
            break;
        /**
         * short press event to move
         */   
        case( LV_EVENT_SHORT_CLICKED ):
            if ( obj == osmmap_north_btn ) {
                OSMMAP_APP_LOG("nav north direction");
                osm_map_nav_direction( osmmap_location, north );
            }
            else if ( obj == osmmap_south_btn ) {
                OSMMAP_APP_LOG("nav south direction");
                osm_map_nav_direction( osmmap_location, south );
            }
            else if ( obj == osmmap_west_btn ) {
                OSMMAP_APP_LOG("nav west direction");
                osm_map_nav_direction( osmmap_location, west );
            }
            else if ( obj == osmmap_east_btn ) {
                OSMMAP_APP_LOG("nav east direction");
                osm_map_nav_direction( osmmap_location, east );
            }
            else {
                OSMMAP_APP_LOG("direction source unknown");
            }
            if ( osmmap_app_active )
                osmmap_update_request();
            break;
    }
}

static void nav_center_osmmap_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        /**
         * long press event for center
         */
        case( LV_EVENT_LONG_PRESSED ):
            OSMMAP_APP_LOG("center map to pos");
            osm_map_center_location( osmmap_location );
            osmmap_update_request();
            break;
        /**
         * short press event to zoom in
         */
        case( LV_EVENT_SHORT_CLICKED ):
            if ( obj == osmmap_zoom_northwest_btn ) {
                OSMMAP_APP_LOG("nav northwest center");
                osm_map_nav_direction( osmmap_location, zoom_northwest );
            }
            else if ( obj == osmmap_zoom_northeast_btn ) {
                OSMMAP_APP_LOG("nav northeast center");
                osm_map_nav_direction( osmmap_location, zoom_northeast );
            }
            else if ( obj == osmmap_zoom_southwest_btn ) {
                OSMMAP_APP_LOG("nav southwest center");
                osm_map_nav_direction( osmmap_location, zoom_southwest );
            }
            else if ( obj == osmmap_zoom_southeast_btn ) {
                OSMMAP_APP_LOG("nav southeast center");
                osm_map_nav_direction( osmmap_location, zoom_southeast );
            }
            else {
                OSMMAP_APP_LOG("zoom source unknown");
                osm_map_nav_direction( osmmap_location, east );
            }
            if ( osmmap_app_active )
                osmmap_update_request();
            break;
    }
}

static void zoom_in_osmmap_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   
            /**
             * increase zoom level
             */
            osm_map_zoom_in( osmmap_location );
            if ( osmmap_app_active )
                osmmap_update_request();
            break;
    }
}

static void zoom_out_osmmap_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   
            /**
             * decrease zoom level
             */
            osm_map_zoom_out( osmmap_location );
            if ( osmmap_app_active )
                osmmap_update_request();
            break;
    }
}

static void layers_btn_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            if ( lv_obj_get_hidden( osmmap_sub_menu_setting ) ) {
                osmmap_app_set_setting_menu( osmmap_sub_menu_setting );
                lv_obj_set_hidden( osmmap_sub_menu_setting, false );
            }
            else {
                lv_obj_set_hidden( osmmap_sub_menu_setting, true );
                lv_obj_set_hidden( osmmap_sub_menu_layers, true );
            }
            break;
    }
}

static void osmmap_tile_server_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_CLICKED: {
            SpiRamJsonDocument doc( strlen( (const char*)osm_server_json_start ) * 2 );
            DeserializationError error = deserializeJson( doc, (const char *)osm_server_json_start );

            if ( error ) {
                OSMMAP_APP_ERROR_LOG("osm server list deserializeJson() failed: %s", error.c_str() );
            }
            else {
                const char *tile_server = doc[ lv_list_get_btn_text( obj ) ];
                OSMMAP_APP_INFO_LOG("new tile server url: %s", tile_server );
                osm_map_set_tile_server( osmmap_location, tile_server );
                strncpy( osmmap_config.osmmap, lv_list_get_btn_text( obj ), sizeof( osmmap_config.osmmap ) );
                osmmap_add_tile_server_list( osmmap_sub_menu_layers );
                osmmap_update_request();
            }
            doc.clear();
            lv_obj_set_hidden( osmmap_sub_menu_layers, true );            
            break;
        }
    }
}

void osmmap_add_tile_server_list( lv_obj_t *layers_list ) {
    lv_obj_t * list_btn;
    
    SpiRamJsonDocument doc( strlen( (const char*)osm_server_json_start ) * 2 );
    DeserializationError error = deserializeJson( doc, (const char *)osm_server_json_start );

    if ( error ) {
        OSMMAP_APP_ERROR_LOG("osm server list deserializeJson() failed: %s", error.c_str() );
    }
    else {
        while ( lv_list_remove( layers_list, 0 ) );
        JsonObject obj = doc.as<JsonObject>();
        for ( JsonPair p : obj ) {
            OSMMAP_APP_LOG("server: %s", p.key().c_str() );
            list_btn = lv_list_add_btn( layers_list, !strcmp( osmmap_config.osmmap, p.key().c_str() ) ? &checked_dark_16px : &unchecked_dark_16px, p.key().c_str() );
            lv_obj_set_event_cb( list_btn, osmmap_tile_server_event_cb );
            if ( !strcmp( osmmap_config.osmmap, p.key().c_str() ) ) {
                const char *osmmap_url = doc[ p.key().c_str() ];
                OSMMAP_APP_INFO_LOG("set osmmap url: %s, %s", p.key().c_str(), osmmap_url );
                osm_map_set_tile_server( osmmap_location, osmmap_url );
            }
        }        
    }
    doc.clear();
}

void osmmap_activate_cb( void ) {
    /**
     * save block show messages state
     */
    osmmap_gps_state = gpsctl_get_autoon();
    if( osmmap_config.gps_autoon ) {
        gpsctl_on();
    }
    /**
     * save block show messages state
     */
    osmmap_wifi_state = wifictl_get_autoon();
    if( osmmap_config.wifi_autoon ) {
        wifictl_on();
        wifictl_set_autoon( osmmap_config.wifi_autoon );
    }
    /**
     * save block show messages state
     */
    osmmap_gps_on_standby_state = gpsctl_get_enable_on_standby();
    if ( osmmap_config.gps_on_standby ) {
        gpsctl_set_enable_on_standby( true );
    }
    /**
     * save block show messages state
     */
#ifdef NATIVE_64BIT

#else
    osmmap_block_watchface = watchface_get_enable_tile_after_wakeup();
    watchface_enable_tile_after_wakeup( false );
#endif
    /**
     * save block show messages state
     */
    osmmap_block_show_messages = blectl_get_show_notification();
    blectl_set_show_notification( false );
    /**
     * save black return to maintile state
     */
    osmmap_block_return_maintile = display_get_block_return_maintile();
    display_set_block_return_maintile( true );
    /**
     * force redraw screen
     */
    lv_obj_invalidate( lv_scr_act() );
    /**
     * set osm app active
     */
    osmmap_app_active = true;
    last_touch = millis();
#ifdef NATIVE_64BIT

#else
    /**
     * start background osm tile image update Task
     */
    xEventGroupClearBits( osmmap_event_handle, OSM_APP_TASK_EXIT_REQUEST );
    xTaskCreate(    osmmap_update_Task,      /* Function to implement the task */
                    "osmmap update Task",    /* Name of the task */
                    5000,                            /* Stack size in words */
                    NULL,                            /* Task input parameter */
                    1,                               /* Priority of the task */
                    &_osmmap_update_Task );  /* Task handle. */

    xTaskCreate(    osmmap_load_ahead_Task,      /* Function to implement the task */
                    "osmmap load ahead Task",    /* Name of the task */
                    5000,                            /* Stack size in words */
                    NULL,                            /* Task input parameter */
                    1,                               /* Priority of the task */
                    &_osmmap_load_ahead_Task );  /* Task handle. */
#endif
    osmmap_update_request();
    lv_img_cache_invalidate_src( osmmap_app_tile_img );
    powermgm_set_perf_mode();
}

void osmmap_hibernate_cb( void ) {
    /**
     * restore back to maintile and status force dark mode
     */
    blectl_set_show_notification( osmmap_block_show_messages );
    display_set_block_return_maintile( osmmap_block_return_maintile );
    gpsctl_set_autoon( osmmap_gps_state );
    wifictl_set_autoon( osmmap_wifi_state );
    gpsctl_set_enable_on_standby( osmmap_gps_on_standby_state );
#ifdef NATIVE_64BIT

#else
    watchface_enable_tile_after_wakeup( osmmap_block_watchface );
#endif
    /**
     * clear cache
     */
    osm_map_clear_cache( osmmap_location );
    /**
     * set osm app inactive
     */
    osmmap_app_active = false;
    /**
     * stop background osm tile image update Task
     */
#ifdef NATIVE_64BIT
    eventmask |= OSM_APP_TASK_EXIT_REQUEST;
#else
    xEventGroupSetBits( osmmap_event_handle, OSM_APP_TASK_EXIT_REQUEST );
#endif
    powermgm_set_normal_mode();
    /**
     * save config
     */
    osmmap_config.save();
}
