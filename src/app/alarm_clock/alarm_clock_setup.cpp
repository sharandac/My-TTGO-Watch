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

#include "alarm_clock_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "widget_factory.h"

#include "widget_factory.h"
#include "widget_styles.h"

lv_obj_t * vibe_switch = NULL;
lv_obj_t * fade_switch = NULL;
lv_obj_t * beep_switch = NULL;
lv_obj_t * main_tile_switch = NULL;

static void exit_alarm_clock_setup_event_cb( lv_obj_t * obj, lv_event_t event );

void alarm_clock_setup_setup( uint32_t tile_num ) {
    lv_obj_t *tile = mainbar_get_tile_obj( tile_num );
    lv_obj_add_style( tile, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );
    lv_obj_t *tile_container = wf_add_tile_container(tile, LV_LAYOUT_COLUMN_MID);

    lv_obj_t *exit_btn = NULL;
    wf_add_settings_header(tile_container, "Alarm Settings", &exit_btn);
    lv_obj_set_event_cb( exit_btn, exit_alarm_clock_setup_event_cb );

    wf_add_labeled_switch(tile_container, "Vibrate", &vibe_switch);
    wf_add_labeled_switch(tile_container, "Fade", &fade_switch);
    wf_add_labeled_switch(tile_container, "Beep", &beep_switch);
    wf_add_labeled_switch(tile_container, "Show on main tile", &main_tile_switch);
}

static void exit_alarm_clock_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mainbar_jump_to_tilenumber( alarm_clock_get_app_main_tile_num(), LV_ANIM_ON );
            break;
    }
}

static void set_switch_state(lv_obj_t *switch_obj, bool state){
    if (state){
        lv_switch_on(switch_obj, LV_ANIM_OFF);
    }
    else{
        lv_switch_off(switch_obj, LV_ANIM_OFF);
    }
}
void alarm_clock_setup_set_data_to_display(alarm_properties_t *alarm_properties){
    set_switch_state(vibe_switch, alarm_properties->vibe);
    set_switch_state(fade_switch, alarm_properties->fade);
    set_switch_state(beep_switch, alarm_properties->beep);
    set_switch_state(main_tile_switch, alarm_properties->show_on_main_tile);
}

alarm_properties_t *alarm_clock_setup_get_data_to_store(){
    static alarm_properties_t properties;
    properties.vibe = lv_switch_get_state(vibe_switch);
    properties.fade = lv_switch_get_state(fade_switch);
    properties.beep = lv_switch_get_state(beep_switch);
    properties.show_on_main_tile = lv_switch_get_state(main_tile_switch);
    return &properties;
}

bool alarm_clock_setup_is_main_tile_switch_on(){
    return lv_switch_get_state(main_tile_switch);
}