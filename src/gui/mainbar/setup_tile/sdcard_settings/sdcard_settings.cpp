/****************************************************************************
 *   Apr 23 14:17:11 2021
 *   Copyright  2021  Cornelius Wild
 *   Email: tt-watch-code@dervomsee.de
 *   Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch"
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
#include "sdcard_settings.h"
#include <Arduino.h>

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "utils/webserver/webserver.h"

lv_obj_t *sdcard_settings_tile=NULL;
lv_style_t sdcard_settings_style;
uint32_t sdcard_settings_tile_num;
lv_obj_t *sdcard_fs_browser_onoff = NULL;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(sdcard_settings_64px);

static void enter_sdcard_settings_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_sdcard_settings_event_cb( lv_obj_t * obj, lv_event_t event );
static void sdcard_fs_browser_onoff_event_handler(lv_obj_t * obj, lv_event_t event);

void sdcard_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    sdcard_settings_tile_num = mainbar_add_app_tile( 1, 1, "SD card setup" );
    sdcard_settings_tile = mainbar_get_tile_obj( sdcard_settings_tile_num );
    lv_style_copy( &sdcard_settings_style, ws_get_setup_tile_style() );
    lv_obj_add_style( sdcard_settings_tile, LV_OBJ_PART_MAIN, &sdcard_settings_style );

    icon_t *utilities_setup_icon = setup_register( "SD card", &sdcard_settings_64px, enter_sdcard_settings_event_cb );
    setup_hide_indicator( utilities_setup_icon );

    lv_obj_t *exit_btn = lv_imgbtn_create( sdcard_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &sdcard_settings_style );
    lv_obj_align( exit_btn, sdcard_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_sdcard_settings_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( sdcard_settings_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &sdcard_settings_style  );
    lv_label_set_text( exit_label, "SD card setup");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *sdcard_fs_browser_enable_cont = lv_obj_create( sdcard_settings_tile, NULL );
    lv_obj_set_size( sdcard_fs_browser_enable_cont, lv_disp_get_hor_res( NULL ) , 32);
    lv_obj_add_style( sdcard_fs_browser_enable_cont, LV_OBJ_PART_MAIN, &sdcard_settings_style  );
    lv_obj_align( sdcard_fs_browser_enable_cont, sdcard_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    sdcard_fs_browser_onoff = wf_add_switch( sdcard_fs_browser_enable_cont, false );
    lv_obj_align( sdcard_fs_browser_onoff, sdcard_fs_browser_enable_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( sdcard_fs_browser_onoff, sdcard_fs_browser_onoff_event_handler );
    lv_obj_t *bluetooth_enable_label = lv_label_create( sdcard_fs_browser_enable_cont, NULL);
    lv_obj_add_style( bluetooth_enable_label, LV_OBJ_PART_MAIN, &sdcard_settings_style  );
    lv_label_set_text( bluetooth_enable_label, "Enable SD card in \nFS browser");
    lv_obj_align( bluetooth_enable_label, sdcard_fs_browser_enable_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

}

static void enter_sdcard_settings_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( sdcard_settings_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_sdcard_settings_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
                                        break;
    }
}


static void sdcard_fs_browser_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    if( event == LV_EVENT_VALUE_CHANGED)
    {
        if( lv_switch_get_state( obj ) )
        {
            log_d("enable SD");
            setFsEditorFilesystem(SD);
        }
        else
        {
            log_d("enable SPIFFS");
            setFsEditorFilesystem(SPIFFS);
        }
    }
}