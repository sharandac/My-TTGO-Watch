/****************************************************************************
 *   Aug 14 12:37:31 2020
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
#include "bluetooth_pairing.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"
#include "hardware/blectl.h"
#include "hardware/powermgm.h"
#include "hardware/motor.h"

lv_obj_t *bluetooth_pairing_tile=NULL;
lv_style_t bluetooth_pairing_style;
uint32_t bluetooth_pairing_tile_num;

lv_obj_t *bluetooth_pairing_img = NULL;
lv_obj_t *bluetooth_pairing_info_label = NULL;

LV_IMG_DECLARE(cancel_32px);
LV_IMG_DECLARE(bluetooth_64px);

LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);
LV_FONT_DECLARE(Ubuntu_48px);
LV_FONT_DECLARE(Ubuntu_72px);

#if defined( BIG_THEME )
    lv_font_t *pin_font = &Ubuntu_72px;
#elif defined( MID_THEME )
    lv_font_t *pin_font = &Ubuntu_48px;
#else
    lv_font_t *pin_font = &Ubuntu_32px;
#endif

bool bluetooth_pairing_style_change_event_cb( EventBits_t event, void * arg );
static void exit_bluetooth_pairing_event_cb( lv_obj_t * obj, lv_event_t event );
bool bluetooth_pairing_event_cb( EventBits_t event, void *arg );

void bluetooth_pairing_tile_setup( void ) {
    // get an app tile and copy mainstyle
    bluetooth_pairing_tile_num = mainbar_add_app_tile( 1, 1, "bluetooth pairing" );
    bluetooth_pairing_tile = mainbar_get_tile_obj( bluetooth_pairing_tile_num );

    lv_style_copy( &bluetooth_pairing_style, SETUP_STYLE );
    lv_style_set_text_font( &bluetooth_pairing_style, LV_STATE_DEFAULT, pin_font );
    lv_obj_add_style( bluetooth_pairing_tile, LV_OBJ_PART_MAIN, &bluetooth_pairing_style );

    lv_obj_t *bluetooth_pairing_exit_btn = wf_add_exit_button( bluetooth_pairing_tile, exit_bluetooth_pairing_event_cb );
    lv_obj_align( bluetooth_pairing_exit_btn, bluetooth_pairing_tile, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, THEME_PADDING );

    bluetooth_pairing_img = lv_img_create( bluetooth_pairing_tile, NULL );
    lv_img_set_src( bluetooth_pairing_img, &bluetooth_64px );
    lv_obj_align( bluetooth_pairing_img, bluetooth_pairing_tile, LV_ALIGN_CENTER, 0, 0 );

    bluetooth_pairing_info_label = lv_label_create( bluetooth_pairing_tile, NULL);
    lv_obj_add_style( bluetooth_pairing_info_label, LV_OBJ_PART_MAIN, &bluetooth_pairing_style  );
    lv_label_set_text( bluetooth_pairing_info_label, "");
    lv_obj_align( bluetooth_pairing_info_label, bluetooth_pairing_img, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    blectl_register_cb( BLECTL_PIN_AUTH | BLECTL_PAIRING_SUCCESS | BLECTL_PAIRING_ABORT, bluetooth_pairing_event_cb, "bluetooth pairing" );
    styles_register_cb( STYLE_CHANGE, bluetooth_pairing_style_change_event_cb, "bluetooth pairing style" );
}

bool bluetooth_pairing_style_change_event_cb( EventBits_t event, void * arg ) {
    switch( event ) {
        case STYLE_CHANGE:  lv_style_copy( &bluetooth_pairing_style, SETUP_STYLE );
                            lv_style_set_text_font( &bluetooth_pairing_style, LV_STATE_DEFAULT, pin_font );
                            break;
    }
    return( true );
}
bool bluetooth_pairing_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BLECTL_PIN_AUTH:           statusbar_hide( true );
                                        powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                                        mainbar_jump_to_tilenumber( bluetooth_pairing_tile_num, LV_ANIM_OFF );
                                        lv_label_set_text( bluetooth_pairing_info_label, (const char*)arg );
                                        lv_obj_align( bluetooth_pairing_info_label, bluetooth_pairing_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
                                        lv_obj_invalidate( lv_scr_act() );
                                        motor_vibe(20);
                                        break;
        case BLECTL_PAIRING_SUCCESS:    statusbar_hide( true );
                                        powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                                        lv_label_set_text( bluetooth_pairing_info_label, (const char*)arg );
                                        lv_obj_align( bluetooth_pairing_info_label, bluetooth_pairing_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
                                        mainbar_jump_to_tilenumber( bluetooth_pairing_tile_num, LV_ANIM_OFF );
                                        lv_obj_invalidate( lv_scr_act() );
                                        motor_vibe(20);
                                        break;
        case BLECTL_PAIRING_ABORT:      statusbar_hide( true );
                                        powermgm_set_event( POWERMGM_WAKEUP_REQUEST );
                                        lv_label_set_text( bluetooth_pairing_info_label, (const char*)arg );
                                        lv_obj_align( bluetooth_pairing_info_label, bluetooth_pairing_img, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
                                        mainbar_jump_to_tilenumber( bluetooth_pairing_tile_num, LV_ANIM_OFF );
                                        lv_obj_invalidate( lv_scr_act() );
                                        motor_vibe(20);
                                        break;
    }
    return( true );
}

static void exit_bluetooth_pairing_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

