/****************************************************************************
* Walking 
* 11/2020 by Neuroplant
*
*  Based on Example App 
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

#include "Walking.h"
#include "Walking_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

lv_obj_t *Walking_main_tile = NULL;
lv_style_t Walking_main_style;

lv_task_t *_Walking_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_72px);

//LV_IMG_DECLARE(Layer1);
//LV_IMG_DECLARE(Layer2);
//LV_IMG_DECLARE(Layer3);
//LV_IMG_DECLARE(Walking_8_120px);
LV_IMG_DECLARE(Aniwalk_220_640);

static lv_obj_t *app_obj = NULL;
static lv_obj_t *Layer1_obj = NULL;
static lv_obj_t *Layer2_obj = NULL;
static lv_obj_t *Layer3_obj = NULL;
static lv_obj_t *Walker_obj = NULL;

static int Offset_Layer1 = 0;
static int Offset_Layer2 = 0;
static int Offset_Layer3 = 0;
static int Offset_Walker = 0;

static void exit_Walking_main_event_cb(lv_obj_t *obj, lv_event_t event);
static void enter_Walking_setup_event_cb(lv_obj_t *obj, lv_event_t event);
static void Walking_task(lv_task_t *task);

void Walking_main_setup(uint32_t tile_num)
{

    Walking_main_tile = mainbar_get_tile_obj(tile_num);
    lv_style_copy(&Walking_main_style, mainbar_get_style());

    lv_obj_t *exit_btn = lv_imgbtn_create(Walking_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &Walking_main_style);
    lv_obj_align(exit_btn, Walking_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10);
    lv_obj_set_event_cb(exit_btn, exit_Walking_main_event_cb);

    lv_obj_t *setup_btn = lv_imgbtn_create(Walking_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &Walking_main_style);
    lv_obj_align(setup_btn, Walking_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);
    lv_obj_set_event_cb(setup_btn, enter_Walking_setup_event_cb);

    // uncomment the following block of code to remove the "myapp" label in background

 /*   app_obj = lv_obj_create(Walking_main_tile, NULL);
    lv_obj_set_size(app_obj, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(app_obj, NULL, LV_ALIGN_CENTER, 0, 0);

/*  Layer3_obj = lv_img_create(Walking_main_tile, NULL);
    lv_img_set_src(Layer3_obj,&Layer3);
    lv_obj_set_size(Layer3_obj, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(Layer3_obj, NULL, LV_ALIGN_CENTER, 0, 0);

    Layer2_obj = lv_img_create(Walking_main_tile, NULL);
    lv_img_set_src(Layer2_obj,&Layer2);
    lv_obj_set_size(Layer2_obj, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(Layer2_obj, NULL, LV_ALIGN_CENTER, 0, 0);
*/
    Walker_obj = lv_img_create(Walking_main_tile, NULL);
    lv_img_set_src(Walker_obj,&Aniwalk_220_640);
    lv_obj_set_size(Walker_obj, 125, 213);
    lv_obj_align(Walker_obj, NULL, LV_ALIGN_CENTER, 0, 0);

/*  Layer1_obj = lv_img_create(Walking_main_tile, NULL);
    lv_img_set_src(Layer1_obj,&Layer1);
    lv_obj_set_size(Layer1_obj, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(Layer1_obj, NULL, LV_ALIGN_CENTER, 0, 0);
*/  // create an task that runs every secound
    _Walking_task = lv_task_create(Walking_task, 80, LV_TASK_PRIO_MID, NULL);
}

static void enter_Walking_setup_event_cb(lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        statusbar_hide(true);
        mainbar_jump_to_tilenumber(Walking_get_app_setup_tile_num(), LV_ANIM_ON);
        break;
    }
}

static void exit_Walking_main_event_cb(lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        mainbar_jump_to_maintile(LV_ANIM_OFF);
        break;
    }
}

void Walking_task(lv_task_t *task)
{
    //Offset_Layer1 += 10;
    //Offset_Layer2 += 5;
    //Offset_Layer3 += 1;
    Offset_Walker -= 125;
 // lv_img_set_offset_x(Layer1_obj, (Offset_Layer1 / 10) % 960);
 // lv_img_set_offset_x(Layer2_obj, (Offset_Layer2 / 10) % 960);
 // lv_img_set_offset_x(Layer3_obj, (Offset_Layer3 / 10) % 960);
    lv_img_set_offset_x(Walker_obj, Offset_Walker % 1500);
    //int wait = millis()+250;		//Replace by Stepcounter
    //while (wait > millis()){};		//
}