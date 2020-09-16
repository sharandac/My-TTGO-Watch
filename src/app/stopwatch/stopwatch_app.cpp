/****************************************************************************
 *   Aug 21 17:26:00 2020
 *   Copyright  2020  Chris McNamee
 *   Email: chris.mcna@gmail.com
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

#include "stopwatch_app.h"
#include "stopwatch_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t stopwatch_app_main_tile_num;

// app and widget icon
icon_t *stopwatch_app = NULL;
icon_t *stopwatch_widget = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(stopwatch_app_64px);

// declare callback functions
static void enter_stopwatch_app_event_cb( lv_obj_t * obj, lv_event_t event );

// setup routine for stopwatch app
void stopwatch_app_setup( void ) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    stopwatch_app_main_tile_num = mainbar_add_app_tile( 1, 1, "Stopwatch App" );

    stopwatch_app = app_register( "stop\nwatch", &stopwatch_app_64px, enter_stopwatch_app_event_cb );

    // init main and setup tile, see stopwatch_app_main.cpp and stopwatch_app_setup.cpp
    stopwatch_app_main_setup( stopwatch_app_main_tile_num );
}

uint32_t stopwatch_app_get_app_main_tile_num( void ) {
    return( stopwatch_app_main_tile_num );
}

/*
 *
 */
static void enter_stopwatch_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        stopwatch_app_hide_app_icon_info( true );
                                        mainbar_jump_to_tilenumber( stopwatch_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}


void stopwatch_add_widget( void ) {
    stopwatch_widget = widget_register( "stopwatch", &stopwatch_app_64px, enter_stopwatch_app_event_cb );
}

void stopwatch_remove_widget( void ) {
    stopwatch_widget = widget_remove( stopwatch_widget );
}

void stopwatch_app_update_widget_label( char *label ) {
    widget_set_label( stopwatch_widget, label );
}
/*
 *
 */
void stopwatch_app_hide_app_icon_info( bool show ) {
    if ( !show ) {
        app_set_indicator( stopwatch_app, ICON_INDICATOR_1 );
    }
    else {
        app_hide_indicator( stopwatch_app );
    }
}

/*
 *
 */
void stopwatch_app_hide_widget_icon_info( bool show ) {
    if ( !show ) {
        widget_set_indicator( stopwatch_widget, ICON_INDICATOR_1 );
    }
    else {
        widget_hide_indicator( stopwatch_widget );
    }
}