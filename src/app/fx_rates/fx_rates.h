#ifndef _fxrates_APP_H
    #define _fxrates_APP_H

    #include <TTGO.h>

    void fxrates_app_setup();

    bool fxrates_wifictl_event_cb(EventBits_t event, void *arg);
    bool fetch_fx_rates(String apiKey, String pair1, String pair2);

    void build_main_page();
    void build_settings();

#endif // _fxrates_APP_H