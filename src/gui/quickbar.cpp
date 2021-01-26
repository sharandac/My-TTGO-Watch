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
#include "gui/mainbar/setup_tile/setup_tile.h"

#include "hardware/pmu.h"
#include "hardware/powermgm.h"
#include "hardware/motor.h"

static bool quickbar_init = false;

static lv_obj_t *quickbar = NULL;
static lv_obj_t *quickbar_maintile_img = NULL;
static lv_obj_t *quickbar_setup_img = NULL;
static lv_obj_t *quickbar_screenshot_img = NULL;

static lv_anim_t quickbar_maintile_anim;
static lv_anim_t quickbar_setup_anim;
static lv_anim_t quickbar_screenshot_anim;

static lv_style_t quickbarstyle[ QUICKBAR_STYLE_NUM ];

LV_IMG_DECLARE(maintile_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(camera_32px);

lv_task_t * quickbar_task;
static uint32_t quickbar_counter = 0;

void quickbar_maintile_event_cb( lv_obj_t *bluetooth, lv_event_t event );
void quickbar_setup_event_cb( lv_obj_t *bluetooth, lv_event_t event );
void quickbar_screenshot_event_cb( lv_obj_t *bluetooth, lv_event_t event );
bool quickbar_pmuctl_event_cb( EventBits_t event, void *arg );
bool quickbar_powermgm_event_cb( EventBits_t event, void *arg );
void quickbar_counter_task( lv_task_t * task );

void quickbar_setup( void ){
    /*
     * check if quickbar already initialized
     */
    if ( quickbar_init ) {
        log_e("quickbar already initialized");
        return;
    }

    /*Copy a built-in style to initialize the new style*/
    lv_style_init( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ] );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_bg_opa( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_OPA_80 );
    lv_style_set_border_width( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_text_color( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, 16 );
    lv_style_set_image_recolor( &quickbarstyle[ QUICKBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );

    lv_style_init( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ] );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_bg_opa( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, LV_OPA_0 );
    lv_style_set_border_width( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_text_color( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_OBJ_PART_MAIN, 16 );
    lv_style_set_image_recolor( &quickbarstyle[ QUICKBAR_STYLE_LIGHT ], LV_IMGBTN_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_image_recolor_opa( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_IMGBTN_PART_MAIN, LV_OPA_COVER );

    lv_style_init( &quickbarstyle[ QUICKBAR_STYLE_DARK ] );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, LV_OPA_0 );
    lv_style_set_border_width( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_OBJ_PART_MAIN, 16 );
    lv_style_set_image_recolor( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_IMGBTN_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_image_recolor_opa( &quickbarstyle[ QUICKBAR_STYLE_DARK ], LV_IMGBTN_PART_MAIN, LV_OPA_COVER );

    lv_style_init( &quickbarstyle[ QUICKBAR_STYLE_TRANS ] );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_TRANS ], LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &quickbarstyle[ QUICKBAR_STYLE_TRANS ], LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &quickbarstyle[ QUICKBAR_STYLE_TRANS ], LV_OBJ_PART_MAIN, LV_OPA_TRANSP );
    lv_style_set_border_width( &quickbarstyle[ QUICKBAR_STYLE_TRANS ], LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &quickbarstyle[ QUICKBAR_STYLE_TRANS ], LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_radius( &quickbarstyle[ QUICKBAR_STYLE_TRANS ], LV_OBJ_PART_MAIN, 16 );

    quickbar = lv_cont_create( lv_scr_act(), NULL );
    lv_obj_set_width( quickbar, 48 * 3 );
    lv_obj_set_height( quickbar, 48 );
    lv_obj_reset_style_list( quickbar, LV_OBJ_PART_MAIN );
    lv_obj_add_style( quickbar, LV_OBJ_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_NORMAL ] );
    lv_obj_align( quickbar, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0 );

    lv_obj_t *quickbar_maintile = lv_btn_create( quickbar, NULL );
    lv_obj_set_width( quickbar_maintile, 48 );
    lv_obj_set_height( quickbar_maintile, 48 );
    lv_obj_add_protect( quickbar_maintile, LV_PROTECT_CLICK_FOCUS );
    lv_obj_add_style( quickbar_maintile, LV_BTN_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_TRANS ] );
    lv_obj_align( quickbar_maintile, quickbar, LV_ALIGN_IN_LEFT_MID, 8, 0 );
    lv_obj_set_event_cb( quickbar_maintile, quickbar_maintile_event_cb );
    quickbar_maintile_img = lv_img_create( quickbar_maintile, NULL );
    lv_img_set_src( quickbar_maintile_img, &maintile_32px );
    lv_obj_add_style( quickbar_maintile_img, LV_IMG_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_DARK ] );
    lv_obj_align( quickbar_maintile_img, quickbar_maintile, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_click( quickbar_maintile_img, false );

    lv_obj_t *quickbar_setup = lv_btn_create( quickbar, NULL );
    lv_obj_set_width( quickbar_setup, 48 );
    lv_obj_set_height( quickbar_setup, 48 );
    lv_obj_add_protect( quickbar_setup, LV_PROTECT_CLICK_FOCUS );
    lv_obj_add_style( quickbar_setup, LV_BTN_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_TRANS ] );
    lv_obj_align( quickbar_setup, quickbar, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_event_cb( quickbar_setup, quickbar_setup_event_cb );
    quickbar_setup_img = lv_img_create( quickbar_setup, NULL );
    lv_img_set_src( quickbar_setup_img, &setup_32px );
    lv_obj_add_style( quickbar_setup_img, LV_IMG_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_DARK ] );
    lv_obj_align( quickbar_setup_img, quickbar_setup, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_click( quickbar_setup_img, false );

    lv_obj_t *quickbar_screenshot = lv_btn_create( quickbar, NULL );
    lv_obj_set_width( quickbar_screenshot, 48 );
    lv_obj_set_height( quickbar_screenshot, 48 );
    lv_obj_add_protect( quickbar_screenshot, LV_PROTECT_CLICK_FOCUS );
    lv_obj_add_style( quickbar_screenshot, LV_BTN_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_TRANS ] );
    lv_obj_align( quickbar_screenshot, quickbar, LV_ALIGN_IN_RIGHT_MID, -8, 0 );
    lv_obj_set_event_cb( quickbar_screenshot, quickbar_screenshot_event_cb );
    quickbar_screenshot_img = lv_img_create( quickbar_screenshot, NULL );
    lv_img_set_src( quickbar_screenshot_img, &camera_32px );
    lv_obj_add_style( quickbar_screenshot_img, LV_IMG_PART_MAIN, &quickbarstyle[ QUICKBAR_STYLE_DARK ] );
    lv_obj_align( quickbar_screenshot_img, quickbar_screenshot, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_click( quickbar_screenshot_img, false );

	lv_anim_init( &quickbar_maintile_anim );
	lv_anim_set_exec_cb( &quickbar_maintile_anim, (lv_anim_exec_xcb_t)lv_img_set_zoom );
	lv_anim_set_time( &quickbar_maintile_anim, 300 );

	lv_anim_init( &quickbar_setup_anim );
	lv_anim_set_exec_cb( &quickbar_setup_anim, (lv_anim_exec_xcb_t)lv_img_set_zoom );
	lv_anim_set_time( &quickbar_setup_anim, 300 );

	lv_anim_init( &quickbar_screenshot_anim );
	lv_anim_set_exec_cb( &quickbar_screenshot_anim, (lv_anim_exec_xcb_t)lv_img_set_zoom );
	lv_anim_set_time( &quickbar_screenshot_anim, 300 );

    /*
     * quickbar init complete
     */
    quickbar_init = true;

    quickbar_hide( true );

    pmu_register_cb( PMUCTL_LONG_PRESS, quickbar_pmuctl_event_cb, "quickbar pmu event");
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, quickbar_powermgm_event_cb, "quickbar powermgm event" );

    return;
}

bool quickbar_powermgm_event_cb( EventBits_t event, void *arg ) {
    /*
     * check if quickar already initialized
     */
    if ( !quickbar_init ) {
        log_e("quickbar not initialized");
        return( true );
    }

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
    /*
     * check if quickar already initialized
     */
    if ( !quickbar_init ) {
        log_e("quickbar not initialized");
        return( true );
    }
    
    bool retval = true;
    
    switch ( event ) {
        case PMUCTL_LONG_PRESS:
            motor_vibe(3);
            lv_disp_trig_activity( NULL );
            if( lv_obj_get_hidden( quickbar ) ) {
                quickbar_hide( false );
            }
            else {
                quickbar_hide( true );
            }
            break;
    }
    return( retval );
}

void quickbar_hide( bool hide ) {
    /*
     * check if quickar already initialized
     */
    if ( !quickbar_init ) {
        log_e("quickbar not initialized");
        return;
    }

    if ( hide ) {
        lv_obj_set_hidden( quickbar, hide );
        lv_obj_invalidate( lv_scr_act() );

        lv_anim_set_var( &quickbar_maintile_anim, quickbar_maintile_img );
        lv_anim_set_values( &quickbar_maintile_anim, 256, 1 );
        lv_anim_start( &quickbar_maintile_anim );

        lv_anim_set_var( &quickbar_setup_anim, quickbar_setup_img );
        lv_anim_set_values( &quickbar_setup_anim, 256, 1 );
        lv_anim_set_delay( &quickbar_setup_anim, 100 );
        lv_anim_start( &quickbar_setup_anim );

        lv_anim_set_var( &quickbar_screenshot_anim, quickbar_screenshot_img );
        lv_anim_set_values( &quickbar_screenshot_anim, 256, 1 );
        lv_anim_set_delay( &quickbar_screenshot_anim, 200 );
        lv_anim_start( &quickbar_screenshot_anim );
    }
    else {
        lv_obj_set_hidden( quickbar, hide );
        lv_obj_invalidate( lv_scr_act() );

        lv_anim_set_var( &quickbar_maintile_anim, quickbar_maintile_img );
        lv_anim_set_values( &quickbar_maintile_anim, 1, 256 );
        lv_anim_start( &quickbar_maintile_anim );

        lv_anim_set_var( &quickbar_setup_anim, quickbar_setup_img );
        lv_anim_set_values( &quickbar_setup_anim, 1, 256 );
        lv_anim_set_delay( &quickbar_setup_anim, 100 );
        lv_anim_start( &quickbar_setup_anim );

        lv_anim_set_var( &quickbar_screenshot_anim, quickbar_screenshot_img );
        lv_anim_set_values( &quickbar_screenshot_anim, 1, 256 );
        lv_anim_set_delay( &quickbar_screenshot_anim, 200 );
        lv_anim_start( &quickbar_screenshot_anim );
    }
}

void quickbar_maintile_event_cb( lv_obj_t *bluetooth, lv_event_t event ) {
    /*
     * check if quickar already initialized
     */
    if ( !quickbar_init ) {
        log_e("quickbar not initialized");
        return;
    }
    
    switch ( event ) {
        case ( LV_EVENT_CLICKED ):
            quickbar_hide( true );
            mainbar_jump_to_maintile( LV_ANIM_OFF );
            break;
    }
}

void quickbar_setup_event_cb( lv_obj_t *bluetooth, lv_event_t event ) {
    /*
     * check if quickar already initialized
     */
    if ( !quickbar_init ) {
        log_e("quickbar not initialized");
        return;
    }
    
    switch ( event ) {
        case ( LV_EVENT_CLICKED ):
            quickbar_hide( true );
            mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
            break;
    }
}

void quickbar_screenshot_event_cb( lv_obj_t *bluetooth, lv_event_t event ) {
    /*
     * check if quickar already initialized
     */
    if ( !quickbar_init ) {
        log_e("quickbar not initialized");
        return;
    }
    
    switch ( event ) {
        case ( LV_EVENT_CLICKED ):
            quickbar_hide( true );
            lv_disp_trig_activity( NULL );
            lv_task_handler();
            quickbar_counter = 3;
            quickbar_task = lv_task_create( quickbar_counter_task, 1000, LV_TASK_PRIO_MID, NULL );
            break;
    }
}

void quickbar_counter_task( lv_task_t * task ) {
    /*
     * check if quickar already initialized
     */
    if ( !quickbar_init ) {
        log_e("quickbar not initialized");
        return;
    }
    
    quickbar_counter--;
    if ( quickbar_counter == 0 ) {
        screenshot_take();
        screenshot_save();
        lv_task_del( quickbar_task );
    }
}