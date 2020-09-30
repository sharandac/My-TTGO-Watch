/****************************************************************************
 *   Copyright  2020  Jakub Vesely
 *   Email: jakub_vesely@seznam.cz
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

#include "alarm_clock.h"
#include "alarm_clock_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

#include "widget_factory.h"
#include "widget_styles.h"
#include "hardware/rtcctl.h"

lv_obj_t *alarm_enabled_switch = NULL;
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);

#define ROLLER_ROW_COUNT 4

static bool clock_format_24 = false;
static lv_obj_t *hour_roller = NULL;
static lv_obj_t *minute_roller = NULL;
static lv_obj_t *monday_btn = NULL;
static lv_obj_t *tuesday_btn = NULL;
static lv_obj_t *wednesday_btn = NULL;
static lv_obj_t *thursday_btn = NULL;
static lv_obj_t *friday_btn = NULL;
static lv_obj_t *saturday_btn = NULL;
static lv_obj_t *sunday_btn = NULL;

static void exit_alarm_clock_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_alarm_clock_setup_event_cb( lv_obj_t * obj, lv_event_t event );

static char* get_roller_content(int count, bool zeros, bool am_pm_roller){
    static char content[60 * 3]; //max(60 sec * 2 digits, 24 * (2 + 1 + AM||PM) ) + \n on each line (on last line is \0 instead)
    int pos = 0;
    for (int index = 0; index < count; ++index){
        int number = index;
        if (am_pm_roller){
            number = alarm_clock_get_am_pm_hour(number);
        }

        if (number < 10){
            if (zeros){
                content[pos++] = '0';
            }
        }
        else{
            content[pos++] = '0' + number / 10;
        }

        content[pos++] = '0' + number % 10;
        if (am_pm_roller){
            content[pos++] = ' ';
            char const *am_pm_value = alarm_clock_get_am_pm_value(index, false);
            strcpy(content + pos, am_pm_value);
            pos += strlen(am_pm_value);
        }

        content[pos++] = (index == count - 1 ? '\0' : '\n');
    }
    return content;
}

void alarm_clock_main_setup( uint32_t tile_num ) {
    lv_obj_t * main_tile = mainbar_get_tile_obj( tile_num );
    

    lv_obj_t * tile_container = wf_add_tile_container(main_tile, LV_LAYOUT_COLUMN_MID);
    lv_obj_t * footer_container = wf_add_tile_footer_container(main_tile, LV_LAYOUT_PRETTY_TOP);

    wf_add_labeled_switch(tile_container, "Activated", &alarm_enabled_switch);

    lv_obj_t * weekday_container = wf_add_container(tile_container, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT);
    lv_obj_set_style_local_pad_inner( weekday_container, LV_CONT_PART_MAIN , LV_STATE_DEFAULT, 2);

    static const int day_btn_width = 30;
    static const int day_btn_height = 29;
    monday_btn = wf_add_button(weekday_container, alarm_clock_get_week_day(1, true), day_btn_width, day_btn_height, NULL);
    tuesday_btn = wf_add_button(weekday_container, alarm_clock_get_week_day(2, true), day_btn_width, day_btn_height, NULL);
    wednesday_btn = wf_add_button(weekday_container, alarm_clock_get_week_day(3, true), day_btn_width, day_btn_height, NULL);
    thursday_btn = wf_add_button(weekday_container, alarm_clock_get_week_day(4, true), day_btn_width, day_btn_height, NULL);
    friday_btn = wf_add_button(weekday_container, alarm_clock_get_week_day(5, true), day_btn_width, day_btn_height, NULL);
    saturday_btn = wf_add_button(weekday_container, alarm_clock_get_week_day(6, true), day_btn_width, day_btn_height, NULL);
    sunday_btn = wf_add_button(weekday_container, alarm_clock_get_week_day(0, true), day_btn_width, day_btn_height, NULL);

    lv_obj_t *roller_container = wf_add_container(tile_container, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT);
    lv_obj_set_style_local_pad_left( roller_container, LV_CONT_PART_MAIN , LV_STATE_DEFAULT, 10);
    lv_obj_set_style_local_pad_right( roller_container, LV_CONT_PART_MAIN , LV_STATE_DEFAULT, 10);

    hour_roller = wf_add_roller(
        roller_container, get_roller_content(24, false, !clock_format_24), LV_ROLLER_MODE_INIFINITE, ROLLER_ROW_COUNT
    );
    lv_obj_set_width(hour_roller, 90);
    wf_add_label(roller_container, ":");
    minute_roller = wf_add_roller(
        roller_container, get_roller_content(60, true, false), LV_ROLLER_MODE_INIFINITE, ROLLER_ROW_COUNT
    );
    lv_obj_set_width(minute_roller, 90);

    wf_add_image_button(footer_container, exit_32px, exit_alarm_clock_main_event_cb);
    wf_add_image_button(footer_container, setup_32px, enter_alarm_clock_setup_event_cb);
}

void alarm_clock_main_set_data_to_display(rtcctl_alarm_t *alarm_data, bool clock_24){
    if (clock_format_24 != clock_24){
        clock_format_24 = clock_24;
        lv_roller_set_options(hour_roller, get_roller_content(24, false, !clock_24) , LV_ROLLER_MODE_INIFINITE);
    }
    lv_roller_set_selected(hour_roller, alarm_data->hour, LV_ANIM_OFF);
    lv_roller_set_selected(minute_roller, alarm_data->minute, LV_ANIM_OFF);

    if (alarm_data->enabled){
        lv_switch_on(alarm_enabled_switch, LV_ANIM_OFF);
    }
    else{
        lv_switch_off(alarm_enabled_switch, LV_ANIM_OFF);
    }
    lv_btn_set_state(sunday_btn, alarm_data->week_days[0] ? LV_BTN_STATE_CHECKED_RELEASED : LV_BTN_STATE_RELEASED);
    lv_btn_set_state(monday_btn, alarm_data->week_days[1] ? LV_BTN_STATE_CHECKED_RELEASED : LV_BTN_STATE_RELEASED);
    lv_btn_set_state(tuesday_btn, alarm_data->week_days[2] ? LV_BTN_STATE_CHECKED_RELEASED : LV_BTN_STATE_RELEASED);
    lv_btn_set_state(wednesday_btn, alarm_data->week_days[3] ? LV_BTN_STATE_CHECKED_RELEASED : LV_BTN_STATE_RELEASED);
    lv_btn_set_state(thursday_btn, alarm_data->week_days[4] ? LV_BTN_STATE_CHECKED_RELEASED : LV_BTN_STATE_RELEASED);
    lv_btn_set_state(friday_btn, alarm_data->week_days[5] ? LV_BTN_STATE_CHECKED_RELEASED : LV_BTN_STATE_RELEASED);
    lv_btn_set_state(saturday_btn, alarm_data->week_days[6] ? LV_BTN_STATE_CHECKED_RELEASED : LV_BTN_STATE_RELEASED);
}

rtcctl_alarm_t *alarm_clock_main_get_data_to_store(){
    static rtcctl_alarm_t data;
    data.enabled = lv_switch_get_state(alarm_enabled_switch);
    data.hour = lv_roller_get_selected(hour_roller);
    data.minute = lv_roller_get_selected(minute_roller);
    data.week_days[0] = (lv_btn_get_state(sunday_btn) == LV_BTN_STATE_CHECKED_RELEASED);
    data.week_days[1] = (lv_btn_get_state(monday_btn) == LV_BTN_STATE_CHECKED_RELEASED);
    data.week_days[2] = (lv_btn_get_state(tuesday_btn) == LV_BTN_STATE_CHECKED_RELEASED);
    data.week_days[3] = (lv_btn_get_state(wednesday_btn) == LV_BTN_STATE_CHECKED_RELEASED);
    data.week_days[4] = (lv_btn_get_state(thursday_btn) == LV_BTN_STATE_CHECKED_RELEASED);
    data.week_days[5] = (lv_btn_get_state(friday_btn) == LV_BTN_STATE_CHECKED_RELEASED);
    data.week_days[6] = (lv_btn_get_state(saturday_btn) == LV_BTN_STATE_CHECKED_RELEASED);
    
    return &data;
}

static void enter_alarm_clock_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            statusbar_hide( true );
            mainbar_jump_to_tilenumber( alarm_clock_get_app_setup_tile_num(), LV_ANIM_ON );
            break;
    }
}

static void exit_alarm_clock_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mainbar_jump_to_maintile( LV_ANIM_OFF ); // user action (return back) will be performed first
            break;
    }
}
