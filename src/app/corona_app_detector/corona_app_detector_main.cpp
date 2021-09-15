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

#include "corona_app_detector.h"
#include "corona_app_detector_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

lv_obj_t *corona_app_detector_main_tile = NULL;
lv_style_t corona_app_detector_main_style;

lv_task_t * _corona_app_detector_task;

LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_corona_app_detector_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_corona_app_detector_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void corona_app_detector_task( lv_task_t * task );

void corona_app_detector_main_setup( uint32_t tile_num ) {

    corona_app_detector_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &corona_app_detector_main_style, ws_get_mainbar_style() );

    lv_obj_t * exit_btn = wf_add_exit_button( corona_app_detector_main_tile, exit_corona_app_detector_main_event_cb, &corona_app_detector_main_style );
    lv_obj_align(exit_btn, corona_app_detector_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );

    lv_obj_t * setup_btn = wf_add_setup_button( corona_app_detector_main_tile, enter_corona_app_detector_setup_event_cb, &corona_app_detector_main_style );
    lv_obj_align(setup_btn, corona_app_detector_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );

    // create an task that runs every secound
    _corona_app_detector_task = lv_task_create( corona_app_detector_task, 1000, LV_TASK_PRIO_MID, NULL );
}

static void enter_corona_app_detector_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( corona_app_detector_get_main_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_corona_app_detector_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

void corona_app_detector_task( lv_task_t * task ) {
    // put your code her
}