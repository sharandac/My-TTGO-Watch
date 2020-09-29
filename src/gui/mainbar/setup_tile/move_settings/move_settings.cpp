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

#include "hardware/bma.h"
#include "hardware/motor.h"

lv_obj_t *move_settings_tile=NULL;
lv_style_t move_settings_style;
uint32_t move_tile_num;

lv_obj_t *stepcounter_onoff=NULL;
lv_obj_t *doubleclick_onoff=NULL;
lv_obj_t *tilt_onoff=NULL;
lv_obj_t *daily_stepcounter_onoff=NULL;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(move_64px);

static void enter_move_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_move_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void stepcounter_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void doubleclick_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void tilt_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void daily_stepcounter_onoff_event_handler(lv_obj_t * obj, lv_event_t event);

void move_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    move_tile_num = mainbar_add_app_tile( 1, 1, "move settings" );
    move_settings_tile = mainbar_get_tile_obj( move_tile_num );
    lv_style_copy( &move_settings_style, mainbar_get_style() );
    lv_style_set_bg_color( &move_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &move_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &move_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( move_settings_tile, LV_OBJ_PART_MAIN, &move_settings_style );

    icon_t *move_setup_icon = setup_register( "move", &move_64px, enter_move_setup_event_cb );
    setup_hide_indicator( move_setup_icon );

    lv_obj_t *exit_btn = lv_imgbtn_create( move_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &move_settings_style );
    lv_obj_align( exit_btn, move_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_move_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( move_settings_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &move_settings_style  );
    lv_label_set_text( exit_label, "movement settings");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *stepcounter_cont = lv_obj_create( move_settings_tile, NULL );
    lv_obj_set_size(stepcounter_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( stepcounter_cont, LV_OBJ_PART_MAIN, &move_settings_style  );
    lv_obj_align( stepcounter_cont, move_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    stepcounter_onoff = lv_switch_create( stepcounter_cont, NULL );
    lv_obj_add_protect( stepcounter_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( stepcounter_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( stepcounter_onoff, LV_ANIM_ON );
    lv_obj_align( stepcounter_onoff, stepcounter_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( stepcounter_onoff, stepcounter_onoff_event_handler );
    lv_obj_t *stepcounter_label = lv_label_create( stepcounter_cont, NULL);
    lv_obj_add_style( stepcounter_label, LV_OBJ_PART_MAIN, &move_settings_style  );
    lv_label_set_text( stepcounter_label, "step counter");
    lv_obj_align( stepcounter_label, stepcounter_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *doubleclick_cont = lv_obj_create( move_settings_tile, NULL );
    lv_obj_set_size(doubleclick_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( doubleclick_cont, LV_OBJ_PART_MAIN, &move_settings_style  );
    lv_obj_align( doubleclick_cont, stepcounter_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    doubleclick_onoff = lv_switch_create( doubleclick_cont, NULL );
    lv_obj_add_protect( doubleclick_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( doubleclick_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( doubleclick_onoff, LV_ANIM_ON );
    lv_obj_align( doubleclick_onoff, doubleclick_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( doubleclick_onoff, doubleclick_onoff_event_handler );
    lv_obj_t *doubleclick_label = lv_label_create( doubleclick_cont, NULL);
    lv_obj_add_style( doubleclick_label, LV_OBJ_PART_MAIN, &move_settings_style  );
    lv_label_set_text( doubleclick_label, "double click");
    lv_obj_align( doubleclick_label, doubleclick_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *tilt_cont = lv_obj_create( move_settings_tile, NULL );
    lv_obj_set_size(tilt_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( tilt_cont, LV_OBJ_PART_MAIN, &move_settings_style  );
    lv_obj_align( tilt_cont, doubleclick_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    tilt_onoff = lv_switch_create( tilt_cont, NULL );
    lv_obj_add_protect( tilt_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( tilt_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( tilt_onoff, LV_ANIM_ON );
    lv_obj_align( tilt_onoff, tilt_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( tilt_onoff, tilt_onoff_event_handler );
    lv_obj_t *tilt_label = lv_label_create( tilt_cont, NULL);
    lv_obj_add_style( tilt_label, LV_OBJ_PART_MAIN, &move_settings_style  );
    lv_label_set_text( tilt_label, "tilt");
    lv_obj_align( tilt_label, tilt_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *daily_stepcounter_cont = lv_obj_create( move_settings_tile, NULL );
    lv_obj_set_size(daily_stepcounter_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( daily_stepcounter_cont, LV_OBJ_PART_MAIN, &move_settings_style  );
    lv_obj_align( daily_stepcounter_cont, tilt_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    daily_stepcounter_onoff = lv_switch_create( daily_stepcounter_cont, NULL );
    lv_obj_add_protect( daily_stepcounter_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( daily_stepcounter_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( daily_stepcounter_onoff, LV_ANIM_ON );
    lv_obj_align( daily_stepcounter_onoff, daily_stepcounter_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( daily_stepcounter_onoff, daily_stepcounter_onoff_event_handler );
    lv_obj_t *daily_stepcounter_label = lv_label_create( daily_stepcounter_cont, NULL);
    lv_obj_add_style( daily_stepcounter_label, LV_OBJ_PART_MAIN, &move_settings_style  );
    lv_label_set_text( daily_stepcounter_label, "daily stepcounter");
    lv_obj_align( daily_stepcounter_label, daily_stepcounter_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    if ( bma_get_config( BMA_DOUBLECLICK ) )
        lv_switch_on( doubleclick_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( doubleclick_onoff, LV_ANIM_OFF );

    if ( bma_get_config( BMA_STEPCOUNTER ) )
        lv_switch_on( stepcounter_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( stepcounter_onoff, LV_ANIM_OFF );

    if ( bma_get_config( BMA_TILT ) )
        lv_switch_on( tilt_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( tilt_onoff, LV_ANIM_OFF );

    if ( bma_get_config( BMA_DAILY_STEPCOUNTER ) )
        lv_switch_on( daily_stepcounter_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( daily_stepcounter_onoff, LV_ANIM_OFF );
}


static void enter_move_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( move_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_move_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
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