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

static lv_tile_t *tile = NULL;
static lv_point_t *tile_pos_table = NULL;
static uint32_t tile_entrys = 0;
static uint32_t app_tile_pos = MAINBAR_APP_TILE_X_START;

void mainbar_setup( void ) {
    lv_style_init( &mainbarstyle );
    lv_style_set_radius(&mainbarstyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color(&mainbarstyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa(&mainbarstyle, LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_border_width(&mainbarstyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color(&mainbarstyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_image_recolor(&mainbarstyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);

    mainbar = lv_tileview_create( lv_scr_act(), NULL);
    lv_tileview_set_valid_positions(mainbar, tile_pos_table, tile_entrys );
    lv_tileview_set_edge_flash( mainbar, false);
    lv_obj_add_style( mainbar, LV_OBJ_PART_MAIN, &mainbarstyle );
    lv_page_set_scrlbar_mode(mainbar, LV_SCRLBAR_MODE_OFF);
}

uint32_t mainbar_add_tile( uint16_t x, uint16_t y ) {
    
    tile_entrys++;

    if ( tile_pos_table == NULL ) {
        tile_pos_table = ( lv_point_t * )malloc( sizeof( lv_point_t ) * tile_entrys );
        tile = ( lv_tile_t * )malloc( sizeof( lv_tile_t ) * tile_entrys );
    }
    else {
        tile_pos_table = ( lv_point_t * )realloc( tile_pos_table, sizeof( lv_point_t ) * tile_entrys );
        tile = ( lv_tile_t * )realloc( tile, sizeof( lv_tile_t ) * tile_entrys );
    }

    if ( tile_pos_table == NULL || tile == NULL ) {
        log_e("memory alloc faild");
        while(true);
    }

    tile_pos_table[ tile_entrys - 1 ].x = x;
    tile_pos_table[ tile_entrys - 1 ].y = y;

    lv_obj_t *my_tile = lv_obj_create( mainbar, NULL);  
    tile[ tile_entrys - 1 ].tile = my_tile;
    lv_obj_set_size( tile[ tile_entrys - 1 ].tile, LV_HOR_RES, LV_VER_RES);
    lv_obj_reset_style_list( tile[ tile_entrys - 1 ].tile, LV_OBJ_PART_MAIN );
    lv_obj_add_style( tile[ tile_entrys - 1 ].tile, LV_OBJ_PART_MAIN, &mainbarstyle );
    lv_obj_set_pos( tile[ tile_entrys - 1 ].tile, tile_pos_table[ tile_entrys - 1 ].x * LV_HOR_RES , tile_pos_table[ tile_entrys - 1 ].y * LV_VER_RES );
    lv_tileview_add_element( mainbar, tile[ tile_entrys - 1 ].tile );
    lv_tileview_set_valid_positions( mainbar, tile_pos_table, tile_entrys );
    log_d("add tile: x=%d, y=%d", tile_pos_table[ tile_entrys - 1 ].x, tile_pos_table[ tile_entrys - 1 ].y );

    return( tile_entrys - 1 );
}

lv_style_t *mainbar_get_style( void ) {
    return( &mainbarstyle );
}

uint32_t mainbar_add_app_tile( uint16_t x, uint16_t y ) {
    uint32_t retval = -1;

    for ( int hor = 0 ; hor < x ; hor++ ) {
        for ( int ver = 0 ; ver < y ; ver++ ) {
            if ( retval == -1 ) {
                retval = mainbar_add_tile( hor + app_tile_pos, ver + MAINBAR_APP_TILE_Y_START );
            }
            else {
                mainbar_add_tile( hor + app_tile_pos, ver + MAINBAR_APP_TILE_Y_START );
            }
        }
    }
    app_tile_pos = app_tile_pos + x + 1;
    return( retval );
}

lv_obj_t *mainbar_get_tile_obj( uint32_t tile_number ) {
    if ( tile_number < tile_entrys ) {
        return( tile[ tile_number ].tile );
    }
    else {
        log_e("tile number %d do not exist", tile_number );
    }
    return( NULL );
}

void mainbar_jump_to_maintile( lv_anim_enable_t anim ) {
    lv_tileview_set_tile_act( mainbar, 0, 0, anim );
}

void mainbar_jump_to_tile( lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim ) {
    lv_tileview_set_tile_act( mainbar, x, y, anim );
}

void mainbar_jump_to_tilenumber( uint32_t tile_number, lv_anim_enable_t anim ) {
    if ( tile_number < tile_entrys ) {
        lv_tileview_set_tile_act( mainbar, tile_pos_table[ tile_number ].x, tile_pos_table[ tile_number ].y, anim );
    }
    else {
        log_e("tile number %d do not exist", tile_number );
    }
}
