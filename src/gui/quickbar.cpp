/****************************************************************************
 *   Su Jan 17 23:05:51 2021
 *   Copyright  2021  Dirk Brosswick
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

#include "quickbar.h"
#include "screenshot.h"
#include "gui/mainbar/mainbar.h"

#include "hardware/pmu.h"
#include "hardware/powermgm.h"
#include "hardware/motor.h"

static lv_obj_t *quickbar = NULL;
static lv_style_t quickbarstyle[ QUICKBAR_STYLE_NUM ];

LV_IMG_DECLARE(maintile_32px);
LV_IMG_DECLARE(power_32px);
LV_IMG_DECLARE(camera_32px);

lv_task_t * quickbar_task;
static uint32_t quickbar_counter = 0;

void quickbar_maintile_event_cb( lv_obj_t *bluetooth, lv_event_t event );
void quickbar_power_event_cb( lv_obj_t *bluetooth, lv_event_t event );
void quickbar_screenshot_event_cb( lv_obj_t *bluetooth, lv_event_t event );
bool quickbar_pmuctl_event_cb( EventBits_t event, void *arg );
bool quickbar_powermgm_event_cb( EventBits_t event, void *arg );
void quickbar_counter_task( lv_task_t * task );

void quickbar_setup( void ){

    /*Copy a built-in style to initialize the new style*/
    lv_style_init( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ] );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_bg_opa( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_OPA_80 );
    lv_style_set_border_width( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_text_color( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, 5 );
    lv_style_set_image_recolor( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );

    lv_style_init( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ] );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_bg_opa( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, LV_OPA_0 );
    lv_style_set_border_width( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_text_color( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, 16 );
    lv_style_set_image_recolor( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );

    lv_style_init( &quickbarstyle[ QUICKBAR_STYLE_DARK ] );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, LV_OPA_0 );
    lv_style_set_border_width( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, 16 );
    lv_style_set_image_recolor( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, LV_COLOR_BLACK );

    quickbar = lv_cont_create( lv_scr_act(), NULL );
    lv_obj_set_width( quickbar, 48 * 2 );
    lv_obj_set_height( quickbar, 48 );
    lv_obj_reset_style_list( quickbar, LV_OBJ_PART_MAIN );
    lv_obj_add_style( quickbar, LV_OBJ_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_NORMAL ] );
    lv_obj_align( quickbar, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    
    lv_obj_t *quickbar_maintile = lv_imgbtn_create( quickbar, NULL);
    lv_imgbtn_set_src( quickbar_maintile, LV_BTN_STATE_RELEASED, &maintile_32px );
    lv_imgbtn_set_src( quickbar_maintile, LV_BTN_STATE_PRESSED, &maintile_32px );
    lv_imgbtn_set_src( quickbar_maintile, LV_BTN_STATE_CHECKED_RELEASED, &maintile_32px );
    lv_imgbtn_set_src( quickbar_maintile, LV_BTN_STATE_CHECKED_PRESSED, &maintile_32px );
    lv_imgbtn_set_checkable (quickbar_maintile, true );
    lv_obj_add_style( quickbar_maintile, LV_IMGBTN_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_LIGHT ] );
    lv_obj_align( quickbar_maintile, quickbar, LV_ALIGN_IN_LEFT_MID, 8, 0 );
    lv_obj_set_event_cb( quickbar_maintile, quickbar_maintile_event_cb );
    lv_imgbtn_set_state( quickbar_maintile, LV_BTN_STATE_CHECKED_PRESSED );
/*
    lv_obj_t *quickbar_power = lv_imgbtn_create( quickbar, NULL);
    lv_imgbtn_set_src( quickbar_power, LV_BTN_STATE_RELEASED, &power_32px );
    lv_imgbtn_set_src( quickbar_power, LV_BTN_STATE_PRESSED, &power_32px );
    lv_imgbtn_set_src( quickbar_power, LV_BTN_STATE_CHECKED_RELEASED, &power_32px );
    lv_imgbtn_set_src( quickbar_power, LV_BTN_STATE_CHECKED_PRESSED, &power_32px );
    lv_imgbtn_set_checkable (quickbar_power, true );
    lv_obj_add_style( quickbar_power, LV_IMGBTN_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_LIGHT ] );
    lv_obj_align( quickbar_power, quickbar, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_event_cb( quickbar_power, quickbar_power_event_cb );
    lv_imgbtn_set_state( quickbar_power, LV_BTN_STATE_CHECKED_PRESSED );
*/
    lv_obj_t *quickbar_screenshot = lv_imgbtn_create( quickbar, NULL);
    lv_imgbtn_set_src( quickbar_screenshot, LV_BTN_STATE_RELEASED, &camera_32px );
    lv_imgbtn_set_src( quickbar_screenshot, LV_BTN_STATE_PRESSED, &camera_32px );
    lv_imgbtn_set_src( quickbar_screenshot, LV_BTN_STATE_CHECKED_RELEASED, &camera_32px );
    lv_imgbtn_set_src( quickbar_screenshot, LV_BTN_STATE_CHECKED_PRESSED, &camera_32px );
    lv_imgbtn_set_checkable(quickbar_screenshot, true );
    lv_obj_add_style( quickbar_screenshot, LV_IMGBTN_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_LIGHT ] );
    lv_obj_align( quickbar_screenshot, quickbar, LV_ALIGN_IN_RIGHT_MID, -8, 0 );
    lv_obj_set_event_cb( quickbar_screenshot, quickbar_screenshot_event_cb );
    lv_imgbtn_set_state( quickbar_screenshot, LV_BTN_STATE_CHECKED_PRESSED );

    pmu_register_cb( PMUCTL_LONG_PRESS, quickbar_pmuctl_event_cb, "quickbar pmu event");
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, quickbar_powermgm_event_cb, "quickbar powermgm event" );

    quickbar_hide( true );

    return;
}

bool quickbar_powermgm_event_cb( EventBits_t event, void *arg ) {
    bool retval = true;
    
    switch (event) {
        case POWERMGM_WAKEUP:
            quickbar_hide( true );
            break;
        case POWERMGM_STANDBY:
            quickbar_hide( true );
            break;
        case POWERMGM_SILENCE_WAKEUP:
            quickbar_hide( true );
            break;
    }
    return( retval );
}

bool quickbar_pmuctl_event_cb( EventBits_t event, void *arg ) {
    bool retval = true;
    
    switch ( event ) {
        case PMUCTL_LONG_PRESS:
            motor_vibe(3);
            quickbar_hide( false );
            break;
    }
    return( retval );
}

void quickbar_hide( bool hide ) {
    lv_obj_set_hidden( quickbar, hide );
    lv_obj_invalidate( lv_scr_act() );
}

void quickbar_maintile_event_cb( lv_obj_t *bluetooth, lv_event_t event ) {
    switch ( event ) {
        case ( LV_EVENT_VALUE_CHANGED ):
            quickbar_hide( true );
            mainbar_jump_to_maintile( LV_ANIM_OFF );
            break;
    }
}

void quickbar_power_event_cb( lv_obj_t *bluetooth, lv_event_t event ) {
    switch ( event ) {
        case ( LV_EVENT_VALUE_CHANGED ):
            quickbar_hide( true );
            break;
    }
}

void quickbar_screenshot_event_cb( lv_obj_t *bluetooth, lv_event_t event ) {
    switch ( event ) {
        case ( LV_EVENT_VALUE_CHANGED ):
            quickbar_hide( true );
            lv_disp_trig_activity( NULL );
            lv_task_handler();
            quickbar_counter = 3;
            quickbar_task = lv_task_create( quickbar_counter_task, 1000, LV_TASK_PRIO_MID, NULL );
            break;
    }
}

void quickbar_counter_task( lv_task_t * task ) {
    quickbar_counter--;
    if ( quickbar_counter == 0 ) {
        screenshot_take();
        screenshot_save();
        lv_task_del( quickbar_task );
    }
}