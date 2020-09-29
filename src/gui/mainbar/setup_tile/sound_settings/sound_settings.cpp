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
#include "sound_settings.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"

#include "hardware/sound.h"


icon_t *sound_setup_icon = NULL;

lv_obj_t *sound_settings_tile = NULL;
lv_style_t sound_settings_style;
uint32_t sound_tile_num;

lv_obj_t *sound_volume_slider = NULL;
lv_obj_t *sound_volume_slider_label = NULL;
lv_obj_t *sound_enable = NULL;
lv_obj_t *sound_icon = NULL;

LV_IMG_DECLARE(sound_64px);
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(sound_32px);
LV_IMG_DECLARE(sound_mute_32px);

static void enter_sound_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_sound_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void sound_volume_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void sound_enable_setup_event_cb( lv_obj_t * obj, lv_event_t event );

bool sound_soundctl_event_cb( EventBits_t event, void *arg );

void sound_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    sound_tile_num = mainbar_add_app_tile( 1, 2, "sound setup" );
    sound_settings_tile = mainbar_get_tile_obj( sound_tile_num );

    lv_style_copy( &sound_settings_style, mainbar_get_style() );
    lv_style_set_bg_color( &sound_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &sound_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &sound_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( sound_settings_tile, LV_OBJ_PART_MAIN, &sound_settings_style );

    sound_setup_icon = setup_register( "sound", &sound_64px, enter_sound_setup_event_cb );
    setup_hide_indicator( sound_setup_icon );

    lv_obj_t *exit_btn_1 = lv_imgbtn_create( sound_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn_1, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn_1, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn_1, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn_1, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn_1, LV_IMGBTN_PART_MAIN, &sound_settings_style );
    lv_obj_align( exit_btn_1, sound_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn_1, exit_sound_setup_event_cb );
    
    lv_obj_t *exit_label_1 = lv_label_create( sound_settings_tile, NULL );
    lv_obj_add_style( exit_label_1, LV_OBJ_PART_MAIN, &sound_settings_style  );
    lv_label_set_text( exit_label_1, "sound settings");
    lv_obj_align( exit_label_1, exit_btn_1, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *sound_enable_cont = lv_obj_create( sound_settings_tile, NULL );
    lv_obj_set_size(sound_enable_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( sound_enable_cont, LV_OBJ_PART_MAIN, &sound_settings_style );
    lv_obj_align( sound_enable_cont, sound_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    sound_enable = lv_switch_create( sound_enable_cont, NULL );
    lv_obj_add_protect( sound_enable, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( sound_enable, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( sound_enable, LV_ANIM_ON );
    lv_obj_align( sound_enable, sound_enable_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( sound_enable, sound_enable_setup_event_cb );
    lv_obj_t *sound_enable_label = lv_label_create( sound_enable_cont, NULL);
    lv_obj_add_style( sound_enable_label, LV_OBJ_PART_MAIN, &sound_settings_style );
    lv_label_set_text( sound_enable_label, "enable");
    lv_obj_align( sound_enable_label, sound_enable_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );


    lv_obj_t *sound_volume_cont = lv_obj_create( sound_settings_tile, NULL );
    lv_obj_set_size( sound_volume_cont, lv_disp_get_hor_res( NULL ) , 58 );
    lv_obj_add_style( sound_volume_cont, LV_OBJ_PART_MAIN, &sound_settings_style  );
    lv_obj_align( sound_volume_cont, sound_enable_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    sound_volume_slider = lv_slider_create( sound_volume_cont, NULL );
    lv_obj_add_protect( sound_volume_slider, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( sound_volume_slider, LV_SLIDER_PART_INDIC, mainbar_get_slider_style() );
    lv_obj_add_style( sound_volume_slider, LV_SLIDER_PART_KNOB, mainbar_get_slider_style() );
    lv_slider_set_range( sound_volume_slider, 1, 100 );
    lv_obj_set_size(sound_volume_slider, lv_disp_get_hor_res( NULL ) - 100 , 10 );
    lv_obj_align( sound_volume_slider, sound_volume_cont, LV_ALIGN_IN_TOP_RIGHT, -30, 10 );
    lv_obj_set_event_cb( sound_volume_slider, sound_volume_setup_event_cb );
    sound_volume_slider_label = lv_label_create( sound_volume_cont, NULL );
    lv_obj_add_style( sound_volume_slider_label, LV_OBJ_PART_MAIN, &sound_settings_style  );
    lv_label_set_text( sound_volume_slider_label, "");
    lv_obj_align( sound_volume_slider_label, sound_volume_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, -5 );
    sound_icon = lv_img_create( sound_volume_cont, NULL );
    lv_img_set_src( sound_icon, &sound_32px );
    lv_obj_align( sound_icon, sound_volume_cont, LV_ALIGN_IN_LEFT_MID, 15, 0 );

    log_i("Setting initial volume configuration to %d", sound_get_volume_config());
    lv_slider_set_value( sound_volume_slider, sound_get_volume_config(), LV_ANIM_OFF );
    char temp[16]="";
    snprintf( temp, sizeof( temp ), "volume %d", lv_slider_get_value( sound_volume_slider ) );
    lv_label_set_text( sound_volume_slider_label, temp );
    lv_obj_align( sound_volume_slider_label, sound_volume_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );


    if ( sound_get_enabled_config() ) {
        log_i("Setting initial volume configuration to enabled");
        lv_switch_on( sound_enable, LV_ANIM_OFF );
    } else {
        log_i("Setting initial volume configuration to disabled");
        lv_switch_off( sound_enable, LV_ANIM_OFF );
    }

    lv_tileview_add_element( sound_settings_tile, sound_enable_cont );
    lv_tileview_add_element( sound_settings_tile, sound_volume_cont );

    sound_register_cb( SOUNDCTL_ENABLED | SOUNDCTL_VOLUME, sound_soundctl_event_cb, "sound settings");
}

static void enter_sound_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( sound_tile_num, LV_ANIM_OFF );
                                        break;
    }

}

static void exit_sound_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
                                        sound_save_config();
                                        break;
    }
}

static void sound_enable_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     
                                            sound_set_enabled_config( lv_switch_get_state( obj ) );
                                            if (sound_get_enabled_config()) {
                                                lv_img_set_src( sound_icon, &sound_32px );
                                            }
                                            else {
                                                lv_img_set_src( sound_icon, &sound_mute_32px );
                                            }
                                            break;
    }
}

static void sound_volume_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     
                                            char temp[16]="";
                                            snprintf( temp, sizeof( temp ), "volume %d", lv_slider_get_value( sound_volume_slider ) );
                                            lv_label_set_text( sound_volume_slider_label, temp );
                                            lv_obj_align( sound_volume_slider_label, sound_volume_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );
                                            sound_set_volume_config( lv_slider_get_value( obj ) );
                                            break;
    }
}

bool sound_soundctl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case SOUNDCTL_ENABLED:  
            if ( *(bool*)arg ) {
                lv_switch_on( sound_enable, LV_ANIM_OFF);
                lv_img_set_src( sound_icon, &sound_32px );
            }
            else {
                lv_switch_off( sound_enable, LV_ANIM_OFF);
                lv_img_set_src( sound_icon, &sound_mute_32px );
            }
            break;
        case SOUNDCTL_VOLUME:
            lv_slider_set_value( sound_volume_slider, *(int8_t*)arg, LV_ANIM_OFF );
            char temp[16]="";
            snprintf( temp, sizeof( temp ), "volume %d", lv_slider_get_value( sound_volume_slider ) );
            lv_label_set_text( sound_volume_slider_label, temp );
            lv_obj_align( sound_volume_slider_label, sound_volume_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );
            break;
    }
    return( true );
}