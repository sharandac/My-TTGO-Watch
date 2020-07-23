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
#include <stdio.h>
#include <time.h>

#include "config.h"
#include "../mainbar.h"
#include "main_tile.h"

static lv_obj_t *main_cont = NULL;
static lv_obj_t *clock_cont = NULL;
static lv_obj_t *timelabel = NULL;
static lv_obj_t *datelabel = NULL;

static lv_style_t timestyle;
static lv_style_t datestyle;

lv_widget_entry_t widget_entry[ MAX_WIDGET_NUM ];

lv_task_t * task;

LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(Ubuntu_16px);

void main_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres ) {

    main_cont = tile;

    lv_style_copy( &timestyle, style);
    lv_style_set_text_font( &timestyle, LV_STATE_DEFAULT, &Ubuntu_72px);

    lv_style_copy( &datestyle, style);
    lv_style_set_text_font( &datestyle, LV_STATE_DEFAULT, &Ubuntu_16px);

    clock_cont = lv_obj_create( tile, NULL );
    lv_obj_set_size( clock_cont, hres , hres/2 );
    lv_obj_add_style( clock_cont, LV_OBJ_PART_MAIN, style );
    lv_obj_align( clock_cont, tile, LV_ALIGN_CENTER, 0, 0 );

    timelabel = lv_label_create( clock_cont , NULL);
    lv_label_set_text(timelabel, "00:00");
    lv_obj_reset_style_list( timelabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( timelabel, LV_OBJ_PART_MAIN, &timestyle );
    lv_obj_align(timelabel, NULL, LV_ALIGN_CENTER, 0, 0);

    datelabel = lv_label_create( clock_cont , NULL);
    lv_label_set_text(datelabel, "1.Jan 1970");
    lv_obj_reset_style_list( datelabel, LV_OBJ_PART_MAIN );
    lv_obj_add_style( datelabel, LV_OBJ_PART_MAIN, &datestyle );
    lv_obj_align(datelabel, timelabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    time_t now;
    struct tm  info;
    char buf[64];

    time( &now );
    localtime_r( &now, &info );

    strftime( buf, sizeof(buf), "%H:%M", &info );
    lv_label_set_text( timelabel, buf );
    strftime( buf, sizeof(buf), "%a %d.%b %Y", &info );
    lv_label_set_text( datelabel, buf );
    lv_obj_align( datelabel, timelabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );

    for ( int widget = 0 ; widget < MAX_WIDGET_NUM ; widget++ ) {
        widget_entry[ widget ].widget = lv_obj_create( main_cont, NULL );
        widget_entry[ widget ].active = false;
        widget_entry[ widget ].x = 0;
        widget_entry[ widget ].y = 0;
        lv_obj_reset_style_list( widget_entry[ widget ].widget, LV_OBJ_PART_MAIN );
        lv_obj_add_style( widget_entry[ widget ].widget, LV_OBJ_PART_MAIN, style );
        lv_obj_set_size( widget_entry[ widget ].widget, 64, 80 );
        lv_obj_set_hidden( widget_entry[ widget ].widget, true );
    }

    task = lv_task_create( main_tile_task, 1000, LV_TASK_PRIO_MID, NULL );
}

lv_obj_t *main_tile_register_widget( void ) {
    for( int widget = 0 ; widget < MAX_WIDGET_NUM ; widget++ ) {
        if ( widget_entry[ widget ].active == false ) {
            widget_entry[ widget ].active = true;
            lv_obj_align( clock_cont, main_cont, LV_ALIGN_IN_TOP_MID, 0, 0 );
            lv_obj_align( widget_entry[ widget ].widget , main_cont, LV_ALIGN_IN_BOTTOM_MID, 0, -32 );
            lv_obj_set_hidden( widget_entry[ widget ].widget, false );
            return( widget_entry[ widget ].widget );
        }
    }
    return( NULL );
}

/*
 *
 */
void main_tile_task( lv_task_t * task ) {
    time_t now;
    struct tm  info;
    char buf[64];

    time( &now );
    localtime_r( &now, &info );

    strftime( buf, sizeof(buf), "%H:%M", &info );
    lv_label_set_text( timelabel, buf );
    strftime( buf, sizeof(buf), "%a %d.%b %Y", &info );
    lv_label_set_text( datelabel, buf );
    lv_obj_align( datelabel, timelabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
}