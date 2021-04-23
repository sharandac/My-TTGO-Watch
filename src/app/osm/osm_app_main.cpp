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
#include "osm_helper.h"
#include "app/osm/images/bg.h"

#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_styles.h"

#include "hardware/display.h"
#include "hardware/gpsctl.h"

#include "utils/json_psram_allocator.h"

lv_obj_t *osm_app_main_tile = NULL;
lv_obj_t *osm_app_direction_img = NULL;
lv_style_t osm_app_main_style;

static bool osm_block_return_maintile = false;
osm_location_t osm_location;

lv_img_dsc_t *osm_map_data[2];

LV_IMG_DECLARE(exit_dark_48px);
LV_IMG_DECLARE(zoom_in_dark_48px);
LV_IMG_DECLARE(zoom_out_dark_48px);
LV_IMG_DECLARE(osm_64px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

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
//    lv_img_set_auto_size( osm_app_direction_img, true );
    lv_img_set_src( osm_app_direction_img, &osm_64px );
    lv_obj_align( osm_app_direction_img, osm_cont, LV_ALIGN_CENTER, 0, 0 );

    lv_obj_t * exit_btn = lv_imgbtn_create( osm_cont, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_dark_48px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_dark_48px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_dark_48px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_dark_48px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &osm_app_main_style );
    lv_obj_align( exit_btn, osm_cont, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_osm_app_main_event_cb );

    lv_obj_t * zoom_in_btn = lv_imgbtn_create( osm_cont, NULL);
    lv_imgbtn_set_src( zoom_in_btn, LV_BTN_STATE_RELEASED, &zoom_in_dark_48px);
    lv_imgbtn_set_src( zoom_in_btn, LV_BTN_STATE_PRESSED, &zoom_in_dark_48px);
    lv_imgbtn_set_src( zoom_in_btn, LV_BTN_STATE_CHECKED_RELEASED, &zoom_in_dark_48px);
    lv_imgbtn_set_src( zoom_in_btn, LV_BTN_STATE_CHECKED_PRESSED, &zoom_in_dark_48px);
    lv_obj_add_style( zoom_in_btn, LV_IMGBTN_PART_MAIN, &osm_app_main_style );
    lv_obj_align( zoom_in_btn, osm_cont, LV_ALIGN_IN_TOP_RIGHT, -10, 10 );
    lv_obj_set_event_cb( zoom_in_btn, zoom_in_osm_app_main_event_cb );

    lv_obj_t * zoom_out_btn = lv_imgbtn_create( osm_cont, NULL);
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
}

bool osm_gpsctl_event_cb( EventBits_t event, void *arg ) {
    gps_data_t *gps_data = NULL;
    
    switch ( event ) {
        case GPSCTL_SET_APP_LOCATION:
            gps_data = ( gps_data_t *)arg;
            osm_update_map( &osm_location, gps_data->lon, gps_data->lat, 16 );
            break;
        case GPSCTL_UPDATE_LOCATION:
            gps_data = ( gps_data_t *)arg;
            osm_update_map( &osm_location, gps_data->lon, gps_data->lat, osm_location.zoom );
            break;
    }
    return( true );
}

void osm_update_map( osm_location_t *osm_location, double lon, double lat, uint32_t zoom ) {
    static uint8_t osm_map_data_index = 0;

    if ( osm_helper_location_update( osm_location, lon , lat, zoom ) ) {
        
        osm_map_data_index++;
        osm_map_data_index %= 2;
        
        osm_map_data[ osm_map_data_index ] = osm_helper_get_tile_image( osm_location, osm_map_data[ osm_map_data_index ] );
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
            mainbar_jump_to_maintile( LV_ANIM_OFF );
            break;
    }
}

static void zoom_in_osm_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   
            if ( osm_location.zoom < 18 ) {
                osm_location.zoom++;
                osm_update_map( &osm_location, osm_location.lon , osm_location.lat, osm_location.zoom );
            }
            break;
    }
}

static void zoom_out_osm_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):   
            if ( osm_location.zoom > 0 ) {
                osm_location.zoom--;
                osm_update_map( &osm_location, osm_location.lon , osm_location.lat, osm_location.zoom );
            }
            break;
    }
}

void osm_activate_cb( void ) {
    osm_block_return_maintile = display_get_block_return_maintile();
    display_set_block_return_maintile( true );
    lv_img_cache_invalidate_src( osm_map_data[ 0 ] );
    lv_img_cache_invalidate_src( osm_map_data[ 1 ] );
    lv_obj_invalidate( lv_scr_act() );
}

void osm_hibernate_cb( void ) {
    display_set_block_return_maintile( osm_block_return_maintile );
}
