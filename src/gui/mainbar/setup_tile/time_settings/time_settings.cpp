#include "config.h"
#include "time_settings.h"
#include <WiFi.h>

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "hardware/timesync.h"

lv_obj_t *time_settings_tile=NULL;
lv_style_t time_settings_style;
lv_obj_t *utczone_list = NULL;
lv_obj_t *wifisync_onoff = NULL;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(time_32px);

static void exit_time_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void wifisync_onoff_event_handler(lv_obj_t * obj, lv_event_t event);
static void utczone_event_handler(lv_obj_t * obj, lv_event_t event);

void time_settings_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {
    lv_style_init( &time_settings_style );
    lv_style_set_radius( &time_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color( &time_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &time_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &time_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &time_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor( &time_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    time_settings_tile = lv_obj_create( tile, NULL);
    lv_obj_set_size(time_settings_tile, hres , vres);
    lv_obj_align(time_settings_tile, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style( time_settings_tile, LV_OBJ_PART_MAIN, &time_settings_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( time_settings_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( exit_btn, time_settings_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_time_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( time_settings_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( exit_label, "time settings");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *wifisync_cont = lv_obj_create( time_settings_tile, NULL );
    lv_obj_set_size(wifisync_cont, hres , 40);
    lv_obj_add_style( wifisync_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( wifisync_cont, time_settings_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    wifisync_onoff = lv_switch_create( wifisync_cont, NULL );
    lv_switch_off( wifisync_onoff, LV_ANIM_ON );
    lv_obj_align( wifisync_onoff, wifisync_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
    lv_obj_set_event_cb( wifisync_onoff, wifisync_onoff_event_handler );
    lv_obj_t *wifisync_label = lv_label_create( wifisync_cont, NULL);
    lv_obj_add_style( wifisync_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( wifisync_label, "sync when connect");
    lv_obj_align( wifisync_label, wifisync_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *utczone_cont = lv_obj_create( time_settings_tile, NULL );
    lv_obj_set_size(utczone_cont, hres , 40);
    lv_obj_add_style( utczone_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( utczone_cont, wifisync_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );    
    lv_obj_t *utczone_label = lv_label_create( utczone_cont, NULL);
    lv_obj_add_style( utczone_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( utczone_label, "utc timezone");
    lv_obj_align( utczone_label, utczone_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    utczone_list = lv_dropdown_create( utczone_cont, NULL);
    lv_dropdown_set_options( utczone_list, "-12\n-11\n-10\n-9\n-8\n-7\n-6\n-5\n-4\n-3\n-2\n-1\n0\n+1\n+2\n+3\n+4\n+5\n+6\n+7\n+8\n+9\n+10\n+11\n+12\n" );
    lv_obj_align(utczone_list, utczone_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_set_event_cb(utczone_list, utczone_event_handler);

    if ( timesync_get_timesync() )
        lv_switch_on( wifisync_onoff, LV_ANIM_OFF );
    else
        lv_switch_off( wifisync_onoff, LV_ANIM_OFF );

    lv_dropdown_set_selected( utczone_list, timesync_get_timezone() + 12 );
}

static void exit_time_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( SETUP_TILE, LV_ANIM_OFF );
                                        break;
    }
}

static void wifisync_onoff_event_handler(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_VALUE_CHANGED) {
        if( lv_switch_get_state( obj ) ) {
            timesync_set_timesync( true );
        }
        else {
            timesync_set_timesync( false );
        }
    }
}

static void utczone_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        timesync_set_timezone( lv_dropdown_get_selected( obj ) - 12 );
        printf("timezone: %d\n", lv_dropdown_get_selected( obj ) - 12 );
    }
}
