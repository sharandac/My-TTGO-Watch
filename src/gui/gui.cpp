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
#include "lvgl.h"
#include "gui.h"
#include "statusbar.h"
#include "quickbar.h"
#include "screenshot.h"
#include "widget_styles.h"
#include "keyboard.h"
#include "gui/lv_fs/lv_fs_spiffs.h"
#include "mainbar/mainbar.h"
#include "mainbar/main_tile/main_tile.h"
#include "mainbar/app_tile/app_tile.h"
#include "mainbar/note_tile/note_tile.h"
#include "mainbar/setup_tile/setup_tile.h"
#include "mainbar/setup_tile/display_settings/display_settings.h"
#include "mainbar/setup_tile/gps_settings/gps_settings.h"
#include "mainbar/setup_tile/move_settings/move_settings.h"
#include "mainbar/setup_tile/battery_settings/battery_settings.h"
#include "mainbar/setup_tile/sound_settings/sound_settings.h"
#include "mainbar/setup_tile/sdcard_settings/sdcard_settings.h"
#include "mainbar/setup_tile/utilities/utilities.h"
#include "mainbar/setup_tile/bluetooth_settings/bluetooth_settings.h"
#include "mainbar/setup_tile/wlan_settings/wlan_settings.h"
#include "mainbar/setup_tile/time_settings/time_settings.h"
#include "mainbar/setup_tile/watchface/watchface_manager.h"
#include "gui/mainbar/setup_tile/watchface/config/watchface_expr.h"
#include "mainbar/setup_tile/update/update.h"
#include "mainbar/setup_tile/style_settings/style_settings.h"
#include "hardware/powermgm.h"
#include "hardware/framebuffer.h"
#include "hardware/display.h"
#include "hardware/hardware.h"
#include "utils/filepath_convert.h"

#ifdef NATIVE_64BIT
    #include <iostream>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include "utils/logging.h"
#else
    #include <Arduino.h>
#endif

lv_obj_t *img_bin = NULL;

static volatile bool force_redraw = false;

bool gui_powermgm_event_cb( EventBits_t event, void *arg );
bool gui_powermgm_loop_event_cb( EventBits_t event, void *arg );

void gui_setup( void ) {
    /**
     * install lv fs spiffs wrapper
     * files begin with "P:/foo.bar" -> "/spiffs/foo.bar"
     */
    lv_fs_if_spiffs_init();
    /*
     * Create an blank wallpaper
     */
    img_bin = lv_img_create( lv_scr_act() , NULL );
    lv_obj_set_width( img_bin, lv_disp_get_hor_res( NULL ) );
    lv_obj_set_height( img_bin, lv_disp_get_ver_res( NULL ) );
    lv_obj_align( img_bin, NULL, LV_ALIGN_CENTER, 0, 0 );

    log_i("mainbar setup");
    mainbar_setup();
    /*
     * add the four mainbar screens
     */
    log_i("mainbar tile setup");
    main_tile_setup();
    log_i("app tile setup");
    app_tile_setup();
    log_i("note tile setup");
    note_tile_setup();
    log_i("setup tile setup");
    setup_tile_setup();
    /*
     * add input and status
     */
    log_i("statusbar setup");
    statusbar_setup();
    log_i("quickbar setup");
    quickbar_setup();
    log_i("keyboard setup");
    keyboard_setup();
    log_i("num keyboard setup");
    num_keyboard_setup();
    /*
     * add setup tool to the setup tile
     */

    battery_settings_tile_setup();
    display_settings_tile_setup();
    move_settings_tile_setup();
    style_settings_tile_setup();
    wlan_settings_tile_setup();
    time_settings_tile_setup();
    gps_settings_tile_setup();
    utilities_tile_setup();
    sound_settings_tile_setup();
    #ifndef NO_UPDATES
        update_tile_setup();
    #endif
    #ifndef NO_BLUETOOTH
        bluetooth_settings_tile_setup();
    #endif
    #ifndef NO_WATCHFACE
        watchface_manager_setup();
        watchface_expr_setup();
    #endif

    #if defined( LILYGO_WATCH_HAS_SDCARD )
        sdcard_settings_tile_setup();
    #endif

    /*
     * trigger an activity
     */
    lv_disp_trig_activity( NULL );
    /*
     * setup background image
     */
    gui_set_background_image( display_get_background_image() );
    /*
     * register the main powermgm routine for the gui
     */
    powermgm_register_cb_with_prio( POWERMGM_STANDBY, gui_powermgm_event_cb, "gui", CALL_CB_FIRST );
    powermgm_register_cb_with_prio( POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, gui_powermgm_event_cb, "gui", CALL_CB_LAST );
    powermgm_register_loop_cb( POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, gui_powermgm_loop_event_cb, "gui loop" );

#if defined( NATIVE_64BIT ) && defined( ROUND_DISPLAY )
    LV_IMG_DECLARE( rounddisplaymask_240px );

    lv_obj_t *watch2021_mask_bin = lv_img_create( lv_scr_act() , NULL );
    lv_obj_set_width( watch2021_mask_bin, lv_disp_get_hor_res( NULL ) );
    lv_obj_set_height( watch2021_mask_bin, lv_disp_get_ver_res( NULL ) );
    lv_obj_align( watch2021_mask_bin, NULL, LV_ALIGN_CENTER, 0, 0 );
    lv_img_set_src( watch2021_mask_bin, &rounddisplaymask_240px );
    lv_obj_align( watch2021_mask_bin, NULL, LV_ALIGN_CENTER, 0, 0 );
#endif

#ifdef M5PAPER
    widget_style_theme_set( 0 );
#endif
}

bool gui_powermgm_event_cb( EventBits_t event, void *arg ) {

    switch ( event ) {
        case POWERMGM_STANDBY:          /*
                                         * get back to maintile if configure and
                                         * stop all LVGL activitys and tasks
                                         */
                                        log_i("go standby");                  
                                        #ifdef NATIVE_64BIT
                                        #else
                                            lv_obj_invalidate( lv_scr_act() );
                                            lv_refr_now( NULL );
                                            hardware_detach_lvgl_ticker();
                                        #endif
                                        break;
        case POWERMGM_WAKEUP:           /*
                                         * resume all LVGL activitys and tasks
                                         */
                                        log_i("go wakeup");
                                        #ifdef NATIVE_64BIT
                                        #else
                                            hardware_attach_lvgl_ticker();
                                        #endif
                                        lv_disp_trig_activity( NULL );
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   /*
                                         * resume all LVGL activitys and tasks
                                         */
                                        log_i("go silence wakeup");
                                        #ifdef NATIVE_64BIT
                                        #else
                                            hardware_attach_lvgl_ticker();
                                        #endif
                                        lv_disp_trig_activity( NULL );
                                        break;
        case POWERMGM_DISABLE_INTERRUPTS:
                                        /*
                                         * stop LVGL ticks
                                         */
                                        break;
        case POWERMGM_ENABLE_INTERRUPTS:
                                        /*
                                         * stop LVGL ticks
                                         */
                                        break;                                        
    }
    return( true );
}


void gui_force_redraw( bool force ) {
    force_redraw = force;
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
        case 5: {
            FILE* file;
            char filename[256] = "";
            filepath_convert( filename, sizeof( filename ), BACKGROUNDIMAGE );
            file = fopen( filename, "rb" );
            if ( file ) {
                log_i("set custom background image from spiffs");
                fclose( file );
                lv_img_set_src( img_bin, filename );
                lv_obj_align( img_bin, NULL, LV_ALIGN_CENTER, 0, 0 );
                lv_obj_set_hidden( img_bin, false );
            }
            else {
                log_i("not custom background image found on spiffs, set to black");
                lv_obj_set_hidden( img_bin, true );
            }
            break;
        }
        default:
            lv_obj_set_hidden( img_bin, true ); 
    }
}

bool gui_powermgm_loop_event_cb( EventBits_t event, void *arg ) {
    #ifdef NATIVE_64BIT

    #else
        switch ( event ) {
            case POWERMGM_WAKEUP:           if ( lv_disp_get_inactive_time( NULL ) < display_get_timeout() * 1000  || display_get_timeout() == DISPLAY_MAX_TIMEOUT ) {
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
    #endif

    lv_task_handler();

    if ( force_redraw ) {
        force_redraw = !force_redraw;
        lv_obj_invalidate( lv_scr_act() );
    }
    return( true );
}
