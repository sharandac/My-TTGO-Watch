#include "config.h"
#include <TTGO.h>
#include "battery_settings.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "hardware/pmu.h"

lv_obj_t *battery_settings_tile=NULL;
lv_style_t battery_settings_style;

lv_obj_t *battery_design_cap;
lv_obj_t *battery_current_cap;
lv_obj_t *battery_voltage;
lv_obj_t *charge_current;
lv_obj_t *discharge_current;
lv_obj_t *vbus_voltage;

LV_IMG_DECLARE(exit_32px);

static void exit_battery_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void battery_update_task( lv_task_t *task );

void battery_settings_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {
    lv_style_init( &battery_settings_style );
    lv_style_set_radius( &battery_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color( &battery_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &battery_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &battery_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &battery_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor( &battery_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    battery_settings_tile = lv_obj_create( tile, NULL);
    lv_obj_set_size(battery_settings_tile, hres , vres);
    lv_obj_align(battery_settings_tile, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style( battery_settings_tile, LV_OBJ_PART_MAIN, &battery_settings_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( battery_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( exit_btn, battery_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_battery_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( battery_settings_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( exit_label, "battery");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *battery_design_cont = lv_obj_create( battery_settings_tile, NULL );
    lv_obj_set_size( battery_design_cont, hres , 25 );
    lv_obj_add_style( battery_design_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( battery_design_cont, battery_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    lv_obj_t *battery_design_cap_label = lv_label_create( battery_design_cont, NULL);
    lv_obj_add_style( battery_design_cap_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( battery_design_cap_label, "designed cap");
    lv_obj_align( battery_design_cap_label, battery_design_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    battery_design_cap = lv_label_create( battery_design_cont, NULL);
    lv_obj_add_style( battery_design_cap, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( battery_design_cap, "380mAh");
    lv_obj_align( battery_design_cap, battery_design_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *battery_current_cont = lv_obj_create( battery_settings_tile, NULL );
    lv_obj_set_size( battery_current_cont, hres , 25 );
    lv_obj_add_style( battery_current_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( battery_current_cont, battery_design_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_current_cap_label = lv_label_create( battery_current_cont, NULL);
    lv_obj_add_style( battery_current_cap_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( battery_current_cap_label, "charged capacity");
    lv_obj_align( battery_current_cap_label, battery_current_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    battery_current_cap = lv_label_create( battery_current_cont, NULL);
    lv_obj_add_style( battery_current_cap, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( battery_current_cap, "380mAh");
    lv_obj_align( battery_current_cap, battery_current_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *battery_voltage_cont = lv_obj_create( battery_settings_tile, NULL );
    lv_obj_set_size( battery_voltage_cont, hres , 25 );
    lv_obj_add_style( battery_voltage_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( battery_voltage_cont, battery_current_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_voltage_label = lv_label_create( battery_voltage_cont, NULL);
    lv_obj_add_style( battery_voltage_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( battery_voltage_label, "battery voltage");
    lv_obj_align( battery_voltage_label, battery_voltage_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    battery_voltage = lv_label_create( battery_voltage_cont, NULL);
    lv_obj_add_style( battery_voltage, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( battery_voltage, "2.4mV");
    lv_obj_align( battery_voltage, battery_voltage_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *battery_charge_cont = lv_obj_create( battery_settings_tile, NULL );
    lv_obj_set_size( battery_charge_cont, hres , 25 );
    lv_obj_add_style( battery_charge_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( battery_charge_cont, battery_voltage_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
    lv_obj_t *battery_charge_label = lv_label_create( battery_charge_cont, NULL);
    lv_obj_add_style( battery_charge_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( battery_charge_label, "charge current");
    lv_obj_align( battery_charge_label, battery_charge_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    charge_current = lv_label_create( battery_charge_cont, NULL);
    lv_obj_add_style( charge_current, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( charge_current, "100mA");
    lv_obj_align( charge_current, battery_charge_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *battery_discharge_cont = lv_obj_create( battery_settings_tile, NULL );
    lv_obj_set_size( battery_discharge_cont, hres , 25 );
    lv_obj_add_style( battery_discharge_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( battery_discharge_cont, battery_charge_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *battery_discharge_label = lv_label_create( battery_discharge_cont, NULL);
    lv_obj_add_style( battery_discharge_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( battery_discharge_label, "discharge current");
    lv_obj_align( battery_discharge_label, battery_discharge_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    discharge_current = lv_label_create( battery_discharge_cont, NULL);
    lv_obj_add_style( discharge_current, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( discharge_current, "100mA");
    lv_obj_align( discharge_current, battery_discharge_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *vbus_voltage_cont = lv_obj_create( battery_settings_tile, NULL );
    lv_obj_set_size( vbus_voltage_cont, hres , 25 );
    lv_obj_add_style( vbus_voltage_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( vbus_voltage_cont, battery_discharge_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *vbus_voltage_label = lv_label_create( vbus_voltage_cont, NULL);
    lv_obj_add_style( vbus_voltage_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( vbus_voltage_label, "VBUS voltage");
    lv_obj_align( vbus_voltage_label, vbus_voltage_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    vbus_voltage = lv_label_create( vbus_voltage_cont, NULL);
    lv_obj_add_style( vbus_voltage, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( vbus_voltage, "2.4mV");
    lv_obj_align( vbus_voltage, vbus_voltage_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_task_t * task = lv_task_create(battery_update_task, 1000,  LV_TASK_PRIO_LOWEST, NULL );
}


static void exit_battery_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( SETUP_TILE, LV_ANIM_OFF );
                                        break;
    }
}


void battery_update_task( lv_task_t *task ) {
    char temp[16]="";
    TTGOClass *ttgo = TTGOClass::getWatch();

    snprintf( temp, sizeof( temp ), "%0.1fmAh", ttgo->power->getCoulombData() );
    lv_label_set_text( battery_current_cap, temp );
    lv_obj_align( battery_current_cap, lv_obj_get_parent( battery_current_cap ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    snprintf( temp, sizeof( temp ), "%0.2fV", ttgo->power->getBattVoltage() / 1000 );
    lv_label_set_text( battery_voltage, temp );
    lv_obj_align( battery_voltage, lv_obj_get_parent( battery_voltage ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    snprintf( temp, sizeof( temp ), "%0.1fmA", ttgo->power->getBattChargeCurrent() );
    lv_label_set_text( charge_current, temp );
    lv_obj_align( charge_current, lv_obj_get_parent( charge_current ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    snprintf( temp, sizeof( temp ), "%0.1fmA", ttgo->power->getBattDischargeCurrent() );
    lv_label_set_text( discharge_current, temp );
    lv_obj_align( discharge_current, lv_obj_get_parent( discharge_current ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    snprintf( temp, sizeof( temp ), "%0.2fV", ttgo->power->getVbusVoltage() / 1000 );
    lv_label_set_text( vbus_voltage, temp );
    lv_obj_align( vbus_voltage, lv_obj_get_parent( vbus_voltage ), LV_ALIGN_IN_RIGHT_MID, -5, 0 );
}
