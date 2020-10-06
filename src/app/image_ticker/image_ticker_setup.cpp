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

#include "image_ticker.h"
#include "image_ticker_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"

lv_obj_t *image_ticker_setup_tile = NULL;
lv_style_t image_ticker_setup_style;

lv_obj_t *image_ticker_url_textfield = NULL;
lv_obj_t *image_ticker_autosync_switch = NULL;

LV_IMG_DECLARE(exit_32px);

static void exit_image_ticker_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void image_ticker_url_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void image_ticker_autosync_switch_event_cb( lv_obj_t * obj, lv_event_t event );
static void image_ticker_reset_url_event_cb( lv_obj_t * obj, lv_event_t event );

void image_ticker_setup_setup( uint32_t tile_num ) {

    image_ticker_config_t * image_ticker_config = image_ticker_get_config();

    image_ticker_setup_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &image_ticker_setup_style, mainbar_get_style() );

    lv_style_set_bg_color( &image_ticker_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &image_ticker_setup_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &image_ticker_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( image_ticker_setup_tile, LV_OBJ_PART_MAIN, &image_ticker_setup_style );

    lv_obj_t *exit_cont = lv_obj_create( image_ticker_setup_tile, NULL );
    lv_obj_set_size( exit_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( exit_cont, LV_OBJ_PART_MAIN, &image_ticker_setup_style  );
    lv_obj_align( exit_cont, image_ticker_setup_tile, LV_ALIGN_IN_TOP_MID, 0, 10 );

    lv_obj_t *exit_btn = lv_imgbtn_create( exit_cont, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &image_ticker_setup_style );
    lv_obj_align( exit_btn, exit_cont, LV_ALIGN_IN_TOP_LEFT, 10, 0 );
    lv_obj_set_event_cb( exit_btn, exit_image_ticker_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( exit_cont, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &image_ticker_setup_style  );
    lv_label_set_text( exit_label, "ImgTick setup");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );


    lv_obj_t *image_ticker_url_cont = lv_obj_create( image_ticker_setup_tile, NULL );
    lv_obj_set_size(image_ticker_url_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( image_ticker_url_cont, LV_OBJ_PART_MAIN, &image_ticker_setup_style  );
    lv_obj_align( image_ticker_url_cont, exit_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20 );
    lv_obj_t *image_ticker_url_label = lv_label_create( image_ticker_url_cont, NULL);
    lv_obj_add_style( image_ticker_url_label, LV_OBJ_PART_MAIN, &image_ticker_setup_style  );
    lv_label_set_text( image_ticker_url_label, "URL");
    lv_obj_align( image_ticker_url_label, image_ticker_url_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    image_ticker_url_textfield = lv_textarea_create( image_ticker_url_cont, NULL);
    lv_textarea_set_text( image_ticker_url_textfield, image_ticker_config->url );
    lv_textarea_set_pwd_mode( image_ticker_url_textfield, false);
    lv_textarea_set_one_line( image_ticker_url_textfield, true);
    lv_textarea_set_cursor_hidden( image_ticker_url_textfield, true);
    lv_obj_set_width( image_ticker_url_textfield, LV_HOR_RES /4 * 3 );
    lv_obj_align( image_ticker_url_textfield, image_ticker_url_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( image_ticker_url_textfield, image_ticker_url_textarea_event_cb );


    lv_obj_t *image_ticker_autosync_switch_cont = lv_obj_create( image_ticker_setup_tile, NULL );
    lv_obj_set_size( image_ticker_autosync_switch_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( image_ticker_autosync_switch_cont, LV_OBJ_PART_MAIN, &image_ticker_setup_style  );
    lv_obj_align( image_ticker_autosync_switch_cont, image_ticker_url_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0 );

    image_ticker_autosync_switch = lv_switch_create( image_ticker_autosync_switch_cont, NULL );
    lv_obj_add_protect( image_ticker_autosync_switch, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( image_ticker_autosync_switch, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    image_ticker_config->autosync ? lv_switch_on( image_ticker_autosync_switch, LV_ANIM_ON ) : lv_switch_off( image_ticker_autosync_switch, LV_ANIM_ON );
    lv_obj_align( image_ticker_autosync_switch, image_ticker_autosync_switch_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( image_ticker_autosync_switch, image_ticker_autosync_switch_event_cb );

    lv_obj_t *image_ticker_autosync_switch_label = lv_label_create( image_ticker_autosync_switch_cont, NULL);
    lv_obj_add_style( image_ticker_autosync_switch_label, LV_OBJ_PART_MAIN, &image_ticker_setup_style  );
    lv_label_set_text( image_ticker_autosync_switch_label, "autosync");
    lv_obj_align( image_ticker_autosync_switch_label, image_ticker_autosync_switch_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *image_ticker_reset_url_btn = lv_btn_create( image_ticker_setup_tile, NULL);
    lv_obj_set_event_cb( image_ticker_reset_url_btn, image_ticker_reset_url_event_cb );
    lv_obj_add_style( image_ticker_reset_url_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( image_ticker_reset_url_btn, image_ticker_url_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 60);
    lv_obj_t *image_ticker_reset_url_label = lv_label_create( image_ticker_reset_url_btn, NULL );
    lv_label_set_text( image_ticker_reset_url_label, "set default url");

    if ( image_ticker_config->autosync )
        lv_switch_on( image_ticker_autosync_switch, LV_ANIM_OFF);
    else
        lv_switch_off( image_ticker_autosync_switch, LV_ANIM_OFF);


}


static void image_ticker_url_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        keyboard_set_textarea( obj );
    }
}

static void image_ticker_autosync_switch_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): image_ticker_config_t *image_ticker_config = image_ticker_get_config();
                                        image_ticker_config->autosync = lv_switch_get_state( obj );
                                        break;
    }
}

static void image_ticker_reset_url_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        lv_textarea_set_text( image_ticker_url_textfield, DEFAULT_URL );
    }
}

static void exit_image_ticker_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                    keyboard_hide();
                    image_ticker_config_t *image_ticker_config = image_ticker_get_config();
                    strlcpy( image_ticker_config->url, lv_textarea_get_text( image_ticker_url_textfield ), sizeof( image_ticker_config->url ) );
                    image_ticker_config->autosync = lv_switch_get_state( image_ticker_autosync_switch );
                    image_ticker_save_config();
                    mainbar_jump_to_tilenumber( image_ticker_get_app_main_tile_num(), LV_ANIM_ON );
                                        break;
    }
}
