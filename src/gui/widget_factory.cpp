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
LV_IMG_DECLARE(setup_32px);
#define CLICKABLE_PADDING 6
#define CONTAINER_INNER_PADDING CLICKABLE_PADDING * 2

lv_obj_t * wf_add_container(lv_obj_t *parent_tile, lv_layout_t layout, lv_fit_t hor_fit, lv_fit_t ver_fit, bool add_padding){
    lv_obj_t *container = lv_cont_create( parent_tile, NULL );
     lv_obj_add_style( container, LV_OBJ_PART_MAIN, ws_get_container_style() );
    lv_cont_set_fit2(container, hor_fit, ver_fit );

    lv_obj_set_style_local_pad_all( container, LV_CONT_PART_MAIN , LV_STATE_DEFAULT, add_padding ? CLICKABLE_PADDING : 0);
    lv_obj_set_style_local_pad_inner( container, LV_CONT_PART_MAIN , LV_STATE_DEFAULT, CONTAINER_INNER_PADDING);
    lv_cont_set_layout(container, layout);

    return container;
}

lv_obj_t * wf_add_tile_container(lv_obj_t *parent_tile, lv_layout_t layout){
    lv_obj_set_style_local_pad_all(parent_tile, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_t * container = wf_add_container(parent_tile, layout, LV_FIT_PARENT, LV_FIT_TIGHT, true);
    lv_obj_align( container, parent_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    return container;
}
/**
 * unfortunately it is still not possible to add a "stretch gap"
 * according this discussion: https://forum.lvgl.io/t/how-can-i-have-some-layout-children-stretch-to-fill-remaining-space/2967
 * flexbox will be available in lvgl v8
 */
lv_obj_t * wf_add_tile_footer_container(lv_obj_t *tile, lv_layout_t layout){
    lv_obj_t *container = wf_add_container(tile, layout, LV_FIT_PARENT, LV_FIT_TIGHT, true);
    lv_obj_set_style_local_pad_all( container, LV_CONT_PART_MAIN , LV_STATE_DEFAULT, CLICKABLE_PADDING);
    lv_obj_set_style_local_pad_inner( container, LV_CONT_PART_MAIN , LV_STATE_DEFAULT, CONTAINER_INNER_PADDING);
    lv_obj_align( container, tile, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    return container;
}

lv_obj_t * wf_add_label(lv_obj_t *parent, char const * text){
    lv_obj_t *label = lv_label_create( parent, NULL);
    lv_label_set_text( label, text);
    lv_obj_add_style( label, LV_OBJ_PART_MAIN, ws_get_label_style() );
    return label;
}

lv_obj_t * wf_add_label_container(lv_obj_t *parent, char const * text) {
    lv_obj_t *container = wf_add_container( parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT );
    wf_add_label( container, text);
    return container;
}

lv_obj_t * wf_add_roller(lv_obj_t *parent, char const * data, lv_roller_mode_t mode, int row_count){
    lv_obj_t *roller = lv_roller_create(parent, NULL);
    lv_obj_add_style(roller, LV_ROLLER_PART_BG, ws_get_roller_bg_style() );
    lv_obj_add_style(roller, LV_ROLLER_PART_SELECTED, ws_get_roller_part_selected_style() );
    lv_roller_set_auto_fit(roller, false);
    lv_roller_set_align(roller, LV_LABEL_ALIGN_CENTER);
    lv_roller_set_visible_row_count(roller, row_count);
    lv_roller_set_options(roller, data, mode);
    

    lv_obj_set_ext_click_area(roller, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING);
    return roller;

}

lv_obj_t * wf_add_switch(lv_obj_t *parent, bool on){
    lv_obj_t *_switch = lv_switch_create( parent, NULL );
    //TODO: must it be here?
    lv_obj_add_protect( _switch, LV_PROTECT_CLICK_FOCUS);

    if (on) {
        lv_switch_on( _switch, LV_ANIM_OFF );
    } else {
        lv_switch_off( _switch, LV_ANIM_OFF );
    }

    lv_obj_add_style( _switch, LV_SWITCH_PART_INDIC, ws_get_switch_style() );

    lv_obj_set_ext_click_area(_switch, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING);
    return _switch;
}

lv_obj_t * wf_add_labeled_switch(lv_obj_t *parent, char const * text, lv_obj_t ** ret_switch_obj){
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT);
    wf_add_label( container, text);
    *ret_switch_obj = wf_add_switch( container );
    return container;
}

lv_obj_t * wf_add_labeled_switch(lv_obj_t *parent, char const * text, lv_obj_t ** ret_switch_obj, bool state, lv_event_cb_t event_cb ){
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT);
    wf_add_label( container, text);
    *ret_switch_obj = wf_add_switch( container, state );
    lv_obj_set_event_cb( *ret_switch_obj, event_cb );
    return container;
}

lv_obj_t * wf_add_button(lv_obj_t *parent, char const * label, int width, int height, lv_event_cb_t event_cb){
    lv_obj_t * button = lv_btn_create(parent, NULL);
    lv_obj_t * label_obj = lv_label_create(button, NULL);
    lv_label_set_text(label_obj, label);

    lv_btn_set_checkable(button, true);
    lv_btn_set_state(button, LV_BTN_STATE_RELEASED);
    lv_btn_toggle(button);
    lv_obj_add_style( button, LV_OBJ_PART_MAIN, ws_get_button_style() );

    if (width != -1){
        lv_obj_set_width(button, width);
    }
    if (height != -1){
        lv_obj_set_height(button, height);
    }
    return button;
}

lv_obj_t * wf_add_image_button(lv_obj_t *parent, lv_img_dsc_t const &image, lv_event_cb_t event_cb, lv_style_t *style){
    lv_obj_t * button = lv_imgbtn_create( parent, NULL );
    lv_imgbtn_set_src( button, LV_BTN_STATE_RELEASED, &image );
    lv_imgbtn_set_src( button, LV_BTN_STATE_PRESSED, &image );
    lv_imgbtn_set_src( button, LV_BTN_STATE_CHECKED_RELEASED, &image );
    lv_imgbtn_set_src( button, LV_BTN_STATE_CHECKED_PRESSED, &image );

    if (!style) {
        style = ws_get_img_button_style();
    }
    lv_obj_add_style( button, LV_IMGBTN_PART_MAIN, style );
    lv_obj_set_ext_click_area(button, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING);

    if (event_cb != NULL) {
        lv_obj_set_event_cb( button, event_cb );
    }
    return button;
}

lv_obj_t * wf_add_exit_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, exit_32px, event_cb, style);
}

lv_obj_t * wf_add_setup_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, setup_32px, event_cb, style);
}

lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_obj_t ** ret_back_btn){
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_ROW_MID, LV_FIT_PARENT, LV_FIT_TIGHT);
    lv_obj_t *exit_btn = wf_add_exit_button(container, NULL);
    if (title != NULL && strlen(title) > 0) {
        wf_add_label(container, title);
    }
    *ret_back_btn = exit_btn;
    return container;
}

lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_event_cb_t event_cb){
    lv_obj_t *exit_btn;
    lv_obj_t *cont = wf_add_settings_header( parent, title, &exit_btn );
    lv_obj_set_event_cb( exit_btn, event_cb );
    return cont;
}

lv_obj_t *wf_get_settings_header_title(lv_obj_t *parent) {
    lv_obj_t *title = NULL;
    lv_obj_t *child = lv_obj_get_child(parent, NULL);
    while(child) {
        lv_obj_type_t buf;
        lv_obj_get_type(child, &buf);
        if (!strcmp(buf.type[0], "lv_label")) {
            log_i("header_title found");
            /* found */
            title = child;
            /* Break the loop */
            child = NULL;
        } else {
            log_i("header_title not found");
            /* continue the loop */
            child = lv_obj_get_child(parent, child);
        }
    }
    return title;
}

lv_obj_t * wf_add_image(lv_obj_t *parent, lv_img_dsc_t const &image){
    lv_obj_t * img_obj = lv_img_create( parent, NULL );
    lv_img_set_src( img_obj, &image );
    return img_obj;
}
