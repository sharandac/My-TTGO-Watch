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
#include "keyboard.h"
#include "statusbar.h"
#include "widget_factory.h"

static lv_obj_t *kb_screen = NULL;
static lv_obj_t *kb_textarea = NULL;
static lv_obj_t *kb = NULL;
static lv_obj_t *nkb = NULL;
static lv_obj_t *kb_user_textarea = NULL;
static bool kb_style_initialized = false;

static void kb_event_cb(lv_obj_t * ta, lv_event_t event);

void keyboard_prelim( void ) {
    if( !kb_style_initialized ) {
        kb_screen = lv_cont_create( lv_scr_act(), NULL );
        lv_obj_add_style( kb_screen, LV_OBJ_PART_MAIN, SETUP_STYLE );
        lv_obj_set_size( kb_screen, lv_disp_get_hor_res( NULL ) , lv_disp_get_ver_res( NULL ) );
        lv_obj_align( kb_screen, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, STATUSBAR_HEIGHT );
        
        kb_textarea = lv_textarea_create( kb_screen, NULL );
        lv_obj_add_protect( kb_textarea, LV_PROTECT_CLICK_FOCUS );
        lv_obj_set_size( kb_textarea, lv_disp_get_hor_res( NULL ) - THEME_PADDING, 40 );
        lv_textarea_set_one_line( kb_textarea, true);
        lv_obj_align( kb_textarea, kb_screen, LV_ALIGN_IN_TOP_MID, 0, THEME_PADDING );
        kb_style_initialized = true;
    }
}


void keyboard_setup( void ) {
    /*
     * check if keyboard already initialized
     */
    if ( kb != NULL )
        return;

    keyboard_prelim();

    kb = lv_keyboard_create( kb_screen , NULL);
    lv_obj_set_size( kb, lv_disp_get_hor_res( NULL ), ( ( lv_disp_get_ver_res( NULL ) / 4 ) * 3 ) > 240 ? 240:( ( lv_disp_get_ver_res( NULL ) / 4 ) * 3 - 20 )  );
    lv_obj_align( kb, kb_screen, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_obj_add_style( kb, LV_OBJ_PART_ALL, SETUP_STYLE );
    lv_obj_add_style( kb, LV_KEYBOARD_PART_BTN, ws_get_button_style() );
    lv_keyboard_set_cursor_manage( kb, true);
    lv_obj_set_event_cb( kb, kb_event_cb );

    keyboard_hide();
}

void num_keyboard_setup( void ) {
    /*
     * check if keyboard already initialized
     */
    if ( nkb != NULL )
        return;

    keyboard_prelim();
    nkb = lv_keyboard_create( kb_screen , NULL);
    lv_obj_set_size( nkb, lv_disp_get_hor_res( NULL ), ( ( lv_disp_get_ver_res( NULL ) / 4 ) * 3 ) > 200 ? 200:( ( lv_disp_get_ver_res( NULL ) / 4 ) * 3 - 20 ) );
    lv_obj_align( nkb, kb_screen, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_obj_add_style( nkb, LV_OBJ_PART_ALL, SETUP_STYLE );
    lv_obj_add_style( nkb, LV_KEYBOARD_PART_BTN, ws_get_button_style() );
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
    /*
     * check if keyboard already initialized
     */
    if ( kb == NULL )
        return;

    keyboard_show();
    kb_user_textarea = textarea;
    lv_textarea_set_text( kb_textarea, lv_textarea_get_text( textarea ) );
    lv_keyboard_set_textarea( kb, kb_textarea );
}

void num_keyboard_set_textarea( lv_obj_t *textarea ){
    /*
     * check if keyboard already initialized
     */
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
    /*
     * check if keyboard already initialized
     */
    if ( kb == NULL )
        return;

    lv_obj_set_hidden( kb_screen, false );
    lv_obj_set_hidden( kb_textarea, false );
    lv_obj_set_hidden( kb, false );
    lv_obj_align( kb_screen, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, statusbar_get_hidden_state()?0:STATUSBAR_HEIGHT );
    lv_obj_align( kb, kb_screen, LV_ALIGN_IN_BOTTOM_MID, 0, statusbar_get_hidden_state()?0:-STATUSBAR_HEIGHT );
    lv_obj_align( nkb, kb_screen, LV_ALIGN_IN_BOTTOM_MID, 0, statusbar_get_hidden_state()?0:-STATUSBAR_HEIGHT );

}

void num_keyboard_show( void ) {
    /*
     * check if keyboard already initialized
     */
    if ( nkb == NULL )
        return;
    lv_obj_set_hidden( kb_screen, false );
    lv_obj_set_hidden( kb_textarea, false );
    lv_obj_set_hidden( nkb, false );
    lv_obj_align( kb_screen, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, statusbar_get_hidden_state()?0:STATUSBAR_HEIGHT );
    lv_obj_align( kb, kb_screen, LV_ALIGN_IN_BOTTOM_MID, 0, statusbar_get_hidden_state()?0:-STATUSBAR_HEIGHT );
    lv_obj_align( nkb, kb_screen, LV_ALIGN_IN_BOTTOM_MID, 0, statusbar_get_hidden_state()?0:-STATUSBAR_HEIGHT );
 }
