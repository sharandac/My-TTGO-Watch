/****************************************************************************
 *  watchface_tile.cpp
 *  Neuroplant  11/2020
 *  Email: m.roych@gmail.com
 *
 *
 *  Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  note-tile"
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
#include "watchface_tile.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/time_settings/time_settings.h"

#include "hardware/timesync.h"
#include "hardware/powermgm.h"
#include "hardware/alloc.h"
void watchface_tile_setup(void)
{
	watchface_cont = mainbar_get_tile_obj(mainbar_add_tile(0, 1, "watchface tile"));
	lv_obj_set_click(watchface_cont,true);
	lv_obj_set_event_cb(watchface_cont, hide_statusbar_toggle_event_cb);

	///
	lv_style_init(&APartS.Hands_Style);
	lv_style_set_image_opa(&APartS.Hands_Style, LV_STATE_DEFAULT, LV_OPA_40);
	//Marks
	APart.Marks = lv_img_create(watchface_cont, NULL);
	lv_img_set_src(APart.Marks, &dial_silver);
	lv_obj_set_size(APart.Marks, LV_HOR_RES, LV_VER_RES);
	lv_obj_align(APart.Marks, watchface_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0);

	// Hands
	APartS.Hour = lv_img_create(APart.Marks, NULL);
	APartS.Minute = lv_img_create(APart.Marks, NULL);
	APartS.Second = lv_img_create(APart.Marks, NULL);

	APart.Hour = lv_img_create(APart.Marks, NULL);
	APart.Minute = lv_img_create(APart.Marks, NULL);
	APart.Second = lv_img_create(APart.Marks, NULL);

	// HourHand
	lv_img_set_src(APartS.Hour, &HourHand_S);
	lv_obj_add_style(APartS.Hour, LV_OBJ_PART_MAIN, &APartS.Hands_Style);
	lv_img_set_antialias(APartS.Hour, true);
	lv_img_set_pivot(APartS.Hour, 20, 8);
	lv_obj_align(APartS.Hour, APart.Marks, LV_ALIGN_IN_TOP_LEFT, (LV_HOR_RES / 2) - 20 + SHADOW_X, (LV_VER_RES / 2) - 8 + SHADOW_Y);
	lv_img_set_angle(APartS.Hour, 3300);
	// MinuteHand
	lv_img_set_src(APartS.Minute, &MinuteHand_S);
	lv_obj_add_style(APartS.Minute, LV_OBJ_PART_MAIN, &APartS.Hands_Style);
	lv_img_set_antialias(APartS.Minute, true);
	lv_img_set_pivot(APartS.Minute, 20, 8);
	lv_obj_align(APartS.Minute, APart.Marks, LV_ALIGN_IN_TOP_LEFT, (LV_HOR_RES / 2) - 20 + SHADOW_X, (LV_VER_RES / 2) - 8 + SHADOW_Y);
	lv_img_set_angle(APartS.Minute, 2100);
	// SecondHand
	lv_img_set_src(APartS.Second, &SecondHand_S);
	lv_obj_add_style(APartS.Second, LV_OBJ_PART_MAIN, &APartS.Hands_Style);
	lv_img_set_antialias(APartS.Second, true);
	lv_img_set_pivot(APartS.Second, 20, 8);
	lv_obj_align(APartS.Second, APart.Marks, LV_ALIGN_IN_TOP_LEFT, (LV_HOR_RES / 2) - 20 + SHADOW_X, (LV_VER_RES / 2) - 8 + SHADOW_Y);
	lv_img_set_angle(APartS.Second, 900);

	// HourHand
	lv_img_set_src(APart.Hour, &HourHand);
	lv_img_set_antialias(APart.Hour, true);
	lv_img_set_pivot(APart.Hour, 20, 8);
	lv_obj_align(APart.Hour, APart.Marks, LV_ALIGN_IN_TOP_LEFT, (LV_HOR_RES / 2) - 20, (LV_VER_RES / 2) - 8);
	lv_img_set_angle(APart.Hour, 3300);
	// MinuteHand
	lv_img_set_src(APart.Minute, &MinuteHand);
	lv_img_set_antialias(APart.Minute, true);
	lv_img_set_pivot(APart.Minute, 20, 8);
	lv_obj_align(APart.Minute, APart.Marks, LV_ALIGN_IN_TOP_LEFT, (LV_HOR_RES / 2) - 20, (LV_VER_RES / 2) - 8);
	lv_img_set_angle(APart.Minute, 2100);
	// SecondHand
	lv_img_set_src(APart.Second, &SecondHand);
	lv_img_set_antialias(APart.Second, true);
	lv_img_set_pivot(APart.Second, 20, 8);
	lv_obj_align(APart.Second, APart.Marks, LV_ALIGN_IN_TOP_LEFT, (LV_HOR_RES / 2) - 20, (LV_VER_RES / 2) - 8);
	lv_img_set_angle(APart.Second, 900);

	// AnimationSecond
	lv_anim_init(&Second_Ani);
	lv_anim_set_exec_cb(&Second_Ani, (lv_anim_exec_xcb_t)lv_img_set_angle);
	lv_anim_set_time(&Second_Ani, 750);

	/*Set the path of animation*/
	lv_anim_path_init(&FlowsecPath);
	lv_anim_path_set_cb(&FlowsecPath, lv_anim_path_ease_in_out);
	lv_anim_set_path(&Second_Ani, &FlowsecPath);

	watchface_tile_task = lv_task_create(watchface_tile_update_task, 1000, LV_TASK_PRIO_MID, NULL);

	powermgm_register_cb(POWERMGM_WAKEUP, watchface_tile_powermgm_event_cb, "watchface tile time update");
}

bool watchface_tile_powermgm_event_cb(EventBits_t event, void *arg)
{
	switch (event)
	{
	case POWERMGM_WAKEUP:
		watchface_tile_update_time();
		break;
	}
	return (true);
}

void watchface_tile_update_task(lv_task_t *task)
{
	watchface_tile_update_time();
}
void watchface_tile_update_time(void)
{
	tm info;
	time_t now;
	time(&now);
	localtime_r( &now, &info );

	//Angle calculation for Hands
	int Angle_S = (int)((info.tm_sec % 60) * 60) + 2700;
	int Angle_M = (int)((info.tm_min % 60) * 60) + 2700;
	int Angle_H = (int)((info.tm_hour % 24) * 300) + (int)((info.tm_min % 60) * 5) + 2700;

	while (Angle_S >= 3600)
		Angle_S = Angle_S - 3600;
	while (Angle_M >= 3600)
		Angle_M = Angle_M - 3600;
	while (Angle_H >= 3600)
		Angle_H = Angle_H - 3600;

	//Normal
	lv_img_set_angle(APart.Minute, Angle_M);
	lv_img_set_angle(APart.Hour, Angle_H);
	lv_anim_set_var(&Second_Ani, APart.Second);
	lv_anim_set_values(&Second_Ani, (Angle_S), (Angle_S + 60));
	lv_anim_start(&Second_Ani);

	//Shadow
	lv_img_set_angle(APartS.Minute, Angle_M);
	lv_img_set_angle(APartS.Hour, Angle_H);
	lv_anim_set_var(&Second_Ani, APartS.Second);
	lv_anim_set_values(&Second_Ani, (Angle_S), (Angle_S + 60));
	lv_anim_start(&Second_Ani);
}

static void hide_statusbar_toggle_event_cb(lv_obj_t *obj, lv_event_t event)
{
	switch (event)
	{
	case (LV_EVENT_CLICKED):
		lv_obj_set_hidden( statusbar, !(lv_obj_get_hidden(statusbar));
		break;
	}
}