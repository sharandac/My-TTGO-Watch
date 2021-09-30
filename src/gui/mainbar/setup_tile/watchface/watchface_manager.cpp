/****************************************************************************
 *   Sep 3 23:05:42 2020
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

#include "watchface_manager_app.h"
#include "watchface_manager.h"
#include "watchface_setup.h"
#include "watchface_tile.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/app.h"
#include "gui/widget.h"
/**
 * app and widget icon
 */
icon_t *watchface_manager_app = NULL;
icon_t *watchface_setup = NULL;
/**
 * watchface manager main tile number
 */
uint32_t watchface_manager_app_main_tile_num;
uint32_t watchface_setup_tile_num;
/**
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(watchface_manager_64px);
LV_IMG_DECLARE(watchface_64px);
/**
 * internal function declaration
 */
static void enter_watchface_manager_app_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_watchface_setup_event_cb( lv_obj_t * obj, lv_event_t event );
/**
 * setup routine for watchface manager app
 */
void watchface_manager_setup( void ) {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif
    /**
     * register 1 app tile
     */
    watchface_manager_app_main_tile_num = mainbar_add_app_tile( 1, 1, "Watchface manager" );
    watchface_manager_app = app_register( "watchface\nmanager", &watchface_manager_64px, enter_watchface_manager_app_event_cb );
    /*
     * register 1 tiles for setup
     */
    watchface_setup_tile_num = mainbar_add_setup_tile( 1, 1, "WatchFace Setup" );
    watchface_setup = setup_register( "watchface", &watchface_64px, enter_watchface_setup_event_cb );
    /*
     * init main tile, see example_app_main.cpp and example_app_setup.cpp
     */
    watchface_manager_app_setup( watchface_manager_app_main_tile_num );
    watchface_tile_setup();
    watchface_setup_tile_setup( watchface_setup_tile_num );
}

uint32_t watchface_manager_get_app_tile_num( void ) {
    return( watchface_manager_app_main_tile_num );
}

uint32_t watchface_manager_get_setup_tile_num( void ) {
    return( watchface_setup_tile_num );
}

static void enter_watchface_manager_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( watchface_manager_app_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        break;
    }    
}

static void enter_watchface_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( watchface_setup_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( false );
                                        break;
    }    
}