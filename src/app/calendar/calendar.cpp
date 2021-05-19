#include "config.h"
#include <TTGO.h>
#include "quickglui/quickglui.h"

#include "calendar.h"
#include "gui/mainbar/mainbar.h"
#include "gui/widget_styles.h"
#include "hardware/motor.h"

// App icon must have an size of 64x64 pixel with an alpha channel
// Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
LV_IMG_DECLARE(calendar_64px);

static Application calendarApp;

static lv_obj_t * calendar;

static void build_main_page();
static void refresh_main_page();

static void calendar_activate_cb();

/*
 * setup routine for application
 */
void calendar_app_setup() {
    // Create and register new application
    //   params: name, icon, auto add "refresh" button (this app will use synchronize function of the SynchronizedApplication class).
    //   Also, you can configure count of the required pages in the next two params (to have more app screens).
    calendarApp.init("calendar", &calendar_64px, 1, 1);

    mainbar_add_tile_activate_cb( calendarApp.mainTileId(), calendar_activate_cb );

    // Build and configure application
    build_main_page();

    refresh_main_page();
}

static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if (event == LV_EVENT_LONG_PRESSED) {
        mainbar_jump_back();
    }
}
void build_main_page()
{
    AppPage& screen = calendarApp.mainPage(); // This is parent for all main screen widgets

    calendar = lv_calendar_create(screen.handle(), NULL);
    lv_obj_set_size(calendar, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(calendar, screen.handle(), LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_event_cb(calendar, event_handler);

    /*Make the date number smaller to be sure they fit into their area*/
    lv_obj_set_style_local_text_font(calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, lv_theme_get_font_small());
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
