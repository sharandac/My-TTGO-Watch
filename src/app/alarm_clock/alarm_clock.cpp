/****************************************************************************
 *   Copyright  2020  Jakub Vesely
 *   Email: jakub_vesely@seznam.cz
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
#include <TTGO.h>

#include "alarm_clock.h"
#include "alarm_clock_main.h"
#include "alarm_clock_setup.h"
#include "alarm_in_progress.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "hardware/rtcctl.h"
#include "alarm_data.h"

// declare you images or fonts you need
LV_IMG_DECLARE(alarm_clock_64px);
LV_IMG_DECLARE(alarm_clock_48px);

static uint32_t main_tile_num;
static uint32_t setup_tile_num;
static lv_obj_t *alarm_clock_widget_label;
static lv_obj_t *alarm_clock_widget_cont;

// declare callback functions
static void enter_alarm_clock_event_cb( lv_obj_t * obj, lv_event_t event );

static void alarm_term_changed_cb(EventBits_t event ){
    switch ( event ){
        case ( RTCCTL_ALARM_ENABLED ):
            lv_label_set_text( alarm_clock_widget_label, alarm_in_progress_get_clock_label());
            break;
        case ( RTCCTL_ALARM_DISABLED ):
            lv_label_set_text( alarm_clock_widget_label, "");
            break;
    }
    //content has been changed text position must be recalculated
    lv_obj_align( alarm_clock_widget_label, alarm_clock_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

// setup routine for example app
void alarm_clock_setup( void ) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    main_tile_num = mainbar_add_app_tile( 1, 2 );
    setup_tile_num = main_tile_num + 1;

    alarm_setup(); //load persistant values

    // create an app icon, label it and get the lv_obj_t icon container
    lv_obj_t * alarm_clock_icon_cont = app_tile_register_app( "alarm");
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/
    lv_obj_t * alarm_clock_icon = lv_imgbtn_create( alarm_clock_icon_cont, NULL );
    lv_imgbtn_set_src( alarm_clock_icon, LV_BTN_STATE_RELEASED, &alarm_clock_64px);
    lv_imgbtn_set_src( alarm_clock_icon, LV_BTN_STATE_PRESSED, &alarm_clock_64px);
    lv_imgbtn_set_src( alarm_clock_icon, LV_BTN_STATE_CHECKED_RELEASED, &alarm_clock_64px);
    lv_imgbtn_set_src( alarm_clock_icon, LV_BTN_STATE_CHECKED_PRESSED, &alarm_clock_64px);
    lv_obj_reset_style_list( alarm_clock_icon, LV_OBJ_PART_MAIN );
    lv_obj_align( alarm_clock_icon , alarm_clock_icon_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( alarm_clock_icon, enter_alarm_clock_event_cb );

    // make app icon drag scroll the mainbar
    mainbar_add_slide_element(alarm_clock_icon);

    // init main and setup tile, see alarm_clock_main.cpp and alarm_clock_setup.cpp
    alarm_clock_main_setup( main_tile_num );
    alarm_clock_setup_setup( setup_tile_num );
    alarm_in_progress_tile_setup();

#ifdef ALARM_CLOCK_WIDGET
    // app icon container
    // get an widget container from main_tile
    // remember, an widget icon must have an size of 64x64 pixel
    // total size of the container is 64x80 pixel, the bottom 16 pixel is for your label
    alarm_clock_widget_cont = main_tile_register_widget();
    lv_obj_t *alarm_clock_widget_icon = lv_imgbtn_create( alarm_clock_widget_cont, NULL );
    lv_imgbtn_set_src( alarm_clock_widget_icon, LV_BTN_STATE_RELEASED, &alarm_clock_64px);
    lv_imgbtn_set_src( alarm_clock_widget_icon, LV_BTN_STATE_PRESSED, &alarm_clock_64px);
    lv_imgbtn_set_src( alarm_clock_widget_icon, LV_BTN_STATE_CHECKED_RELEASED, &alarm_clock_64px);
    lv_imgbtn_set_src( alarm_clock_widget_icon, LV_BTN_STATE_CHECKED_PRESSED, &alarm_clock_64px);
    lv_obj_reset_style_list( alarm_clock_widget_icon, LV_OBJ_PART_MAIN );
    lv_obj_align( alarm_clock_widget_icon , alarm_clock_widget_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( alarm_clock_widget_icon, enter_alarm_clock_event_cb );

    // make widget icon drag scroll the mainbar
    mainbar_add_slide_element(alarm_clock_widget_icon);

    // label your widget
    alarm_clock_widget_label = lv_label_create( alarm_clock_widget_cont , NULL);
    lv_label_set_text( alarm_clock_widget_label, alarm_is_enabled() ? alarm_in_progress_get_clock_label() : "");
    lv_obj_reset_style_list( alarm_clock_widget_label, LV_OBJ_PART_MAIN );
    lv_obj_align( alarm_clock_widget_label, alarm_clock_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

    // RTCCTL_ALARM_TERM_SET doesn't have to be catched because alarm is disabled when new alarm is set. After the set it is enabled again
    rtcctl_register_cb(RTCCTL_ALARM_ENABLED | RTCCTL_ALARM_DISABLED, alarm_term_changed_cb);
#endif // ALARM_CLOCK_WIDGET
}

uint32_t alarm_clock_get_app_main_tile_num( void ) {
    return( main_tile_num );
}

uint32_t alarm_clock_get_app_setup_tile_num( void ) {
    return( setup_tile_num );
}

static void enter_alarm_clock_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
            statusbar_hide( true );
            mainbar_jump_to_tilenumber( main_tile_num, LV_ANIM_OFF );
            break;
    }
}
