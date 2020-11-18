/****************************************************************************
 *  FindPhone_main.cpp
 *  Copyright  2020  David Stewart
 *  Email: genericsoftwaredeveloper@gmail.com
 *
 *  Requires Library: IRremoteESP8266 by David Conran
 *
 *  Remote codes are stored in FindPhone_main.h, currently only a handful
 *  of Samsung TV remote codes are defined.
 *
 *
 *  Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  Example_App"
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
 * 
 */
#include "config.h"
#include <TTGO.h>

#include "FindPhone.h"
#include "FindPhone_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

#include "hardware/blectl.h"
#include "hardware/json_psram_allocator.h"
#include "hardware/powermgm.h"

lv_obj_t *FindPhone_main_tile = NULL;
lv_style_t FindPhone_main_style;

lv_task_t * _FindPhone_search_task; 

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
//LV_IMG_DECLARE(refresh_32px); //unused
LV_FONT_DECLARE(Ubuntu_32px);

static void exit_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event );
//static void enter_FindPhone_setup_event_cb( lv_obj_t * obj, lv_event_t event );

static void stop_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void go_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event );

static void FindPhone_search_task( lv_task_t * task );

void FindPhone_main_setup( uint32_t tile_num ) {

    FindPhone_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &FindPhone_main_style, mainbar_get_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( FindPhone_main_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px );
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px );
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px );
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px );
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &FindPhone_main_style );
    lv_obj_align( exit_btn, FindPhone_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_FindPhone_main_event_cb );

    /*//No use for this just yet
    lv_obj_t * setup_btn = lv_imgbtn_create( FindPhone_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &FindPhone_main_style );
    lv_obj_align(setup_btn, FindPhone_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_FindPhone_setup_event_cb );
    */
    
    //Top Left, power button
    lv_obj_t *FindPhone_main_go_btn = NULL;
    FindPhone_main_go_btn = lv_btn_create( FindPhone_main_tile, NULL);  
    lv_obj_set_size( FindPhone_main_go_btn, 170, 40);
    lv_obj_set_event_cb( FindPhone_main_go_btn, go_FindPhone_main_event_cb );
    lv_obj_add_style( FindPhone_main_go_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( FindPhone_main_go_btn, NULL, LV_ALIGN_CENTER, 0, -90 );
    lv_obj_t *FindPhone_main_pwr_label = lv_label_create( FindPhone_main_go_btn, NULL);
    lv_label_set_text( FindPhone_main_pwr_label, "Start Search");
    
    //Middle Left, mute button
    lv_obj_t *FindPhone_main_stop_btn = NULL;
    FindPhone_main_stop_btn = lv_btn_create( FindPhone_main_tile, NULL);  
    lv_obj_set_event_cb( FindPhone_main_stop_btn, stop_FindPhone_main_event_cb );
    lv_obj_set_size( FindPhone_main_stop_btn, 170, 40);
    lv_obj_add_style( FindPhone_main_stop_btn, LV_BTN_PART_MAIN, mainbar_get_button_style() );
    lv_obj_align( FindPhone_main_stop_btn, NULL, LV_ALIGN_CENTER, 0, -30 );
    lv_obj_t *FindPhone_main_mute_label = lv_label_create( FindPhone_main_stop_btn, NULL);
    lv_label_set_text( FindPhone_main_mute_label, "Stop Search");
    
    lv_style_set_text_opa( &FindPhone_main_style, LV_OBJ_PART_MAIN, LV_OPA_70);
    lv_style_set_text_font( &FindPhone_main_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_obj_t *app_label = lv_label_create( FindPhone_main_tile, NULL);
    lv_label_set_text( app_label, "Find\nPhone");
    lv_obj_reset_style_list( app_label, LV_OBJ_PART_MAIN );
    lv_obj_add_style( app_label, LV_OBJ_PART_MAIN, &FindPhone_main_style );
    lv_obj_align( app_label, FindPhone_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    
}

/*//Not yet in use
static void enter_FindPhone_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( FindPhone_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}
*/



static void exit_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event ) 
{
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        pinMode(13, INPUT); //Leave this tristated when we leave
                                        mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}
static void go_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event ) 
{
    switch( event ) {
        case( LV_EVENT_CLICKED ):       //msg: {"t":"find","n":true}
                                        searching = true;
									    _FindPhone_search_task = lv_task_create( FindPhone_search_task, 1000, LV_TASK_PRIO_MID, NULL );
                                        break;
    }
}
static void stop_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event ) 
{
    switch( event ) {
        case( LV_EVENT_CLICKED ):       //msg: {"t":"find","n":false}
                                        searching = false;
										//lv_task_del(FindPhone_search_task);
                                        break;
    }
}

static void FindPhone_search_task( lv_task_t * task )
{ 
	if (searching) 
	{
		blectl_send_msg( (char*)"\r\n{t:\"findPhone\", n:\"true\"}\r\n" );
	} else {
		blectl_send_msg( (char*)"\r\n{t:\"findPhone\", n:\"false\"}\r\n" );
		//lv_task_del(_FindPhone_search_task);
	}
}