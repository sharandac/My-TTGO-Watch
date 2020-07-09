#ifndef _WLAN_SETTINGS_H
    #define _WLAN_SETTINGS_H

    #include <TTGO.h>

    void wlan_settings_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );
    void wlan_password_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );

#endif // _WLAN_SETTINGS_H