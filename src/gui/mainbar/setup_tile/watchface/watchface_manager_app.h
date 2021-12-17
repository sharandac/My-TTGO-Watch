#ifndef _WATCHFACE_MANAGER_APP_H
    #define _WATCHFACE_MANAGER_APP_H

    #ifdef NATIVE_64BIT
        #include "utils/io.h"
        #include <string>

        using namespace std;
        #define String string
    #else
        #include <Arduino.h>
    #endif

    #define WATCHFACE_MANAGER_APP_INFO_LOG                  log_i
    #define WATCHFACE_MANAGER_APP_DEBUG_LOG                 log_d
    #define WATCHFACE_MANAGER_APP_ERROR_LOG                 log_e

    #define WATCHFACE_MANAGER_APP_GET_THEME_JSON_REQUEST    _BV(0)
    #define WATCHFACE_MANAGER_APP_GET_NEXT_THEME            _BV(1)
    #define WATCHFACE_MANAGER_APP_GET_PREV_THEME            _BV(2)
    #define WATCHFACE_MANAGER_APP_UPDATE_THEME_PREV         _BV(3)
    #define WATCHFACE_MANAGER_APP_DOWNLOAD_THEME            _BV(4)
    #define WATCHFACE_MANAGER_APP_INSTALL_THEME             _BV(5)
    #define WATCHFACE_MANAGER_APP_TASK_EXIT_REQUEST         _BV(8)

    /**
     * @brief watchface theme config stucture
     */
    typedef struct {
        char *watchface_theme_json_list = NULL;
        String watchface_manager_theme_name = "";
        String watchface_manager_theme_url = "";
        String watchface_manager_theme_prev_url = "";
        int32_t watchface_manager_theme_entrys = 0;
        int32_t watchface_manager_current_theme_entry = 0;
        lv_img_dsc_t watchface_theme_prev;
    } watchface_theme_t;

    /**
     * @brief setup watchface manager app
     */
    void watchface_manager_app_setup( uint32_t tile_num );

#endif // _WATCHFACE_MANAGER_APP_H