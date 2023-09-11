/****************************************************************************
 *   Copyright  2020  Chris McNamee
 *   Copyright  2022  Pavel Machek
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

#include "astro_app.h"
#include "astro_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
#endif
/*
 * app tiles
 */
uint32_t astro_app_main_tile_num;
/*
 * app and widget icon
 */
icon_t *astro_app = NULL;
icon_t *astro_widget = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(astro_app_64px);
/*
 * declare callback functions
 */
static void enter_astro_app_event_cb( lv_obj_t * obj, lv_event_t event );
/*
 * automatic register the app setup function with explicit call in main.cpp
 */
static int registed = app_autocall_function( &astro_app_setup, 8 );           /** @brief app autocall function */
/*
 * setup routine for astro app
 */
void astro_app_setup( void ) {
    /*
     * check if app already registered for autocall
     */
    if( !registed ) {
        return;
    }

    astro_app_main_tile_num = mainbar_add_app_tile( 1, 1, "Astro App" );
    astro_app = app_register( "astro", &astro_app_64px, enter_astro_app_event_cb );
    astro_app_main_setup( astro_app_main_tile_num );
}
/**
 * @brief get the app main tile number
 * 
 * @return uint32_t 
 */
uint32_t astro_app_get_app_main_tile_num( void ) {
    return( astro_app_main_tile_num );
}
/**
 * @brief callback function to enter the app
 * 
 * @param obj           pointer to the object
 * @param event         the event
 */
static void enter_astro_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       astro_app_hide_app_icon_info( true );
                                        mainbar_jump_to_tilenumber( astro_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}
/**
 * @brief add the widget icon to the widget bar
 */
void astro_add_widget( void ) {
    astro_widget = widget_register( "astro", &astro_app_64px, enter_astro_app_event_cb );
}
/**
 * @brief remove the widget icon from the widget bar
 */
void astro_remove_widget( void ) {
    astro_widget = widget_remove( astro_widget );
}
/**
 * @brief update the widget label
 * 
 * @param label         the new label
 */
void astro_app_update_widget_label( char *label ) {
    widget_set_label( astro_widget, label );
}
/**
 * @brief hide the app icon info
 * 
 * @param show      true to show, false to hide
 */
void astro_app_hide_app_icon_info( bool show ) {
    if ( !show ) {
        app_set_indicator( astro_app, ICON_INDICATOR_1 );
    }
    else {
        app_hide_indicator( astro_app );
    }
}
/**
 * @brief hide the widget icon info
 * 
 * @param show      true to show, false to hide
 */
void astro_app_hide_widget_icon_info( bool show ) {
    if ( !show ) {
        widget_set_indicator( astro_widget, ICON_INDICATOR_1 );
    }
    else {
        widget_hide_indicator( astro_widget );
    }
}
