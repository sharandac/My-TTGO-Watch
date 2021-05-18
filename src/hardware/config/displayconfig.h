/****************************************************************************
 *   Tu May 22 21:23:51 2020
 *   Copyright  2020  Dirk Brosswick
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
#ifndef _DISPLAY_CONFIG_H
    #define _DISPLAY_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define DISPLAY_JSON_CONFIG_FILE    "/display.json" /** @brief defines json config file name */
    
    #define DISPLAY_MIN_TIMEOUT         15              /** @brief min display timeout */
    #define DISPLAY_MAX_TIMEOUT         300             /** @brief max display timeout */
    #define DISPLAY_MIN_BRIGHTNESS      8               /** @brief min display brightness */
    #define DISPLAY_MAX_BRIGHTNESS      255             /** @brief max display brightness */
    #define DISPLAY_MIN_ROTATE          0               /** @brief min display rotation */
    #define DISPLAY_MAX_ROTATE          270             /** @brief max display rotation */

    /**
     * @brief display config structure
     */
    class display_config_t : public BaseJsonConfig {
        public:
        display_config_t();
        uint32_t brightness = DISPLAY_MAX_BRIGHTNESS;   /** @brief display brightness */
        uint32_t timeout = DISPLAY_MIN_TIMEOUT;         /** @brief display time out */
        uint32_t rotation = 0;                          /** @brief display rotation */
        bool block_return_maintile = false;             /** @brief block back to main tile on standby */
        bool use_dma = true;                            /** @brief use dma framebuffer */
        bool use_double_buffering = false;              /** @brief use double framebuffer */
        bool vibe = true;                               /** @brief vibe for touch feedback */
        uint32_t background_image = 4;                  /** @brief background image */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    };
#endif // _DISPLAY_CONFIG_H
