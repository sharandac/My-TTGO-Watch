/****************************************************************************
 *   Aug 14 12:37:31 2020
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
#include "bluetooth_FindPhone.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/sound/piep.h"
#include "hardware/blectl.h"
#include "hardware/powermgm.h"
#include "hardware/sound.h"
#include "hardware/motor.h"
#include "hardware/json_psram_allocator.h"

lv_obj_t *bluetooth_FindPhone_tile = NULL;
lv_style_t bluetooth_FindPhone_style;
uint32_t bluetooth_FindPhone_tile_num;

lv_obj_t *bluetooth_FindPhone_img = NULL;
lv_obj_t *bluetooth_FindPhone_label = NULL;

LV_IMG_DECLARE(cancel_32px);
LV_IMG_DECLARE(eye_200px);
LV_FONT_DECLARE(Ubuntu_32px);

static void exit_bluetooth_FindPhone_event_cb(lv_obj_t *obj, lv_event_t event);
bool bluetooth_FindPhone_event_cb(EventBits_t event, void *arg);
static void bluetooth_FindPhone_msg_pharse(const char *msg);

void bluetooth_FindPhone_tile_setup(void)
{
    // get an app tile and copy mainstyle
    bluetooth_FindPhone_tile_num = mainbar_add_app_tile(1, 1, "bluetooth FindPhone");
    bluetooth_FindPhone_tile = mainbar_get_tile_obj(bluetooth_FindPhone_tile_num);

    lv_style_copy(&bluetooth_FindPhone_style, mainbar_get_style());
    lv_style_set_bg_color(&bluetooth_FindPhone_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&bluetooth_FindPhone_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width(&bluetooth_FindPhone_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font(&bluetooth_FindPhone_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_style_set_text_color(&bluetooth_FindPhone_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_add_style(bluetooth_FindPhone_tile, LV_OBJ_PART_MAIN, &bluetooth_FindPhone_style);

    bluetooth_FindPhone_img = lv_img_create(bluetooth_FindPhone_tile, NULL);
    lv_img_set_src(bluetooth_FindPhone_img, &eye_200px);
    lv_obj_align(bluetooth_FindPhone_img, bluetooth_FindPhone_tile, LV_ALIGN_CENTER, 0, 0);
    bluetooth_FindPhone_label=lv_label_create(bluetooth_FindPhone_tile,NULL);
    lv_obj_add_style(bluetooth_FindPhone_label, LV_OBJ_PART_MAIN, &bluetooth_FindPhone_style);
    lv_obj_align(bluetooth_FindPhone_label, bluetooth_FindPhone_tile, LV_ALIGN_IN_LEFT_MID, 5, 0);

    lv_obj_t *exit_btn = lv_imgbtn_create(bluetooth_FindPhone_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &cancel_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &cancel_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &cancel_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &cancel_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &bluetooth_FindPhone_style);
    lv_obj_align(exit_btn, bluetooth_FindPhone_tile, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
    lv_obj_set_event_cb(exit_btn, exit_bluetooth_FindPhone_event_cb);

    blectl_register_cb(BLECTL_MSG, bluetooth_FindPhone_event_cb, "bluetooth_FindPhone");
}

bool bluetooth_FindPhone_event_cb(EventBits_t event, void *arg)
{
    switch (event)
    {
    case BLECTL_MSG:
        bluetooth_FindPhone_msg_pharse((const char *)arg);
        break;
    }
    return (true);
}

static void exit_bluetooth_FindPhone_event_cb(lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        mainbar_jump_to_maintile(LV_ANIM_OFF);
        break;
    }
}

void bluetooth_FindPhone_msg_pharse(const char *msg)
{
    static bool standby = false;

    SpiRamJsonDocument doc(strlen(msg) * 4);

    DeserializationError error = deserializeJson(doc, msg);
    if (error)
    {
        log_e("bluetooth FindPhone deserializeJson() failed: %s", error.c_str());
    }
    else
    {
        if ( !strcmp( doc["t"], "find" )  )
		{
            statusbar_hide(true);
            if (powermgm_get_event(POWERMGM_STANDBY))
            {
                standby = true;
            }
            else
            {
                standby = false;
            }
            powermgm_set_event(POWERMGM_WAKEUP_REQUEST);
            mainbar_jump_to_tilenumber(bluetooth_FindPhone_tile_num, LV_ANIM_OFF);
            lv_label_set_text(bluetooth_FindPhone_label, "Looking for me?");
            sound_play_progmem_wav( piep_wav, piep_wav_len );
            lv_obj_invalidate(lv_scr_act());
            motor_vibe(100);            
        }
    }
    doc.clear();
}
