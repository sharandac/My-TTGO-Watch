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
#include "gps_settings.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"

#include "hardware/gpsctl.h"

lv_obj_t *gps_settings_tile = NULL;
lv_style_t gps_settings_style;
uint32_t gps_tile_num;

lv_obj_t *autoon_onoff = NULL;
lv_obj_t *enable_on_standby_onoff = NULL;
lv_obj_t *app_use_gps_onoff = NULL;
lv_obj_t *app_control_permission_onoff = NULL;
lv_obj_t *fakegps_onoff = NULL;
//lv_obj_t *gps_latlon_label = NULL;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(gps_64px);

static void enter_gps_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_gps_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void autoon_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void enable_on_standby_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void app_use_gps_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void app_control_permission_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void fakegps_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
//bool gps_settings_latlon_update_cb( EventBits_t event, void *arg );

void gps_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    gps_tile_num = mainbar_add_app_tile( 1, 1, "gps settings" );
    gps_settings_tile = mainbar_get_tile_obj( gps_tile_num );
    lv_style_copy( &gps_settings_style, mainbar_get_style() );
    lv_style_set_bg_color( &gps_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &gps_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &gps_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_obj_add_style( gps_settings_tile, LV_OBJ_PART_MAIN, &gps_settings_style );

    icon_t *gps_setup_icon = setup_register( "gps", &gps_64px, enter_gps_setup_event_cb );
    setup_hide_indicator( gps_setup_icon );

    lv_obj_t *exit_btn = lv_imgbtn_create( gps_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, &gps_settings_style );
    lv_obj_align( exit_btn, gps_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_gps_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( gps_settings_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, &gps_settings_style );
    lv_label_set_text( exit_label, "gps settings");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *autoon_cont = lv_obj_create( gps_settings_tile, NULL );
    lv_obj_set_size(autoon_cont, lv_disp_get_hor_res( NULL ) , 32);
    lv_obj_add_style( autoon_cont, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_obj_align( autoon_cont, gps_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    autoon_onoff = lv_switch_create( autoon_cont, NULL );
    lv_obj_add_protect( autoon_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( autoon_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( autoon_onoff, LV_ANIM_ON );
    lv_obj_align( autoon_onoff, autoon_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( autoon_onoff, autoon_onoff_event_handler );
    lv_obj_t *autoon_label = lv_label_create( autoon_cont, NULL);
    lv_obj_add_style( autoon_label, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_label_set_text( autoon_label, "autoon");
    lv_obj_align( autoon_label, autoon_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *enable_on_standby_cont = lv_obj_create( gps_settings_tile, NULL );
    lv_obj_set_size(enable_on_standby_cont, lv_disp_get_hor_res( NULL ) , 32);
    lv_obj_add_style( enable_on_standby_cont, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_obj_align( enable_on_standby_cont, autoon_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    enable_on_standby_onoff = lv_switch_create( enable_on_standby_cont, NULL );
    lv_obj_add_protect( enable_on_standby_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( enable_on_standby_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( enable_on_standby_onoff, LV_ANIM_ON );
    lv_obj_align( enable_on_standby_onoff, enable_on_standby_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( enable_on_standby_onoff, enable_on_standby_onoff_event_handler );
    lv_obj_t *enable_on_standby_label = lv_label_create( enable_on_standby_cont, NULL);
    lv_obj_add_style( enable_on_standby_label, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_label_set_text( enable_on_standby_label, "enable on standby");
    lv_obj_align( enable_on_standby_label, enable_on_standby_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *app_use_gps_cont = lv_obj_create( gps_settings_tile, NULL );
    lv_obj_set_size(app_use_gps_cont, lv_disp_get_hor_res( NULL ) , 32);
    lv_obj_add_style( app_use_gps_cont, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_obj_align( app_use_gps_cont, enable_on_standby_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    app_use_gps_onoff = lv_switch_create( app_use_gps_cont, NULL );
    lv_obj_add_protect( app_use_gps_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( app_use_gps_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( app_use_gps_onoff, LV_ANIM_ON );
    lv_obj_align( app_use_gps_onoff, app_use_gps_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( app_use_gps_onoff, app_use_gps_onoff_event_handler );
    lv_obj_t *app_use_gps_label = lv_label_create( app_use_gps_cont, NULL);
    lv_obj_add_style( app_use_gps_label, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_label_set_text( app_use_gps_label, "apps use gps");
    lv_obj_align( app_use_gps_label, app_use_gps_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *app_control_gps_cont = lv_obj_create( gps_settings_tile, NULL );
    lv_obj_set_size(app_control_gps_cont, lv_disp_get_hor_res( NULL ) , 32);
    lv_obj_add_style( app_control_gps_cont, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_obj_align( app_control_gps_cont, app_use_gps_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    app_control_permission_onoff = lv_switch_create( app_control_gps_cont, NULL );
    lv_obj_add_protect( app_control_permission_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( app_control_permission_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( app_control_permission_onoff, LV_ANIM_ON );
    lv_obj_align( app_control_permission_onoff, app_control_gps_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( app_control_permission_onoff, app_control_permission_onoff_event_handler );
    lv_obj_t *app_control_gps_label = lv_label_create( app_control_gps_cont, NULL);
    lv_obj_add_style( app_control_gps_label, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_label_set_text( app_control_gps_label, "apps control gps");
    lv_obj_align( app_control_gps_label, app_control_gps_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *fakegps_cont = lv_obj_create( gps_settings_tile, NULL );
    lv_obj_set_size(fakegps_cont, lv_disp_get_hor_res( NULL ) , 32);
    lv_obj_add_style( fakegps_cont, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_obj_align( fakegps_cont, app_control_gps_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    fakegps_onoff = lv_switch_create( fakegps_cont, NULL );
    lv_obj_add_protect( fakegps_onoff, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( fakegps_onoff, LV_SWITCH_PART_INDIC, mainbar_get_switch_style() );
    lv_switch_off( fakegps_onoff, LV_ANIM_ON );
    lv_obj_align( fakegps_onoff, fakegps_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( fakegps_onoff, fakegps_onoff_event_handler );
    lv_obj_t *fakegps_label = lv_label_create( fakegps_cont, NULL);
    lv_obj_add_style( fakegps_label, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_label_set_text( fakegps_label, "fake gps via ip");
    lv_obj_align( fakegps_label, fakegps_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    /*
    gps_latlon_label = lv_label_create( gps_settings_tile, NULL);
    lv_obj_add_style( gps_latlon_label, LV_OBJ_PART_MAIN, &gps_settings_style  );
    lv_label_set_text( gps_latlon_label, "fix: - lat: - lon: -");
    lv_obj_align( gps_latlon_label, gps_settings_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -5 );
    */

    if ( gpsctl_get_autoon() )
        lv_switch_on( autoon_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( autoon_onoff, LV_ANIM_OFF );

    if ( gpsctl_get_enable_on_standby() )
        lv_switch_on( enable_on_standby_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( enable_on_standby_onoff, LV_ANIM_OFF );

    if ( gpsctl_get_app_use_gps() )
        lv_switch_on( app_use_gps_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( app_use_gps_onoff, LV_ANIM_OFF );

    if ( gpsctl_get_app_control_gps() )
        lv_switch_on( app_control_permission_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( app_control_permission_onoff, LV_ANIM_OFF );

    if ( gpsctl_get_gps_over_ip() )
        lv_switch_on( fakegps_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( fakegps_onoff, LV_ANIM_OFF );

    //gpsctl_register_cb( GPSCTL_FIX | GPSCTL_NOFIX | GPSCTL_UPDATE_LOCATION, gps_settings_latlon_update_cb, "gps settings" );

}

/*
bool gps_settings_latlon_update_cb( EventBits_t event, void *arg ) {
    static bool gpsfix = false;
    static double lat = 0;
    static double lon = 0;
    gps_data_t *gps_data = NULL;
    char msg[64] = "";

    switch( event ) {
        case GPSCTL_UPDATE_LOCATION:
            gps_data = (gps_data_t*)arg;
            lat = gps_data->lat;
            lon = gps_data->lon;
            break;
        case GPSCTL_NOFIX:
            gpsfix = false;
            break;
        case GPSCTL_FIX:
            gpsfix = true;
            break;
    }
    snprintf( msg, sizeof( msg ), "%s @ lat: %.3f lon: %.3f", gpsfix?"fix":"nofix", lat, lon );
    lv_label_set_text( gps_latlon_label, msg );
    lv_obj_align( gps_latlon_label, gps_settings_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -5 );

    return( true );
}
*/

static void enter_gps_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( gps_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_gps_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
                                        break;
    }
}

static void autoon_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED):  gpsctl_set_autoon( lv_switch_get_state( obj ) );
    }
}

static void enable_on_standby_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED):  gpsctl_set_enable_on_standby( lv_switch_get_state( obj ) );
    }
}

static void app_use_gps_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED):  gpsctl_set_app_use_gps( lv_switch_get_state( obj ) );
    }
}

static void app_control_permission_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED):  gpsctl_set_app_control_gps( lv_switch_get_state( obj ) );
    }
}

static void fakegps_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED):  gpsctl_set_gps_over_ip( lv_switch_get_state( obj ) );
    }
}
