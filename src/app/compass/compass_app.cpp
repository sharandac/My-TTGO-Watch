/****************************************************************************
 *   Jul 05 17:13:51 2022
 *   Copyright  2022  Dirk Brosswick
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

#include "compass_app.h"
#include "compass_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget_styles.h"
#include "gui/widget_factory.h"

#include "hardware/compass.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
#endif

lv_obj_t *compass_app_main_tile = NULL;
uint32_t compass_app_main_tile_num;
/*
 * app icon
 */
icon_t *compass_app = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(compass_64px);
/*
 * declare callback functions for the app 
 */
static void compass_enter_app_event_cb( lv_obj_t * obj, lv_event_t event );
/*
 * setup routine for wifimon app
 */
void compass_app_setup( void ) {
    /**
     * abort if no compass available
     */
    if( !compass_available() )
        return;
    /**
     * setup compass app
     */
    compass_app_main_tile_num = mainbar_add_app_tile( 1, 1, "compass" );
    compass_app_main_tile = mainbar_get_tile_obj( compass_app_main_tile_num );
    compass_app = app_register( "compass", &compass_64px, compass_enter_app_event_cb );
    compass_app_main_setup( compass_app_main_tile_num );
}

uint32_t compass_app_get_app_main_tile_num( void ) {
    return( compass_app_main_tile_num );
}

static void compass_enter_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       app_hide_indicator( compass_app );
                                        mainbar_jump_to_tilenumber( compass_app_main_tile_num, LV_ANIM_OFF, true );
                                        break;
    }    
}
