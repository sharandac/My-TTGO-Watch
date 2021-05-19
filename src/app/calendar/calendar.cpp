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
#include "gui/mainbar/mainbar.h"
#include "gui/icon.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

uint32_t calendar_main_tile_num;

// app icon
icon_t *calendar_icon = NULL;

// App icon must have an size of 64x64 pixel with an alpha channel
// Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
LV_IMG_DECLARE(calendar_64px);


static lv_obj_t * calendar;
static lv_style_t exit_style;

static void enter_calendar_event_cb( lv_obj_t * obj, lv_event_t event );
static void event_handler(lv_obj_t * obj, lv_event_t event);
static void build_main_page();
static void refresh_main_page();

static void calendar_activate_cb();

/*
 * setup routine for application
 */
void calendar_app_setup( void ) {
    calendar_main_tile_num = mainbar_add_app_tile( 1, 1, "Calendar" );
    mainbar_add_tile_activate_cb( calendar_main_tile_num, calendar_activate_cb );

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/ and declare it like LV_IMG_DECLARE( your_icon );
    calendar_icon = app_register( "Calendar", &calendar_64px, enter_calendar_event_cb );
    app_set_indicator( calendar_icon, ICON_INDICATOR_OK );

    // Build and configure application
    build_main_page();

    refresh_main_page();
}

static void enter_calendar_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):
               mainbar_jump_to_tilenumber( calendar_main_tile_num, LV_ANIM_OFF );
               statusbar_hide( true );
               app_hide_indicator( calendar_icon );
               break;
    }
}

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if ( event == LV_EVENT_CLICKED ) {
        mainbar_jump_back();
    }
}

void build_main_page()
{
    lv_obj_t *main_tile = mainbar_get_tile_obj( calendar_main_tile_num );

    calendar = lv_calendar_create(main_tile, NULL);
    lv_obj_set_size(calendar, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(calendar, main_tile, LV_ALIGN_IN_TOP_MID, 0, 0);

    /*Make the date number smaller to be sure they fit into their area*/
    lv_obj_set_style_local_text_font(calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, lv_theme_get_font_small());

    lv_style_copy( &exit_style, ws_get_mainbar_style() );
    lv_style_set_bg_color( &exit_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &exit_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    //lv_style_set_border_width( &exit_style, LV_OBJ_PART_MAIN, 0);

    lv_obj_t *exit_button = wf_add_exit_button(main_tile, event_handler, &exit_style);
    lv_obj_align( exit_button, main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
}

void refresh_main_page()
{
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

    lv_calendar_set_today_date(calendar, &today);
    lv_calendar_set_showed_date(calendar, &today);
}

void calendar_activate_cb()
{
    refresh_main_page();
}
