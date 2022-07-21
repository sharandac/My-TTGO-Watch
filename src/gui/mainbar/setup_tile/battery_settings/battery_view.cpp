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
#include "battery_settings.h"
#include "battery_view.h"
#include "battery_history.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"
#include "hardware/pmu.h"
#include "hardware/motor.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else

#endif

lv_obj_t *battery_view_tile=NULL;
uint32_t battery_view_tile_num;

lv_obj_t *battery_view_design_cap;
lv_obj_t *battery_view_current_cap;
lv_obj_t *battery_view_voltage;
lv_obj_t *charge_view_current;
lv_obj_t *discharge_view_current;
lv_obj_t *vbus_view_voltage;
lv_task_t *battery_view_task = NULL;

static void battery_view_update_task( lv_task_t *task );
static bool battery_view_button_cb( EventBits_t event, void *arg );
static void battery_activate_cb( void );
static void battery_hibernate_cb( void );

void battery_view_tile_setup( uint32_t tile_num ) {
    // get an app tile and copy mainstyle
    battery_view_tile_num = tile_num + 1;
    battery_view_tile = mainbar_get_tile_obj( battery_view_tile_num );

    lv_obj_t *battery_design_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( battery_design_cont, lv_disp_get_hor_res( NULL ) , 25 );
    lv_obj_add_style( battery_design_cont, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_obj_align( battery_design_cont, battery_view_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    lv_obj_t *battery_design_cap_label = lv_label_create( battery_design_cont, NULL);
    lv_obj_add_style( battery_design_cap_label, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( battery_design_cap_label, "designed cap");
    lv_obj_align( battery_design_cap_label, battery_design_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    battery_view_design_cap = lv_label_create( battery_design_cont, NULL);
    lv_obj_add_style( battery_view_design_cap, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( battery_view_design_cap, "300mAh");
    lv_obj_align( battery_view_design_cap, battery_design_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );

    lv_obj_t *battery_current_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( battery_current_cont, lv_disp_get_hor_res( NULL ) , 22 );
    lv_obj_add_style( battery_current_cont, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_obj_align( battery_current_cont, battery_design_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_current_cap_label = lv_label_create( battery_current_cont, NULL);
    lv_obj_add_style( battery_current_cap_label, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );
    lv_label_set_text( battery_current_cap_label, "charged capacity");
    lv_obj_align( battery_current_cap_label, battery_current_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    battery_view_current_cap = lv_label_create( battery_current_cont, NULL);
    lv_obj_add_style( battery_view_current_cap, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( battery_view_current_cap, "300mAh");
    lv_obj_align( battery_view_current_cap, battery_current_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );

    lv_obj_t *battery_voltage_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( battery_voltage_cont, lv_disp_get_hor_res( NULL ) , 22 );
    lv_obj_add_style( battery_voltage_cont, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_obj_align( battery_voltage_cont, battery_current_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_voltage_label = lv_label_create( battery_voltage_cont, NULL);
    lv_obj_add_style( battery_voltage_label, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( battery_voltage_label, "battery voltage");
    lv_obj_align( battery_voltage_label, battery_voltage_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    battery_view_voltage = lv_label_create( battery_voltage_cont, NULL);
    lv_obj_add_style( battery_view_voltage, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( battery_view_voltage, "2.4mV");
    lv_obj_align( battery_view_voltage, battery_voltage_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );

    lv_obj_t *battery_charge_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( battery_charge_cont, lv_disp_get_hor_res( NULL ) , 22 );
    lv_obj_add_style( battery_charge_cont, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_obj_align( battery_charge_cont, battery_voltage_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_charge_label = lv_label_create( battery_charge_cont, NULL);
    lv_obj_add_style( battery_charge_label, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( battery_charge_label, "charge current");
    lv_obj_align( battery_charge_label, battery_charge_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    charge_view_current = lv_label_create( battery_charge_cont, NULL);
    lv_obj_add_style( charge_view_current, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( charge_view_current, "100mA");
    lv_obj_align( charge_view_current, battery_charge_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );

    lv_obj_t *battery_discharge_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( battery_discharge_cont, lv_disp_get_hor_res( NULL ) , 22 );
    lv_obj_add_style( battery_discharge_cont, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_obj_align( battery_discharge_cont, battery_charge_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_discharge_label = lv_label_create( battery_discharge_cont, NULL);
    lv_obj_add_style( battery_discharge_label, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( battery_discharge_label, "discharge current");
    lv_obj_align( battery_discharge_label, battery_discharge_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    discharge_view_current = lv_label_create( battery_discharge_cont, NULL);
    lv_obj_add_style( discharge_view_current, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( discharge_view_current, "100mA");
    lv_obj_align( discharge_view_current, battery_discharge_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );

    lv_obj_t *vbus_voltage_cont = lv_obj_create( battery_view_tile, NULL );
    lv_obj_set_size( vbus_voltage_cont, lv_disp_get_hor_res( NULL ) , 22 );
    lv_obj_add_style( vbus_voltage_cont, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_obj_align( vbus_voltage_cont, battery_discharge_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *vbus_voltage_label = lv_label_create( vbus_voltage_cont, NULL);
    lv_obj_add_style( vbus_voltage_label, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( vbus_voltage_label, "VBUS voltage");
    lv_obj_align( vbus_voltage_label, vbus_voltage_cont, LV_ALIGN_IN_LEFT_MID, THEME_PADDING, 0 );
    vbus_view_voltage = lv_label_create( vbus_voltage_cont, NULL);
    lv_obj_add_style( vbus_view_voltage, LV_OBJ_PART_MAIN, ws_get_setup_tile_style()  );
    lv_label_set_text( vbus_view_voltage, "2.4mV");
    lv_obj_align( vbus_view_voltage, vbus_voltage_cont, LV_ALIGN_IN_RIGHT_MID, -THEME_PADDING, 0 );

    mainbar_add_tile_button_cb( battery_view_tile_num, battery_view_button_cb );

    mainbar_add_tile_activate_cb( tile_num, battery_activate_cb );
    mainbar_add_tile_activate_cb( tile_num + 1, battery_activate_cb );
    mainbar_add_tile_hibernate_cb( tile_num, battery_hibernate_cb );
    mainbar_add_tile_hibernate_cb( tile_num + 1, battery_hibernate_cb );
}

static bool battery_view_button_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_EXIT:
            mainbar_jump_back();
            break;
    }

    return( true );
}

static void battery_activate_cb( void ) {
    if( !battery_view_task )
        battery_view_task = lv_task_create( battery_view_update_task, 1000,  LV_TASK_PRIO_LOWEST, NULL );
}

static void battery_hibernate_cb( void ) {
    if( battery_view_task ) {
        lv_task_del( battery_view_task );
        battery_view_task = NULL;
    }
}

static void battery_view_update_task( lv_task_t *task ) {
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
