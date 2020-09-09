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

LV_IMG_DECLARE(bg2);

void gui_setup( void )
{
    //Create wallpaper
    lv_obj_t *img_bin = lv_img_create( lv_scr_act() , NULL );
    lv_img_set_src( img_bin, &bg2 );
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

    keyboard_setup();

    return;
}

void gui_loop( void ) {
    // if we run in silence mode    
    if ( powermgm_get_event( POWERMGM_SILENCE_WAKEUP ) ) {
        if ( lv_disp_get_inactive_time(NULL) < display_get_timeout() * 1000 ) {
            lv_task_handler();
        }
        else {
            powermgm_set_event( POWERMGM_STANDBY_REQUEST );
        }
    }
    // if we run on normal mode
    else if ( !powermgm_get_event( POWERMGM_STANDBY ) ) {
        if ( lv_disp_get_inactive_time(NULL) < display_get_timeout() * 1000 || display_get_timeout() == DISPLAY_MAX_TIMEOUT ) {
            lv_task_handler();
        }
        else {
            powermgm_set_event( POWERMGM_STANDBY_REQUEST );
        }
    }
}