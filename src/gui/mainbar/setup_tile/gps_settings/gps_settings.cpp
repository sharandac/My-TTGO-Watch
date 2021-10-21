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
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/gpsctl.h"

lv_obj_t *gps_settings_tile = NULL;
uint32_t gps_tile_num;

lv_obj_t *autoon_onoff = NULL;
lv_obj_t *enable_on_standby_onoff = NULL;
lv_obj_t *app_use_gps_onoff = NULL;
lv_obj_t *fakegps_onoff = NULL;
lv_obj_t *gps_latlon_label = NULL;
lv_obj_t *gps_port_list = NULL;

LV_IMG_DECLARE(gps_64px);

static void enter_gps_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_gps_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void autoon_onoff_event_handler( lv_obj_t * obj, lv_event_t event);
static void enable_on_standby_onoff_event_handler( lv_obj_t * obj, lv_event_t event);
static void app_use_gps_onoff_event_handler( lv_obj_t * obj, lv_event_t event);
static void fakegps_onoff_event_handler( lv_obj_t * obj, lv_event_t event);
static void gps_port_list_event_handler( lv_obj_t *obj, lv_event_t event );
bool gps_settings_config_update_cb( EventBits_t event, void *arg );
bool gps_settings_latlon_update_cb( EventBits_t event, void *arg );

void gps_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    gps_tile_num = mainbar_add_setup_tile( 1, 1, "gps settings" );
    gps_settings_tile = mainbar_get_tile_obj( gps_tile_num );

    lv_obj_add_style( gps_settings_tile, LV_OBJ_PART_MAIN, SETUP_STYLE );

    icon_t *gps_setup_icon = setup_register( "gps", &gps_64px, enter_gps_setup_event_cb );
    setup_hide_indicator( gps_setup_icon );
    /**
     * add setup header
     */
    lv_obj_t *header = wf_add_settings_header( gps_settings_tile, "gps settings", exit_gps_setup_event_cb );
    lv_obj_align( header, gps_settings_tile, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, STATUSBAR_HEIGHT + THEME_PADDING );
    /**
     * add autoon switch
     */
    lv_obj_t *autoon_cont = wf_add_labeled_switch( gps_settings_tile, "autoon", &autoon_onoff, gpsctl_get_autoon(), autoon_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( autoon_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );
    /**
     * add enable on standby switch
     */
    lv_obj_t *enable_on_standby_cont = wf_add_labeled_switch( gps_settings_tile, "enable on standby", &enable_on_standby_onoff, gpsctl_get_enable_on_standby(), enable_on_standby_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( enable_on_standby_cont, autoon_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );
    /**
     * add app use gps switch
     */
    lv_obj_t *app_use_gps_cont = wf_add_labeled_switch( gps_settings_tile, "apps use gps", &app_use_gps_onoff, gpsctl_get_app_use_gps(), app_use_gps_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( app_use_gps_cont, enable_on_standby_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );
    /**
     * add fake gps via ip switch
     */
    lv_obj_t *fakegps_cont = wf_add_labeled_switch( gps_settings_tile, "fake gps via ip", &fakegps_onoff, gpsctl_get_gps_over_ip(), fakegps_onoff_event_handler, SETUP_STYLE );
    lv_obj_align( fakegps_cont, app_use_gps_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    #if defined( M5PAPER )
        lv_obj_t *gps_port_cont = wf_add_labeled_list( gps_settings_tile, "gps port (need reboot)", &gps_port_list, "PORT.A\nPORT.B\nPORT.C\nNONE", gps_port_list_event_handler, SETUP_STYLE );
        lv_obj_align( gps_port_cont, fakegps_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );
    #endif

    gps_latlon_label = lv_label_create( gps_settings_tile, NULL);
    lv_obj_add_style( gps_latlon_label, LV_OBJ_PART_MAIN, ws_get_mainbar_style()  );
    lv_label_set_text( gps_latlon_label, "fix: - lat: - lon: -");
    lv_obj_align( gps_latlon_label, gps_settings_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -5 );

    gpsctl_register_cb( GPSCTL_FIX | GPSCTL_NOFIX | GPSCTL_UPDATE_LOCATION, gps_settings_latlon_update_cb, "gps settings" );
    gpsctl_register_cb( GPSCTL_UPDATE_CONFIG, gps_settings_config_update_cb, "gps settings" );
}

bool gps_settings_config_update_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case GPSCTL_UPDATE_CONFIG:
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

            if ( gpsctl_get_gps_over_ip() )
                lv_switch_on( fakegps_onoff, LV_ANIM_OFF );
            else
                lv_switch_off( fakegps_onoff, LV_ANIM_OFF );

            #if defined( M5PAPER )
                int8_t rx,tx;
                gpsctl_get_gps_rx_tx_pin( &rx, &tx );
                switch( rx ) {
                    case 32:    lv_dropdown_set_selected( gps_port_list, 0 );
                                break;
                    case 33:    lv_dropdown_set_selected( gps_port_list, 1 );
                                break;
                    case 19:    lv_dropdown_set_selected( gps_port_list, 2 );
                                break;
                    default:    lv_dropdown_set_selected( gps_port_list, 3 );
                }
            #endif

            break;
    }
    return( true );
}

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

static void enter_gps_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( gps_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_gps_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}

static void gps_port_list_event_handler( lv_obj_t *obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):
            uint16_t port = lv_dropdown_get_selected( obj );
            switch( port ) {
                case 0:     gpsctl_set_gps_rx_tx_pin( 32, 25 );
                            break;
                case 1:     gpsctl_set_gps_rx_tx_pin( 33, 26 );
                            break;
                case 2:     gpsctl_set_gps_rx_tx_pin( 19, 18 );
                            break;
                default:    gpsctl_set_gps_rx_tx_pin( -1, -1 );
                            break;
            }
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

static void fakegps_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED):  gpsctl_set_gps_over_ip( lv_switch_get_state( obj ) );
    }
}

uint32_t gps_get_setup_tile_num( void ) {
    return( gps_tile_num );
}