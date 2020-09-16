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
#include <TTGO.h>

#include "stopwatch_app.h"
#include "stopwatch_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

long stopwatch_milliseconds = 0;
time_t prev_time;

lv_obj_t *stopwatch_app_main_tile = NULL;
lv_style_t stopwatch_app_main_style;

lv_style_t stopwatch_app_main_stopwatchstyle;
lv_obj_t *stopwatch_app_main_stopwatchlabel = NULL;

lv_obj_t *stopwatch_app_main_start_btn = NULL;
lv_obj_t *stopwatch_app_main_stop_btn = NULL;
lv_obj_t *stopwatch_app_main_reset_btn = NULL;

lv_task_t * _stopwatch_app_task;

LV_IMG_DECLARE(exit_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void start_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void stop_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void reset_stopwatch_app_main_event_cb( lv_obj_t * obj, lv_event_t event );

void stopwatch_app_task( lv_task_t * task );

void stopwatch_app_main_setup( uint32_t tile_num ) {

    stopwatch_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &stopwatch_app_main_style, mainbar_get_style());

    lv_style_copy( &stopwatch_app_main_stopwatchstyle, &stopwatch_app_main_style);
    lv_style_set_text_font( &stopwatch_app_main_stopwatchstyle, LV_STATE_DEFAULT, &Ubuntu_72px);


    lv_obj_t * stopwatch_cont = mainbar_obj_create( stopwatch_app_main_tile );
    lv_obj_set_size( stopwatch_cont, LV_HOR_RES , LV_VER_RES / 2 );
    lv_obj_add_style( stopwatch_cont, LV_OBJ_PART_MAIN, &stopwatch_app_main_style );
    lv_obj_align( stopwatch_cont, stopwatch_app_main_tile, LV_ALIGN_CENTER, 0, 0 );

    stopwatch_app_main_stopwatchlabel = lv_label_create( stopwatch_cont , NULL);
    lv_label_set_text(stopwatch_app_main_stopwatchlabel, "00:00");
    lv_obj_reset_style_list( stopwatch_app_main_stopwatchlabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( stopwatch_app_main_stopwatchlabel, LV_OBJ_PART_MAIN, &stopwatch_app_main_stopwatchstyle );
    lv_obj_align(stopwatch_app_main_stopwatchlabel, NULL, LV_ALIGN_CENTER, 0, 0);

    stopwatch_app_main_start_btn = lv_btn_create(stopwatch_app_main_tile, NULL);  
    lv_obj_set_size(stopwatch_app_main_start_btn, 50, 50);
    lv_obj_add_style(stopwatch_app_main_start_btn, LV_IMGBTN_PART_MAIN, &stopwatch_app_main_style );
    lv_obj_align(stopwatch_app_main_start_btn, stopwatch_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_obj_set_event_cb( stopwatch_app_main_start_btn, start_stopwatch_app_main_event_cb );

    lv_obj_t *stopwatch_app_main_start_btn_label = lv_label_create(stopwatch_app_main_start_btn, NULL);
    lv_label_set_text(stopwatch_app_main_start_btn_label, LV_SYMBOL_PLAY);

    stopwatch_app_main_stop_btn = lv_btn_create(stopwatch_app_main_tile, NULL);  
    lv_obj_set_size(stopwatch_app_main_stop_btn, 50, 50);
    lv_obj_add_style(stopwatch_app_main_stop_btn, LV_IMGBTN_PART_MAIN, &stopwatch_app_main_style );
    lv_obj_align(stopwatch_app_main_stop_btn, stopwatch_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, 0, 0 );
    lv_obj_set_event_cb( stopwatch_app_main_stop_btn, stop_stopwatch_app_main_event_cb );
    lv_obj_set_hidden(stopwatch_app_main_stop_btn, true);

    lv_obj_t *stopwatch_app_main_stop_btn_label = lv_label_create(stopwatch_app_main_stop_btn, NULL);
    lv_label_set_text(stopwatch_app_main_stop_btn_label, LV_SYMBOL_STOP);

    stopwatch_app_main_reset_btn = lv_btn_create(stopwatch_app_main_tile, NULL);  
    lv_obj_set_size(stopwatch_app_main_reset_btn, 50, 50);
    lv_obj_add_style(stopwatch_app_main_reset_btn, LV_IMGBTN_PART_MAIN, &stopwatch_app_main_style );
    lv_obj_align(stopwatch_app_main_reset_btn, stopwatch_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT,  -20, 0 );
    lv_obj_set_event_cb( stopwatch_app_main_reset_btn, reset_stopwatch_app_main_event_cb );

    lv_obj_t *stopwatch_app_main_reset_btn_label = lv_label_create(stopwatch_app_main_reset_btn, NULL);
    lv_label_set_text(stopwatch_app_main_reset_btn_label, LV_SYMBOL_EJECT);

    lv_obj_t * exit_btn = lv_imgbtn_create( stopwatch_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &stopwatch_app_main_style );
    lv_obj_align(exit_btn, stopwatch_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_stopwatch_app_main_event_cb );

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
                                        lv_task_del(_stopwatch_app_task);
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
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
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