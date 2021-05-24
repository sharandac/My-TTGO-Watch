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
#include <TTGO.h>
#include <math.h>
#include <lwip/sockets.h>

#include "esp_wifi.h"

#include "hardware/wifictl.h"

#include "wifimon_app.h"
#include "wifimon_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_styles.h"

lv_obj_t *wifimon_app_main_tile = NULL;
lv_obj_t * chart;
lv_obj_t * roller1; 
lv_chart_series_t * ser1;
lv_chart_series_t * ser2;
lv_chart_series_t * ser3;
lv_style_t wifimon_app_main_style;

lv_task_t * _wifimon_app_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(wifimon_app_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_wifimon_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_wifimon_app_next_event_cb( lv_obj_t * obj, lv_event_t event );

void wifimon_app_task( lv_task_t * task );

uint8_t level = 0, channel = 1;
int data = 0, mgmt = 0, misc = 0; 
static wifi_country_t wifi_country = {.cc="CN", .schan = 1, .nchan = 13}; 

typedef struct {
  unsigned frame_ctrl:16;
  unsigned duration_id:16;
  uint8_t addr1[6]; /* receiver address */
  uint8_t addr2[6]; /* sender address */
  uint8_t addr3[6]; /* filtering address */
  unsigned sequence_ctrl:16;
  uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
  wifi_ieee80211_mac_hdr_t hdr;
  uint8_t payload[0]; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;


void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type)
{
    switch(type) 
    {
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

void wifi_sniffer_set_channel(uint8_t channel)
{
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
         char buf[32];
         lv_roller_get_selected_str(obj, buf, sizeof(buf));
         printf("Selected: %d\n", atoi(buf));
         wifi_sniffer_set_channel(atoi(buf));
    }
}

void wifimon_app_main_setup( uint32_t tile_num ) {

    wifimon_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &wifimon_app_main_style, ws_get_mainbar_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( wifimon_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &wifimon_app_main_style );
    lv_obj_align(exit_btn, wifimon_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_wifimon_app_main_event_cb );

    lv_obj_t * next_btn = lv_imgbtn_create( wifimon_app_main_tile, NULL);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_RELEASED, &wifimon_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_PRESSED, &wifimon_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_CHECKED_RELEASED, &wifimon_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_CHECKED_PRESSED, &wifimon_app_32px);
    lv_obj_add_style(next_btn, LV_IMGBTN_PART_MAIN, &wifimon_app_main_style );
    lv_obj_align(next_btn, wifimon_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, (LV_HOR_RES / 2) -15 , -10 );
    lv_obj_set_event_cb( next_btn, enter_wifimon_app_next_event_cb );

    roller1 = lv_roller_create(wifimon_app_main_tile, NULL);
    lv_roller_set_options(roller1,
                        "1\n"
                        "2\n"
                        "3\n"
                        "4\n"
                        "5\n"
                        "6\n"
                        "7\n"
                        "8\n"
                        "9\n"
                        "10\n"
                        "11\n"
                        "12\n"
                        "13",
                        LV_ROLLER_MODE_INIFINITE);

    lv_roller_set_visible_row_count(roller1, 5);
    lv_obj_align(roller1, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_obj_set_event_cb(roller1, event_handler);

    chart = lv_chart_create(wifimon_app_main_tile, NULL);
    lv_obj_set_size(chart, 195, 190);
    lv_obj_align(chart, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   

    /*Add a faded are effect*/
    lv_obj_set_style_local_bg_opa(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_50); /*Max. opa.*/
    lv_obj_set_style_local_bg_grad_dir(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);    /*Max opa on the top*/
    lv_obj_set_style_local_bg_grad_stop(chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0);      /*Transparent on the bottom*/

    ser1 = lv_chart_add_series(chart, LV_COLOR_RED);
    ser2 = lv_chart_add_series(chart, LV_COLOR_GREEN);
    ser3 = lv_chart_add_series(chart, LV_COLOR_YELLOW);

    lv_obj_t * label1 = lv_label_create(wifimon_app_main_tile, NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_align(label1, LV_LABEL_ALIGN_RIGHT);       
    lv_label_set_text(label1, "#ff0000 - mgmt#\n#11ff00 - data#"); 
    lv_obj_set_width(label1, 150);
    lv_obj_align(label1, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, -5);
}

static void exit_wifimon_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):     if(_wifimon_app_task != NULL)
                                      {
                                           lv_task_del(_wifimon_app_task);
                                           _wifimon_app_task = NULL;
                                      }  
                                      esp_wifi_set_promiscuous(false); 
                                      wifictl_off();
                                      wifictl_on();
                                      mainbar_jump_to_maintile( LV_ANIM_OFF );
                                      break;
    }
}

static void enter_wifimon_app_next_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):     wifictl_off();
                                      wifictl_on();
                                      wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
                                      esp_wifi_init(&cfg);
                                      esp_wifi_set_country(&wifi_country);
                                      esp_wifi_set_mode(WIFI_MODE_NULL); 
                                      esp_wifi_start();
                                      esp_wifi_set_promiscuous(true);
                                      esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
                                      lv_roller_set_selected(roller1, 0, LV_ANIM_OFF);
                                      wifi_sniffer_set_channel(1);
                                      _wifimon_app_task = lv_task_create( wifimon_app_task, 400, LV_TASK_PRIO_MID, NULL );
                                      break;
    }
}

void wifimon_app_task( lv_task_t * task ) {
    if(mgmt > 100)
        mgmt = 100; 
    if(data > 100)
        data = 100; 
    if(misc > 100)
        misc = 100; 
    lv_chart_set_next(chart, ser1, mgmt);
    lv_chart_set_next(chart, ser2, data);
    lv_chart_set_next(chart, ser3, misc);
    lv_chart_refresh(chart);
    data = 0;
    mgmt = 0;
    misc = 0; 
}

