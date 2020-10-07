/****************************************************************************
 *   Oct 05 23:05:42 2020
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
#include "bluetooth_media.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/sound/piep.h"

#include "hardware/blectl.h"

lv_obj_t *bluetooth_media_tile = NULL;
lv_style_t bluetooth_media_style;
uint32_t bluetooth_media_tile_num;

lv_obj_t *bluetooth_media_play = NULL;
lv_obj_t *bluetooth_media_prev = NULL;
lv_obj_t *bluetooth_media_next = NULL;

LV_IMG_DECLARE(cancel_32px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

bool bluetooth_media_event_cb( EventBits_t event, void *arg );
static void exit_bluetooth_media_event_cb( lv_obj_t * obj, lv_event_t event );

void bluetooth_media_tile_setup( void ) {
    // get an app tile and copy mainstyle
    bluetooth_media_tile_num = mainbar_add_app_tile( 1, 1, "bluetooth media" );
    bluetooth_media_tile = mainbar_get_tile_obj( bluetooth_media_tile_num );

    lv_style_copy( &bluetooth_media_style, mainbar_get_style() );
    lv_style_set_bg_color( &bluetooth_media_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &bluetooth_media_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &bluetooth_media_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font( &bluetooth_media_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_obj_add_style( bluetooth_media_tile, LV_OBJ_PART_MAIN, &bluetooth_media_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( bluetooth_media_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &cancel_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &cancel_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &bluetooth_media_style );
    lv_obj_align( exit_btn, bluetooth_media_tile, LV_ALIGN_IN_TOP_RIGHT, -10, 10 );
    lv_obj_set_event_cb( exit_btn, exit_bluetooth_media_event_cb );

    blectl_register_cb( BLECTL_MSG, bluetooth_media_event_cb, "bluetooth media" );
}

static void exit_bluetooth_media_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

bool bluetooth_media_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_MSG:            
            break;
    }
    return( true );
}