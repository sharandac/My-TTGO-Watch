#include "config.h"
#include <TTGO.h>
#include "quickglui/quickglui.h"

#include "activity.h"
#include "gui/mainbar/mainbar.h"
#include "hardware/bma.h"

// App icon must have an size of 64x64 pixel with an alpha channel
// Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
LV_IMG_DECLARE(move_64px);
LV_FONT_DECLARE(Ubuntu_16px);

static Application activityApp;
static JsonConfig config("activity.json");

// Options
static String size, length, goal;
// Widgets
static Label lblStepcounter, lblStepachievement;

static Style text;


static void build_main_page();
static void refresh_main_page();
static void build_settings();

static void activity_activate_cb();


/*
 * setup routine for application
 */
void activity_app_setup() {
    // Create and register new application
    //   params: name, icon, auto add "refresh" button (this app will use synchronize function of the SynchronizedApplication class).
    //   Also, you can configure count of the required pages in the next two params (to have more app screens).
    activityApp.init("activity", &move_64px, 1, 1);
    
    mainbar_add_tile_activate_cb( activityApp.mainTileId(), activity_activate_cb );

    // Build and configure application
    build_main_page();
    build_settings();

    refresh_main_page();
}

void build_main_page()
{
    text = Style::Create(mainbar_get_style(), true);
    text.textFont(&Ubuntu_16px)
      .textOpacity(LV_OPA_80);

    AppPage& screen = activityApp.mainPage(); // This is parent for all main screen widgets

    lblStepcounter = Label(&screen);
    lblStepcounter.text("0")
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(text, true)
        .alignInParentTopLeft(0, 0);
    
    lblStepachievement = Label(&screen);
    lblStepachievement.text("0")
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(text, true)
        .align(lblStepcounter, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
}

void refresh_main_page()
{
    char buff[36];
    // Get current value
    uint32_t stp = bma_get_stepcounter();
    uint32_t ach = 100 * stp / goal.toInt();
    log_i("Refresh activity: %d steps", stp);
    // Raw steps
    snprintf( buff, sizeof( buff ), "Number of steps: %d", stp );
    lblStepcounter.text(buff);
    // Achievement
    snprintf( buff, sizeof( buff ), "Achievement: %d%%", ach );
    lblStepachievement.text(buff);
}

void activity_activate_cb()
{
    refresh_main_page();
}

void build_settings()
{
    // Create full options list and attach items to variables
    config.addString("Taille", 5, "170").setDigitsMode(true,"0123456789").assign(&size);
    config.addString("Longueur de pas", 3, "50").setDigitsMode(true,"0123456789").assign(&length);
    config.addString("Objectif", 7, "10000").setDigitsMode(true,"0123456789").assign(&goal);

    activityApp.useConfig(config, true); // true - auto create settings page widgets
}
