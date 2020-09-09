/****************************************************************************
 *   Sep 2 08:21:51 2020
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

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"

#include "setup.h"

icon_t *setup_register( const char* setupname, const lv_img_dsc_t *icon, lv_event_cb_t event_cb ) {

    icon_t *setup = setup_tile_get_free_setup_icon();

    if ( setup == NULL ) {
        log_e("no free app icon");
        return( NULL );
    }

    setup->active = true;
    // setup label
    lv_label_set_text( setup->label, setupname );
    lv_obj_align( setup->label , setup->icon_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_label_set_align( setup->label, LV_LABEL_ALIGN_CENTER );
    lv_obj_set_hidden( setup->icon_cont, false );
    lv_obj_set_hidden( setup->label, false );
    // setup icon and set event callback
    setup->icon_img = lv_imgbtn_create( setup->icon_cont , NULL );
    lv_imgbtn_set_src( setup->icon_img, LV_BTN_STATE_RELEASED, icon);
    lv_imgbtn_set_src( setup->icon_img, LV_BTN_STATE_PRESSED, icon);
    lv_imgbtn_set_src( setup->icon_img, LV_BTN_STATE_CHECKED_RELEASED, icon);
    lv_imgbtn_set_src( setup->icon_img, LV_BTN_STATE_CHECKED_PRESSED, icon);
    lv_obj_reset_style_list( setup->icon_img, LV_OBJ_PART_MAIN );
    lv_obj_align( setup->icon_img , setup->icon_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( setup->icon_img, event_cb );
    // setup icon indicator
    setup->icon_indicator = lv_img_create( setup->icon_cont, NULL );
    lv_img_set_src( setup->icon_indicator, &info_ok_16px );
    lv_obj_align( setup->icon_indicator, setup->icon_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( setup->icon_indicator, true );
    mainbar_add_slide_element( setup->icon_img );
    
    lv_obj_invalidate( lv_scr_act() );

    return( setup );
}

void setup_set_indicator( icon_t *setup, icon_indicator_t indicator ) {
    if ( setup == NULL ) {
        return;
    }

    if ( setup->active == false ) {
        return;
    }

    switch( indicator ) {
        case ICON_INDICATOR_OK:     lv_img_set_src( setup->icon_indicator, &info_ok_16px );
                                    break;
        case ICON_INDICATOR_FAIL:   lv_img_set_src( setup->icon_indicator, &info_fail_16px );
                                    break;
        case ICON_INDICATOR_UPDATE: lv_img_set_src( setup->icon_indicator, &info_update_16px );
                                    break;
        case ICON_INDICATOR_1:      lv_img_set_src( setup->icon_indicator, &info_1_16px );
                                    break;
        case ICON_INDICATOR_2:      lv_img_set_src( setup->icon_indicator, &info_2_16px );
                                    break;
        case ICON_INDICATOR_3:      lv_img_set_src( setup->icon_indicator, &info_3_16px );
                                    break;
        case ICON_INDICATOR_N:      lv_img_set_src( setup->icon_indicator, &info_n_16px );
                                    break;
    }
    lv_obj_align( setup->icon_indicator, setup->icon_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( setup->icon_indicator, false );
    lv_obj_invalidate( lv_scr_act() );
}

void setup_hide_indicator( icon_t *setup ) {
    if ( setup == NULL ) {
        return;
    }

    if ( setup->active == false ) {
        return;
    }

    lv_obj_set_hidden( setup->icon_indicator, true );
    lv_obj_invalidate( lv_scr_act() );
}

void setup_set_icon( icon_t *setup, lv_obj_t *icon ) {
    if ( setup == NULL ) {
        return;
    }

    if ( setup->active == false ) {
        return;
    }

    lv_imgbtn_set_src( setup->icon_img, LV_BTN_STATE_RELEASED, icon);
    lv_imgbtn_set_src( setup->icon_img, LV_BTN_STATE_PRESSED, icon);
    lv_imgbtn_set_src( setup->icon_img, LV_BTN_STATE_CHECKED_RELEASED, icon);
    lv_imgbtn_set_src( setup->icon_img, LV_BTN_STATE_CHECKED_PRESSED, icon);
    lv_obj_reset_style_list( setup->icon_img, LV_OBJ_PART_MAIN );
    lv_obj_align( setup->icon_img , setup->icon_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_invalidate( lv_scr_act() );
}