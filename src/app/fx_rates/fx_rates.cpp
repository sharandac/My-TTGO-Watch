#include "config.h"

#include "quickglui/quickglui.h"
#include "fx_rates.h"
#include "gui/mainbar/mainbar.h"
#include "gui/widget_styles.h"
#include "hardware/wifictl.h"

// App icon must have an size of 64x64 pixel with an alpha channel
// Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
LV_IMG_DECLARE(fx_rates_64px);
LV_FONT_DECLARE(Ubuntu_48px);

static SynchronizedApplication fxratesApp;
static JsonConfig config("fx-rates.json");

static String apiKey, mainPair, secondPair;
static String mainPairValue, secondPairValue, updatedAt;
static Label lblCurrency1, lblCurrency2, lblUpdatedAt;

static Style big;

/*
 * setup routine for application
 */
void fxrates_app_setup() {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif
    // Create and register new application
    //   params: name, icon, auto add "refresh" button (this app will use synchronize function of the SynchronizedApplication class).
    //   Also, you can configure count of the required pages in the next two params (to have more app screens).
    fxratesApp.init("forex rates", &fx_rates_64px, true, 1, 1);
    
    // Build and configure application
    build_main_page();
    build_settings();

    // Executed when user click "refresh" button or when a WiFi connection is established
    fxratesApp.synchronizeActionHandler([](SyncRequestSource source) {
        auto result = fetch_fx_rates(apiKey, mainPair, secondPair);
        lblUpdatedAt.text(updatedAt);
        if (result)
        {
            fxratesApp.icon().widgetText(mainPairValue);
            lblCurrency1.text(mainPairValue).alignInParentCenter(0, -30);
            lblCurrency2.text(secondPairValue).alignOutsideBottomMid(lblCurrency1);
            fxratesApp.icon().showIndicator(ICON_INDICATOR_OK);
        } else {
            // In case of fail
            fxratesApp.icon().showIndicator(ICON_INDICATOR_FAIL);
        }
    });
    
    // We want to start syncronization every time when WiFi connection is established. So we will listen system events:
    wifictl_register_cb(WIFICTL_CONNECT | WIFICTL_OFF, fxrates_wifictl_event_cb, "fxrates app widget");
}

bool fxrates_wifictl_event_cb(EventBits_t event, void *arg) {
    switch(event) {
        case WIFICTL_CONNECT:
            fxratesApp.icon().hideIndicator();
            if ( config.getBoolean("autosync", false ) )
                fxratesApp.startSynchronization(SyncRequestSource::ConnectionEvent);
            break;

        case WIFICTL_OFF:
            fxratesApp.icon().hideIndicator();
            break;
    }
    return true;
}

void build_main_page()
{
    big = Style::Create(ws_get_mainbar_style(), true);
    big.textFont(&Ubuntu_48px)
      .textOpacity(LV_OPA_80);

    AppPage& screen = fxratesApp.mainPage(); // This is parent for all main screen widgets

    lblCurrency1 = Label(&screen);
    lblCurrency1.text(mainPair)
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(big, true)
        .alignInParentCenter(0, -30);

    lblCurrency2 = Label(&screen);
    lblCurrency2.text(secondPairValue)
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(big, true)
        .alignOutsideBottomMid(lblCurrency1);

    lblUpdatedAt = Label(&screen);
    lblUpdatedAt.text("loading...")
        .alignText(LV_LABEL_ALIGN_LEFT)
        .alignInParentTopLeft(5, 5);
}

void build_settings()
{
    // Create full options list and attach items to variables
    config.addString("apikey", 32).assign(&apiKey);
    config.addString("pair1", 12, "EUR_USD").assign(&mainPair);
    config.addString("pair2", 12).assign(&secondPair);
    config.addBoolean("autosync", false);
    config.addBoolean("widget", false);

    // Switch desktop widget state based on the cuurent settings when changed
    config.onLoadSaveHandler([](JsonConfig& cfg) {
        bool widgetEnabled = cfg.getBoolean("widget"); // Is app widget enabled?
        if (widgetEnabled)
            fxratesApp.icon().registerDesktopWidget("fx", &fx_rates_64px);
        else
            fxratesApp.icon().unregisterDesktopWidget();
    });

    fxratesApp.useConfig(config, true); // true - auto create settings page widgets
}

bool fetch_fx_rates(String apiKey, String pair1, String pair2) {
    char url[256]=""; float p1=0, p2=0;
    snprintf(url, sizeof(url), "http://free.currconv.com/api/v7/convert?apiKey=%s&compact=ultra&q=%s,%s", apiKey.c_str(), pair1.c_str(), pair2.c_str());
    if (pair2.length() == 0) // If single currency used - remove ',' char
        url[strlen(url)-1]='\0';

    JsonRequest request(320);
    if (!request.process(url)) {
        updatedAt = request.errorString();
        return false;
    }

    mainPairValue = secondPairValue = "";
    p1 = request[mainPair].as<float>();
    mainPairValue = String(p1, 2);
    if (request.size() > 1) { // Second currency pair available
        p2 = request[secondPair].as<float>();
        secondPairValue = String(p2, 2);
    }
    updatedAt = request.formatCompletedAt("Upd: %d.%m %H:%M.%S");
    //log_i("fx rates: %d = %f, %f", doc.size(), p1, p2);

    return true;
}
