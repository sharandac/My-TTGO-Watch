/****************************************************************************
 *   Tu May 22 21:23:51 2020
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
#include "touch_settings.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/touch.h"

lv_obj_t *touch_settings_tile=NULL;
uint32_t touch_tile_num;

LV_IMG_DECLARE(touch_64px);

static void enter_touch_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_touch_setup_event_cb( lv_obj_t * obj, lv_event_t event );

void touch_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    touch_tile_num = mainbar_add_setup_tile( 1, 1, "touch settings" );
    touch_settings_tile = mainbar_get_tile_obj( touch_tile_num );

    lv_obj_add_style( touch_settings_tile, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );

    icon_t *touch_setup_icon = setup_register( "touch", &touch_64px, enter_touch_setup_event_cb );
    setup_hide_indicator( touch_setup_icon );

    lv_obj_t *header = wf_add_settings_header( touch_settings_tile, "touch settings", exit_touch_setup_event_cb );
    lv_obj_align( header, touch_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
}


static void enter_touch_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( touch_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_touch_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}
