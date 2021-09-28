/****************************************************************************
 *  FindPhone.cpp
 *  Neuroplant  11/2020
 *  Email: m.roych@gmail.com
 *
 *
 *  Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  Example_App"
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

#include "FindPhone.h"
#include "FindPhone_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t FindPhone_main_tile_num;
uint32_t FindPhone_setup_tile_num;

// app icon
icon_t *FindPhone = NULL;

// widget icon container
icon_t *FindPhone_widget = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(eye_64px);
LV_IMG_DECLARE(info_1_16px);

// declare callback functions for the app and widget icon to enter the app
static void enter_FindPhone_event_cb( lv_obj_t * obj, lv_event_t event );
//static void enter_FindPhone_widget_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for example app
 */
void FindPhone_setup( void ) {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif
    // register 1 vertical tile and get the first tile number and save it for later use
    FindPhone_main_tile_num = mainbar_add_app_tile( 1, 1, "FindPhone" );
    FindPhone = app_register( "Find\nPhone", &eye_64px, enter_FindPhone_event_cb );
    // init main and setup tile, see FindPhone_main.cpp and FindPhone_setup.cpp
    FindPhone_main_setup( FindPhone_main_tile_num );	
	bluetooth_FindPhone_tile_setup();
}

/*
 *
 */
uint32_t FindPhone_get_app_main_tile_num( void ) {
    return( FindPhone_main_tile_num );
}

/*
 *
 */
uint32_t FindPhone_get_app_setup_tile_num( void ) {
    return( FindPhone_setup_tile_num );
}

/*
 *
 */
static void enter_FindPhone_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       app_hide_indicator( FindPhone );
                                        mainbar_jump_to_tilenumber( FindPhone_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        break;
    }    
}

/*
 *
 */

