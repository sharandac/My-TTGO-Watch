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
#include "move_settings.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/motion.h"
#include "hardware/motor.h"

lv_obj_t *move_settings_tile=NULL;
uint32_t move_tile_num;

lv_obj_t *stepcounter_onoff=NULL;
lv_obj_t *doubleclick_onoff=NULL;
lv_obj_t *tilt_onoff=NULL;
lv_obj_t *daily_stepcounter_onoff=NULL;

LV_IMG_DECLARE(move_64px);

static void enter_move_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_move_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void stepcounter_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void doubleclick_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void tilt_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void daily_stepcounter_onoff_event_handler(lv_obj_t * obj, lv_event_t event);

void move_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    move_tile_num = mainbar_add_setup_tile( 1, 1, "move settings" );
    move_settings_tile = mainbar_get_tile_obj( move_tile_num );

    lv_obj_add_style( move_settings_tile, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );

    icon_t *move_setup_icon = setup_register( "move", &move_64px, enter_move_setup_event_cb );
    setup_hide_indicator( move_setup_icon );

    lv_obj_t *header = wf_add_settings_header( move_settings_tile, "movement settings", exit_move_setup_event_cb );
    lv_obj_align( header, move_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );

    lv_obj_t *stepcounter_cont = wf_add_labeled_switch( move_settings_tile, "step counter", &stepcounter_onoff, bma_get_config( BMA_STEPCOUNTER ), stepcounter_onoff_event_handler, ws_get_setup_tile_style() );
    lv_obj_align( stepcounter_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, 8 );

    lv_obj_t *doubleclick_cont = wf_add_labeled_switch( move_settings_tile, "double click", &doubleclick_onoff, bma_get_config( BMA_DOUBLECLICK ), doubleclick_onoff_event_handler, ws_get_setup_tile_style() );
    lv_obj_align( doubleclick_cont, stepcounter_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 8 );

    lv_obj_t *tilt_cont = wf_add_labeled_switch( move_settings_tile, "tilt", &tilt_onoff, bma_get_config( BMA_TILT ), tilt_onoff_event_handler, ws_get_setup_tile_style() );
    lv_obj_align( tilt_cont, doubleclick_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 8 );

    lv_obj_t *daily_stepcounter_cont = wf_add_labeled_switch( move_settings_tile, "daily stepcounter", &daily_stepcounter_onoff, bma_get_config( BMA_DAILY_STEPCOUNTER ), daily_stepcounter_onoff_event_handler, ws_get_setup_tile_style() );
    lv_obj_align( daily_stepcounter_cont, tilt_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 8 );
}


static void enter_move_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( move_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_move_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}

static void stepcounter_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED):  bma_set_config( BMA_STEPCOUNTER, lv_switch_get_state( obj ) );
    }
}

static void doubleclick_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED):  bma_set_config( BMA_DOUBLECLICK, lv_switch_get_state( obj ) );
    }
}

static void tilt_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED):  bma_set_config( BMA_TILT, lv_switch_get_state( obj ) );
    }
}

static void daily_stepcounter_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED):  bma_set_config( BMA_DAILY_STEPCOUNTER, lv_switch_get_state( obj ) );
    }
}
