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

#include "my_basic_app.h"
#include "my_basic_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"


#include "bas_arduino.h"

lv_obj_t *my_basic_app_main_tile = NULL;
lv_style_t my_basic_app_main_style;
lv_style_t my_basic_page_main_style;
lv_style_t my_basic_text_main_style;

lv_task_t * _my_basic_app_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_16px);

static void exit_my_basic_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_my_basic_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void refresh_output_event_cb( lv_obj_t * obj, lv_event_t event );
void my_basic_app_task( lv_task_t * task );

lv_obj_t *output;

void DoBasic( void );

void my_basic_app_main_setup( uint32_t tile_num ) {

    my_basic_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &my_basic_app_main_style, mainbar_get_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( my_basic_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &my_basic_app_main_style );
    lv_obj_align(exit_btn, my_basic_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_my_basic_app_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( my_basic_app_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &my_basic_app_main_style );
    lv_obj_align(setup_btn, my_basic_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_my_basic_app_setup_event_cb );


        /*Create a page*/
    lv_obj_t * page = lv_page_create(my_basic_app_main_tile, NULL);
    lv_style_copy( &my_basic_page_main_style, &my_basic_app_main_style );
    lv_style_set_bg_opa( &my_basic_page_main_style, LV_OBJ_PART_MAIN, LV_OPA_50);
    lv_obj_set_size(page, 240, 192);
    lv_obj_align(page, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);



    /*Create a label on the page*/
    lv_style_copy( &my_basic_text_main_style, &my_basic_page_main_style );
    lv_style_set_text_font( &my_basic_text_main_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    lv_style_set_text_color(&my_basic_text_main_style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    output = lv_label_create(page, NULL);
    lv_obj_add_style( output, LV_OBJ_PART_MAIN, &my_basic_text_main_style );
    lv_label_set_long_mode(output, LV_LABEL_LONG_BREAK);            /*Automatically break long lines*/
    lv_obj_set_width(output, lv_page_get_width_fit(page));          /*Set the label width to max value to not show hor. scroll bars*/


    lv_obj_t * reload_btn = lv_imgbtn_create( my_basic_app_main_tile, NULL);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_PRESSED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_PRESSED, &refresh_32px);
    lv_obj_add_style(reload_btn, LV_IMGBTN_PART_MAIN, &my_basic_page_main_style );
    lv_obj_align(reload_btn, my_basic_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, 0 , -10 );
    lv_obj_set_event_cb( reload_btn, refresh_output_event_cb );






    // create an task that runs every secound
    _my_basic_app_task = lv_task_create( my_basic_app_task, 1000, LV_TASK_PRIO_MID, NULL );
}

static void enter_my_basic_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( my_basic_app_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_my_basic_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

static void refresh_output_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       DoBasic();
                                        break;
    }
}

void my_basic_app_task( lv_task_t * task ) {
    // put your code her
}

void DoBasic() {
    struct mb_interpreter_t* bas = NULL;

    const char *program =
            "PRINT \"Hello world !\"\r\n"
            "FOR i=1 to 3\r\n"
            "  PRINT i\r\n"
            "NEXT i\r\n"
            "PRINT \"Yes I'm working\"\r\n";

    Serial.printf("Free heap: %d\r\n", ESP.getFreeHeap());
    Serial.printf("Free PSRAM: %d\r\n", ESP.getFreePsram());
    
    Serial.printf("My Basic RUN\r\n");

	mb_init();
	mb_open(&bas);
    enableArduinoBindings(bas);
	mb_load_string(bas, program, true);
	mb_run(bas, true);
	mb_close(&bas);
	mb_dispose();

    Serial.printf("My Basic END\r\n");

    Serial.printf("Free heap: %d\r\n", ESP.getFreeHeap());
    Serial.printf("Free PSRAM: %d\r\n", ESP.getFreePsram());

}