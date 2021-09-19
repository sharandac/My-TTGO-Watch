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

#include "FindPhone.h"
#include "FindPhone_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/sound/piep.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/blectl.h"
#include "hardware/sound.h"
#include "hardware/motor.h"
#include "hardware/powermgm.h"

#include "utils/bluejsonrequest.h"

uint32_t bluetooth_FindPhone_tile_num;

lv_obj_t *FindPhone_main_tile = NULL;
lv_obj_t *FindPhone_main_iris = NULL;
lv_obj_t *bluetooth_FindPhone_tile = NULL;
lv_obj_t *bluetooth_FindPhone_img = NULL;
lv_obj_t *bluetooth_FindPhone_label = NULL;										   

lv_style_t bluetooth_FindPhone_style;

lv_task_t * _FindPhone_PhoneSearch_task; 
lv_task_t * _FindPhone_WatchFind_task = nullptr;

LV_IMG_DECLARE(eye_200px);
LV_IMG_DECLARE(eye_lid_closed);
LV_IMG_DECLARE(eye_lid_open);
LV_IMG_DECLARE(eye_iris);
LV_FONT_DECLARE(Ubuntu_32px);

int rem_iris_x = 0;
int rem_iris_y = 0;
static bool searching_phone = false;

static void exit_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void go_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_bluetooth_FindPhone_event_cb(lv_obj_t *obj, lv_event_t event);
bool bluetooth_FindPhone_event_cb(EventBits_t event, void *arg);
static void bluetooth_FindPhone_msg_pharse(BluetoothJsonRequest &doc);
static void FindPhone_PhoneSearch_task( lv_task_t * task );
static void FindPhone_WatchFind_task( lv_task_t * task );

void bluetooth_FindPhone_tile_setup(void)
{
    // get an app tile and copy mainstyle
    bluetooth_FindPhone_tile_num = mainbar_add_app_tile(1, 1, "bluetooth FindPhone");
    bluetooth_FindPhone_tile = mainbar_get_tile_obj(bluetooth_FindPhone_tile_num);

    lv_style_copy(&bluetooth_FindPhone_style, ws_get_mainbar_style());
    lv_style_set_bg_color(&bluetooth_FindPhone_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&bluetooth_FindPhone_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width(&bluetooth_FindPhone_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font(&bluetooth_FindPhone_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_style_set_text_color(&bluetooth_FindPhone_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_add_style(bluetooth_FindPhone_tile, LV_OBJ_PART_MAIN, &bluetooth_FindPhone_style);

    bluetooth_FindPhone_img = lv_img_create(bluetooth_FindPhone_tile, NULL);
    lv_img_set_src(bluetooth_FindPhone_img, &eye_200px);
    lv_obj_align(bluetooth_FindPhone_img, bluetooth_FindPhone_tile, LV_ALIGN_CENTER, 0, 0);
    bluetooth_FindPhone_label=lv_label_create(bluetooth_FindPhone_tile,NULL);
    lv_obj_add_style(bluetooth_FindPhone_label, LV_OBJ_PART_MAIN, &bluetooth_FindPhone_style);
    lv_obj_align(bluetooth_FindPhone_label, bluetooth_FindPhone_tile, LV_ALIGN_IN_LEFT_MID, 5, 0);

    lv_obj_t *header = wf_add_settings_header( bluetooth_FindPhone_tile, NULL, exit_bluetooth_FindPhone_event_cb );
    lv_obj_align(header, bluetooth_FindPhone_tile, LV_ALIGN_IN_TOP_RIGHT, -10, 10);

    blectl_register_cb(BLECTL_MSG_JSON, bluetooth_FindPhone_event_cb, "bluetooth_FindPhone");
}

void FindPhone_main_setup( uint32_t tile_num ) {

    FindPhone_main_tile = mainbar_get_tile_obj( tile_num );

    lv_obj_t * exit_btn = wf_add_exit_button( FindPhone_main_tile, exit_FindPhone_main_event_cb );
    lv_obj_align( exit_btn, FindPhone_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    // eye toggle button
	lv_obj_t *FindPhone_main_go_btn = NULL;
    FindPhone_main_go_btn = wf_add_image_button( FindPhone_main_tile, eye_lid_open, go_FindPhone_main_event_cb );
	lv_btn_set_checkable(FindPhone_main_go_btn, true);
    lv_btn_toggle(FindPhone_main_go_btn);
    lv_obj_align( FindPhone_main_go_btn, NULL, LV_ALIGN_CENTER, 0, 0 );
	
	// iris 
	FindPhone_main_iris = lv_img_create( FindPhone_main_tile, NULL);
	lv_img_set_src (FindPhone_main_iris,&eye_iris);
	lv_obj_set_hidden(FindPhone_main_iris,true);
    lv_obj_align( FindPhone_main_iris, NULL, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_t *app_label = lv_label_create( FindPhone_main_tile, NULL);
    lv_label_set_text( app_label, "Find\nPhone");
    lv_obj_reset_style_list( app_label, LV_OBJ_PART_MAIN );
    lv_obj_add_style( app_label, LV_OBJ_PART_MAIN, APP_STYLE );
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
	if (searching_phone) 
	{
		REM(false);
		blectl_send_msg( (char*)"\r\n{t:\"findPhone\", n:\"false\"}\r\n" );
		searching_phone = false;
	}else {
	    _FindPhone_PhoneSearch_task = lv_task_create( FindPhone_PhoneSearch_task, 1000, LV_TASK_PRIO_MID, NULL );
		searching_phone = true;
	}		
}

static void exit_FindPhone_main_event_cb( lv_obj_t * obj, lv_event_t event ) 
{
    switch( event ) {
        case( LV_EVENT_CLICKED ):       
                                        mainbar_jump_back();
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

static void FindPhone_PhoneSearch_task( lv_task_t * task )
{ 
	if (searching_phone) 
	{
		blectl_send_msg( (char*)"\r\n{t:\"findPhone\", n:\"true\"}\r\n" );
		REM(true);
	} else {
		blectl_send_msg( (char*)"\r\n{t:\"findPhone\", n:\"false\"}\r\n" );
	}
    lv_task_del( _FindPhone_PhoneSearch_task );
}

static void FindPhone_WatchFind_task( lv_task_t * task )
{
    sound_play_progmem_wav( piep_wav, piep_wav_len ); 
	motor_vibe(100); 
}

bool bluetooth_FindPhone_event_cb(EventBits_t event, void *arg)
{
    switch (event)
    {
    case BLECTL_MSG_JSON:
        bluetooth_FindPhone_msg_pharse(*(BluetoothJsonRequest *)arg);
        break;
    }
    return (true);
}

static void exit_bluetooth_FindPhone_event_cb(lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        mainbar_jump_back();
        break;
    }
}

static void bluetooth_FindPhone_msg_pharse(BluetoothJsonRequest &doc)
{    
    if ( doc.isEqualKeyValue("t", "find") && doc.isEqualKeyValue("n", true) )
    {
        log_i("FindPhone screen active");
        powermgm_get_event(POWERMGM_STANDBY);          
        powermgm_set_event(POWERMGM_WAKEUP_REQUEST);
        mainbar_jump_to_tilenumber(bluetooth_FindPhone_tile_num, LV_ANIM_OFF);
        statusbar_hide(true);
        lv_label_set_text(bluetooth_FindPhone_label, "Looking for me?");
        lv_obj_invalidate(lv_scr_act());
        _FindPhone_WatchFind_task = lv_task_create( FindPhone_WatchFind_task, 1500, LV_TASK_PRIO_MID, NULL );           
    }

    if ( doc.isEqualKeyValue("t", "find") && doc.isEqualKeyValue("n", false) )
    {
        log_i("FindPhone screen closed");
        if( _FindPhone_WatchFind_task!=nullptr)
        {
            lv_task_del( _FindPhone_WatchFind_task );
            _FindPhone_WatchFind_task = nullptr;
        }            
        mainbar_jump_back();          
    }
}