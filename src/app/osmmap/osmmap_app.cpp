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

#include "osmmap_app.h"
#include "osmmap_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"
/*
 * app tiles
 */
uint32_t osmmap_app_main_tile_num;
uint32_t osmmap_app_setup_tile_num;
/*
 * app icon
 */
icon_t *osmmap_app = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(osm_64px);
/*
 * declare callback functions
 */
static void enter_osmmap_app_event_cb( lv_obj_t * obj, lv_event_t event );
/*
 * automatic register the app setup function with explicit call in main.cpp
 */
static int registed = app_autocall_function( &osmmap_app_setup, 16 );           /** @brief app autocall function */
/*
 * setup routine for example app
 */
void osmmap_app_setup( void ) {
    /*
     * check if app already registered for autocall
     */
    if( !registed ) {
        return;
    }
    /*
     * register 2 vertical tiles and get the first tile number and save it for later use
     */
    osmmap_app_main_tile_num = mainbar_add_app_tile( 1, 1, "OsmMap App" );
    /*
     * register an app icon and the app enter function
     */
    osmmap_app = app_register( "OsmMap", &osm_64px, enter_osmmap_app_event_cb );
    /*
     * init main tile, see example_app_main.cpp and example_app_setup.cpp
     */
    osmmap_app_main_setup( osmmap_app_main_tile_num );
}
/**
 * @brief Get the app main tile num object
 * 
 * @return uint32_t 
 */
uint32_t osmmap_app_get_app_main_tile_num( void ) {
    return( osmmap_app_main_tile_num );
}
/**
 * @brief callback function to enter the app
 * 
 * @param obj 
 * @param event 
 */
static void enter_osmmap_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( osmmap_app_main_tile_num, LV_ANIM_OFF, true );
                                        break;
    }    
}
