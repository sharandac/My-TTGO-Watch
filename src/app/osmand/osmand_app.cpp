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

#include "osmand_app.h"
#include "osmand_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t osmand_app_main_tile_num;
uint32_t osmand_app_setup_tile_num;

// app icon
icon_t *osmand_app = NULL;
icon_t *osmand_widget = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(osmand_64px);

// declare callback functions
static void enter_osmand_app_event_cb( lv_obj_t * obj, lv_event_t event );

// setup routine for example app
void osmand_app_setup( void ) {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif
    // register 2 vertical tiles and get the first tile number and save it for later use
    osmand_app_main_tile_num = mainbar_add_app_tile( 1, 1, "OsmAnd App" );

    osmand_app = app_register( "OsmAnd", &osmand_64px, enter_osmand_app_event_cb );

//    osmand_widget = widget_register( "OsmAnd", &osmand_64px, enter_osmand_app_event_cb );

    // init main tile, see example_app_main.cpp and example_app_setup.cpp
    osmand_app_main_setup( osmand_app_main_tile_num );
}

uint32_t osmand_app_get_app_main_tile_num( void ) {
    return( osmand_app_main_tile_num );
}

static void enter_osmand_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( osmand_app_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        break;
    }    
}
