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
#include "hardware/motor.h"

icon_t setup_entry[ MAX_SETUP_ICON ];

lv_obj_t *setup_cont[ MAX_SETUP_TILES ];
uint32_t setup_tile_num[ MAX_SETUP_TILES ];
lv_style_t setup_style;

void setup_tile_setup( void ) {

    for ( int tiles = 0 ; tiles < MAX_SETUP_TILES ; tiles++ ) {
        setup_tile_num[ tiles ] = mainbar_add_tile( 1 + tiles , 1, "setup tile" );
        setup_cont[ tiles ] = mainbar_get_tile_obj( setup_tile_num[ tiles ] );
    }

    lv_style_copy( &setup_style, mainbar_get_style() );

    for ( int setup = 0 ; setup < MAX_SETUP_ICON ; setup++ ) {
        // set x, y and mark it as inactive
        setup_entry[ setup ].x = SETUP_FIRST_X_POS + ( ( setup % MAX_SETUP_ICON_HORZ ) * ( SETUP_ICON_X_SIZE + SETUP_ICON_X_CLEARENCE ) );
        setup_entry[ setup ].y = SETUP_FIRST_Y_POS + ( ( ( setup % ( MAX_SETUP_ICON_VERT * MAX_SETUP_ICON_HORZ ) ) / MAX_SETUP_ICON_HORZ ) * ( SETUP_ICON_Y_SIZE + SETUP_ICON_Y_CLEARENCE ) );
        setup_entry[ setup ].active = false;
        // create app icon container
        setup_entry[ setup ].icon_cont = mainbar_obj_create( setup_cont[ setup / ( MAX_SETUP_ICON_HORZ * MAX_SETUP_ICON_VERT ) ] );
        lv_obj_reset_style_list( setup_entry[ setup ].icon_cont, LV_OBJ_PART_MAIN );
        lv_obj_add_style( setup_entry[ setup ].icon_cont, LV_OBJ_PART_MAIN, &setup_style );
        lv_obj_set_size( setup_entry[ setup ].icon_cont, SETUP_ICON_X_SIZE, SETUP_ICON_Y_SIZE );
        lv_obj_align( setup_entry[ setup ].icon_cont , setup_cont[ setup / ( MAX_SETUP_ICON_HORZ * MAX_SETUP_ICON_VERT ) ], LV_ALIGN_IN_TOP_LEFT, setup_entry[ setup ].x, setup_entry[ setup ].y );
        // create app label
        setup_entry[ setup ].label = lv_label_create( setup_cont[ setup / ( MAX_SETUP_ICON_HORZ * MAX_SETUP_ICON_VERT ) ], NULL );
        mainbar_add_slide_element(setup_entry[ setup ].label);
        lv_obj_reset_style_list( setup_entry[ setup ].label, LV_OBJ_PART_MAIN );
        lv_obj_add_style( setup_entry[ setup ].label, LV_OBJ_PART_MAIN, &setup_style );
        lv_obj_set_size( setup_entry[ setup ].label, SETUP_LABEL_X_SIZE, SETUP_LABEL_Y_SIZE );
        lv_obj_align( setup_entry[ setup ].label , setup_entry[ setup ].icon_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );

        lv_obj_set_hidden( setup_entry[ setup ].icon_cont, true );
        lv_obj_set_hidden( setup_entry[ setup ].label, true );

        log_d("icon screen/x/y: %d/%d/%d", setup / ( MAX_SETUP_ICON_HORZ * MAX_SETUP_ICON_VERT ), setup_entry[ setup ].x, setup_entry[ setup ].y );
    }
}

lv_obj_t *setup_tile_register_setup( void ) {
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
    for( int setup = 0 ; setup < MAX_SETUP_ICON ; setup++ ) {
        if ( setup_entry[ setup ].active == false ) {
            return( &setup_entry[ setup ] );
        }
    }
    log_e("no space for an setup icon");
    return( NULL );
}

uint32_t setup_get_tile_num( void ) {
    return( setup_tile_num[ 0 ] );
}
