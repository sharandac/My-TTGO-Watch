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
#include "hardware/alloc.h"

#include "hardware/json_psram_allocator.h"
// Source: https://raw.githubusercontent.com/nayarsystems/posix_tz_db/master/zones.json
// 2020a-1
extern const uint8_t timezones_json_start[] asm("_binary_src_gui_mainbar_setup_tile_time_settings_timezones_json_start");
extern const uint8_t timezones_json_end[] asm("_binary_src_gui_mainbar_setup_tile_time_settings_timezones_json_end");
const size_t capacity = JSON_OBJECT_SIZE(460) + 14920;
String timezone = String("");
String regionlist = String("");
String locationlist = String("");
String region = String("");
String location = String("");

lv_obj_t *time_settings_tile=NULL;
lv_style_t time_settings_style;
uint32_t time_tile_num;

lv_obj_t *region_list = NULL;
lv_obj_t *location_list = NULL;
lv_obj_t *wifisync_onoff = NULL;
lv_obj_t *clock_fmt_onoff = NULL;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(time_32px);
LV_IMG_DECLARE(time_64px);

static void enter_time_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_time_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void wifisync_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void region_event_handler(lv_obj_t * obj, lv_event_t event);
static void location_event_handler(lv_obj_t * obj, lv_event_t event);
static void clock_fmt_onoff_event_handler(lv_obj_t * obj, lv_event_t event);

static void time_setting_set_region_location( const char *timezone ) {
    char *timezone_tmp = NULL;
    char *region_tmp = NULL;
    char *location_tmp = NULL;

    timezone_tmp = (char*)MALLOC( strlen( timezone ) + 1 );
    if ( !timezone_tmp ) {
        log_e("timezone_tmp malloc failed");
        while(1);
    }
    strlcpy( timezone_tmp, timezone, strlen( timezone ) + 1 );

    region_tmp = timezone_tmp;
    location_tmp = timezone_tmp;
    while( location_tmp ) {
        if ( *location_tmp == '/' ) {
            *location_tmp = '\0';
            location_tmp++;
            break;
        }
        location_tmp++;
    }
    location = location_tmp;
    region = region_tmp;
    free( timezone_tmp );
    log_i("timezone = %s", timezone );
    log_i("region = %s", region.c_str() );
    log_i("location = %s", location.c_str() );
}

int32_t time_settings_create_regionlist( const char* selected_region ) {
    int32_t selected_entry = 0, entry = -1;
    SpiRamJsonDocument doc( capacity );
    DeserializationError error = deserializeJson( doc, (const char *)timezones_json_start );

    if ( error ) {
        log_e("timezones deserializeJson() failed: %s", error.c_str() );
    }
    else {
        regionlist = "";
        JsonObject obj = doc.as<JsonObject>();
        for ( JsonPair p : obj ) {
            int len = strlen( p.key().c_str() ) + 1 ;
            char * region = (char*)MALLOC( len );
            strlcpy( region, p.key().c_str(), len );
            char * key = region;
            while( key ) {
                if ( *key == '/' ) {
                    *key = '\0';
                    break;
                }
                key++;
            }
            if (!strstr( regionlist.c_str(), region ) ) {
                entry++;
                if ( entry != 0 )
                    regionlist += "\n";
                regionlist += region;
                if ( !strcmp( selected_region, region ) )
                    selected_entry = entry;
            }
            free( region );
        }        
    }
    doc.clear();
    log_i("selected region entry = %d", selected_entry );
    return( selected_entry );
}

int32_t time_settings_create_locationlist( const char* selected_region, const char* selected_location ) {
    int32_t selected_entry = 0, entry = -1;

    SpiRamJsonDocument doc( capacity );
    DeserializationError error = deserializeJson( doc, (const char *)timezones_json_start );

    if ( error ) {
        log_e("timezones deserializeJson() failed: %s", error.c_str() );
    }
    else {
        locationlist = "";
        JsonObject obj = doc.as<JsonObject>();
        for ( JsonPair p : obj ) {
            int len = strlen( p.key().c_str() ) +  1;
            char * region = (char*)MALLOC( len );
            strlcpy( region, p.key().c_str(), len );
            char * location = region;
            while( location ) {
                if ( *location == '/' ) {
                    *location = '\0';
                    location++;
                    break;
                }
                location++;
            }
            if ( strstr( selected_region, region ) ) {
                entry++;
                if ( entry != 0 )
                    locationlist += "\n";
                locationlist += location;
                if ( !strcmp( selected_location, location ) )
                    selected_entry = entry;
            }
            free( region );
        }        
    }
    doc.clear();
    log_i("selected location entry = %d", selected_entry );
    return( selected_entry );
}

static void time_settings_set_timezone_timerule( void ) {
    char region_str[32] = "";
    lv_dropdown_get_selected_str( region_list, region_str, sizeof( region_str ) );
    region = region_str;
    char location_str[32] = "";
    lv_dropdown_get_selected_str( location_list, location_str, sizeof( location_str ) );
    location = location_str;

    timezone = region + "/" + location;
    timesync_set_timezone_name( (char*)timezone.c_str() );

    SpiRamJsonDocument doc( capacity );
    DeserializationError error = deserializeJson( doc, (const char *)timezones_json_start );
    if ( error ) {
        log_e("timezones deserializeJson() failed: %s", error.c_str() );
        return;
    }
    else {
        const char * timezone_rule = doc[ timezone.c_str() ];
        timesync_set_timezone_rule( timezone_rule );
    }
    doc.clear();

    log_i("set timezone \"%s\" and timerule \"%s\"", timesync_get_timezone_name() , timesync_get_timezone_rule() );

}

void time_settings_tile_setup( void ) {
    int32_t selected_region = 0;
    int32_t selected_location = 0;

    timezone =+ timesync_get_timezone_name();
    time_setting_set_region_location( timezone.c_str() );
    selected_region = time_settings_create_regionlist( region.c_str() );
    selected_location = time_settings_create_locationlist( region.c_str(), location.c_str() );

    // get an app tile and copy mainstyle
    time_tile_num = mainbar_add_app_tile( 1, 1, "time setup" );
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

    lv_obj_t *clock_fmt_cont = lv_obj_create( time_settings_tile, NULL );
    lv_obj_set_size(clock_fmt_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( clock_fmt_cont, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_obj_align( clock_fmt_cont, wifisync_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
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

    lv_obj_t *region_cont = lv_obj_create( time_settings_tile, NULL );
    lv_obj_set_size( region_cont, lv_disp_get_hor_res( NULL ) , 40 );
    lv_obj_add_style( region_cont, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_obj_align( region_cont, clock_fmt_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *region_label = lv_label_create( region_cont, NULL);
    lv_obj_add_style( region_label, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_label_set_text( region_label, "region");
    lv_obj_align( region_label, region_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    region_list = lv_dropdown_create( region_cont, NULL);
    lv_dropdown_set_options( region_list, regionlist.c_str() );
    lv_obj_set_size( region_list, lv_disp_get_hor_res( NULL )/2, 35 );
    lv_obj_align( region_list, region_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( region_list, region_event_handler);
    lv_dropdown_set_selected( region_list, selected_region );

    lv_obj_t *location_cont = lv_obj_create( time_settings_tile, NULL );
    lv_obj_set_size( location_cont, lv_disp_get_hor_res( NULL ) , 40 );
    lv_obj_add_style( location_cont, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_obj_align( location_cont, region_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_t *location_label = lv_label_create( location_cont, NULL);
    lv_obj_add_style( location_label, LV_OBJ_PART_MAIN, &time_settings_style  );
    lv_label_set_text( location_label, "location");
    lv_obj_align( location_label, location_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    location_list = lv_dropdown_create( location_cont, NULL);
    lv_dropdown_set_options( location_list, locationlist.c_str() );
    lv_obj_set_size( location_list, lv_disp_get_hor_res( NULL )/2, 35 );
    lv_obj_align( location_list, location_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( location_list, location_event_handler);
    lv_dropdown_set_selected( location_list, selected_location );

    if ( timesync_get_timesync() )
        lv_switch_on( wifisync_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( wifisync_onoff, LV_ANIM_OFF );

    if ( timesync_get_24hr() )
        lv_switch_on( clock_fmt_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( clock_fmt_onoff, LV_ANIM_OFF );
}

static void enter_time_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( time_tile_num, LV_ANIM_OFF );
                                        break;
    }
}

static void exit_time_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       time_settings_set_timezone_timerule();
                                        mainbar_jump_to_tilenumber( setup_get_tile_num(), LV_ANIM_OFF );
                                        break;
    }
}

static void wifisync_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED):     timesync_set_timesync( lv_switch_get_state( obj ) );
    }
}

static void region_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED):     char region_str[32] = "";
                                            lv_dropdown_get_selected_str( obj, region_str, sizeof( region_str ) );
                                            region = region_str;
                                            time_settings_create_locationlist( region.c_str(), location.c_str() );
                                            lv_dropdown_set_options( location_list, locationlist.c_str() );
                                            lv_obj_invalidate( lv_scr_act() );
                                            time_settings_set_timezone_timerule();
                                            break;
    }
}

static void location_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED):     char location_str[32] = "";
                                            lv_dropdown_get_selected_str( obj, location_str, sizeof( location_str ) );
                                            location = location_str;
                                            time_settings_set_timezone_timerule();
                                            break;
    }
}

static void clock_fmt_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    switch( event ) {
        case ( LV_EVENT_VALUE_CHANGED):     timesync_set_24hr( lv_switch_get_state( obj ) );
    }
}
