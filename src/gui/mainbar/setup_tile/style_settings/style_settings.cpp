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
#include "style_settings.h"
#include "config/styleconfig.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/motion.h"
#include "hardware/motor.h"

lv_obj_t *style_settings_tile=NULL;
lv_obj_t *theme_list=NULL;
uint32_t style_tile_num;

style_config_t style_config;

LV_IMG_DECLARE(style_64px);

static void select_style_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_style_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_style_setup_event_cb( lv_obj_t * obj, lv_event_t event );

void style_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    style_config.load();

    style_tile_num = mainbar_add_setup_tile( 1, 1, "style settings" );
    style_settings_tile = mainbar_get_tile_obj( style_tile_num );

    lv_obj_add_style( style_settings_tile, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );

    icon_t *style_setup_icon = setup_register( "themes", &style_64px, enter_style_setup_event_cb );
    setup_hide_indicator( style_setup_icon );

    lv_obj_t *header = wf_add_settings_header( style_settings_tile, "theme settings", exit_style_setup_event_cb );
    lv_obj_align( header, style_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );

    lv_obj_t *theme_cont = wf_add_labeled_list( style_settings_tile, "theme", &theme_list, "E-Ink\nE-Ink neg\nlight\ndark", select_style_event_cb, SETUP_STYLE );
    lv_obj_align( theme_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    lv_dropdown_set_selected( theme_list, style_config.theme );
    widget_style_theme_set( style_config.theme );
    wf_enable_anim( style_config.anim );

    lv_obj_t *hint_cont = wf_add_label( style_settings_tile, "Not all apps support\ntheme change on the fly.\nAt this time it is only\nfor testing.", SETUP_STYLE );
    lv_obj_align( hint_cont, theme_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );
}


static void select_style_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     widget_style_theme_set( lv_dropdown_get_selected( obj ) );
                                            wf_enable_anim( style_config.anim );
                                            lv_obj_invalidate( lv_scr_act() );
                                            lv_refr_now( NULL );
                                            style_config.theme = lv_dropdown_get_selected( obj );
                                            break;
    }
}

static void enter_style_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( style_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_style_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       style_config.save();
                                        mainbar_jump_back();
                                        break;
    }
}
