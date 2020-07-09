#include "config.h"
#include <stdio.h>

#include "config.h"
#include <Arduino.h>
#include <time.h>
#include "gui.h"
#include <WiFi.h>
#include "string.h"
#include <Ticker.h>
#include "FS.h"
#include "SD.h"

#include "statusbar.h"

#include "hardware/motor.h"
#include "hardware/powermgm.h"
#include "hardware/wifictl.h"

static lv_obj_t *statusbar = NULL;
static lv_obj_t *statusbar_wifi = NULL;
static lv_obj_t *statusbar_wifilabel = NULL;
static lv_obj_t *statusbar_bluetooth = NULL;
static lv_obj_t *statusbar_bluetoothlabel = NULL;
static lv_obj_t *statusbar_stepcounterlabel = NULL;
static lv_style_t statusbarstyle[ STATUSBAR_STYLE_NUM ];

lv_status_bar_t statusicon[ STATUSBAR_NUM ] = 
{
    { NULL, NULL, LV_ALIGN_IN_TOP_RIGHT, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_BATTERY_FULL, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_WIFI, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_BELL, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
    { NULL, LV_SYMBOL_WARNING, LV_ALIGN_OUT_LEFT_MID, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] },
};

void statusbar_event( lv_obj_t * statusbar, lv_event_t event );
void statusbar_wifi_event_cb( lv_obj_t *wifi, lv_event_t event );
void statusbar_bluetooth_event_cb( lv_obj_t *wifi, lv_event_t event );

LV_IMG_DECLARE(wifi_64px);
LV_IMG_DECLARE(bluetooth_64px);
LV_IMG_DECLARE(foot_16px);
    
/**
 * Create a demo application
 */
void statusbar_setup( void )
{
    /*Copy a built-in style to initialize the new style*/
    lv_style_init(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    lv_style_set_radius(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_OPA_20);
    lv_style_set_border_width(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE);

    lv_style_copy( &statusbarstyle[ STATUSBAR_STYLE_WHITE ], &statusbarstyle[ STATUSBAR_STYLE_WHITE ] );
    lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_text_color(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_image_recolor(&statusbarstyle[ STATUSBAR_STYLE_WHITE ], LV_OBJ_PART_MAIN, LV_COLOR_WHITE);

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
        if ( i == 0 )
            lv_obj_align(statusicon[i].icon, NULL, statusicon[i].align, -5, 4);
        else
            lv_obj_align(statusicon[i].icon, statusicon[i-1].icon, statusicon[i].align, -5, 0);
    }

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_copy( &style, &statusbarstyle[ STATUSBAR_STYLE_GRAY ] );

    lv_style_set_image_recolor_opa(&style, LV_BTN_STATE_RELEASED, LV_OPA_100);
    lv_style_set_image_recolor(&style, LV_BTN_STATE_RELEASED, LV_COLOR_GRAY);
    lv_style_set_image_recolor_opa(&style, LV_BTN_STATE_PRESSED, LV_OPA_100);
    lv_style_set_image_recolor(&style, LV_BTN_STATE_PRESSED, LV_COLOR_GREEN);
    lv_style_set_image_recolor_opa(&style, LV_BTN_STATE_CHECKED_RELEASED, LV_OPA_100);
    lv_style_set_image_recolor(&style, LV_BTN_STATE_CHECKED_RELEASED, LV_COLOR_GRAY);
    lv_style_set_image_recolor_opa(&style, LV_BTN_STATE_CHECKED_PRESSED, LV_OPA_100);
    lv_style_set_image_recolor(&style, LV_BTN_STATE_CHECKED_PRESSED, LV_COLOR_GRAY);
    lv_style_set_image_recolor_opa(&style, LV_BTN_STATE_DISABLED, LV_OPA_100);
    lv_style_set_image_recolor(&style, LV_BTN_STATE_DISABLED, LV_COLOR_GREEN);

    statusbar_wifi = lv_imgbtn_create( statusbar, NULL);
    lv_imgbtn_set_src(statusbar_wifi, LV_BTN_STATE_RELEASED, &wifi_64px);
    lv_imgbtn_set_src(statusbar_wifi, LV_BTN_STATE_PRESSED, &wifi_64px);
    lv_imgbtn_set_src(statusbar_wifi, LV_BTN_STATE_CHECKED_RELEASED, &wifi_64px);
    lv_imgbtn_set_src(statusbar_wifi, LV_BTN_STATE_CHECKED_PRESSED, &wifi_64px);
    lv_imgbtn_set_checkable(statusbar_wifi, true);
    lv_obj_add_style(statusbar_wifi, LV_IMGBTN_PART_MAIN, &style );
    lv_obj_align(statusbar_wifi, statusbar, LV_ALIGN_CENTER, 0, STATUSBAR_EXPAND_HEIGHT / 2 );
    lv_obj_set_event_cb(statusbar_wifi, statusbar_wifi_event_cb );
    lv_imgbtn_set_state( statusbar_wifi, LV_BTN_STATE_CHECKED_RELEASED );

    /*Create a label on the Image button*/
    statusbar_wifilabel = lv_label_create(statusbar, NULL);
    lv_obj_reset_style_list( statusbar_wifilabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( statusbar_wifilabel, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_GREEN ] );
    lv_label_set_text(statusbar_wifilabel, "");
    lv_obj_align(statusbar_wifilabel, statusbar_wifi, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );

    lv_obj_t *statusbar_stepicon = lv_img_create(statusbar, NULL );
    lv_img_set_src( statusbar_stepicon, &foot_16px );
    lv_obj_reset_style_list( statusbar_stepicon, LV_OBJ_PART_MAIN );
    lv_obj_add_style( statusbar_stepicon, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] );
    lv_obj_align(statusbar_stepicon, statusbar, LV_ALIGN_IN_TOP_LEFT, 5, 4 );

    statusbar_stepcounterlabel = lv_label_create(statusbar, NULL);
    lv_obj_reset_style_list( statusbar_stepcounterlabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( statusbar_stepcounterlabel, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_WHITE ] );
    lv_label_set_text(statusbar_stepcounterlabel, "0");
    lv_obj_align(statusbar_stepcounterlabel, statusbar_stepicon, LV_ALIGN_OUT_RIGHT_MID, 5, 0 );

    statusbar_hide_icon( STATUSBAR_BELL );
    statusbar_hide_icon( STATUSBAR_WARNING );
    statusbar_hide_icon( STATUSBAR_WIFI );
}

/*
 *
 */
void statusbar_wifi_event_cb( lv_obj_t *wifi, lv_event_t event ) {
    if ( event == LV_EVENT_VALUE_CHANGED ) {
        switch ( lv_imgbtn_get_state( wifi ) ) {
            case( LV_BTN_STATE_CHECKED_RELEASED ):  wifictl_off(); 
                                            break;
            case( LV_BTN_STATE_RELEASED ):   wifictl_on();
                                            break;
            default:                        break;
        }
        motor_vibe( 1 );
    }
}

/*
 *
 */
void statusbar_bluetooth_event_cb( lv_obj_t *wifi, lv_event_t event ) {
    if ( event == LV_EVENT_VALUE_CHANGED ) {
        switch ( lv_imgbtn_get_state( wifi ) ) {
            case( LV_BTN_STATE_RELEASED ):   break;
            case( LV_BTN_STATE_PRESSED ):    break;
            default:                        break;
        }
        motor_vibe( 1 );
    }
}

/*
 *
 */
void statusbar_wifi_set_state( bool state, const char *wifiname ) {
    if( state ) {
        lv_imgbtn_set_state( statusbar_wifi, LV_BTN_STATE_RELEASED );
    }
    else {
        lv_imgbtn_set_state( statusbar_wifi, LV_BTN_STATE_CHECKED_RELEASED );
    }
    lv_label_set_text( statusbar_wifilabel, wifiname );
    lv_obj_align( statusbar_wifilabel, statusbar_wifi, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    statusbar_refresh();
}

/*
 *
 */
void statusbar_bluetooth_set_state( bool state ) {
    if ( state ) {
        lv_imgbtn_set_state( statusbar_bluetooth, LV_BTN_STATE_RELEASED );
    }
    else {
        lv_imgbtn_set_state( statusbar_bluetooth, LV_BTN_STATE_PRESSED );
    }
    statusbar_refresh();
}

/*
 *
 */
void statusbar_hide_icon( int icon ) {
    if ( icon >= STATUSBAR_NUM ) return;

    lv_obj_set_hidden( statusicon[ icon ].icon, true );
    statusbar_refresh();
}

/*
 *
 */
void statusbar_show_icon( int icon ) {
    if ( icon >= STATUSBAR_NUM ) return;

    lv_obj_set_hidden( statusicon[ icon ].icon, false );
    statusbar_refresh();
}

/*
 *
 */
void statusbar_style_icon( int icon, int style ) {
    if ( icon >= STATUSBAR_NUM || style >= STATUSBAR_STYLE_NUM ) return;

    statusicon[ icon ].style = &statusbarstyle[ style ];
    statusbar_refresh();
}
/*
 *
 */
void statusbar_refresh( void ) {
    for ( int i = 0 ; i < STATUSBAR_NUM ; i++ ) {
        if ( !lv_obj_get_hidden( statusicon[ i ].icon ) ) {
            if ( i == 0 ) {
                lv_obj_align( statusicon[ i ].icon, NULL, statusicon[ i ].align, -5, 4);
            } else {
                lv_obj_align( statusicon[ i ].icon, statusicon[ i - 1 ].icon, statusicon[ i ].align, -5, 0);
            }
            lv_obj_reset_style_list( statusicon[ i ].icon, LV_OBJ_PART_MAIN );
            lv_obj_add_style( statusicon[ i ].icon, LV_OBJ_PART_MAIN, statusicon[i].style );
//            lv_obj_set_style( statusicon[ i ].icon, statusicon[i].style );
        }
    }
}

/*
 *
 */
void statusbar_event( lv_obj_t * statusbar, lv_event_t event ) {
    if ( event == LV_EVENT_PRESSED ) {
        lv_obj_set_height( statusbar, STATUSBAR_EXPAND_HEIGHT );
        lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_OPA_50);
        lv_obj_reset_style_list( statusbar, LV_OBJ_PART_MAIN );
        lv_obj_add_style( statusbar, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    } 
    else if ( event == LV_EVENT_RELEASED ) {
        lv_obj_set_height( statusbar, STATUSBAR_HEIGHT );
        lv_style_set_bg_opa(&statusbarstyle[ STATUSBAR_STYLE_NORMAL ], LV_OBJ_PART_MAIN, LV_OPA_20);
        lv_obj_reset_style_list( statusbar, LV_OBJ_PART_MAIN );
        lv_obj_add_style( statusbar, LV_OBJ_PART_MAIN, &statusbarstyle[ STATUSBAR_STYLE_NORMAL ] );
    }
    statusbar_refresh();
}


/*
 *
 */
void statusbar_update_stepcounter( int step ) {
    char stepcounter[12]="";
    snprintf( stepcounter, sizeof( stepcounter ), "%d%", step );    
    lv_label_set_text( statusbar_stepcounterlabel, (const char *)stepcounter );
}
/*
 *
 */
void statusbar_update_battery( uint32_t percent, bool charging, bool plug ) {
    char level[8]="";
    snprintf( level, sizeof( level ), "%d%%", percent );
    lv_label_set_text( statusicon[  STATUSBAR_BATTERY_PERCENT ].icon, (const char *)level );   

    if ( charging && plug ) {
        lv_img_set_src( statusicon[ STATUSBAR_BATTERY ].icon, LV_SYMBOL_CHARGE );
        statusbar_style_icon( STATUSBAR_BATTERY, STATUSBAR_STYLE_RED );
    }
    else { 
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
        }
        else if ( percent >= 15 ) {
            statusbar_style_icon( STATUSBAR_BATTERY, STATUSBAR_STYLE_YELLOW );
        }
        else {
            statusbar_style_icon( STATUSBAR_BATTERY, STATUSBAR_STYLE_RED );
        }

        if ( plug ) {
            statusbar_style_icon( STATUSBAR_BATTERY, STATUSBAR_STYLE_GREEN );
        }
    }
    statusbar_refresh();
}
