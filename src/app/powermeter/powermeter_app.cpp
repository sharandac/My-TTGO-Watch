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
#include "powermeter_app.h"
#include "powermeter_main.h"
#include "powermeter_setup.h"
#include "config/powermeter_config.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

#include "utils/json_psram_allocator.h"

powermeter_config_t powermeter_config;

// app and widget icon
icon_t *powermeter_app = NULL;
icon_t *powermeter_widget = NULL;

uint32_t powermeter_app_main_tile_num;
uint32_t powermeter_app_setup_tile_num;

// declare you images or fonts you need
LV_IMG_DECLARE(powermeter_64px);

// declare callback functions
static void enter_powermeter_app_event_cb( lv_obj_t * obj, lv_event_t event );

// setup routine for example app
void powermeter_app_setup( void ) {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif
    powermeter_config.load();

    // register 2 vertical tiles and get the first tile number and save it for later use
    powermeter_app_main_tile_num = mainbar_add_app_tile( 1, 1, "Powermeter App" );
    powermeter_app_setup_tile_num = mainbar_add_setup_tile( 1, 2, "Powermeter App" );

    powermeter_app = app_register( "power-\nmeter", &powermeter_64px, enter_powermeter_app_event_cb );

    if ( powermeter_config.widget ) {
        powermeter_add_widget();
    }

    powermeter_main_tile_setup( powermeter_app_main_tile_num );
    powermeter_setup_tile_setup( powermeter_app_setup_tile_num );
}

uint32_t powermeter_get_app_main_tile_num( void ) {
    return( powermeter_app_main_tile_num );
}

uint32_t powermeter_get_app_setup_tile_num( void ) {
    return( powermeter_app_setup_tile_num );
}

icon_t *powermeter_get_app_icon( void ) {
    return( powermeter_app );
}

icon_t *powermeter_get_widget_icon( void ) {
    return( powermeter_widget );
}

static void enter_powermeter_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( powermeter_app_main_tile_num, LV_ANIM_OFF );
                                        statusbar_hide( true );
                                        break;
    }    
}


powermeter_config_t *powermeter_get_config( void ) {
    return( &powermeter_config );
}

bool powermeter_add_widget( void ) {
    if ( powermeter_widget == NULL ) {
        powermeter_widget = widget_register( "n/a", &powermeter_64px, enter_powermeter_app_event_cb );
        widget_hide_indicator( powermeter_widget );
        if ( powermeter_widget != NULL ) {
            return( true );
        }
        else {
            return( false );
        }
    }
    return( true );
}

bool powermeter_remove_widget( void ) {
    powermeter_widget = widget_remove( powermeter_widget );
    return( true );
}
