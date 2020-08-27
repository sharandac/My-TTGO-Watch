/****************************************************************************
 *   Aug 22 16:36:11 2020
 *   Copyright  2020  Chris McNamee
 *   Email: chris.mcna@gmail.com
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

#include "crypto_ticker.h"
#include "crypto_ticker_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"


lv_obj_t *crypto_ticker_setup_tile = NULL;
lv_style_t crypto_ticker_setup_style;

lv_obj_t *crypto_ticker_symbol_textfield = NULL;
lv_obj_t *crypto_ticker_autosync_switch = NULL;

LV_IMG_DECLARE(exit_32px);

static void exit_crypto_ticker_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void crypto_ticker_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void crypto_ticker_autosync_switch_event_cb( lv_obj_t * obj, lv_event_t event );

void crypto_ticker_setup_setup( uint32_t tile_num ) {

    crypto_ticker_config_t * crypto_ticker_config = crypto_ticker_get_config();

    crypto_ticker_setup_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &crypto_ticker_setup_style, mainbar_get_style() );

    lv_style_set_bg_color( &crypto_ticker_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &crypto_ticker_setup_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &crypto_ticker_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( crypto_ticker_setup_tile, LV_OBJ_PART_MAIN, &crypto_ticker_setup_style );

    lv_obj_t *exit_cont = lv_obj_create( crypto_ticker_setup_tile, NULL );
    lv_obj_set_size( exit_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( exit_cont, LV_OBJ_PART_MAIN, &crypto_ticker_setup_style  );
    lv_obj_align( exit_cont, crypto_ticker_setup_tile, LV_ALIGN_IN_TOP_MID, 0, 10 );

    lv_obj_t *exit_btn = lv_imgbtn_create( exit_cont, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &crypto_ticker_setup_style );
    lv_obj_align( exit_btn, exit_cont, LV_ALIGN_IN_TOP_LEFT, 10, 0 );
    lv_obj_set_event_cb( exit_btn, exit_crypto_ticker_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( exit_cont, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &crypto_ticker_setup_style  );
    lv_label_set_text( exit_label, "Crypto Ticker");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );


    lv_obj_t *crypto_ticker_symbol_cont = lv_obj_create( crypto_ticker_setup_tile, NULL );
    lv_obj_set_size(crypto_ticker_symbol_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( crypto_ticker_symbol_cont, LV_OBJ_PART_MAIN, &crypto_ticker_setup_style  );
    lv_obj_align( crypto_ticker_symbol_cont, exit_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20 );
    lv_obj_t *crypto_ticker_symbol_label = lv_label_create( crypto_ticker_symbol_cont, NULL);
    lv_obj_add_style( crypto_ticker_symbol_label, LV_OBJ_PART_MAIN, &crypto_ticker_setup_style  );
    lv_label_set_text( crypto_ticker_symbol_label, "Symbol");
    lv_obj_align( crypto_ticker_symbol_label, crypto_ticker_symbol_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    crypto_ticker_symbol_textfield = lv_textarea_create( crypto_ticker_symbol_cont, NULL);
    lv_textarea_set_text( crypto_ticker_symbol_textfield, crypto_ticker_config->symbol );
    lv_textarea_set_pwd_mode( crypto_ticker_symbol_textfield, false);
    lv_textarea_set_one_line( crypto_ticker_symbol_textfield, true);
    lv_textarea_set_cursor_hidden( crypto_ticker_symbol_textfield, true);
    lv_obj_set_width( crypto_ticker_symbol_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( crypto_ticker_symbol_textfield, crypto_ticker_symbol_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( crypto_ticker_symbol_textfield, crypto_ticker_textarea_event_cb );


    lv_obj_t *crypto_ticker_autosync_switch_cont = lv_obj_create( crypto_ticker_setup_tile, NULL );
    lv_obj_set_size( crypto_ticker_autosync_switch_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( crypto_ticker_autosync_switch_cont, LV_OBJ_PART_MAIN, &crypto_ticker_setup_style  );
    lv_obj_align( crypto_ticker_autosync_switch_cont, crypto_ticker_symbol_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );

    crypto_ticker_autosync_switch = lv_switch_create( crypto_ticker_autosync_switch_cont, NULL );
    lv_obj_add_protect( crypto_ticker_autosync_switch, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( crypto_ticker_autosync_switch, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    crypto_ticker_config->autosync ? lv_switch_on( crypto_ticker_autosync_switch, LV_ANIM_ON ) : lv_switch_off( crypto_ticker_autosync_switch, LV_ANIM_ON );
    lv_obj_align( crypto_ticker_autosync_switch, crypto_ticker_autosync_switch_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( crypto_ticker_autosync_switch, crypto_ticker_autosync_switch_event_cb );

    lv_obj_t *crypto_ticker_autosync_switch_label = lv_label_create( crypto_ticker_autosync_switch_cont, NULL);
    lv_obj_add_style( crypto_ticker_autosync_switch_label, LV_OBJ_PART_MAIN, &crypto_ticker_setup_style  );
    lv_label_set_text( crypto_ticker_autosync_switch_label, "autosync");
    lv_obj_align( crypto_ticker_autosync_switch_label, crypto_ticker_autosync_switch_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
}



static void crypto_ticker_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void crypto_ticker_autosync_switch_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): crypto_ticker_config_t *crypto_ticker_config = crypto_ticker_get_config();
                                        crypto_ticker_config->autosync = lv_switch_get_state( obj );
                                        break;
    }
}

static void exit_crypto_ticker_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        keyboard_hide();
                                        crypto_ticker_config_t *crypto_ticker_config = crypto_ticker_get_config();
                                        strlcpy( crypto_ticker_config->symbol, lv_textarea_get_text( crypto_ticker_symbol_textfield ), sizeof( crypto_ticker_config->symbol ) );
                                        crypto_ticker_save_config();
                                        crypto_ticker_jump_to_main();
                                        break;
    }
}
