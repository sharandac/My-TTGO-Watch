/****************************************************************************
 *   linuxthor 2020
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

#include "hardware/wifictl.h"
#include "hardware/display.h"

#include "wifimon_app.h"
#include "wifimon_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_styles.h"
#include "gui/widget_factory.h"

#ifdef NATIVE_64BIT
    #include <time.h>
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
    #include <math.h>
    #include <lwip/sockets.h>
    #include "esp_wifi.h"

    void wifimon_sniffer_packet_handler( void* buff, wifi_promiscuous_pkt_type_t type );
    static wifi_country_t wifi_country = {.cc="CN", .schan = 1, .nchan = 13}; 
#endif

lv_obj_t *wifimon_app_main_tile = NULL;
lv_obj_t *chart = NULL;
lv_obj_t *channel_select = NULL; 
lv_chart_series_t *ser1 = NULL;
lv_chart_series_t *ser2 = NULL;
lv_chart_series_t *ser3 = NULL;
lv_task_t *_wifimon_app_task = NULL;
int wifimon_display_timeout = 0;

LV_IMG_DECLARE(exit_dark_48px);
LV_IMG_DECLARE(wifimon_app_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_wifimon_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void wifimon_sniffer_set_channel( uint8_t channel );
static void wifimon_app_task( lv_task_t * task );
static void wifimon_activate_cb( void );
static void wifimon_hibernate_cb( void );

uint8_t level = 0, channel = 1;
int data = 0, mgmt = 0, misc = 0; 

#ifdef NATIVE_64BIT

#else
void wifimon_sniffer_packet_handler( void* buff, wifi_promiscuous_pkt_type_t type ) {
    switch( type ) {
        case WIFI_PKT_MGMT: 
            mgmt++;
            break;
        case WIFI_PKT_DATA:
            data++; 
            break; 
        default:  
            misc++;
            break;
    }
}
#endif

static void wifimon_sniffer_set_channel( uint8_t channel ) {
#ifdef NATIVE_64BIT

#else
    esp_wifi_set_channel( channel, WIFI_SECOND_CHAN_NONE );
#endif
    log_i("set wifi channel: %d", channel );
}

static void wifimon_channel_select_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_VALUE_CHANGED: {
            char buf[32];
            lv_roller_get_selected_str( obj, buf, sizeof( buf ) );
            wifimon_sniffer_set_channel( atoi(buf) );
            break;
        }
    }
}

void wifimon_app_main_setup( uint32_t tile_num ) {

    wifimon_app_main_tile = mainbar_get_tile_obj( tile_num );
    /**
     * add chart widget
     */
    chart = lv_chart_create( wifimon_app_main_tile, NULL );
    lv_obj_set_size( chart, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) - THEME_ICON_SIZE );
    lv_obj_align( chart, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_chart_set_type( chart, LV_CHART_TYPE_LINE );  
    lv_chart_set_point_count( chart, 32 );
    lv_obj_set_style_local_bg_opa( chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50 );
    lv_obj_set_style_local_bg_grad_dir( chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER );
    lv_obj_set_style_local_bg_main_stop( chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255 );
    lv_obj_set_style_local_bg_grad_stop( chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0 );
    /**
     * add chart series
     */
    ser1 = lv_chart_add_series( chart, LV_COLOR_RED );
    ser2 = lv_chart_add_series( chart, LV_COLOR_GREEN );
    ser3 = lv_chart_add_series( chart, LV_COLOR_YELLOW );
    /**
     * add exit button
     */
    lv_obj_t * exit_btn = wf_add_exit_button( wifimon_app_main_tile, exit_wifimon_app_main_event_cb );
    lv_obj_align( exit_btn, wifimon_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_ICON_PADDING, -THEME_ICON_PADDING );
    /**
     * add channel select roller
     */
    channel_select = lv_roller_create(wifimon_app_main_tile, NULL);
    lv_roller_set_options( channel_select, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13", LV_ROLLER_MODE_INIFINITE );
    lv_roller_set_visible_row_count( channel_select, 5 );
    lv_obj_align( channel_select, NULL, LV_ALIGN_IN_TOP_LEFT, THEME_ICON_PADDING, THEME_ICON_PADDING );
    lv_obj_set_event_cb( channel_select, wifimon_channel_select_event_handler );
    /**
     * add chart series label
     */
    lv_obj_t * chart_series_label = lv_label_create( wifimon_app_main_tile, NULL );
    lv_label_set_long_mode( chart_series_label, LV_LABEL_LONG_BREAK );
    lv_label_set_recolor( chart_series_label, true );
    lv_label_set_align( chart_series_label, LV_LABEL_ALIGN_RIGHT );       
    lv_label_set_text( chart_series_label, "#ffff00 - misc#\n#ff0000 - mgmt#\n#11ff00 - data#"); 
    lv_obj_set_width( chart_series_label, 70 );
    lv_obj_align( chart_series_label, NULL, LV_ALIGN_IN_TOP_RIGHT, -THEME_ICON_PADDING, THEME_ICON_PADDING );

    mainbar_add_tile_activate_cb( tile_num, wifimon_activate_cb );
    mainbar_add_tile_hibernate_cb( tile_num, wifimon_hibernate_cb );
}

static void exit_wifimon_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):     mainbar_jump_back();
                                      break;
    }
}

static void wifimon_hibernate_cb( void ) {
    if(_wifimon_app_task != NULL) {
        lv_task_del(_wifimon_app_task);
        _wifimon_app_task = NULL;
    }  
#ifdef NATIVE_64BIT

#else
    esp_wifi_set_promiscuous( false ); 
#endif
    wifictl_off();
    /**
     * restore display timeout time
     */
    display_set_timeout( wifimon_display_timeout );
}

static void wifimon_activate_cb( void ) {
    /**
     * restart wifi
     */
    wifictl_off();
    /**
     * setup promiscuous mode
     */
#ifdef NATIVE_64BIT

#else
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init( &cfg );
    esp_wifi_set_country( &wifi_country );
    esp_wifi_set_mode( WIFI_MODE_NULL ); 
    esp_wifi_start();
    esp_wifi_set_promiscuous( true );
    esp_wifi_set_promiscuous_rx_cb( &wifimon_sniffer_packet_handler );
    lv_roller_set_selected( channel_select, 0, LV_ANIM_OFF );
    wifimon_sniffer_set_channel( 1 );
#endif
    /**
     * start stats fetch task
     */
    _wifimon_app_task = lv_task_create( wifimon_app_task, 1000, LV_TASK_PRIO_MID, NULL );
    /**
     * save display timeout time
     */
    wifimon_display_timeout = display_get_timeout();
    display_set_timeout( DISPLAY_MAX_TIMEOUT );
}

static void wifimon_app_task( lv_task_t * task ) {
    /**
     * limit scale
     */
    if( mgmt > 100 ) mgmt = 100; 
    if( data > 100 ) data = 100; 
    if( misc > 100 ) misc = 100; 
    /**
     * add seria data
     */
    lv_chart_set_next(chart, ser1, mgmt);
    lv_chart_set_next(chart, ser2, data);
    lv_chart_set_next(chart, ser3, misc);
    /**
     * refresh chart
     */
    lv_chart_refresh(chart);
    /**
     * reset packet counter
     */
    data = 0;
    mgmt = 0;
    misc = 0; 
}

