/****************************************************************************
 *   Tu May 22 21:23:51 2020
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
#include "battery_settings.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "hardware/pmu.h"
#include "hardware/motor.h"

lv_obj_t *battery_view_tile=NULL;
lv_style_t battery_view_style;
uint32_t battery_view_tile_num;

lv_obj_t *battery_view_design_cap;
lv_obj_t *battery_view_current_cap;
lv_obj_t *battery_view_voltage;
lv_obj_t *charge_view_current;
lv_obj_t *discharge_view_current;
lv_obj_t *vbus_view_voltage;
lv_task_t *battery_view_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);

static void enter_battery_settings_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_battery_view_event_cb( lv_obj_t * obj, lv_event_t event );
void battery_view_update_task( lv_task_t *task );
void battery_activate_cb( void );
void battery_hibernate_cb( void );

void battery_view_tile_setup( uint32_t tile_num ) {
    // get an app tile and copy mainstyle
    battery_view_tile_num = tile_num;
    battery_view_tile = mainbar_get_tile_obj( battery_view_tile_num );

    lv_style_copy( &battery_view_style, mainbar_get_style() );
    lv_style_set_bg_color( &battery_view_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &battery_view_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &battery_view_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( battery_view_tile, LV_OBJ_PART_MAIN, &battery_view_style );

    // create the battery settings */
    lv_obj_t *exit_btn = lv_imgbtn_create( battery_view_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &battery_view_style );
    lv_obj_align( exit_btn, battery_view_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_battery_view_event_cb );
    
    // create the battery settings */
    lv_obj_t *setup_btn = lv_imgbtn_create( battery_view_tile, NULL);
    lv_imgbtn_set_src( setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src( setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src( setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src( setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style( setup_btn, LV_IMGBTN_PART_MAIN, &battery_view_style );
    lv_obj_align( setup_btn, battery_view_tile, LV_ALIGN_IN_TOP_RIGHT, -10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( setup_btn, enter_battery_settings_event_cb );

    lv_obj_t *exit_label = lv_label_create( battery_view_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &battery_view_style );
    lv_label_set_text( exit_label, "battery / energy");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *battery_design_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( battery_design_cont, lv_disp_get_hor_res( NULL ) , 25 );
    lv_obj_add_style( battery_design_cont, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_obj_align( battery_design_cont, battery_view_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    lv_obj_t *battery_design_cap_label = lv_label_create( battery_design_cont, NULL);
    lv_obj_add_style( battery_design_cap_label, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( battery_design_cap_label, "designed cap");
    lv_obj_align( battery_design_cap_label, battery_design_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    battery_view_design_cap = lv_label_create( battery_design_cont, NULL);
    lv_obj_add_style( battery_view_design_cap, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( battery_view_design_cap, "300mAh");
    lv_obj_align( battery_view_design_cap, battery_design_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *battery_current_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( battery_current_cont, lv_disp_get_hor_res( NULL ) , 22 );
    lv_obj_add_style( battery_current_cont, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_obj_align( battery_current_cont, battery_design_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_current_cap_label = lv_label_create( battery_current_cont, NULL);
    lv_obj_add_style( battery_current_cap_label, LV_OBJ_PART_MAIN, &battery_view_style );
    lv_label_set_text( battery_current_cap_label, "charged capacity");
    lv_obj_align( battery_current_cap_label, battery_current_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    battery_view_current_cap = lv_label_create( battery_current_cont, NULL);
    lv_obj_add_style( battery_view_current_cap, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( battery_view_current_cap, "300mAh");
    lv_obj_align( battery_view_current_cap, battery_current_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *battery_voltage_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( battery_voltage_cont, lv_disp_get_hor_res( NULL ) , 22 );
    lv_obj_add_style( battery_voltage_cont, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_obj_align( battery_voltage_cont, battery_current_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_voltage_label = lv_label_create( battery_voltage_cont, NULL);
    lv_obj_add_style( battery_voltage_label, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( battery_voltage_label, "battery voltage");
    lv_obj_align( battery_voltage_label, battery_voltage_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    battery_view_voltage = lv_label_create( battery_voltage_cont, NULL);
    lv_obj_add_style( battery_view_voltage, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( battery_view_voltage, "2.4mV");
    lv_obj_align( battery_view_voltage, battery_voltage_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *battery_charge_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( battery_charge_cont, lv_disp_get_hor_res( NULL ) , 22 );
    lv_obj_add_style( battery_charge_cont, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_obj_align( battery_charge_cont, battery_voltage_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_charge_label = lv_label_create( battery_charge_cont, NULL);
    lv_obj_add_style( battery_charge_label, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( battery_charge_label, "charge current");
    lv_obj_align( battery_charge_label, battery_charge_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    charge_view_current = lv_label_create( battery_charge_cont, NULL);
    lv_obj_add_style( charge_view_current, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( charge_view_current, "100mA");
    lv_obj_align( charge_view_current, battery_charge_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *battery_discharge_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( battery_discharge_cont, lv_disp_get_hor_res( NULL ) , 22 );
    lv_obj_add_style( battery_discharge_cont, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_obj_align( battery_discharge_cont, battery_charge_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_discharge_label = lv_label_create( battery_discharge_cont, NULL);
    lv_obj_add_style( battery_discharge_label, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( battery_discharge_label, "discharge current");
    lv_obj_align( battery_discharge_label, battery_discharge_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    discharge_view_current = lv_label_create( battery_discharge_cont, NULL);
    lv_obj_add_style( discharge_view_current, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( discharge_view_current, "100mA");
    lv_obj_align( discharge_view_current, battery_discharge_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *vbus_voltage_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( vbus_voltage_cont, lv_disp_get_hor_res( NULL ) , 22 );
    lv_obj_add_style( vbus_voltage_cont, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_obj_align( vbus_voltage_cont, battery_discharge_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *vbus_voltage_label = lv_label_create( vbus_voltage_cont, NULL);
    lv_obj_add_style( vbus_voltage_label, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( vbus_voltage_label, "VBUS voltage");
    lv_obj_align( vbus_voltage_label, vbus_voltage_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    vbus_view_voltage = lv_label_create( vbus_voltage_cont, NULL);
    lv_obj_add_style( vbus_view_voltage, LV_OBJ_PART_MAIN, &battery_view_style  );
    lv_label_set_text( vbus_view_voltage, "2.4mV");
    lv_obj_align( vbus_view_voltage, vbus_voltage_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    mainbar_add_tile_activate_cb( battery_view_tile_num, battery_activate_cb );
    mainbar_add_tile_activate_cb( battery_view_tile_num + 1, battery_activate_cb );
    mainbar_add_tile_hibernate_cb( battery_view_tile_num, battery_hibernate_cb );
    mainbar_add_tile_hibernate_cb( battery_view_tile_num + 1, battery_hibernate_cb );
}

void battery_activate_cb( void ) {
    battery_view_task = lv_task_create(battery_view_update_task, 1000,  LV_TASK_PRIO_LOWEST, NULL );
}

void battery_hibernate_cb( void ) {
    lv_task_del( battery_view_task );
}

static void enter_battery_settings_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( battery_view_tile_num + 1, LV_ANIM_OFF );
                                        break;
    }

}

static void exit_battery_view_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( setup_get_tile_num() , LV_ANIM_OFF );
                                        break;
    }
}


void battery_view_update_task( lv_task_t *task ) {
    char temp[16]="";

    if ( pmu_get_battery_percent( ) >= 0 ) {
        snprintf( temp, sizeof( temp ), "%0.1fmAh", pmu_get_coulumb_data() );
    }
    else {
        snprintf( temp, sizeof( temp ), "unknown" );        
    }
    lv_label_set_text( battery_view_current_cap, temp );
    lv_obj_align( battery_view_current_cap, lv_obj_get_parent( battery_view_current_cap ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    snprintf( temp, sizeof( temp ), "%dmAh", pmu_get_designed_battery_cap() );
    lv_label_set_text( battery_view_design_cap, temp );
    lv_obj_align( battery_view_design_cap, lv_obj_get_parent( battery_view_design_cap ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    snprintf( temp, sizeof( temp ), "%0.2fV", pmu_get_battery_voltage() / 1000 );
    lv_label_set_text( battery_view_voltage, temp );
    lv_obj_align( battery_view_voltage, lv_obj_get_parent( battery_view_voltage ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    snprintf( temp, sizeof( temp ), "%0.1fmA", pmu_get_battery_charge_current() );
    lv_label_set_text( charge_view_current, temp );
    lv_obj_align( charge_view_current, lv_obj_get_parent( charge_view_current ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    snprintf( temp, sizeof( temp ), "%0.1fmA", pmu_get_battery_discharge_current() );
    lv_label_set_text( discharge_view_current, temp );
    lv_obj_align( discharge_view_current, lv_obj_get_parent( discharge_view_current ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    snprintf( temp, sizeof( temp ), "%0.2fV", pmu_get_vbus_voltage() / 1000 );
    lv_label_set_text( vbus_view_voltage, temp );
    lv_obj_align( vbus_view_voltage, lv_obj_get_parent( vbus_view_voltage ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );
}
