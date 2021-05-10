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
#include <TTGO.h>

#include "gui/mainbar/setup_tile/watchface/watchface_manager.h"
#include "gui/mainbar/setup_tile/watchface/watchface_setup.h"
#include "gui/mainbar/setup_tile/watchface/watchface_tile.h"
#include "watchface_manager_app.h"

#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_styles.h"
#include "gui/widget_factory.h"

/**
 * watchface manager app tile container
 */
lv_obj_t *watchface_manager_app_tile = NULL;
/**
 * watchface manager app container
 */
lv_obj_t *watchface_manager_app_cont = NULL;
lv_obj_t *watchface_manager_app_preview_cont = NULL;
lv_obj_t *watchface_manager_app_info_cont = NULL;
lv_obj_t *watchface_manager_app_info_label = NULL;
lv_obj_t *watchface_manager_app_info_progressbar = NULL;
lv_style_t watchface_manager_style;

/**
 * font we need
 */
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_48px);
/**
 * images we need
 */
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);

static void exit_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event );
static void setup_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event );

void watchface_manager_app_setup( uint32_t tile_num ) {
    /**
     * geht app tile
     */
    watchface_manager_app_tile = mainbar_get_tile_obj( tile_num );
    /**
     * get and set main style
     */
    lv_style_copy( &watchface_manager_style, ws_get_app_opa_style() );
    lv_style_set_text_font( &watchface_manager_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_style_set_bg_color( &watchface_manager_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_obj_add_style( watchface_manager_app_tile, LV_OBJ_PART_MAIN, &watchface_manager_style );
    /**
     * create global watchface manager container
     */
    lv_obj_t *watchface_manager_app_cont = lv_obj_create( watchface_manager_app_tile, NULL );
    lv_obj_set_size( watchface_manager_app_cont, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) );
    lv_obj_add_style( watchface_manager_app_cont, LV_OBJ_PART_MAIN, &watchface_manager_style );
    lv_obj_align( watchface_manager_app_cont, watchface_manager_app_tile, LV_ALIGN_CENTER, 0, 0 );
    /**
     * create watchface preview container
     */
    lv_obj_t *watchface_manager_app_preview_cont = lv_obj_create( watchface_manager_app_cont, NULL );
    lv_obj_set_size( watchface_manager_app_preview_cont, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) / 2 );
    lv_obj_add_style( watchface_manager_app_preview_cont, LV_OBJ_PART_MAIN, &watchface_manager_style );
    lv_obj_align( watchface_manager_app_preview_cont, watchface_manager_app_cont, LV_ALIGN_IN_TOP_MID, 0, 0 );
    /**
     * create watchface info container
     * + info label and progress bar
     */
    watchface_manager_app_info_cont = lv_obj_create( watchface_manager_app_cont, NULL );
    lv_obj_set_size( watchface_manager_app_info_cont, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) / 4 );
    lv_obj_add_style( watchface_manager_app_info_cont, LV_OBJ_PART_MAIN, &watchface_manager_style );
    lv_obj_align( watchface_manager_app_info_cont, watchface_manager_app_preview_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    watchface_manager_app_info_progressbar = lv_bar_create( watchface_manager_app_info_cont, NULL);
    lv_obj_set_size( watchface_manager_app_info_progressbar, ( lv_disp_get_hor_res( NULL ) / 4 ) * 3, 24 );
    lv_obj_align( watchface_manager_app_info_progressbar, watchface_manager_app_info_cont, LV_ALIGN_IN_TOP_MID, 0, 0 );
    lv_bar_set_anim_time( watchface_manager_app_info_progressbar, 1000 );
    lv_bar_set_value( watchface_manager_app_info_progressbar, 50, LV_ANIM_ON );
    lv_obj_add_style( watchface_manager_app_info_progressbar, LV_BAR_PART_BG, &watchface_manager_style );
    watchface_manager_app_info_label = wf_add_label( watchface_manager_app_info_cont, "Star Trek" );
    lv_obj_align( watchface_manager_app_info_label, watchface_manager_app_info_cont, LV_ALIGN_IN_TOP_MID, 0, 4 );
    /**
     * add exit button
     */
    lv_obj_t *watchface_manager_exit_btn = wf_add_image_button( watchface_manager_app_cont, exit_32px, exit_watchface_manager_app_event_cb, NULL );
    lv_obj_align( watchface_manager_exit_btn, watchface_manager_app_cont, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    /**
     * add setup button
     */
    lv_obj_t *watchface_manager_setup_btn = wf_add_image_button( watchface_manager_app_cont, setup_32px, setup_watchface_manager_app_event_cb, NULL );
    lv_obj_align( watchface_manager_setup_btn, watchface_manager_app_cont, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
}

static void exit_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case LV_EVENT_CLICKED:
            mainbar_jump_back( LV_ANIM_OFF );
            break;
    }
}

static void setup_watchface_manager_app_event_cb(  lv_obj_t * obj, lv_event_t event ) {
    switch ( event ) {
        case LV_EVENT_CLICKED:
            mainbar_jump_to_tilenumber( watchface_manager_get_setup_tile_num(), LV_ANIM_OFF );
            break;
    }
}