/****************************************************************************
 *   June 04 02:01:00 2021
 *   Copyright  2021  Dirk Sarodnick
 *   Email: programmer@dirk-sarodnick.de
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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "calc_app.h"
#include "calc_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

lv_obj_t *calc_app_main_tile = NULL;
lv_style_t result_style;
lv_obj_t *result_label;
lv_style_t history_style;
lv_obj_t *history_label;
lv_obj_t *button_matrix;

LV_FONT_DECLARE(Ubuntu_12px);
LV_FONT_DECLARE(Ubuntu_32px);

static const char* buttons[20] = {"7","8","9","/","\n","4","5","6","*","\n","1","2","3","-","\n","0","C/CE",".","+",""};
float inputs[2] = { 0, 0 };
char input[16] = "\0";
char op = '\0';
char oop = '\0';

bool calc_mainbar_button_event_cb( EventBits_t event, void *arg );
void calc_button_event_cb( lv_obj_t * obj, lv_event_t event );
void calc_result_event_cb( lv_obj_t * obj, lv_event_t event );
void calc_update_button();
void calc_process_button(char cmd);
void calc_process_operator(char cmd, char op);
void calc_show_result(float output);
void calc_show_history(float left, char op, float right, bool showLeft, bool showRight, bool showEquals = false);

void calc_app_main_setup( uint32_t tile_num ) {

    calc_app_main_tile = mainbar_get_tile_obj( tile_num );

    lv_obj_t * exit_btn = wf_add_exit_button( calc_app_main_tile, exit_calc_app_main_event_cb );
    #if defined( ROUND_DISPLAY )
        lv_obj_align(exit_btn, calc_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, -( THEME_ICON_SIZE / 2 ), -THEME_ICON_PADDING );
    #else
        lv_obj_align(exit_btn, calc_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_ICON_PADDING, -THEME_ICON_PADDING );
    #endif

    lv_obj_t * result_btn = wf_add_equal_button( calc_app_main_tile, calc_result_event_cb );
    #if defined( ROUND_DISPLAY )
        lv_obj_align(result_btn, calc_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, THEME_ICON_SIZE / 2, -THEME_ICON_PADDING );
    #else
        lv_obj_align(result_btn, calc_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_ICON_PADDING, -THEME_ICON_PADDING );
    #endif

    // result label
    lv_style_copy(&result_style, ws_get_label_style());
    lv_style_set_text_color(&result_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_color(&result_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&result_style, LV_STATE_DEFAULT, LV_OPA_80);
    lv_style_set_text_font(&result_style, LV_STATE_DEFAULT, &Ubuntu_32px);
	lv_style_set_pad_top(&result_style, LV_STATE_DEFAULT, 10);
	lv_style_set_pad_left(&result_style, LV_STATE_DEFAULT, 3);
	lv_style_set_pad_right(&result_style, LV_STATE_DEFAULT, 3);

    result_label = lv_label_create( calc_app_main_tile, NULL);
    lv_label_set_text(result_label, "0");
    #if defined( ROUND_DISPLAY )
        lv_label_set_align(result_label, LV_LABEL_ALIGN_CENTER );
	#else
        lv_label_set_align(result_label, LV_LABEL_ALIGN_RIGHT );
    #endif
    lv_label_set_long_mode(result_label, LV_LABEL_LONG_CROP);
    lv_obj_add_style(result_label, LV_OBJ_PART_MAIN, &result_style);
    lv_obj_align(result_label, calc_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
	lv_obj_set_size(result_label, lv_disp_get_hor_res( NULL ), 45 );

    // history label
    lv_style_copy(&history_style, ws_get_label_style());
    lv_style_set_text_color(&history_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_bg_color(&history_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&history_style, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_style_set_text_opa(&history_style, LV_STATE_DEFAULT, LV_OPA_50);
    lv_style_set_text_font(&history_style, LV_STATE_DEFAULT, &Ubuntu_12px);
	lv_style_set_pad_left(&history_style, LV_STATE_DEFAULT, 3);
	lv_style_set_pad_right(&history_style, LV_STATE_DEFAULT, 3);

    history_label = lv_label_create( calc_app_main_tile, NULL);
    lv_label_set_text(history_label, "");
    #if defined( ROUND_DISPLAY )
        lv_label_set_align(history_label, LV_LABEL_ALIGN_CENTER );
	#else
        lv_label_set_align(history_label, LV_LABEL_ALIGN_RIGHT );
    #endif
    lv_label_set_long_mode(history_label, LV_LABEL_LONG_CROP);
    lv_obj_add_style(history_label, LV_OBJ_PART_MAIN, &history_style);
    lv_obj_align(history_label, calc_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, -2 );
	lv_obj_set_size(history_label, lv_disp_get_hor_res( NULL ), 15 );
    
    button_matrix = lv_btnmatrix_create(calc_app_main_tile, NULL);
	lv_obj_add_style(button_matrix, LV_BTNMATRIX_PART_BG, APP_STYLE );
	lv_obj_add_style(button_matrix, LV_BTNMATRIX_PART_BTN, ws_get_button_style() );
    lv_obj_set_style_local_bg_opa( button_matrix, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_OPA_80 );
	lv_obj_set_pos( button_matrix, 0, 45);
	lv_obj_set_size(button_matrix, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) - 38 - THEME_ICON_SIZE );

	lv_btnmatrix_set_map(button_matrix, buttons);
	lv_btnmatrix_set_one_check(button_matrix, false);
    lv_btnmatrix_set_btn_ctrl(button_matrix, 3, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_btn_ctrl(button_matrix, 7, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_btn_ctrl(button_matrix, 11, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_btn_ctrl(button_matrix, 15, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_obj_set_event_cb(button_matrix, calc_button_event_cb);

    mainbar_add_tile_button_cb( tile_num, calc_mainbar_button_event_cb );
}

bool calc_mainbar_button_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_EXIT:   mainbar_jump_back();
                            break;
        case BUTTON_SETUP:  calc_process_button('=');
                            calc_update_button();
                            break;
    }
    return( true );
}

void calc_button_event_cb( lv_obj_t * obj, lv_event_t event ) 
{
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): {
                                        const char * txt = lv_btnmatrix_get_active_btn_text(obj);
                                        calc_process_button(txt[0]);
                                        calc_update_button();
                                        break;
        }
        case( LV_EVENT_LONG_PRESSED ): {
                                        const char * txt = lv_btnmatrix_get_active_btn_text(obj);
                                        if (txt[0] == 'C') {
                                            calc_process_button('D');
                                            calc_update_button();
                                        }
                                        break;
        }
        case( LV_EVENT_CLICKED ):       calc_update_button();
                                        break;
    }
}

void calc_result_event_cb( lv_obj_t * obj, lv_event_t event ) 
{
    switch( event ) {
        case( LV_EVENT_CLICKED ):       calc_process_button('=');
                                        calc_update_button();
                                        break;
    }
}

/*
 * Update the operator buttons checked styles
*/
void calc_update_button()
{
    switch( op ) {
        case '/':
            lv_btnmatrix_set_btn_ctrl(button_matrix, 3, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 7, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 15, LV_BTNMATRIX_CTRL_CHECK_STATE);
            break;
        case '*':
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 3, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_set_btn_ctrl(button_matrix, 7, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 15, LV_BTNMATRIX_CTRL_CHECK_STATE);
            break;
        case '-':
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 3, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 7, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_set_btn_ctrl(button_matrix, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 15, LV_BTNMATRIX_CTRL_CHECK_STATE);
            break;
        case '+':
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 3, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 7, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_set_btn_ctrl(button_matrix, 15, LV_BTNMATRIX_CTRL_CHECK_STATE);
            break;
        default:
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 3, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 7, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_clear_btn_ctrl(button_matrix, 15, LV_BTNMATRIX_CTRL_CHECK_STATE);
            break;
    }
}

/*
 * Processes the pressed button
 * The idea of this part is inspired by https://github.com/wfdudley/T-watch-2020/blob/master/appCalc.cpp
*/
void calc_process_button(char cmd)
{
    switch( cmd ) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '.':
            if ( op == '=') {
                inputs[1] = 0;
                oop = '\0';
                op = '\0';
                calc_show_history(inputs[1], oop, inputs[0], false, false);
            }
            input[strlen(input)] = cmd;
            input[strlen(input)] = '\0';
            inputs[0] = atof(input);
            calc_show_result(inputs[0]);
            break;
        case '+':
            calc_process_operator(cmd, op);
            oop = cmd;
            op = cmd;
            calc_show_result(inputs[1]);
            calc_show_history(inputs[1], oop, inputs[0], true, false);
            break;
        case '-':
            calc_process_operator(cmd, op);
            oop = cmd;
            op = cmd;
            calc_show_result(inputs[1]);
            calc_show_history(inputs[1], oop, inputs[0], true, false);
            break;
        case '*':
            calc_process_operator(cmd, op);
            oop = cmd;
            op = cmd;
            calc_show_result(inputs[1]);
            calc_show_history(inputs[1], oop, inputs[0], true, false);
            break;
        case '/':
            calc_process_operator(cmd, op);
            oop = cmd;
            op = cmd;
            calc_show_result(inputs[1]);
            calc_show_history(inputs[1], oop, inputs[0], true, false);
            break;
        case 'C':
            memset(input, '\0', sizeof(input)/sizeof(char));
            if ( op == '=') {
                inputs[1] = 0;
                oop = '\0';
                op = '\0';
                calc_show_history(inputs[1], oop, inputs[0], false, false);
            } else {
                inputs[0] = 0;
                calc_show_history(inputs[1], oop, inputs[0], true, false);
            }
            calc_show_result(inputs[0]);
            break;
        case 'D':
            memset(input, '\0', sizeof(input)/sizeof(char));
            inputs[1] = 0;
            inputs[0] = 0;
            oop = '\0';
            op = '\0';
            calc_show_result(inputs[0]);
            calc_show_history(inputs[1], oop, inputs[0], false, false);
            break;
        case '=':
            calc_show_history(inputs[1], oop, inputs[0], true, true, true);
            calc_process_operator(cmd, op);
            op = cmd;
            calc_show_result(inputs[1]);
            break;
    }
}

/*
 * Process the pressed button in combination with the current operator
*/
void calc_process_operator(char cmd, char op)
{
    switch( op ) {
        case '+':
            memset(input, '\0', sizeof(input)/sizeof(char));
            inputs[1] = inputs[1] + inputs[0];
            inputs[0] = 0;
            break;
        case '-':
            memset(input, '\0', sizeof(input)/sizeof(char));
            inputs[1] = inputs[1] - inputs[0];
            inputs[0] = 0;
            break;
        case '*':
            memset(input, '\0', sizeof(input)/sizeof(char));
            inputs[1] = inputs[1] * inputs[0];
            inputs[0] = 0;
            break;
        case '/':
            memset(input, '\0', sizeof(input)/sizeof(char));
            inputs[1] = inputs[1] / inputs[0];
            inputs[0] = 0;
            break;
        case '=':
            memset(input, '\0', sizeof(input)/sizeof(char));
            inputs[0] = 0;
            break;
        case '\0':
            memset(input, '\0', sizeof(input)/sizeof(char));
            inputs[1] = inputs[0];
            inputs[0] = 0;
            break;
    }
}

void calc_show_result(float output)
{
    char temp[16];
    snprintf(temp, sizeof(temp), "%g", output);
    lv_label_set_text(result_label, temp);
    lv_event_send_refresh(result_label);
}

void calc_show_history(float left, char op, float right, bool showLeft, bool showRight, bool showEquals)
{
    char temp[16];
    if (showLeft && showRight && op != '\0') {
        snprintf(temp, sizeof(temp), showEquals ? "%g %c %g =" : "%g %c %g", left, op, right);
    } else if (showLeft && op != '\0') {
        snprintf(temp, sizeof(temp), "%g %c", left, op);
    } else {
        memset(temp, '\0', sizeof(temp)/sizeof(char));
    }
    lv_label_set_text(history_label, temp);
    lv_event_send_refresh(history_label);
}
