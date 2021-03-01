#ifndef _esp3d_APP_H
    #define _esp3d_APP_H

    #include <TTGO.h>

    void esp3d_app_setup();

    bool esp3d_wifictl_event_cb(EventBits_t event, void *arg);
    bool fetch_esp3d_data(String esp3dServer, String pair1, String pair2);

    void build_main_esp_page();//might neeed to edit this and the next too
    void build_esp_settings();

#endif // _esp3d_APP_H