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
#include "gui/mainbar/main_tile/main_tile.h"

#include "widget.h"

icon_t *widget_register( const char* widgetname, const lv_img_dsc_t *icon, lv_event_cb_t event_cb ) {

    icon_t *widget = main_tile_get_free_widget_icon();

    if ( widget == NULL ) {
        log_e("no free widget icon");
        return( NULL );
    }

    widget->active = true;
    // setup label and ext_label
    lv_label_set_text( widget->label, widgetname );
    lv_obj_align( widget->label , widget->icon_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_label_set_text( widget->ext_label, "" );
    lv_obj_align( widget->ext_label , widget->label, LV_ALIGN_OUT_TOP_MID, 0, 0 );
    // setup icon and set event callback
    lv_imgbtn_set_src( widget->icon_img, LV_BTN_STATE_RELEASED, icon);
    lv_imgbtn_set_src( widget->icon_img, LV_BTN_STATE_PRESSED, icon);
    lv_imgbtn_set_src( widget->icon_img, LV_BTN_STATE_CHECKED_RELEASED, icon);
    lv_imgbtn_set_src( widget->icon_img, LV_BTN_STATE_CHECKED_PRESSED, icon);
    lv_obj_reset_style_list( widget->icon_img, LV_OBJ_PART_MAIN );
    lv_obj_align( widget->icon_img , widget->icon_cont, LV_ALIGN_IN_TOP_MID, 0, 0 );
    lv_obj_set_event_cb( widget->icon_img, event_cb );
    // setup icon indicator
    lv_img_set_src( widget->icon_indicator, &info_ok_16px );
    lv_obj_align( widget->icon_indicator, widget->icon_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );

    lv_obj_set_hidden( widget->icon_cont, false );
    lv_obj_set_hidden( widget->label, false );
    lv_obj_set_hidden( widget->ext_label, false );
    lv_obj_set_hidden( widget->icon_img, false );
    lv_obj_set_hidden( widget->icon_indicator, false );

    mainbar_add_slide_element( widget->icon_cont );
    mainbar_add_slide_element( widget->icon_img );
    main_tile_align_widgets();
    lv_obj_invalidate( lv_scr_act() );

    return( widget );
}

icon_t *widget_remove( icon_t *widget ) {

    if ( widget == NULL ) {
        log_e("no widget icon selected");
        return( NULL );
    }

    widget->active = false;
    lv_obj_set_hidden( widget->icon_cont, true );
    lv_obj_set_hidden( widget->icon_img, true );
    lv_obj_set_hidden( widget->icon_indicator, true );
    lv_obj_set_hidden( widget->label, true );
    lv_obj_set_hidden( widget->ext_label, true );
    main_tile_align_widgets();
    lv_obj_invalidate( lv_scr_act() );
    return( NULL );
}

void widget_set_indicator( icon_t *widget, icon_indicator_t indicator ) {
    if ( widget == NULL ) {
        return;
    }

    switch( indicator ) {
        case ICON_INDICATOR_OK:      lv_img_set_src( widget->icon_indicator, &info_ok_16px );
                                     break;
        case ICON_INDICATOR_FAIL:    lv_img_set_src( widget->icon_indicator, &info_fail_16px );
                                     break;
        case ICON_INDICATOR_UPDATE:  lv_img_set_src( widget->icon_indicator, &info_update_16px );
                                     break;
        case ICON_INDICATOR_1:       lv_img_set_src( widget->icon_indicator, &info_1_16px );
                                     break;
        case ICON_INDICATOR_2:       lv_img_set_src( widget->icon_indicator, &info_2_16px );
                                     break;
        case ICON_INDICATOR_3:       lv_img_set_src( widget->icon_indicator, &info_3_16px );
                                     break;
        case ICON_INDICATOR_N:       lv_img_set_src( widget->icon_indicator, &info_n_16px );
                                     break;
    }
    lv_obj_align( widget->icon_indicator, widget->icon_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( widget->icon_indicator, false );
    lv_obj_invalidate( lv_scr_act() );
}

void widget_hide_indicator( icon_t *widget ) {
    if ( widget == NULL ) {
        return;
    }

    if ( widget->active == false ) {
        return;
    }

    lv_obj_set_hidden( widget->icon_indicator, true );
    lv_obj_invalidate( lv_scr_act() );
}

void widget_set_icon( icon_t *widget, lv_obj_t *icon ) {
    if ( widget == NULL ) {
        return;
    }

    if ( widget->active == false ) {
        return;
    }

    lv_imgbtn_set_src( widget->icon_img, LV_BTN_STATE_RELEASED, icon);
    lv_imgbtn_set_src( widget->icon_img, LV_BTN_STATE_PRESSED, icon);
    lv_imgbtn_set_src( widget->icon_img, LV_BTN_STATE_CHECKED_RELEASED, icon);
    lv_imgbtn_set_src( widget->icon_img, LV_BTN_STATE_CHECKED_PRESSED, icon);
    lv_obj_reset_style_list( widget->icon_img, LV_OBJ_PART_MAIN );
    lv_obj_align( widget->icon_img , widget->icon_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_invalidate( lv_scr_act() );
}

void widget_set_label( icon_t *widget, const char* text ) {
    if ( widget == NULL ) {
        return;
    }

    if ( widget->active == false ) {
        return;
    }

    lv_label_set_text( widget->label, text );
    lv_obj_align( widget->label , widget->icon_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_label_set_align( widget->label, LV_LABEL_ALIGN_CENTER );
    lv_obj_invalidate( lv_scr_act() );
}

void widget_set_extended_label( icon_t *widget, const char* text ) {
    if ( widget == NULL ) {
        return;
    }

    if ( widget->active == false ) {
        return;
    }

    lv_label_set_text( widget->ext_label, text );
    lv_obj_align( widget->ext_label , widget->label, LV_ALIGN_OUT_TOP_MID, 0, 0 );
    lv_label_set_align( widget->ext_label, LV_LABEL_ALIGN_CENTER );
    lv_obj_invalidate( lv_scr_act() );
}