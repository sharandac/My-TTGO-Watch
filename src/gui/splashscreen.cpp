#include "config.h"

#include "hardware/display.h"

lv_obj_t *preload = NULL;
lv_obj_t *preload_label = NULL;
lv_style_t style;

void splash_screen_stage_one( TTGOClass *ttgo ) {
    lv_style_init( &style );
    lv_style_set_radius(&style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color(&style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_bg_opa(&style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width(&style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color(&style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);

    lv_obj_t *background = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_size( background, LV_HOR_RES_MAX, LV_VER_RES_MAX );
    lv_obj_add_style( background, LV_OBJ_PART_MAIN, &style );
    lv_obj_align(background, NULL, LV_ALIGN_CENTER, 0, 0);

    preload = lv_bar_create( background, NULL);
    lv_obj_set_size(preload, LV_HOR_RES_MAX - 40, 20);
    lv_obj_add_style( preload, LV_OBJ_PART_MAIN, &style );
    lv_obj_align(preload, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_bar_set_anim_time(preload, 2000);
    lv_bar_set_value(preload, 0, LV_ANIM_ON);

    preload_label = lv_label_create( background, NULL);
    lv_label_set_text( preload_label, "booting" );
    lv_obj_add_style( preload_label, LV_OBJ_PART_MAIN, &style );
    lv_obj_align(preload_label, preload, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    lv_disp_trig_activity(NULL);
    lv_task_handler();

    for( int bl = 0 ; bl < display_get_brightness() ; bl++ ) {
        ttgo->bl->adjust( bl );
        delay(5);
    }    
}

void splash_screen_stage_update( const char* msg, int value ) {
    lv_disp_trig_activity(NULL);
    lv_task_handler();
    delay(100);
    lv_bar_set_value( preload, value, LV_ANIM_ON);
    lv_label_set_text( preload_label, msg );
    lv_obj_align( preload_label, preload, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
    lv_task_handler();
}

void splash_screen_stage_finish( TTGOClass *ttgo ) {
    ttgo->bl->adjust( 0 );
    for( int bl = display_get_brightness() ; bl > 0 ; bl-- ) {
        ttgo->bl->adjust( bl );
        delay(1);
    }    
    lv_obj_del( preload );
    lv_task_handler();
}
