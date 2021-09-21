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
#include "calendar_create.h"

#include "gui/mainbar/mainbar.h"
#include "gui/icon.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
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
    #include <Arduino.h>
#endif
/**
 * calendar tile store
 */
uint32_t calendar_create_tile_num;                  /** @brief allocated calendar ctreate/edit tile number */
uint32_t calendar_create_date_select_tile_num;      /** @brief allocated calendar date select tile number */
/**
 * calendar icon
 */
LV_FONT_DECLARE(Ubuntu_12px);                   /** @brief calendar font */
LV_FONT_DECLARE(Ubuntu_16px);                   /** @brief calendar font */
LV_FONT_DECLARE(Ubuntu_32px);                   /** @brief calendar font */

#if defined( BIG_THEME )
    lv_font_t *date_create_font = &Ubuntu_32px;
#elif defined( MID_THEME )
    lv_font_t *date_create_font = &Ubuntu_16px;
#else
    lv_font_t *date_create_font = &Ubuntu_12px;
#endif
/**
 * lv objects
 */
lv_obj_t *calendar_create_hour_list = NULL;         /** @brief hour dropdown list */
lv_obj_t *calendar_create_min_list = NULL;          /** @brief min dropdown list */
lv_obj_t *claendar_create_textfield = NULL;         /** @brief data content textfield */
lv_obj_t *calendar_select_date_btn_label = NULL;    /** @brief selected date button */
lv_obj_t *calendar_create_date_select = NULL;       /** @brief select date calendar */
lv_style_t calendar_create_date_select_style;       /** @brief calendar select style */
/**
 * internal variables
 */
static int calendar_create_edit_rowid = -1;
static int calendar_create_year = 0;                /** @brief current year in calendar overview */
static int calendar_create_month = 0;               /** @brief current month in calendar overview */
static int calendar_create_day = 0;                 /** @brief current day in calendar overview */
static int calendar_create_hour = 0;                /** @brief current day in calendar overview */
static int calendar_create_min = 0;                 /** @brief current day in calendar overview */
/**
 * internal function declaration
 */
void calendar_create_build_ui( void );
static int calendar_create_get_content_callback( void *data, int argc, char **argv, char **azColName );
static int calendar_create_dummy_callback( void *data, int argc, char **argv, char **azColName );
static void calendar_create_date_selected_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_create_date_select_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_create_exit_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_create_add_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_create_trash_event_cb( lv_obj_t * obj, lv_event_t event );
static void calendar_create_text_event_cb( lv_obj_t * obj, lv_event_t event );
void calendar_create_update_date( void );
void calendar_create_insert_date( void );
void calendar_create_delete_date( void );
void calendar_create_activate_cb( void );
void calendar_create_hibernate_cb( void );
/**
 * setup routine for application
 */
void calendar_create_setup( void ) {
    /**
     * register a tile
     */
    calendar_create_tile_num = mainbar_add_app_tile( 1, 1, "calendar create" );
    calendar_create_date_select_tile_num = mainbar_add_app_tile( 1, 1, "calendar create select date" );
    /**
     * set activation/hibernation call back
     */
    mainbar_add_tile_activate_cb( calendar_create_tile_num, calendar_create_activate_cb );
    mainbar_add_tile_hibernate_cb( calendar_create_tile_num, calendar_create_hibernate_cb );
    /**
     * build calendar day ovweview ui
     */
    calendar_create_build_ui();
}

uint32_t calendar_create_get_tile( void ) {
    return( calendar_create_tile_num );
}

void calendar_create_build_ui( void ) {
    /**
     * get calendar object from tile number
     */
    lv_obj_t *calendar_create_tile = mainbar_get_tile_obj( calendar_create_tile_num );
    lv_obj_t *calendar_create_date_select_tile = mainbar_get_tile_obj( calendar_create_date_select_tile_num );
    /**
     * copy mainbar style and set it to calendar
     */
    lv_style_copy( &calendar_create_date_select_style, APP_STYLE );
    lv_style_set_radius( &calendar_create_date_select_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_border_width( &calendar_create_date_select_style, LV_OBJ_PART_MAIN, 0 );
    lv_style_set_bg_color( &calendar_create_date_select_style, LV_OBJ_PART_MAIN, LV_COLOR_WHITE );
    lv_style_set_bg_opa( &calendar_create_date_select_style, LV_OBJ_PART_MAIN, LV_OPA_100 );
    /**
     * 
     */
    calendar_create_date_select = lv_calendar_create( calendar_create_date_select_tile, NULL );
    lv_obj_set_size( calendar_create_date_select, lv_disp_get_hor_res( NULL ) - THEME_ICON_SIZE, lv_disp_get_ver_res( NULL ) );
    lv_obj_align( calendar_create_date_select, calendar_create_date_select_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( calendar_create_date_select, calendar_create_date_selected_event_cb );
    lv_obj_add_style( calendar_create_date_select, LV_OBJ_PART_MAIN, &calendar_create_date_select_style );
    lv_obj_set_style_local_text_font( calendar_create_date_select, LV_CALENDAR_PART_HEADER, LV_STATE_DEFAULT, date_create_font );
    lv_obj_set_style_local_text_color( calendar_create_date_select, LV_CALENDAR_PART_HEADER, LV_STATE_DEFAULT, LV_COLOR_BLACK );
    lv_obj_set_style_local_text_font( calendar_create_date_select, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, date_create_font );
    lv_obj_set_style_local_text_color( calendar_create_date_select, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, LV_COLOR_BLACK );
    lv_obj_set_style_local_bg_color( calendar_create_date_select, LV_CALENDAR_PART_DATE, LV_STATE_CHECKED, LV_COLOR_RED );
    lv_obj_set_style_local_bg_color( calendar_create_date_select, LV_CALENDAR_PART_DATE, LV_STATE_FOCUSED, LV_COLOR_BLUE );
    /**
     * add exit button
     */
    lv_obj_t *date_select_exit_button = wf_add_exit_button( calendar_create_date_select_tile, calendar_create_exit_event_cb );
    lv_obj_align( date_select_exit_button, calendar_create_date_select_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_ICON_PADDING, -THEME_ICON_PADDING );
    /**
     * 
     */
    lv_obj_t *calendar_select_date_btn = lv_btn_create( calendar_create_tile , NULL);
    lv_obj_add_style( calendar_select_date_btn, LV_OBJ_PART_MAIN, ws_get_button_style() );
    lv_obj_set_size( calendar_select_date_btn, lv_disp_get_hor_res( NULL ) - ( 2 * THEME_PADDING ), 40 );
    lv_obj_align( calendar_select_date_btn, NULL, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, THEME_PADDING );
    lv_obj_set_event_cb( calendar_select_date_btn, calendar_create_date_select_event_cb );
    calendar_select_date_btn_label = lv_label_create( calendar_select_date_btn, NULL );
    lv_label_set_text( calendar_select_date_btn_label, "n / a" );
    /**
     * 
     */
    calendar_create_hour_list = wf_add_list( calendar_create_tile, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23", ws_get_setup_dropdown_style() );
    lv_obj_set_size( calendar_create_hour_list, lv_disp_get_hor_res( NULL ) / 2 - THEME_PADDING - THEME_PADDING / 2, 40 );
    lv_obj_align( calendar_create_hour_list, calendar_select_date_btn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, THEME_PADDING );
    /**
     * 
     */
    calendar_create_min_list = wf_add_list( calendar_create_tile, "0\n15\n30\n45", ws_get_setup_dropdown_style() );
    lv_obj_set_size( calendar_create_min_list, lv_disp_get_hor_res( NULL ) / 2 - THEME_PADDING - THEME_PADDING / 2 , 40 );
    lv_obj_align( calendar_create_min_list, calendar_select_date_btn, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, THEME_PADDING );
    /**
     * 
     */
    claendar_create_textfield = lv_textarea_create( calendar_create_tile, NULL);
    lv_textarea_set_text( claendar_create_textfield, "" );
    lv_textarea_set_pwd_mode( claendar_create_textfield, false );
    lv_textarea_set_cursor_hidden( claendar_create_textfield, false );
    lv_textarea_set_one_line( claendar_create_textfield, true);
    // lv_obj_set_height( claendar_create_textfield, 140 );
    lv_obj_set_width( claendar_create_textfield, lv_disp_get_hor_res( NULL ) - ( 2 * THEME_PADDING ) );
    lv_obj_align( claendar_create_textfield, calendar_create_hour_list, LV_ALIGN_OUT_BOTTOM_LEFT, 0, THEME_PADDING );
    lv_obj_set_event_cb( claendar_create_textfield, calendar_create_text_event_cb );
    lv_obj_add_style( claendar_create_textfield, LV_OBJ_PART_MAIN, ws_get_button_style() );
    /**
     * add exit button
     */
    lv_obj_t *exit_button = wf_add_exit_button( calendar_create_tile, calendar_create_exit_event_cb );
    lv_obj_align( exit_button, calendar_create_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_ICON_PADDING, -THEME_ICON_PADDING );
    /**
     * add add button
     */
    lv_obj_t *create_button = wf_add_add_button( calendar_create_tile, calendar_create_add_event_cb );
    lv_obj_align( create_button, calendar_create_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_ICON_PADDING, -THEME_ICON_PADDING );
    /**
     * add trash button
     */
    lv_obj_t *trash_button = wf_add_trash_button( calendar_create_tile, calendar_create_trash_event_cb );
    lv_obj_align( trash_button, calendar_create_tile, LV_ALIGN_IN_BOTTOM_MID, 0, -THEME_ICON_PADDING );
}

static void calendar_create_date_selected_event_cb( lv_obj_t * obj, lv_event_t event ) {
    lv_calendar_date_t * date;
    
    switch( event ) {
        case LV_EVENT_VALUE_CHANGED:
            date = lv_calendar_get_pressed_date( obj );
            if( date ) {
                calendar_create_year = date->year;
                calendar_create_month = date->month;
                calendar_create_day = date->day;
                CALENDAR_DEBUG_LOG("selected date: %04d-%02d-%02d", calendar_create_year, calendar_create_month, calendar_create_day );
                calendar_day_overview_refresh( calendar_create_year, calendar_create_month, calendar_create_day );
            }
            mainbar_jump_back();
            break;    
    }
}

static void calendar_create_date_select_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_CLICKED:
            mainbar_jump_to_tilenumber( calendar_create_date_select_tile_num, LV_ANIM_OFF );
            break;
    }
}

static void calendar_create_exit_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_CLICKED:
            mainbar_jump_back();
            break;
    }
}

static void calendar_create_add_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_CLICKED:
            if ( calendar_create_edit_rowid != -1 ) {
                calendar_create_update_date();
            }
            else {
                calendar_create_insert_date();
            }
            calendar_day_overview_refresh( calendar_create_year, calendar_create_month, calendar_create_day );
            mainbar_jump_back();
            break;
    }
}

static void calendar_create_trash_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_CLICKED:
            calendar_create_delete_date();
            calendar_day_overview_refresh( calendar_create_year, calendar_create_month, calendar_create_day );
            mainbar_jump_back();
            break;
    }
}

static void calendar_create_text_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       keyboard_set_textarea( obj );
                                        break;
    }
}

void calendar_create_set_date( int year, int month, int day ) {
    calendar_create_year = year;
    calendar_create_month = month;
    calendar_create_day = day;
}

void calendar_create_set_time( int hour, int min ) {
    calendar_create_hour = hour;
    calendar_create_min = min;
    lv_dropdown_set_selected( calendar_create_hour_list, calendar_create_hour );
    lv_dropdown_set_selected( calendar_create_min_list, calendar_create_min / 15 );
}

void calendar_create_set_content( void ) {
    /**
     * clear textfield
     */
    lv_textarea_set_text( claendar_create_textfield, "" );
    calendar_create_edit_rowid = -1;
    /**
     * build sql query string
     */
#ifdef NATIVE_64BIT
    char sql[512]="";
    snprintf( sql, sizeof( sql ),   "SELECT rowid, content FROM calendar WHERE year == %d AND month == %d AND day == %d AND hour == %d AND min == %d;",
                                    calendar_create_year,
                                    calendar_create_month,
                                    calendar_create_day,
                                    calendar_create_hour,
                                    calendar_create_min );
    /**
     * exec sql query
     */
    if ( calendar_db_exec( calendar_create_get_content_callback, sql ) ) {
        if ( calendar_create_edit_rowid != -1 ) {
            CALENDAR_DAY_DEBUG_LOG("entry exist, set content from rowid %d", calendar_create_edit_rowid );
        }
        else {
            CALENDAR_DAY_DEBUG_LOG("new entry");
        }
    }
#else
    String sql = (String) "SELECT rowid, content FROM calendar WHERE year == " + calendar_create_year + " AND month == " + calendar_create_month + " AND day == " + calendar_create_day + " AND hour == " + calendar_create_hour + " AND min == " + calendar_create_min + ";";
    /**
     * exec sql query
     */
    if ( calendar_db_exec( calendar_create_get_content_callback, sql.c_str() ) ) {
        if ( calendar_create_edit_rowid != -1 ) {
            CALENDAR_DAY_DEBUG_LOG("entry exist, set content from rowid %d", calendar_create_edit_rowid );
        }
        else {
            CALENDAR_DAY_DEBUG_LOG("new entry");
        }
    }
#endif
}

void calendar_create_clear_content( void ) {
    /**
     * clear textfield
     */
    lv_textarea_set_text( claendar_create_textfield, "" );
    calendar_create_edit_rowid = -1;
}

static int calendar_create_get_content_callback( void *data, int argc, char **argv, char **azColName ) {
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
        if( !strcmp( azColName[i], "content" ) ) {
            lv_textarea_set_text( claendar_create_textfield, argv[i] );
        }
        if( !strcmp( azColName[i], "rowid" ) ) {
            calendar_create_edit_rowid = atoi( argv[i] );
        }
    }
    CALENDAR_DAY_DEBUG_LOG("Result = %s", Result.c_str() );
    return( 0 );
}

static int calendar_create_dummy_callback( void *data, int argc, char **argv, char **azColName ) {
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
    }
    CALENDAR_DAY_DEBUG_LOG("Result = %s", Result.c_str() );
    return( 0 );
}

void calendar_create_update_date( void ) {
    /**
     * build sql query string
     */
    char date[30]="";
    snprintf( date, sizeof( date ),"%04d%02d%02d%02d%02d", calendar_create_year, calendar_create_month, calendar_create_day, lv_dropdown_get_selected( calendar_create_hour_list ), lv_dropdown_get_selected( calendar_create_min_list ) * 15 );
#ifdef NATIVE_64BIT
    char sql[512]="";
    snprintf( sql, sizeof( sql ), "UPDATE calendar SET date=%s,year=%d,month=%d,day=%d,hour=%d,min=%d,content='%s' WHERE rowid == %d;",
                            date,
                            calendar_create_year,
                            calendar_create_month,
                            calendar_create_day,
                            lv_dropdown_get_selected( calendar_create_hour_list ),
                            lv_dropdown_get_selected( calendar_create_min_list ) * 15,
                            lv_textarea_get_text( claendar_create_textfield ),
                            calendar_create_edit_rowid );

    CALENDAR_DAY_DEBUG_LOG("UPDATE query: %s", sql );
    /**
     * exec sql query
     */
    if ( calendar_db_exec( calendar_create_dummy_callback, sql ) ) {
        CALENDAR_DAY_DEBUG_LOG("sql query ok");
    }
#else
    String sql = (String)   "UPDATE calendar SET " + 
                            "date=" + date + "," +
                            "year=" + calendar_create_year + "," +
                            "month=" + calendar_create_month + "," +
                            "day=" + calendar_create_day + "," +
                            "hour=" + lv_dropdown_get_selected( calendar_create_hour_list ) + "," +
                            "min=" + lv_dropdown_get_selected( calendar_create_min_list ) * 15 + "," +
                            "content='" + lv_textarea_get_text( claendar_create_textfield ) + "' " +
                            "WHERE rowid == " + calendar_create_edit_rowid + ";";
    CALENDAR_DAY_DEBUG_LOG("UPDATE query: %s", sql.c_str() );
    /**
     * exec sql query
     */
    if ( calendar_db_exec( calendar_create_dummy_callback, sql.c_str() ) ) {
        CALENDAR_DAY_DEBUG_LOG("sql query ok");
    }
#endif
}

void calendar_create_insert_date( void ) {
    /**
     * build sql query string
     */
    char date[30]="";
    snprintf( date, sizeof( date ),"%04d%02d%02d%02d%02d", calendar_create_year, calendar_create_month, calendar_create_day, lv_dropdown_get_selected( calendar_create_hour_list ), lv_dropdown_get_selected( calendar_create_min_list ) * 15 );
#ifdef NATIVE_64BIT
    char sql[512]="";
    snprintf( sql, sizeof( sql ),   "INSERT INTO calendar VALUES ( %s, %d, %d, %d, %d, %d, '%s');" ,
                                    date,
                                    calendar_create_year, 
                                    calendar_create_month,
                                    calendar_create_day,
                                    lv_dropdown_get_selected( calendar_create_hour_list ),
                                    lv_dropdown_get_selected( calendar_create_min_list ) * 15,
                                    lv_textarea_get_text( claendar_create_textfield ) );

    CALENDAR_DAY_DEBUG_LOG("UPDATE query: %s", sql );
    /**
     * exec sql query
     */
    if ( calendar_db_exec( calendar_create_dummy_callback, sql ) ) {
        CALENDAR_DAY_DEBUG_LOG("sql query ok");
    }
#else
    String sql = (String)   "INSERT INTO calendar VALUES ( " +
                            date + "," +
                            calendar_create_year + "," +
                            calendar_create_month + "," +
                            calendar_create_day + "," +
                            lv_dropdown_get_selected( calendar_create_hour_list ) + "," +
                            lv_dropdown_get_selected( calendar_create_min_list ) * 15 + "," +
                            "'" + lv_textarea_get_text( claendar_create_textfield ) + "'" +
                            ");";

    CALENDAR_DAY_DEBUG_LOG("UPDATE query: %s", sql.c_str() );
    /**
     * exec sql query
     */
    if ( calendar_db_exec( calendar_create_dummy_callback, sql.c_str() ) ) {
        CALENDAR_DAY_DEBUG_LOG("sql query ok");
    }
#endif
}

void calendar_create_delete_date( void ) {
    /**
     * build sql query string
     */
#ifdef NATIVE_64BIT
    char sql[512] = "";
    snprintf( sql, sizeof( sql ), "DELETE FROM calendar WHERE rowid == %d;", calendar_create_edit_rowid );
    CALENDAR_DAY_DEBUG_LOG("UPDATE query: %s", sql );
    /**
     * exec sql query
     */
    if ( calendar_db_exec( calendar_create_dummy_callback, sql ) ) {
        CALENDAR_DAY_DEBUG_LOG("sql query ok");
    }
#else
    String sql = (String)   "DELETE FROM calendar WHERE rowid == " + calendar_create_edit_rowid + ";";
    CALENDAR_DAY_DEBUG_LOG("UPDATE query: %s", sql.c_str() );
    /**
     * exec sql query
     */
    if ( calendar_db_exec( calendar_create_dummy_callback, sql.c_str() ) ) {
        CALENDAR_DAY_DEBUG_LOG("sql query ok");
    }
#endif
}

void calendar_create_activate_cb( void ) {
    /**
     * set select date btn label
     */
    char select_date[32]="";
    snprintf( select_date, sizeof( select_date ), "%d-%d-%d", calendar_create_year, calendar_create_month, calendar_create_day );
    lv_label_set_text( calendar_select_date_btn_label, select_date );
    /**
     * set select date calender
     */
    lv_calendar_date_t today;
    today.year = calendar_create_year;
    today.month = calendar_create_month;
    today.day = calendar_create_day;
    lv_calendar_set_today_date( calendar_create_date_select, &today);
    lv_calendar_set_showed_date( calendar_create_date_select, &today);    
    /**
     * open calendar date base
     */
    if ( !calendar_db_open() ) {
        log_e("open calendar date base failed");
    }
}

void calendar_create_hibernate_cb( void ) {
    /**
     * close calendar date base
     */
    calendar_db_close();
}