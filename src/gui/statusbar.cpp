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
#include <stdio.h>

#include "config.h"
#include <time.h>
#include "gui.h"
#include "string.h"

#include "statusbar.h"

#include "hardware/powermgm.h"
#include "hardware/wifictl.h"
#include "hardware/blectl.h"
#include "hardware/rtcctl.h"
#include "hardware/motion.h"
#include "hardware/pmu.h"
#include "hardware/sound.h"
#include "hardware/display.h"
#include "hardware/gpsctl.h"

#include "gui/widget_factory.h"
#include "gui/widget_styles.h"
#include "gui/mainbar/mainbar.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #include <WiFi.h>
    #include <Ticker.h>
    #include "FS.h"
    #include "SD.h"
#endif

static bool statusbar_init = false;
static bool statusbar_expanded = false;
static bool statusbar_refresh_update = false;
static bool force_dark_mode = false;

static lv_obj_t *statusbar = NULL;
static lv_obj_t *statusbar_wifi = NULL;
static lv_obj_t *statusbar_wifilabel = NULL;
static lv_obj_t *statusbar_wifiiplabel = NULL;
static lv_obj_t *statusbar_bluetooth = NULL;
static lv_obj_t *statusbar_gps = NULL;
static lv_obj_t *statusbar_stepcounterlabel = NULL;
static lv_obj_t *statusbar_volume_slider = NULL;
static lv_obj_t *statusbar_brightness_slider = NULL;
static lv_obj_t *statusbar_sound_icon = NULL;
static lv_obj_t *statusbar_brightness_icon = NULL;
static lv_style_t statusbarstyle[ STATUSBAR_STYLE_NUM ];

lv_color_t statusbar_retracted_color = LV_COLOR_WHITE;
lv_color_t statusbar_extended_color = LV_COLOR_BLACK;

LV_IMG_DECLARE(wifi_64px);
LV_IMG_DECLARE(bluetooth_64px);
LV_IMG_DECLARE(alarm_16px);
LV_IMG_DECLARE(brightness_32px);
LV_IMG_DECLARE(sound_32px);
LV_IMG_DECLARE(sound_mute_32px);
LV_IMG_DECLARE(gps_64px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_FONT_DECLARE(Ubuntu_32px);

lv_status_bar_t statusicon[ STATUSBAR_NUM ] = 
{
    { NULL, NULL, LV_ALIGN_IN_RIGHT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_BATTERY_FULL, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_BLUETOOTH, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_WIFI, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_VOLUME_MAX, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_GPS, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_BELL, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_WARNING, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_BELL , LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
};

bool should_save_brightness_config = false;
bool should_save_sound_config = false;

void statusbar_event( lv_obj_t * statusbar, lv_event_t event );
void statusbar_wifi_event_cb( lv_obj_t *wifi, lv_event_t event );
void statusbar_bluetooth_event_cb( lv_obj_t *bluetooth, lv_event_t event );
void statusbar_volume_slider_event_handler_cb( lv_obj_t *sound_slider, lv_event_t event );
bool statusbar_gpsctl_event_cb( EventBits_t event, void *arg );
void statusbar_sound_event_cb( lv_obj_t *sound, lv_event_t event );
void statusbar_display_event_cb( lv_obj_t *display, lv_event_t event );
void statusbar_brightness_slider_event_handler_cb( lv_obj_t *brightness_slider, lv_event_t event );

bool statusbar_soundctl_event_cb( EventBits_t event, void *arg );
bool statusbar_blectl_event_cb( EventBits_t event, void *arg );
bool statusbar_wifictl_event_cb( EventBits_t event, void *arg );
bool statusbar_rtcctl_event_cb( EventBits_t event, void *arg );
bool statusbar_bmactl_event_cb( EventBits_t event, void *arg );
bool statusbar_pmuctl_event_cb( EventBits_t event, void *arg );
bool statusbar_displayctl_event_cb( EventBits_t event, void *arg );
bool statusbar_style_event_cb( EventBits_t event, void *arg );

void statusbar_wifi_set_state( bool state, const char *wifiname );
void statusbar_wifi_set_ip_state( bool state, const char *ip );
void statusbar_bluetooth_set_state( bool state );
void statusbar_gps_event_cb( lv_obj_t *gps, lv_event_t event );
void statusbar_set_dark( bool dark_mode );

lv_task_t * statusbar_task;
void statusbar_update_task( lv_task_t * task );

void statusbar_setup( void )
{
    if ( statusbar_init ) {
        log_e("statusbar already init");
        return;
    }

#if defined( BIG_THEME )
    const lv_font_t *statusbar_font = &lv_font_montserrat_32;
#else
    const lv_font_t *statusbar_font = &lv_font_montserrat_14;
#endif

#if defined( ROUND_DISPLAY )
    #define STATUSBAR_ICON_X_OFFSET     -70
#else
    #define STATUSBAR_ICON_X_OFFSET     -5
#endif

    /*Copy a built-in style to initialize the new style*/
    lv_style_init(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    lv_style_set_radius(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_OPA_20);
    lv_style_set_border_width(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_text_font( &statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, statusbar_font );

    lv_style_copy( &statusbarstyle[ STATUSBAR_STYLE_WHITE ], &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, statusbar_retracted_color );
    lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, statusbar_retracted_color );

    lv_style_copy( &statusbarstyle[ STATUSBAR_STYLE_BLACK ], &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_BLACK ], LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_BLACK ], LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_BLACK ], LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    lv_style_copy( &statusbarstyle[ STATUSBAR_STYLE_RED ], &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_RED ], LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_RED ], LV_OBJ_PART_MAIN, LV_COLOR_RED);
    lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_RED ], LV_OBJ_PART_MAIN, LV_COLOR_RED);

    lv_style_copy( &statusbarstyle[ STATUSBAR_STYLE_GRAY ], &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_GRAY ], LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_GRAY ], LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_GRAY ], LV_OBJ_PART_MAIN, LV_COLOR_GRAY);

    lv_style_copy( &statusbarstyle[ STATUSBAR_STYLE_GREEN ], &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_GREEN ], LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_GREEN ], LV_OBJ_PART_MAIN, LV_COLOR_GREEN);
    lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_GREEN ], LV_OBJ_PART_MAIN, LV_COLOR_GREEN);

    lv_style_copy( &statusbarstyle[ STATUSBAR_STYLE_YELLOW ], &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_YELLOW ], LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_YELLOW ], LV_OBJ_PART_MAIN, LV_COLOR_YELLOW);
    lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_YELLOW ], LV_OBJ_PART_MAIN, LV_COLOR_YELLOW);

    lv_style_copy( &statusbarstyle[ STATUSBAR_STYLE_BLUE ], &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_BLUE ], LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_BLUE ], LV_OBJ_PART_MAIN, LV_COLOR_BLUE);
    lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_BLUE ], LV_OBJ_PART_MAIN, LV_COLOR_BLUE);

    statusbar = lv_cont_create( lv_scr_act(), NULL );
    lv_obj_set_width( statusbar, lv_disp_get_hor_res( NULL ) );
    lv_obj_set_height( statusbar, STATUSBAR_HEIGHT );
    lv_obj_reset_style_list( statusbar, LV_OBJ_PART_MAIN );
    lv_obj_add_style( statusbar, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    lv_obj_align( statusbar, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 0 );
    lv_obj_set_event_cb( statusbar, statusbar_event );

    for( int i = 0 ; i < STATUSBAR_NUM ; i++ ) {
        if ( statusicon[i].symbol == NULL ) {
            statusicon[i].icon = lv_label_create( statusbar, NULL);
            lv_label_set_text( statusicon[i].icon, "100%" );
        }
        else {
            statusicon[i].icon = lv_img_create( statusbar , NULL);
            lv_img_set_src( statusicon[i].icon, statusicon[i].symbol );
        }
        lv_obj_reset_style_list( statusicon[i].icon, LV_OBJ_PART_MAIN );
        lv_obj_add_style( statusicon[i].icon, LV_OBJ_PART_MAIN, statusicon[i].style );
        if ( i == 0 ) {
            lv_obj_align(statusicon[i].icon, statusbar, statusicon[i].align, STATUSBAR_ICON_X_OFFSET, 0 );
        }
        else {
            lv_obj_align(statusicon[i].icon, statusicon[i-1].icon, statusicon[i].align, -5, 0);
        }
    }

    static lv_style_t style;
    lv_style_copy( &style, &statusbarstyle[ STATUSBAR_STYLE_GRAY ] );

    lv_style_set_image_recolor_opa( &style, LV_BTN_STATE_RELEASED, LV_OPA_100 );
    lv_style_set_image_recolor( &style, LV_BTN_STATE_RELEASED, LV_COLOR_GREEN );
    lv_style_set_image_recolor_opa( &style, LV_BTN_STATE_PRESSED, LV_OPA_100 );
    lv_style_set_image_recolor( &style, LV_BTN_STATE_PRESSED, LV_COLOR_RED );
    lv_style_set_image_recolor_opa( &style, LV_BTN_STATE_CHECKED_RELEASED, LV_OPA_100 );
    lv_style_set_image_recolor( &style, LV_BTN_STATE_CHECKED_RELEASED, LV_COLOR_RED );
    lv_style_set_image_recolor_opa( &style, LV_BTN_STATE_CHECKED_PRESSED, LV_OPA_100 );
    lv_style_set_image_recolor( &style, LV_BTN_STATE_CHECKED_PRESSED, LV_COLOR_GREEN );

    statusbar_wifi = wf_add_image_button( statusbar, wifi_64px, statusbar_wifi_event_cb, &style );
    lv_imgbtn_set_checkable (statusbar_wifi, true );
    lv_obj_align( statusbar_wifi, statusbar, LV_ALIGN_IN_TOP_MID, 0, STATUSBAR_HEIGHT );
    lv_imgbtn_set_state( statusbar_wifi, LV_BTN_STATE_CHECKED_PRESSED );

    /*Create a label on the Image button*/
    statusbar_wifilabel = lv_label_create( statusbar, NULL);
    lv_obj_reset_style_list( statusbar_wifilabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( statusbar_wifilabel, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_GREEN ] );
    lv_label_set_text( statusbar_wifilabel, "");
    lv_obj_align( statusbar_wifilabel, statusbar_wifi, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );

    /*Create a label on the Image button*/
    statusbar_wifiiplabel = lv_label_create( statusbar, NULL);
    lv_obj_reset_style_list( statusbar_wifiiplabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( statusbar_wifiiplabel, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_GREEN ] );
    lv_label_set_text(statusbar_wifiiplabel, "");
    lv_obj_align(statusbar_wifiiplabel, statusbar_wifilabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );

    statusbar_bluetooth = wf_add_image_button( statusbar, bluetooth_64px, statusbar_bluetooth_event_cb, &style );
    lv_imgbtn_set_checkable (statusbar_bluetooth, true );
    lv_obj_align( statusbar_bluetooth, statusbar, LV_ALIGN_IN_TOP_RIGHT, -8, STATUSBAR_HEIGHT );
    lv_imgbtn_set_state( statusbar_bluetooth, LV_BTN_STATE_CHECKED_PRESSED );

    statusbar_gps = wf_add_image_button( statusbar, gps_64px, statusbar_gps_event_cb, &style );
    lv_imgbtn_set_checkable (statusbar_gps, true );
    lv_obj_align( statusbar_gps, statusbar, LV_ALIGN_IN_TOP_LEFT, 8, STATUSBAR_HEIGHT );
    lv_imgbtn_set_state( statusbar_gps, LV_BTN_STATE_CHECKED_PRESSED );

    statusbar_stepcounterlabel = lv_label_create(statusbar, NULL );
    lv_obj_reset_style_list( statusbar_stepcounterlabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( statusbar_stepcounterlabel, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] );
    lv_label_set_text( statusbar_stepcounterlabel, "0");
    lv_obj_align( statusbar_stepcounterlabel, statusbar, LV_ALIGN_IN_LEFT_MID, 5, 0 );

    lv_obj_t *statusbar_volume_cont = lv_obj_create( statusbar, NULL );
    lv_obj_add_style( statusbar_volume_cont, LV_OBJ_PART_MAIN, &style );
    lv_obj_set_size( statusbar_volume_cont, lv_disp_get_hor_res( NULL ) , 36 );
    lv_obj_align( statusbar_volume_cont, statusbar, LV_ALIGN_IN_BOTTOM_MID, 0, STATUSBAR_EXPAND_HEIGHT - STATUSBAR_HEIGHT );
    statusbar_volume_slider = lv_slider_create( statusbar_volume_cont, NULL );
    lv_obj_set_size( statusbar_volume_slider, lv_disp_get_hor_res( NULL ) - 100 , 10 );
    lv_obj_align( statusbar_volume_slider, statusbar_volume_cont, LV_ALIGN_IN_RIGHT_MID, -30, 0 );
    lv_obj_add_protect( statusbar_volume_slider, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( statusbar_volume_slider, LV_SLIDER_PART_INDIC, ws_get_slider_style() );
    lv_obj_add_style( statusbar_volume_slider, LV_SLIDER_PART_KNOB, ws_get_slider_style() );
    lv_slider_set_range( statusbar_volume_slider, 0, 100 );
    lv_obj_set_event_cb( statusbar_volume_slider, statusbar_volume_slider_event_handler_cb ) ;
    statusbar_sound_icon = lv_img_create( statusbar_volume_cont, NULL );
    lv_obj_set_click( statusbar_sound_icon, true );
    lv_obj_set_event_cb( statusbar_sound_icon, statusbar_sound_event_cb );
    lv_img_set_src( statusbar_sound_icon, &sound_32px );
    lv_obj_align( statusbar_sound_icon, statusbar_volume_cont, LV_ALIGN_IN_LEFT_MID, 15, 0 );

    lv_obj_t *statusbar_brightness_cont = lv_obj_create( statusbar, NULL );
    lv_obj_add_style( statusbar_brightness_cont, LV_OBJ_PART_MAIN, &style );
    lv_obj_set_size( statusbar_brightness_cont, lv_disp_get_hor_res( NULL ) , 40 );
    lv_obj_align( statusbar_brightness_cont, statusbar_volume_cont, LV_ALIGN_OUT_TOP_MID, 0, 0 );
    statusbar_brightness_slider = lv_slider_create( statusbar_brightness_cont, NULL );
    lv_obj_set_size( statusbar_brightness_slider, lv_disp_get_hor_res( NULL ) - 100 , 10 );
    lv_obj_align( statusbar_brightness_slider, statusbar_brightness_cont, LV_ALIGN_IN_RIGHT_MID, -30, 0 );
    lv_slider_set_range( statusbar_brightness_slider, DISPLAY_MIN_BRIGHTNESS, DISPLAY_MAX_BRIGHTNESS );
    lv_obj_add_protect( statusbar_brightness_slider, LV_PROTECT_CLICK_FOCUS);
    lv_obj_add_style( statusbar_brightness_slider, LV_SLIDER_PART_INDIC, ws_get_slider_style() );
    lv_obj_add_style( statusbar_brightness_slider, LV_SLIDER_PART_KNOB, ws_get_slider_style() );
    lv_obj_set_event_cb( statusbar_brightness_slider, statusbar_brightness_slider_event_handler_cb ) ;
    statusbar_brightness_icon = lv_img_create( statusbar_brightness_cont, NULL );
    lv_obj_set_click( statusbar_brightness_icon, true );
    lv_obj_set_event_cb( statusbar_brightness_icon, statusbar_display_event_cb );
    lv_img_set_src( statusbar_brightness_icon, &brightness_32px );
    lv_obj_align( statusbar_brightness_icon, statusbar_brightness_cont, LV_ALIGN_IN_LEFT_MID, 15, 0 );

    lv_slider_set_value( statusbar_brightness_slider, display_get_brightness(), LV_ANIM_OFF );
    lv_slider_set_value( statusbar_volume_slider, sound_get_volume_config(), LV_ANIM_OFF );

    /*
     * init fin
     */
    statusbar_init = true;

    statusbar_hide_icon( STATUSBAR_BELL );
    statusbar_hide_icon( STATUSBAR_WARNING );
    statusbar_hide_icon( STATUSBAR_WIFI );
    statusbar_hide_icon( STATUSBAR_BLUETOOTH );
    statusbar_hide_icon( STATUSBAR_VOLUME );
    statusbar_hide_icon( STATUSBAR_GPS );

    if ( rtcctl_get_alarm_data()->enabled ) {
        statusbar_show_icon( STATUSBAR_ALARM );
    }
    else {
        statusbar_hide_icon( STATUSBAR_ALARM );
    }

    statusbar_style_icon( STATUSBAR_BLUETOOTH, STATUSBAR_STYLE_GRAY );

    blectl_register_cb( BLECTL_CONNECT | BLECTL_DISCONNECT | BLECTL_ON | BLECTL_OFF, statusbar_blectl_event_cb, "statusbar bluetooth" );
    wifictl_register_cb( WIFICTL_CONNECT | WIFICTL_DISCONNECT | WIFICTL_OFF | WIFICTL_ON | WIFICTL_MSG | WIFICTL_WPS_SUCCESS | WIFICTL_WPS_FAILED | WIFICTL_CONNECT_IP, statusbar_wifictl_event_cb, "statusbar wifi" );
    rtcctl_register_cb( RTCCTL_ALARM_ENABLED | RTCCTL_ALARM_DISABLED, statusbar_rtcctl_event_cb, "statusbar rtc" );
    bma_register_cb( BMACTL_STEPCOUNTER, statusbar_bmactl_event_cb, "statusbar stepcounter" );
    pmu_register_cb( PMUCTL_STATUS, statusbar_pmuctl_event_cb, "statusbar pmu");
    display_register_cb( DISPLAYCTL_BRIGHTNESS, statusbar_displayctl_event_cb, "statusbar display" );
    gpsctl_register_cb( GPSCTL_ENABLE | GPSCTL_DISABLE | GPSCTL_FIX | GPSCTL_NOFIX, statusbar_gpsctl_event_cb, "statusbar gps" );
    styles_register_cb( STYLE_DARKMODE | STYLE_LIGHTMODE, statusbar_style_event_cb, "statusbar style event" );

    statusbar_task = lv_task_create( statusbar_update_task, 250, LV_TASK_PRIO_MID, NULL );

    if( sound_get_available() ) {
        sound_register_cb( SOUNDCTL_ENABLED | SOUNDCTL_VOLUME, statusbar_soundctl_event_cb, "statusbar sound");
    }
    else {
        lv_obj_set_hidden( statusbar_volume_cont, true );
    }
}

void statusbar_update_task( lv_task_t * task ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    if ( statusbar_refresh_update ) {
        statusbar_refresh();
        statusbar_refresh_update = false;
    }
}

bool statusbar_style_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_LIGHTMODE:   statusbar_retracted_color = LV_COLOR_WHITE;
                                statusbar_extended_color = LV_COLOR_BLACK;
                                statusbar_set_dark( false );
                                break;
        case STYLE_DARKMODE:    statusbar_retracted_color = LV_COLOR_BLACK;
                                statusbar_extended_color = LV_COLOR_BLACK;
                                statusbar_set_dark( true );
    }
    
    return( true );
}

bool statusbar_gpsctl_event_cb( EventBits_t event, void *arg ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return( true );
    }

    switch( event ) {
        case GPSCTL_DISABLE:  
            statusbar_hide_icon( STATUSBAR_GPS );
            statusbar_style_icon( STATUSBAR_GPS, STATUSBAR_STYLE_GRAY );
            lv_imgbtn_set_state( statusbar_gps, LV_BTN_STATE_CHECKED_RELEASED );
            break;
        case GPSCTL_ENABLE:  
            statusbar_show_icon( STATUSBAR_GPS );
            statusbar_style_icon( STATUSBAR_GPS, STATUSBAR_STYLE_GRAY );
            lv_imgbtn_set_state( statusbar_gps, LV_BTN_STATE_RELEASED );
            break;
        case GPSCTL_FIX:  
            statusbar_show_icon( STATUSBAR_GPS );
            statusbar_style_icon( STATUSBAR_GPS, STATUSBAR_STYLE_WHITE );
            break;
        case GPSCTL_NOFIX:  
            statusbar_show_icon( STATUSBAR_GPS );
            statusbar_style_icon( STATUSBAR_GPS, STATUSBAR_STYLE_GRAY );
            break;
    }
    return( true );

}

bool statusbar_soundctl_event_cb( EventBits_t event, void *arg ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return( true );
    }

    switch( event ) {
        case SOUNDCTL_ENABLED:  
            if ( *(bool*)arg ) {
                statusbar_show_icon( STATUSBAR_VOLUME );
                lv_img_set_src( statusbar_sound_icon, &sound_32px );
            }
            else {
                statusbar_hide_icon( STATUSBAR_VOLUME );
                lv_img_set_src( statusbar_sound_icon, &sound_mute_32px );
                lv_slider_set_value( statusbar_volume_slider, 0, LV_ANIM_OFF );
            }
            break;
        case SOUNDCTL_VOLUME:
            lv_slider_set_value( statusbar_volume_slider, sound_get_volume_config(), LV_ANIM_OFF );
            break;
    }
    return( true );
}

bool statusbar_displayctl_event_cb( EventBits_t event, void *arg ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return( true );
    }

    switch( event ) {
        case DISPLAYCTL_BRIGHTNESS:  
            lv_slider_set_value( statusbar_brightness_slider, display_get_brightness(), LV_ANIM_OFF );
            break;
    }
    return( true );
}

static void statusbar_pmuctl_update_batt( int32_t percent, bool charging, bool plug) {
    char level[8]="";

    if ( percent >= 0 && percent <= 100 ) {
        snprintf( level, sizeof( level ), "%d%%", percent );
    }
    else if ( percent > 100 ) {
        snprintf( level, sizeof( level ), "!%d%%", percent );
    }
    else {
        snprintf( level, sizeof( level ), "?" );
        percent = 0;
    }
    lv_label_set_text( statusicon[  STATUSBAR_BATTERY_PERCENT ].icon, (const char *)level );
    if ( !plug ) {
        if ( percent >= 75 ) { 
            lv_img_set_src( statusicon[ STATUSBAR_BATTERY ].icon, LV_SYMBOL_BATTERY_FULL );
        } else if( percent >=50 && percent < 74) {
            lv_img_set_src( statusicon[ STATUSBAR_BATTERY ].icon, LV_SYMBOL_BATTERY_3 );
        } else if( percent >=35 && percent < 49) {
            lv_img_set_src( statusicon[ STATUSBAR_BATTERY ].icon, LV_SYMBOL_BATTERY_2 );
        } else if( percent >=15 && percent < 34) {
            lv_img_set_src( statusicon[ STATUSBAR_BATTERY ].icon, LV_SYMBOL_BATTERY_1 );
        } else if( percent >=0 && percent < 14) {
            lv_img_set_src( statusicon[ STATUSBAR_BATTERY ].icon, LV_SYMBOL_BATTERY_EMPTY );
        }

        if ( percent >= 25 ) {
            statusbar_style_icon( STATUSBAR_BATTERY, STATUSBAR_STYLE_WHITE );
        } else if ( percent >= 15 ) {
            statusbar_style_icon( STATUSBAR_BATTERY, STATUSBAR_STYLE_YELLOW );
        } else {
            statusbar_style_icon( STATUSBAR_BATTERY, STATUSBAR_STYLE_RED );
        }       
    }

    if ( charging ) {
        statusbar_style_icon( STATUSBAR_BATTERY, STATUSBAR_STYLE_RED );
    }
    else {
        statusbar_style_icon( STATUSBAR_BATTERY, STATUSBAR_STYLE_WHITE );
    }

    if ( plug ) {
        lv_img_set_src( statusicon[ STATUSBAR_BATTERY ].icon, LV_SYMBOL_CHARGE );
        statusbar_style_icon( STATUSBAR_BATTERY, STATUSBAR_STYLE_GREEN );
    }
}

bool statusbar_pmuctl_event_cb( EventBits_t event, void *arg ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return( true );
    }

    switch( event ) {
        case PMUCTL_STATUS:
            statusbar_pmuctl_update_batt(*(int32_t*)arg & PMUCTL_STATUS_PERCENT,
                                         *(int32_t*)arg & PMUCTL_STATUS_CHARGING,
                                         *(int32_t*)arg & PMUCTL_STATUS_PLUG);
            break;
    }
    return( true );
}

bool statusbar_bmactl_event_cb( EventBits_t event, void *arg ) {
    char stepcounter[16]="";
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return( true );
    }

    switch( event ) {
        case BMACTL_STEPCOUNTER:    snprintf( stepcounter, sizeof( stepcounter ), "%d", *(uint32_t *)arg );
                                    lv_label_set_text( statusbar_stepcounterlabel, stepcounter );
                                    break;
    }
    return( true );
}

bool statusbar_rtcctl_event_cb( EventBits_t event, void *arg ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return( true );
    }

    switch( event ) {
        case RTCCTL_ALARM_ENABLED:  
            statusbar_show_icon( STATUSBAR_ALARM );
            break;
        case RTCCTL_ALARM_DISABLED: 
            statusbar_hide_icon( STATUSBAR_ALARM );
            break;
    }
    statusbar_refresh_update = true;
    return( true );
}

bool statusbar_blectl_event_cb( EventBits_t event, void *arg ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return( true );
    }

    switch( event ) {
        case BLECTL_ON:             statusbar_show_icon( STATUSBAR_BLUETOOTH );
                                    statusbar_style_icon( STATUSBAR_BLUETOOTH, STATUSBAR_STYLE_GRAY );
                                    lv_imgbtn_set_state( statusbar_bluetooth, LV_BTN_STATE_RELEASED );
                                    break;
        case BLECTL_OFF:            statusbar_hide_icon( STATUSBAR_BLUETOOTH );
                                    lv_imgbtn_set_state( statusbar_bluetooth, LV_BTN_STATE_CHECKED_RELEASED );
                                    break;
        case BLECTL_CONNECT:        statusbar_style_icon( STATUSBAR_BLUETOOTH, STATUSBAR_STYLE_WHITE );
                                    break;
        case BLECTL_DISCONNECT:     statusbar_style_icon( STATUSBAR_BLUETOOTH, STATUSBAR_STYLE_GRAY );
                                    break;
    }
    statusbar_refresh_update = true;
    return( true );
}

bool statusbar_wifictl_event_cb( EventBits_t event, void *arg ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return( true );
    }

    switch( event ) {
        case WIFICTL_CONNECT:       statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_WHITE );
                                    statusbar_wifi_set_state( true, (char *)arg );
                                    statusbar_show_icon( STATUSBAR_WIFI );
                                    break;
        case WIFICTL_CONNECT_IP:    statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_WHITE );
                                    statusbar_wifi_set_ip_state( true, (char *)arg );
                                    statusbar_show_icon( STATUSBAR_WIFI );
                                    break;
        case WIFICTL_DISCONNECT:    statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_GRAY );
                                    statusbar_wifi_set_state( false, (char *)arg );
                                    statusbar_show_icon( STATUSBAR_WIFI );
                                    break;
        case WIFICTL_OFF:           statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_GRAY );
                                    statusbar_hide_icon( STATUSBAR_WIFI );
                                    statusbar_wifi_set_state( false, "" );
                                    break;
        case WIFICTL_ON:            statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_GRAY );
                                    statusbar_wifi_set_state( true, (char *)arg );
                                    statusbar_show_icon( STATUSBAR_WIFI );
                                    break;
        case WIFICTL_WPS_SUCCESS:   statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_GRAY );
                                    statusbar_wifi_set_state( true, (char *)arg );
                                    statusbar_show_icon( STATUSBAR_WIFI );
                                    break;
        case WIFICTL_WPS_FAILED:    statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_GRAY );
                                    statusbar_wifi_set_state( true, (char *)arg );
                                    statusbar_show_icon( STATUSBAR_WIFI );
                                    break;
        case WIFICTL_MSG:           statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_GRAY );
                                    statusbar_wifi_set_state( true, (char *)arg );
                                    statusbar_show_icon( STATUSBAR_WIFI );
                                    break;
    }
    statusbar_refresh_update = true;
    return( true );
}

void statusbar_volume_slider_event_handler_cb(lv_obj_t *volume_slider, lv_event_t event) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    if(event == LV_EVENT_VALUE_CHANGED) {
        if( lv_slider_get_value( volume_slider ) == 0){
            sound_set_enabled_config( false );
            sound_set_volume_config( 1 );
        }
        else{
            if( !sound_get_enabled_config() ){
                sound_set_enabled_config( true );
            }
            sound_set_volume_config( lv_slider_get_value( volume_slider ) );
        }
        should_save_sound_config = true;
    }
}

void statusbar_brightness_slider_event_handler_cb(lv_obj_t *brightness_slider, lv_event_t event) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    if(event == LV_EVENT_VALUE_CHANGED) {
        log_d("Brightness value: %d\n", lv_slider_get_value( brightness_slider ));
        display_set_brightness( lv_slider_get_value( brightness_slider ));
        should_save_brightness_config = true;
    }
}


void statusbar_display_event_cb( lv_obj_t *display, lv_event_t event ){
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    switch ( event ) {
        case ( LV_EVENT_LONG_PRESSED ):             
            statusbar_expand( false );
//            mainbar_jump_to_tilenumber( display_get_setup_tile_num(), LV_ANIM_OFF);
            break;
    }
    statusbar_refresh_update = true;
}

void statusbar_sound_event_cb( lv_obj_t *sound, lv_event_t event ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    static uint8_t volume;

    switch ( event ) {
        case ( LV_EVENT_PRESSED ):             
            if ( sound_get_enabled_config() ) {
                volume = sound_get_volume_config();
                sound_set_enabled_config( false );
            }
            else {
                sound_set_enabled_config( true );
                sound_set_volume_config( volume );
            }
            break;
        case ( LV_EVENT_LONG_PRESSED ):             
            statusbar_expand( false );
//            mainbar_jump_to_tilenumber( sound_get_setup_tile_num(), LV_ANIM_OFF);
            break;
    }
    statusbar_refresh_update = true;
}

void statusbar_wifi_event_cb( lv_obj_t *wifi, lv_event_t event ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    switch ( event ) {
        case ( LV_EVENT_VALUE_CHANGED ):
            switch ( lv_imgbtn_get_state( wifi ) ) {
                case( LV_BTN_STATE_CHECKED_RELEASED ):  wifictl_off();
                                                        wifictl_set_autoon( false );
                                                        break;
                case( LV_BTN_STATE_RELEASED ):          wifictl_on();
                                                        wifictl_set_autoon( true );
                                                        break;
            }
            statusbar_refresh_update = true;
            break;
        case ( LV_EVENT_LONG_PRESSED ):             
            statusbar_expand( false );
//            mainbar_jump_to_tilenumber(wifi_get_setup_tile_num(), LV_ANIM_OFF);
            break;
    }
    statusbar_refresh_update = true;
}

void statusbar_gps_event_cb( lv_obj_t *gps, lv_event_t event ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    switch ( event ) {
        case ( LV_EVENT_VALUE_CHANGED ):
            switch ( lv_imgbtn_get_state( gps ) ) {
                case( LV_BTN_STATE_CHECKED_RELEASED ):  gpsctl_off();
                                                        break;
                case( LV_BTN_STATE_RELEASED ):          gpsctl_on();
                                                        break;
            }
            statusbar_refresh_update = true;
            break;
        case ( LV_EVENT_LONG_PRESSED ):             
            statusbar_expand( false );
//            mainbar_jump_to_tilenumber( gps_get_setup_tile_num() , LV_ANIM_OFF);
            statusbar_refresh_update = true;
            break;
    }
}

void statusbar_bluetooth_event_cb( lv_obj_t *bluetooth, lv_event_t event ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    switch ( event ) {
        case ( LV_EVENT_VALUE_CHANGED ):
            switch ( lv_imgbtn_get_state( bluetooth ) ) {
                case( LV_BTN_STATE_CHECKED_RELEASED ):   
                    blectl_off();
                    break;
                case( LV_BTN_STATE_RELEASED ):    
                    blectl_on();
                    break;
                default:
                    break;
            }
            statusbar_refresh_update = true;
            break;
        case ( LV_EVENT_LONG_PRESSED ):             
            statusbar_expand( false );
//            mainbar_jump_to_tilenumber(bluetooth_get_setup_tile_num(), LV_ANIM_OFF);
            break;
    }
    statusbar_refresh_update = true;
}

void statusbar_wifi_set_state( bool state, const char *wifiname ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    if( state ) {
        lv_imgbtn_set_state( statusbar_wifi, LV_BTN_STATE_RELEASED );
    }
    else {
        lv_imgbtn_set_state( statusbar_wifi, LV_BTN_STATE_CHECKED_RELEASED );
    }
    lv_label_set_text( statusbar_wifilabel, wifiname );
    lv_label_set_text( statusbar_wifiiplabel, "" );
    lv_obj_align( statusbar_wifilabel, statusbar_wifi, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_align( statusbar_wifiiplabel, statusbar_wifilabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}

void statusbar_wifi_set_ip_state( bool state, const char *ip ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    lv_label_set_text( statusbar_wifiiplabel, ip );
    lv_obj_align( statusbar_wifiiplabel, statusbar_wifilabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}

void statusbar_bluetooth_set_state( bool state ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    if ( state ) {
        lv_imgbtn_set_state( statusbar_bluetooth, LV_BTN_STATE_RELEASED );
    }
    else {
        lv_imgbtn_set_state( statusbar_bluetooth, LV_BTN_STATE_PRESSED );
    }
}

void statusbar_hide_icon( statusbar_icon_t icon ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    if ( icon >= STATUSBAR_NUM ) {
        return;
    }

    lv_obj_set_hidden( statusicon[ icon ].icon, true );
    statusbar_refresh_update = true;
}

void statusbar_show_icon( statusbar_icon_t icon ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    if ( icon >= STATUSBAR_NUM ) {
        return;
    }

    lv_obj_set_hidden( statusicon[ icon ].icon, false );
    statusbar_refresh_update = true;
}

void statusbar_style_icon( statusbar_icon_t icon, statusbar_style_t style ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    if ( icon >= STATUSBAR_NUM || style >= STATUSBAR_STYLE_NUM ) {
        return;
    }

    statusicon[ icon ].style = &statusbarstyle[ style ];
    statusbar_refresh_update = true;
}

void statusbar_refresh( void ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    lv_obj_t *last_visible = NULL;

    for ( int i = 0 ; i < STATUSBAR_NUM ; i++ ) {
        if ( !lv_obj_get_hidden( statusicon[ i ].icon ) ) {
            if ( last_visible == NULL ) {
                if ( !statusbar_expanded ) {
                    lv_obj_align(statusicon[i].icon, statusbar, statusicon[i].align, STATUSBAR_ICON_X_OFFSET, 0 );
                }
            } else {
                lv_obj_align( statusicon[ i ].icon, last_visible, statusicon[ i ].align, -5, 0);
            }
            lv_obj_reset_style_list( statusicon[ i ].icon, LV_OBJ_PART_MAIN );
            lv_obj_add_style( statusicon[ i ].icon, LV_OBJ_PART_MAIN, statusicon[i].style );
            last_visible = statusicon[ i ].icon;
        }
    }
}

void statusbar_event( lv_obj_t * statusbar, lv_event_t event ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    static bool expand = false;

    switch( event ) {
        case LV_EVENT_PRESSED:
            if ( expand ) {
                statusbar_expand( false );
                expand = false;
            } 
            else {
                statusbar_expand( true );
                expand = true;
            }
        default:
            break;
    }
}

void statusbar_set_force_dark( bool dark_mode ) {
    if( dark_mode ) {
        force_dark_mode = true;
    }
    else {
        force_dark_mode = false;
    }
    statusbar_refresh_update = true;
}

bool statusbar_get_force_dark( void ) {
    return( force_dark_mode );
}


void statusbar_set_dark( bool dark_mode ) {
    if ( dark_mode || force_dark_mode ) {
        lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_OPA_90);
        lv_obj_reset_style_list( statusbar, LV_OBJ_PART_MAIN );
        lv_obj_add_style( statusbar, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );

        lv_style_set_bg_color(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, statusbar_extended_color );
        lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, statusbar_extended_color );
        lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, statusbar_extended_color );
    }
    else {
        lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_OPA_20);
        lv_obj_reset_style_list( statusbar, LV_OBJ_PART_MAIN );
        lv_obj_add_style( statusbar, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );

        lv_style_set_bg_color(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, statusbar_retracted_color );
        lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, statusbar_retracted_color );
        lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, statusbar_retracted_color );
    }
}

void statusbar_expand( bool expand ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    if ( expand ) {
        lv_obj_set_height( statusbar, STATUSBAR_EXPAND_HEIGHT );
        statusbar_set_dark( true );
        statusbar_expanded = true;
    } 
    else {
        lv_obj_set_height( statusbar, STATUSBAR_HEIGHT );
        statusbar_set_dark( false );
        statusbar_expanded = false;
        //Save config here if anything has changed
        if( should_save_brightness_config ){
            display_save_config();
            should_save_brightness_config = false;
        }
        if( should_save_sound_config ){
            sound_save_config();
            should_save_sound_config = false;
        }
    }
    statusbar_refresh_update = true;
}

void statusbar_hide( bool hide ) {
    /*
     * check if statusbar ready
     */
    if ( !statusbar_init ) {
        log_e("statusbar not initialized");
        return;
    }

    lv_obj_set_hidden( statusbar, hide );
    statusbar_refresh_update = true;
}

bool statusbar_get_hidden_state( void ) {
    statusbar_refresh_update = true;
    return( lv_obj_get_hidden( statusbar ) );
}