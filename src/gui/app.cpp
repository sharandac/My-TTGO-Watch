/****************************************************************************
 *   Sep 2 08:21:51 2020
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
#include "gui/mainbar/app_tile/app_tile.h"

#include "app.h"

LV_FONT_DECLARE(Ubuntu_72px);
LV_FONT_DECLARE(Ubuntu_16px);
LV_IMG_DECLARE(info_ok_16px);
LV_IMG_DECLARE(info_fail_16px);
LV_IMG_DECLARE(info_update_16px);
LV_IMG_DECLARE(info_1_16px);
LV_IMG_DECLARE(info_2_16px);
LV_IMG_DECLARE(info_3_16px);
LV_IMG_DECLARE(info_n_16px);

app_icon_t *app_register( const char* appname, const lv_img_dsc_t *icon, lv_event_cb_t event_cb ) {

    app_icon_t *app = app_tile_get_free_app_icon();

    if ( app == NULL ) {
        log_e("no free app icon");
        return( NULL );
    }

    app->active = true;
    // setup label
    lv_label_set_text( app->label, appname );
    lv_obj_align( app->label , app->app_icon_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0 );
    lv_obj_set_hidden( app->app_icon_cont, false );
    lv_obj_set_hidden( app->label, false );
    // setup icon and set event callback
    app->app_icon_img = lv_imgbtn_create( app->app_icon_cont , NULL );
    lv_imgbtn_set_src( app->app_icon_img, LV_BTN_STATE_RELEASED, icon);
    lv_imgbtn_set_src( app->app_icon_img, LV_BTN_STATE_PRESSED, icon);
    lv_imgbtn_set_src( app->app_icon_img, LV_BTN_STATE_CHECKED_RELEASED, icon);
    lv_imgbtn_set_src( app->app_icon_img, LV_BTN_STATE_CHECKED_PRESSED, icon);
    lv_obj_reset_style_list( app->app_icon_img, LV_OBJ_PART_MAIN );
    lv_obj_align( app->app_icon_img , app->app_icon_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( app->app_icon_img, event_cb );
    // setup icon indicator
    app->app_icon_indicator = lv_img_create( app->app_icon_cont, NULL );
    lv_img_set_src( app->app_icon_indicator, &info_ok_16px );
    lv_obj_align( app->app_icon_indicator, app->app_icon_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( app->app_icon_indicator, true );
    mainbar_add_slide_element( app->app_icon_img );
    
    lv_obj_invalidate( lv_scr_act() );

    return( app );
}

void app_set_indicator( app_icon_t *app, app_icon_indicator_t indicator ) {
    if ( app->active == false || app == NULL ) {
        return;
    }

    switch( indicator ) {
        case APP_ICON_INDICATOR_OK:     lv_img_set_src( app->app_icon_indicator, &info_ok_16px );
                                        break;
        case APP_ICON_INDICATOR_FAIL:   lv_img_set_src( app->app_icon_indicator, &info_fail_16px );
                                        break;
        case APP_ICON_INDICATOR_UPDATE: lv_img_set_src( app->app_icon_indicator, &info_update_16px );
                                        break;
        case APP_ICON_INDICATOR_1:      lv_img_set_src( app->app_icon_indicator, &info_1_16px );
                                        break;
        case APP_ICON_INDICATOR_2:      lv_img_set_src( app->app_icon_indicator, &info_2_16px );
                                        break;
        case APP_ICON_INDICATOR_3:      lv_img_set_src( app->app_icon_indicator, &info_3_16px );
                                        break;
        case APP_ICON_INDICATOR_N:      lv_img_set_src( app->app_icon_indicator, &info_n_16px );
                                        break;
    }
    lv_obj_align( app->app_icon_indicator, app->app_icon_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( app->app_icon_indicator, false );
    lv_obj_invalidate( lv_scr_act() );
}

void app_hide_indicator( app_icon_t *app ) {
    if ( app->active == false || app == NULL ) {
        return;
    }

    lv_obj_set_hidden( app->app_icon_indicator, true );
    lv_obj_invalidate( lv_scr_act() );
}

void app_set_icon( app_icon_t *app, lv_obj_t *icon ) {
    if ( app->active == false || app == NULL ) {
        return;
    }

    lv_imgbtn_set_src( app->app_icon_img, LV_BTN_STATE_RELEASED, icon);
    lv_imgbtn_set_src( app->app_icon_img, LV_BTN_STATE_PRESSED, icon);
    lv_imgbtn_set_src( app->app_icon_img, LV_BTN_STATE_CHECKED_RELEASED, icon);
    lv_imgbtn_set_src( app->app_icon_img, LV_BTN_STATE_CHECKED_PRESSED, icon);
    lv_obj_reset_style_list( app->app_icon_img, LV_OBJ_PART_MAIN );
    lv_obj_align( app->app_icon_img , app->app_icon_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_invalidate( lv_scr_act() );
}