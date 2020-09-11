/****************************************************************************
 *   Copyright  2020  Jakub Vesely
 *   Email: jakub_vesely@seznam.cz
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

#include "widget_factory.h"
#include "widget_styles.h"

LV_IMG_DECLARE(exit_32px);

lv_obj_t * wf_add_container(
    lv_obj_t *parent, lv_obj_t * alignment_base, lv_align_t alignment, int offset_x, int offset_y, int width, int height
){
    lv_obj_t *alarm_clock_roller_cont = lv_obj_create( parent, NULL );
    if (width != -1 && height != -1){
        lv_obj_set_size( alarm_clock_roller_cont, width , height);
    }
    lv_obj_add_style( alarm_clock_roller_cont, LV_OBJ_PART_MAIN, ws_get_container_style() );
    lv_obj_align( alarm_clock_roller_cont, alignment_base, alignment, offset_x, offset_y );
    return alarm_clock_roller_cont;
}

lv_obj_t * wf_add_label(
    lv_obj_t *parent, char const * text, lv_obj_t * alignment_base, lv_align_t alignment, int offset_x, int offset_y
){
    lv_obj_t *label = lv_label_create( parent, NULL);
    lv_obj_add_style( label, LV_OBJ_PART_MAIN, ws_get_label_style() );
    lv_label_set_text( label, text);
    lv_obj_align( label, alignment_base, alignment, offset_x, offset_y );
    return label;
}

lv_obj_t * wf_add_roller(lv_obj_t *parent, char const * data, lv_roller_mode_t mode, lv_obj_t * alignment_base, lv_align_t alignment){
    lv_obj_t *roller = lv_roller_create(parent, NULL);
    lv_obj_add_style(roller, LV_ROLLER_PART_BG, ws_get_roller_bg_style() );
    lv_obj_add_style(roller, LV_ROLLER_PART_SELECTED, ws_get_roller_part_selected_style() );
    lv_roller_set_auto_fit(roller, false);
    lv_roller_set_align(roller, LV_LABEL_ALIGN_CENTER);
    lv_roller_set_visible_row_count(roller, 3);
    lv_roller_set_options(roller, data, mode);
    lv_obj_set_width(roller, lv_obj_get_width_grid(parent, 2, 1));
    lv_obj_align( roller, alignment_base, alignment, 0, 0 );
    return roller;
}

lv_obj_t * wf_add_labeled_switch(
    lv_obj_t *parent, char const * text, lv_obj_t * alignment_base, lv_align_t alignment, int offset_x, int offset_y, lv_obj_t ** ret_switch_obj
){
    lv_obj_t *container = lv_obj_create( parent, NULL );
    lv_obj_set_size( container, lv_disp_get_hor_res( NULL ) , 32 );
    lv_obj_add_style( container, LV_OBJ_PART_MAIN, ws_get_container_style() );
    lv_obj_align( container, alignment_base, alignment, 0, offset_y);

    wf_add_label( parent, text, container, LV_ALIGN_IN_LEFT_MID, offset_x, 0 );

    lv_obj_t *_switch = lv_switch_create( parent, NULL );
    lv_obj_add_protect( _switch, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( _switch, LV_SWITCH_PART_INDIC, ws_get_switch_style() );
    lv_switch_off( _switch, LV_ANIM_OFF );
    lv_obj_align( _switch, container, LV_ALIGN_IN_RIGHT_MID, -offset_x, 0 );

    *ret_switch_obj = _switch;
    return container;
}

lv_obj_t * wf_add_image_button(
    lv_obj_t *parent, lv_img_dsc_t const &image, lv_obj_t * alignment_base, lv_align_t alignment, int offset_x, int offset_y, lv_event_cb_t event_cb
){
    lv_obj_t * button = lv_imgbtn_create( parent, NULL );
    lv_imgbtn_set_src( button, LV_BTN_STATE_RELEASED, &image );
    lv_imgbtn_set_src( button, LV_BTN_STATE_PRESSED, &image );
    lv_imgbtn_set_src( button, LV_BTN_STATE_CHECKED_RELEASED, &image );
    lv_imgbtn_set_src( button, LV_BTN_STATE_CHECKED_PRESSED, &image );
    lv_obj_add_style( button, LV_IMGBTN_PART_MAIN, ws_get_img_button_style() );
    lv_obj_align( button, alignment_base, alignment, offset_x, offset_y );
    if (event_cb != NULL) lv_obj_set_event_cb( button, event_cb );
    return button;
}

lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_obj_t ** ret_back_btn){
    lv_obj_t *container = wf_add_container(parent, parent, LV_ALIGN_IN_TOP_LEFT, 0, 10, lv_disp_get_hor_res( NULL ), 32);
    lv_obj_t *exit_btn = wf_add_image_button(parent, exit_32px, container, LV_ALIGN_IN_LEFT_MID, 10, 0, NULL);
    wf_add_label(parent, title, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    *ret_back_btn = exit_btn;
    return container;
}

lv_obj_t * wf_add_image(
    lv_obj_t *parent, lv_img_dsc_t const &image, lv_obj_t * alignment_base, lv_align_t alignment, int offset_x, int offset_y
){
    lv_obj_t * img_obj = lv_img_create( parent, NULL );
    lv_img_set_src( img_obj, &image );
    lv_obj_align( img_obj, alignment_base, alignment, offset_x, offset_y );
    return img_obj;
}
