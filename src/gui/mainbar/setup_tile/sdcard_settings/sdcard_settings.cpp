/****************************************************************************
 *   Apr 23 14:17:11 2021
 *   Copyright  2021  Cornelius Wild
 *   Email: tt-watch-code@dervomsee.de
 *   Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch"
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
#include "sdcard_settings.h"
#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include "utils/webserver/webserver.h"
    #include <SD.h>
    #include <SPIFFS.h>
    #include <Arduino.h>
#endif

lv_obj_t *sdcard_settings_tile = NULL;
lv_style_t sdcard_settings_style;
uint32_t sdcard_settings_tile_num;
lv_obj_t *sdcard_fs_browser_onoff = NULL;

LV_IMG_DECLARE(sdcard_settings_64px);

static void enter_sdcard_settings_event_cb(lv_obj_t *obj, lv_event_t event);
static void exit_sdcard_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void sdcard_fs_browser_onoff_event_handler(lv_obj_t *obj, lv_event_t event);

void sdcard_settings_tile_setup(void)
{
    // get an app tile and copy mainstyle
    sdcard_settings_tile_num = mainbar_add_setup_tile(1, 1, "SD card setup");
    sdcard_settings_tile = mainbar_get_tile_obj(sdcard_settings_tile_num);
    lv_style_copy(&sdcard_settings_style, ws_get_setup_tile_style());
    lv_obj_add_style(sdcard_settings_tile, LV_OBJ_PART_MAIN, &sdcard_settings_style);

    icon_t *utilities_setup_icon = setup_register("SD card", &sdcard_settings_64px, enter_sdcard_settings_event_cb);
    setup_hide_indicator(utilities_setup_icon);

    lv_obj_t *header = wf_add_settings_header( sdcard_settings_tile, "SD card setup", exit_sdcard_setup_event_cb );
    lv_obj_align(header, sdcard_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10);

    lv_obj_t *sdcard_fs_browser_enable_cont = wf_add_labeled_switch( sdcard_settings_tile,
                                                                     "Enable SD card in \nFS browser", 
                                                                     &sdcard_fs_browser_onoff, 
                                                                     0, 
                                                                     sdcard_fs_browser_onoff_event_handler,
                                                                     ws_get_setup_tile_style() );
                                                                     
    lv_obj_align( sdcard_fs_browser_enable_cont, header, LV_ALIGN_OUT_BOTTOM_MID, -5, 10 );

    lv_obj_t *sd_infos_cont = lv_obj_create(sdcard_settings_tile, NULL);
    lv_obj_set_size(sd_infos_cont, lv_disp_get_hor_res(NULL), 15);
    lv_obj_add_style(sd_infos_cont, LV_OBJ_PART_MAIN, &sdcard_settings_style);
    lv_obj_align(sd_infos_cont, sdcard_fs_browser_enable_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_t *sd_infos_label = lv_label_create(sd_infos_cont, NULL);
    lv_obj_add_style(sd_infos_label, LV_OBJ_PART_MAIN, &sdcard_settings_style);
    lv_label_set_text(sd_infos_label, "SD information");
    lv_obj_align(sd_infos_label, sd_infos_cont, LV_ALIGN_IN_LEFT_MID, 15, 0);

    lv_obj_t *sd_type_cont = lv_obj_create(sdcard_settings_tile, NULL);
    lv_obj_set_size(sd_type_cont, lv_disp_get_hor_res(NULL), 15);
    lv_obj_add_style(sd_type_cont, LV_OBJ_PART_MAIN, &sdcard_settings_style);
    lv_obj_align(sd_type_cont, sd_infos_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_t *sd_type_label = lv_label_create(sd_type_cont, NULL);
    lv_obj_add_style(sd_type_label, LV_OBJ_PART_MAIN, &sdcard_settings_style);
    lv_label_set_text(sd_type_label, "SD card type:");
    lv_obj_align(sd_type_label, sd_type_cont, LV_ALIGN_IN_LEFT_MID, 15, 0);

    lv_obj_t *sd_size_cont = lv_obj_create(sdcard_settings_tile, NULL);
    lv_obj_set_size(sd_size_cont, lv_disp_get_hor_res(NULL), 15);
    lv_obj_add_style(sd_size_cont, LV_OBJ_PART_MAIN, &sdcard_settings_style);
    lv_obj_align(sd_size_cont, sd_type_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_t *sd_size_label = lv_label_create(sd_size_cont, NULL);
    lv_obj_add_style(sd_size_label, LV_OBJ_PART_MAIN, &sdcard_settings_style);
    lv_label_set_text(sd_size_label, "SD card size:");
    lv_obj_align(sd_size_label, sd_size_cont, LV_ALIGN_IN_LEFT_MID, 15, 0);

#ifdef LILYGO_WATCH_HAS_SDCARD
    //get SD type
    uint8_t cardType = SD.cardType();
    String sdCardType;
    switch (cardType)
    {
    case CARD_NONE:
        sdCardType = "No SD card";
        break;
    case CARD_MMC:
        sdCardType = "MMC";
        break;
    case CARD_SD:
        sdCardType = "SDSC";
        break;
    case CARD_SDHC:
        sdCardType = "SDHC";
        break;
    default:
        sdCardType = "UNKNOWN";
    }
    lv_obj_t *sd_type_val = lv_label_create(sd_type_cont, NULL);
    lv_obj_add_style(sd_type_val, LV_OBJ_PART_MAIN, &sdcard_settings_style);
    lv_label_set_text(sd_type_val, sdCardType.c_str());
    lv_obj_align(sd_type_val, sd_type_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);

    //get SD size
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    char sdCardSize[25]; //enough to hold all numbers up to 64-bits
    sprintf(sdCardSize, "%lluMB\n", cardSize);
    lv_obj_t *sd_size_val = lv_label_create(sd_size_cont, NULL);
    lv_obj_add_style(sd_size_val, LV_OBJ_PART_MAIN, &sdcard_settings_style);
    lv_label_set_text(sd_size_val, sdCardSize);
    lv_obj_align(sd_size_val, sd_size_cont, LV_ALIGN_IN_RIGHT_MID, -5, 8);
#endif

}

static void enter_sdcard_settings_event_cb(lv_obj_t *obj, lv_event_t event) {
    switch (event) {
        case (LV_EVENT_CLICKED):
            mainbar_jump_to_tilenumber( sdcard_settings_tile_num, LV_ANIM_OFF );
            break;
    }
}

static void exit_sdcard_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}

static void sdcard_fs_browser_onoff_event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        if (lv_switch_get_state(obj))
        {
#ifdef LILYGO_WATCH_HAS_SDCARD
            log_d("enable SD");
            setFsEditorFilesystem(SD);
#endif
        }
        else
        {
#ifndef NATIVE_64BIT
            log_d("enable SPIFFS");
            setFsEditorFilesystem(SPIFFS);
#endif
        }
    }
}