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
#include <TTGO.h>

#include "keyboard.h"

static lv_obj_t *kb_screen = NULL;
static lv_obj_t *kb_textarea = NULL;
static lv_obj_t *kb = NULL;
static lv_obj_t *nkb = NULL;
static lv_obj_t *kb_user_textarea = NULL;
static lv_style_t kb_style;
static bool kb_style_initialized = false;

static void kb_event_cb(lv_obj_t * ta, lv_event_t event);

void keyboard_prelim( void ) {
    if( !kb_style_initialized ) {
        lv_style_init( &kb_style );
        lv_style_set_radius( &kb_style, LV_OBJ_PART_MAIN, 0);
        lv_style_set_bg_color( &kb_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
        lv_style_set_bg_opa( &kb_style, LV_OBJ_PART_MAIN, LV_OPA_100);
        lv_style_set_border_width( &kb_style, LV_OBJ_PART_MAIN, 0);
        lv_style_set_text_color( &kb_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
        lv_style_set_image_recolor( &kb_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
            
        kb_screen = lv_cont_create( lv_scr_act(), NULL );
        lv_obj_add_style( kb_screen, LV_OBJ_PART_MAIN, &kb_style );
        lv_obj_set_size( kb_screen, lv_disp_get_hor_res( NULL ) , lv_disp_get_ver_res( NULL ) - 20 );
        lv_obj_align( kb_screen, lv_scr_act(), LV_ALIGN_CENTER, 0, 20);
        
        kb_textarea = lv_textarea_create( kb_screen, NULL );
        lv_obj_add_protect( kb_textarea, LV_PROTECT_CLICK_FOCUS);
        lv_obj_set_size( kb_textarea, lv_disp_get_hor_res( NULL ) - 10, 60 );
        lv_textarea_set_one_line( kb_textarea, true);
        lv_obj_align( kb_textarea, kb_screen, LV_ALIGN_IN_TOP_MID, 0, 5 );
        kb_style_initialized = true;
    }
}

void keyboard_setup( void ) {
    if ( kb != NULL )
        return;

    keyboard_prelim();

    kb = lv_keyboard_create( lv_scr_act() , NULL);
    lv_obj_set_size (kb, lv_disp_get_hor_res( NULL ), ( lv_disp_get_ver_res( NULL ) / 4 ) * 3 - 20);
    lv_obj_align( kb, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_keyboard_set_cursor_manage( kb, true);
    lv_obj_set_event_cb( kb, kb_event_cb );

    keyboard_hide();
}

void num_keyboard_setup( void ) {
    if ( nkb != NULL )
        return;

    keyboard_prelim();
    nkb = lv_keyboard_create( lv_scr_act() , NULL);
    lv_obj_set_size (nkb, lv_disp_get_hor_res( NULL ), ( lv_disp_get_ver_res( NULL ) / 4 ) * 3 - 20);
    lv_obj_align( nkb, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_keyboard_set_mode( nkb, LV_KEYBOARD_MODE_NUM);
    lv_keyboard_set_cursor_manage( nkb, true);
    lv_obj_set_event_cb( nkb, kb_event_cb );

    keyboard_hide();
}

static void kb_event_cb( lv_obj_t * ta, lv_event_t event ) {
    lv_keyboard_def_event_cb( ta, event );
    switch( event ) {
        case( LV_EVENT_CANCEL ):    keyboard_hide();
                                    break;
        case( LV_EVENT_APPLY ):     lv_textarea_set_text( kb_user_textarea, lv_textarea_get_text( kb_textarea ) );
                                    keyboard_hide();
                                    break;
    }
}

void keyboard_set_textarea( lv_obj_t *textarea ){
    if ( kb == NULL )
        return;
    keyboard_show();
    kb_user_textarea = textarea;
    lv_textarea_set_text( kb_textarea, lv_textarea_get_text( textarea ) );
    lv_keyboard_set_textarea( kb, kb_textarea );
}

void num_keyboard_set_textarea( lv_obj_t *textarea ){
    if ( nkb == NULL )
        return;
    num_keyboard_show();
    kb_user_textarea = textarea;
    lv_textarea_set_text( kb_textarea, lv_textarea_get_text( textarea ) );
    lv_keyboard_set_textarea( nkb, kb_textarea );
}

void keyboard_hide( void ) {
    if ( kb_screen != NULL ) {
    	lv_obj_set_hidden( kb_screen, true );
    }

    if ( kb_textarea != NULL) {
    	lv_obj_set_hidden( kb_textarea, true );
    }

    if( kb != NULL ) {
    	lv_obj_set_hidden( kb, true );
    }

    if( nkb != NULL ) {
    	lv_obj_set_hidden( nkb, true );
    }
}

void keyboard_show( void ) {
    if ( kb == NULL )
        return;

    lv_obj_set_hidden( kb_screen, false );
    lv_obj_set_hidden( kb_textarea, false );
    lv_obj_set_hidden( kb, false );
}

void num_keyboard_show( void ) {
    if ( nkb == NULL )
        return;
    lv_obj_set_hidden( kb_screen, false );
    lv_obj_set_hidden( kb_textarea, false );
    lv_obj_set_hidden( nkb, false );
}
