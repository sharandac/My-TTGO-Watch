/****************************************************************************
 *   Copyright  2020  Chris McNamee
 *   Copyright  2022  Pavel Machek
 *   Email: pavel@ucw.cz
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

#include "astro_app.h"
#include "astro_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/timesync.h"

#ifdef NATIVE_64BIT
    #include <time.h>
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <time.h>
    #include <Arduino.h>
#endif

long astro_milliseconds = 0;
static time_t prev_time;

lv_obj_t *astro_app_main_tile = NULL;
lv_obj_t *astro_app_main_astrolabel = NULL;
lv_obj_t *astro_app_main_start_btn = NULL;
lv_obj_t *astro_app_main_stop_btn = NULL;
lv_obj_t *astro_app_main_reset_btn = NULL;
lv_obj_t *astro_app_main_exit_btn = NULL;

lv_style_t astro_app_main_astrostyle;

lv_task_t * _astro_app_task = NULL;

LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(Ubuntu_32px);

static void astro_main_activate_cb( void );
static bool astro_button_event_cb( EventBits_t event, void *arg );
static bool astro_style_change_event_cb( EventBits_t event, void *arg );
static void exit_astro_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void start_astro_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void stop_astro_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void reset_astro_app_main_event_cb( lv_obj_t * obj, lv_event_t event );

void astro_app_task( lv_task_t * task );

extern "C" {
  extern void sunrise_display_callback( char *buf, int len, int flags );
}


void astro_app_main_setup( uint32_t tile_num ) {

    astro_app_main_tile = mainbar_get_tile_obj( tile_num );

    lv_style_copy( &astro_app_main_astrostyle, APP_STYLE );
    lv_style_set_text_font( &astro_app_main_astrostyle, LV_STATE_DEFAULT, &Ubuntu_32px);

    lv_obj_t * astro_cont = mainbar_obj_create( astro_app_main_tile );
    lv_obj_set_size( astro_cont, LV_HOR_RES , LV_VER_RES - 100 );
    lv_obj_add_style( astro_cont, LV_OBJ_PART_MAIN, APP_STYLE );
    lv_obj_align( astro_cont, astro_app_main_tile, LV_ALIGN_CENTER, 0, 0 );

    astro_app_main_astrolabel = lv_label_create( astro_cont , NULL);
    lv_label_set_text(astro_app_main_astrolabel, "(press start)");
    lv_obj_reset_style_list( astro_app_main_astrolabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( astro_app_main_astrolabel, LV_OBJ_PART_MAIN, &astro_app_main_astrostyle );
    lv_obj_align(astro_app_main_astrolabel, NULL, LV_ALIGN_CENTER, 0, 0);

    astro_app_main_start_btn = wf_add_play_button( astro_app_main_tile, start_astro_app_main_event_cb );
    lv_obj_align(astro_app_main_start_btn, astro_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -THEME_PADDING );

    astro_app_main_stop_btn = wf_add_stop_button( astro_app_main_tile, stop_astro_app_main_event_cb );
    lv_obj_align(astro_app_main_stop_btn, astro_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -THEME_PADDING );
    lv_obj_set_hidden(astro_app_main_stop_btn, true);

    astro_app_main_reset_btn = wf_add_eject_button( astro_app_main_tile, reset_astro_app_main_event_cb );
    lv_obj_align(astro_app_main_reset_btn, astro_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING, -THEME_PADDING );

    astro_app_main_exit_btn = wf_add_exit_button( astro_app_main_tile, exit_astro_app_main_event_cb );
    lv_obj_align( astro_app_main_exit_btn, astro_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_PADDING, -THEME_PADDING );

    styles_register_cb( STYLE_CHANGE, astro_style_change_event_cb, "astro style change" );
    mainbar_add_tile_button_cb( tile_num, astro_button_event_cb );
    mainbar_add_tile_activate_cb( tile_num, astro_main_activate_cb );
}

static void astro_main_activate_cb( void ) {
    wf_image_button_fade_in( astro_app_main_exit_btn, 500, 0 );
    wf_image_button_fade_in( astro_app_main_start_btn, 500, 100 );
    wf_image_button_fade_in( astro_app_main_stop_btn, 500, 100 );
    wf_image_button_fade_in( astro_app_main_reset_btn, 500, 200 );
}

static bool astro_button_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_EXIT:   mainbar_jump_back();
                            break;
    }
    return( true );
}

static bool astro_style_change_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_CHANGE:      lv_style_copy( &astro_app_main_astrostyle, APP_STYLE );
                                lv_style_set_text_font( &astro_app_main_astrostyle, LV_STATE_DEFAULT, &Ubuntu_32px);
                                break;
    }
    return( true );
}

static void astro_app_main_update_astrolabel()
{
    char msg[100];
    sunrise_display_callback(msg, 100, timesync_get_timezone());

    lv_label_set_text(astro_app_main_astrolabel, msg);
    lv_obj_align(astro_app_main_astrolabel, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_align( astro_app_main_astrolabel, LV_LABEL_ALIGN_CENTER);
    astro_app_update_widget_label( msg );
}



static void start_astro_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       // create an task that runs every secound
                                        prev_time = time(0);
                                        if( !_astro_app_task )
                                            _astro_app_task = lv_task_create( astro_app_task, 1000, LV_TASK_PRIO_MID, NULL );
                                        lv_obj_set_hidden(astro_app_main_start_btn, true);
                                        lv_obj_set_hidden(astro_app_main_stop_btn, false);
                                        astro_add_widget();
                                        astro_app_hide_app_icon_info( false );
                                        break;
    }
}

static void stop_astro_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       // create an task that runs every secound
                                        if( _astro_app_task )
                                            lv_task_del(_astro_app_task);
                                        _astro_app_task = NULL;
                                        lv_obj_set_hidden(astro_app_main_start_btn, false);
                                        lv_obj_set_hidden(astro_app_main_stop_btn, true);
                                        astro_remove_widget();
                                        astro_app_hide_app_icon_info( true );
                                        break;
    }
}

static void reset_astro_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       log_i("Reset clicked");
                                        astro_milliseconds = 0;
                                        astro_app_main_update_astrolabel();
                                        break;
    }
}


static void exit_astro_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}

void astro_app_task( lv_task_t * task ) {
    time_t now = time(0);
    double dif_seconds = difftime(now, prev_time);

    astro_milliseconds += dif_seconds * 1000;
    prev_time = now;

    astro_app_main_update_astrolabel();
}
