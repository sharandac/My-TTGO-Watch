/****************************************************************************
              config.h
    Tu May 22 21:23:51 2020
    Copyright  2020  Dirk Brosswick
 *  Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
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
#ifndef _CONFIG_H

    #include "lvgl.h"

    #ifdef NATIVE_64BIT
            #define RES_X_MAX       LV_HOR_RES_MAX
            #define RES_Y_MAX       LV_VER_RES_MAX
            #define HARDWARE_NAME   "NATIVE 64BIT APP"
    #else
        #if defined( LILYGO_WATCH_2020_V1 )
            #undef LILYGO_WATCH_LVGL
            #define HARDWARE_NAME   "T-Watch2020V1"
            #define RES_X_MAX       240
            #define RES_Y_MAX       240
            #define USE_PSRAM_ALLOC_LVGL                    /** @brief enabled LVGL to use PSRAM */ 
            #define ENABLE_WEBSERVER                        /** @brief To disable built-in webserver, comment this line */
            #define ENABLE_FTPSERVER                        /** @brief To disable built-in ftpserver, comment this line */
        #elif defined( LILYGO_WATCH_2020_V2 )
            #undef LILYGO_WATCH_LVGL
            #define HARDWARE_NAME   "T-Watch2020V2"
            #define RES_X_MAX       240
            #define RES_Y_MAX       240
            #define USE_PSRAM_ALLOC_LVGL                    /** @brief enabled LVGL to use PSRAM */ 
            #define ENABLE_WEBSERVER                        /** @brief To disable built-in webserver, comment this line */
            #define ENABLE_FTPSERVER                        /** @brief To disable built-in ftpserver, comment this line */
        #elif defined( LILYGO_WATCH_2020_V3 )
            #undef LILYGO_WATCH_LVGL
            #define HARDWARE_NAME   "T-Watch2020V3"
            #define RES_X_MAX       240
            #define RES_Y_MAX       240
            #define USE_PSRAM_ALLOC_LVGL                    /** @brief enabled LVGL to use PSRAM */ 
            #define ENABLE_WEBSERVER                        /** @brief To disable built-in webserver, comment this line */
            #define ENABLE_FTPSERVER                        /** @brief To disable built-in ftpserver, comment this line */
        #elif defined( LILYGO_WATCH_2021 )
            #define HARDWARE_NAME   "T-Watch2021"
            #define RES_X_MAX       240
            #define RES_Y_MAX       240
            #define ONLY_ESSENTIAL
            #define ROUND_DISPLAY
            #define NO_BLUETOOTH
            #define NO_WATCHFACE
            #define NO_UPDATES
        #elif defined( M5PAPER )
            #define HARDWARE_NAME   "m5stack-fire"
            #define RES_X_MAX       540
            #define RES_Y_MAX       960
            #define ENABLE_WEBSERVER                        /** @brief To disable built-in webserver, comment this line */
            #define ENABLE_FTPSERVER                        /** @brief To disable built-in ftpserver, comment this line */
            #define NO_WATCHFACE
            #define NO_BLUETOOTH
        #elif defined( M5CORE2 )
            #define HARDWARE_NAME   "m5stack-core2"
            #define RES_X_MAX       320
            #define RES_Y_MAX       240
            #define ENABLE_WEBSERVER                        /** @brief To disable built-in webserver, comment this line */
            #define ENABLE_FTPSERVER                        /** @brief To disable built-in ftpserver, comment this line */
            #define NO_BLUETOOTH
        #else
            #error "no destination hardware version defined"
        #endif
    #endif
    /**
     * Enable non-latin languages support
     */
    #define USE_EXTENDED_CHARSET CHARSET_CYRILLIC
    /**
     * firmeware version string
     */
    #define __FIRMWARE__            "2021112201"
    /**
     * Allows to include config.h from C code
     */
    #ifdef __cplusplus
        #ifdef NATIVE_64BIT
        #else
            #ifdef M5PAPER
            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                #include <LilyGoWatch.h>
            #endif
        #endif
        #define _CONFIG_H 
    #endif

#endif // _CONFIG_H