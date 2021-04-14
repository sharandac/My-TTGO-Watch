/****************************************************************************
 *   Apr 13 14:17:11 2021
 *   Copyright  2021  Cornelius Wild
 *   Email: tt-watch-code@dervomsee.de
 *   Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  Example_App"
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

#include "gps_status.h"
#include "gps_status_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

lv_obj_t *gps_status_main_tile = NULL;
lv_style_t gps_status_main_style;
lv_style_t gps_status_value_style;

//objects
static lv_style_t style_led_green;
static lv_style_t style_led_red;
lv_obj_t *satfix_label = NULL;
lv_obj_t *satfix_value_on = NULL;
lv_obj_t *satfix_value_off = NULL;
lv_obj_t *num_satellites_label = NULL;
lv_obj_t *num_satellites_value = NULL;
lv_obj_t *pos_longlat_label = NULL;
lv_obj_t *pos_longlat_value = NULL;
lv_obj_t *altitude_label = NULL;
lv_obj_t *altitude_value = NULL;
lv_obj_t *speed_label = NULL;
lv_obj_t *speed_value = NULL;
static TTGOClass *ttgo = nullptr;
lv_task_t *_gps_status_task;

//V2 specific objects. check define to avoid compile errors on V1/V3 variants
#if defined(LILYGO_WATCH_2020_V2)
static TinyGPSPlus *gps = nullptr;
#endif

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_32px);
LV_FONT_DECLARE(Ubuntu_16px);

static void exit_gps_status_main_event_cb(lv_obj_t *obj, lv_event_t event);
static void enter_gps_status_setup_event_cb(lv_obj_t *obj, lv_event_t event);
void gps_status_task(lv_task_t *task);
void gps_status_hibernate_cb(void);
void gps_status_activate_cb(void);

void gps_status_main_setup(uint32_t tile_num)
{

    gps_status_main_tile = mainbar_get_tile_obj(tile_num);
    lv_style_copy(&gps_status_main_style, mainbar_get_style());

    lv_obj_t *exit_btn = lv_imgbtn_create(gps_status_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &gps_status_main_style);
    lv_obj_align(exit_btn, gps_status_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10);
    lv_obj_set_event_cb(exit_btn, exit_gps_status_main_event_cb);

    lv_obj_t *setup_btn = lv_imgbtn_create(gps_status_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &gps_status_main_style);
    lv_obj_align(setup_btn, gps_status_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);
    lv_obj_set_event_cb(setup_btn, enter_gps_status_setup_event_cb);
    lv_obj_set_hidden(setup_btn, true);

    lv_style_copy(&gps_status_value_style, mainbar_get_style());
    lv_style_set_bg_color(&gps_status_value_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_bg_opa(&gps_status_value_style, LV_OBJ_PART_MAIN, LV_OPA_50);
    lv_style_set_border_width(&gps_status_value_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font(&gps_status_value_style, LV_STATE_DEFAULT, &Ubuntu_16px);

    //led style
    lv_style_init(&style_led_green);
    lv_style_set_bg_color(&style_led_green, LV_STATE_DEFAULT, lv_color_hex(0x00d000));
    lv_style_set_border_color(&style_led_green, LV_STATE_DEFAULT, lv_color_hex(0x00d000));
    lv_style_set_shadow_color(&style_led_green, LV_STATE_DEFAULT, lv_color_hex(0x00d000));
    lv_style_set_shadow_spread(&style_led_green, LV_STATE_DEFAULT, 4);
    lv_style_init(&style_led_red);
    lv_style_set_bg_color(&style_led_red, LV_STATE_DEFAULT, lv_color_hex(0x900000));
    lv_style_set_border_color(&style_led_red, LV_STATE_DEFAULT, lv_color_hex(0x900000));
    lv_style_set_shadow_color(&style_led_red, LV_STATE_DEFAULT, lv_color_hex(0x900000));
    lv_style_set_shadow_spread(&style_led_red, LV_STATE_DEFAULT, 4);

//gps status
#define STATUS_HEIGHT 25
    //num satfix
    satfix_label = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(satfix_label, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(satfix_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(satfix_label, gps_status_main_tile, LV_ALIGN_IN_TOP_MID, 0, 25);
    lv_obj_t *satfix_info_label = lv_label_create(satfix_label, NULL);
    lv_obj_add_style(satfix_info_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(satfix_info_label, "SatFix");
    lv_obj_align(satfix_info_label, satfix_label, LV_ALIGN_IN_LEFT_MID, 5, 0);
    satfix_value_on = lv_led_create(satfix_label, NULL);
    lv_obj_add_style(satfix_value_on, LV_LED_PART_MAIN, &style_led_green);
    lv_obj_set_size(satfix_value_on, 15, 15);
    lv_obj_align(satfix_value_on, satfix_label, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_led_on(satfix_value_on);
    lv_obj_set_hidden(satfix_value_on, true);
    satfix_value_off = lv_led_create(satfix_label, NULL);
    lv_obj_add_style(satfix_value_off, LV_LED_PART_MAIN, &style_led_red);
    lv_obj_set_size(satfix_value_off, 15, 15);
    lv_obj_align(satfix_value_off, satfix_label, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_led_on(satfix_value_off);
    lv_obj_set_hidden(satfix_value_off, false);
    //num satellites
    num_satellites_label = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(num_satellites_label, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(num_satellites_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(num_satellites_label, satfix_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_t *num_satellites_info_label = lv_label_create(num_satellites_label, NULL);
    lv_obj_add_style(num_satellites_info_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(num_satellites_info_label, "Num satellites");
    lv_obj_align(num_satellites_info_label, num_satellites_label, LV_ALIGN_IN_LEFT_MID, 5, 0);
    num_satellites_value = lv_label_create(num_satellites_label, NULL);
    lv_obj_add_style(num_satellites_value, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(num_satellites_value, "n/a");
    lv_obj_align(num_satellites_value, num_satellites_label, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    //altitude
    altitude_label = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(altitude_label, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(altitude_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(altitude_label, num_satellites_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_t *altitude_info_label = lv_label_create(altitude_label, NULL);
    lv_obj_add_style(altitude_info_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(altitude_info_label, "Altitude");
    lv_obj_align(altitude_info_label, altitude_label, LV_ALIGN_IN_LEFT_MID, 5, 0);
    altitude_value = lv_label_create(altitude_label, NULL);
    lv_obj_add_style(altitude_value, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(altitude_value, "n/a");
    lv_obj_align(altitude_value, altitude_label, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    //long lat
    pos_longlat_label = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(pos_longlat_label, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(pos_longlat_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(pos_longlat_label, altitude_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_t *pos_longlat_info_label = lv_label_create(pos_longlat_label, NULL);
    lv_obj_add_style(pos_longlat_info_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(pos_longlat_info_label, "Long/Lat");
    lv_obj_align(pos_longlat_info_label, pos_longlat_label, LV_ALIGN_IN_LEFT_MID, 5, 0);
    pos_longlat_value = lv_label_create(pos_longlat_label, NULL);
    lv_obj_add_style(pos_longlat_value, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(pos_longlat_value, "n/a");
    lv_obj_align(pos_longlat_value, pos_longlat_label, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    //speed
    speed_label = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(speed_label, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(speed_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(speed_label, pos_longlat_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_t *speed_info_label = lv_label_create(speed_label, NULL);
    lv_obj_add_style(speed_info_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(speed_info_label, "Speed");
    lv_obj_align(speed_info_label, speed_label, LV_ALIGN_IN_LEFT_MID, 5, 0);
    speed_value = lv_label_create(speed_label, NULL);
    lv_obj_add_style(speed_value, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(speed_value, "n/a");
    lv_obj_align(speed_value, speed_label, LV_ALIGN_IN_RIGHT_MID, -5, 0);

    //init gps pointer, when on V2 variant
    ttgo = TTGOClass::getWatch();
#if defined(LILYGO_WATCH_2020_V2)
    ttgo->gps_begin();
    gps = ttgo->gps;
#endif

    //create callback
    mainbar_add_tile_activate_cb(tile_num, gps_status_activate_cb);
    mainbar_add_tile_hibernate_cb(tile_num, gps_status_hibernate_cb);
}

static void enter_gps_status_setup_event_cb(lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        statusbar_hide(true);
        mainbar_jump_to_tilenumber(gps_status_get_app_setup_tile_num(), LV_ANIM_ON);
        break;
    }
}

static void exit_gps_status_main_event_cb(lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        mainbar_jump_to_maintile(LV_ANIM_OFF);
        break;
    }
}

void gps_status_task(lv_task_t *task)
{
#if defined(LILYGO_WATCH_2020_V2)
    if (gps != nullptr)
    {
        //get/process data
        while (ttgo->hwSerial->available())
        {
            int r = ttgo->hwSerial->read();
            ttgo->gps->encode(r);
        }

        //view data
        if (gps->location.isValid())
        {
            lv_obj_set_hidden(satfix_value_on, false);
            lv_obj_set_hidden(satfix_value_off, true);
        }
        else
        {
            lv_obj_set_hidden(satfix_value_on, true);
            lv_obj_set_hidden(satfix_value_off, false);
        }
        if (gps->location.isUpdated())
        {
            char temp[20] = "";
            snprintf(temp, sizeof(temp), "%.4f/%.4f", gps->location.lat(), gps->location.lng());
            lv_label_set_text(pos_longlat_value, temp);
        }
        if (!gps->location.isValid())
        {
            lv_label_set_text(pos_longlat_value, "n/a");
        }
        if (gps->satellites.isUpdated())
        {
            char temp[10] = "";
            snprintf(temp, sizeof(temp), "%d", gps->satellites.value());
            lv_label_set_text(num_satellites_value, temp);
        }
        if (!gps->satellites.isValid())
        {
            lv_label_set_text(num_satellites_value, "n/a");
        }
        if (gps->altitude.isUpdated())
        {
            char temp[20] = "";
            snprintf(temp, sizeof(temp), "%.1fm", gps->altitude.meters());
            lv_label_set_text(altitude_value, temp);
        }
        if (!gps->altitude.isValid())
        {
            lv_label_set_text(altitude_value, "n/a");
        }
        if (gps->speed.isUpdated())
        {
            char temp[20] = "";
            snprintf(temp, sizeof(temp), "%.2fkm/h", gps->speed.kmph());
            lv_label_set_text(speed_value, temp);
        }
        if (!gps->speed.isValid())
        {
            lv_label_set_text(speed_value, "n/a");
        }
        lv_obj_align(num_satellites_value, num_satellites_label, LV_ALIGN_IN_RIGHT_MID, -5, 0);
        lv_obj_align(altitude_value, altitude_label, LV_ALIGN_IN_RIGHT_MID, -5, 0);
        lv_obj_align(pos_longlat_value, pos_longlat_label, LV_ALIGN_IN_RIGHT_MID, -5, 0);
        lv_obj_align(speed_value, speed_label, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    }
#endif
}

void gps_status_hibernate_cb(void)
{
    //del task
    lv_task_del(_gps_status_task);
    log_i("turn off GPS");
#if defined(LILYGO_WATCH_2020_V2)
    ttgo->turnOffGPS();
#endif
}

void gps_status_activate_cb(void)
{
    log_i("turn on GPS");
#if defined(LILYGO_WATCH_2020_V2)
    ttgo->trunOnGPS();
#endif

    // create an task that runs every secound
    _gps_status_task = lv_task_create(gps_status_task, 1000, LV_TASK_PRIO_MID, NULL);
}