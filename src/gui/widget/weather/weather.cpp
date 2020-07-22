#include "config.h"
#include <TTGO.h>
#include "weather.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/statusbar.h"

lv_tile_number weather_widget_tile_num = NO_TILE;
lv_tile_number weather_widget_setup_tile_num = NO_TILE;
lv_obj_t *widget_cont = NULL;
lv_obj_t *widget_weather_condition_img = NULL;
lv_obj_t *widget_weather_temperature_label = NULL;

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
LV_IMG_DECLARE(weather_48px);
LV_FONT_DECLARE(Ubuntu_16px);

void weather_widget_setup( void ) {
    // get a free widget tile and a widget setup tile
    weather_widget_tile_num = mainbar_get_next_free_tile( TILE_TYPE_WIDGET_TILE );
    weather_widget_setup_tile_num = mainbar_get_next_free_tile( TILE_TYPE_WIDGET_SETUP );
    // register the widget setup function
    mainbar_set_tile_setup_cb( weather_widget_tile_num, weather_widget_tile_setup );
    mainbar_set_tile_setup_cb( weather_widget_setup_tile_num, weather_widget_setup_tile_setup );

    widget_cont = main_tile_register_widget();

    widget_weather_condition_img = lv_imgbtn_create( widget_cont, NULL );
    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_RELEASED, &weather_48px);
    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_PRESSED, &weather_48px);
    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_CHECKED_RELEASED, &weather_48px);
    lv_imgbtn_set_src( widget_weather_condition_img, LV_BTN_STATE_CHECKED_PRESSED, &weather_48px);
    lv_obj_reset_style_list( widget_weather_condition_img, LV_OBJ_PART_MAIN );
    lv_obj_align( widget_weather_condition_img , widget_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( widget_weather_condition_img, enter_weather_widget_event_cb );

    widget_weather_temperature_label = lv_label_create( widget_cont , NULL);
    lv_label_set_text(widget_weather_temperature_label, "n/a");
    lv_obj_reset_style_list( widget_weather_temperature_label, LV_OBJ_PART_MAIN );
    lv_obj_align(widget_weather_temperature_label, widget_cont, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);    
}

static void enter_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( weather_widget_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

/*
 *
 */
lv_obj_t *weather_widget_setup_tile = NULL;
lv_style_t weather_widget_setup_style;

LV_IMG_DECLARE(exit_32px);

static void exit_weather_widget_setup_event_cb( lv_obj_t * obj, lv_event_t event );

void weather_widget_setup_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {
    Serial.printf("setup widget setup tile\r\n");
    lv_style_init( &weather_widget_setup_style );
    lv_style_set_radius( &weather_widget_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &weather_widget_setup_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor( &weather_widget_setup_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    weather_widget_setup_tile = lv_obj_create( tile, NULL);
    lv_obj_set_size( weather_widget_setup_tile, hres , vres);
    lv_obj_align( weather_widget_setup_tile, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style( weather_widget_setup_tile, LV_OBJ_PART_MAIN, &weather_widget_setup_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( weather_widget_setup_tile, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( exit_btn, weather_widget_setup_tile, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_weather_widget_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( weather_widget_setup_tile, NULL);
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( exit_label, "exit weather setup");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );
}

static void exit_weather_widget_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( weather_widget_tile_num, LV_ANIM_ON );
                                        break;
    }
}

/*
 *
 */
lv_obj_t *weather_widget_tile = NULL;
lv_style_t weather_widget_style;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);

static void exit_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );
static void setup_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event );

void weather_widget_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {
    Serial.printf("setup widget tile\r\n");
    lv_obj_t * exit_btn = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align(exit_btn, tile, LV_ALIGN_IN_BOTTOM_LEFT, 5, -5 );
    lv_obj_set_event_cb( exit_btn, exit_weather_widget_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align(setup_btn, tile, LV_ALIGN_IN_BOTTOM_RIGHT, -5, -5 );
    lv_obj_set_event_cb( setup_btn, setup_weather_widget_event_cb );

    lv_obj_t * reload_btn = lv_imgbtn_create(tile, NULL);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_PRESSED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_PRESSED, &refresh_32px);
    lv_obj_add_style(reload_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align(reload_btn, tile, LV_ALIGN_IN_TOP_RIGHT, -5 , STATUSBAR_HEIGHT + 5 );
    lv_obj_set_event_cb( reload_btn, setup_weather_widget_event_cb );

}

static void exit_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( MAIN_TILE, LV_ANIM_OFF );
                                        break;
    }
}

static void setup_weather_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( weather_widget_setup_tile_num, LV_ANIM_ON );
                                        break;
    }
}