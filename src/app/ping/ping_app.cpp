/****************************************************************************
 *   linuxthor 2020 
 *   ttgo watch ping utility
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

#include "ping_app.h"
#include "ping_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"

uint32_t ping_app_main_tile_num;

// app icon
icon_t *ping_app = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(ping_app_64px);
LV_IMG_DECLARE(info_1_16px);

static void enter_ping_app_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for ping app
 */
void ping_app_setup( void ) {
    ping_app_main_tile_num = mainbar_add_app_tile( 1, 1, "ping app" );

    ping_app = app_register( "ping", &ping_app_64px, enter_ping_app_event_cb );

    // init main tile, see ping_app_main.cpp 
    ping_app_main_setup( ping_app_main_tile_num );
}

/*
 *
 */
uint32_t ping_app_get_app_main_tile_num( void ) {
    return( ping_app_main_tile_num );
}

/*
 *
 */
static void enter_ping_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        app_hide_indicator( ping_app );
                                        mainbar_jump_to_tilenumber( ping_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

