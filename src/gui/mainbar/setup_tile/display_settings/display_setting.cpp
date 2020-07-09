#include "config.h"
#include "display_settings.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "hardware/display.h"

lv_obj_t *display_settings_tile1 = NULL;
lv_obj_t *display_brightness_slider = NULL;
lv_obj_t *display_timeout_slider = NULL;
lv_obj_t *display_timeout_slider_label = NULL;
lv_style_t display_settings_style;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(brightness_64px);
LV_IMG_DECLARE(time_64px);

static void exit_display_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_display_brightness_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void exit_display_timeout_setup_event_cb( lv_obj_t * obj, lv_event_t event );

void display_settings_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {
    lv_style_init( &display_settings_style );
    lv_style_set_radius( &display_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color( &display_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &display_settings_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &display_settings_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color( &display_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor( &display_settings_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    display_settings_tile1 = lv_obj_create( tile, NULL);
    lv_obj_set_size(display_settings_tile1, hres , vres);
    lv_obj_align(display_settings_tile1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style( display_settings_tile1, LV_OBJ_PART_MAIN, &display_settings_style );

    lv_obj_t *exit_btn = lv_imgbtn_create( display_settings_tile1, NULL);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src( exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style( exit_btn, LV_IMGBTN_PART_MAIN, style);
    lv_obj_align( exit_btn, display_settings_tile1, LV_ALIGN_IN_TOP_LEFT, 10, STATUSBAR_HEIGHT + 10 );
    lv_obj_set_event_cb( exit_btn, exit_display_setup_event_cb );
    
    lv_obj_t *exit_label = lv_label_create( display_settings_tile1, NULL );
    lv_obj_add_style( exit_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( exit_label, "display settings");
    lv_obj_align( exit_label, exit_btn, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    lv_obj_t *brightness_cont = lv_obj_create( display_settings_tile1, NULL );
    lv_obj_set_size( brightness_cont, hres , 80 );
    lv_obj_add_style( brightness_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( brightness_cont, display_settings_tile1, LV_ALIGN_IN_TOP_RIGHT, 0, 75 );
    display_brightness_slider = lv_slider_create( brightness_cont, NULL );
    lv_slider_set_range( display_brightness_slider, 32, 255 );
    lv_obj_set_size( display_brightness_slider, hres - 100 , 10 );
    lv_obj_align( display_brightness_slider, brightness_cont, LV_ALIGN_IN_RIGHT_MID, -15, 0 );
    lv_obj_set_event_cb( display_brightness_slider, exit_display_brightness_setup_event_cb );
    lv_obj_t *brightness_icon = lv_img_create( brightness_cont, NULL );
    lv_img_set_src( brightness_icon, &brightness_64px );
    lv_obj_align( brightness_icon, brightness_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *timeout_cont = lv_obj_create( display_settings_tile1, NULL );
    lv_obj_set_size( timeout_cont, hres , 80 );
    lv_obj_add_style( timeout_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( timeout_cont, brightness_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    display_timeout_slider = lv_slider_create( timeout_cont, NULL );
    lv_slider_set_range( display_timeout_slider, 15, 300 );
    lv_obj_set_size(display_timeout_slider, hres - 100 , 10 );
    lv_obj_align( display_timeout_slider, timeout_cont, LV_ALIGN_IN_RIGHT_MID, -15, 0 );
    lv_obj_set_event_cb( display_timeout_slider, exit_display_timeout_setup_event_cb );
    display_timeout_slider_label = lv_label_create( timeout_cont, NULL );
    lv_obj_add_style( display_timeout_slider_label, LV_OBJ_PART_MAIN, style );
    lv_label_set_text( display_timeout_slider_label, "");
    lv_obj_align( display_timeout_slider_label, display_timeout_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, -5 );
    lv_obj_t *timeout_icon = lv_img_create( timeout_cont, NULL );
    lv_img_set_src( timeout_icon, &time_64px );
    lv_obj_align( timeout_icon, timeout_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_slider_set_value( display_brightness_slider, display_get_brightness(), LV_ANIM_OFF );
    lv_slider_set_value( display_timeout_slider, display_get_timeout(), LV_ANIM_OFF );
    char temp[16]="";
    snprintf( temp, sizeof( temp ), "%d secounds", display_get_timeout() );
    lv_label_set_text( display_timeout_slider_label, temp );
    lv_obj_align( display_timeout_slider_label, display_timeout_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );
}


static void exit_display_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( SETUP_TILE, LV_ANIM_OFF );
                                        display_save_config();
                                        break;
    }
}

static void exit_display_brightness_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     display_set_brightness( lv_slider_get_value( obj ) );
                                            break;
    }
}

static void exit_display_timeout_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ):     display_set_timeout( lv_slider_get_value( obj ) );
                                            char temp[16]="";
                                            snprintf( temp, sizeof( temp ), "%d secounds", lv_slider_get_value(obj) );
                                            lv_label_set_text( display_timeout_slider_label, temp );
                                            lv_obj_align( display_timeout_slider_label, display_timeout_slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 15 );
                                            break;
    }
}