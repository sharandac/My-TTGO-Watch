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
#include "hardware/blectl.h"

#include "utils/osm_map/osm_map.h"

EventGroupHandle_t osm_map_event_handle = NULL;     /** @brief osm tile image update event queue */
TaskHandle_t _osm_map_download_Task;                /** @brief osm tile image update Task */
lv_task_t *osm_map_main_tile_task;                  /** @brief osm active/inactive task for show/hide user interface */

lv_obj_t *osm_app_main_tile = NULL;                 /** @brief osm main tile obj */
lv_obj_t *osm_app_tile_img = NULL;                  /** @brief osm tile image obj */
lv_obj_t *osm_app_pos_img = NULL;                   /** @brief osm position point obj */
lv_obj_t *exit_btn = NULL;                          /** @brief osm exit icon/button obj */
lv_obj_t *zoom_in_btn = NULL;                       /** @brief osm zoom in icon/button obj */
lv_obj_t *zoom_out_btn = NULL;                      /** @brief osm zoom out icon/button obj */

lv_style_t osm_app_main_style;                      /** @brief osm main styte obj */

static bool osm_app_active = false;                 /** @brief osm app active/inactive flag, true means active */
static bool osm_block_return_maintile = false;      /** @brief osm block to maintile state store */
static bool osm_block_show_messages = false;        /** @brief osm show messages state store */
static bool osm_statusbar_force_dark_mode = false;  /** @brief osm statusbar force dark mode state store */

osm_location_t *osm_location;                       /** @brief osm location obj */

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
    lv_obj_set_size(osm_cont, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) );
    lv_obj_add_style( osm_cont, LV_OBJ_PART_MAIN, &osm_app_main_style );
    lv_obj_align( osm_cont, osm_app_main_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );

    osm_app_tile_img = lv_img_create( osm_cont, NULL );
    lv_obj_set_width( osm_app_tile_img, lv_disp_get_hor_res( NULL ) );
    lv_obj_set_height( osm_app_tile_img, lv_disp_get_ver_res( NULL ) );
    lv_img_set_src( osm_app_tile_img, &osm_64px );
    lv_obj_align( osm_app_tile_img, osm_cont, LV_ALIGN_CENTER, 0, 0 );

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

    osm_location = osm_map_create_location_obj();
    osm_map_event_handle = xEventGroupCreate();
    osm_map_main_tile_task = lv_task_create( osm_map_main_tile_update_task, 250, LV_TASK_PRIO_MID, NULL );
}

/**
 * @brief when osm is active, this task get the use inactive time and hide
 * the statusbar and icon.
 */
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
            osm_map_set_lon_lat( osm_location, gps_data->lon, gps_data->lat );
            if ( osm_app_active )
                osm_map_update_request();
            break;
        case GPSCTL_UPDATE_LOCATION:
            /**
             * update location and tile map image on new location
             */
            gps_data = ( gps_data_t *)arg;
            osm_map_set_lon_lat( osm_location, gps_data->lon, gps_data->lat );
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
    if ( xEventGroupGetBits( osm_map_event_handle ) & OSM_APP_TILE_IMAGE_REQUEST ) {
        return;
    }
    else {
        xEventGroupSetBits( osm_map_event_handle, OSM_APP_TILE_IMAGE_REQUEST );
    }
}

void osm_map_update_Task( void * pvParameters ) {
    log_i("start osm map tile background update task, heap: %d", ESP.getFreeHeap() );
    while( 1 ) {
        /**
         * check if a tile image update is requested
         */
        if ( xEventGroupGetBits( osm_map_event_handle ) & OSM_APP_TILE_IMAGE_REQUEST ) {
            /**
             * check if a tile image update is required and update them
             */
            if( osm_map_update( osm_location ) ) {
                if ( osm_map_get_tile_image( osm_location ) ) {
                    lv_img_set_src( osm_app_tile_img, osm_map_get_tile_image( osm_location ) );
                }
                lv_obj_align( osm_app_tile_img, lv_obj_get_parent( osm_app_tile_img ), LV_ALIGN_CENTER, 0 , 0 );
            }
            /**
             * update postion point on the tile image
             */
            lv_obj_align( osm_app_pos_img, lv_obj_get_parent( osm_app_pos_img ), LV_ALIGN_IN_TOP_LEFT, osm_location->tilex_pos - 8 , osm_location->tiley_pos - 8 );
            lv_obj_set_hidden( osm_app_pos_img, false );
            /**
             * clear update request flag
             */
            xEventGroupClearBits( osm_map_event_handle, OSM_APP_TILE_IMAGE_REQUEST );
        }
        /**
         * check if for a task exit request
         */
        if ( xEventGroupGetBits( osm_map_event_handle ) & OSM_APP_TASK_EXIT_REQUEST ) {
            xEventGroupClearBits( osm_map_event_handle, OSM_APP_TASK_EXIT_REQUEST );
            break;
        }
        /**
         * block this task for 125ms
         */
        vTaskDelay( 125 );
    }
    log_i("finsh osm map tile background update task, heap: %d", ESP.getFreeHeap() );
    vTaskDelete( NULL );    
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
            osm_map_zoom_in( osm_location );
            break;
    }
}

static void zoom_out_osm_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   
            /**
             * decrease zoom level
             */
            osm_map_zoom_out( osm_location );
            break;
    }
}

void osm_activate_cb( void ) {
    /**
     * save block show messages state
     */
    osm_block_show_messages = blectl_get_show_notification();
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
    lv_obj_invalidate( lv_scr_act() );
    statusbar_expand( true );
    statusbar_expand( false );
    /**
     * set osm app active
     */
    osm_app_active = true;
    /**
     * start background osm tile image update Task
     */
    xTaskCreate(    osm_map_update_Task,      /* Function to implement the task */
                    "osm map update Task",    /* Name of the task */
                    10000,                            /* Stack size in words */
                    NULL,                            /* Task input parameter */
                    1,                               /* Priority of the task */
                    &_osm_map_download_Task );  /* Task handle. */

    osm_map_update_request();
}

void osm_hibernate_cb( void ) {
    /**
     * restore back to maintile and status force dark mode
     */
    blectl_set_show_notification( osm_block_show_messages );
    display_set_block_return_maintile( osm_block_return_maintile );
    statusbar_set_force_dark( osm_statusbar_force_dark_mode );
    /**
     * set osm app inactive
     */
    osm_app_active = false;
    statusbar_hide( false );
    /**
     * stop background osm tile image update Task
     */
    xEventGroupSetBits( osm_map_event_handle, OSM_APP_TASK_EXIT_REQUEST );
}
