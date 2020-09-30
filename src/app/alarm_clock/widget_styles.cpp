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

#define ROLLER_TEXT_SPACE 8 //half of font size - good size for 4 lines roller on the small display

static bool styles_defined = false;

static lv_style_t mainbar_style;
static lv_style_t setup_tile_style;
static lv_style_t button_style;
static lv_style_t img_button_style;
static lv_style_t label_style;
static lv_style_t switch_style;
static lv_style_t roller_bg_style;
static lv_style_t roller_part_selected_style;
static lv_style_t popup_style;

static void define_styles(){
    lv_style_init( &mainbar_style );
    lv_style_set_radius( &mainbar_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY );
    lv_style_set_bg_opa( &mainbar_style, LV_OBJ_PART_MAIN, LV_OPA_0 );
    lv_style_set_border_width( &mainbar_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_text_color( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_image_recolor( &mainbar_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    //lv_style_set_bg_opa( &mainbar_style, LV_OBJ_PART_MAIN, LV_OPA_30);

    lv_style_init( &setup_tile_style );
    lv_style_set_bg_color( &setup_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &setup_tile_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &setup_tile_style, LV_OBJ_PART_MAIN, 0);

    lv_style_init( &button_style );
    lv_style_set_radius(&button_style, LV_STATE_DEFAULT, 4);


    lv_style_copy(&img_button_style, &mainbar_style);

    lv_style_init( &label_style );
    lv_style_set_text_color(&label_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);

    lv_style_init( &switch_style );
    lv_style_set_bg_color( &switch_style, LV_STATE_CHECKED, LV_COLOR_GREEN );

    lv_style_init( &roller_bg_style );
    lv_style_set_text_line_space(&roller_bg_style, LV_STATE_DEFAULT, ROLLER_TEXT_SPACE);
    lv_style_init( &roller_part_selected_style );
    //the default roller color is red - may be it is the best one as default
    //lv_style_set_bg_color( &roller_part_selected_style, LV_STATE_DEFAULT, LV_COLOR_GRAY);

    lv_style_copy( &popup_style, &mainbar_style );
    lv_style_set_bg_color( &popup_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &popup_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &popup_style, LV_OBJ_PART_MAIN, 0);
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
    return &mainbar_style;
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
