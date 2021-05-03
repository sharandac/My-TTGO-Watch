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
#include "setup_tile/setup_tile.h"
#include "note_tile/note_tile.h"
#include "app_tile/app_tile.h"
#include "gui/keyboard.h"
#include "gui/statusbar.h"
#include "gui/widget_styles.h"

#include "hardware/display.h"
#include "hardware/powermgm.h"

#include "utils/alloc.h"

#include "setup_tile/battery_settings/battery_settings.h"
#include "setup_tile/wlan_settings/wlan_settings.h"
#include "setup_tile/move_settings/move_settings.h"
#include "setup_tile/display_settings/display_settings.h"
#include "setup_tile/time_settings/time_settings.h"
#include "setup_tile/update/update.h"

static lv_obj_t *mainbar = NULL;

static lv_tile_t *tile = NULL;
static lv_point_t *tile_pos_table = NULL;
static uint32_t current_tile = 0;
static uint32_t tile_entrys = 0;
static uint32_t app_tile_pos = MAINBAR_APP_TILE_X_START;

static uint32_t mainbar_custom_tile_after_wakeup = 0;
static bool mainbar_custom_tile_after_wakeup_enable = false;

bool mainbar_powermgm_event_cb( EventBits_t event, void *arg );

void mainbar_setup( void ) {
    /*
     * check if mainbar already initialized
     */
    if ( mainbar ) {
        log_e("main already initialized");
        return;
    }

    mainbar = lv_tileview_create( lv_scr_act(), NULL);
    lv_tileview_set_edge_flash( mainbar, false);
    lv_obj_add_style( mainbar, LV_OBJ_PART_MAIN, ws_get_mainbar_style() );
    lv_page_set_scrlbar_mode( mainbar, LV_SCRLBAR_MODE_OFF);
    powermgm_register_cb( POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, mainbar_powermgm_event_cb, "mainbar powermgm" );
}

bool mainbar_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_WAKEUP:
                                        if ( !display_get_block_return_maintile() ) {
                                            if ( mainbar_custom_tile_after_wakeup_enable ) {
                                                mainbar_jump_to_tilenumber( mainbar_custom_tile_after_wakeup, LV_ANIM_OFF );
                                            }
                                            else {
                                                mainbar_jump_to_maintile( LV_ANIM_OFF );
                                            }
                                        }
                                        break;
    }
    return( true );
}

void mainbar_set_custom_tile_after_wakeup( uint32_t tile_number ) {
    mainbar_custom_tile_after_wakeup = tile_number;
}
void mainbar_enable_custom_tile_after_wakeup( bool enable ) {
    mainbar_custom_tile_after_wakeup_enable = enable;
}

uint32_t mainbar_add_tile( uint16_t x, uint16_t y, const char *id ) {
    /*
     * check if mainbar already initialized
     */
    if ( !mainbar ) {
        log_e("main not initialized");
        while( true );
    }

    tile_entrys++;

    if ( tile_pos_table == NULL ) {
        tile_pos_table = ( lv_point_t * )MALLOC( sizeof( lv_point_t ) * tile_entrys );
        if ( tile_pos_table == NULL ) {
            log_e("tile_pos_table malloc faild");
            while(true);
        }
        tile = ( lv_tile_t * )MALLOC( sizeof( lv_tile_t ) * tile_entrys );
        if ( tile == NULL ) {
            log_e("tile malloc faild");
            while(true);
        }
    }
    else {
        lv_point_t *new_tile_pos_table;
        lv_tile_t *new_tile;

        new_tile_pos_table = ( lv_point_t * )REALLOC( tile_pos_table, sizeof( lv_point_t ) * tile_entrys );
        if ( new_tile_pos_table == NULL ) {
            log_e("tile_pos_table realloc faild");
            while(true);
        }
        tile_pos_table = new_tile_pos_table;
        
        new_tile = ( lv_tile_t * )REALLOC( tile, sizeof( lv_tile_t ) * tile_entrys );
        if ( new_tile == NULL ) {
            log_e("tile realloc faild");
            while(true);
        }
        tile = new_tile;
    }

    tile_pos_table[ tile_entrys - 1 ].x = x;
    tile_pos_table[ tile_entrys - 1 ].y = y;

    lv_obj_t *my_tile = lv_cont_create( mainbar, NULL);  
    tile[ tile_entrys - 1 ].tile = my_tile;
    tile[ tile_entrys - 1 ].activate_cb = NULL;
    tile[ tile_entrys - 1 ].hibernate_cb = NULL;
    tile[ tile_entrys - 1 ].x = x;
    tile[ tile_entrys - 1 ].y = y;
    tile[ tile_entrys - 1 ].id = id;
    lv_obj_set_size( tile[ tile_entrys - 1 ].tile, lv_disp_get_hor_res( NULL ), LV_VER_RES);
    //lv_obj_reset_style_list( tile[ tile_entrys - 1 ].tile, LV_OBJ_PART_MAIN );
    lv_obj_add_style( tile[ tile_entrys - 1 ].tile, LV_OBJ_PART_MAIN, ws_get_mainbar_style() );
    lv_obj_set_pos( tile[ tile_entrys - 1 ].tile, tile_pos_table[ tile_entrys - 1 ].x * lv_disp_get_hor_res( NULL ) , tile_pos_table[ tile_entrys - 1 ].y * LV_VER_RES );
    lv_tileview_add_element( mainbar, tile[ tile_entrys - 1 ].tile );
    lv_tileview_set_valid_positions( mainbar, tile_pos_table, tile_entrys );
    log_d("add tile: x=%d, y=%d, id=%s", tile_pos_table[ tile_entrys - 1 ].x, tile_pos_table[ tile_entrys - 1 ].y, tile[ tile_entrys - 1 ].id );

    return( tile_entrys - 1 );
}

bool mainbar_add_tile_hibernate_cb( uint32_t tile_number, MAINBAR_CALLBACK_FUNC hibernate_cb ) {
    /*
     * check if mainbar already initialized
     */
    if ( !mainbar ) {
        log_e("main not initialized");
        while( true );
    }

    if ( tile_number < tile_entrys ) {
        tile[ tile_number ].hibernate_cb = hibernate_cb;
        return( true );
    }
    else {
        log_e("tile number %d do not exist", tile_number );
        return( false );
    }
}

bool mainbar_add_tile_activate_cb( uint32_t tile_number, MAINBAR_CALLBACK_FUNC activate_cb ) {
    /*
     * check if mainbar already initialized
     */
    if ( !mainbar ) {
        log_e("main not initialized");
        while( true );
    }

    if ( tile_number < tile_entrys ) {
        tile[ tile_number ].activate_cb = activate_cb;
        return( true );
    }
    else {
        log_e("tile number %d do not exist", tile_number );
        return( false );
    }
}

uint32_t mainbar_add_app_tile( uint16_t x, uint16_t y, const char *id ) {
    /*
     * check if mainbar already initialized
     */
    if ( !mainbar ) {
        log_e("main not initialized");
        while( true );
    }

    uint32_t retval = -1;

    for ( int hor = 0 ; hor < x ; hor++ ) {
        for ( int ver = 0 ; ver < y ; ver++ ) {
            if ( retval == -1 ) {
                retval = mainbar_add_tile( hor + app_tile_pos, ver + MAINBAR_APP_TILE_Y_START, id );
            }
            else {
                mainbar_add_tile( hor + app_tile_pos, ver + MAINBAR_APP_TILE_Y_START, id );
            }
        }
    }
    app_tile_pos = app_tile_pos + x + 1;
    return( retval );
}

lv_obj_t *mainbar_get_tile_obj( uint32_t tile_number ) {
    /*
     * check if mainbar already initialized
     */
    if ( !mainbar ) {
        log_e("main not initialized");
        while( true );
    }

    if ( tile_number < tile_entrys ) {
        return( tile[ tile_number ].tile );
    }
    else {
        log_e( "tile number %d do not exist", tile_number );
    }
    return( NULL );
}

void mainbar_jump_to_maintile( lv_anim_enable_t anim ) {
    /*
     * check if mainbar already initialized
     */
    if ( !mainbar ) {
        log_e("main not initialized");
        while( true );
    }

    if ( tile_entrys != 0 ) {
        mainbar_jump_to_tilenumber( 0, anim );
        keyboard_hide();
        statusbar_hide( false );
        statusbar_expand( false );
    }
    else {
        log_e( "main tile do not exist" );
    }
}

void mainbar_jump_to_tilenumber( uint32_t tile_number, lv_anim_enable_t anim ) {
    /*
     * check if mainbar already initialized
     */
    if ( !mainbar ) {
        log_e("main not initialized");
        while( true );
    }

    if ( tile_number < tile_entrys ) {
        log_d("jump to tile %d from tile %d", tile_number, current_tile );
        lv_tileview_set_tile_act( mainbar, tile_pos_table[ tile_number ].x, tile_pos_table[ tile_number ].y, anim );
        // call hibernate callback for the current tile if exist
        if ( tile[ current_tile ].hibernate_cb != NULL ) {
            log_d("call hibernate cb for tile: %d", current_tile );
            tile[ current_tile ].hibernate_cb();
        }
        // call activate callback for the new tile if exist
        if ( tile[ tile_number ].activate_cb != NULL ) { 
            log_d("call activate cb for tile: %d", tile_number );
            tile[ tile_number ].activate_cb();
        }
        current_tile = tile_number;
    }
    else {
        log_e( "tile number %d do not exist", tile_number );
    }
}

lv_obj_t * mainbar_obj_create(lv_obj_t *parent) {
    /*
     * check if mainbar already initialized
     */
    if ( !mainbar ) {
        log_e("main not initialized");
        while( true );
    }

    lv_obj_t * child = lv_obj_create( parent, NULL );
    lv_tileview_add_element( mainbar, child );

    return child;
}

void mainbar_add_slide_element(lv_obj_t *element) {
    /*
     * check if mainbar already initialized
     */
    if ( !mainbar ) {
        log_e("main not initialized");
        while( true );
    }

    lv_tileview_add_element( mainbar, element );
}