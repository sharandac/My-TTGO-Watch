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
lv_obj_t *FindPhone_main_iris = NULL;

lv_style_t FindPhone_main_style;

lv_task_t * _FindPhone_search_task; 

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(eye_lid_closed);
LV_IMG_DECLARE(eye_lid_open);
LV_IMG_DECLARE(eye_iris);
//LV_IMG_DECLARE(refresh_32px); //unused
LV_FONT_DECLARE(Ubuntu_32px);

int rem_iris_x = 0;
int rem_iris_y = 0;

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
    
    // eye toggle button
	lv_obj_t *FindPhone_main_go_btn = NULL;
    FindPhone_main_go_btn = lv_imgbtn_create( FindPhone_main_tile, NULL);  
	lv_imgbtn_set_src(FindPhone_main_go_btn,LV_BTN_STATE_RELEASED, &eye_lid_open);
	lv_imgbtn_set_src(FindPhone_main_go_btn,LV_BTN_STATE_CHECKED_PRESSED, &eye_lid_open);
    lv_imgbtn_set_src(FindPhone_main_go_btn, LV_BTN_STATE_CHECKED_RELEASED, &eye_lid_closed);
    lv_imgbtn_set_src(FindPhone_main_go_btn, LV_BTN_STATE_CHECKED_PRESSED, &eye_lid_closed);
	lv_btn_set_checkable(FindPhone_main_go_btn, true);
    lv_btn_toggle(FindPhone_main_go_btn);
    lv_obj_align( FindPhone_main_go_btn, NULL, LV_ALIGN_CENTER, 0, 0 );
	lv_obj_set_event_cb( FindPhone_main_go_btn, go_FindPhone_main_event_cb );

	
	// iris 
	FindPhone_main_iris = lv_img_create( FindPhone_main_tile, NULL);
	lv_img_set_src (FindPhone_main_iris,&eye_iris);
	lv_obj_set_hidden(FindPhone_main_iris,true);
    lv_obj_align( FindPhone_main_iris, NULL, LV_ALIGN_CENTER, 0, 0 );
    
    lv_style_set_text_opa( &FindPhone_main_style, LV_OBJ_PART_MAIN, LV_OPA_70);
    lv_style_set_text_font( &FindPhone_main_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_obj_t *app_label = lv_label_create( FindPhone_main_tile, NULL);
    lv_label_set_text( app_label, "Find\nPhone");
    lv_obj_reset_style_list( app_label, LV_OBJ_PART_MAIN );
    lv_obj_add_style( app_label, LV_OBJ_PART_MAIN, &FindPhone_main_style );
    lv_obj_align( app_label, FindPhone_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    lv_obj_align( FindPhone_main_go_btn, NULL, LV_ALIGN_CENTER, 0, 0 );

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
static void REM (bool val)
{
	if (!val )
	{
		lv_obj_set_hidden(FindPhone_main_iris,true);
	} else {
		lv_obj_set_hidden(FindPhone_main_iris,false);
		//move iris between +-50,+-50 around center
		if (rand()%2)
		{ 
			//Horizontal Move
			lv_anim_t rem_iris_x_anim;
			lv_anim_init(&rem_iris_x_anim);
			lv_anim_set_exec_cb(&rem_iris_x_anim,(lv_anim_exec_xcb_t)lv_obj_set_x);
			lv_anim_set_var(&rem_iris_x_anim,FindPhone_main_iris);
			lv_anim_set_time(&rem_iris_x_anim,1000);
			lv_anim_set_values(&rem_iris_x_anim,lv_obj_get_x(FindPhone_main_iris),(LV_HOR_RES/2)+(rand()%40)-50);
			lv_anim_start(&rem_iris_x_anim);
		} else {
			//Vertical Move
			lv_anim_t rem_iris_y_anim;
			lv_anim_init(&rem_iris_y_anim);
			lv_anim_set_exec_cb(&rem_iris_y_anim,(lv_anim_exec_xcb_t)lv_obj_set_y);
			lv_anim_set_var(&rem_iris_y_anim,FindPhone_main_iris);
			lv_anim_set_time(&rem_iris_y_anim,1000);
			lv_anim_set_values(&rem_iris_y_anim,lv_obj_get_y(FindPhone_main_iris),(LV_VER_RES/2)+(rand()%30)-60);
			lv_anim_start(&rem_iris_y_anim);
		}	
	}		
}

static void toggle_searching ()
{
	if (searching) 
	{
		//lv_task_del(_FindPhone_search_task);
		REM(false);
		blectl_send_msg( (char*)"\r\n{t:\"findPhone\", n:\"false\"}\r\n" );
		//lv_btn_set_state(FindPhone_main_go_btn,LV_BTN_STATE_RELEASED);
		searching = false;
	}else {
	    _FindPhone_search_task = lv_task_create( FindPhone_search_task, 1000, LV_TASK_PRIO_MID, NULL );
		//lv_btn_set_state(FindPhone_main_go_btn,LV_BTN_STATE_RELEASED);
		searching = true;
	}		
}

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
        case( LV_EVENT_VALUE_CHANGED ):       //msg: {"t":"find","n":true}
                                        toggle_searching();
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
		REM(true);
	} else {
		blectl_send_msg( (char*)"\r\n{t:\"findPhone\", n:\"false\"}\r\n" );
		//lv_task_del(_FindPhone_search_task);
	}
}