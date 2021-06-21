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

#include "wireless_app.h"
#include "wireless_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_styles.h"

lv_obj_t *wireless_app_main_tile = NULL;
lv_style_t wireless_app_main_style;
lv_obj_t * throbber = NULL; 

lv_task_t * _wireless_app_task;

LV_IMG_DECLARE(wireless_app_32px);
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_wireless_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_wireless_app_next_event_cb( lv_obj_t * obj, lv_event_t event );
void wireless_app_task( lv_task_t * task );

esp_err_t esp_wifi_80211_tx(wifi_interface_t ifx, const void *buffer, int len, bool en_sys_seq);
esp_err_t esp_wifi_internal_tx(wifi_interface_t ifx, const void *buffer, int len);

uint8_t beacon_raw[] = {
	0x80, 0x00,							// 0-1: Frame Control
	0x00, 0x00,							// 2-3: Duration
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff,				// 4-9: Destination address (broadcast)
	0xba, 0xde, 0xaf, 0xfe, 0x00, 0x06,				// 10-15: Source address
	0xba, 0xde, 0xaf, 0xfe, 0x00, 0x06,				// 16-21: BSSID
	0x00, 0x00,							// 22-23: Sequence / fragment number
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,			// 24-31: Timestamp (GETS OVERWRITTEN TO 0 BY HARDWARE)
	0x64, 0x00,							// 32-33: Beacon interval
	0x31, 0x04,							// 34-35: Capability info
	0x00, 0x00, /* FILL CONTENT HERE */				// 36-38: SSID parameter set, 0x00:length:content
	0x01, 0x08, 0x82, 0x84,	0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24,	// 39-48: Supported rates
	0x03, 0x01, 0x01,						// 49-51: DS Parameter set, current channel 1 (= 0x01),
	0x05, 0x04, 0x01, 0x02, 0x00, 0x00,				// 52-57: Traffic Indication Map	
};

char *rick_ssids[] = {
	"01 Never gonna give you up",
	"02 Never gonna let you down",
	"03 Never gonna run around",
	"04 and desert you",
	"05 Never gonna make you cry",
	"06 Never gonna say goodbye",
	"07 Never gonna tell a lie",
	"08 and hurt you"
};

#define BEACON_SSID_OFFSET 38
#define SRCADDR_OFFSET 10
#define BSSID_OFFSET 16
#define SEQNUM_OFFSET 22
#define TOTAL_LINES (sizeof(rick_ssids) / sizeof(char *))

esp_err_t event_handler(void *ctx, system_event_t *event) {
	return ESP_OK;
}

void wireless_app_main_setup( uint32_t tile_num ) {

    wireless_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &wireless_app_main_style, ws_get_mainbar_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( wireless_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &wireless_app_main_style );
    lv_obj_align(exit_btn, wireless_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_wireless_app_main_event_cb );

    lv_obj_t * next_btn = lv_imgbtn_create( wireless_app_main_tile, NULL);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_RELEASED, &wireless_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_PRESSED, &wireless_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_CHECKED_RELEASED, &wireless_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_CHECKED_PRESSED, &wireless_app_32px);
    lv_obj_add_style(next_btn, LV_IMGBTN_PART_MAIN, &wireless_app_main_style );
    lv_obj_align(next_btn, wireless_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, (LV_HOR_RES / 2) -5 , -10 );
    lv_obj_set_event_cb( next_btn, enter_wireless_app_next_event_cb );
}

void spam_task(void *pvParameter) {
	uint8_t line = 0;
                   int x;

	// Keep track of beacon sequence numbers on a per-songline-basis
	uint16_t seqnum[TOTAL_LINES] = { 0 };

	for (x=0;x<666;x++) 
        {
		vTaskDelay(10);

		// Insert line of Rick Astley's "Never Gonna Give You Up" into beacon packet
		log_i("%i %i %s", strlen(rick_ssids[line]), TOTAL_LINES, rick_ssids[line]);

		uint8_t beacon_rick[200];
		memcpy(beacon_rick, beacon_raw, BEACON_SSID_OFFSET - 1);
		beacon_rick[BEACON_SSID_OFFSET - 1] = strlen(rick_ssids[line]);
		memcpy(&beacon_rick[BEACON_SSID_OFFSET], rick_ssids[line], strlen(rick_ssids[line]));
		memcpy(&beacon_rick[BEACON_SSID_OFFSET + strlen(rick_ssids[line])], &beacon_raw[BEACON_SSID_OFFSET], sizeof(beacon_raw) - BEACON_SSID_OFFSET);

		// Last byte of source address / BSSID will be line number - emulate multiple APs broadcasting one song line each
		beacon_rick[SRCADDR_OFFSET + 5] = line;
		beacon_rick[BSSID_OFFSET + 5] = line;

		// Update sequence number
		beacon_rick[SEQNUM_OFFSET] = (seqnum[line] & 0x0f) << 4;
		beacon_rick[SEQNUM_OFFSET + 1] = (seqnum[line] & 0xff0) >> 4;
		seqnum[line]++;
		if (seqnum[line] > 0xfff)
			seqnum[line] = 0;

		esp_wifi_80211_tx(WIFI_IF_AP, beacon_rick, sizeof(beacon_raw) + strlen(rick_ssids[line]), false);

		if (++line >= TOTAL_LINES)
			line = 0;
	}
        //ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        lv_obj_del(throbber);
        vTaskDelete(NULL);
}

static void enter_wireless_app_next_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {	
                case(LV_EVENT_CLICKED):

                    throbber = lv_spinner_create(lv_scr_act(), NULL);
                    lv_obj_set_size(throbber, 100, 100);
                    lv_obj_align(throbber, NULL, LV_ALIGN_CENTER, 0, 0);
	            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
	            wifi_config_t ap_config = { }; 
		    strcpy((char *)ap_config.ap.ssid, "23pse"); 
                    ap_config.ap.ssid_len = 0;
                    strcpy((char *)ap_config.ap.password, "00112233440");
	            ap_config.ap.channel = 1;
	            ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
	            ap_config.ap.ssid_hidden = 1;
	            ap_config.ap.max_connection = 1;
	            ap_config.ap.beacon_interval = 60000;
	            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
	            ESP_ERROR_CHECK(esp_wifi_start());
				ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_MIN_MODEM));
	            xTaskCreate(&spam_task, "spam_task", 4096, NULL, 5, NULL);
                    break;
    }
}

static void exit_wireless_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
										ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_MAX_MODEM));
                                        break;
    }
}

