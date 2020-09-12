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

#include "alarm_data.h"
#include "widget_factory.h"
#include "widget_styles.h"
#include "hardware/timesync.h"

#define AM "AM"
#define PM "PM"

lv_obj_t *alarm_enabled_switch = NULL;
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);


static bool clock_24 = false;
static lv_obj_t *hour_roller = NULL;
static lv_obj_t *minute_roller = NULL;

static void exit_alarm_clock_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_alarm_clock_setup_event_cb( lv_obj_t * obj, lv_event_t event );

int alarm_clock_main_get_am_pm_hour(int hour24){
    //FIXME: taken from main_tile.cpp. It would be good to place common function somewhere and use it on both places
    if (hour24 == 0){
        return 12;
    }
    if (hour24 > 12){
        return hour24 - 12;
    }
    return hour24;
}

char* alarm_clock_main_get_am_pm_value(int hour24){
    if (hour24 < 12){
        return (char*)AM;
    }

    return (char*)PM;
}

static char* get_roller_content(int count, bool zeros, bool am_pm_roller){
    static char content[60 * 3]; //max(60 sec * 2 digits, 24 * (2 + 1 + AM||PM) ) + \n on each line (on last line is \0 instead)
    int pos = 0;
    for (int index = 0; index < count; ++index){
        int number = index;
        if (am_pm_roller){
            number = alarm_clock_main_get_am_pm_hour(number);
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
            char *am_pm_value = alarm_clock_main_get_am_pm_value(index);
            strcpy(content + pos, am_pm_value);
            pos += strlen(am_pm_value);
        }

        content[pos++] = (index == count - 1 ? '\0' : '\n');
    }
    return content;
}

static void tile_activate_callback (){
    alarm_clock_set_gui_values();
}

static void tile_hibernate_callback (){
    if (alarm_is_enabled()){
        alarm_set_enabled(false); //to will not be alarm started before everything is stored (there was an writing issue sometimes)
    }
    alarm_set_term(lv_roller_get_selected(hour_roller), lv_roller_get_selected(minute_roller));
    alarm_data_store_data(lv_switch_get_state(alarm_enabled_switch));
    alarm_set_enabled(lv_switch_get_state(alarm_enabled_switch));
}

void alarm_clock_main_setup( uint32_t tile_num ) {
    lv_obj_t * main_tile = mainbar_get_tile_obj( tile_num );
    mainbar_add_tile_activate_cb(tile_num, tile_activate_callback);
    mainbar_add_tile_hibernate_cb(tile_num, tile_hibernate_callback);

    clock_24 = timesync_get_24hr();
    lv_obj_t * labeled_alarm_switch  = wf_add_labeled_switch(
        main_tile, "Activated", main_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10, &alarm_enabled_switch
    );

    int cont_width = lv_disp_get_hor_res( NULL ) - 20;
    int cont_height = lv_disp_get_ver_res( NULL ) - 32 - 30 - 40;
    lv_obj_t *alarm_clock_roller_cont = wf_add_container(
        main_tile, labeled_alarm_switch, LV_ALIGN_OUT_BOTTOM_MID, 0, 10, cont_width , cont_height
    );
    hour_roller = wf_add_roller(
        main_tile,
        get_roller_content(24, false, !clock_24),
        LV_ROLLER_MODE_INIFINITE,
        alarm_clock_roller_cont,
        LV_ALIGN_IN_LEFT_MID
    );

    wf_add_label(main_tile, ":", alarm_clock_roller_cont, LV_ALIGN_CENTER, 0, 0);
    minute_roller = wf_add_roller(
        main_tile,
        get_roller_content(60, true, false),
        LV_ROLLER_MODE_INIFINITE,
        alarm_clock_roller_cont,
        LV_ALIGN_IN_RIGHT_MID
    );

    wf_add_image_button(
        main_tile, exit_32px, main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10, exit_alarm_clock_main_event_cb
    );

    wf_add_image_button(
        main_tile, setup_32px, main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10, enter_alarm_clock_setup_event_cb
    );
}

void alarm_clock_set_gui_values(){
    //for the case that time format was changed in setiings dialog
    if (clock_24 != timesync_get_24hr()){
        clock_24 = !clock_24;
        lv_roller_set_options(hour_roller, get_roller_content(24, false, !clock_24) , LV_ROLLER_MODE_INIFINITE);
    }
    lv_roller_set_selected(hour_roller, alarm_get_hour(), LV_ANIM_OFF);
    lv_roller_set_selected(minute_roller, alarm_get_minute(), LV_ANIM_OFF);

    if (alarm_is_enabled()){
        lv_switch_on(alarm_enabled_switch, LV_ANIM_OFF);
    }
    else{
        lv_switch_off(alarm_enabled_switch, LV_ANIM_OFF);
    }
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
