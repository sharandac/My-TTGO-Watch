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
#include <stdio.h>
#include "config.h"

#include "mainbar.h"
#include "main_tile/main_tile.h"
#include "setup_tile/setup.h"
#include "note_tile/note_tile.h"
#include "app_tile/app_tile.h"
#include "gui/keyboard.h"

#include "setup_tile/battery_settings/battery_settings.h"
#include "setup_tile/wlan_settings/wlan_settings.h"
#include "setup_tile/move_settings/move_settings.h"
#include "setup_tile/display_settings/display_settings.h"
#include "setup_tile/time_settings/time_settings.h"
#include "setup_tile/update/update.h"

static lv_style_t mainbarstyle;
static lv_obj_t *mainbar = NULL;

static lv_point_t valid_pos[ TILE_NUM ];

lv_tile_entry_t tile_entry[ TILE_NUM ] {
    { NULL, TILE_TYPE_MAIN_TILE, MAIN_TILE, main_tile_setup, { 0 , 0 } },
    { NULL, TILE_TYPE_APP_TILE, APP_TILE, app_tile_setup, { 1 , 0 } },
    { NULL, TILE_TYPE_NOTE_TILE, NOTE_TILE, note_tile_setup, { 0 , 1 } },
    { NULL, TILE_TYPE_KEYBOARD_TILE, KEYBOARD_TILE, NULL, { 0 , 6 } },
    { NULL, TILE_TYPE_SETUP_TILE, SETUP_TILE, setup_tile_setup, { 1 , 1 } },
    { NULL, TILE_TYPE_SETUP, WLAN_SETTINGS_TILE, wlan_settings_tile_setup, { 0,3 } },
    { NULL, TILE_TYPE_SETUP, WLAN_PASSWORD_TILE, wlan_password_tile_setup, { 0,4 } },
    { NULL, TILE_TYPE_SETUP, MOVE_SETTINGS_TILE, move_settings_tile_setup, { 2,3 } },
    { NULL, TILE_TYPE_SETUP, DISPLAY_SETTINGS_TILE, display_settings_tile_setup, { 4,3 } },
    { NULL, TILE_TYPE_SETUP, BATTERY_SETTINGS_TILE, battery_settings_tile_setup, { 6,3 } },
    { NULL, TILE_TYPE_SETUP, TIME_SETTINGS_TILE, time_settings_tile_setup, { 8,3 } },
    { NULL, TILE_TYPE_SETUP, UPDATE_SETTINGS_TILE, update_tile_setup, { 10,3 } },
    { NULL, TILE_TYPE_WIDGET_TILE, WIDGET1_1_TILE, NULL, { 12,3 } },
    { NULL, TILE_TYPE_WIDGET_SETUP, WIDGET1_2_TILE, NULL, { 12,4 } },
    { NULL, TILE_TYPE_WIDGET_TILE, WIDGET2_1_TILE, NULL, { 14,3 } },
    { NULL, TILE_TYPE_WIDGET_SETUP, WIDGET2_2_TILE, NULL, { 14,4 } }
};

void mainbar_setup( void ) {

    lv_style_init( &mainbarstyle );
    lv_style_set_radius(&mainbarstyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color(&mainbarstyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa(&mainbarstyle, LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_border_width(&mainbarstyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color(&mainbarstyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_image_recolor(&mainbarstyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    
    mainbar = lv_tileview_create( lv_scr_act(), NULL);
    lv_tileview_set_valid_positions(mainbar, valid_pos, TILE_NUM );
    lv_tileview_set_edge_flash(mainbar, false);
    lv_obj_add_style( mainbar, LV_OBJ_PART_MAIN, &mainbarstyle );
    lv_page_set_scrlbar_mode(mainbar, LV_SCRLBAR_MODE_OFF);

    for( int tile = 0 ; tile < TILE_NUM ; tile++ ) {
        tile_entry[ tile ].tile = lv_obj_create( mainbar, NULL);
        lv_obj_set_size( tile_entry[ tile ].tile, LV_HOR_RES, LV_VER_RES);
        lv_obj_reset_style_list( tile_entry[ tile ].tile, LV_OBJ_PART_MAIN );
        lv_obj_add_style( tile_entry[ tile ].tile, LV_OBJ_PART_MAIN, &mainbarstyle );
        lv_obj_set_pos( tile_entry[ tile ].tile, tile_entry[ tile ].pos.x * LV_HOR_RES , tile_entry[ tile ].pos.y * LV_VER_RES );
        lv_tileview_add_element( mainbar, tile_entry[ tile ].tile);
        if ( tile_entry[ tile ].tilecallback != NULL )
            tile_entry[ tile ].tilecallback( tile_entry[ tile ].tile, &mainbarstyle, LV_HOR_RES , LV_VER_RES );
        valid_pos[ tile ].x = tile_entry[ tile ].pos.x;
        valid_pos[ tile ].y = tile_entry[ tile ].pos.y;
    }
    mainbar_jump_to_maintile( LV_ANIM_OFF );
}

lv_obj_t * mainbar_get_tile_obj( lv_tile_number tile_number ) {
    if ( tile_number < TILE_NUM ) {
        for ( int tile = 0 ; tile < TILE_NUM; tile++ ) {
            if ( tile_entry[ tile ].tile_number == tile_number ) {
                return( tile_entry[ tile ].tile );
            }
        }
    }
    return( NULL );
}

lv_tile_number mainbar_get_next_free_tile( lv_tile_type tile_type ) {
    for ( int tile = 0 ; tile < TILE_NUM; tile++ ) {
        if ( tile_entry[ tile ].tile_type == tile_type && tile_entry[ tile ].tilecallback == NULL ) {
            return( tile_entry[ tile ].tile_number );
        }
    }
    return( NO_TILE );
}

void mainbar_set_tile_setup_cb( lv_tile_number tile_number, TILE_CALLBACK_FUNC callback ) {
    if ( tile_number < TILE_NUM ) {
        tile_entry[ tile_number ].tilecallback = callback;
        tile_entry[ tile_number ].tilecallback( tile_entry[ tile_number ].tile, &mainbarstyle, LV_HOR_RES , LV_VER_RES );
    }
}


void mainbar_jump_to_tile( lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim ) {
    lv_tileview_set_tile_act( mainbar, x, y, anim );
}

void mainbar_jump_to_tilenumber( lv_tile_number tile_number, lv_anim_enable_t anim ) {
    for ( int tile = 0 ; tile < TILE_NUM; tile++ ) {
        if ( tile_entry[ tile ].tile_number == tile_number ) {
            lv_tileview_set_tile_act( mainbar, tile_entry[ tile ].pos.x, tile_entry[ tile ].pos.y, anim );
            break;
        }
    }
}

void mainbar_jump_to_maintile( lv_anim_enable_t anim ) {
    for ( int tile = 0 ; tile < TILE_NUM; tile++ ) {
        if ( tile_entry[ tile ].tile_number == MAIN_TILE ) {
            lv_tileview_set_tile_act(mainbar, tile_entry[ tile ].pos.x, tile_entry[ tile ].pos.y, anim );
            break;
        }
    }
}