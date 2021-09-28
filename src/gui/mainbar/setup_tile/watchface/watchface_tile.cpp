/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#include "utils/alloc.h"

#include "watchface_manager.h"
#include "watchface_tile.h"
#include "watchface_setup.h"
#include "gui/mainbar/setup_tile/watchface/config/watchface_expr.h"
#include "gui/mainbar/setup_tile/watchface/config/watchface_theme_config.h"
#include "gui/mainbar/setup_tile/watchface/config/watchface_config.h"
#include "gui/gui.h"
#include "app/alarm_clock/alarm_in_progress.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"
#include "gui/widget_styles.h"
#include "gui/mainbar/setup_tile/bluetooth_settings/bluetooth_message.h"
#include "hardware/blectl.h"
#include "hardware/rtcctl.h"
#include "hardware/powermgm.h"
#include "hardware/display.h"
#include "hardware/touch.h"
#include "hardware/pmu.h"
#include "hardware/motion.h"
#include "hardware/wifictl.h"
#include "utils/filepath_convert.h"

#ifdef NATIVE_64BIT
    #include <iostream>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include "utils/logging.h"
#else
    #include <WiFi.h>
    #include <Arduino.h>

    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #endif

    #include "utils/decompress/decompress.h"
#endif

/**
 * watchface task and states
 */
lv_task_t *watchface_tile_task;
volatile bool watchface_active = false;
volatile bool watchface_tile_block_show_messages = false;
volatile bool watchface_test = false;
volatile uint32_t watchface_return_tile = 0;
volatile bool watchface_enable_after_wakeup = false;
/**
 * watchface theme config
 */
watchface_theme_config_t watchface_theme_config;
/**
 * watchface lv objects and styles
 */
uint32_t watchface_app_tile_num;
lv_obj_t *watchface_app_tile = NULL;
lv_obj_t *watchface_dial_img = NULL;
lv_obj_t *watchface_hour_img = NULL;
lv_obj_t *watchface_min_img = NULL;
lv_obj_t *watchface_sec_img = NULL;
lv_obj_t *watchface_hour_s_img = NULL;
lv_obj_t *watchface_min_s_img = NULL;
lv_obj_t *watchface_sec_s_img = NULL;
lv_obj_t *watchface_btn = NULL;
lv_obj_t *watchface_image[ WATCHFACE_IMAGE_NUM ];
lv_font_t *watchface_custom_font[ WATCHFACE_LABEL_NUM ];
lv_obj_t *watchface_label[ WATCHFACE_LABEL_NUM ];
lv_style_t *watchface_app_label_style[ WATCHFACE_LABEL_NUM ];
lv_style_t watchface_app_tile_style;
lv_style_t watchface_app_image_style;
/**
 * default watchface
 */
LV_IMG_DECLARE(swiss_dial_240px);
LV_IMG_DECLARE(swiss_hour_240px);
LV_IMG_DECLARE(swiss_min_240px);
LV_IMG_DECLARE(swiss_sec_240px);
LV_IMG_DECLARE(swiss_hour_s_240px);
LV_IMG_DECLARE(swiss_min_s_240px);
LV_IMG_DECLARE(swiss_sec_s_240px);
/**
 * default fonts
 */
LV_FONT_DECLARE(Ubuntu_12px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);
LV_FONT_DECLARE(Ubuntu_48px);
LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(LCD_12px);
LV_FONT_DECLARE(LCD_16px);
LV_FONT_DECLARE(LCD_32px);
LV_FONT_DECLARE(LCD_48px);
LV_FONT_DECLARE(LCD_72px);

void watchface_app_tile_update( void );
static void exit_watchface_app_tile_event_cb( lv_obj_t * obj, lv_event_t event );
void watchface_app_tile_update_task( lv_task_t *task );
bool watchface_rtcctl_event_cb( EventBits_t event, void *arg );
bool watchface_powermgm_event_cb( EventBits_t event, void *arg );
void watchface_avtivate_cb( void );
void watchface_hibernate_cb( void );
void watchface_remove_theme_files ( void );
lv_font_t *watchface_get_font( const char *font, int32_t font_size );
lv_color_t watchface_get_color( char *color );
lv_align_t watchface_get_align( char *align );
void watchface_app_label_update( tm &info );
void watchface_app_image_update( tm &info );

void watchface_tile_setup( void ) {
    watchface_app_tile_num = mainbar_add_app_tile( 1, 1, "WatchFace Tile" );
    watchface_app_tile = mainbar_get_tile_obj( watchface_app_tile_num );

    watchface_theme_config.load();

    lv_style_copy( &watchface_app_tile_style, ws_get_app_style() );
    lv_style_set_radius( &watchface_app_tile_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &watchface_app_tile_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &watchface_app_tile_style, LV_OBJ_PART_MAIN, LV_OPA_0 );
    lv_style_set_border_width( &watchface_app_tile_style, LV_OBJ_PART_MAIN, 0 );

    lv_style_copy( &watchface_app_image_style, ws_get_mainbar_style() );
    lv_style_reset( &watchface_app_image_style );
    lv_style_set_radius( &watchface_app_image_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &watchface_app_image_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &watchface_app_image_style, LV_OBJ_PART_MAIN, LV_OPA_0 );
    lv_style_set_border_width( &watchface_app_image_style, LV_OBJ_PART_MAIN, 0 );

    lv_obj_t *watchface_cont = lv_obj_create( watchface_app_tile, NULL );
    lv_obj_set_size( watchface_cont, 240, 240 );
    lv_obj_add_style( watchface_cont, LV_OBJ_PART_MAIN, &watchface_app_tile_style );
    lv_obj_align( watchface_cont, watchface_app_tile, LV_ALIGN_CENTER, 0, 0 );
    /**
     * alloc default dial
     */
    watchface_dial_img = lv_img_create( watchface_cont, NULL );
    lv_img_set_src( watchface_dial_img, &swiss_dial_240px );
    lv_obj_align( watchface_dial_img, watchface_cont, LV_ALIGN_CENTER, watchface_theme_config.dial.dial.x_offset, watchface_theme_config.dial.dial.y_offset );
    /**
     * alloc labels and set to defauts
     */
    for( int i = 0 ; i < WATCHFACE_LABEL_NUM ; i++ ) {
        /**
         * alloc and copy default style
         */
        watchface_app_label_style[ i ] = (lv_style_t *)MALLOC( sizeof( lv_style_t) );
        if ( !watchface_app_label_style[ i ] ) {
            log_e("watchface_app_label_style[ %d ] alloc failed", i ); while( true );
        }
        lv_style_copy( watchface_app_label_style[ i ], ws_get_mainbar_style() );
        lv_style_set_text_font( watchface_app_label_style[ i ], LV_OBJ_PART_MAIN, watchface_get_font( watchface_theme_config.dial.label[ i ].font, watchface_theme_config.dial.label[ i ].font_size ) );
        lv_style_set_text_color( watchface_app_label_style[ i ], LV_OBJ_PART_MAIN, watchface_get_color( watchface_theme_config.dial.label[ i ].font_color ) );
        /**
         * alloc and setup label container
         */
        lv_obj_t *watchface_label_cont = lv_cont_create( watchface_cont, NULL );
        lv_obj_set_width( watchface_label_cont, watchface_theme_config.dial.label[ i ].x_size );
        lv_obj_set_height( watchface_label_cont, watchface_theme_config.dial.label[ i ].y_size );
        lv_obj_set_pos( watchface_label_cont, watchface_theme_config.dial.label[ i ].x_offset, watchface_theme_config.dial.label[ i ].y_offset );
        lv_obj_set_hidden( watchface_label_cont, !(watchface_theme_config.dial.label[ i ].enable != NULL && watchface_expr_eval( watchface_theme_config.dial.label[ i ].enable)) );
        lv_obj_add_style( watchface_label_cont, LV_OBJ_PART_MAIN, &watchface_app_tile_style );
        /**
         * alloc and setup label
         */
        watchface_label[ i ] = lv_label_create( watchface_label_cont, NULL );
        lv_label_set_text( watchface_label[ i ], (const char*)watchface_theme_config.dial.label[ i ].label );
        lv_obj_add_style( watchface_label[ i ], LV_OBJ_PART_MAIN, watchface_app_label_style[ i ] );
        lv_obj_align( watchface_label[ i ], lv_obj_get_parent( watchface_label[ i ] ), watchface_get_align( watchface_theme_config.dial.label[ i ].align ), 0, 0 );
        /**
         * free font
         */
        watchface_custom_font[ i ] = NULL;
    }
    /**
     * alloc image and set to defauts
     */
    for( int i = 0 ; i < WATCHFACE_IMAGE_NUM ; i++ ) {
        /**
         * alloc and setup image container
         */
        lv_obj_t *watchface_image_cont = lv_cont_create( watchface_cont, NULL );
        lv_obj_set_width( watchface_image_cont, watchface_theme_config.dial.image[ i ].x_size );
        lv_obj_set_height( watchface_image_cont, watchface_theme_config.dial.image[ i ].y_size );
        lv_obj_set_pos( watchface_image_cont, watchface_theme_config.dial.image[ i ].x_offset, watchface_theme_config.dial.image[ i ].y_offset );
        lv_obj_set_hidden( watchface_image_cont, !(watchface_theme_config.dial.image[ i ].enable != NULL && watchface_expr_eval( watchface_theme_config.dial.image[ i ].enable)) );
        lv_obj_add_style( watchface_image_cont, LV_OBJ_PART_MAIN, &watchface_app_image_style );
        /**
         * alloc and setup image
         */
        watchface_image[ i ] = lv_img_create( watchface_image_cont, NULL );
        lv_img_set_src( watchface_image[ i ], watchface_theme_config.dial.image[ i ].file );
        lv_obj_add_style( watchface_image[ i ], LV_OBJ_PART_MAIN, &watchface_app_image_style );
        lv_obj_align( watchface_image[ i ], watchface_image_cont, LV_ALIGN_CENTER, 0, 0 );
        lv_img_set_pivot( watchface_image[ i ], watchface_theme_config.dial.image[ i ].rotation_x_origin, watchface_theme_config.dial.image[ i ].rotation_y_origin );
        lv_img_set_angle( watchface_image[ i ], watchface_theme_config.dial.image[ i ].rotation_start );
    }
    /**
     * alloc default index shadows
     */
    watchface_hour_s_img = lv_img_create( watchface_cont, NULL );
    lv_img_set_src( watchface_hour_s_img, &swiss_hour_s_240px );
    lv_obj_align( watchface_hour_s_img, watchface_cont, LV_ALIGN_CENTER, watchface_theme_config.dial.hour_shadow.x_offset, watchface_theme_config.dial.hour_shadow.y_offset );
    lv_img_set_angle( watchface_hour_s_img, 0 );
    watchface_min_s_img = lv_img_create( watchface_cont, NULL );
    lv_img_set_src( watchface_min_s_img, &swiss_min_s_240px );
    lv_obj_align( watchface_min_s_img, watchface_cont, LV_ALIGN_CENTER, watchface_theme_config.dial.min_shadow.x_offset, watchface_theme_config.dial.min_shadow.y_offset );
    lv_img_set_angle( watchface_min_s_img, 0 );
    watchface_sec_s_img = lv_img_create( watchface_cont, NULL );
    lv_img_set_src( watchface_sec_s_img, &swiss_sec_s_240px );
    lv_obj_align( watchface_sec_s_img, watchface_cont, LV_ALIGN_CENTER, watchface_theme_config.dial.sec_shadow.x_offset, watchface_theme_config.dial.sec_shadow.y_offset );
    lv_img_set_angle( watchface_sec_s_img, 0 );
    /**
     * alloc default index
     */
    watchface_hour_img = lv_img_create( watchface_cont, NULL );
    lv_img_set_src( watchface_hour_img, &swiss_hour_240px );
    lv_obj_align( watchface_hour_img, watchface_cont, LV_ALIGN_CENTER, watchface_theme_config.dial.hour.x_offset, watchface_theme_config.dial.hour.y_offset );
    lv_img_set_angle( watchface_hour_img, 0 );
    watchface_min_img = lv_img_create( watchface_cont, NULL );
    lv_img_set_src( watchface_min_img, &swiss_min_240px );
    lv_obj_align( watchface_min_img, watchface_cont, LV_ALIGN_CENTER, watchface_theme_config.dial.min.x_offset, watchface_theme_config.dial.min.y_offset );
    lv_img_set_angle( watchface_hour_img, 0 );
    watchface_sec_img = lv_img_create( watchface_cont, NULL );
    lv_img_set_src( watchface_sec_img, &swiss_sec_240px );
    lv_obj_align( watchface_sec_img, watchface_cont, LV_ALIGN_CENTER, watchface_theme_config.dial.sec.x_offset, watchface_theme_config.dial.sec.y_offset );
    lv_img_set_angle( watchface_hour_img, 0 );
    /**
     * watchface exit on touch
     */
    watchface_btn = lv_btn_create( watchface_app_tile, NULL );
    lv_obj_set_width( watchface_btn, lv_disp_get_hor_res( NULL ) );
    lv_obj_set_height( watchface_btn, lv_disp_get_ver_res( NULL ) );
    lv_obj_add_protect( watchface_btn, LV_PROTECT_CLICK_FOCUS );
    lv_obj_add_style( watchface_btn, LV_OBJ_PART_MAIN, &watchface_app_tile_style );
    lv_obj_align( watchface_btn, watchface_app_tile, LV_ALIGN_CENTER, 0, 0 );
    lv_obj_set_event_cb( watchface_btn, exit_watchface_app_tile_event_cb );
    /**
     * setup activate and hibernate callback function
     */
    mainbar_add_tile_activate_cb( watchface_app_tile_num, watchface_avtivate_cb );
    mainbar_add_tile_hibernate_cb( watchface_app_tile_num, watchface_hibernate_cb );
    /**
     * setup powermgm and touch callback function
     */
    powermgm_register_cb( POWERMGM_STANDBY, watchface_powermgm_event_cb, "watchface powermgm" );
    /**
     * setup watchface background task
     */
    watchface_tile_task = lv_task_create( watchface_app_tile_update_task, 1000, LV_TASK_PRIO_MID, NULL );
    /**
     * reload and setup theme config
     */
    watchface_reload_theme();
}

void watchface_tile_set_antialias( bool enable ) {
    lv_img_set_antialias( watchface_hour_s_img, enable );
    lv_img_set_antialias( watchface_min_s_img, enable );
    lv_img_set_antialias( watchface_sec_s_img, enable );
    lv_img_set_antialias( watchface_hour_img, enable );
    lv_img_set_antialias( watchface_min_img, enable );
    lv_img_set_antialias( watchface_sec_img, enable );
}

void watchface_decompress_theme( void ) {
    FILE* file;
    char filename[256] ="";

    file = fopen( filepath_convert( filename, sizeof( filename ), "/spiffs" WATCHFACE_THEME_FILE ), "rb" );
    if ( file ) {
        fclose( file );
        watchface_setup_set_info_label( "clear watchface theme, wait ..." );
        watchface_remove_theme_files();
        watchface_setup_set_info_label( "unzip watchface theme, wait ..." );
        #ifdef NATIVE_64BIT
            /**
             * no implementation
             */
        #else
                decompress_file_into_spiffs( WATCHFACE_THEME_FILE, "/watchface", NULL );
        #endif
        watchface_setup_set_info_label( "done!" );
        mainbar_jump_to_tilenumber( watchface_app_tile_num, LV_ANIM_OFF );
    }
    else {
        watchface_setup_set_info_label( "no /watchface.tar.gz found" );
    }
    watchface_reload_theme();
}

void watchface_default_theme( void ) {    
    watchface_setup_set_info_label( "clear watchface theme, wait ..." );
    watchface_remove_theme_files();
    watchface_reload_theme();
    mainbar_jump_to_tilenumber( watchface_app_tile_num, LV_ANIM_OFF );
    watchface_setup_set_info_label( "done!" );
}

void watchface_remove_theme_files ( void ) {
    char filename[256] ="";

    remove( filepath_convert( filename, sizeof( filename ), "/spiffs" WATCHFACE_THEME_JSON_CONFIG_FILE ) );
    remove( filepath_convert( filename, sizeof( filename ), WATCHFACE_DIAL_IMAGE_FILE ) );
    remove( filepath_convert( filename, sizeof( filename ), WATCHFACE_HOUR_IMAGE_FILE ) );
    remove( filepath_convert( filename, sizeof( filename ), WATCHFACE_HOUR_SHADOW_IMAGE_FILE ) );
    remove( filepath_convert( filename, sizeof( filename ), WATCHFACE_MIN_IMAGE_FILE ) );
    remove( filepath_convert( filename, sizeof( filename ), WATCHFACE_MIN_SHADOW_IMAGE_FILE ) );
    remove( filepath_convert( filename, sizeof( filename ), WATCHFACE_SEC_IMAGE_FILE ) );
    remove( filepath_convert( filename, sizeof( filename ), WATCHFACE_SEC_SHADOW_IMAGE_FILE ) );
}

void watchface_reload_and_test( void ) {
    /**
     * reload and setup theme config
     */
    watchface_reload_theme();
    /**
     * jump the watchface
     */
    mainbar_jump_to_tilenumber( watchface_app_tile_num, LV_ANIM_OFF );
}

static void exit_watchface_app_tile_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_SHORT_CLICKED ): mainbar_jump_back();
                                        break;
        case( LV_EVENT_LONG_PRESSED ):  mainbar_jump_to_tilenumber( watchface_manager_get_setup_tile_num(), LV_ANIM_OFF );
                                        break;
    }    
}

void watchface_reload_theme( void ) {
    /**
     * reload theme config
     */
    watchface_theme_config.load();
    lv_img_cache_set_size(0);
    FILE* file;
    char filename[256] = "";
    /**
     * load dial image
     * 240x240px
     */
    file = fopen( filepath_convert( filename, sizeof( filename ), WATCHFACE_DIAL_IMAGE_FILE ), "rb" );
    if ( file ) {
        fclose( file );
        lv_img_set_src( watchface_dial_img, filepath_convert( filename, sizeof( filename ), WATCHFACE_DIAL_IMAGE_FILE ) );
        lv_img_cache_invalidate_src( filepath_convert( filename, sizeof( filename ), WATCHFACE_DIAL_IMAGE_FILE ) );
        WATCHFACE_LOG("load custom watchface dial from %s", filepath_convert( filename, sizeof( filename ), WATCHFACE_DIAL_IMAGE_FILE ) );
    }
    else {
        lv_img_set_src( watchface_dial_img, &swiss_dial_240px );
        lv_img_cache_invalidate_src( &swiss_dial_240px  );
        WATCHFACE_LOG("load standard watchface dial");
    }
    lv_obj_align( watchface_dial_img, watchface_app_tile, LV_ALIGN_CENTER, watchface_theme_config.dial.dial.x_offset, watchface_theme_config.dial.dial.y_offset );
    lv_obj_set_hidden( watchface_dial_img, !watchface_theme_config.dial.dial.enable );
    lv_obj_invalidate( watchface_dial_img );
    /**
     * load hour shadow image
     * 40x240px, center x=20, y=120
     */
    file = fopen( filepath_convert( filename, sizeof( filename ), WATCHFACE_HOUR_SHADOW_IMAGE_FILE ), "rb" );
    if ( file ) {
        fclose( file );
        lv_img_set_src( watchface_hour_s_img, filepath_convert( filename, sizeof( filename ), WATCHFACE_HOUR_SHADOW_IMAGE_FILE ) );
        lv_img_cache_invalidate_src( filepath_convert( filename, sizeof( filename ), WATCHFACE_HOUR_SHADOW_IMAGE_FILE ) );
        WATCHFACE_LOG("load custom watchface hour shadow from %s", filepath_convert( filename, sizeof( filename ), WATCHFACE_HOUR_SHADOW_IMAGE_FILE ) );
    }
    else {
        lv_img_set_src( watchface_hour_s_img, &swiss_hour_s_240px );
        lv_img_cache_invalidate_src( &swiss_hour_s_240px );
        WATCHFACE_LOG("load standard watchface hour shadow");
    }
    lv_obj_align( watchface_hour_s_img, watchface_app_tile, LV_ALIGN_CENTER, watchface_theme_config.dial.hour_shadow.x_offset, watchface_theme_config.dial.hour_shadow.y_offset );
    lv_obj_set_hidden( watchface_hour_s_img, !watchface_theme_config.dial.hour_shadow.enable );
    lv_obj_invalidate( watchface_hour_s_img );
    /**
     * load min shadow image
     * 40x240px, center x=20, y=120
     */
    file = fopen( filepath_convert( filename, sizeof( filename ), WATCHFACE_MIN_SHADOW_IMAGE_FILE ), "rb" );
    if ( file ) {
        fclose( file );
        lv_img_set_src( watchface_min_s_img, filepath_convert( filename, sizeof( filename ), WATCHFACE_MIN_SHADOW_IMAGE_FILE ) );
        lv_img_cache_invalidate_src( filepath_convert( filename, sizeof( filename ), WATCHFACE_MIN_SHADOW_IMAGE_FILE ) );
        WATCHFACE_LOG("load custom watchface min shadow from %s", filepath_convert( filename, sizeof( filename ), WATCHFACE_MIN_SHADOW_IMAGE_FILE ) );
    }
    else {
        lv_img_set_src( watchface_min_s_img, &swiss_min_s_240px );
        lv_img_cache_invalidate_src( &swiss_min_s_240px );
        WATCHFACE_LOG("load standard watchface min shadow");
    }
    lv_obj_align( watchface_min_s_img, watchface_app_tile, LV_ALIGN_CENTER, watchface_theme_config.dial.min_shadow.x_offset, watchface_theme_config.dial.min_shadow.y_offset );
    lv_obj_set_hidden( watchface_min_s_img, !watchface_theme_config.dial.min_shadow.enable );
    lv_obj_invalidate( watchface_min_s_img );
    /**
     * load sec shadow image
     * 40x240px, center x=20, y=120
     */
    file = fopen( filepath_convert( filename, sizeof( filename ), WATCHFACE_SEC_SHADOW_IMAGE_FILE ), "rb" );
    if ( file ) {
        fclose( file );
        lv_img_set_src( watchface_sec_s_img, filepath_convert( filename, sizeof( filename ), WATCHFACE_SEC_SHADOW_IMAGE_FILE ) );
        lv_img_cache_invalidate_src( filepath_convert( filename, sizeof( filename ), WATCHFACE_SEC_SHADOW_IMAGE_FILE ) );
        WATCHFACE_LOG("load custom watchface sec shadow from %s", filepath_convert( filename, sizeof( filename ), WATCHFACE_SEC_SHADOW_IMAGE_FILE ) );
    }
    else {
        lv_img_set_src( watchface_sec_s_img, &swiss_sec_s_240px );
        lv_img_cache_invalidate_src( &swiss_sec_s_240px );
        WATCHFACE_LOG("load standard watchface sec shadow");
    }
    lv_obj_align( watchface_sec_s_img, watchface_app_tile, LV_ALIGN_CENTER, watchface_theme_config.dial.sec_shadow.x_offset, watchface_theme_config.dial.sec_shadow.y_offset );       
    lv_obj_set_hidden( watchface_sec_s_img, !watchface_theme_config.dial.sec_shadow.enable );
    lv_obj_invalidate( watchface_sec_s_img );
    /**
     * load hour image
     * 40x240px, center x=20, y=120
     */
    file = fopen( filepath_convert( filename, sizeof( filename ), WATCHFACE_HOUR_IMAGE_FILE ), "rb" );
    if ( file ) {
        fclose( file );
        lv_img_set_src( watchface_hour_img, filepath_convert( filename, sizeof( filename ), WATCHFACE_HOUR_IMAGE_FILE ) );
        lv_img_cache_invalidate_src( filepath_convert( filename, sizeof( filename ), WATCHFACE_HOUR_IMAGE_FILE ) );
        WATCHFACE_LOG("load custom watchface hour from %s", filepath_convert( filename, sizeof( filename ), WATCHFACE_HOUR_IMAGE_FILE ) );
    }
    else {
        lv_img_set_src( watchface_hour_img, &swiss_hour_240px );
        lv_img_cache_invalidate_src( &swiss_hour_240px );
        WATCHFACE_LOG("load standard watchface hour");
    }
    lv_obj_align( watchface_hour_img, watchface_app_tile, LV_ALIGN_CENTER, watchface_theme_config.dial.hour.x_offset, watchface_theme_config.dial.hour.y_offset );        
    lv_obj_set_hidden( watchface_hour_img, !watchface_theme_config.dial.hour.enable );
    lv_obj_invalidate( watchface_hour_img );
    /**
     * load min image
     * 40x240px, center x=20, y=120
     */
    file = fopen( filepath_convert( filename, sizeof( filename ), WATCHFACE_MIN_IMAGE_FILE ), "rb" );
    if ( file ) {
        fclose( file );
        lv_img_set_src( watchface_min_img, filepath_convert( filename, sizeof( filename ), WATCHFACE_MIN_IMAGE_FILE ) );
        lv_img_cache_invalidate_src( filepath_convert( filename, sizeof( filename ), WATCHFACE_MIN_IMAGE_FILE ) );
        WATCHFACE_LOG("load custom watchface min from %s", filepath_convert( filename, sizeof( filename ), WATCHFACE_MIN_IMAGE_FILE ) );
    }
    else {
        lv_img_set_src( watchface_min_img, &swiss_min_240px );
        lv_img_cache_invalidate_src( &swiss_min_240px );
        WATCHFACE_LOG("load standard watchface min");
    }
    lv_obj_align( watchface_min_img, watchface_app_tile, LV_ALIGN_CENTER, watchface_theme_config.dial.min.x_offset, watchface_theme_config.dial.min.y_offset );       
    lv_obj_set_hidden( watchface_min_img, !watchface_theme_config.dial.min.enable );
    lv_obj_invalidate( watchface_min_img );
    /**
     * load sec image
     * 40x240px, center x=20, y=120
     */
    file = fopen( filepath_convert( filename, sizeof( filename ), WATCHFACE_SEC_IMAGE_FILE ), "rb" );
    if ( file ) {
        fclose( file );
        lv_img_set_src( watchface_sec_img, filepath_convert( filename, sizeof( filename ), WATCHFACE_SEC_IMAGE_FILE ) );
        lv_img_cache_invalidate_src( filepath_convert( filename, sizeof( filename ), WATCHFACE_SEC_IMAGE_FILE ) );
        WATCHFACE_LOG("load custom watchface sec from %s", filepath_convert( filename, sizeof( filename ), WATCHFACE_SEC_IMAGE_FILE ) );
    }
    else {
        lv_img_set_src( watchface_sec_img, &swiss_sec_240px );
        lv_img_cache_invalidate_src( &swiss_sec_240px );
        WATCHFACE_LOG("load standard watchface sec");
    }
    lv_obj_align( watchface_sec_img, watchface_app_tile, LV_ALIGN_CENTER, watchface_theme_config.dial.sec.x_offset, watchface_theme_config.dial.sec.y_offset );        
    lv_obj_set_hidden( watchface_sec_img, !watchface_theme_config.dial.sec.enable );
    lv_obj_invalidate( watchface_sec_img );
    /**
     * alloc labels and set to defauts
     */
    for( int i = 0 ; i < WATCHFACE_LABEL_NUM ; i++ ) {
        /**
         * load font from spiffs if font name ends with ".font"
         */
        if ( strstr( watchface_theme_config.dial.label[ i ].font, ".font" ) ) {
            /**
             * build lv_fs path
             */
            String fontname = watchface_theme_config.dial.label[ i ].font;
            #ifdef NATIVE_64BIT
                char filename[512] = "";
                char path[512] = "";
                snprintf( path, sizeof( path ), "/spiffs/watchface/%s", fontname.c_str() );
                filepath_convert( filename, sizeof( filename ), path );
                String font = "P:";
                font += filename;
            #else
                String font = "P:/spiffs/watchface/" + fontname;
            #endif
            /**
             * clear old font
             */
            if ( watchface_custom_font[ i ] ) {
                lv_font_free( watchface_custom_font[ i ] );
            }
            /**
             * load new font
             */
            log_i("load font from: %s", font.c_str() );
            watchface_custom_font[ i ] = lv_font_load( font.c_str() );
            if ( watchface_custom_font[ i ] ) {
                /**
                 * set new font
                 */
                lv_style_set_text_font( watchface_app_label_style[ i ], LV_OBJ_PART_MAIN, watchface_custom_font[ i ] );
            }
            else {
                /**
                 * set default font if load failed
                 */
                log_e("load font failed");                
                lv_style_set_text_font( watchface_app_label_style[ i ], LV_OBJ_PART_MAIN, watchface_get_font( watchface_theme_config.dial.label[ i ].font, watchface_theme_config.dial.label[ i ].font_size ) );
            }
        }
        else {
            lv_style_set_text_font( watchface_app_label_style[ i ], LV_OBJ_PART_MAIN, watchface_get_font( watchface_theme_config.dial.label[ i ].font, watchface_theme_config.dial.label[ i ].font_size ) );
        }
        lv_style_set_text_color( watchface_app_label_style[ i ], LV_OBJ_PART_MAIN, watchface_get_color( watchface_theme_config.dial.label[ i ].font_color ) );
        /**
         * alloc and setup label container
         */
        lv_obj_set_width( lv_obj_get_parent( watchface_label[ i ] ), watchface_theme_config.dial.label[ i ].x_size );
        lv_obj_set_height( lv_obj_get_parent( watchface_label[ i ] ), watchface_theme_config.dial.label[ i ].y_size );
        lv_obj_set_pos( lv_obj_get_parent( watchface_label[ i ] ), watchface_theme_config.dial.label[ i ].x_offset, watchface_theme_config.dial.label[ i ].y_offset );
        lv_obj_set_hidden( lv_obj_get_parent( watchface_label[ i ] ), !(watchface_theme_config.dial.label[ i ].enable != NULL && watchface_expr_eval( watchface_theme_config.dial.label[ i ].enable)) );
        lv_obj_add_style( lv_obj_get_parent( watchface_label[ i ] ), LV_OBJ_PART_MAIN, &watchface_app_tile_style );
        /**
         * alloc and setup label
         */
        lv_label_set_text( watchface_label[ i ], (const char*)watchface_theme_config.dial.label[ i ].label );
        lv_obj_reset_style_list( watchface_label[ i ], LV_OBJ_PART_MAIN );
        lv_obj_add_style( watchface_label[ i ], LV_OBJ_PART_MAIN, watchface_app_label_style[ i ] );
        lv_obj_align( watchface_label[ i ], lv_obj_get_parent( watchface_label[ i ] ), watchface_get_align( watchface_theme_config.dial.label[ i ].align ), 0, 0 );
    }
    /**
     * alloc image and set to defauts
     */
    for( int i = 0 ; i < WATCHFACE_IMAGE_NUM ; i++ ) {
        /**
         * alloc and setup image container
         */
        lv_obj_set_width( lv_obj_get_parent( watchface_image[ i ] ), watchface_theme_config.dial.image[ i ].x_size );
        lv_obj_set_height( lv_obj_get_parent( watchface_image[ i ] ), watchface_theme_config.dial.image[ i ].y_size );
        lv_obj_set_pos( lv_obj_get_parent( watchface_image[ i ] ), watchface_theme_config.dial.image[ i ].x_offset, watchface_theme_config.dial.image[ i ].y_offset );
        lv_obj_set_hidden( lv_obj_get_parent( watchface_image[ i ] ), !(watchface_theme_config.dial.image[ i ].enable != NULL && watchface_expr_eval( watchface_theme_config.dial.image[ i ].enable)) );
        /**
         * alloc and setup image
         */
        String imagename = watchface_theme_config.dial.image[ i ].file;
        #ifdef NATIVE_64BIT
            char filename[512] = "";
            char path[512] = "";
            snprintf( path, sizeof( path ), "/spiffs/watchface/%s", imagename.c_str() );
            filepath_convert( filename, sizeof( filename ), path );
            String image = filename;
        #else
            String image = "/spiffs/watchface/" + imagename;
        #endif
        lv_img_set_src( watchface_image[ i ], image.c_str() );
        lv_obj_align( watchface_image[ i ], lv_obj_get_parent( watchface_image[ i ] ), LV_ALIGN_CENTER, 0, 0 );
        lv_img_set_pivot( watchface_image[ i ], watchface_theme_config.dial.image[ i ].rotation_x_origin, watchface_theme_config.dial.image[ i ].rotation_y_origin );
        lv_img_set_angle( watchface_image[ i ], watchface_theme_config.dial.image[ i ].rotation_start );
    }
    /**
     * write clear json back
     */
    watchface_theme_config.save( 32000 );
    watchface_app_tile_update();
    lv_img_cache_set_size(250);
    lv_obj_invalidate( lv_scr_act() );
    lv_refr_now( NULL );
}

lv_font_t *watchface_get_font( const char *font, int32_t font_size ) {
    lv_font_t *lv_font = &Ubuntu_12px;
    if ( !strcmp( font, "Ubuntu" ) ) {
        switch( font_size ) {
            case 12:    lv_font = &Ubuntu_12px;
                        break;
            case 16:    lv_font = &Ubuntu_16px;
                        break;
            case 32:    lv_font = &Ubuntu_32px;
                        break;
            case 48:    lv_font = &Ubuntu_48px;
                        break;
            case 72:    lv_font = &Ubuntu_72px;
                        break;
        }
    }
    else if ( !strcmp( font, "Montserrat" ) ) {
        switch( font_size ) {
            #if LV_FONT_MONTSERRAT_12
            case 12:    lv_font = &lv_font_montserrat_12;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_14
            case 14:    lv_font = &lv_font_montserrat_14;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_16
            case 16:    lv_font = &lv_font_montserrat_16;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_18
            case 18:    lv_font = &lv_font_montserrat_18;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_20
            case 20:    lv_font = &lv_font_montserrat_20;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_22
            case 22:    lv_font = &lv_font_montserrat_22;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_24
            case 24:    lv_font = &lv_font_montserrat_24;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_26
            case 26:    lv_font = &lv_font_montserrat_26;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_28
            case 28:    lv_font = &lv_font_montserrat_28;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_30
            case 30:    lv_font = &lv_font_montserrat_30;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_32
            case 32:    lv_font = &lv_font_montserrat_32;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_34
            case 34:    lv_font = &lv_font_montserrat_34;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_36
            case 36:    lv_font = &lv_font_montserrat_36;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_38
            case 38:    lv_font = &lv_font_montserrat_38;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_40
            case 40:    lv_font = &lv_font_montserrat_40;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_42
            case 42:    lv_font = &lv_font_montserrat_42;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_44
            case 44:    lv_font = &lv_font_montserrat_44;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_46
            case 46:    lv_font = &lv_font_montserrat_46;
                        break;
            #endif
            #if LV_FONT_MONTSERRAT_48
            case 48:    lv_font = &lv_font_montserrat_48;
                        break;
            #endif
        }
    }
    else if ( !strcmp( font, "LCD" ) ) {
        switch( font_size ) {
            case 12:    lv_font = &LCD_12px;
                        break;
            case 16:    lv_font = &LCD_16px;
                        break;
            case 32:    lv_font = &LCD_32px;
                        break;
            case 48:    lv_font = &LCD_48px;
                        break;
            case 72:    lv_font = &LCD_72px;
                        break;
        }
    }
    else {
        switch( font_size ) {
            case 12:    lv_font = &Ubuntu_12px;
                        break;
            case 16:    lv_font = &Ubuntu_16px;
                        break;
            case 32:    lv_font = &Ubuntu_32px;
                        break;
            case 48:    lv_font = &Ubuntu_48px;
                        break;
            case 72:    lv_font = &Ubuntu_72px;
                        break;
        }
    }
    return( lv_font );
}

lv_color_t watchface_get_color( char *color ) {
    char spanset[] = "0123456789ABCDEFabcdef";
    char *ptr = color;
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    /**
     * if the first char an '#' ?
     */
    if ( *color == '#' ) {
        /**
         * check if color string
         */
        ptr++;
        while( *ptr ) {
            ptr = strpbrk( ptr, spanset );
            if ( !ptr ) {
                log_e("wrong color format");
                return( LV_COLOR_BLACK );
            }
            ptr++;
        }
        color++;
        red = ( ( *color <= '9') ? *color - '0' : ( *color & 0x7) + 9 ) << 4;
        color++;
        red = red + ( ( *color <= '9') ? *color - '0' : ( *color & 0x7) + 9 );
        color++;
        green = ( ( *color <= '9') ? *color - '0' : ( *color & 0x7) + 9 ) << 4;
        color++;
        green = green + ( ( *color <= '9') ? *color - '0' : ( *color & 0x7) + 9 );
        color++;
        blue = ( ( *color <= '9') ? *color - '0' : ( *color & 0x7) + 9 ) << 4;
        color++;
        blue = blue + ( ( *color <= '9') ? *color - '0' : ( *color & 0x7) + 9 );
    }
    return( LV_COLOR_MAKE( red, green, blue ) );
}


lv_align_t watchface_get_align( char *align ) {
    if ( !strcmp( align, "center" ) ) {
        return( LV_ALIGN_CENTER );
    }
    else if ( !strcmp( align, "left" ) ) {
        return( LV_ALIGN_IN_LEFT_MID );
    }
    else if ( !strcmp( align, "right" ) ) {
        return( LV_ALIGN_IN_RIGHT_MID );
    }
    else {
        return( LV_ALIGN_CENTER );
    }
}
bool watchface_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch ( event ) {
        case POWERMGM_STANDBY:
            /**
             * switch on standby to watchface for better wakeup perfomance
             */
            if ( watchface_enable_after_wakeup ) {
                watchface_app_tile_update();
                mainbar_jump_to_tilenumber( watchface_app_tile_num, LV_ANIM_OFF );
                lv_obj_invalidate( lv_scr_act() );
                lv_refr_now( NULL );
                /**
                 * disable redraw on next gui cycle
                 */
                gui_force_redraw( false );
            }
            break;
    }
    return( true );
}

void watchface_avtivate_cb( void ) {
    /**
     * set watchface active flag
     */
    watchface_active = true;
    /**
     * save block show messages state
     */
    watchface_tile_block_show_messages = blectl_get_show_notification();
    blectl_set_show_notification( false );
    /**
     * hide statusbar
     */
    statusbar_hide( true );
    /**
     * set full cpu clock
     */
    powermgm_set_perf_mode();
}

void watchface_hibernate_cb( void ) {
    watchface_active = false;
    blectl_set_show_notification( watchface_tile_block_show_messages );
    powermgm_set_normal_mode();
}

void watchface_app_tile_update( void ) {
	if ( watchface_active ) {
        tm info;
        time_t now;
        time(&now);
        localtime_r( &now, &info );

        // Update global context
        watchface_expr_update( info );

        //Angle calculation for Hands
        int Angle_S = (int)((info.tm_sec % 60) * 60 );
        int Angle_M = (int)((info.tm_min % 60) * 60 ) + ( watchface_theme_config.dial.min.smooth ? (int)(info.tm_sec % 60) : 0 );
        int Angle_H = (int)((info.tm_hour % 12 ) * 300 ) + (int)((info.tm_min % 60 ) * 5);

        while (Angle_S >= 3600)
            Angle_S = Angle_S - 3600;
        while (Angle_M >= 3600)
            Angle_M = Angle_M - 3600;
        while (Angle_H >= 3600)
            Angle_H = Angle_H - 3600;

        lv_img_set_angle( watchface_hour_img, Angle_H );
        lv_img_set_angle( watchface_min_img, Angle_M );
        lv_img_set_angle( watchface_sec_img, Angle_S );

        lv_img_set_angle( watchface_hour_s_img, Angle_H );
        lv_img_set_angle( watchface_min_s_img, Angle_M );
        lv_img_set_angle( watchface_sec_s_img, Angle_S );

        watchface_app_label_update( info );
        watchface_app_image_update( info );
    }
}

void watchface_app_image_update( tm &info ) {
    for( int i = 0 ; i < WATCHFACE_IMAGE_NUM ; i++ ) {
        /**
         * check if label enabled
         */
        if ( watchface_theme_config.dial.image[ i ].enable != NULL && watchface_expr_eval( watchface_theme_config.dial.image[ i ].enable ) ) {
            int32_t angle = 0;
            /**
             * check label type
             */
            if ( !strcmp( "battery_percent", watchface_theme_config.dial.image[ i ].type ) ) {
                angle = watchface_theme_config.dial.image[ i ].rotation_start + ( ( pmu_get_battery_percent() * watchface_theme_config.dial.image[ i ].rotation_range ) / 100 );
            }
            if ( !strcmp( "battery_voltage", watchface_theme_config.dial.image[ i ].type ) ) {
                angle = watchface_theme_config.dial.image[ i ].rotation_start + ( ( ( pmu_get_battery_voltage() / 1000 ) * watchface_theme_config.dial.image[ i ].rotation_range ) / 5 );
            }
            else if ( !strcmp( "time_hour", watchface_theme_config.dial.image[ i ].type ) ) {
                angle = watchface_theme_config.dial.image[ i ].rotation_start + ( ( info.tm_hour * watchface_theme_config.dial.image[ i ].rotation_range ) / 24 );
            }
            else if ( !strcmp( "time_min", watchface_theme_config.dial.image[ i ].type ) ) {
                angle = watchface_theme_config.dial.image[ i ].rotation_start + ( ( info.tm_min * watchface_theme_config.dial.image[ i ].rotation_range ) / 60 );
            }
            else if ( !strcmp( "time_sec", watchface_theme_config.dial.image[ i ].type ) ) {
                angle = watchface_theme_config.dial.image[ i ].rotation_start + ( ( info.tm_sec * watchface_theme_config.dial.image[ i ].rotation_range ) / 60 );
            }
            angle = angle % 3600;
            lv_img_set_angle( watchface_image[ i ], angle );
            /**
             * check toggle option
             */
            if ( watchface_theme_config.dial.image[ i ].hide_interval ) {
                /**
                 * temp toggle value
                 */
                int32_t hide_interval = watchface_theme_config.dial.image[ i ].hide_interval;
                if ( hide_interval < 0 ) {
                    hide_interval = abs( hide_interval );
                }
                /**
                 * toggle hide depend
                 * 
                 * a positive hide interval means hide/show toggle interval
                 * a negative hide interval means show/hide toggle interval                    
                 */
                if ( ( info.tm_sec / hide_interval ) % 2 ) {
                    /**
                     */
                    if ( watchface_theme_config.dial.image[ i ].hide_interval > 0 ) {
                        lv_obj_set_hidden( watchface_image[ i ], true );
                    }
                    else {
                        lv_obj_set_hidden( watchface_image[ i ], false );
                    }
                }
                else {
                    if ( watchface_theme_config.dial.label[ i ].hide_interval > 0 ) {
                        lv_obj_set_hidden( watchface_image[ i ], false );
                    }
                    else {
                        lv_obj_set_hidden( watchface_image[ i ], true );
                    }
                }
            }
            else {
                lv_obj_set_hidden( watchface_image[ i ], false );                
            }
        }
    }
}

void watchface_app_label_update( tm &info ) {
    for( int i = 0 ; i < WATCHFACE_LABEL_NUM ; i++ ) {
        char temp_str[ 64 ] = "";
        /**
         * check if label enabled
         */
        if ( watchface_theme_config.dial.label[ i ].enable != NULL && watchface_expr_eval( watchface_theme_config.dial.label[ i ].enable ) ) {
            /**
             * check label type
             */
            if ( !strcmp( "date", watchface_theme_config.dial.label[ i ].type ) ) {
                strftime( temp_str, sizeof( temp_str ), watchface_theme_config.dial.label[ i ].label, &info );
            }
            else if ( !strcmp( "text", watchface_theme_config.dial.label[ i ].type ) ) {
                snprintf( temp_str, sizeof( temp_str ), watchface_theme_config.dial.label[ i ].label, NULL );
            }
            else if ( !strcmp( "battery_percent", watchface_theme_config.dial.label[ i ].type ) ) {
                snprintf( temp_str, sizeof( temp_str ), watchface_theme_config.dial.label[ i ].label, pmu_get_battery_percent() );
            }
            else if ( !strcmp( "battery_voltage", watchface_theme_config.dial.label[ i ].type ) ) {
                snprintf( temp_str, sizeof( temp_str ), watchface_theme_config.dial.label[ i ].label, pmu_get_battery_voltage() / 1000 );
            }
            else if ( !strcmp( "bluetooth_messages", watchface_theme_config.dial.label[ i ].type ) ) {
                snprintf( temp_str, sizeof( temp_str ), watchface_theme_config.dial.label[ i ].label, bluetooth_get_number_of_msg() );
            }
            else if ( !strcmp( "steps", watchface_theme_config.dial.label[ i ].type ) ) {
                snprintf( temp_str, sizeof( temp_str ), watchface_theme_config.dial.label[ i ].label, bma_get_stepcounter() );
            }
            else if ( !strcmp( "expr", watchface_theme_config.dial.label[ i ].type ) && watchface_theme_config.dial.label[ i ].expr != NULL ) {
                double val = watchface_expr_eval(watchface_theme_config.dial.label[ i ].expr);
                snprintf( temp_str, sizeof( temp_str ), watchface_theme_config.dial.label[ i ].label, val );
            }
            else {
                snprintf( temp_str, sizeof( temp_str ), "n/a" );
            }
            /**
             * check toggle option
             */
            if ( watchface_theme_config.dial.label[ i ].hide_interval ) {
                /**
                 * temp toggle value
                 */
                int32_t hide_interval = watchface_theme_config.dial.label[ i ].hide_interval;
                if ( hide_interval < 0 ) {
                    hide_interval = abs( hide_interval );
                }
                /**
                 * toggle hide depend
                 * 
                 * a positive hide interval means hide/show toggle interval
                 * a negative hide interval means show/hide toggle interval                    
                 */
                if ( ( info.tm_sec / hide_interval ) % 2 ) {
                    /**
                     */
                    if ( watchface_theme_config.dial.label[ i ].hide_interval > 0 ) {
                        lv_obj_set_hidden( watchface_label[ i ], true );
                    }
                    else {
                        lv_obj_set_hidden( watchface_label[ i ], false );
                    }
                }
                else {
                    if ( watchface_theme_config.dial.label[ i ].hide_interval > 0 ) {
                        lv_obj_set_hidden( watchface_label[ i ], false );
                    }
                    else {
                        lv_obj_set_hidden( watchface_label[ i ], true );
                    }
                }
            }
            else {
                lv_obj_set_hidden( watchface_label[ i ], false );                
            }
            /**
             * align label
             */
            lv_label_set_text( watchface_label[ i ], temp_str );
            lv_obj_align( watchface_label[ i ], lv_obj_get_parent( watchface_label[ i ] ), watchface_get_align( watchface_theme_config.dial.label[ i ].align ), 0, 0 );
        }
    }
}

void watchface_app_tile_update_task( lv_task_t *task ) {
    watchface_app_tile_update();
}

bool watchface_get_enable_tile_after_wakeup( void ) {
    return( watchface_enable_after_wakeup );
}

void watchface_enable_tile_after_wakeup( bool enable ) {
    watchface_enable_after_wakeup = enable;
}
