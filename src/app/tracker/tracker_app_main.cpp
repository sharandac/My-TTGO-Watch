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

#include "tracker_app.h"
#include "tracker_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget_styles.h"
#include "gui/widget_factory.h"

#include "utils/alloc.h"

#include "hardware/motor.h"
#include "hardware/gpsctl.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
#endif

/**
 * global state variable
 */
bool tracker_logging_state = false;
bool tracker_logging_gps_state = false;
gps_data_t *tracker_gps_data = NULL;
lv_task_t *_tracker_app_task = NULL;
/**
 * local lv obj 
 */
lv_obj_t *tracker_progress_arc = NULL;
lv_style_t tracker_progress_arc_style;
/**
 * call back functions
 */
static void tracker_app_task( lv_task_t * task );
void tracker_app_main_activate_cb( void );
void tracker_app_main_hibernate_cb( void );
static bool tracker_app_main_gps_event_cb( EventBits_t event, void *arg );
static void tracker_app_main_enter_location_cb( lv_obj_t * obj, lv_event_t event );
static void tracker_app_main_enter_menu_cb( lv_obj_t * obj, lv_event_t event );
/*
 * setup routine for wifimon app
 */
void tracker_app_main_setup( uint32_t tile ) {
    tracker_progress_arc = lv_arc_create( mainbar_get_tile_obj( tile ), NULL);
    lv_coord_t size = lv_disp_get_ver_res( NULL ) > lv_disp_get_hor_res( NULL ) ? ( ( lv_disp_get_hor_res( NULL ) / 4 ) * 3 ) : ( ( lv_disp_get_ver_res( NULL ) / 4 ) * 3 );
    lv_obj_set_size( tracker_progress_arc, size, size );
    lv_obj_align( tracker_progress_arc, mainbar_get_tile_obj( tile ), LV_ALIGN_CENTER, 0, 0 );
    lv_arc_set_bg_angles( tracker_progress_arc, 0, 0 );
    lv_arc_set_angles( tracker_progress_arc, 0, 360 );
    lv_arc_set_rotation( tracker_progress_arc, 90 );
    lv_arc_set_end_angle( tracker_progress_arc, 0 );
    lv_obj_add_style( tracker_progress_arc, LV_OBJ_PART_MAIN, APP_STYLE );
    lv_obj_set_style_local_line_width( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, 30 );
    lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED );
    /**
     * add exit, menu and setup button to the main app tile
     */
    lv_obj_t *tracker_exit_btn = wf_add_exit_button( mainbar_get_tile_obj( tile ) );
    lv_obj_align( tracker_exit_btn, mainbar_get_tile_obj( tile ), LV_ALIGN_IN_BOTTOM_LEFT, THEME_PADDING, -THEME_PADDING );

    lv_obj_t *tracker_menu_btn = wf_add_menu_button( mainbar_get_tile_obj( tile ), tracker_app_main_enter_menu_cb );
    lv_obj_align( tracker_menu_btn, mainbar_get_tile_obj( tile ), LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, THEME_PADDING );
    
    lv_obj_t *tracker_location_icon = wf_add_location_button( mainbar_get_tile_obj( tile ), tracker_app_main_enter_location_cb );
    lv_obj_align( tracker_location_icon, mainbar_get_tile_obj( tile ), LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_hidden( tracker_location_icon, false );

    gpsctl_register_cb( GPSCTL_FIX | GPSCTL_NOFIX | GPSCTL_DISABLE | GPSCTL_ENABLE | GPSCTL_UPDATE_LOCATION | GPSCTL_UPDATE_ALTITUDE | GPSCTL_UPDATE_SOURCE | GPSCTL_UPDATE_SPEED | GPSCTL_UPDATE_SATELLITE, tracker_app_main_gps_event_cb, "tracker gps" );
    mainbar_add_tile_activate_cb( tile, tracker_app_main_activate_cb );
    mainbar_add_tile_hibernate_cb( tile, tracker_app_main_hibernate_cb );
}

static void tracker_app_task( lv_task_t * task ) {

}

void tracker_app_main_activate_cb( void ) {
}

void tracker_app_main_hibernate_cb( void ) {
}

/**
 * @brief gps ctl event call back
 * 
 * @param event         event bits
 * @param arg           pointer to a gps_data_t structure
 * @return true 
 * @return false 
 */
static bool tracker_app_main_gps_event_cb( EventBits_t event, void *arg ) {
    gps_data_t *gps_data = (gps_data_t*)arg;

    if( !tracker_gps_data )
        tracker_gps_data = (gps_data_t*)MALLOC( sizeof( gps_data_t ) );

    if( !tracker_gps_data ) {
        log_e("tracker gps data alloc failed");
        while( true );
    }

    if( gps_data )
        memcpy( tracker_gps_data, gps_data, sizeof( gps_data_t ) );

    switch( event ) {
        case GPSCTL_FIX:
            tracker_logging_gps_state = true;
            if( tracker_logging_state )
                lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_GREEN );
            log_i("gps fix");
            break;
        case GPSCTL_NOFIX:
            tracker_logging_gps_state = false;
            if( tracker_logging_state )
                lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE );
            log_i("no gps fix");
            break;
        case GPSCTL_DISABLE:
            tracker_logging_gps_state = false;
            tracker_logging_state = false;
            lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED );
            lv_arc_set_end_angle( tracker_progress_arc, 0 );
            log_i("gps disabled");
            break;
        case GPSCTL_UPDATE_LOCATION:
            log_i("location: %f/%f", tracker_gps_data->lat, tracker_gps_data->lon );
            break;
        case GPSCTL_UPDATE_ALTITUDE:
            log_i("altitude: %f", tracker_gps_data->altitude_meters );
            break;
        case GPSCTL_UPDATE_SOURCE:
            log_i("source: %s", gpsctl_get_source_str( tracker_gps_data->gps_source ) );
            break;
        case GPSCTL_UPDATE_SPEED:
            log_i("speed: %f", tracker_gps_data->speed_kmh );
            break;
        case GPSCTL_UPDATE_SATELLITE:
            log_i("satellite: %d", tracker_gps_data->satellites );
            break;
        default:
            log_i("event not implement: %x", event );
            break;
    }
    return( true );
}

/**
 * @brief event call back function when the setup button was clicked
 * 
 * @param obj       pointer to the lv_obj were send the event
 * @param event     event type
 */
static void tracker_app_main_enter_location_cb( lv_obj_t * obj, lv_event_t event ) {
    static int counter = 0;
    /**
     * obj event handling
     */
    switch ( event ) {
        case LV_EVENT_PRESSING:
            if( counter < 36 ) {
                counter++;
                lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED );
            }
            else {
                lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_GREEN );
            }
            break;
        case LV_EVENT_RELEASED:
            if( counter >= 36 ) {
                if( !tracker_logging_state ) {
                    log_i("start logging");
                    tracker_logging_state = true;
                    gpsctl_on();
                    _tracker_app_task = lv_task_create( tracker_app_task, 1000, LV_TASK_PRIO_MID, NULL );
                }
                else {
                    log_i("stop logging");
                    tracker_logging_state = false;
                    gpsctl_off();
                    if( _tracker_app_task != NULL ) {
                        lv_task_del( _tracker_app_task );
                        _tracker_app_task = NULL;
                    } 
                }
                motor_vibe( 250, false );
            }
            counter = 0;
            break;
    }
    /**
     * handling arc display
     */
    if( counter ) {
        lv_arc_set_end_angle( tracker_progress_arc, counter * 10 );
    }
    else {
        if( tracker_logging_state ) {
            lv_arc_set_end_angle( tracker_progress_arc, 360 );
            if( tracker_logging_gps_state )
                lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_GREEN );
            else
                lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE );
        }
        else {
            lv_arc_set_end_angle( tracker_progress_arc, 0 );
        }
    }

}

/**
 * @brief event call back function when the menu button was clicked
 * 
 * @param obj       pointer to the lv_obj were send the event
 * @param event     event type
 */
static void tracker_app_main_enter_menu_cb( lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case LV_EVENT_CLICKED:
            break;    
    }
}
