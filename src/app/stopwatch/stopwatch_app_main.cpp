/****************************************************************************
 *   Aug 21 17:26:00 2020
 *   Copyright  2020  Chris McNamee
 *   Email: chris.mcna@gmail.com
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

#include "stopwatch_app.h"
#include "stopwatch_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#ifdef NATIVE_64BIT
    #include <time.h>
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <time.h>
    #include <Arduino.h>
#endif

long stopwatch_milliseconds = 0;
static time_t prev_time;

lv_obj_t *stopwatch_app_main_tile = NULL;
lv_obj_t *stopwatch_app_main_stopwatchlabel = NULL;
lv_obj_t *stopwatch_app_main_start_btn = NULL;
lv_obj_t *stopwatch_app_main_stop_btn = NULL;
lv_obj_t *stopwatch_app_main_reset_btn = NULL;
lv_obj_t * stopwatch_app_main_exit_btn = NULL;

lv_style_t stopwatch_app_main_stopwatchstyle;

lv_task_t * _stopwatch_app_task = NULL;

LV_FONT_DECLARE(Ubuntu_72px);

static void stopwatch_activate_event_cb( void );
bool stopwatch_button_event_cb( EventBits_t event, void *arg );
bool stopwatch_style_change_event_cb( EventBits_t event, void *arg );
static void exit_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void start_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void stop_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void reset_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event );

void stopwatch_app_task( lv_task_t * task );

void stopwatch_app_main_setup( uint32_t tile_num ) {

    stopwatch_app_main_tile = mainbar_get_tile_obj( tile_num );

    lv_style_copy( &stopwatch_app_main_stopwatchstyle, APP_STYLE );
    lv_style_set_text_font( &stopwatch_app_main_stopwatchstyle, LV_STATE_DEFAULT, &Ubuntu_72px);

    lv_obj_t * stopwatch_cont = mainbar_obj_create( stopwatch_app_main_tile );
    lv_obj_set_size( stopwatch_cont, LV_HOR_RES , LV_VER_RES / 2 );
    lv_obj_add_style( stopwatch_cont, LV_OBJ_PART_MAIN, APP_STYLE );
    lv_obj_align( stopwatch_cont, stopwatch_app_main_tile, LV_ALIGN_CENTER, 0, 0 );

    stopwatch_app_main_stopwatchlabel = lv_label_create( stopwatch_cont , NULL);
    lv_label_set_text(stopwatch_app_main_stopwatchlabel, "00:00");
    lv_obj_reset_style_list( stopwatch_app_main_stopwatchlabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( stopwatch_app_main_stopwatchlabel, LV_OBJ_PART_MAIN, &stopwatch_app_main_stopwatchstyle );
    lv_obj_align(stopwatch_app_main_stopwatchlabel, NULL, LV_ALIGN_CENTER, 0, 0);

    stopwatch_app_main_start_btn = wf_add_play_button( stopwatch_app_main_tile, start_stopwatch_app_main_event_cb );
    lv_obj_align(stopwatch_app_main_start_btn, stopwatch_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -THEME_PADDING );

    stopwatch_app_main_stop_btn = wf_add_stop_button( stopwatch_app_main_tile, stop_stopwatch_app_main_event_cb );
    lv_obj_align(stopwatch_app_main_stop_btn, stopwatch_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -THEME_PADDING );
    lv_obj_set_hidden(stopwatch_app_main_stop_btn, true);

    stopwatch_app_main_reset_btn = wf_add_eject_button( stopwatch_app_main_tile, reset_stopwatch_app_main_event_cb );
    lv_obj_align(stopwatch_app_main_reset_btn, stopwatch_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING, -THEME_PADDING );

    stopwatch_app_main_exit_btn = wf_add_exit_button( stopwatch_app_main_tile, exit_stopwatch_app_main_event_cb );
    lv_obj_align(stopwatch_app_main_exit_btn, stopwatch_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_PADDING, -THEME_PADDING );

    styles_register_cb( STYLE_CHANGE, stopwatch_style_change_event_cb, "stopwatch style change" );
    mainbar_add_tile_button_cb( tile_num, stopwatch_button_event_cb );
    mainbar_add_tile_activate_cb( tile_num, stopwatch_activate_event_cb );
}

static void stopwatch_activate_event_cb( void ) {
    wf_image_button_fade_in(stopwatch_app_main_exit_btn, 300, 0 );
    if( !_stopwatch_app_task )
        wf_image_button_fade_in(stopwatch_app_main_start_btn, 300, 100 );
    else 
        wf_image_button_fade_in(stopwatch_app_main_stop_btn, 300, 100 );
    wf_image_button_fade_in(stopwatch_app_main_reset_btn, 300, 200 );
}

bool stopwatch_button_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_EXIT:   mainbar_jump_back();
                            break;
    }
    return( true );
}

bool stopwatch_style_change_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_CHANGE:      lv_style_copy( &stopwatch_app_main_stopwatchstyle, APP_STYLE );
                                lv_style_set_text_font( &stopwatch_app_main_stopwatchstyle, LV_STATE_DEFAULT, &Ubuntu_72px);
                                break;
    }
    return( true );
}

static void stopwatch_app_main_update_stopwatchlabel()
{
    //int hr = (stopwatch_milliseconds / (1000 * 60 * 60)) % 24;

    // minutes
    int min = (stopwatch_milliseconds / (1000 * 60)) % 60;

    // seconds
    int sec = (stopwatch_milliseconds / 1000) % 60;

    // milliseconds
    //int mill = stopwatch_milliseconds % 1000;

    char msg[10];
    sprintf(msg,"%02d:%02d", min, sec);

    lv_label_set_text(stopwatch_app_main_stopwatchlabel, msg);
    lv_obj_align(stopwatch_app_main_stopwatchlabel, NULL, LV_ALIGN_CENTER, 0, 0);
    stopwatch_app_update_widget_label( msg );
}



static void start_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       // create an task that runs every secound
                                        prev_time = time(0);
                                        if( !_stopwatch_app_task )
                                            _stopwatch_app_task = lv_task_create( stopwatch_app_task, 1000, LV_TASK_PRIO_MID, NULL );
                                        lv_obj_set_hidden(stopwatch_app_main_start_btn, true);
                                        lv_obj_set_hidden(stopwatch_app_main_stop_btn, false);
                                        stopwatch_add_widget();
                                        stopwatch_app_hide_app_icon_info( false );
                                        break;
    }
}

static void stop_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       // create an task that runs every secound
                                        if( _stopwatch_app_task )
                                            lv_task_del(_stopwatch_app_task);
                                        _stopwatch_app_task = NULL;
                                        lv_obj_set_hidden(stopwatch_app_main_start_btn, false);
                                        lv_obj_set_hidden(stopwatch_app_main_stop_btn, true);
                                        stopwatch_remove_widget();
                                        stopwatch_app_hide_app_icon_info( true );
                                        break;
    }
}

static void reset_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       log_i("Reset clicked");
                                        stopwatch_milliseconds = 0;
                                        stopwatch_app_main_update_stopwatchlabel();
                                        break;
    }
}


static void exit_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}

void stopwatch_app_task( lv_task_t * task ) {

    time_t now = time(0);
    double dif_seconds = difftime(now,prev_time);
    stopwatch_milliseconds += dif_seconds * 1000;
    prev_time = now;

    stopwatch_app_main_update_stopwatchlabel();
}
