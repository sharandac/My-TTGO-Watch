#ifndef _STATUSBAR_H

    #define _STATUSBAR_H

    #include "config.h"

    #define STATUSBAR_HEIGHT            26
    #define STATUSBAR_EXPAND_HEIGHT     128

    typedef struct {
        lv_obj_t *icon;
        const void *symbol;
        lv_align_t align;
        lv_style_t *style;
    } lv_status_bar_t;

    typedef enum {
        STATUSBAR_BATTERY_PERCENT,
        STATUSBAR_BATTERY,
        STATUSBAR_WIFI,
        STATUSBAR_BELL,
        STATUSBAR_WARNING,
        STATUSBAR_NUM
    } statusbar_icon_t;
    
    typedef enum {
        STATUSBAR_STYLE_NORMAL,
        STATUSBAR_STYLE_WHITE,
        STATUSBAR_STYLE_RED,
        STATUSBAR_STYLE_GRAY,
        STATUSBAR_STYLE_YELLOW,
        STATUSBAR_STYLE_GREEN,
        STATUSBAR_STYLE_BLUE,
        STATUSBAR_STYLE_NUM
    } statusbar_style_t;

    /*
     * @brief setup statusbar
     */
    void statusbar_setup( void );
    /*
     * @brief hide an icon from statusbar
     * 
     * @param icon  icon name
     */
    void statusbar_hide_icon( statusbar_icon_t icon );
    /*
     * @brief show an icon from statusbar
     * 
     * @param icon  icon name
     */
    void statusbar_show_icon( statusbar_icon_t icon );
    /*
     * @brief hide an icon from statusbar
     * 
     * @param icon  icon name
     * @param style style name
     */
    void statusbar_style_icon( statusbar_icon_t icon, statusbar_style_t style );
    void statusbar_refresh( void );
    void statusbar_update_stepcounter( int step );
    void statusbar_update_battery( int32_t percent, bool charging, bool plug );
    void statusbar_wifi_set_state( bool state, const char *wifiname );
    void statusbar_bluetooth_set_state( bool state );

#endif // _STATUSBAR_H

