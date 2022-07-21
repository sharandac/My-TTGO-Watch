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
#include "config.h"
#include "setup_tile.h"

#include "gui/mainbar/mainbar.h"
#include "gui/icon.h"
#include "gui/widget_factory.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/note_tile/note_tile.h"
#include "gui/mainbar/setup_tile/setup_tile.h"

#include "utils/alloc.h"

#include "hardware/motor.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
#endif

static bool setuptile_init = false;

icon_t *setup_entry = NULL;

lv_obj_t *setup_cont[ MAX_SETUP_TILES ];
uint32_t setup_tile_num[ MAX_SETUP_TILES ];
lv_style_t setup_style;
lv_style_t setup_icon_style;

static bool setup_tile_button_event_cb( EventBits_t event, void *arg );

void setup_tile_setup( void ) {
    /*
     * check if setuptile alread initialized
     */
    if ( setuptile_init ) {
        log_e("setuptile already initialized");
        return;
    }

    setup_entry = (icon_t*)MALLOC_ASSERT( sizeof( icon_t ) * MAX_SETUP_ICON, "error while setup_entry alloc" );

    for ( int tiles = 0 ; tiles < MAX_SETUP_TILES ; tiles++ ) {
    #if defined( M5PAPER )
        setup_tile_num[ tiles ] = mainbar_add_tile( 0, 2 + tiles, "setup tile", ws_get_mainbar_style() );
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 ) || defined( M5CORE2 )
        setup_tile_num[ tiles ] = mainbar_add_tile( 1 + tiles , 1, "setup tile", ws_get_mainbar_style() );
    #elif defined( LILYGO_WATCH_2021 )
        setup_tile_num[ tiles ] = mainbar_add_tile( 1 + tiles , 1, "setup tile", ws_get_mainbar_style() );
    #elif defined( WT32_SC01 )
        setup_tile_num[ tiles ] = mainbar_add_tile( 1 + tiles , 1, "setup tile", ws_get_mainbar_style() );
    #else
        setup_tile_num[ tiles ] = mainbar_add_tile( 1 + tiles , 1, "setup tile", ws_get_mainbar_style() );
        #warning "no setup tiles set"
    #endif
        setup_cont[ tiles ] = mainbar_get_tile_obj( setup_tile_num[ tiles ] );
        mainbar_add_tile_button_cb( setup_tile_num[ tiles ], setup_tile_button_event_cb );
    }

    lv_style_copy( &setup_style, ws_get_mainbar_style() );

    for ( int setup = 0 ; setup < MAX_SETUP_ICON ; setup++ ) {
        // set x, y and mark it as inactive
        setup_entry[ setup ].x = SETUP_FIRST_X_POS + ( ( setup % MAX_SETUP_ICON_HORZ ) * ( SETUP_ICON_X_SIZE + SETUP_ICON_X_CLEARENCE ) ) + SETUP_ICON_X_OFFSET;
        setup_entry[ setup ].y = SETUP_FIRST_Y_POS + ( ( ( setup % ( MAX_SETUP_ICON_VERT * MAX_SETUP_ICON_HORZ ) ) / MAX_SETUP_ICON_HORZ ) * ( SETUP_ICON_Y_SIZE + SETUP_ICON_Y_CLEARENCE ) ) + SETUP_ICON_Y_OFFSET;
        setup_entry[ setup ].active = false;
        // create app icon container
        setup_entry[ setup ].icon_cont = mainbar_obj_create( setup_cont[ setup / ( MAX_SETUP_ICON_HORZ * MAX_SETUP_ICON_VERT ) ] );
        lv_obj_reset_style_list( setup_entry[ setup ].icon_cont, LV_OBJ_PART_MAIN );
        lv_obj_set_size( setup_entry[ setup ].icon_cont, SETUP_ICON_X_SIZE, SETUP_ICON_Y_SIZE );
        lv_obj_align( setup_entry[ setup ].icon_cont , setup_cont[ setup / ( MAX_SETUP_ICON_HORZ * MAX_SETUP_ICON_VERT ) ], LV_ALIGN_IN_TOP_LEFT, setup_entry[ setup ].x, setup_entry[ setup ].y );
        // create app label
        setup_entry[ setup ].label = lv_label_create( setup_cont[ setup / ( MAX_SETUP_ICON_HORZ * MAX_SETUP_ICON_VERT ) ], NULL );
        mainbar_add_slide_element(setup_entry[ setup ].label);
        lv_obj_reset_style_list( setup_entry[ setup ].label, LV_OBJ_PART_MAIN );
        lv_obj_set_size( setup_entry[ setup ].label, SETUP_LABEL_X_SIZE, SETUP_LABEL_Y_SIZE );
        lv_obj_align( setup_entry[ setup ].label , setup_entry[ setup ].icon_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );

        lv_obj_set_hidden( setup_entry[ setup ].icon_cont, true );
        lv_obj_set_hidden( setup_entry[ setup ].label, true );

        log_d("icon screen/x/y: %d/%d/%d", setup / ( MAX_SETUP_ICON_HORZ * MAX_SETUP_ICON_VERT ), setup_entry[ setup ].x, setup_entry[ setup ].y );
    }

    setuptile_init = true;
    log_i("setup tile finish");
}

static bool setup_tile_button_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_LEFT:   mainbar_jump_to_tilenumber( app_tile_get_tile_num(), LV_ANIM_ON );
                            mainbar_clear_history();
                            break;
        case BUTTON_RIGHT:  mainbar_jump_to_tilenumber( note_tile_get_tile_num(), LV_ANIM_ON );
                            mainbar_clear_history();
                            break;
        case BUTTON_EXIT:   mainbar_jump_to_maintile( LV_ANIM_ON );
                            break;
    }
    return( true );
}

lv_obj_t *setup_tile_register_setup( void ) {
    /*
     * check if setuptile alread initialized
     */
    if ( !setuptile_init ) {
        log_e("setuptile not initialized");
        while( true );
    }

    for( int setup = 0 ; setup < MAX_SETUP_ICON ; setup++ ) {
        if ( setup_entry[ setup ].active == false ) {
            setup_entry[ setup ].active = true;
            lv_obj_set_hidden( setup_entry[ setup ].icon_cont, false );
            return( setup_entry[ setup ].icon_cont );
        }
    }
    log_e("no space for an setup icon");
    return( NULL );
}

icon_t *setup_tile_get_free_setup_icon( void ) {
    /*
     * check if setuptile alread initialized
     */
    if ( !setuptile_init ) {
        log_e("setuptile not initialized");
        while( true );
    }

    for( int setup = 0 ; setup < MAX_SETUP_ICON ; setup++ ) {
        if ( setup_entry[ setup ].active == false ) {
            return( &setup_entry[ setup ] );
        }
    }
    log_e("no space for an setup icon");
    return( NULL );
}

uint32_t setup_get_tile_num( void ) {
    /*
     * check if setuptile alread initialized
     */
    if ( !setuptile_init ) {
        log_e("setuptile not initialized");
        while( true );
    }

    return( setup_tile_num[ 0 ] );
}
