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

#include "calendar.h"
#include "calendar_db.h"
#include "calendar_day.h"
#include "calendar_create.h"

#include "gui/mainbar/mainbar.h"
#include "gui/icon.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "utils/alloc.h"
/**
 * calendar tile store
 */
uint32_t calendar_create_tile_num;                                                    /** @brief allocated calendar overview tile number */

lv_obj_t *calendar_create_hour_list = NULL;
lv_obj_t *calendar_create_min_list = NULL;
/**
 * calendar icon
 */
LV_FONT_DECLARE(Ubuntu_12px);                                                           /** @brief calendar font */

void calendar_create_build_ui( void );
static void calendar_create_exit_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_create_add_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_create_trash_event_cb( lv_obj_t * obj, lv_event_t event );
void calendar_create_activate_cb( void );
void calendar_create_hibernate_cb( void );
/**
 * setup routine for application
 */
void calendar_create_setup( void ) {
    /**
     * register a tile
     */
    calendar_create_tile_num = mainbar_add_app_tile( 1, 1, "calendar day" );
    /**
     * set activation/hibernation call back
     */
    mainbar_add_tile_activate_cb( calendar_create_tile_num, calendar_create_activate_cb );
    mainbar_add_tile_hibernate_cb( calendar_create_tile_num, calendar_create_hibernate_cb );
    /**
     * build calendar day ovweview ui
     */
    calendar_create_build_ui();
}

uint32_t calendar_create_get_tile( void ) {
    return( calendar_create_tile_num );
}

void calendar_create_build_ui( void ) {
    /**
     * get calendar object from tile number
     */
    lv_obj_t *calendar_create_tile = mainbar_get_tile_obj( calendar_create_tile_num );
    /**
     * 
     */
    calendar_create_hour_list = lv_dropdown_create( calendar_create_tile, NULL );
    lv_dropdown_set_options( calendar_create_hour_list, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23" );
    lv_obj_set_size( calendar_create_hour_list, 110, 40 );
    lv_obj_align( calendar_create_hour_list, calendar_create_tile, LV_ALIGN_IN_TOP_LEFT, 5, 5 );
//    lv_obj_set_event_cb( calendar_create_hour_list, );
    /**
     * 
     */
    calendar_create_min_list = lv_dropdown_create( calendar_create_tile, NULL );
    lv_dropdown_set_options( calendar_create_min_list, "0\n15\n30\n45" );
    lv_obj_set_size( calendar_create_min_list, 110, 40 );
    lv_obj_align( calendar_create_min_list, calendar_create_tile, LV_ALIGN_IN_TOP_RIGHT, -5, 5 );
//    lv_obj_set_event_cb( calendar_create_min_list, );
    /**
     * add exit button
     */
    lv_obj_t *exit_button = wf_add_exit_button( calendar_create_tile, calendar_create_exit_event_cb, ws_get_mainbar_style() );
    lv_obj_align( exit_button, calendar_create_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    /**
     * add add button
     */
    lv_obj_t *create_button = wf_add_add_button( calendar_create_tile, calendar_create_add_event_cb, ws_get_mainbar_style() );
    lv_obj_align( create_button, calendar_create_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    /**
     * add trash button
     */
    lv_obj_t *trash_button = wf_add_trash_button( calendar_create_tile, calendar_create_trash_event_cb, ws_get_mainbar_style() );
    lv_obj_align( trash_button, calendar_create_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -10 );
}

static void calendar_create_exit_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if ( event == LV_EVENT_CLICKED ) {
        mainbar_jump_back();
    }
}

static void calendar_create_add_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if ( event == LV_EVENT_CLICKED ) {
        mainbar_jump_back();
    }
}

static void calendar_create_trash_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if ( event == LV_EVENT_CLICKED ) {
        mainbar_jump_back();
    }
}

void calendar_create_activate_cb( void ) {
    /**
     * open calendar date base
     */
    if ( calendar_db_open() ) {
        /**
         * refresh day overview
         */
    }
    else {
        log_e("open calendar date base failed");
    }
}

void calendar_create_hibernate_cb( void ) {
    /**
     * close calendar date base
     */
    calendar_db_close();
}