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
#include "time_settings.h"
#include <WiFi.h>

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "hardware/timesync.h"
#include "hardware/motor.h"

lv_obj_t *time_settings_tile=NULL;
lv_style_t time_settings_style;
uint32_t time_tile_num;

lv_obj_t *utczone_list = NULL;
lv_obj_t *wifisync_onoff = NULL;
lv_obj_t *daylight_onoff = NULL;
lv_obj_t *clock_fmt_onoff = NULL;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(time_32px);
LV_IMG_DECLARE(time_64px);

static void enter_time_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_time_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void wifisync_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void daylight_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void utczone_event_handler(lv_obj_t * obj, lv_event_t event);
static void clock_fmt_onoff_event_handler(lv_obj_t * obj, lv_event_t event);

void time_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    time_tile_num = mainbar_add_app_tile( 1, 1 );
    time_settings_tile = mainbar_get_tile_obj( time_tile_num );
    lv_style_copy( &time_settings_style, mainbar_get_style() );
    lv_style_set_bg_color( &time_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &time_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &time_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( time_settings_tile, LV_OBJ_PART_MAIN, &time_settings_style );

    icon_t *time_setup_icon = setup_register( "time", &time_64px, enter_time_setup_event_cb );
    setup_hide_indicator( time_setup_icon );

    lv_obj_t *exit_btn = lv_imgbtn_create( time_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &time_settings_style );
    lv_obj_align( exit_btn, time_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_time_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( time_settings_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_label_set_text( exit_label, "time settings");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *wifisync_cont = lv_obj_create( time_settings_tile, NULL );
    lv_obj_set_size(wifisync_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( wifisync_cont, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_obj_align( wifisync_cont, time_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    wifisync_onoff = lv_switch_create( wifisync_cont, NULL );
    lv_obj_add_protect( wifisync_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( wifisync_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( wifisync_onoff, LV_ANIM_ON );
    lv_obj_align( wifisync_onoff, wifisync_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( wifisync_onoff, wifisync_onoff_event_handler );
    lv_obj_t *wifisync_label = lv_label_create( wifisync_cont, NULL);
    lv_obj_add_style( wifisync_label, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_label_set_text( wifisync_label, "sync when connect");
    lv_obj_align( wifisync_label, wifisync_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *daylight_cont = lv_obj_create( time_settings_tile, NULL );
    lv_obj_set_size(daylight_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( daylight_cont, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_obj_align( daylight_cont, wifisync_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    daylight_onoff = lv_switch_create( daylight_cont, NULL );
    lv_obj_add_protect( daylight_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( daylight_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( daylight_onoff, LV_ANIM_ON );
    lv_obj_align( daylight_onoff, daylight_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( daylight_onoff, daylight_onoff_event_handler );
    lv_obj_t *daylight_label = lv_label_create( daylight_cont, NULL);
    lv_obj_add_style( daylight_label, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_label_set_text( daylight_label, "daylight saving");
    lv_obj_align( daylight_label, daylight_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *utczone_cont = lv_obj_create( time_settings_tile, NULL );
    lv_obj_set_size(utczone_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( utczone_cont, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_obj_align( utczone_cont, daylight_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );    
    lv_obj_t *utczone_label = lv_label_create( utczone_cont, NULL);
    lv_obj_add_style( utczone_label, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_label_set_text( utczone_label, "utc timezone");
    lv_obj_align( utczone_label, utczone_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    utczone_list = lv_dropdown_create( utczone_cont, NULL);
    lv_dropdown_set_options( utczone_list, "-12\n-11\n-10\n-9\n-8\n-7\n-6\n-5\n-4\n-3\n-2\n-1\n0\n+1\n+2\n+3\n+4\n+5\n+6\n+7\n+8\n+9\n+10\n+11\n+12" );
    lv_obj_set_size( utczone_list, 70, 40 );
    lv_obj_align(utczone_list, utczone_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_set_event_cb(utczone_list, utczone_event_handler);

    lv_obj_t *clock_fmt_cont = lv_obj_create( time_settings_tile, NULL );
    lv_obj_set_size(clock_fmt_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( clock_fmt_cont, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_obj_align( clock_fmt_cont, utczone_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    clock_fmt_onoff = lv_switch_create( clock_fmt_cont, NULL );
    lv_obj_add_protect( clock_fmt_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( clock_fmt_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( clock_fmt_onoff, LV_ANIM_ON );
    lv_obj_align( clock_fmt_onoff, clock_fmt_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( clock_fmt_onoff, clock_fmt_onoff_event_handler );
    lv_obj_t *clock_fmt_label = lv_label_create( clock_fmt_cont, NULL);
    lv_obj_add_style( clock_fmt_label, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_label_set_text( clock_fmt_label, "use 24hr clock");
    lv_obj_align( clock_fmt_label, clock_fmt_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    if ( timesync_get_timesync() )
        lv_switch_on( wifisync_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( wifisync_onoff, LV_ANIM_OFF );

    if ( timesync_get_daylightsave() )
        lv_switch_on( daylight_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( daylight_onoff, LV_ANIM_OFF );

    if ( timesync_get_24hr() )
        lv_switch_on( clock_fmt_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( clock_fmt_onoff, LV_ANIM_OFF );

    lv_dropdown_set_selected( utczone_list, timesync_get_timezone() + 12 );
}

static void enter_time_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( time_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_time_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
                                        break;
    }
}

static void wifisync_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED):     timesync_set_timesync( lv_switch_get_state( obj ) );
    }
}

static void daylight_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED):     timesync_set_daylightsave( lv_switch_get_state( obj ) );
    }
}

static void utczone_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED):     timesync_set_timezone( lv_dropdown_get_selected( obj ) - 12 );
    }
}

static void clock_fmt_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED):     timesync_set_24hr( lv_switch_get_state( obj ) );
    }
}
