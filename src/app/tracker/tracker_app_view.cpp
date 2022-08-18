/****************************************************************************
 *   Aug 11 17:13:51 2020
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
#include <stdio.h>
#include <unistd.h> 

#include "tracker_app.h"
#include "tracker_app_main.h"
#include "tracker_app_view.h"
#include "config/tracker_config.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget_styles.h"
#include "gui/widget_factory.h"

#include "hardware/gpsctl.h"
/**
 * local vaiables
 */
lv_coord_t tracker_speed_min = -1;
lv_coord_t tracker_speed_max = -1;
lv_coord_t tracker_altitude_min = -1;
lv_coord_t tracker_altitude_max = -1;
/**
 * local lv obj 
 */
lv_obj_t *tracker_speed_min_label = NULL;
lv_obj_t *tracker_speed_max_label = NULL;
lv_obj_t *tracker_altitude_min_label = NULL;
lv_obj_t *tracker_altitude_max_label = NULL;
lv_obj_t *tracker_speed_chart = NULL;
lv_obj_t *tracker_altitude_chart = NULL;
lv_chart_series_t *tracker_speed_series = NULL;
lv_chart_series_t *tracker_altitude_series = NULL;
/**
 * 
 */
static bool tracker_app_view_button_cb( EventBits_t event, void *arg );
/**
 * 
 */
void tracker_app_view_setup( uint32_t tile ) {
    /**
     * add chart widget
     */
    tracker_speed_chart = lv_chart_create( mainbar_get_tile_obj( tile ), NULL );
    lv_obj_set_size( tracker_speed_chart, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) / 2 );
    lv_obj_align( tracker_speed_chart, NULL, LV_ALIGN_IN_TOP_MID, 0, 0 );
    lv_chart_set_type( tracker_speed_chart, LV_CHART_TYPE_LINE );  
    lv_chart_set_point_count( tracker_speed_chart, lv_disp_get_hor_res( NULL ) / 2 );
    lv_chart_set_div_line_count( tracker_speed_chart, 1, 1 );
    lv_obj_add_style( tracker_speed_chart, LV_OBJ_PART_MAIN, APP_STYLE );
    lv_obj_set_style_local_size( tracker_speed_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 1 );
    lv_obj_set_style_local_bg_opa( tracker_speed_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_100 );
    lv_obj_set_style_local_bg_grad_dir( tracker_speed_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER );
    lv_obj_set_style_local_bg_main_stop( tracker_speed_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255 );
    lv_obj_set_style_local_bg_grad_stop( tracker_speed_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0 );

    tracker_altitude_chart = lv_chart_create( mainbar_get_tile_obj( tile ), NULL );
    lv_obj_set_size( tracker_altitude_chart, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) / 2 );
    lv_obj_align( tracker_altitude_chart, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_chart_set_type( tracker_altitude_chart, LV_CHART_TYPE_LINE );  
    lv_chart_set_point_count( tracker_altitude_chart, lv_disp_get_hor_res( NULL ) / 2 );
    lv_chart_set_div_line_count( tracker_altitude_chart, 1, 1 );
    lv_obj_add_style( tracker_altitude_chart, LV_OBJ_PART_MAIN, APP_STYLE );
    lv_obj_set_style_local_size( tracker_altitude_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 1 );
    lv_obj_set_style_local_bg_opa( tracker_altitude_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_OPA_100 );
    lv_obj_set_style_local_bg_grad_dir( tracker_altitude_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER );
    lv_obj_set_style_local_bg_main_stop( tracker_altitude_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255 );
    lv_obj_set_style_local_bg_grad_stop( tracker_altitude_chart, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0 );

    tracker_speed_series = lv_chart_add_series( tracker_speed_chart, LV_COLOR_GREEN );
    tracker_altitude_series = lv_chart_add_series( tracker_altitude_chart, LV_COLOR_BLUE );

    lv_obj_t *tracker_speed_label = wf_add_label( mainbar_get_tile_obj( tile ), "speed", APP_ICON_LABEL_STYLE );
    lv_obj_align( tracker_speed_label, tracker_speed_chart, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING * 2, THEME_PADDING  * 2);

    lv_obj_t *tracker_altitude_label = wf_add_label( mainbar_get_tile_obj( tile ), "altitude", APP_ICON_LABEL_STYLE );
    lv_obj_align( tracker_altitude_label, tracker_altitude_chart, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING * 2, THEME_PADDING * 2 );

    tracker_speed_min_label = wf_add_label( mainbar_get_tile_obj( tile ), "0km/h", APP_ICON_LABEL_STYLE );
    lv_obj_align( tracker_speed_min_label, tracker_speed_chart, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING * 2, -THEME_PADDING  * 2);

    tracker_speed_max_label = wf_add_label( mainbar_get_tile_obj( tile ), "0km/h", APP_ICON_LABEL_STYLE );
    lv_obj_align( tracker_speed_max_label, tracker_speed_chart, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING * 2, THEME_PADDING  * 2);

    tracker_altitude_min_label = wf_add_label( mainbar_get_tile_obj( tile ), "0m", APP_ICON_LABEL_STYLE );
    lv_obj_align( tracker_altitude_min_label, tracker_altitude_chart, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING * 2, -THEME_PADDING  * 2);

    tracker_altitude_max_label = wf_add_label( mainbar_get_tile_obj( tile ), "0m", APP_ICON_LABEL_STYLE );
    lv_obj_align( tracker_altitude_max_label, tracker_altitude_chart, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING * 2, THEME_PADDING  * 2);

    mainbar_add_slide_element( tracker_speed_chart );
    mainbar_add_slide_element( tracker_altitude_chart );
    mainbar_add_tile_button_cb( tile, tracker_app_view_button_cb );
}

/**
 * @brief 
 * 
 * @param event 
 * @param arg 
 * @return true 
 * @return false 
 */
static bool tracker_app_view_button_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_LEFT:
            mainbar_jump_back();
            break;
        case BUTTON_EXIT:
            mainbar_jump_back();
            break;
    }
    return( true );
}

void tracker_app_view_add_data( gps_data_t *gps_data ) {
    lv_chart_set_next( tracker_speed_chart, tracker_speed_series, gps_data->speed_kmh );
    lv_chart_set_next( tracker_altitude_chart, tracker_altitude_series, gps_data->altitude_meters );

    lv_coord_t speed = gps_data->speed_kmh;
    lv_coord_t altitude = gps_data->altitude_meters;

    if( tracker_speed_min == -1 && tracker_speed_max == -1 ) {
        tracker_speed_min = tracker_speed_max = speed;
    }
    else {
        if( speed > tracker_speed_max )
            tracker_speed_max = speed;
        else if( speed < tracker_speed_min )
            tracker_speed_min = speed;
        lv_chart_set_y_range( tracker_speed_chart, lv_chart_get_series_axis( tracker_speed_chart, tracker_speed_series ), tracker_speed_min, tracker_speed_max );
        wf_label_printf( tracker_speed_min_label, tracker_speed_chart, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING * 2, -THEME_PADDING  * 2, "%dkm/h", tracker_speed_min );
        wf_label_printf( tracker_speed_max_label, tracker_speed_chart, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING * 2, THEME_PADDING  * 2, "%dkm/h", tracker_speed_max );
    }

    if( tracker_altitude_min == -1 && tracker_altitude_max == -1 ) {
        tracker_altitude_min = tracker_altitude_max = altitude;
    }
    else {
        if( altitude > tracker_altitude_max )
            tracker_altitude_max = altitude;
        else if( altitude < tracker_altitude_min )
            tracker_altitude_min = altitude;
        lv_chart_set_y_range( tracker_altitude_chart, lv_chart_get_series_axis( tracker_altitude_chart, tracker_altitude_series ), tracker_altitude_min, tracker_altitude_max );
        wf_label_printf( tracker_altitude_min_label, tracker_altitude_chart, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING * 2, -THEME_PADDING  * 2, "%dm", tracker_altitude_min );
        wf_label_printf( tracker_altitude_max_label, tracker_altitude_chart, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING * 2, THEME_PADDING  * 2, "%dm", tracker_altitude_max );
    }
}

void tracker_app_view_clean_data( void ) {
    lv_chart_clear_series( tracker_speed_chart, tracker_speed_series );
    lv_chart_clear_series( tracker_altitude_chart, tracker_altitude_series );
    tracker_speed_min = -1;
    tracker_speed_max = -1;
    tracker_altitude_min = -1;
    tracker_altitude_max = -1;
}