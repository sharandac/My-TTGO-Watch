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
#include "hardware/powermgm.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else

#endif

lv_obj_t *battery_history_tile=NULL;
lv_style_t battery_history_style;
uint32_t battery_history_tile_num;

lv_obj_t *battery_history_voltage_chart = NULL;
lv_obj_t *battery_history_current_chart = NULL;
lv_chart_series_t *battery_history_voltage_series = NULL;
lv_chart_series_t *battery_history_charge_series = NULL;
lv_chart_series_t *battery_history_discharge_series = NULL;

static bool battery_history_button_event_cb( EventBits_t event, void *arg );
static bool battery_history_powermgm_loop_cb( EventBits_t event, void *arg );

void battery_history_tile_setup( uint32_t tile_num ) {
    battery_history_tile_num = tile_num + 2;
    /**
     * add chart widget
     */
    battery_history_voltage_chart = lv_chart_create( mainbar_get_tile_obj( battery_history_tile_num ), NULL );
    lv_obj_set_size( battery_history_voltage_chart, lv_disp_get_hor_res( NULL ), ( lv_disp_get_ver_res( NULL ) - STATUSBAR_HEIGHT ) / 2 );
    lv_obj_align( battery_history_voltage_chart, NULL, LV_ALIGN_IN_TOP_MID, 0, STATUSBAR_HEIGHT );
    lv_chart_set_type( battery_history_voltage_chart, LV_CHART_TYPE_LINE );  
    lv_chart_set_point_count( battery_history_voltage_chart, lv_disp_get_hor_res( NULL ) / 2);
    lv_chart_set_div_line_count( battery_history_voltage_chart, 1, 1 );
    lv_obj_add_style( battery_history_voltage_chart, LV_OBJ_PART_MAIN, APP_STYLE );
    lv_obj_set_style_local_size( battery_history_voltage_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 1 );
    lv_obj_set_style_local_bg_opa( battery_history_voltage_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_100 );
    lv_obj_set_style_local_bg_grad_dir( battery_history_voltage_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER );
    lv_obj_set_style_local_bg_main_stop( battery_history_voltage_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255 );
    lv_obj_set_style_local_bg_grad_stop( battery_history_voltage_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0 );
//    lv_obj_set_style_local_line_width ( battery_history_voltage_chart, LV_CHART_PART_SERIES_BG, LV_STATE_DEFAULT, 0 );

    battery_history_current_chart = lv_chart_create( mainbar_get_tile_obj( battery_history_tile_num ), NULL );
    lv_obj_set_size( battery_history_current_chart, lv_disp_get_hor_res( NULL ), ( lv_disp_get_ver_res( NULL ) - STATUSBAR_HEIGHT ) / 2 );
    lv_obj_align( battery_history_current_chart, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_chart_set_type( battery_history_current_chart, LV_CHART_TYPE_LINE );  
    lv_chart_set_point_count( battery_history_current_chart, lv_disp_get_hor_res( NULL ) / 2 );
    lv_chart_set_div_line_count( battery_history_current_chart, 5, 1 );
    lv_obj_add_style( battery_history_current_chart, LV_OBJ_PART_MAIN, APP_STYLE );
    lv_obj_set_style_local_size( battery_history_current_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 1 );
    lv_obj_set_style_local_bg_opa( battery_history_current_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_100 );
    lv_obj_set_style_local_bg_grad_dir( battery_history_current_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER );
    lv_obj_set_style_local_bg_main_stop( battery_history_current_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255 );
    lv_obj_set_style_local_bg_grad_stop( battery_history_current_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0 );
//    lv_obj_set_style_local_line_width ( battery_history_current_chart, LV_CHART_PART_SERIES_BG, LV_STATE_DEFAULT, 0 );

    battery_history_voltage_series = lv_chart_add_series( battery_history_voltage_chart, LV_COLOR_BLUE );
    battery_history_charge_series = lv_chart_add_series( battery_history_current_chart, LV_COLOR_GREEN );
    battery_history_discharge_series = lv_chart_add_series( battery_history_current_chart, LV_COLOR_RED );

    lv_obj_t *battery_history_voltage_label = wf_add_label( battery_history_voltage_chart, "voltage", APP_ICON_LABEL_STYLE );
    lv_obj_align( battery_history_voltage_label, battery_history_voltage_chart, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING * 2, THEME_PADDING * 2 );

    lv_obj_t *battery_history_current_label = wf_add_label( battery_history_current_chart, "current", APP_ICON_LABEL_STYLE );
    lv_obj_align( battery_history_current_label, battery_history_current_chart, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING * 2 , THEME_PADDING * 2 );

    lv_obj_t *battery_history_voltage_min_label = wf_add_label( battery_history_voltage_chart, "3.0V", APP_ICON_LABEL_STYLE );
    lv_obj_align( battery_history_voltage_min_label, battery_history_voltage_chart, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING * 2, -THEME_PADDING * 2 );

    lv_obj_t *battery_history_voltage_max_label = wf_add_label( battery_history_voltage_chart, "4.4V", APP_ICON_LABEL_STYLE );
    lv_obj_align( battery_history_voltage_max_label, battery_history_voltage_chart, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING * 2, THEME_PADDING * 2 );

    lv_obj_t *battery_history_current_min_label = wf_add_label( battery_history_current_chart, "0mA", APP_ICON_LABEL_STYLE );
    lv_obj_align( battery_history_current_min_label, battery_history_current_chart, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING * 2, -THEME_PADDING * 2 );

    lv_obj_t *battery_history_current_max_label = wf_add_label( battery_history_current_chart, "300mA", APP_ICON_LABEL_STYLE );
    lv_obj_align( battery_history_current_max_label, battery_history_current_chart, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING * 2, THEME_PADDING * 2 );

    lv_chart_set_y_range( battery_history_voltage_chart, lv_chart_get_series_axis( battery_history_voltage_chart, battery_history_voltage_series ), 3000, 4400 );
    lv_chart_set_y_range( battery_history_current_chart, lv_chart_get_series_axis( battery_history_current_chart, battery_history_charge_series ), 0, 300 );
    lv_chart_set_y_range( battery_history_current_chart, lv_chart_get_series_axis( battery_history_current_chart, battery_history_discharge_series ), 0, 300 );

    mainbar_add_tile_button_cb( battery_history_tile_num, battery_history_button_event_cb );

    mainbar_add_slide_element( battery_history_voltage_chart );
    mainbar_add_slide_element( battery_history_current_chart );
}

static bool battery_history_button_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_EXIT:
            mainbar_jump_back();
            break;
    }

    return( true );
}

void battery_history_start_chart_logging( void ) {
    static bool activate = false;

    if( !activate ) {
        powermgm_register_loop_cb( POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, battery_history_powermgm_loop_cb, "powermgm battery history" );
        activate = true;
        log_i("start battery logging into chart");
    }
}

static bool battery_history_powermgm_loop_cb( EventBits_t event, void *arg ) {
    static uint64_t NextMillis = 0;

    if( !NextMillis )
        NextMillis = millis();

    if( millis() > NextMillis ) {
        NextMillis = millis() + BATTERY_HISTORY_INTERVALL * 1000;
        lv_chart_set_next( battery_history_voltage_chart, battery_history_voltage_series, pmu_get_battery_voltage() );
        lv_chart_set_next( battery_history_current_chart, battery_history_charge_series, pmu_get_battery_charge_current() );
        lv_chart_set_next( battery_history_current_chart, battery_history_discharge_series, pmu_get_battery_discharge_current() );
    }
    return( true );
}