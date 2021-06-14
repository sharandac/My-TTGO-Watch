/****************************************************************************
 *   June 14 02:01:00 2021
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
#include <TTGO.h>

#include "kodi_remote_app.h"
#include "kodi_remote_app_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

lv_obj_t *kodi_remote_app_setup_tile = NULL;
lv_style_t kodi_remote_app_setup_style;

static void exit_kodi_remote_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );

void kodi_remote_app_setup_setup( uint32_t tile_num ) {
    /**
     * get tile obj from tile number
     */
    kodi_remote_app_setup_tile = mainbar_get_tile_obj( tile_num );
    /**
     * get mainbar style, setup it and add it to setup tile
     */
    lv_style_copy( &kodi_remote_app_setup_style, ws_get_setup_tile_style() );
    lv_obj_add_style( kodi_remote_app_setup_tile, LV_OBJ_PART_MAIN, &kodi_remote_app_setup_style );
    /**
     * add setup header with exit button
     */
    lv_obj_t *header = wf_add_settings_header( kodi_remote_app_setup_tile, "kodi remote setup", exit_kodi_remote_app_setup_event_cb );
    lv_obj_align( header, kodi_remote_app_setup_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );
}

static void exit_kodi_remote_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( kodi_remote_app_get_app_main_tile_num(), LV_ANIM_ON );
                                        kodi_remote_save_config();
                                        break;
    }
}
