#include "config.h"
#include "setup.h"

#include "gui/mainbar/mainbar.h"

static void wifi_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void battery_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void move_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void display_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void time_setup_event_cb( lv_obj_t * obj, lv_event_t event );

LV_IMG_DECLARE(wifi_64px);
LV_IMG_DECLARE(battery_icon_64px);
LV_IMG_DECLARE(move_64px);
LV_IMG_DECLARE(brightness_64px);
LV_IMG_DECLARE(time_64px);


void setup_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {

    lv_obj_t * wifi_setup = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src(wifi_setup, LV_BTN_STATE_RELEASED, &wifi_64px);
    lv_imgbtn_set_src(wifi_setup, LV_BTN_STATE_PRESSED, &wifi_64px);
    lv_imgbtn_set_src(wifi_setup, LV_BTN_STATE_CHECKED_RELEASED, &wifi_64px);
    lv_imgbtn_set_src(wifi_setup, LV_BTN_STATE_CHECKED_PRESSED, &wifi_64px);
    lv_obj_add_style(wifi_setup, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align(wifi_setup, tile, LV_ALIGN_IN_TOP_LEFT, 16, 48 );
    lv_obj_set_event_cb( wifi_setup, wifi_setup_event_cb );

    lv_obj_t * battery_setup = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src( battery_setup, LV_BTN_STATE_RELEASED, &battery_icon_64px);
    lv_imgbtn_set_src( battery_setup, LV_BTN_STATE_PRESSED, &battery_icon_64px);
    lv_imgbtn_set_src( battery_setup, LV_BTN_STATE_CHECKED_RELEASED, &battery_icon_64px);
    lv_imgbtn_set_src( battery_setup, LV_BTN_STATE_CHECKED_PRESSED, &battery_icon_64px);
    lv_obj_add_style( battery_setup, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( battery_setup, tile, LV_ALIGN_IN_TOP_LEFT, 16+70, 48 );
    lv_obj_set_event_cb( battery_setup, battery_setup_event_cb );

    lv_obj_t * move_setup = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src( move_setup, LV_BTN_STATE_RELEASED, &move_64px);
    lv_imgbtn_set_src( move_setup, LV_BTN_STATE_PRESSED, &move_64px);
    lv_imgbtn_set_src( move_setup, LV_BTN_STATE_CHECKED_RELEASED, &move_64px);
    lv_imgbtn_set_src( move_setup, LV_BTN_STATE_CHECKED_PRESSED, &move_64px);
    lv_obj_add_style( move_setup, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( move_setup, tile, LV_ALIGN_IN_TOP_LEFT, 16, 48+86 );
    lv_obj_set_event_cb( move_setup, move_setup_event_cb );

    lv_obj_t * brightness_setup = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src( brightness_setup, LV_BTN_STATE_RELEASED, &brightness_64px);
    lv_imgbtn_set_src( brightness_setup, LV_BTN_STATE_PRESSED, &brightness_64px);
    lv_imgbtn_set_src( brightness_setup, LV_BTN_STATE_CHECKED_RELEASED, &brightness_64px);
    lv_imgbtn_set_src( brightness_setup, LV_BTN_STATE_CHECKED_PRESSED, &brightness_64px);
    lv_obj_add_style( brightness_setup, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( brightness_setup, tile, LV_ALIGN_IN_TOP_LEFT, 16+70, 48+86 );
    lv_obj_set_event_cb( brightness_setup, display_setup_event_cb );

    lv_obj_t * time_setup = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src( time_setup, LV_BTN_STATE_RELEASED, &time_64px);
    lv_imgbtn_set_src( time_setup, LV_BTN_STATE_PRESSED, &time_64px);
    lv_imgbtn_set_src( time_setup, LV_BTN_STATE_CHECKED_RELEASED, &time_64px);
    lv_imgbtn_set_src( time_setup, LV_BTN_STATE_CHECKED_PRESSED, &time_64px);
    lv_obj_add_style( time_setup, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( time_setup, tile, LV_ALIGN_IN_TOP_LEFT, 16+70+70, 48 );
    lv_obj_set_event_cb( time_setup, time_setup_event_cb );
}

static void wifi_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( WLAN_SETTINGS_TILE, LV_ANIM_OFF );
                                        break;
    }
}

static void battery_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( BATTERY_SETTINGS_TILE, LV_ANIM_OFF );
                                        break;
    }
}

static void move_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( MOVE_SETTINGS_TILE, LV_ANIM_OFF );
                                        break;
    }
}

static void display_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( DISPLAY_SETTINGS_TILE, LV_ANIM_OFF );
                                        break;
    }
}

static void time_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( TIME_SETTINGS_TILE, LV_ANIM_OFF );
                                        break;
    }
}