/****************************************************************************
 *   Aug 3 12:17:11 2020
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

#include "osm_app.h"
#include "osm_app_main.h"

#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_styles.h"

#include "hardware/display.h"
#include "hardware/gpsctl.h"

#include "utils/osm_helper/osm_helper.h"

EventGroupHandle_t osm_map_event_handle = NULL;
TaskHandle_t _osm_map_download_Task;
lv_task_t * osm_map_main_tile_task;

lv_obj_t *osm_app_main_tile = NULL;
lv_obj_t *osm_app_direction_img = NULL;
lv_obj_t *osm_app_pos_img = NULL;
lv_obj_t * exit_btn = NULL;
lv_obj_t * zoom_in_btn = NULL;
lv_obj_t * zoom_out_btn = NULL;

lv_style_t osm_app_main_style;

static bool osm_app_active = false;
static bool osm_block_return_maintile = false;
static bool osm_statusbar_force_dark_mode = false;
osm_location_t osm_location;

lv_img_dsc_t *osm_map_data[2];

LV_IMG_DECLARE(exit_dark_48px);
LV_IMG_DECLARE(zoom_in_dark_48px);
LV_IMG_DECLARE(zoom_out_dark_48px);
LV_IMG_DECLARE(osm_64px);
LV_IMG_DECLARE(info_fail_16px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

void osm_map_main_tile_update_task( lv_task_t * task );
void osm_map_update_request( void );
void osm_map_update_Task( void * pvParameters );
static void zoom_in_osm_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void zoom_out_osm_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_osm_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
void osm_update_map( osm_location_t *osm_location, double lon, double lat, uint32_t zoom );
bool osm_gpsctl_event_cb( EventBits_t event, void *arg );
void osm_activate_cb( void );
void osm_hibernate_cb( void );

void osm_app_main_setup( uint32_t tile_num ) {

    osm_app_main_tile = mainbar_get_tile_obj( tile_num );

    lv_style_copy( &osm_app_main_style, ws_get_mainbar_style() );
    lv_obj_add_style( osm_app_main_tile, LV_OBJ_PART_MAIN, &osm_app_main_style );

    lv_obj_t *osm_cont = lv_obj_create( osm_app_main_tile, NULL );
    lv_obj_set_size(osm_cont, lv_disp_get_hor_res( NULL ) , lv_disp_get_ver_res( NULL ) );
    lv_obj_add_style( osm_cont, LV_OBJ_PART_MAIN, &osm_app_main_style );
    lv_obj_align( osm_cont, osm_app_main_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );

    osm_app_direction_img = lv_img_create( osm_cont, NULL );
    lv_obj_set_width( osm_app_direction_img, lv_disp_get_hor_res( NULL ) );
    lv_obj_set_height( osm_app_direction_img, lv_disp_get_ver_res( NULL ) );
    lv_img_set_src( osm_app_direction_img, &osm_64px );
//    lv_img_set_zoom( osm_app_direction_img, 240 );
    lv_obj_align( osm_app_direction_img, osm_cont, LV_ALIGN_CENTER, 0, 0 );

    osm_app_pos_img = lv_img_create( osm_cont, NULL );
    lv_img_set_src( osm_app_pos_img, &info_fail_16px );
    lv_obj_align( osm_app_pos_img, osm_cont, LV_ALIGN_IN_TOP_LEFT, 120, 120 );
    lv_obj_set_hidden( osm_app_pos_img, true );

    exit_btn = lv_imgbtn_create( osm_cont, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_dark_48px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_dark_48px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_dark_48px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_dark_48px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &osm_app_main_style );
    lv_obj_align( exit_btn, osm_cont, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_osm_app_main_event_cb );

    zoom_in_btn = lv_imgbtn_create( osm_cont, NULL);
    lv_imgbtn_set_src( zoom_in_btn, LV_BTN_STATE_RELEASED, &zoom_in_dark_48px);
    lv_imgbtn_set_src( zoom_in_btn, LV_BTN_STATE_PRESSED, &zoom_in_dark_48px);
    lv_imgbtn_set_src( zoom_in_btn, LV_BTN_STATE_CHECKED_RELEASED, &zoom_in_dark_48px);
    lv_imgbtn_set_src( zoom_in_btn, LV_BTN_STATE_CHECKED_PRESSED, &zoom_in_dark_48px);
    lv_obj_add_style( zoom_in_btn, LV_IMGBTN_PART_MAIN, &osm_app_main_style );
    lv_obj_align( zoom_in_btn, osm_cont, LV_ALIGN_IN_TOP_RIGHT, -10, 10 + STATUSBAR_HEIGHT );
    lv_obj_set_event_cb( zoom_in_btn, zoom_in_osm_app_main_event_cb );

    zoom_out_btn = lv_imgbtn_create( osm_cont, NULL);
    lv_imgbtn_set_src( zoom_out_btn, LV_BTN_STATE_RELEASED, &zoom_out_dark_48px);
    lv_imgbtn_set_src( zoom_out_btn, LV_BTN_STATE_PRESSED, &zoom_out_dark_48px);
    lv_imgbtn_set_src( zoom_out_btn, LV_BTN_STATE_CHECKED_RELEASED, &zoom_out_dark_48px);
    lv_imgbtn_set_src( zoom_out_btn, LV_BTN_STATE_CHECKED_PRESSED, &zoom_out_dark_48px);
    lv_obj_add_style( zoom_out_btn, LV_IMGBTN_PART_MAIN, &osm_app_main_style );
    lv_obj_align( zoom_out_btn, osm_cont, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( zoom_out_btn, zoom_out_osm_app_main_event_cb );

    mainbar_add_tile_activate_cb( tile_num, osm_activate_cb );
    mainbar_add_tile_hibernate_cb( tile_num, osm_hibernate_cb );
    gpsctl_register_cb( GPSCTL_SET_APP_LOCATION | GPSCTL_UPDATE_LOCATION, osm_gpsctl_event_cb, "osm" );

    osm_helper_location_update( &osm_location, osm_location.lon , osm_location.lat, osm_location.zoom );

    osm_map_data[0] = NULL;
    osm_map_data[1] = NULL;

    osm_location.tilex_dest_px_res = lv_disp_get_hor_res( NULL );
    osm_location.tiley_dest_px_res = lv_disp_get_ver_res( NULL );

    osm_map_event_handle = xEventGroupCreate();

    osm_map_main_tile_task = lv_task_create( osm_map_main_tile_update_task, 250, LV_TASK_PRIO_MID, NULL );
}

void osm_map_main_tile_update_task( lv_task_t * task ) {
    /*
     * check if maintile alread initialized
     */
    if ( osm_app_active ) {
        if ( lv_disp_get_inactive_time( NULL ) > 5000 ) {
            lv_obj_set_hidden( exit_btn, true );
            lv_obj_set_hidden( zoom_in_btn, true );
            lv_obj_set_hidden( zoom_out_btn, true );
            statusbar_hide( true );
            statusbar_expand( false );
        }
        else {
            lv_obj_set_hidden( exit_btn, false );
            lv_obj_set_hidden( zoom_in_btn, false );
            lv_obj_set_hidden( zoom_out_btn, false );
            statusbar_hide( false );
        }
    }
}

bool osm_gpsctl_event_cb( EventBits_t event, void *arg ) {
    gps_data_t *gps_data = NULL;
    
    switch ( event ) {
        case GPSCTL_SET_APP_LOCATION:
            /**
             * update location and tile map image on new location
             */
            gps_data = ( gps_data_t *)arg;
            osm_location.lon = gps_data->lon;
            osm_location.lat = gps_data->lat;
            if( !osm_location.zoom_valid ) {
                osm_location.zoom = 16;
                osm_location.zoom_valid = true;
            }
            if ( osm_app_active )
                osm_map_update_request();
            break;
        case GPSCTL_UPDATE_LOCATION:
            /**
             * update location and tile map image on new location
             */
            gps_data = ( gps_data_t *)arg;
            osm_location.lon = gps_data->lon;
            osm_location.lat = gps_data->lat;
            if ( osm_app_active )
                osm_map_update_request();
            break;
    }
    return( true );
}


void osm_map_update_request( void ) {
    /**
     * check if another osm tile image update is running
     */
    if ( xEventGroupGetBits( osm_map_event_handle ) & OSM_APP_DOWNLOAD_REQUEST ) {
        return;
    }
    else {
        /**
         * start separate osm tile image update Task
         */
        xEventGroupSetBits( osm_map_event_handle, OSM_APP_DOWNLOAD_REQUEST );
        xTaskCreate(    osm_map_update_Task,      /* Function to implement the task */
                        "osm map uupdate Task",    /* Name of the task */
                        5000,                            /* Stack size in words */
                        NULL,                            /* Task input parameter */
                        1,                               /* Priority of the task */
                        &_osm_map_download_Task );  /* Task handle. */ 
    }
}

void osm_map_update_Task( void * pvParameters ) {
    osm_update_map( &osm_location, osm_location.lon, osm_location.lat, osm_location.zoom );
    lv_obj_align( osm_app_pos_img, lv_obj_get_parent( osm_app_pos_img ), LV_ALIGN_IN_TOP_LEFT, osm_location.tilex_pos - 8, osm_location.tiley_pos - 8 );
    lv_obj_set_hidden( osm_app_pos_img, false );
    xEventGroupClearBits( osm_map_event_handle, OSM_APP_DOWNLOAD_REQUEST );
    vTaskDelete( NULL );    
}


void osm_update_map( osm_location_t *osm_location, double lon, double lat, uint32_t zoom ) {
    static uint8_t osm_map_data_index = 0;
    /**
     * check if an tile map update necessitate
     */
    if ( osm_helper_location_update( osm_location, lon , lat, zoom ) ) {
        /**
         * use next lv_img_dsc
         */ 
        osm_map_data_index++;
        osm_map_data_index %= 2;
        /**
         * update osm tile map image
         */
        osm_map_data[ osm_map_data_index ] = osm_helper_get_tile_image( osm_location, osm_map_data[ osm_map_data_index ] );
        /**
         * if an image update was successfull, update img_src
         */
        if ( osm_map_data[ osm_map_data_index ] ) {
            lv_img_set_src( osm_app_direction_img, osm_map_data[ osm_map_data_index ] );
            lv_obj_align( osm_app_direction_img, osm_app_main_tile, LV_ALIGN_CENTER, 0, 0 );
            lv_obj_invalidate( lv_scr_act() );
        }
    }

}

static void exit_osm_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            /**
             * exit to mainbar
             */
            mainbar_jump_to_maintile( LV_ANIM_OFF );
            break;
    }
}

static void zoom_in_osm_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   
            /**
             * increase zoom level
             */
            if ( osm_location.zoom < 18 ) {
                osm_location.zoom++;
                osm_map_update_request();
            }
            break;
    }
}

static void zoom_out_osm_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   
            /**
             * decrease zoom level
             */
            if ( osm_location.zoom > 4 ) {
                osm_location.zoom--;
                osm_map_update_request();
            }
            break;
    }
}

void osm_activate_cb( void ) {
    /**
     * save black return to maintile state
     */
    osm_block_return_maintile = display_get_block_return_maintile();
    display_set_block_return_maintile( true );
    /**
     * save statusbar force dark mode state
     */
    osm_statusbar_force_dark_mode = statusbar_get_force_dark();
    statusbar_set_force_dark( true );
    /**
     * force redraw screen
     */
    lv_img_cache_invalidate_src( osm_map_data[ 0 ] );
    lv_img_cache_invalidate_src( osm_map_data[ 1 ] );
    lv_obj_invalidate( lv_scr_act() );
    statusbar_expand( true );
    statusbar_expand( false );
    /**
     * set osm app active
     */
    osm_app_active = true;
    osm_map_update_request();
}

void osm_hibernate_cb( void ) {
    /**
     * restore back to maintile and status force dark mode
     */
    display_set_block_return_maintile( osm_block_return_maintile );
    statusbar_set_force_dark( osm_statusbar_force_dark_mode );
    /**
     * set osm app inactive
     */
    osm_app_active = false;
    statusbar_hide( false );
}
