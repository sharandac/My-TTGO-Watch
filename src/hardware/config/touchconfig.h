/****************************************************************************
 *   Tu May 4 17:23:51 2022
 *   Copyright  2022  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
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
#ifndef _TOUCH_CONFIG_H
    #define _TOUCH_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define TOUCH_JSON_CONFIG_FILE    "/touch.json"     /** @brief defines json config file name */

    #ifdef NATIVE_64BIT
        #define TOUCH_X_SCALE       1.0
        #define TOUCH_Y_SCALE       1.0
    #else
        #if defined( M5PAPER )
            #define TOUCH_X_SCALE       1.0
            #define TOUCH_Y_SCALE       1.0
        #elif defined( M5CORE2 )
            #define TOUCH_X_SCALE       1.0
            #define TOUCH_Y_SCALE       1.0
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            #define TOUCH_X_SCALE       1.15
            #define TOUCH_Y_SCALE       1.0
        #elif defined( LILYGO_WATCH_2021 )
            #define TOUCH_X_SCALE       1.0
            #define TOUCH_Y_SCALE       1.0
        #elif defined( WT32_SC01 )
            #define TOUCH_X_SCALE       1.0
            #define TOUCH_Y_SCALE       1.0
        #else
            #define TOUCH_X_SCALE       1.0
            #define TOUCH_Y_SCALE       1.0
            #warning "no special hardware configuration for touch"
        #endif
    #endif
    /**
     * @brief touch config structure
     */
    class touch_config_t : public BaseJsonConfig {
        public:
        touch_config_t();
        float x_scale = TOUCH_X_SCALE;        /** @brief x-scale factor */
        float y_scale = TOUCH_Y_SCALE;        /** @brief y-scale factor */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad( JsonDocument& document );
        virtual bool onSave( JsonDocument& document );
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    };
#endif // _TOUCH_CONFIG_H
