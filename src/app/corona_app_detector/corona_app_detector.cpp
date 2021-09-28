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

#include "corona_app_detector.h"
#include "corona_app_detector_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t corona_app_detector_main_tile_num;
uint32_t corona_app_detector_setup_tile_num;

/*
 * app icon
 */
icon_t *corona_app_detector = NULL;
/*
 * widget icon
 */
icon_t *corona_app_detector_widget = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(corona_app_detector_64px);
LV_IMG_DECLARE(info_1_16px);
/*
 * declare callback functions for the app and widget icon to enter the app
 */
static void enter_corona_app_detector_event_cb( lv_obj_t * obj, lv_event_t event );

void corona_app_detector_setup( void ) {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif
    /**
     * register app main tile
     */
    corona_app_detector_main_tile_num = mainbar_add_app_tile( 1, 1, "corona app detector" );
    corona_app_detector = app_register( "corona\ndetector", &corona_app_detector_64px, enter_corona_app_detector_event_cb );
    /**
     * setup app main tile
     */
    corona_app_detector_main_setup( corona_app_detector_main_tile_num );
}

uint32_t corona_app_detector_get_main_tile_num( void ) {
    return( corona_app_detector_main_tile_num );
}

uint32_t corona_app_detector_get_setup_tile_num( void ) {
    return( corona_app_detector_setup_tile_num );
}

static void enter_corona_app_detector_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( corona_app_detector_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        app_hide_indicator( corona_app_detector );
                                        break;
    }    
}
