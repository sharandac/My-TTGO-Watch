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

#include "calendar.h"
#include "calendar_db.h"
#include "calendar_day.h"
#include "calendar_overview.h"

#include "gui/mainbar/mainbar.h"
#include "gui/icon.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "utils/alloc.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
    #include <string>

    using namespace std;
    #define String string
#else
    #include <time.h>
    #include <Arduino.h>
#endif
/**
 * calendar tile store
 */
uint32_t calendar_ovreview_tile_num;                                                /** @brief allocated calendar overview tile number */
/**
 * calendar icon and fonts
 */
LV_FONT_DECLARE(Ubuntu_32px);                                                       /** @brief calendar font */
LV_FONT_DECLARE(Ubuntu_16px);                                                       /** @brief calendar font */
LV_FONT_DECLARE(Ubuntu_12px);                                                       /** @brief calendar font */

#if defined( BIG_THEME )
    lv_font_t *calandar_font = &Ubuntu_32px;
#elif defined( MID_THEME )
    lv_font_t *calandar_font = &Ubuntu_16px;
#else
    lv_font_t *calandar_font = &Ubuntu_12px;
#endif

/**
 * calendar objects
 */
lv_obj_t *calendar_overview;                                                        /** @brief calendar lv object */
lv_style_t calendar_overview_style;                                                 /** @brief calendar style object */
lv_calendar_date_t *calendar_overview_highlighted_days = NULL;                      /** @brief highlighted days table */
/**
 * internal variables
 */
static bool calendar_overview_highlight_table[ CALENDAR_OVREVIEW_HIGHLIGHTED_DAYS ];/** @brief highlighted days table bool table for sql query */
static int calendar_year = 0;                                                       /** @brief current year in calendar overview */
static int calendar_month = 0;                                                      /** @brief current month in calendar overview */
static int calendar_day = 0;                                                        /** @brief current day in calendar overview */
/**
 * internal function declaration
 */
static void calendar_overview_exit_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_overview_date_event_cb( lv_obj_t * obj, lv_event_t event );
static int calendar_overview_highlight_day_callback( void *data, int argc, char **argv, char **azColName );
int calendar_overview_highlight_day( int year, int month );
void calendar_overview_build_ui( void );
void calendar_overview_refresh_showed_ui( void );
void calendar_overview_refresh_today_ui( void );
void calendar_overview_activate_cb( void );
void calendar_overview_hibernate_cb( void );
/**
 * setup routine for application
 */
void calendar_overview_setup( void ) {
    /**
     * register a tile
     */
    calendar_ovreview_tile_num = mainbar_add_app_tile( 1, 1, "calendar overview" );
    /**
     * Build and configure application
     */
    calendar_overview_build_ui();
    calendar_overview_refresh_today_ui();
    calendar_overview_refresh_showed_ui();
    /**
     * set activation/hibernation call back
     */
    mainbar_add_tile_activate_cb( calendar_ovreview_tile_num, calendar_overview_activate_cb );
    mainbar_add_tile_hibernate_cb( calendar_ovreview_tile_num, calendar_overview_hibernate_cb );
    /**
     * alloc highlighted days table
     */
    calendar_overview_highlighted_days = (lv_calendar_date_t*)MALLOC( sizeof( lv_calendar_date_t ) * CALENDAR_HIGHLIGHTED_DAYS );
    if ( calendar_overview_highlighted_days ) {
        for( int i = 0 ; i < CALENDAR_OVREVIEW_HIGHLIGHTED_DAYS ; i++ ) {
            calendar_overview_highlighted_days[ i ].year = 0;
            calendar_overview_highlighted_days[ i ].month = 0;
            calendar_overview_highlighted_days[ i ].day = 0;
        }
    }
    else {
        log_e("alloac highlighted days table failed");
        while( true );
    }
}

void calendar_overview_build_ui( void ) {
    /**
     * get calendar object from tile number
     */
    lv_obj_t *calendar_overview_tile = mainbar_get_tile_obj( calendar_ovreview_tile_num );
    /**
     * copy mainbar style and set it to calendar
     */
    lv_style_copy( &calendar_overview_style, APP_STYLE );
    lv_style_set_radius( &calendar_overview_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_border_width( &calendar_overview_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &calendar_overview_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_bg_opa( &calendar_overview_style, LV_OBJ_PART_MAIN, LV_OPA_100 );
    /**
     * create calendar object
     */
    calendar_overview = lv_calendar_create( calendar_overview_tile, NULL);
    lv_obj_set_size( calendar_overview, lv_disp_get_hor_res( NULL ) - THEME_ICON_SIZE, lv_disp_get_ver_res( NULL ) );
    lv_obj_align( calendar_overview, calendar_overview_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0);
    lv_obj_set_event_cb( calendar_overview, calendar_overview_date_event_cb );
    lv_obj_add_style( calendar_overview, LV_OBJ_PART_MAIN, &calendar_overview_style );
    /**
     * Make the date number smaller to be sure they fit into their area
     */
    lv_obj_set_style_local_text_font( calendar_overview, LV_CALENDAR_PART_HEADER, LV_STATE_DEFAULT, calandar_font );
    lv_obj_set_style_local_text_color( calendar_overview, LV_CALENDAR_PART_HEADER, LV_STATE_DEFAULT, LV_COLOR_BLACK );
    lv_obj_set_style_local_text_font( calendar_overview, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, calandar_font );
    lv_obj_set_style_local_text_color( calendar_overview, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, LV_COLOR_BLACK );
    lv_obj_set_style_local_bg_color( calendar_overview, LV_CALENDAR_PART_DATE, LV_STATE_CHECKED, LV_COLOR_RED );
    lv_obj_set_style_local_bg_color( calendar_overview, LV_CALENDAR_PART_DATE, LV_STATE_FOCUSED, LV_COLOR_BLUE );
    /**
     * add exit button
     */
    lv_obj_t *exit_button = wf_add_exit_button( calendar_overview_tile, calendar_overview_exit_event_cb );
    lv_obj_align( exit_button, calendar_overview_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_ICON_PADDING, -THEME_ICON_PADDING );
}

void calendar_overview_refresh_showed_ui( void ) {
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

    lv_calendar_set_showed_date( calendar_overview, &today);
}

void calendar_overview_refresh_today_ui( void ) {
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
    
    lv_calendar_set_today_date( calendar_overview, &today);
}

void calendar_overview_activate_cb( void ) {
    /**
     * open calendar date base
     */
    if ( calendar_db_open() ) {
        /**
         * highlight day with dates
         */
        calendar_overview_refresh_today_ui();
        lv_calendar_set_highlighted_dates( calendar_overview, calendar_overview_highlighted_days, calendar_overview_highlight_day( calendar_year, calendar_month ) );
    }
    else {
        log_e("open calendar date base failed");
    }
}

void calendar_overview_hibernate_cb( void ) {
    /**
     * close calendar date base
     */
    calendar_db_close();
}


static void calendar_overview_date_event_cb( lv_obj_t * obj, lv_event_t event ) {
    lv_calendar_date_t * date;
    
    switch( event ) {
        case LV_EVENT_VALUE_CHANGED:
            date = lv_calendar_get_pressed_date( obj );
            if( date ) {
                calendar_year = date->year;
                calendar_month = date->month;
                calendar_day = date->day;
                CALENDAR_OVREVIEW_DEBUG_LOG("Clicked date: %02d.%02d.%d", date->day, date->month, date->year );
                calendar_day_overview_refresh( date->year, date->month, date->day );
                mainbar_jump_to_tilenumber( calendar_day_get_tile(), LV_ANIM_OFF );
            }
            break;
        case LV_EVENT_CLICKED:
            date = lv_calendar_get_showed_date( obj );
            if( date ) {
                calendar_year = date->year;
                calendar_month = date->month;
                CALENDAR_OVREVIEW_DEBUG_LOG("current year and month: %d %d", date->year, date->month );
            }
            /**
             * highlight day with dates
             */
            lv_calendar_set_highlighted_dates( calendar_overview, calendar_overview_highlighted_days, calendar_overview_highlight_day( calendar_year, calendar_month ) );
            break;
    }
}

static void calendar_overview_exit_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if ( event == LV_EVENT_CLICKED ) {
        mainbar_jump_back();
    }
}

uint32_t calendar_overview_get_tile( void ) {
    return( calendar_ovreview_tile_num );
}

static int calendar_overview_highlight_day_callback( void *data, int argc, char **argv, char **azColName ) {
    String Result = "";
    /**
     * count all key/values pairs
     */
    for ( int i = 0; i < argc ; i++ ) {
       /**
         * build an result string for one line presentation
         */
        Result += i != 0 ? "," : "";
        Result += azColName[i];
        Result += "=";
        Result += argv[i] ? argv[i] : "NULL";
        /**
         * mark day in calendar_overview_highlight_table as a day with an date
         */
        if ( !strcmp( azColName[ i ], "day") ) {
            calendar_overview_highlight_table[ atoi( argv[ i ] ) ] = true;
            CALENDAR_OVREVIEW_DEBUG_LOG("highlight day %d", atoi( argv[ i ] ) );
        }
    }
    CALENDAR_OVREVIEW_DEBUG_LOG("Result = %s", Result.c_str() );
    return 0;
}

int calendar_overview_highlight_day( int year, int month ) {
    int hitcounter = 0;
    /**
     * clear calendar_overview_highlight_table table
     */
    for ( int i = 0 ; i < CALENDAR_OVREVIEW_HIGHLIGHTED_DAYS ; i++ ) {
        calendar_overview_highlight_table[ i ] = false;
    }
    /**
     * build sql query string
     */
#ifdef NATIVE_64BIT
    char sql[ 512 ] = "";
    snprintf( sql, sizeof( sql ), "SELECT rowid, year, month, day, hour, min, content FROM calendar WHERE year == %d AND month == %d;", year, month );
    /**
     * exec sql query
     */
    if ( calendar_db_exec( calendar_overview_highlight_day_callback, sql ) ) {
        /**
         * count day with day and marked days with dates
         */
        for ( int i = 0 ; i < CALENDAR_OVREVIEW_HIGHLIGHTED_DAYS ; i++ ) {
            if ( calendar_overview_highlight_table[ i ] ) {
                CALENDAR_OVREVIEW_DEBUG_LOG("add year %d and month %d to highlight", year, month );
                calendar_overview_highlighted_days[ hitcounter ].day = i;
                calendar_overview_highlighted_days[ hitcounter ].month = month;
                calendar_overview_highlighted_days[ hitcounter ].year = year;
                hitcounter++;
            }
        }
    }
#else
    String sql = (String) "SELECT rowid, year, month, day, hour, min, content FROM calendar WHERE year == " + year + " AND month == " + month + ";";
    /**
     * exec sql query
     */
    if ( calendar_db_exec( calendar_overview_highlight_day_callback, sql.c_str() ) ) {
        /**
         * count day with day and marked days with dates
         */
        for ( int i = 0 ; i < CALENDAR_OVREVIEW_HIGHLIGHTED_DAYS ; i++ ) {
            if ( calendar_overview_highlight_table[ i ] ) {
                CALENDAR_OVREVIEW_DEBUG_LOG("add year %d and month %d to highlight", year, month );
                calendar_overview_highlighted_days[ hitcounter ].day = i;
                calendar_overview_highlighted_days[ hitcounter ].month = month;
                calendar_overview_highlighted_days[ hitcounter ].year = year;
                hitcounter++;
            }
        }
    }
#endif
    return( hitcounter );
}