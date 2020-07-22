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

static lv_obj_t *kb = NULL;
static void kb_event_cb(lv_obj_t * ta, lv_event_t event);

void keyboard_setup( void ) {
    if ( kb != NULL )
        return;
        
    kb = lv_keyboard_create( lv_scr_act() , NULL);
//    lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2 );
    lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 3);
    lv_obj_align( kb, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_keyboard_set_cursor_manage(kb, true);
    lv_obj_set_event_cb( kb, kb_event_cb );
    lv_obj_set_hidden( kb, true );

}

static void kb_event_cb(lv_obj_t * ta, lv_event_t event) {
    lv_keyboard_def_event_cb( ta, event );
    if ( event == LV_EVENT_CANCEL ) {
        lv_obj_set_hidden( kb, true );
    }
    if ( event == LV_EVENT_APPLY ) {
        lv_obj_set_hidden( kb, true );
    }
}

void keyboard_set_textarea( lv_obj_t *textarea ){
    if ( kb == NULL )
        return;
    keyboard_show();
    lv_keyboard_set_textarea( kb, textarea );
}

void keyboard_hide( void ) {
    if ( kb == NULL )
        return;
    lv_obj_set_hidden( kb, true );
}

void keyboard_show( void ) {
    if ( kb == NULL )
        return;
    lv_obj_set_hidden( kb, false );
}
