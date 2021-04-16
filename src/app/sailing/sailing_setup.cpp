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

#include "sailing.h"
#include "sailing_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

lv_obj_t *sailing_setup_tile = NULL;
lv_style_t sailing_setup_style;

lv_obj_t *sailing_foobar_switch = NULL;

LV_IMG_DECLARE(exit_32px);

static void exit_sailing_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void sailing_foobar_switch_event_cb( lv_obj_t * obj, lv_event_t event );

void sailing_setup_setup( uint32_t tile_num ) {

    sailing_setup_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &sailing_setup_style, mainbar_get_style() );

    lv_style_set_bg_color( &sailing_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &sailing_setup_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &sailing_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( sailing_setup_tile, LV_OBJ_PART_MAIN, &sailing_setup_style );

    lv_obj_t *exit_cont = lv_obj_create( sailing_setup_tile, NULL );
    lv_obj_set_size( exit_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( exit_cont, LV_OBJ_PART_MAIN, &sailing_setup_style  );
    lv_obj_align( exit_cont, sailing_setup_tile, LV_ALIGN_IN_TOP_MID, 0, 10 );

    lv_obj_t *exit_btn = lv_imgbtn_create( exit_cont, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &sailing_setup_style );
    lv_obj_align( exit_btn, exit_cont, LV_ALIGN_IN_TOP_LEFT, 10, 0 );
    lv_obj_set_event_cb( exit_btn, exit_sailing_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( exit_cont, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &sailing_setup_style  );
    lv_label_set_text( exit_label, "my sailing app setup");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *sailing_foobar_switch_cont = lv_obj_create( sailing_setup_tile, NULL );
    lv_obj_set_size( sailing_foobar_switch_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( sailing_foobar_switch_cont, LV_OBJ_PART_MAIN, &sailing_setup_style  );
    lv_obj_align( sailing_foobar_switch_cont, exit_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );

    sailing_foobar_switch = lv_switch_create( sailing_foobar_switch_cont, NULL );
    lv_obj_add_protect( sailing_foobar_switch, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( sailing_foobar_switch, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( sailing_foobar_switch, LV_ANIM_ON );
    lv_obj_align( sailing_foobar_switch, sailing_foobar_switch_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( sailing_foobar_switch, sailing_foobar_switch_event_cb );

    lv_obj_t *sailing_foobar_switch_label = lv_label_create( sailing_foobar_switch_cont, NULL);
    lv_obj_add_style( sailing_foobar_switch_label, LV_OBJ_PART_MAIN, &sailing_setup_style  );
    lv_label_set_text( sailing_foobar_switch_label, "foo bar");
    lv_obj_align( sailing_foobar_switch_label, sailing_foobar_switch_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
}

static void sailing_foobar_switch_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): Serial.printf( "switch value = %d\r\n", lv_switch_get_state( obj ) );
                                        break;
    }
}

static void exit_sailing_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( sailing_get_app_main_tile_num(), LV_ANIM_ON );
                                        break;
    }
}