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
#include "setup.h"

#include "gui/mainbar/mainbar.h"
#include "hardware/motor.h"

lv_setup_entry_t setup_entry[ MAX_SETUP_ICON ];

lv_obj_t *setup_cont = NULL;
lv_style_t setup_style;
uint32_t setup_tile_num;

void setup_tile_setup( void ) {
    setup_tile_num = mainbar_add_tile( 1, 1 );
    setup_cont = mainbar_get_tile_obj( setup_tile_num );
    lv_style_copy( &setup_style, mainbar_get_style() );

    for ( int setup = 0 ; setup < MAX_SETUP_ICON ; setup++ ) {
        // set x, y and mark it as inactive
        setup_entry[ setup ].x = SETUP_FIRST_X_POS + ( ( setup % MAX_SETUP_ICON_HORZ ) * ( SETUP_ICON_X_SIZE + SETUP_ICON_X_CLEARENCE ) );
        setup_entry[ setup ].y = SETUP_FIRST_Y_POS + ( ( setup / MAX_SETUP_ICON_HORZ ) * ( SETUP_ICON_Y_SIZE + SETUP_ICON_Y_CLEARENCE ) );
        setup_entry[ setup ].active = false;
        // create app icon container
        setup_entry[ setup ].setup = mainbar_obj_create( setup_cont );
        lv_obj_reset_style_list( setup_entry[ setup ].setup, LV_OBJ_PART_MAIN );
        lv_obj_add_style( setup_entry[ setup ].setup, LV_OBJ_PART_MAIN, &setup_style );
        lv_obj_set_size( setup_entry[ setup ].setup, SETUP_ICON_X_SIZE, SETUP_ICON_Y_SIZE );
        lv_obj_align( setup_entry[ setup ].setup , setup_cont, LV_ALIGN_IN_TOP_LEFT, setup_entry[ setup ].x, setup_entry[ setup ].y );

        lv_obj_set_hidden( setup_entry[ setup ].setup, true );

        log_d("icon x/y: %d/%d", setup_entry[ setup ].x, setup_entry[ setup ].y );
    }
}

lv_obj_t *setup_tile_register_setup( void ) {
    for( int setup = 0 ; setup < MAX_SETUP_ICON ; setup++ ) {
        if ( setup_entry[ setup ].active == false ) {
            setup_entry[ setup ].active = true;
            lv_obj_set_hidden( setup_entry[ setup ].setup, false );
            return( setup_entry[ setup ].setup );
        }
    }
    log_e("no space for an setup icon");
    return( NULL );
}

uint32_t setup_get_tile_num( void ) {
    return( setup_tile_num );
}
