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
#include "battery_calibration.h"
#include "battery_settings.h"
#include "battery_view.h"
#include "battery_history.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"
#include "hardware/pmu.h"
#include "hardware/powermgm.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else

#endif

lv_obj_t *battery_calibration_tile = NULL;
lv_obj_t *battery_calibration_page = NULL;
lv_obj_t *battery_calibration_info_label = NULL;
lv_obj_t *battery_calibration_store_switch = NULL;
lv_style_t battery_calibration__style;
uint32_t battery_calibration_tile_num;

static void battery_calibration_store_switch_event_cb( lv_obj_t *obj, lv_event_t event );
static bool battery_calibration_button_event_cb( EventBits_t event, void *arg );
static bool battery_calibration_pmu_event_cb( EventBits_t event, void *arg );

void battery_calibration_tile_setup( uint32_t tile_num ) {
    battery_calibration_tile_num = tile_num + 3;

    battery_calibration_page = lv_page_create( mainbar_get_tile_obj( battery_calibration_tile_num ), NULL);
    lv_obj_set_size( battery_calibration_page, lv_disp_get_hor_res( NULL ) - 30 , lv_disp_get_ver_res( NULL ) - 50 );
    lv_obj_add_style( battery_calibration_page, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_page_set_scrlbar_mode( battery_calibration_page, LV_SCRLBAR_MODE_DRAG );
    lv_obj_align( battery_calibration_page, mainbar_get_tile_obj( battery_calibration_tile_num ), LV_ALIGN_CENTER, 0, THEME_PADDING );

    battery_calibration_info_label = lv_label_create( battery_calibration_page, NULL );
    lv_label_set_long_mode( battery_calibration_info_label, LV_LABEL_LONG_BREAK );
    lv_obj_set_width( battery_calibration_info_label, lv_page_get_width_fit ( battery_calibration_page ) );
    lv_obj_add_style( battery_calibration_info_label, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_label_set_text( battery_calibration_info_label, "Test message from bar.");

    battery_calibration_store_switch = wf_add_switch( mainbar_get_tile_obj( battery_calibration_tile_num ), false ,battery_calibration_store_switch_event_cb );
    lv_obj_align( battery_calibration_store_switch, mainbar_get_tile_obj( battery_calibration_tile_num ), LV_ALIGN_IN_BOTTOM_MID, 0, -10 );
    lv_obj_set_hidden( battery_calibration_store_switch, true );

    mainbar_add_slide_element( battery_calibration_page );
    mainbar_add_slide_element( battery_calibration_info_label );

    pmu_register_cb( PMUCTL_CALIBRATION_START | PMUCTL_CALIBRATION_UPDATE, battery_calibration_pmu_event_cb, "pmu battery calibration");
}

static void battery_calibration_store_switch_event_cb( lv_obj_t *obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): pmu_battery_calibration_loop( true, lv_switch_get_state( obj ) );
                                        powermgm_set_lightsleep( false );
                                        powermgm_set_normal_mode();
                                        log_i("start calibration");
                                        break;
    }
}

static bool battery_calibration_pmu_event_cb( EventBits_t event, void *arg ) {
    calibration_data_t *calibration_data = (calibration_data_t*)arg;

    switch( event ) {
        case PMUCTL_CALIBRATION_START:
            wf_label_printf( battery_calibration_info_label, "battery calibration started" );
            break;
        case PMUCTL_CALIBRATION_UPDATE:
            wf_label_printf( battery_calibration_info_label, "battery = %.0fmV\nmin = %.0fmV\nmax = %.0fmV\nmaxCharge = %.0fmV\nchargingOffset = %.0fmV\nVBUS = %s\ncharging = %s\ncalibration run = %s\nstore = %s", calibration_data->batteryVoltage, calibration_data->minVoltage, calibration_data->maxVoltage, calibration_data->maxVoltageCharge, calibration_data->chargingVoltageOffset, calibration_data->VBUS ? "true":"false", calibration_data->charging ? "true":"false", calibration_data->run ? "true":"false", calibration_data->store ? "true":"false" );
            if( calibration_data->VBUS && !calibration_data->charging )
                lv_obj_set_hidden( battery_calibration_store_switch, false );
            else
                lv_obj_set_hidden( battery_calibration_store_switch, true );
            break;
    }

    return( true );
}