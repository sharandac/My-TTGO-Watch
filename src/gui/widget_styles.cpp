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

#include "widget_styles.h"
#include "hardware/callback.h"

callback_t *styles_callback = NULL;

static bool styles_defined = false;

static lv_style_t background_style;

static lv_style_t mainbar_style;
static lv_style_t mainbar_dropdown_style;

static lv_style_t app_style;
static lv_style_t app_dropdown_style;
static lv_style_t app_opa_style;

static lv_style_t setup_tile_style;
static lv_style_t setup_header_tile_style;
static lv_style_t setup_dropdown_style;

static lv_style_t system_icon_style;
static lv_style_t app_icon_style;
static lv_style_t system_icon_label_style;
static lv_style_t app_icon_label_style;

static lv_style_t button_style;
static lv_style_t img_button_style;
static lv_style_t label_style;
static lv_style_t switch_style;
static lv_style_t roller_bg_style;
static lv_style_t roller_part_selected_style;
static lv_style_t popup_style;
static lv_style_t slider_style;

// Arc has two parts
static lv_style_t arc_bg_style;
static lv_style_t arc_style;

LV_FONT_DECLARE(Ubuntu_12px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_48px);

#if defined( BIG_THEME )
    lv_font_t *mainbar_font = &lv_font_montserrat_22;
    lv_font_t *app_font = &lv_font_montserrat_22;
    lv_font_t *setup_font = &lv_font_montserrat_22;
    lv_font_t *setup_header_font = &lv_font_montserrat_32;
    lv_font_t *app_icon_label_font = &lv_font_montserrat_22;
    lv_font_t *system_icon_label_font = &lv_font_montserrat_22;
#elif defined( MID_THEME )
    lv_font_t *mainbar_font = &lv_font_montserrat_14;
    lv_font_t *app_font = &lv_font_montserrat_14;
    lv_font_t *setup_font = &lv_font_montserrat_14;
    lv_font_t *setup_header_font = &lv_font_montserrat_14;
    lv_font_t *app_icon_label_font = &lv_font_montserrat_14;
    lv_font_t *system_icon_label_font = &lv_font_montserrat_14;
#else
    lv_font_t *mainbar_font = &lv_font_montserrat_14;
    lv_font_t *app_font = &lv_font_montserrat_14;
    lv_font_t *setup_font = &lv_font_montserrat_14;
    lv_font_t *setup_header_font = &lv_font_montserrat_14;
    lv_font_t *app_icon_label_font = &lv_font_montserrat_14;
    lv_font_t *system_icon_label_font = &lv_font_montserrat_14;
#endif

bool styles_send_event_cb( EventBits_t event, void *arg );

bool styles_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( styles_callback == NULL ) {
        styles_callback = callback_init( "styles" );
    }
    return( callback_register( styles_callback, event, callback_func, id ) );
}

bool styles_send_event_cb( EventBits_t event, void *arg ) {
    return( callback_send( styles_callback, event, arg ) );
}

void widget_style_theme_set( int theme ) {
    switch( theme ) {
        case( 0 ):      /** background **/
                        lv_style_set_bg_color( &background_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        /** maibbar colors **/
                        lv_style_set_bg_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &mainbar_style, LV_OBJ_PART_MAIN, mainbar_font );
                        /** mainbar dropdown colors **/
                        lv_style_set_bg_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_bg_opa( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, 2 );
                        lv_style_set_text_font( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, mainbar_font );
                        lv_style_set_text_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** app colors **/
                        lv_style_set_bg_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &app_style, LV_OBJ_PART_MAIN, app_font );
                        /** app ops style color **/
                        lv_style_set_bg_color( &app_opa_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &app_opa_style, LV_OBJ_PART_MAIN, app_font );
                        /** app dropdown colors **/
                        lv_style_set_bg_color( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_bg_opa( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &app_dropdown_style, LV_OBJ_PART_MAIN, 1 );
                        lv_style_set_text_font( &app_dropdown_style, LV_OBJ_PART_MAIN, app_font );
                        lv_style_set_text_color( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** setup colors **/
                        lv_style_set_bg_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_image_recolor( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &setup_tile_style, LV_OBJ_PART_MAIN, setup_font );
                        /** setup dropdown style **/
                        lv_style_set_bg_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_bg_opa( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &setup_dropdown_style, LV_OBJ_PART_MAIN, 2 );
                        lv_style_set_border_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** setup header style **/
                        lv_style_set_bg_color( &setup_header_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_color( &setup_header_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &setup_header_tile_style, LV_OBJ_PART_MAIN, setup_header_font );
                        /** system/app icon/label style **/
                        lv_style_set_image_recolor( &system_icon_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_color( &system_icon_label_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &system_icon_label_style, LV_OBJ_PART_MAIN, system_icon_label_font );
                        lv_style_set_bg_opa( &app_icon_style, LV_OBJ_PART_MAIN, LV_OPA_20 );
                        lv_style_set_text_color( &app_icon_label_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &app_icon_label_style, LV_OBJ_PART_MAIN, app_icon_label_font );
                        /** button/switch/slider  style **/
                        lv_style_set_radius( &button_style, LV_STATE_DEFAULT, 0 );
                        lv_style_set_border_color( &button_style, LV_STATE_DEFAULT, LV_COLOR_BLACK );
                        lv_style_set_bg_color( &switch_style, LV_STATE_CHECKED, LV_COLOR_BLACK );
                        lv_style_set_bg_color( &slider_style, LV_STATE_DEFAULT, LV_COLOR_BLACK );
                        /** trigger style change event **/
                        styles_send_event_cb( STYLE_CHANGE, (void*)NULL );
                        styles_send_event_cb( STYLE_DARKMODE, (void*)NULL );
                        break;
        case( 1 ):      /** background **/
                        lv_style_set_bg_color( &background_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** maibbar colors **/
                        lv_style_set_bg_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &mainbar_style, LV_OBJ_PART_MAIN, mainbar_font );
                        /** mainbar dropdown colors **/
                        lv_style_set_bg_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_bg_opa( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, 2 );
                        lv_style_set_text_font( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, mainbar_font );
                        lv_style_set_text_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** app colors **/
                        lv_style_set_bg_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &app_style, LV_OBJ_PART_MAIN, app_font );
                        /** app ops style color **/
                        lv_style_set_bg_color( &app_opa_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &app_opa_style, LV_OBJ_PART_MAIN, app_font );
                        /** app dropdown colors **/
                        lv_style_set_bg_color( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_bg_opa( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &app_dropdown_style, LV_OBJ_PART_MAIN, 1 );
                        lv_style_set_text_font( &app_dropdown_style, LV_OBJ_PART_MAIN, app_font );
                        lv_style_set_text_color( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** setup colors **/
                        lv_style_set_bg_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_image_recolor( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &setup_tile_style, LV_OBJ_PART_MAIN, setup_font );
                        /** setup dropdown style **/
                        lv_style_set_bg_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_bg_opa( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &setup_dropdown_style, LV_OBJ_PART_MAIN, 2 );
                        lv_style_set_border_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** setup header style **/
                        lv_style_set_bg_color( &setup_header_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_color( &setup_header_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &setup_header_tile_style, LV_OBJ_PART_MAIN, setup_header_font );
                        /** system/app icon/label style **/
                        lv_style_set_image_recolor( &system_icon_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_color( &system_icon_label_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &system_icon_label_style, LV_OBJ_PART_MAIN, system_icon_label_font );
                        lv_style_set_bg_opa( &app_icon_style, LV_OBJ_PART_MAIN, LV_OPA_20 );
                        lv_style_set_text_color( &app_icon_label_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &app_icon_label_style, LV_OBJ_PART_MAIN, app_icon_label_font );
                        /** button/switch/slider  style **/
                        lv_style_set_radius( &button_style, LV_STATE_DEFAULT, 0 );
                        lv_style_set_border_color( &button_style, LV_STATE_DEFAULT, LV_COLOR_GRAY );
                        lv_style_set_bg_color( &switch_style, LV_STATE_CHECKED, LV_COLOR_GRAY );
                        lv_style_set_bg_color( &slider_style, LV_STATE_DEFAULT, LV_COLOR_GRAY );
                        /** trigger style change event **/
                        styles_send_event_cb( STYLE_CHANGE, (void*)NULL );
                        styles_send_event_cb( STYLE_DARKMODE, (void*)NULL );
                        break;
        case( 3 ):      lv_style_set_bg_color( &background_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** maibbar colors **/
                        lv_style_set_bg_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &mainbar_style, LV_OBJ_PART_MAIN, mainbar_font );
                        /** mainbar dropdown colors **/
                        lv_style_set_bg_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
                        lv_style_set_bg_opa( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, 0 );
                        lv_style_set_text_font( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, mainbar_font );
                        lv_style_set_text_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** app colors **/                        
                        lv_style_set_bg_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &app_style, LV_OBJ_PART_MAIN, app_font );
                        /** app ops style color **/
                        lv_style_set_bg_color( &app_opa_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &app_opa_style, LV_OBJ_PART_MAIN, app_font );
                        /** app dropdown colors **/
                        lv_style_set_bg_color( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
                        lv_style_set_bg_opa( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &app_dropdown_style, LV_OBJ_PART_MAIN, 0 );
                        lv_style_set_text_font( &app_dropdown_style, LV_OBJ_PART_MAIN, app_font );
                        lv_style_set_text_color( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** setup colors **/
                        lv_style_set_bg_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY );
                        lv_style_set_text_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_image_recolor( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &setup_tile_style, LV_OBJ_PART_MAIN, setup_font );
                        /** setup dropdown style **/
                        lv_style_set_bg_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
                        lv_style_set_bg_opa( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &setup_dropdown_style, LV_OBJ_PART_MAIN, 0 );
                        lv_style_set_border_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
                        lv_style_set_text_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** setup header style **/
                        lv_style_set_bg_color( &setup_header_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY );
                        lv_style_set_text_color( &setup_header_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &setup_header_tile_style, LV_OBJ_PART_MAIN, setup_header_font );
                        /** system/app icon/label style **/
                        lv_style_set_image_recolor( &system_icon_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_color( &system_icon_label_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &system_icon_label_style, LV_OBJ_PART_MAIN, system_icon_label_font );
                        lv_style_set_bg_opa( &app_icon_style, LV_OBJ_PART_MAIN, LV_OPA_40 );
                        lv_style_set_text_color( &app_icon_label_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &app_icon_label_style, LV_OBJ_PART_MAIN, app_icon_label_font );
                        /** button/switch/slider  style **/
                        lv_style_set_radius( &button_style, LV_STATE_DEFAULT, 3 );
                        lv_style_set_border_color( &button_style, LV_STATE_DEFAULT, LV_COLOR_BLACK );
                        lv_style_set_bg_color( &switch_style, LV_STATE_CHECKED, LV_COLOR_GREEN );
                        lv_style_set_bg_color( &slider_style, LV_STATE_DEFAULT, LV_COLOR_GREEN );

                        styles_send_event_cb( STYLE_CHANGE, (void*)NULL );
                        styles_send_event_cb( STYLE_DARKMODE, (void*)NULL );
                        break;
        default:        lv_style_set_bg_color( &background_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        /** maibbar colors **/
                        lv_style_set_bg_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &mainbar_style, LV_OBJ_PART_MAIN, mainbar_font );
                        /** mainbar dropdown colors **/
                        lv_style_set_bg_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
                        lv_style_set_bg_opa( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, 0 );
                        lv_style_set_text_font( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, mainbar_font );
                        lv_style_set_text_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        /** app colors **/                        
                        lv_style_set_bg_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &app_style, LV_OBJ_PART_MAIN, app_font );
                        /** app ops style color **/
                        lv_style_set_bg_color( &app_opa_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
                        lv_style_set_text_font( &app_opa_style, LV_OBJ_PART_MAIN, app_font );
                        /** app dropdown colors **/
                        lv_style_set_bg_color( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
                        lv_style_set_bg_opa( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &app_dropdown_style, LV_OBJ_PART_MAIN, 0 );
                        lv_style_set_text_font( &app_dropdown_style, LV_OBJ_PART_MAIN, app_font );
                        lv_style_set_text_color( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        /** setup colors **/
                        lv_style_set_bg_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY );
                        lv_style_set_text_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_image_recolor( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &setup_tile_style, LV_OBJ_PART_MAIN, setup_font );
                        /** setup dropdown style **/
                        lv_style_set_bg_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
                        lv_style_set_bg_opa( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
                        lv_style_set_border_width( &setup_dropdown_style, LV_OBJ_PART_MAIN, 0 );
                        lv_style_set_border_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
                        lv_style_set_text_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        /** setup header style **/
                        lv_style_set_bg_color( &setup_header_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY );
                        lv_style_set_text_color( &setup_header_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &setup_header_tile_style, LV_OBJ_PART_MAIN, setup_header_font );
                        /** system/app icon/label style **/
                        lv_style_set_image_recolor( &system_icon_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_color( &system_icon_label_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &system_icon_label_style, LV_OBJ_PART_MAIN, system_icon_label_font );
                        lv_style_set_bg_opa( &app_icon_style, LV_OBJ_PART_MAIN, LV_OPA_40 );
                        lv_style_set_text_color( &app_icon_label_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
                        lv_style_set_text_font( &app_icon_label_style, LV_OBJ_PART_MAIN, app_icon_label_font );
                        /** button/switch/slider  style **/
                        lv_style_set_radius( &button_style, LV_STATE_DEFAULT, 3 );
                        lv_style_set_border_color( &button_style, LV_STATE_DEFAULT, LV_COLOR_WHITE );
                        lv_style_set_bg_color( &switch_style, LV_STATE_CHECKED, LV_COLOR_GREEN );
                        lv_style_set_bg_color( &slider_style, LV_STATE_DEFAULT, LV_COLOR_GREEN );

                        styles_send_event_cb( STYLE_CHANGE, (void*)NULL );
                        styles_send_event_cb( STYLE_LIGHTMODE, (void*)NULL );
                        break;
    }
}

static void define_styles(){
    /**
     * background style
     */
    lv_style_init( &background_style );
    lv_style_set_radius( &background_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &background_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &background_style, LV_OBJ_PART_MAIN, LV_OPA_100 );
    /**
     * mainbar theme
     */
    lv_style_init( &mainbar_style );
    lv_style_set_radius( &mainbar_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &mainbar_style, LV_OBJ_PART_MAIN, LV_OPA_0 );
    lv_style_set_border_width( &mainbar_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_image_recolor( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_text_font( &mainbar_style, LV_OBJ_PART_MAIN, mainbar_font );
    lv_style_set_text_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    /* setup dropdown list theme */
    lv_style_copy( &mainbar_dropdown_style, &mainbar_style );
    lv_style_set_bg_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
    lv_style_set_bg_opa( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_border_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
    lv_style_set_text_font( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, mainbar_font );
    lv_style_set_text_color( &mainbar_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    /**
     * app theme
     */
    lv_style_init( &app_style );
    lv_style_set_radius( &app_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &app_style, LV_OBJ_PART_MAIN, LV_OPA_0 );
    lv_style_set_border_width( &app_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_text_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_image_recolor( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_text_font( &app_style, LV_OBJ_PART_MAIN, app_font );
    /* app opa style */
    lv_style_copy( &app_opa_style, &mainbar_style );
    lv_style_set_bg_color( &app_opa_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &app_opa_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &app_opa_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font( &app_opa_style, LV_OBJ_PART_MAIN, app_font );
    /* setup dropdown list theme */
    lv_style_copy( &app_dropdown_style, &mainbar_dropdown_style );
    lv_style_set_bg_color( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
    lv_style_set_bg_opa( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &app_dropdown_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_border_color( &app_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
    lv_style_set_text_font( &app_dropdown_style, LV_OBJ_PART_MAIN, app_font );
    lv_style_set_text_color( &app_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    /**
     * setup theme
     */
    lv_style_copy( &setup_tile_style, &mainbar_style );
    lv_style_set_bg_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY );
    lv_style_set_bg_opa( &setup_tile_style, LV_OBJ_PART_MAIN, LV_OPA_100 );
    lv_style_set_border_width( &setup_tile_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_image_recolor( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_image_recolor_opa( &setup_tile_style, LV_OBJ_PART_MAIN, LV_OPA_100 );
    lv_style_set_image_opa( &setup_tile_style, LV_OBJ_PART_MAIN, LV_OPA_100 );
    lv_style_set_text_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_text_font( &setup_tile_style, LV_OBJ_PART_MAIN, setup_font );
    /* setup header theme */
    lv_style_copy( &setup_header_tile_style, &setup_tile_style );
    lv_style_set_bg_color( &setup_header_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY );
    lv_style_set_text_color( &setup_header_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_text_font( &setup_header_tile_style, LV_OBJ_PART_MAIN, setup_header_font );
    /* setup dropdown list theme */
    lv_style_copy( &setup_dropdown_style, &mainbar_style );
    lv_style_set_bg_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
    lv_style_set_bg_opa( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &setup_dropdown_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_border_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_MAKE( 0xa0, 0xa0, 0xa0 ) );
    lv_style_set_text_color( &setup_dropdown_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_text_font( &setup_dropdown_style, LV_OBJ_PART_MAIN, setup_font );
    /**
     * general system icon style
     */
    lv_style_init( &system_icon_style );
    lv_style_set_image_recolor( &system_icon_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_image_recolor_opa( &system_icon_style, LV_OBJ_PART_MAIN, LV_OPA_100 );
    lv_style_set_image_opa( &system_icon_style, LV_OBJ_PART_MAIN, LV_OPA_100 );
    /**
     * general app icon style
     */
    lv_style_init( &app_icon_style );
    lv_style_set_image_recolor( &app_icon_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_image_recolor_opa( &app_icon_style, LV_OBJ_PART_MAIN, LV_OPA_0 );
    lv_style_set_image_opa( &app_icon_style, LV_OBJ_PART_MAIN, LV_OPA_100 );
    lv_style_set_radius( &app_icon_style, LV_OBJ_PART_MAIN, 20 );
    lv_style_set_bg_color( &app_icon_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY );
    lv_style_set_bg_opa( &app_icon_style, LV_OBJ_PART_MAIN, LV_OPA_40 );
    lv_style_set_border_width( &app_icon_style, LV_OBJ_PART_MAIN, 0 );
    /**
     * general system icon label style
     */
    lv_style_init( &system_icon_label_style );
    lv_style_set_text_font( &system_icon_label_style, LV_OBJ_PART_MAIN, system_icon_label_font );
    lv_style_set_text_color( &system_icon_label_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    /**
     * general app icon style
     */
    lv_style_init( &app_icon_label_style );
    lv_style_set_text_font( &app_icon_label_style, LV_OBJ_PART_MAIN, app_icon_label_font );
    lv_style_set_text_color( &app_icon_label_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    /**
     * general button style
     */
    lv_style_init( &button_style );
    lv_style_set_radius( &button_style, LV_STATE_DEFAULT, 3 );
    lv_style_set_border_color( &button_style, LV_STATE_DEFAULT, LV_COLOR_WHITE );
    /**
     * general img button style
     */
    lv_style_copy(&img_button_style, &mainbar_style);
    /**
     * general label style
     */
    lv_style_init( &label_style );
    lv_style_set_text_color(&label_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    /**
     * general switch style
     */
    lv_style_init( &switch_style );
    lv_style_set_bg_color( &switch_style, LV_STATE_CHECKED, LV_COLOR_GREEN );
    /**
     * general roller style
     */
    lv_style_init( &roller_bg_style );
    lv_style_set_text_line_space(&roller_bg_style, LV_STATE_DEFAULT, ROLLER_TEXT_SPACE);
    lv_style_init( &roller_part_selected_style );
    /**
     * general arc style
     */
    lv_style_init( &arc_bg_style );
    lv_style_set_bg_opa( &arc_bg_style, LV_ARC_PART_BG, LV_OPA_TRANSP );
    lv_style_set_border_width( &arc_bg_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_init( &arc_style );
    lv_style_set_line_rounded( &arc_style, LV_STATE_DEFAULT, false );
    /**
     * general slider style
     */
    lv_style_init( &slider_style );
    lv_style_set_bg_color( &slider_style, LV_STATE_DEFAULT, LV_COLOR_GREEN );
    /**
     * general popup style
     */
    lv_style_copy( &popup_style, &mainbar_style );
    lv_style_set_bg_color( &popup_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &popup_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &popup_style, LV_OBJ_PART_MAIN, 0);
    /**
     * trigger style change event
     **/
#ifdef M5PAPER
    widget_style_theme_set( 0 );
#else
    widget_style_theme_set( ~0 );
#endif

    styles_defined = true;
}

lv_style_t *ws_get_background_style(){
    if (!styles_defined){
         define_styles();
    }
    return &background_style;
}

lv_style_t *ws_get_mainbar_style(){
    if (!styles_defined){
         define_styles();
    }
    return &mainbar_style;
}

lv_style_t *ws_get_app_style(){
    if (!styles_defined){
         define_styles();
    }
    return &app_style;
}
lv_style_t *ws_get_app_opa_style(){
    if (!styles_defined){
         define_styles();
    }
    return &app_opa_style;
}

lv_style_t *ws_get_container_style(){
    if (!styles_defined){
         define_styles();
    }
    return &mainbar_style;
}

lv_style_t *ws_get_setup_tile_style(){
    if (!styles_defined){
         define_styles();
    }
    return &setup_tile_style;
}

lv_style_t *ws_get_setup_header_tile_style(){
    if (!styles_defined){
         define_styles();
    }
    return &setup_header_tile_style;
}

lv_style_t *ws_get_button_style(){
    if (!styles_defined){
         define_styles();
    }
    return &button_style;
}

lv_style_t *ws_get_img_button_style(){
    if (!styles_defined){
         define_styles();
    }
    return &img_button_style;
}

lv_style_t *ws_get_label_style(){
    if (!styles_defined){
         define_styles();
    }
    return &label_style;
}

lv_style_t *ws_get_switch_style() {
    if (!styles_defined){
         define_styles();
    }
    return &switch_style;
}

lv_style_t *ws_get_roller_bg_style(){
    if (!styles_defined){
         define_styles();
    }
    return &roller_bg_style;
}

lv_style_t *ws_get_setup_dropdown_style(){
    if (!styles_defined){
         define_styles();
    }
    return &setup_dropdown_style;
}

lv_style_t *ws_get_mainbar_dropdown_style(){
    if (!styles_defined){
         define_styles();
    }
    return &mainbar_dropdown_style;
}

lv_style_t *ws_get_app_dropdown_style(){
    if (!styles_defined){
         define_styles();
    }
    return &app_dropdown_style;
}

lv_style_t *ws_get_roller_part_selected_style(){
    if (!styles_defined){
         define_styles();
    }
    return &roller_part_selected_style;
}

lv_style_t *ws_get_popup_style(){
    if (!styles_defined){
         define_styles();
    }
    return &popup_style;
}

lv_style_t *ws_get_arc_style( void ) {
    if (!styles_defined){
         define_styles();
    }

    return( &arc_style );
}

lv_style_t *ws_get_arc_bg_style( void ) {
    if (!styles_defined){
         define_styles();
    }

    return( &arc_bg_style );
}

lv_style_t *ws_get_slider_style( void ) {
    if (!styles_defined){
         define_styles();
    }

    return( &slider_style );
}


lv_style_t *ws_get_system_icon_style( void ) {
    if (!styles_defined){
         define_styles();
    }

    return( &system_icon_style );
}

lv_style_t *ws_get_app_icon_style( void ) {
    if (!styles_defined){
         define_styles();
    }

    return( &app_icon_style );
}

lv_style_t *ws_get_system_icon_label_style( void ) {
    if (!styles_defined){
         define_styles();
    }

    return( &system_icon_label_style );
}

lv_style_t *ws_get_app_icon_label_style( void ) {
    if (!styles_defined){
         define_styles();
    }

    return( &app_icon_label_style );
}