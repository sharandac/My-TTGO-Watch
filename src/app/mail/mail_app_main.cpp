/****************************************************************************
 *   Sep 29 12:17:11 2021
 *   Copyright  2021  Dirk Brosswick
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
#include "mail_app.h"
#include "mail_app_main.h"
#include "gui/mainbar/mainbar.h"
#include "gui/widget_factory.h"

#if defined( NATIVE_64BIT )
    #include "utils/logging.h"
#else
    #include <Arduino.h>
#endif

uint32_t mail_main_tile_num = 0;
lv_obj_t *mail_main_tile = NULL;
lv_obj_t *mail_main_overview_page = NULL;
lv_obj_t *mail_main_overview = NULL;
lv_style_t mail_main_cell_style;

bool mail_main_style_event_cb( EventBits_t event, void *arg );
static void mail_main_refresh_event_cb( lv_obj_t *obj, lv_event_t event );
static void mail_main_setup_event_cb( lv_obj_t *obj, lv_event_t event );
bool mail_main_button_event_cb( EventBits_t event, void *arg );
void mail_main_refresh( void );
void mail_main_setup( void );
void mail_main_clear_overview( void );
void mail_main_add_mail_entry( const char *from, const char *date );

void mail_app_main_setup( uint32_t tile_num ) {
    /**
     * get tile num and obj
     */
    mail_main_tile_num = tile_num;
    mail_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &mail_main_cell_style, APP_STYLE );
    lv_style_set_pad_all( &mail_main_cell_style, LV_TABLE_PART_BG, 0 );
    /**
     * set button callback for this tile
     * set style change event
     */
    mainbar_add_tile_button_cb( mail_main_tile_num, mail_main_button_event_cb );
    styles_register_cb( STYLE_CHANGE, mail_main_style_event_cb, "mail style" );
    /**
     * build header
     */
    lv_obj_t *mail_main_exit_btn = wf_add_exit_button( mail_main_tile );
    lv_obj_align( mail_main_exit_btn, mail_main_tile, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, THEME_PADDING );
    lv_obj_t *mail_main_header_label = lv_label_create( mail_main_tile, NULL );
    lv_label_set_text( mail_main_header_label, "mail" );
    lv_obj_add_style( mail_main_header_label, LV_OBJ_PART_MAIN, APP_STYLE );
    lv_obj_align( mail_main_header_label, mail_main_exit_btn, LV_ALIGN_OUT_RIGHT_MID, THEME_PADDING, 0 );
    lv_obj_t *mail_main_refresh_btn = wf_add_refresh_button( mail_main_tile, mail_main_refresh_event_cb );
    lv_obj_align( mail_main_refresh_btn, mail_main_tile, LV_ALIGN_IN_TOP_RIGHT, -THEME_PADDING, THEME_PADDING );
    lv_obj_t *mail_main_setup_btn = wf_add_setup_button( mail_main_tile, mail_main_setup_event_cb );
    lv_obj_align( mail_main_setup_btn, mail_main_refresh_btn, LV_ALIGN_OUT_LEFT_MID, -THEME_PADDING, 0 );
    /**
     * create mail main overview
     */
    mail_main_overview_page = lv_page_create( mail_main_tile, NULL);
    lv_obj_set_size( mail_main_overview_page, lv_disp_get_hor_res( NULL ), lv_disp_get_ver_res( NULL ) - THEME_ICON_SIZE );
    lv_obj_add_style( mail_main_overview_page, LV_OBJ_PART_MAIN, &mail_main_cell_style );
    lv_page_set_scrlbar_mode( mail_main_overview_page, LV_SCRLBAR_MODE_AUTO );
    lv_obj_align( mail_main_overview_page, mail_main_tile, LV_ALIGN_IN_TOP_MID, 0, THEME_ICON_SIZE );
    mail_main_overview = lv_table_create( mail_main_overview_page, NULL );
    lv_obj_add_style( mail_main_overview, LV_OBJ_PART_MAIN, &mail_main_cell_style );
    lv_page_glue_obj(mail_main_overview, true);
    mail_main_clear_overview();
}

bool mail_main_style_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_CHANGE:
            lv_style_copy( &mail_main_cell_style, APP_STYLE );
            lv_style_set_pad_all( &mail_main_cell_style, LV_TABLE_PART_BG, 0 );
            break;
    }
}

bool mail_main_button_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case BUTTON_EXIT:
            mainbar_jump_back();
            break;
        case BUTTON_REFRESH:
            mail_main_refresh();
            break;
        case BUTTON_SETUP:
            mail_main_setup();
        case BUTTON_LEFT:
            lv_page_scroll_ver(mail_main_overview_page, lv_disp_get_ver_res( NULL ) / 8 );
            break;
        case BUTTON_RIGHT:
            lv_page_scroll_ver(mail_main_overview_page, -( lv_disp_get_ver_res( NULL ) / 8 ) );
            break;
    }
    return( true );
}

static void mail_main_refresh_event_cb( lv_obj_t *obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_CLICKED:
            mail_main_add_mail_entry( "mail@foo.bar", "23:42" );
            break;
    }
}

static void mail_main_setup_event_cb( lv_obj_t *obj, lv_event_t event ) {
    switch( event ) {
        case LV_EVENT_CLICKED:
            mainbar_jump_to_tilenumber( mail_app_get_app_setup_tile_num(), LV_ANIM_OFF );
            break;
    }    
}

void mail_main_refresh( void ) {
    mail_main_clear_overview();
}

void mail_main_setup( void ) {

}

void mail_main_clear_overview( void ) {
    log_i("delete/vlear table");
    lv_table_set_col_cnt( mail_main_overview, 2 );
    lv_table_set_row_cnt( mail_main_overview, 1 );
    lv_table_set_cell_type( mail_main_overview, 0, 0, 1 );
    lv_table_set_cell_type( mail_main_overview, 0, 1, 1 );
    lv_table_set_cell_value( mail_main_overview, 0, 0, "FROM");
    lv_table_set_cell_value( mail_main_overview, 0, 1, "TIME");
    lv_table_set_col_width( mail_main_overview, 0, ( lv_page_get_width_fit(mail_main_overview_page) / 16 ) * 11 - THEME_PADDING );
    lv_table_set_col_width( mail_main_overview, 1, ( lv_page_get_width_fit(mail_main_overview_page) / 16 ) * 5 );
    lv_obj_align( mail_main_overview, mail_main_overview_page, LV_ALIGN_IN_TOP_MID, 0, 0 );

}

void mail_main_add_mail_entry( const char *from, const char *date ) {
    lv_table_set_row_cnt( mail_main_overview, lv_table_get_row_cnt( mail_main_overview ) + 1 );
    lv_table_set_cell_value( mail_main_overview, lv_table_get_row_cnt( mail_main_overview ) - 1, 0, from );
    lv_table_set_cell_value( mail_main_overview, lv_table_get_row_cnt( mail_main_overview ) - 1, 1, date );
    lv_table_set_cell_type( mail_main_overview, lv_table_get_row_cnt( mail_main_overview ) - 1, 0, 3 );
    lv_table_set_cell_type( mail_main_overview, lv_table_get_row_cnt( mail_main_overview ) - 1, 1, 3 );
}