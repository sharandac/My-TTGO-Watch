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

#include "splashscreen.h"

#include "hardware/display.h"
#include "gui/png_decoder/png_decoder.h"

lv_obj_t *logo = NULL;
lv_obj_t *preload = NULL;
lv_obj_t *preload_label = NULL;
lv_style_t style;

LV_IMG_DECLARE(hedgehog);

void splash_screen_stage_one( void ) {

    TTGOClass *ttgo = TTGOClass::getWatch();

    png_decoder_init();
    lv_img_cache_set_size(100);

    lv_style_init( &style );
    lv_style_set_radius( &style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &style, LV_OBJ_PART_MAIN, LV_OPA_100 );
    lv_style_set_border_width( &style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_text_color( &style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );

    lv_obj_t *background = lv_bar_create(lv_scr_act(), NULL);
    lv_obj_set_size( background, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) );
    lv_obj_add_style( background, LV_OBJ_PART_MAIN, &style );
    lv_obj_align( background, NULL, LV_ALIGN_CENTER, 0, 0 );

    logo = lv_img_create( background , NULL );

    // load boot logo from spiffs if exsist
    FILE* file;
    file = fopen( SPLASHSCREENLOGO, "rb" );

    if ( file ) {
        log_i("use custom boot logo from spiffs");
        fclose( file );
        lv_img_set_src( logo, SPLASHSCREENLOGO );
    }
    else {
        log_i("use default boot logo");
        lv_img_set_src( logo, &hedgehog );
    }
    lv_obj_align( logo, NULL, LV_ALIGN_CENTER, 0, 0 );

    preload = lv_bar_create( lv_scr_act(), NULL );
    lv_obj_set_size( preload, lv_disp_get_hor_res( NULL ) - 80, 20 );
    lv_obj_add_style( preload, LV_OBJ_PART_MAIN, &style );
    lv_obj_align( preload, logo, LV_ALIGN_OUT_BOTTOM_MID, 0, 30 );
    lv_bar_set_anim_time( preload, 2000);
    lv_bar_set_value( preload, 0, LV_ANIM_ON);

    preload_label = lv_label_create( lv_scr_act(), NULL );
    lv_label_set_text( preload_label, "booting" );
    lv_obj_add_style( preload_label, LV_OBJ_PART_MAIN, &style );
    lv_obj_align( preload_label, preload, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );

    lv_disp_trig_activity( NULL );

    lv_obj_move_foreground( preload_label );

    lv_task_handler();

    for( int bl = 0 ; bl < display_get_brightness() ; bl++ ) {
        ttgo->bl->adjust( bl );
        delay(5);
    }    
}

void splash_screen_stage_update( const char* msg, int value ) {
    lv_obj_move_foreground( preload );
    lv_disp_trig_activity( NULL );
    lv_task_handler();
    delay(100);
//    lv_bar_set_value( preload, value, LV_ANIM_ON );
    lv_bar_set_value( preload, 0, LV_ANIM_ON );
    lv_label_set_text( preload_label, msg );
    lv_obj_align( preload_label, preload, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
    lv_task_handler();
    delay(500);
}

void splash_screen_stage_finish( void ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    for( int bl = display_get_brightness() ; bl >= 0 ; bl-- ) {
        ttgo->bl->adjust( bl );
        delay(5);
    }
    lv_obj_del( logo );
    lv_obj_del( preload );
    lv_obj_del( preload_label );
    lv_task_handler();
}
