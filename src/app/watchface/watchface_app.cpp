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
#include <TTGO.h>

#include "watchface_app.h"
#include "watchface_app_main.h"
#include "watchface_app_tile.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t watchface_app_main_tile_num;
/*
 * app icon
 */
icon_t *watchface_app = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(watchface_64px);
/*
 * declare callback functions
 */
static void enter_watchface_app_event_cb( lv_obj_t * obj, lv_event_t event );
/*
 * setup routine for example app
 */
void watchface_app_setup( void ) {
    /*
     * register 2 vertical tiles and get the first tile number and save it for later use
     */
    watchface_app_main_tile_num = mainbar_add_app_tile( 1, 1, "WatchFace App" );
    /*
     * register an app icon and the app enter function
     */
    watchface_app = app_register( "WatchFace", &watchface_64px, enter_watchface_app_event_cb );
    /*
     * init main tile, see example_app_main.cpp and example_app_setup.cpp
     */
    watchface_app_tile_setup();
    watchface_app_main_setup( watchface_app_main_tile_num );
}

uint32_t watchface_app_get_app_main_tile_num( void ) {
    return( watchface_app_main_tile_num );
}

static void enter_watchface_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( false );
                                        mainbar_jump_to_tilenumber( watchface_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}