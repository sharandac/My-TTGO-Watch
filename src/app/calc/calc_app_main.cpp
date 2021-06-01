/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#include <TTGO.h>

#include "calc_app.h"
#include "calc_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

lv_obj_t *calc_app_main_tile = NULL;
lv_style_t calc_app_main_style;
lv_style_t result_style;
lv_obj_t *result_label;
lv_obj_t **numbers = new lv_obj_t*[10];

LV_FONT_DECLARE(Ubuntu_32px);

float inputs[2] = { 0.0, 0.0 };
char input[16] = "\0";

void calc_number_event_cb( lv_obj_t * obj, lv_event_t event );
uint8_t calc_get_number( lv_obj_t * obj );
void calc_process(uint8_t number);

void calc_app_main_setup( uint32_t tile_num ) {

    calc_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &calc_app_main_style, ws_get_mainbar_style() );

    lv_obj_t * exit_btn = wf_add_exit_button( calc_app_main_tile, exit_calc_app_main_event_cb, &calc_app_main_style );
    lv_obj_align(exit_btn, calc_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    // result label
    lv_style_copy(&result_style, ws_get_label_style());
    lv_style_set_text_color(&result_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_bg_color(&result_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_bg_opa(&result_style, LV_OBJ_PART_MAIN, LV_OPA_80);
    lv_style_set_text_font(&result_style, LV_OBJ_PART_MAIN, &Ubuntu_32px);

    result_label = lv_label_create( calc_app_main_tile, NULL);
    lv_label_set_text(result_label, "");
    lv_obj_add_style(result_label, LV_OBJ_PART_MAIN, &result_style);
    lv_obj_align(result_label, calc_app_main_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_label_set_align(result_label, LV_LABEL_ALIGN_RIGHT);

    // number buttons
    for (uint8_t number = 0; number <= 9; number++)
    {
        uint8_t row = 3;
        uint8_t column = 1;
        if (number > 0) {
            row = floor((number - 1) / 3);
            column = (number - 1) % 3;
        }

        const char *name = new char[2] { (char)(number + 48), '\0' };
        lv_obj_t *calc_number_btn = wf_add_button( calc_app_main_tile, name, 50, 35, calc_number_event_cb );
        lv_obj_align( calc_number_btn, NULL, LV_ALIGN_IN_TOP_LEFT, 55 * column, (40 * row) + 40 );
        lv_btn_set_checkable(calc_number_btn, false);
        numbers[number] = calc_number_btn;
    }
    
    // point button
    //TODO

    // operator buttons
    //TODO

    // result button
    //TODO
}

void calc_number_event_cb( lv_obj_t * obj, lv_event_t event ) 
{
    switch( event ) {
        case( LV_EVENT_CLICKED ):       uint8_t number = calc_get_number(obj);
                                        if (number > 0) calc_process(number);
                                        break;
    }
}

uint8_t calc_get_number( lv_obj_t * obj )
{
    for (uint8_t number = 0; number <= 9; number++)
    {
        if (numbers[number] != obj) continue;
        return number;
    }
    
    return 0;
}

void calc_process(uint8_t number)
{
    input[strlen(input)] = number + 48;
    input[strlen(input)+1] = '\0';

    inputs[0] = atof(input);
    //TODO: Implement calculation based on the last operator

    char temp[16];
    snprintf(temp, sizeof(temp), "%g", inputs[0]);
    log_i("label content would be: %g", inputs[0]);
    lv_label_set_text(result_label, temp);
    lv_event_send_refresh(result_label);
}