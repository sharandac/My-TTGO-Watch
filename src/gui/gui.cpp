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
#include <stdio.h>
#include <TTGO.h>

#include "gui.h"
#include "statusbar.h"
#include "screenshot.h"
#include "keyboard.h"

#include "mainbar/mainbar.h"
#include "mainbar/main_tile/main_tile.h"
#include "mainbar/app_tile/app_tile.h"
#include "mainbar/note_tile/note_tile.h"
#include "mainbar/setup_tile/setup_tile.h"

#include "mainbar/setup_tile/battery_settings/battery_settings.h"
#include "mainbar/setup_tile/display_settings/display_settings.h"
#include "mainbar/setup_tile/move_settings/move_settings.h"
#include "mainbar/setup_tile/time_settings/time_settings.h"
#include "mainbar/setup_tile/update/update.h"
#include "mainbar/setup_tile/wlan_settings/wlan_settings.h"
#include "mainbar/setup_tile/bluetooth_settings/bluetooth_settings.h"
#include "mainbar/setup_tile/sound_settings/sound_settings.h"

#include "mainbar/setup_tile/utilities/utilities.h"

#include "hardware/powermgm.h"
#include "hardware/display.h"

lv_obj_t *img_bin;

bool gui_powermgm_event_cb( EventBits_t event, void *arg );
bool gui_powermgm_loop_event_cb( EventBits_t event, void *arg );

void gui_setup( void )
{
    //Create wallpaper
    img_bin = lv_img_create( lv_scr_act() , NULL );
    lv_obj_set_width( img_bin, lv_disp_get_hor_res( NULL ) );
    lv_obj_set_height( img_bin, lv_disp_get_ver_res( NULL ) );
    lv_obj_align( img_bin, NULL, LV_ALIGN_CENTER, 0, 0 );
    mainbar_setup();
    /* add the four mainbar screens */
    main_tile_setup();
    app_tile_setup();
    note_tile_setup();
    setup_tile_setup();

    /* add setup */
    battery_settings_tile_setup();
    display_settings_tile_setup();
    move_settings_tile_setup();
    wlan_settings_tile_setup();
    bluetooth_settings_tile_setup();
    time_settings_tile_setup();
    update_tile_setup();
    utilities_tile_setup();
    sound_settings_tile_setup();

    statusbar_setup();
    lv_disp_trig_activity( NULL );

    gui_set_background_image( display_get_background_image() );

    keyboard_setup();
    num_keyboard_setup();

    powermgm_register_cb( POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, gui_powermgm_event_cb, "gui" );
    powermgm_register_loop_cb( POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, gui_powermgm_loop_event_cb, "gui loop" );
}

bool gui_powermgm_event_cb( EventBits_t event, void *arg ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    switch ( event ) {
        case POWERMGM_STANDBY:          log_i("go standby");
                                        if ( !display_get_block_return_maintile() ) {
                                            mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        }                               
                                        ttgo->stopLvglTick();
                                        break;
        case POWERMGM_WAKEUP:           log_i("go wakeup");
                                        ttgo->startLvglTick();
                                        lv_disp_trig_activity( NULL );
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   log_i("go silence wakeup");
                                        ttgo->startLvglTick();
                                        lv_disp_trig_activity( NULL );
                                        break;
    }
    return( true );
}

void gui_set_background_image ( uint32_t background_image ) {
    switch ( background_image ) {
        case 0:
            LV_IMG_DECLARE( bg );
            lv_img_set_src( img_bin, &bg );
            lv_obj_align( img_bin, NULL, LV_ALIGN_CENTER, 0, 0 );
            lv_obj_set_hidden( img_bin, false );
            break;
        case 1:
            LV_IMG_DECLARE( bg1 );
            lv_img_set_src( img_bin, &bg1 );
            lv_obj_align( img_bin, NULL, LV_ALIGN_CENTER, 0, 0 );
            lv_obj_set_hidden( img_bin, false );
            break;
        case 2:
            LV_IMG_DECLARE( bg2 );
            lv_img_set_src( img_bin, &bg2 );
            lv_obj_align( img_bin, NULL, LV_ALIGN_CENTER, 0, 0 );
            lv_obj_set_hidden( img_bin, false );
            break;
        case 3:
            LV_IMG_DECLARE( bg3 );
            lv_img_set_src( img_bin, &bg3 );
            lv_obj_align( img_bin, NULL, LV_ALIGN_CENTER, 0, 0 );
            lv_obj_set_hidden( img_bin, false );
            break;
        case 4:
            lv_obj_set_hidden( img_bin, true );
            break;
        case 5:
            FILE* file;
            file = fopen( BACKGROUNDIMAGE, "rb" );

            if ( file ) {
                log_i("set custom background image from spiffs");
                fclose( file );
                lv_img_set_src( img_bin, BACKGROUNDIMAGE );
                lv_obj_align( img_bin, NULL, LV_ALIGN_CENTER, 0, 0 );
                lv_obj_set_hidden( img_bin, false );
            }
            else {
                log_i("not custom background image found on spiffs, set to black");
                lv_obj_set_hidden( img_bin, true );
            }
            break;
        default:
            lv_obj_set_hidden( img_bin, true ); 
    }
}

bool gui_powermgm_loop_event_cb( EventBits_t event, void *arg ) {
    switch ( event ) {
        case POWERMGM_WAKEUP:           if ( lv_disp_get_inactive_time( NULL ) < display_get_timeout() * 1000 || display_get_timeout() == DISPLAY_MAX_TIMEOUT ) {
                                            lv_task_handler();
                                        }
                                        else {
                                            powermgm_set_event( POWERMGM_STANDBY_REQUEST );
                                        }
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   if ( lv_disp_get_inactive_time( NULL ) < display_get_timeout() * 1000 ) {
                                            lv_task_handler();
                                        }
                                        else {
                                            powermgm_set_event( POWERMGM_STANDBY_REQUEST );
                                        }
                                        break;
    }
    return( true );
}
