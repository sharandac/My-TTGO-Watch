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
#include "quickglui/quickglui.h"

#include "watchface_app.h"
#include "watchface_app_main.h"
#include "watchface_app_tile.h"
#include "app/watchface/config/watchface_config.h"

#include "gui/mainbar/mainbar.h"
#include "gui/widget_styles.h"
#include "gui/widget_factory.h"

lv_obj_t *watchface_app_main_tile = NULL;              /** @brief osm main tile obj */
lv_style_t watchface_app_main_style;
lv_style_t watchface_app_button_style;
lv_obj_t *watchface_exit_btn = NULL;                          /** @brief osm exit icon/button obj */
lv_obj_t *watchface_onoff = NULL;
watchface_config_t watchface_config;

LV_IMG_DECLARE(exit_32px);
static void watchface_app_reload_and_test_cb( lv_obj_t *obj, lv_event_t event );
static void watchface_enable_event_cb( lv_obj_t *obj, lv_event_t event );
static void exit_watchface_app_main_event_cb( lv_obj_t * obj, lv_event_t event );

void watchface_app_main_setup( uint32_t tile_num ) {
    /**
     * load watchface config file
     */
    watchface_config.load();
    /**
     * geht app tile
     */
    watchface_app_main_tile = mainbar_get_tile_obj( tile_num );
    /**
     * copy styles from mainbar
     */
    lv_style_copy( &watchface_app_main_style, ws_get_mainbar_style() );
    lv_obj_add_style( watchface_app_main_tile, LV_OBJ_PART_MAIN, &watchface_app_main_style );
    lv_style_copy( &watchface_app_button_style, ws_get_button_style() );

    watchface_exit_btn = lv_imgbtn_create( watchface_app_main_tile, NULL);
    lv_imgbtn_set_src( watchface_exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( watchface_exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( watchface_exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( watchface_exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( watchface_exit_btn, LV_IMGBTN_PART_MAIN, &watchface_app_main_style );
    lv_obj_align( watchface_exit_btn, watchface_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( watchface_exit_btn, exit_watchface_app_main_event_cb );

    lv_obj_t *watchface_onoff_cont = lv_obj_create( watchface_app_main_tile, NULL );
    lv_obj_set_size( watchface_onoff_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( watchface_onoff_cont, LV_OBJ_PART_MAIN, &watchface_app_main_style );
    lv_obj_align( watchface_onoff_cont, watchface_app_main_tile, LV_ALIGN_IN_TOP_MID, 0,35 );
    watchface_onoff = wf_add_switch( watchface_onoff_cont, false );
    lv_obj_align( watchface_onoff, watchface_onoff_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( watchface_onoff, watchface_enable_event_cb );
    lv_obj_t *watchface_onoff_label = lv_label_create( watchface_onoff_cont, NULL);
    lv_obj_add_style( watchface_onoff_label, LV_OBJ_PART_MAIN, &watchface_app_main_style );
    lv_label_set_text( watchface_onoff_label, "enable watchface");
    lv_obj_align( watchface_onoff_label, watchface_onoff_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *watchface_reload_and_test_btn = lv_btn_create( watchface_app_main_tile, NULL );
    lv_obj_align( watchface_reload_and_test_btn, watchface_app_main_tile, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_event_cb( watchface_reload_and_test_btn, watchface_app_reload_and_test_cb );
    lv_obj_add_style( watchface_reload_and_test_btn, LV_OBJ_PART_MAIN, &watchface_app_button_style );
    lv_obj_t *watchface_reload_and_test_btn_label = lv_label_create( watchface_reload_and_test_btn, NULL );
    lv_label_set_text( watchface_reload_and_test_btn_label, "reload and test");

    if ( watchface_config.watchface_enable )
        lv_switch_on( watchface_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( watchface_onoff, LV_ANIM_OFF );

    watchface_enable_tile_after_wakeup( lv_switch_get_state( watchface_onoff ) );
}

static void watchface_app_reload_and_test_cb( lv_obj_t *obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_CLICKED:
            watchface_reload_and_test( watchface_app_get_app_main_tile_num() );
            break;
    }
}

static void watchface_enable_event_cb( lv_obj_t *obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     watchface_enable_tile_after_wakeup( lv_switch_get_state( obj ) );
                                            watchface_config.watchface_enable = lv_switch_get_state( obj );
                                            watchface_config.save();
                                            break;
    }
}

static void exit_watchface_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            /**
             * exit to mainbar
             */
            mainbar_jump_to_maintile( LV_ANIM_OFF );
            break;
    }
}