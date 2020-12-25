/****************************************************************************
 *  FindPhone_main.cpp
 *  Neuroplant  11/2020
 *  Email: m.roych@gmail.com
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
 * 
 */
#include "config.h"
#include <TTGO.h>

#include "FindPhone.h"
#include "FindPhone_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/sound/piep.h"

#include "hardware/blectl.h"
#include "hardware/sound.h"
#include "hardware/motor.h"
#include "hardware/json_psram_allocator.h"
#include "hardware/powermgm.h"

lv_obj_t *FindPhone_main_tile = NULL;
lv_obj_t *FindPhone_main_iris = NULL;
lv_style_t FindPhone_main_style;
lv_obj_t *bluetooth_FindPhone_tile = NULL;
lv_style_t bluetooth_FindPhone_style;
lv_style_t bluetooth_FindPhone_exit_btn_style;
uint32_t bluetooth_FindPhone_tile_num;

lv_obj_t *bluetooth_FindPhone_img = NULL;
lv_obj_t *bluetooth_FindPhone_label = NULL;										   
LV_IMG_DECLARE(cancel_32px);

lv_task_t * _FindPhone_search_task; 
LV_IMG_DECLARE(eye_200px);
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(eye_lid_closed);
LV_IMG_DECLARE(eye_lid_open);
LV_IMG_DECLARE(eye_iris);
LV_FONT_DECLARE(Ubuntu_32px);

int rem_iris_x = 0;
int rem_iris_y = 0;
static bool searching = false;

static void exit_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event );

static void go_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_FindPhone_event_cb(lv_obj_t *obj, lv_event_t event);
bool bluetooth_FindPhone_event_cb(EventBits_t event, void *arg);
static void bluetooth_FindPhone_msg_pharse(const char *msg);

void bluetooth_FindPhone_tile_setup(void)
{
    // get an app tile and copy mainstyle
    bluetooth_FindPhone_tile_num = mainbar_add_app_tile(1, 1, "bluetooth FindPhone");
    bluetooth_FindPhone_tile = mainbar_get_tile_obj(bluetooth_FindPhone_tile_num);

    lv_style_copy(&bluetooth_FindPhone_style, mainbar_get_style());
    lv_style_set_bg_color(&bluetooth_FindPhone_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&bluetooth_FindPhone_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width(&bluetooth_FindPhone_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font(&bluetooth_FindPhone_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_style_set_text_color(&bluetooth_FindPhone_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_add_style(bluetooth_FindPhone_tile, LV_OBJ_PART_MAIN, &bluetooth_FindPhone_style);

    lv_style_copy(&bluetooth_FindPhone_exit_btn_style, mainbar_get_style());
    lv_style_set_bg_color(&bluetooth_FindPhone_exit_btn_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_bg_opa(&bluetooth_FindPhone_exit_btn_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width(&bluetooth_FindPhone_exit_btn_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font(&bluetooth_FindPhone_exit_btn_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_style_set_text_color(&bluetooth_FindPhone_exit_btn_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_add_style(bluetooth_FindPhone_tile, LV_OBJ_PART_MAIN, &bluetooth_FindPhone_style);

    bluetooth_FindPhone_img = lv_img_create(bluetooth_FindPhone_tile, NULL);
    lv_img_set_src(bluetooth_FindPhone_img, &eye_200px);
    lv_obj_align(bluetooth_FindPhone_img, bluetooth_FindPhone_tile, LV_ALIGN_CENTER, 0, 0);
    bluetooth_FindPhone_label=lv_label_create(bluetooth_FindPhone_tile,NULL);
    lv_obj_add_style(bluetooth_FindPhone_label, LV_OBJ_PART_MAIN, &bluetooth_FindPhone_style);
    lv_obj_align(bluetooth_FindPhone_label, bluetooth_FindPhone_tile, LV_ALIGN_IN_LEFT_MID, 5, 0);

    lv_obj_t *exit_btn = lv_imgbtn_create(bluetooth_FindPhone_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &cancel_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &cancel_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &cancel_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &cancel_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &bluetooth_FindPhone_style);
    lv_obj_align(exit_btn, bluetooth_FindPhone_tile, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
    lv_obj_set_event_cb(exit_btn, exit_bluetooth_FindPhone_event_cb);

    blectl_register_cb(BLECTL_MSG, bluetooth_FindPhone_event_cb, "bluetooth_FindPhone");
}
static void FindPhone_search_task( lv_task_t * task );

void FindPhone_main_setup( uint32_t tile_num ) {

    FindPhone_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &FindPhone_main_style, mainbar_get_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( FindPhone_main_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px );
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px );
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px );
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px );
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &bluetooth_FindPhone_exit_btn_style );
    lv_obj_align( exit_btn, FindPhone_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_FindPhone_main_event_cb );

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
		REM(false);
		blectl_send_msg( (char*)"\r\n{t:\"findPhone\", n:\"false\"}\r\n" );
		searching = false;
	}else {
	    _FindPhone_search_task = lv_task_create( FindPhone_search_task, 1000, LV_TASK_PRIO_MID, NULL );
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

static void FindPhone_search_task( lv_task_t * task )
{ 
	if (searching) 
	{
		blectl_send_msg( (char*)"\r\n{t:\"findPhone\", n:\"true\"}\r\n" );
		REM(true);
	} else {
		blectl_send_msg( (char*)"\r\n{t:\"findPhone\", n:\"false\"}\r\n" );
	}
    lv_task_del( _FindPhone_search_task );
}
bool bluetooth_FindPhone_event_cb(EventBits_t event, void *arg)
{
    switch (event)
    {
    case BLECTL_MSG:
        bluetooth_FindPhone_msg_pharse((const char *)arg);
        break;
    }
    return (true);
}

static void exit_bluetooth_FindPhone_event_cb(lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        mainbar_jump_to_maintile(LV_ANIM_OFF);
        break;
    }
}

void bluetooth_FindPhone_msg_pharse(const char *msg)
{
    SpiRamJsonDocument doc(strlen(msg) * 4);
    DeserializationError error = deserializeJson(doc, msg);
    if (error)
    {
        log_e("bluetooth FindPhone deserializeJson() failed: %s", error.c_str());
    }
    else
    {
        if ( !strcmp( doc["t"], "find" )  )
		{
            statusbar_hide(true);
            powermgm_get_event(POWERMGM_STANDBY);          
            powermgm_set_event(POWERMGM_WAKEUP_REQUEST);
            mainbar_jump_to_tilenumber(bluetooth_FindPhone_tile_num, LV_ANIM_OFF);
            lv_label_set_text(bluetooth_FindPhone_label, "Looking for me?");
            sound_play_progmem_wav( piep_wav, piep_wav_len );
            lv_obj_invalidate(lv_scr_act());
            motor_vibe(100);            
        }
										
    }
    doc.clear();
}