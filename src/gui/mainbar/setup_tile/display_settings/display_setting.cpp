/****************************************************************************
 *   Tu May 22 21:23:51 2020
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
#include "display_settings.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/gui.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/display.h"
#include "hardware/motion.h"


icon_t *display_setup_icon = NULL;

lv_obj_t *display_settings_tile_1 = NULL;
lv_obj_t *display_settings_tile_2 = NULL;
uint32_t display_tile_num_1;
uint32_t display_tile_num_2;

lv_obj_t *display_brightness_slider = NULL;
lv_obj_t *display_timeout_slider = NULL;
lv_obj_t *display_timeout_slider_label = NULL;
lv_obj_t *display_rotation_list = NULL;
lv_obj_t *display_bg_img_list = NULL;
lv_obj_t *display_vibe_onoff = NULL;
lv_obj_t *display_block_return_maintile_onoff = NULL;
lv_obj_t *display_use_dma_cont_onoff = NULL;
lv_obj_t *display_background_image = NULL;

LV_IMG_DECLARE(brightness_64px);
LV_IMG_DECLARE(up_32px);
LV_IMG_DECLARE(down_32px);
LV_IMG_DECLARE(brightness_32px);
LV_IMG_DECLARE(time_32px);
LV_IMG_DECLARE(info_update_16px);

static void enter_display_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_display_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void down_display_setup_event_cb( lv_obj_t * obj, lv_event_t event );
bool display_displayctl_brightness_event_cb( EventBits_t event, void *arg );
static void display_brightness_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void display_timeout_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void display_rotation_event_handler(lv_obj_t * obj, lv_event_t event);
static void display_vibe_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void display_block_return_maintile_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void display_background_image_setup_event_cb( lv_obj_t * obj, lv_event_t event );

void display_settings_tile_setup( void ) {
    lv_obj_t *header_1 = NULL;
    lv_obj_t *header_2 = NULL;
    // get an app tile and copy mainstyle

    #if RES_Y_MAX < 340
        display_tile_num_1 = mainbar_add_setup_tile( 1, 2, "display settings" );
        display_tile_num_2 = display_tile_num_1 + 1;
        display_settings_tile_1 = mainbar_get_tile_obj( display_tile_num_1 );
        display_settings_tile_2 = mainbar_get_tile_obj( display_tile_num_2 );
        lv_obj_add_style( display_settings_tile_1, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );
        lv_obj_add_style( display_settings_tile_2, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );
    #else
        display_tile_num_1 = mainbar_add_app_tile( 1, 1, "display settings" );
        display_settings_tile_1 = mainbar_get_tile_obj( display_tile_num_1 );
        display_settings_tile_2 = NULL;
        lv_obj_add_style( display_settings_tile_1, LV_OBJ_PART_MAIN, ws_get_setup_tile_style() );
    #endif

    display_setup_icon = setup_register( "display", &brightness_64px, enter_display_setup_event_cb );
    setup_hide_indicator( display_setup_icon );

    header_1 = wf_add_settings_header( display_settings_tile_1, "display settings", exit_display_setup_event_cb );
    lv_obj_align( header_1, display_settings_tile_1, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );

    /**
     * add page 2 header if a tile 2 exist
     */
    if ( display_settings_tile_2 ) {
        header_2 = wf_add_settings_header( display_settings_tile_2, "display settings", exit_display_setup_event_cb );
        lv_obj_align( header_2, display_settings_tile_2, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );

        lv_obj_t *down_btn_1 = wf_add_down_button( display_settings_tile_1, down_display_setup_event_cb, SETUP_STYLE );
        lv_obj_align( down_btn_1, display_settings_tile_1, LV_ALIGN_IN_TOP_RIGHT, -10, STATUSBAR_HEIGHT + 10 );
    }

    lv_obj_t *brightness_cont = lv_obj_create( display_settings_tile_1, NULL );
    lv_obj_set_size( brightness_cont, lv_disp_get_hor_res( NULL ) , 48 );
    lv_obj_add_style( brightness_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( brightness_cont, header_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 8 );
    display_brightness_slider = lv_slider_create( brightness_cont, NULL );
    lv_obj_add_protect( display_brightness_slider, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( display_brightness_slider, LV_SLIDER_PART_INDIC, ws_get_slider_style() );
    lv_obj_add_style( display_brightness_slider, LV_SLIDER_PART_KNOB, ws_get_slider_style() );
    lv_slider_set_range( display_brightness_slider, DISPLAY_MIN_BRIGHTNESS, DISPLAY_MAX_BRIGHTNESS );
    lv_obj_set_size( display_brightness_slider, lv_disp_get_hor_res( NULL ) - 100 , 10 );
    lv_obj_align( display_brightness_slider, brightness_cont, LV_ALIGN_IN_RIGHT_MID, -30, 0 );
    lv_obj_set_event_cb( display_brightness_slider, display_brightness_setup_event_cb );
    lv_obj_t *brightness_icon = lv_img_create( brightness_cont, NULL );
    lv_img_set_src( brightness_icon, &brightness_32px );
    lv_obj_align( brightness_icon, brightness_cont, LV_ALIGN_IN_LEFT_MID, 15, 0 );

    lv_obj_t *timeout_cont = lv_obj_create( display_settings_tile_1, NULL );
    lv_obj_set_size( timeout_cont, lv_disp_get_hor_res( NULL ) , 58 );
    lv_obj_add_style( timeout_cont, LV_OBJ_PART_MAIN, SETUP_STYLE  );
    lv_obj_align( timeout_cont, brightness_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    display_timeout_slider = lv_slider_create( timeout_cont, NULL );
    lv_obj_add_protect( display_timeout_slider, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( display_timeout_slider, LV_SLIDER_PART_INDIC, ws_get_slider_style() );
    lv_obj_add_style( display_timeout_slider, LV_SLIDER_PART_KNOB, ws_get_slider_style() );
    lv_slider_set_range( display_timeout_slider, DISPLAY_MIN_TIMEOUT, DISPLAY_MAX_TIMEOUT );
    lv_obj_set_size(display_timeout_slider, lv_disp_get_hor_res( NULL ) - 100 , 10 );
    lv_obj_align( display_timeout_slider, timeout_cont, LV_ALIGN_IN_TOP_RIGHT, -30, 10 );
    lv_obj_set_event_cb( display_timeout_slider, display_timeout_setup_event_cb );
    display_timeout_slider_label = lv_label_create( timeout_cont, NULL );
    lv_obj_add_style( display_timeout_slider_label, LV_OBJ_PART_MAIN, SETUP_STYLE );
    lv_label_set_text( display_timeout_slider_label, "");
    lv_obj_align( display_timeout_slider_label, display_timeout_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, -5 );
    lv_obj_t *timeout_icon = lv_img_create( timeout_cont, NULL );
    lv_img_set_src( timeout_icon, &time_32px );
    lv_obj_align( timeout_icon, timeout_cont, LV_ALIGN_IN_LEFT_MID, 15, 0 );

    lv_obj_t *rotation_cont = wf_add_labeled_list( display_settings_tile_1, "rotation", &display_rotation_list, "0°\n90°\n180°\n270°", display_rotation_event_handler, SETUP_STYLE );
    lv_obj_align( rotation_cont, timeout_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );

    lv_obj_t *vibe_cont = wf_add_labeled_switch( display_settings_tile_2?display_settings_tile_2:display_settings_tile_1, "vibe feedback", &display_vibe_onoff, display_get_vibe(), display_vibe_setup_event_cb, SETUP_STYLE );
    lv_obj_align( vibe_cont, display_settings_tile_2?header_2:rotation_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 8 );

    lv_obj_t *block_return_maintile_cont = wf_add_labeled_switch( display_settings_tile_2?display_settings_tile_2:display_settings_tile_1, "block maintile", &display_block_return_maintile_onoff, display_get_block_return_maintile(), display_block_return_maintile_setup_event_cb, SETUP_STYLE );
    lv_obj_align( block_return_maintile_cont, vibe_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 8 );

    lv_obj_t *display_background_image_cont = wf_add_labeled_list( display_settings_tile_2?display_settings_tile_2:display_settings_tile_1, "BG image", &display_bg_img_list, "bg\nbg1\nbg2\nbg3\nnone\nbg.png", display_background_image_setup_event_cb, SETUP_STYLE );
    lv_obj_align( display_background_image_cont, block_return_maintile_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 8 );

    lv_slider_set_value( display_brightness_slider, display_get_brightness(), LV_ANIM_OFF );
    lv_slider_set_value( display_timeout_slider, display_get_timeout(), LV_ANIM_OFF );

    char temp[16]="";
    if ( lv_slider_get_value( display_timeout_slider ) == DISPLAY_MAX_TIMEOUT ) {
        snprintf( temp, sizeof( temp ), "no timeout" );
        setup_set_indicator( display_setup_icon, ICON_INDICATOR_FAIL );
    }
    else {
        snprintf( temp, sizeof( temp ), "%d seconds", lv_slider_get_value( display_timeout_slider ) );
    }

    lv_label_set_text( display_timeout_slider_label, temp );
    lv_obj_align( display_timeout_slider_label, display_timeout_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );

    lv_dropdown_set_selected( display_rotation_list, display_get_rotation() / 90 );
    lv_dropdown_set_selected( display_bg_img_list, display_get_background_image() );

    display_register_cb( DISPLAYCTL_BRIGHTNESS, display_displayctl_brightness_event_cb, "display settings" );
}

bool display_displayctl_brightness_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case DISPLAYCTL_BRIGHTNESS:
            lv_slider_set_value( display_brightness_slider, display_get_brightness() , LV_ANIM_OFF );
            break;
        case DISPLAYCTL_TIMEOUT:
            lv_slider_set_value( display_timeout_slider, display_get_timeout() , LV_ANIM_OFF );
            break;
    }
    return( true );
}

static void enter_display_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( display_tile_num_1, LV_ANIM_OFF );
                                        break;
    }

}

static void down_display_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( display_tile_num_2, LV_ANIM_ON );
                                        break;
    }

}

static void exit_display_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        display_save_config();
                                        break;
    }
}

static void display_vibe_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     display_set_vibe( lv_slider_get_value( obj ) );
                                            break;
    }
}

static void display_block_return_maintile_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     display_set_block_return_maintile( lv_slider_get_value( obj ) );
                                            break;
    }
}

static void display_brightness_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     display_set_brightness( lv_slider_get_value( obj ) );
                                            break;
    }
}

static void display_timeout_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     display_set_timeout( lv_slider_get_value( obj ) );
                                            char temp[16]="";
                                            if ( lv_slider_get_value(obj) == DISPLAY_MAX_TIMEOUT ) {
                                                snprintf( temp, sizeof( temp ), "no timeout" );
                                                setup_set_indicator( display_setup_icon, ICON_INDICATOR_FAIL );
                                            }
                                            else {
                                                snprintf( temp, sizeof( temp ), "%d seconds", lv_slider_get_value(obj) );
                                                setup_hide_indicator( display_setup_icon );
                                            }
                                            lv_label_set_text( display_timeout_slider_label, temp );
                                            lv_obj_align( display_timeout_slider_label, display_timeout_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );
                                            break;
    }
}

static void display_rotation_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     display_set_rotation( lv_dropdown_get_selected( obj ) * 90 );
                                            bma_set_rotate_tilt( lv_dropdown_get_selected( obj ) * 90 );
    }
}

static void display_background_image_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     
                                            display_set_background_image( lv_dropdown_get_selected( obj ) );
                                            gui_set_background_image( lv_dropdown_get_selected( obj ) );
    }
}

uint32_t display_get_setup_tile_num( void ) {
    return ( display_tile_num_1 );
}
