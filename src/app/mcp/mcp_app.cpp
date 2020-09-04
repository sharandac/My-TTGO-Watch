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
#include <TTGO.h>

#include "mcp_app.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

// app and widget icon
icon_t *mcp_app = NULL;
icon_t *mcp_widget = NULL;

uint32_t mcp_app_main_tile_num;

// declare you images or fonts you need
LV_IMG_DECLARE(mcp_64px);

// declare callback functions
static void enter_mcp_app_event_cb( lv_obj_t * obj, lv_event_t event );

// setup routine for example app
void mcp_app_setup( void ) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    mcp_app_main_tile_num = mainbar_add_app_tile( 1, 1 );

    mcp_app = app_register( "MCP", &mcp_64px, enter_mcp_app_event_cb );

#ifdef MCP_WIDGET
    mcp_widget = widget_register( "MCP", &mcp_64px, enter_mcp_app_event_cb );
    widget_set_indicator( mcp_widget, ICON_INDICATOR_UPDATE );
#endif // MCP_WIDGET

}

uint32_t mcp_app_get_app_main_tile_num( void ) {
    return( mcp_app_main_tile_num );
}

static void enter_mcp_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
/*
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( mcp_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
*/
}