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
#include "config.h"
#include "widget_factory.h"
#include "widget_styles.h"
#include "mainbar/mainbar.h"
#include "utils/alloc.h"
#include <stdarg.h>

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
#endif

#if defined( BIG_THEME )
    LV_IMG_DECLARE(menu_96px);
    LV_IMG_DECLARE(setup_96px);
    LV_IMG_DECLARE(exit_96px);
    LV_IMG_DECLARE(refresh_96px);
    LV_IMG_DECLARE(trash_96px);
    LV_IMG_DECLARE(add_96px);
    LV_IMG_DECLARE(close_96px);
    LV_IMG_DECLARE(check_96px);
    LV_IMG_DECLARE(download_96px);
    LV_IMG_DECLARE(equal_96px);
    LV_IMG_DECLARE(zoomin_96px);
    LV_IMG_DECLARE(zoomout_96px);
    LV_IMG_DECLARE(left_96px);
    LV_IMG_DECLARE(right_96px);
    LV_IMG_DECLARE(up_96px);
    LV_IMG_DECLARE(down_96px);
    LV_IMG_DECLARE(location_96px);
    LV_IMG_DECLARE(play_96px);
    LV_IMG_DECLARE(stop_96px);
    LV_IMG_DECLARE(eject_96px);
    LV_IMG_DECLARE(reply_96px);

    const lv_img_dsc_t down_icon = down_96px;
    const lv_img_dsc_t up_icon = up_96px;
    const lv_img_dsc_t right_icon = right_96px;
    const lv_img_dsc_t left_icon = left_96px;
    const lv_img_dsc_t zoomout_icon = zoomout_96px;
    const lv_img_dsc_t zoomin_icon = zoomin_96px;
    const lv_img_dsc_t equal_icon = equal_96px;
    const lv_img_dsc_t download_icon = download_96px;
    const lv_img_dsc_t check_icon = check_96px;
    const lv_img_dsc_t close_icon = close_96px;
    const lv_img_dsc_t add_icon = add_96px;
    const lv_img_dsc_t trash_icon = trash_96px;
    const lv_img_dsc_t setup_icon = setup_96px;
    const lv_img_dsc_t exit_icon = exit_96px;
    const lv_img_dsc_t menu_icon = menu_96px;
    const lv_img_dsc_t refresh_icon = refresh_96px;
    const lv_img_dsc_t location_icon = location_96px;
    const lv_img_dsc_t play_icon = play_96px;
    const lv_img_dsc_t stop_icon = stop_96px;
    const lv_img_dsc_t eject_icon = eject_96px;
    const lv_img_dsc_t reply_icon = reply_96px;
#elif defined( MID_THEME )
    LV_IMG_DECLARE(menu_64px);
    LV_IMG_DECLARE(setup_64px);
    LV_IMG_DECLARE(exit_64px);
    LV_IMG_DECLARE(refresh_64px);
    LV_IMG_DECLARE(trash_64px);
    LV_IMG_DECLARE(add_64px);
    LV_IMG_DECLARE(close_64px);
    LV_IMG_DECLARE(check_64px);
    LV_IMG_DECLARE(download_64px);
    LV_IMG_DECLARE(equal_64px);
    LV_IMG_DECLARE(zoomin_64px);
    LV_IMG_DECLARE(zoomout_64px);
    LV_IMG_DECLARE(left_64px);
    LV_IMG_DECLARE(right_64px);
    LV_IMG_DECLARE(up_64px);
    LV_IMG_DECLARE(down_64px);
    LV_IMG_DECLARE(location_64px);
    LV_IMG_DECLARE(play_64px);
    LV_IMG_DECLARE(stop_64px);
    LV_IMG_DECLARE(eject_64px);
    LV_IMG_DECLARE(reply_64px);

    const lv_img_dsc_t down_icon = down_64px;
    const lv_img_dsc_t up_icon = up_64px;
    const lv_img_dsc_t right_icon = right_64px;
    const lv_img_dsc_t left_icon = left_64px;
    const lv_img_dsc_t zoomout_icon = zoomout_64px;
    const lv_img_dsc_t zoomin_icon = zoomin_64px;
    const lv_img_dsc_t equal_icon = equal_64px;
    const lv_img_dsc_t download_icon = download_64px;
    const lv_img_dsc_t check_icon = check_64px;
    const lv_img_dsc_t close_icon = close_64px;
    const lv_img_dsc_t add_icon = add_64px;
    const lv_img_dsc_t trash_icon = trash_64px;
    const lv_img_dsc_t setup_icon = setup_64px;
    const lv_img_dsc_t exit_icon = exit_64px;
    const lv_img_dsc_t menu_icon = menu_64px;
    const lv_img_dsc_t refresh_icon = refresh_64px;
    const lv_img_dsc_t location_icon = location_64px;
    const lv_img_dsc_t play_icon = play_64px;
    const lv_img_dsc_t stop_icon = stop_64px;
    const lv_img_dsc_t eject_icon = eject_64px;
    const lv_img_dsc_t reply_icon = reply_64px;
#else
    LV_IMG_DECLARE(menu_32px);
    LV_IMG_DECLARE(setup_32px);
    LV_IMG_DECLARE(exit_32px);
    LV_IMG_DECLARE(refresh_32px);
    LV_IMG_DECLARE(trash_32px);
    LV_IMG_DECLARE(add_32px);
    LV_IMG_DECLARE(close_32px);
    LV_IMG_DECLARE(check_32px);
    LV_IMG_DECLARE(download_32px);
    LV_IMG_DECLARE(equal_32px);
    LV_IMG_DECLARE(zoomin_32px);
    LV_IMG_DECLARE(zoomout_32px);
    LV_IMG_DECLARE(left_32px);
    LV_IMG_DECLARE(right_32px);
    LV_IMG_DECLARE(up_32px);
    LV_IMG_DECLARE(down_32px);
    LV_IMG_DECLARE(location_32px);
    LV_IMG_DECLARE(play_32px);
    LV_IMG_DECLARE(stop_32px);
    LV_IMG_DECLARE(eject_32px);
    LV_IMG_DECLARE(reply_32px);

    const lv_img_dsc_t down_icon = down_32px;
    const lv_img_dsc_t up_icon = up_32px;
    const lv_img_dsc_t right_icon = right_32px;
    const lv_img_dsc_t left_icon = left_32px;
    const lv_img_dsc_t zoomout_icon = zoomout_32px;
    const lv_img_dsc_t zoomin_icon = zoomin_32px;
    const lv_img_dsc_t equal_icon = equal_32px;
    const lv_img_dsc_t download_icon = download_32px;
    const lv_img_dsc_t check_icon = check_32px;
    const lv_img_dsc_t close_icon = close_32px;
    const lv_img_dsc_t add_icon = add_32px;
    const lv_img_dsc_t trash_icon = trash_32px;
    const lv_img_dsc_t setup_icon = setup_32px;
    const lv_img_dsc_t exit_icon = exit_32px;
    const lv_img_dsc_t menu_icon = menu_32px;
    const lv_img_dsc_t refresh_icon = refresh_32px;
    const lv_img_dsc_t location_icon = location_32px;
    const lv_img_dsc_t play_icon = play_32px;
    const lv_img_dsc_t stop_icon = stop_32px;
    const lv_img_dsc_t eject_icon = eject_32px;
    const lv_img_dsc_t reply_icon = reply_32px;
#endif

#define CLICKABLE_PADDING 6
#define CONTAINER_INNER_PADDING CLICKABLE_PADDING * 2

static bool wf_anim_enabled = true;

static void exit_jump_back_event_cb( lv_obj_t * obj, lv_event_t event );

lv_obj_t * wf_add_container(lv_obj_t *parent_tile, lv_layout_t layout, lv_fit_t hor_fit, lv_fit_t ver_fit, bool add_padding, lv_style_t *style ){
    lv_obj_t *container = lv_cont_create( parent_tile, NULL );
    lv_obj_add_style( container, LV_OBJ_PART_MAIN, style?style:APP_STYLE );
    lv_cont_set_fit2( container, hor_fit, ver_fit );
    lv_obj_set_style_local_pad_all( container, LV_CONT_PART_MAIN , LV_STATE_DEFAULT, add_padding ? CLICKABLE_PADDING : 0 );
    lv_obj_set_style_local_pad_inner( container, LV_CONT_PART_MAIN , LV_STATE_DEFAULT, CONTAINER_INNER_PADDING );
    lv_cont_set_layout( container, layout );

    return container;
}

lv_obj_t * wf_add_tile_container(lv_obj_t *parent_tile, lv_layout_t layout, lv_style_t *style ){
    lv_obj_set_style_local_pad_all(parent_tile, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_t * container = wf_add_container(parent_tile, layout, LV_FIT_PARENT, LV_FIT_TIGHT, true, style?style:APP_STYLE );
    lv_obj_align( container, parent_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    return container;
}

lv_obj_t * wf_add_tile_container(lv_obj_t *parent_tile, lv_layout_t layout){
    lv_obj_set_style_local_pad_all(parent_tile, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_t * container = wf_add_container(parent_tile, layout, LV_FIT_PARENT, LV_FIT_TIGHT, true, NULL );
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

lv_obj_t * wf_add_label(lv_obj_t *parent, char const * text ){
    lv_obj_t *label = lv_label_create( parent, NULL);
    lv_label_set_text( label, text);
    lv_obj_add_style( label, LV_OBJ_PART_MAIN, ws_get_label_style() );
    return label;
}

lv_obj_t * wf_add_label(lv_obj_t *parent, char const * text, lv_style_t *style ){
    lv_obj_t *label = lv_label_create( parent, NULL);
    lv_label_set_text( label, text);
    if ( style )
        lv_obj_add_style( label, LV_OBJ_PART_MAIN, style );
    else
        lv_obj_add_style( label, LV_OBJ_PART_MAIN, ws_get_label_style() );
    return label;
}

void wf_label_printf( lv_obj_t *label, lv_obj_t *base, lv_align_t align, lv_coord_t x, lv_coord_t y, const char *format, ... ) {
    va_list args;
    va_start(args, format);

    char *buffer = NULL;
    int size = vasprintf( &buffer, format, args );
    va_end(args);

    if( size > 0 ) {
        lv_label_set_text( label, buffer );
        lv_obj_align( label, base, align, x, y );
    }

    if( buffer )
        free( buffer );
    
    return;
}

void wf_label_printf( lv_obj_t *label, const char *format, ... ) {
    va_list args;
    va_start(args, format);

    char *buffer = NULL;
    int size = vasprintf( &buffer, format, args );
    va_end(args);

    if( size > 0 ) {
        lv_label_set_text( label, buffer );
    }

    if( buffer )
        free( buffer );
    
    return;
}

lv_obj_t * wf_add_label_container(lv_obj_t *parent, char const * text) {
    lv_obj_t *container = wf_add_container( parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT );
    wf_add_label( container, text);
    return container;
}

lv_obj_t * wf_add_label_container(lv_obj_t *parent, char const * text, lv_style_t *style ) {
    lv_obj_t *container = wf_add_container( parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, style );
    wf_add_label( container, text, style );
    return container;
}

lv_obj_t * wf_add_roller(lv_obj_t *parent, char const * data, lv_roller_mode_t mode, int row_count){
    lv_obj_t *roller = lv_roller_create(parent, NULL);
//    lv_obj_add_style(roller, LV_OBJ_PART_MAIN, ws_get_mainbar_style() );
//    lv_obj_add_style(roller, LV_ROLLER_PART_BG, ws_get_mainbar_style() );
//    lv_obj_add_style(roller, LV_ROLLER_PART_SELECTED, ws_get_mainbar_style() );
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
    #if defined( BIG_THEME )
        lv_obj_set_size( _switch, 70, 35 );
    #elif defined( MID_THEME )
        lv_obj_set_size( _switch, 50, 25 );
    #endif
    lv_obj_set_ext_click_area(_switch, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING);
    return _switch;
}

lv_obj_t * wf_add_switch(lv_obj_t *parent, bool on, lv_event_cb_t event_cb ){
    lv_obj_t *_switch = lv_switch_create( parent, NULL );
    //TODO: must it be here?
    lv_obj_add_protect( _switch, LV_PROTECT_CLICK_FOCUS);

    if (on) {
        lv_switch_on( _switch, LV_ANIM_OFF );
    } else {
        lv_switch_off( _switch, LV_ANIM_OFF );
    }

    lv_obj_add_style( _switch, LV_SWITCH_PART_INDIC, ws_get_switch_style() );
    #if defined( BIG_THEME )
        lv_obj_set_size( _switch, 70, 35 );
    #elif defined( MID_THEME )
        lv_obj_set_size( _switch, 50, 25 );
    #endif
    lv_obj_set_ext_click_area(_switch, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING);

    lv_obj_set_event_cb( _switch, event_cb );
    return _switch;
}

lv_obj_t * wf_add_labeled_switch(lv_obj_t *parent, char const * text, lv_obj_t ** ret_switch_obj){
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT );
    wf_add_label( container, text);
    *ret_switch_obj = wf_add_switch( container );
    return container;
}

lv_obj_t * wf_add_labeled_switch(lv_obj_t *parent, char const * text, lv_obj_t ** ret_switch_obj, bool state, lv_event_cb_t event_cb, lv_style_t *style  ){
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false, style );
    wf_add_label( container, text, style );
    *ret_switch_obj = wf_add_switch( container, state );
    lv_obj_set_event_cb( *ret_switch_obj, event_cb );
    return container;
}

lv_obj_t * wf_add_labeled_switch(lv_obj_t *parent, char const * text, lv_obj_t ** ret_switch_obj, bool state, lv_event_cb_t event_cb ){
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT);
    wf_add_label( container, text);
    *ret_switch_obj = wf_add_switch( container, state );
    lv_obj_set_event_cb( *ret_switch_obj, event_cb );
    return container;
}

lv_obj_t * wf_add_list(lv_obj_t *parent, const char* _options ){
    lv_obj_t *_list = lv_dropdown_create( parent, NULL );
    lv_dropdown_set_options( _list, _options );
    lv_obj_add_protect( _list, LV_PROTECT_CLICK_FOCUS);
    return _list;
}

lv_obj_t * wf_add_list(lv_obj_t *parent, const char* _options, lv_style_t *style ){
    lv_obj_t *_list = lv_dropdown_create( parent, NULL );
    if ( style ) {
        lv_obj_add_style( _list, LV_DROPDOWN_PART_LIST, style );
        lv_obj_add_style( _list, LV_DROPDOWN_PART_MAIN, style );
        lv_obj_add_style( _list, LV_OBJ_PART_MAIN, style );
        lv_obj_add_style( _list, LV_STATE_DEFAULT, style );
        lv_obj_add_style( _list, LV_STATE_CHECKED, style );
        lv_obj_add_style( _list, LV_STATE_DISABLED, style );
        lv_obj_add_style( _list, LV_STATE_EDITED, style );
        lv_obj_add_style( _list, LV_STATE_FOCUSED, style );
        lv_obj_add_style( _list, LV_STATE_HOVERED, style );
        lv_obj_add_style( _list, LV_STATE_PRESSED, style );
    }
    lv_obj_add_protect( _list, LV_PROTECT_CLICK_FOCUS);
    lv_dropdown_set_options( _list, _options );
    return _list;
}
/**
 * 
 */
lv_obj_t * wf_add_labeled_list(lv_obj_t *parent, char const * text, lv_obj_t ** ret_list_obj, const char *options, lv_event_cb_t event_cb ) {
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false );
    wf_add_label( container, text );
    *ret_list_obj = wf_add_list( container, options );
    lv_obj_set_event_cb( *ret_list_obj, event_cb );
    return container;
}
/**
 * 
 */
lv_obj_t * wf_add_labeled_list(lv_obj_t *parent, char const * text, lv_obj_t ** ret_list_obj, const char *options, lv_event_cb_t event_cb, lv_style_t *style ) {
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false, style );
    wf_add_label( container, text, style );
    if ( style == ws_get_setup_tile_style() ) {
        *ret_list_obj = wf_add_list( container, options, ws_get_setup_dropdown_style() );
    }
    else {
        *ret_list_obj = wf_add_list( container, options );
    }
    lv_obj_set_event_cb( *ret_list_obj, event_cb );
    return container;
}
/**
 * 
 */
lv_obj_t * wf_add_labeled_list(lv_obj_t *parent, char const * text, lv_obj_t ** ret_list_obj, const char *options, lv_event_cb_t event_cb, lv_style_t *style, lv_style_t *dropdown_style ) {
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_PRETTY_MID, LV_FIT_PARENT, LV_FIT_TIGHT, style );
    wf_add_label( container, text, style );
    *ret_list_obj = wf_add_list( container, options, dropdown_style );
    lv_obj_set_event_cb( *ret_list_obj, event_cb );
    return container;
}
/**
 * 
 */
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

    if( event_cb )
        lv_obj_set_event_cb( button, event_cb );

    return button;
}

void wf_enable_anim( bool enable ) {
    wf_anim_enabled = enable;
}

void wf_image_button_fade_out_state_2( _lv_anim_t *anim ) {
    lv_obj_set_hidden( lv_obj_get_parent( (lv_obj_t*)anim->var ), true );
    lv_anim_del( anim->var, (lv_anim_exec_xcb_t)lv_img_set_zoom );
}

void wf_image_button_fade_out( lv_obj_t *button, uint32_t duration, uint32_t delay ) {
    if( !wf_anim_enabled )
        return;

    lv_anim_t wf_btn_icon_anim;

    lv_anim_init( &wf_btn_icon_anim );
	lv_anim_set_exec_cb( &wf_btn_icon_anim, (lv_anim_exec_xcb_t)lv_img_set_zoom );
	lv_anim_set_time( &wf_btn_icon_anim, duration );
    lv_anim_set_ready_cb( &wf_btn_icon_anim, wf_image_button_fade_out_state_2 );

    lv_anim_set_var( &wf_btn_icon_anim, lv_obj_get_child( button, NULL ) );
    lv_anim_set_values( &wf_btn_icon_anim, 256, 1 );
    lv_anim_set_delay( &wf_btn_icon_anim, delay );
    lv_anim_start( &wf_btn_icon_anim );
}

void wf_image_button_fade_in_state_3( _lv_anim_t *anim ) {
    lv_anim_del( anim->var, (lv_anim_exec_xcb_t)lv_img_set_zoom );
}

void wf_image_button_fade_in_state_2( _lv_anim_t *anim ) {

    lv_anim_set_ready_cb( anim, wf_image_button_fade_in_state_3 );
	lv_anim_set_time( anim, 100 );
    lv_anim_set_values( anim, 300, 256 );
    lv_anim_set_delay( anim, 0 );
    lv_anim_start( anim );
}

void wf_image_button_fade_in( lv_obj_t *button, uint32_t duration, uint32_t delay ) {
    if( !wf_anim_enabled )
        return;

    lv_anim_t wf_btn_icon_anim;

    lv_anim_init( &wf_btn_icon_anim );
	lv_anim_set_exec_cb( &wf_btn_icon_anim, (lv_anim_exec_xcb_t)lv_img_set_zoom );
	lv_anim_set_time( &wf_btn_icon_anim, duration );
    lv_anim_set_ready_cb( &wf_btn_icon_anim, wf_image_button_fade_in_state_2 );

    lv_anim_set_var( &wf_btn_icon_anim, lv_obj_get_child( button, NULL ) );
    lv_anim_set_values( &wf_btn_icon_anim, 1, 300 );
    lv_anim_set_delay( &wf_btn_icon_anim, delay );
    lv_anim_start( &wf_btn_icon_anim );

    lv_obj_set_hidden( button, false );
}

void wf_image_button_set_style( lv_obj_t *button, lv_style_t *style ) {
    lv_obj_reset_style_list( lv_obj_get_child( button, NULL ), LV_OBJ_PART_MAIN );
    lv_obj_add_style( lv_obj_get_child( button, NULL ), LV_OBJ_PART_MAIN, style );
}

lv_obj_t * wf_add_image_button_old(lv_obj_t *parent, lv_img_dsc_t const &image, lv_event_cb_t event_cb, lv_style_t *style){
    lv_obj_t * button = lv_imgbtn_create( parent, NULL );
    lv_imgbtn_set_src( button, LV_BTN_STATE_RELEASED, &image );
    lv_imgbtn_set_src( button, LV_BTN_STATE_PRESSED, &image );
    lv_imgbtn_set_src( button, LV_BTN_STATE_CHECKED_RELEASED, &image );
    lv_imgbtn_set_src( button, LV_BTN_STATE_CHECKED_PRESSED, &image );

    if (!style)
        style = ws_get_img_button_style();

    lv_obj_add_style( button, LV_OBJ_PART_MAIN, style );
    lv_obj_set_ext_click_area(button, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING, CLICKABLE_PADDING);

    if ( event_cb != NULL )
        lv_obj_set_event_cb( button, event_cb );

    return button;
}
    
/**
 * 
 */
lv_obj_t * wf_add_image_button(lv_obj_t *parent, lv_img_dsc_t const &image, lv_event_cb_t event_cb, lv_style_t *style){
    lv_obj_t *button = lv_btn_create( parent, NULL );
    lv_obj_set_width( button, image.header.w + THEME_PADDING * 2 );
    lv_obj_set_height( button, image.header.h + THEME_PADDING * 2 );
    lv_obj_add_protect( button, LV_PROTECT_CLICK_FOCUS );
    lv_obj_add_style( button, LV_BTN_PART_MAIN, ws_get_img_button_style() );

    lv_obj_t *button_img = lv_img_create( button, NULL );
    lv_img_set_src( button_img, &image );
    lv_obj_align( button_img, button, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_click( button_img, false );

    if (!style) {
        style = ws_get_img_button_style();
    }

    lv_obj_add_style( button_img, LV_OBJ_PART_MAIN, style );

    if (event_cb != NULL) {
        lv_obj_set_event_cb( button, event_cb );
    }
    return button;
}
/**
 * 
 */
lv_obj_t * wf_add_exit_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
        return wf_add_image_button(parent, exit_icon, event_cb, style?style:SYSTEM_ICON_STYLE);
}
/**
 * 
 */
lv_obj_t * wf_add_exit_button(lv_obj_t *parent, lv_style_t *style){
    return wf_add_exit_button(parent, exit_jump_back_event_cb, style?style:SYSTEM_ICON_STYLE);
}
lv_img_dsc_t const &wf_get_exit_img( void ) { return( exit_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_setup_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, setup_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_setup_img( void ) { return( setup_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_add_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, add_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_add_img( void ) { return( add_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_trash_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, trash_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_trash_img( void ) { return( trash_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_menu_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, menu_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_menu_img( void ) { return( menu_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_refresh_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, refresh_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_refresh_img( void ) { return( refresh_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_close_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, close_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_close_img( void ) { return( close_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_check_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, check_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_check_img( void ) { return( check_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_download_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, download_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_download_img( void ) { return( download_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_equal_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, equal_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_equal_img( void ) { return( equal_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_zoom_in_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, zoomin_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_zoom_in_img( void ) { return( zoomin_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_zoom_out_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, zoomout_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_zoom_out_img( void ) { return( zoomout_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_right_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, right_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_right_img( void ) { return( right_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_left_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, left_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_left_img( void ) { return( left_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_up_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, up_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_up_img( void ) { return( up_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_down_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, down_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_down_img( void ) { return( down_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_location_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, location_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_location_img( void ) { return( location_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_play_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, play_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_play_img( void ) { return( play_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_stop_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, stop_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_stop_img( void ) { return( stop_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_eject_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, eject_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_eject_img( void ) { return( eject_icon ); }
/**
 * 
 */
lv_obj_t * wf_add_reply_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style){
    return wf_add_image_button(parent, reply_icon, event_cb, style?style:SYSTEM_ICON_STYLE );
}
lv_img_dsc_t const &wf_get_reply_img( void ) { return( reply_icon ); }

lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_obj_t ** ret_back_btn, lv_style_t *style ) {
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_ROW_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false, style?style:SETUP_STYLE );
    lv_obj_t *exit_btn = wf_add_exit_button(container, NULL, style?style:SYSTEM_ICON_STYLE );
    if (title != NULL && strlen(title) > 0) {
        wf_add_label(container, title, style?style:ws_get_setup_header_tile_style() );
    }
    *ret_back_btn = exit_btn;
    return container;
}

lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_obj_t ** ret_back_btn) {
    lv_obj_t *container = wf_add_container(parent, LV_LAYOUT_ROW_MID, LV_FIT_PARENT, LV_FIT_TIGHT, false, SETUP_STYLE );
    lv_obj_t *exit_btn = wf_add_exit_button(container, NULL, SYSTEM_ICON_STYLE );
    if (title != NULL && strlen(title) > 0) {
        wf_add_label(container, title, ws_get_setup_header_tile_style() );
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

lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title){
    lv_obj_t *cont = wf_add_settings_header( parent, title, exit_jump_back_event_cb );
    return cont;
}

static void exit_jump_back_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            mainbar_jump_back();
            break;
    }
}

lv_obj_t *wf_get_settings_header_title(lv_obj_t *parent) {
    lv_obj_t *title = NULL;
    lv_obj_t *child = lv_obj_get_child(parent, NULL);
    while(child) {
        lv_obj_type_t buf;
        lv_obj_get_type(child, &buf);
        if (!strcmp(buf.type[0], "lv_label")) {
            log_d("header_title found");
            /* found */
            title = child;
            /* Break the loop */
            child = NULL;
        } else {
            log_d("header_title not found");
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
