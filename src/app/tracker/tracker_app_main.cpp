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
#include <dirent.h>
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

#include "utils/alloc.h"
#include "utils/filepath_convert.h"

#include "hardware/motor.h"
#include "hardware/gpsctl.h"
#include "hardware/sdcard.h"
#include "hardware/display.h"
#include "hardware/blectl.h"
#include "hardware/ble/gadgetbridge.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
    #include "gui/mainbar/setup_tile/watchface/watchface_tile.h"
#endif

/**
 * global state variable
 */
static volatile bool tracker_block_return_maintile = false;     /** @brief osm block to maintile state store */
static volatile bool tracker_block_show_messages = false;       /** @brief osm show messages state store */
static volatile bool tracker_block_watchface = false;           /** @brief osm statusbar force dark mode state store */
static volatile bool tracker_gps_on_standby_state = false;      /** @brief osm gps on standby on enter osmmap */
static volatile bool tracker_logging_state = false;             /** @brief logging state, true for logging and false for inactive */
static volatile bool tracker_logging_gps_state = false;         /** @brief gps state, true for fix, false for no fix */
static volatile double distance = 0.0f;
static volatile double course = 0.0f;

gps_data_t *tracker_gps_data = NULL;
tracker_config_t tracker_config;
/**
 * local lv obj 
 */
lv_obj_t *tracker_exit_btn = NULL;
lv_obj_t *tracker_trash_btn = NULL;
lv_obj_t *tracker_location_icon = NULL;
lv_obj_t *tracker_progress_arc = NULL;
lv_obj_t *tracker_info_label = NULL;
lv_obj_t *tracker_file_info_label = NULL;
/**
 * call back functions
 */
void tracker_app_main_activate_cb( void );
void tracker_app_main_hibernate_cb( void );
static bool tracker_app_main_button_cb( EventBits_t event, void *arg );
const char *tracker_app_main_logging( bool start, gps_data_t *gps_data );
static bool tracker_app_main_gps_event_cb( EventBits_t event, void *arg );
static void tracker_app_main_enter_location_cb( lv_obj_t * obj, lv_event_t event );
static void tracker_app_main_enter_trash_cb( lv_obj_t * obj, lv_event_t event );
/*
 *
 */
LV_FONT_DECLARE(Ubuntu_32px);
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
    lv_obj_set_style_local_line_width( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, 45 );
    lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED );
    /**
     * add exit, menu and setup button to the main app tile
     */
    tracker_exit_btn = wf_add_exit_button( mainbar_get_tile_obj( tile ) );
    lv_obj_align( tracker_exit_btn, mainbar_get_tile_obj( tile ), LV_ALIGN_IN_BOTTOM_LEFT, THEME_PADDING, -THEME_PADDING );

    tracker_trash_btn = wf_add_trash_button( mainbar_get_tile_obj( tile ), tracker_app_main_enter_trash_cb );
    lv_obj_align( tracker_trash_btn, mainbar_get_tile_obj( tile ), LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, THEME_PADDING );
    
    tracker_location_icon = wf_add_location_button( mainbar_get_tile_obj( tile ), tracker_app_main_enter_location_cb );
    lv_obj_align( tracker_location_icon, mainbar_get_tile_obj( tile ), LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_hidden( tracker_location_icon, false );

    tracker_info_label = wf_add_label( mainbar_get_tile_obj( tile ), "", APP_ICON_LABEL_STYLE );
    lv_obj_set_style_local_text_font(tracker_info_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &Ubuntu_32px );
    tracker_file_info_label = wf_add_label( mainbar_get_tile_obj( tile ), "", APP_ICON_LABEL_STYLE );

    gpsctl_register_cb( GPSCTL_FIX | GPSCTL_NOFIX | GPSCTL_DISABLE | GPSCTL_ENABLE | GPSCTL_UPDATE_LOCATION | GPSCTL_UPDATE_ALTITUDE | GPSCTL_UPDATE_SOURCE | GPSCTL_UPDATE_SPEED | GPSCTL_UPDATE_SATELLITE, tracker_app_main_gps_event_cb, "tracker gps" );
    mainbar_add_tile_activate_cb( tile, tracker_app_main_activate_cb );
    mainbar_add_tile_hibernate_cb( tile, tracker_app_main_hibernate_cb );
    mainbar_add_tile_button_cb( tile, tracker_app_main_button_cb );

    mainbar_add_slide_element( tracker_progress_arc );
    mainbar_add_slide_element( tracker_exit_btn );
    mainbar_add_slide_element( tracker_trash_btn );
    mainbar_add_slide_element( tracker_info_label );
    mainbar_add_slide_element( tracker_file_info_label );
}

/**
 * @brief call back function for button if the current tile active
 * 
 * @param event         event like BUTTON_LEFT, BUTTON_RIGHT, ...
 * @param arg           here like NULL
 * @return true 
 * @return false 
 */
static bool tracker_app_main_button_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_RIGHT:
            mainbar_jump_to_tilenumber( tracker_app_get_app_view_tile_num(), LV_ANIM_ON );
            break;        
        case BUTTON_EXIT:
            mainbar_jump_back();
            break;
    }
    return( true );
}
/**
 * @brief call back function if the current tile activate
 * 
 */
void tracker_app_main_activate_cb( void ) {
#ifdef NATIVE_64BIT
#else
    tracker_block_watchface = watchface_get_enable_tile_after_wakeup();
    watchface_enable_tile_after_wakeup( false );
#endif
    tracker_block_show_messages = blectl_get_show_notification();
    blectl_set_show_notification( false );
    
    tracker_block_return_maintile = display_get_block_return_maintile();
    display_set_block_return_maintile( true );

    tracker_config.load();

    wf_image_button_fade_in( tracker_exit_btn, 300, 0 );
    wf_image_button_fade_in( tracker_trash_btn, 300, 100 );
    wf_image_button_fade_in( tracker_location_icon, 300, 200 );
}

/**
 * @brief call back function if the current tile hibernate
 * 
 */
void tracker_app_main_hibernate_cb( void ) {
#ifdef NATIVE_64BIT
#else
    watchface_enable_tile_after_wakeup( tracker_block_watchface );
#endif
    blectl_set_show_notification( tracker_block_show_messages );

    display_set_block_return_maintile( tracker_block_return_maintile );

    tracker_config.save();
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
    static int counter = 0;

    if( !tracker_gps_data )
        tracker_gps_data = (gps_data_t*)CALLOC( 1, sizeof( gps_data_t ) );

    ASSERT( tracker_gps_data, "tracker gps data alloc failed" );

    switch( event ) {
        case GPSCTL_FIX:
            tracker_logging_gps_state = true;
            if( tracker_logging_state )
                lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_GREEN );
            counter = 0;
            break;
        case GPSCTL_NOFIX:
            tracker_logging_gps_state = false;
            if( tracker_logging_state )
                lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLUE );
            counter = 0;
            break;
        case GPSCTL_ENABLE:
            counter = 0;
            break;
        case GPSCTL_DISABLE:
            tracker_logging_gps_state = false;
            tracker_logging_state = false;
            lv_obj_set_style_local_line_color( tracker_progress_arc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_RED );
            lv_arc_set_end_angle( tracker_progress_arc, 0 );
            counter = 0;
            break;
        case GPSCTL_UPDATE_LOCATION:
            if( tracker_logging_gps_state && tracker_logging_state ) {
                /**
                 * calc distance between current gps pos and last gpx pos and avoid large distance jumps
                 */
                if( gpsctl_distance( tracker_gps_data->lat, tracker_gps_data->lon, gps_data->lat, gps_data->lon, EARTH_RADIUS_KM ) < 1.0f ) {
                    distance += gpsctl_distance( tracker_gps_data->lat, tracker_gps_data->lon, gps_data->lat, gps_data->lon, EARTH_RADIUS_KM );
                    course = gpsctl_courseTo( tracker_gps_data->lat, tracker_gps_data->lon, gps_data->lat, gps_data->lon );
                }
                /**
                 * store gpx pos into gpx file at current interval /tracker.json -> interval in sec.
                 */
                if( counter >= tracker_config.interval ) {
                    tracker_app_main_logging( true, gps_data );
                    tracker_app_view_add_data( gps_data );
                    counter = 0;
                }
                counter++;
                wf_label_printf( tracker_info_label, mainbar_get_tile_obj( tracker_app_get_app_main_tile_num() ), LV_ALIGN_IN_TOP_MID, 0, THEME_PADDING, "%.3fkm\n%.0f°", distance, course );
                lv_label_set_align( tracker_info_label, LV_LABEL_ALIGN_CENTER);
            }
            memcpy( tracker_gps_data, gps_data, sizeof( gps_data_t ) );
            break;
        case GPSCTL_UPDATE_ALTITUDE:
            break;
        case GPSCTL_UPDATE_SOURCE:
            break;
        case GPSCTL_UPDATE_SPEED:
            break;
        case GPSCTL_UPDATE_SATELLITE:
            break;
        default:
            break;
    }

    return( true );
}

/**
 * @brief write a .gpx log file.
 * 
 * @param start         true open a new file or continue the old file, false close the current file
 * @param gps_data      gpx_data_t pointer
 * @return const char*  pointer to a current file or NULL
 */
const char *tracker_app_main_logging( bool start, gps_data_t *gps_data ) {
    static bool logging = false;        /** @brief logging state variable, true for loggings active **/
    static char *filename = NULL;       /** @brief current full file name, if NULL no action **/
    static size_t size = 0;             /** @brief file size write counter **/
    
    FILE *fp;
    time_t now;
    struct tm info;
    time( &now );
    localtime_r( &now, &info );

    if( start ) {
        if( !logging ) {
            if( filename )
                return( "error" );
            filename = (char*)MALLOC_ASSERT( 256, "malloc failed" );
            /**
             * gen filename with timestamp and add storage prefix
             */
            char temp_filename[ 128 ];
            strftime( temp_filename, 128, GPX_LOGFILE, &info );
            char prefix_filename[ 128 ] = "";
            strncat( prefix_filename, tracker_config.storage, sizeof( prefix_filename ) );
            strncat( prefix_filename, temp_filename, sizeof( prefix_filename ) );
            /**
             * convert filename to local filename
             */
            filepath_convert( filename, 256, prefix_filename );
            /**
             * create file
             */
            fp = fopen( filename, "w+" );
            if( fp ) {
                /**
                 * write gpx file header ...
                 */
                size_t size = 0;
                size += fprintf( fp, GPX_HEADER );
                size += fprintf( fp, GPX_START );
                size += fprintf( fp, GPX_METADATA );
                size += fprintf( fp, GPX_TRACK_START );
                size += fprintf( fp, GPX_TRACK_SEGMENT_START );
                fclose( fp );
                logging = true;
                wf_label_printf( tracker_file_info_label, mainbar_get_tile_obj( tracker_app_get_app_main_tile_num() ), LV_ALIGN_IN_BOTTOM_MID, 0, -THEME_PADDING, "%d bytes writen", size );
            }
            else {
                free( filename );
                filename = NULL;
                logging = false;
                size = 0;
            }
        }
        else {
            if( !gps_data )
                return( "error" );
            /**
             * open file for append
             */
            fp = fopen( filename, "at" );
            if( fp ) {
                /**
                 * write tracking pointer
                 */
                char timestamp[ 128 ];
                strftime( timestamp, 128, GPX_TRACK_SEGMENT_POINT_TIME_SRF, &info );
                size += fprintf( fp, GPX_TRACK_SEGMENT_POINT_START, gps_data->lat, gps_data->lon );
                size += fprintf( fp, GPX_TRACK_SEGMENT_POINT_ELE, gps_data->altitude_meters );
                size += fprintf( fp, GPX_TRACK_SEGMENT_POINT_TIME, timestamp );
                size += fprintf( fp, GPX_TRACK_SEGMENT_POINT_END );
                fclose( fp );
                wf_label_printf( tracker_file_info_label, mainbar_get_tile_obj( tracker_app_get_app_main_tile_num() ), LV_ALIGN_IN_BOTTOM_MID, 0, -THEME_PADDING, "%d bytes writen", size );
            }
            else {
                free( filename );
                filename = NULL;
                logging = false;
                size = 0;
            }
        }
    }
    else {
        fp = fopen( filename, "at" );
        if( fp ) {
            /**
             * close file
             */
            size += fprintf( fp, GPX_TRACK_SEGMENT_END );
            size += fprintf( fp, GPX_TRACK_END );
            size += fprintf( fp, GPX_END );
            fclose( fp );
            wf_label_printf( tracker_file_info_label, mainbar_get_tile_obj( tracker_app_get_app_main_tile_num() ), LV_ALIGN_IN_BOTTOM_MID, 0, -THEME_PADDING, "%d bytes writen", size );
        }
        free( filename );
        filename = NULL;
        logging = false;
        size = 0;
    }
    return( filename ? filename : "" );
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
                    tracker_logging_state = true;
                    distance = 0.0f;
                    gpsctl_on();
                    sdcard_block_unmounting( true );
                    tracker_app_main_logging( true, NULL );
                    tracker_app_view_clean_data();
                    tracker_gps_on_standby_state = gpsctl_get_enable_on_standby();
                    gpsctl_set_enable_on_standby( true );
                    gadgetbridge_send_msg( "\r\n{\"t\":\"info\",\"msg\":\"gps tracker started\"}\r\n" );
                }
                else {
                    tracker_logging_state = false;
                    gpsctl_off();
                    sdcard_block_unmounting( false );
                    gpsctl_set_enable_on_standby( tracker_gps_on_standby_state );
                    gadgetbridge_send_msg( "\r\n{\"t\":\"info\",\"msg\":\"gps tracker stoped\"}\r\n" );
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
static void tracker_app_main_enter_trash_cb( lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case LV_EVENT_CLICKED: {
            char path[512] = "";
            DIR *d;
            struct dirent *dir;

            #ifdef NATIVE_64BIT
                filepath_convert( path, sizeof( path ), tracker_config.storage );
                strncat( path, "/.", sizeof( path ) );
                chdir( path );
                d = opendir( "." );
            #else
                strncat( path, tracker_config.storage, sizeof( path ) );
                d = opendir( path );
            #endif

            if( d ) {
                int file_count = 0;
                while( ( dir = readdir( d ) ) != NULL ) {
                    if( strstr( dir->d_name, ".gpx" ) ) {
                        char filename[ 256 ] = "";
                        snprintf( path, sizeof( path ), "%s/%s", tracker_config.storage, dir->d_name );
                        filepath_convert( filename, sizeof( filename ), path );
                        remove( filename );
                        file_count++;
                    }
                }
                closedir( d );
                wf_label_printf( tracker_file_info_label, mainbar_get_tile_obj( tracker_app_get_app_main_tile_num() ), LV_ALIGN_IN_BOTTOM_MID, 0, -THEME_PADDING, "remove %d files", file_count );
            }
            break;    
        }
    }
}
