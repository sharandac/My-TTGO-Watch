#include "config.h"
#include <TTGO.h>
#include "quickglui/quickglui.h"

#include "esp3d.h"
#include "gui/mainbar/mainbar.h"
#include "hardware/wifictl.h"




// App icon must have an size of 64x64 pixel with an alpha channel *******************
// Use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha"
LV_IMG_DECLARE(esp3d_64px); 
LV_FONT_DECLARE(Ubuntu_48px);

SynchronizedApplication esp3dApp;
JsonConfig espconfig("esp3d.json");

String esp3dServer, espData1, espData2;
String ESPmainPairValue, ESPsecondPairValue, espupdatedAt;
Label esp3dCurrentStatus, esp3dStatus, lblEspUpdatedAt;

Style Ebig;

/*
 * setup routine for application
 */
void esp3d_app_setup() {
    // Create and register new application
    //   params: name, icon, auto add "refresh" button (this app will use synchronize function of the SynchronizedApplication class).
    //   Also, you can configure count of the required pages in the next two params (to have more app screens).
    esp3dApp.init("esp3d", &esp3d_64px, true, 1, 1);
    
    // Build and configure application
    build_main_esp_page();
    build_esp_settings();

    // Executed when user click "refresh" button or when a WiFi connection is established
    esp3dApp.synchronizeActionHandler([](SyncRequestSource source) {
        auto result = fetch_esp3d_data(esp3dServer, espData1, espData2);//
        lblEspUpdatedAt.text(espupdatedAt);
        if (result)
        {
            esp3dApp.icon().widgetText(ESPmainPairValue);
            esp3dCurrentStatus.text(ESPmainPairValue).alignInParentCenter(0, -30); //display data on widget
            esp3dStatus.text(ESPsecondPairValue).alignOutsideBottomMid(esp3dCurrentStatus);
            esp3dApp.icon().showIndicator(ICON_INDICATOR_OK);
        } else {
            // In case of fail
            esp3dApp.icon().showIndicator(ICON_INDICATOR_FAIL);
        }
    });
    
    // We want to start syncronization every time when WiFi connection is established. So we will listen system events:
    wifictl_register_cb(WIFICTL_CONNECT | WIFICTL_OFF, esp3d_wifictl_event_cb, "esp3d app widget");
}

bool esp3d_wifictl_event_cb(EventBits_t event, void *arg) {
    switch(event) {
        case WIFICTL_CONNECT:
            esp3dApp.icon().hideIndicator();
            if ( espconfig.getBoolean("autosync", false ) )
                esp3dApp.startSynchronization(SyncRequestSource::ConnectionEvent);
            break;

        case WIFICTL_OFF:
            esp3dApp.icon().hideIndicator();
            break;
    }
    return true;
}

void build_main_esp_page()
{
    Ebig = Style::Create(mainbar_get_style(), true);
    Ebig.textFont(&Ubuntu_48px)
      .textOpacity(LV_OPA_80);

    AppPage& screen = esp3dApp.mainPage(); // This is parent for all main screen widgets

    esp3dCurrentStatus = Label(&screen);
    esp3dCurrentStatus.text(espData1)
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(Ebig, true)
        .alignInParentCenter(0, -30);

    esp3dStatus = Label(&screen);
    esp3dStatus.text(espData2)
        .alignText(LV_LABEL_ALIGN_CENTER)
        .style(Ebig, true)
        .alignOutsideBottomMid(esp3dCurrentStatus);

    lblEspUpdatedAt = Label(&screen);
    lblEspUpdatedAt.text("loading...")
        .alignText(LV_LABEL_ALIGN_LEFT)
        .alignInParentTopLeft(5, 5);
}

void build_esp_settings()
{
    // Create full options list and attach items to variables
    espconfig.addString("espServer", 32).assign(&esp3dServer);
    espconfig.addString("esppair1", 12, "EUR_USD").assign(&espData1);
    espconfig.addString("esppair2", 12).assign(&espData2);
    espconfig.addBoolean("autosync", false);
    espconfig.addBoolean("widget", false);

    // Switch desktop widget state based on the cuurent settings when changed
    espconfig.onLoadSaveHandler([](JsonConfig& cfg) {
        bool widgetEnabled = cfg.getBoolean("widget"); // Is app widget enabled?
        if (widgetEnabled)
            esp3dApp.icon().registerDesktopWidget("esp3d", &esp3d_64px);
        else
            esp3dApp.icon().unregisterDesktopWidget();
    });

    esp3dApp.useConfig(espconfig, true); // true - auto create settings page widgets
}

bool fetch_esp3d_data(String esp3dServer, String esppair1, String esppair2) {
    char url[256]=""; float p1=0, p2=0;
    snprintf(url, sizeof(url), "http://192.168.1.215", esp3dServer.c_str(), esppair1.c_str(), esppair2.c_str());
    if (esppair2.length() == 0) // If single currency used - remove ',' char
        url[strlen(url)-1]='\0';

    JsonRequest request(320);
    if (!request.process(url)) {
        espupdatedAt = request.errorString();
        return false;
    }

    ESPmainPairValue = ESPsecondPairValue = "";
    p1 = request[espData1].as<float>();
    ESPmainPairValue = String(p1, 2);
    if (request.size() > 1) { // Second currency pair available
        p2 = request[espData2].as<float>();
        ESPsecondPairValue = String(p2, 2);
    }
    espupdatedAt = request.formatCompletedAt("Upd: %d.%m %H:%M.%S");
    //log_i("fx rates: %d = %f, %f", doc.size(), p1, p2);

    return true;
}
