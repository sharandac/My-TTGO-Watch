/****************************************************************************
 *   Aug 21 17:26:00 2020
 *   Copyright  2020  Chris McNamee
 *   Email: chris.mcna@gmail.com
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

#include "stopwatch_app.h"
#include "stopwatch_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

uint32_t stopwatch_app_main_tile_num;
uint32_t stopwatch_app_setup_tile_num;

// app icon container
lv_obj_t *stopwatch_app_icon_cont = NULL;
lv_obj_t *stopwatch_app_icon = NULL;
lv_obj_t *stopwatch_app_icon_info = NULL;

// widget icon container
lv_obj_t *stopwatch_app_widget_cont = NULL;
lv_obj_t *stopwatch_app_widget_icon = NULL;
lv_obj_t *stopwatch_app_widget_icon_info = NULL;
lv_obj_t *stopwatch_app_widget_label = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(stopwatch_app_64px);
LV_IMG_DECLARE(stopwatch_app_48px);
LV_IMG_DECLARE(info_1_16px);
// declare callback functions
static void enter_stopwatch_app_event_cb( lv_obj_t * obj, lv_event_t event );

// setup routine for stopwatch app
void stopwatch_app_setup( void ) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    stopwatch_app_main_tile_num = mainbar_add_app_tile( 1, 2 );
    stopwatch_app_setup_tile_num = stopwatch_app_main_tile_num + 1;

    // create an app icon, label it and get the lv_obj_t icon container
    stopwatch_app_icon_cont = app_tile_register_app( "stop\nwatch");
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/stopwatchs/images/
    stopwatch_app_icon = lv_imgbtn_create( stopwatch_app_icon_cont, NULL );
    lv_imgbtn_set_src( stopwatch_app_icon, LV_BTN_STATE_RELEASED, &stopwatch_app_64px);
    lv_imgbtn_set_src( stopwatch_app_icon, LV_BTN_STATE_PRESSED, &stopwatch_app_64px);
    lv_imgbtn_set_src( stopwatch_app_icon, LV_BTN_STATE_CHECKED_RELEASED, &stopwatch_app_64px);
    lv_imgbtn_set_src( stopwatch_app_icon, LV_BTN_STATE_CHECKED_PRESSED, &stopwatch_app_64px);
    lv_obj_reset_style_list( stopwatch_app_icon, LV_OBJ_PART_MAIN );
    lv_obj_align( stopwatch_app_icon , stopwatch_app_icon_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( stopwatch_app_icon, enter_stopwatch_app_event_cb );

    // make app icon drag scroll the mainbar
    mainbar_add_slide_element(stopwatch_app_icon);

    // set an small info indicator at your app icon to inform the user about the state or news
    stopwatch_app_icon_info = lv_img_create( stopwatch_app_icon_cont, NULL );
    lv_img_set_src( stopwatch_app_icon_info, &info_1_16px );
    lv_obj_align( stopwatch_app_icon_info, stopwatch_app_icon_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( stopwatch_app_icon_info, true );

    // init main and setup tile, see stopwatch_app_main.cpp and stopwatch_app_setup.cpp
    stopwatch_app_main_setup( stopwatch_app_main_tile_num );

#ifdef stopwatch_WIDGET
    // get an widget container from main_tile
    // remember, an widget icon must have an size of 64x64 pixel
    // total size of the container is 64x80 pixel, the bottom 16 pixel is for your label
    stopwatch_app_widget_cont = main_tile_register_widget();
    stopwatch_app_widget_icon = lv_imgbtn_create( stopwatch_app_widget_cont, NULL );
    lv_imgbtn_set_src( stopwatch_app_widget_icon, LV_BTN_STATE_RELEASED, &stopwatch_app_48px);
    lv_imgbtn_set_src( stopwatch_app_widget_icon, LV_BTN_STATE_PRESSED, &stopwatch_app_48px);
    lv_imgbtn_set_src( stopwatch_app_widget_icon, LV_BTN_STATE_CHECKED_RELEASED, &stopwatch_app_48px);
    lv_imgbtn_set_src( stopwatch_app_widget_icon, LV_BTN_STATE_CHECKED_PRESSED, &stopwatch_app_48px);
    lv_obj_reset_style_list( stopwatch_app_widget_icon, LV_OBJ_PART_MAIN );
    lv_obj_align( stopwatch_app_widget_icon , stopwatch_app_widget_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( stopwatch_app_widget_icon, enter_stopwatch_app_event_cb );

    // make widget icon drag scroll the mainbar
    mainbar_add_slide_element(stopwatch_app_widget_icon);

    // set an small info icon at your widget icon to inform the user about the state or news
    stopwatch_app_widget_icon_info = lv_img_create( stopwatch_app_widget_cont, NULL );
    lv_img_set_src( stopwatch_app_widget_icon_info, &info_1_16px );
    lv_obj_align( stopwatch_app_widget_icon_info, stopwatch_app_widget_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( stopwatch_app_widget_icon_info, false );

    // label your widget
    stopwatch_app_widget_label = lv_label_create( stopwatch_app_widget_cont , NULL);
    lv_label_set_text( stopwatch_app_widget_label, "stopwatch");
    lv_obj_reset_style_list( stopwatch_app_widget_label, LV_OBJ_PART_MAIN );
    lv_obj_align( stopwatch_app_widget_label, stopwatch_app_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
#endif // stopwatch_WIDGET
}

uint32_t stopwatch_app_get_app_main_tile_num( void ) {
    return( stopwatch_app_main_tile_num );
}

uint32_t stopwatch_app_get_app_setup_tile_num( void ) {
    return( stopwatch_app_setup_tile_num );
}

/*
 *
 */
static void enter_stopwatch_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        stopwatch_app_hide_app_icon_info( true );
                                        #ifdef stopwatch_WIDGET
                                            stopwatch_app_hide_widget_icon_info( true );
                                        #endif
                                        mainbar_jump_to_tilenumber( stopwatch_app_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

/*
 *
 */
void stopwatch_app_hide_app_icon_info( bool show ) {
    if ( stopwatch_app_icon_info == NULL )
        return;
    
    lv_obj_set_hidden( stopwatch_app_icon_info, show );
    lv_obj_invalidate( lv_scr_act() );
}

/*
 *
 */
void stopwatch_app_hide_widget_icon_info( bool show ) {
    if ( stopwatch_app_widget_icon_info == NULL )
        return;

    lv_obj_set_hidden( stopwatch_app_widget_icon_info, show );
    lv_obj_invalidate( lv_scr_act() );
}