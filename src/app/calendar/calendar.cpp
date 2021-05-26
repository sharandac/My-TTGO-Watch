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
#include <TTGO.h>

#include "calendar.h"
#include "calendar_db.h"

#include "gui/mainbar/mainbar.h"
#include "gui/icon.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "utils/alloc.h"

/**
 * calendar tile store
 */
uint32_t calendar_main_tile_num;            /** @brief allocated calendar tile number */
/**
 * app icon
 */
icon_t *calendar_icon = NULL;               /** @brief calendar icon */
/**
 * calendar icon
 */
LV_IMG_DECLARE(calendar_64px);              /** @brief calendar icon image */
LV_FONT_DECLARE(Ubuntu_12px);               /** @brief calendar font */
/**
 * calendar objects
 */
static lv_obj_t * calendar;                 /** @brief calendar lv object */
static lv_style_t calendar_style;           /** @brief calendar style object */
lv_calendar_date_t *highlighted_days = NULL;/** @brief highlighted days table */
static int calendar_year = 0;
static int calendar_month = 0;
static int calendar_day = 0;
/**
 * calendar app function declaration
 */
static void calendar_enter_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_date_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_exit_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_build_main_page( void );
static void calendar_refresh_main_page( void );
static void calendar_activate_cb( void );
static void calendar_hibernate_cb( void );
/**
 * setup routine for application
 */
void calendar_app_setup( void ) {
    /**
     * register a tile
     */
    calendar_main_tile_num = mainbar_add_app_tile( 2, 1, "Calendar" );
    /**
     * register app icon on the app tile
     */
    calendar_icon = app_register( "Calendar", &calendar_64px, calendar_enter_event_cb );
    app_set_indicator( calendar_icon, ICON_INDICATOR_OK );
    /**
     * Build and configure application
     */
    calendar_build_main_page();
    calendar_refresh_main_page();
    /**
     * set activation/hibernation call back
     */
    mainbar_add_tile_activate_cb( calendar_main_tile_num, calendar_activate_cb );
    mainbar_add_tile_hibernate_cb( calendar_main_tile_num, calendar_hibernate_cb );
    /**
     * check and init database
     */
    calendar_db_setup();
    /**
     * alloac highlighted days table
     */
    highlighted_days = (lv_calendar_date_t*)MALLOC( sizeof( lv_calendar_date_t ) * CALENDAR_HIGHLIGHTED_DAYS );
    if ( highlighted_days ) {
        for( int i = 0 ; i < CALENDAR_HIGHLIGHTED_DAYS ; i++ ) {
            highlighted_days[ i ].year = 0;
            highlighted_days[ i ].month = 0;
            highlighted_days[ i ].day = 0;
        }
    }
    else {
        log_e("alloac highlighted days table failed");
        while( true );
    }
}

static void calendar_enter_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
               mainbar_jump_to_tilenumber( calendar_main_tile_num, LV_ANIM_OFF );
               statusbar_hide( true );
               app_hide_indicator( calendar_icon );
               break;
    }
}

static void calendar_exit_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if ( event == LV_EVENT_CLICKED ) {
        mainbar_jump_back();
    }
}

static void calendar_date_event_cb( lv_obj_t * obj, lv_event_t event ) {
    lv_calendar_date_t * date;
    
    switch( event ) {
        case LV_EVENT_VALUE_CHANGED:
            date = lv_calendar_get_pressed_date( obj );
            if( date ) {
                calendar_year = date->year;
                calendar_month = date->month;
                calendar_day = date->day;
                log_i("Clicked date: %02d.%02d.%d", date->day, date->month, date->year );
            }
            break;
        case LV_EVENT_CLICKED:
            date = lv_calendar_get_showed_date( obj );
            if( date ) {
                calendar_year = date->year;
                calendar_month = date->month;
                log_i("current year and month: %d %d", date->year, date->month );
            }
            /**
             * highlight day with dates
             */
            lv_calendar_set_highlighted_dates( calendar, highlighted_days, calendar_db_highlight_day( highlighted_days, calendar_year, calendar_month ) );
            break;
    }
}

void calendar_build_main_page( void ) {
    /**
     * get calendar object from tile number
     */
    lv_obj_t *main_tile = mainbar_get_tile_obj( calendar_main_tile_num );
    /**
     * copy mainbar style and set it to calendar
     */
    lv_style_copy( &calendar_style, ws_get_mainbar_style() );
    lv_style_set_radius( &calendar_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_border_width( &calendar_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &calendar_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_bg_opa( &calendar_style, LV_OBJ_PART_MAIN, LV_OPA_80 );
    /**
     * create calendar object
     */
    calendar = lv_calendar_create( main_tile, NULL);
    lv_obj_set_size( calendar, 190, 240 );
    lv_obj_align( calendar, main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_event_cb( calendar, calendar_date_event_cb );
    lv_obj_add_style( calendar, LV_OBJ_PART_MAIN, &calendar_style );
    /**
     * Make the date number smaller to be sure they fit into their area
     */
    lv_obj_set_style_local_text_color( calendar, LV_CALENDAR_PART_HEADER, LV_STATE_DEFAULT, LV_COLOR_BLACK );
    lv_obj_set_style_local_text_font( calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, &Ubuntu_12px );
    lv_obj_set_style_local_text_color( calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, LV_COLOR_BLACK );
    lv_obj_set_style_local_bg_color( calendar, LV_CALENDAR_PART_DATE, LV_STATE_CHECKED, LV_COLOR_RED );
    lv_obj_set_style_local_bg_color( calendar, LV_CALENDAR_PART_DATE, LV_STATE_FOCUSED, LV_COLOR_BLUE );
    /**
     * add exit button
     */
    lv_obj_t *exit_button = wf_add_exit_button(main_tile, calendar_exit_event_cb, ws_get_mainbar_style() );
    lv_obj_align( exit_button, main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
}

void calendar_refresh_main_page( void ) {
    /**
     * Set today's date
     */
    time_t now;
    struct tm time_tm;
    time( &now );
    localtime_r( &now, &time_tm );

    lv_calendar_date_t today;
    today.year = time_tm.tm_year + 1900;
    today.month = time_tm.tm_mon + 1;
    today.day = time_tm.tm_mday;

    calendar_year = today.year;
    calendar_month = today.month;
    calendar_day = today.day;

    lv_calendar_set_today_date(calendar, &today);
    lv_calendar_set_showed_date(calendar, &today);
/*
    static lv_calendar_date_t highlighted_days[3];
    highlighted_days[0].year = 2021;
    highlighted_days[0].month = 5;
    highlighted_days[0].day = 6;
    highlighted_days[1].year = 2021;
    highlighted_days[1].month = 5;
    highlighted_days[1].day = 27;
    lv_calendar_set_highlighted_dates( calendar, highlighted_days, 2 );
*/
}

void calendar_activate_cb( void ) {
    /**
     * refresh the calendar on activation
     */
    calendar_refresh_main_page();
    /**
     * open calendar date base
     */
    if ( calendar_db_open() ) {
        log_e("open calendar date base failed");
    }
    /**
     * highlight day with dates
     */
    lv_calendar_set_highlighted_dates( calendar, highlighted_days, calendar_db_highlight_day( highlighted_days, calendar_year, calendar_month ) );
}

void calendar_hibernate_cb( void ) {
    /**
     * close calendar date base
     */
    calendar_db_close();
}
