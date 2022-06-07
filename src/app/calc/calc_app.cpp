/****************************************************************************
 *   June 04 02:01:00 2021
 *   Copyright  2021  Dirk Sarodnick
 *   Email: programmer@dirk-sarodnick.de
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

#include "calc_app.h"
#include "calc_app_main.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t calc_app_main_tile_num;

// app icon
icon_t *calc_app = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(calc_app_64px);

/*
 * setup routine for example app
 */
void calc_app_setup( void ) {
    calc_app_main_tile_num = mainbar_add_app_tile( 1, 1, "calc app" );
    calc_app = app_register( "Calculator", &calc_app_64px, enter_calc_app_event_cb );
    calc_app_main_setup( calc_app_main_tile_num );
}

/*
 *
 */
uint32_t calc_app_get_app_main_tile_num( void ) {
    return( calc_app_main_tile_num );
}

/*
 *
 */
void enter_calc_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( calc_app_main_tile_num, LV_ANIM_OFF, true );
                                        app_hide_indicator( calc_app );
                                        break;
    }
}

/*
 *
 */
void exit_calc_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}