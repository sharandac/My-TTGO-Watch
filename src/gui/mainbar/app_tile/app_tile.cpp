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

#include "gui/mainbar/mainbar.h"
#include "app_tile.h"

lv_app_entry_t app_entry[ MAX_APPS_ICON ];

static lv_obj_t *app_cont = NULL;
static lv_obj_t *app_label = NULL;
static uint32_t app_tile_num;
static lv_style_t *style;
static lv_style_t appstyle;

LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(Ubuntu_16px);

void app_tile_setup( void ) {
    app_tile_num = mainbar_add_tile( 1, 0 );
    app_cont = mainbar_get_tile_obj( app_tile_num );
    style = mainbar_get_style();

    lv_style_copy( &appstyle, style);
    lv_style_set_text_opa( &appstyle, LV_OBJ_PART_MAIN, LV_OPA_30);
    lv_style_set_text_font( &appstyle, LV_STATE_DEFAULT, &Ubuntu_72px);

    app_label = lv_label_create( app_cont, NULL);
    lv_label_set_text( app_label, "apps");
    lv_obj_reset_style_list( app_label, LV_OBJ_PART_MAIN );
    lv_obj_add_style( app_label, LV_OBJ_PART_MAIN, &appstyle );
    lv_obj_align( app_label, NULL, LV_ALIGN_CENTER, 0, 0);

    for ( int app = 0 ; app < MAX_APPS_ICON ; app++ ) {
        // set x, y and mark it as inactive
        app_entry[ app ].x = APP_FIRST_X_POS + ( ( app % MAX_APPS_ICON_HORZ ) * ( APP_ICON_X_SIZE + APP_ICON_X_CLEARENCE ) );
        app_entry[ app ].y = APP_FIRST_Y_POS + ( ( app / MAX_APPS_ICON_HORZ ) * ( APP_ICON_Y_SIZE + APP_ICON_Y_CLEARENCE ) );
        app_entry[ app ].active = false;
        // create app icon container
        app_entry[ app ].app = lv_obj_create( app_cont, NULL );
        lv_obj_reset_style_list( app_entry[ app ].app, LV_OBJ_PART_MAIN );
        lv_obj_add_style( app_entry[ app ].app, LV_OBJ_PART_MAIN, style );
        lv_obj_set_size( app_entry[ app ].app, APP_ICON_X_SIZE, APP_ICON_Y_SIZE );
        lv_obj_align( app_entry[ app ].app , app_cont, LV_ALIGN_IN_TOP_LEFT, app_entry[ app ].x, app_entry[ app ].y );
        // create app label
        app_entry[ app ].label = lv_label_create( app_cont, NULL );
        lv_obj_reset_style_list( app_entry[ app ].label, LV_OBJ_PART_MAIN );
        lv_obj_add_style( app_entry[ app ].label, LV_OBJ_PART_MAIN, style );
        lv_obj_set_size( app_entry[ app ].label, APP_LABEL_X_SIZE, APP_LABEL_Y_SIZE );
        lv_obj_align( app_entry[ app ].label , app_entry[ app ].app, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
        lv_obj_set_hidden( app_entry[ app ].app, true );
        lv_obj_set_hidden( app_entry[ app ].label, true );
    }
}

lv_obj_t *app_tile_register_app( const char* appname ) {
    for( int app = 0 ; app < MAX_APPS_ICON ; app++ ) {
        if ( app_entry[ app ].active == false ) {
            app_entry[ app ].active = true;
            lv_label_set_text( app_entry[ app ].label, appname );
            lv_obj_align( app_entry[ app ].label , app_entry[ app ].app, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
            lv_obj_set_hidden( app_entry[ app ].app, false );
            lv_obj_set_hidden( app_entry[ app ].label, false );
            return( app_entry[ app ].app );
        }
    }
    log_e("no space for an app icon");
    return( NULL );
}

uint32_t app_tile_get_tile_num( void ) {
    return( app_tile_num );
}
