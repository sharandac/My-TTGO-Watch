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
#ifndef _DEVICE_CONFIG_H
    #define _DEVICE_CONFIG_H

    #include "config.h"
    #include "utils/basejsonconfig.h"

    #define DEVICE_JSON_CONFIG_FILE    "/device.json" /** @brief defines json config file name */
    
    /**
     * @brief display config structure
     */
    class device_config_t : public BaseJsonConfig {
        public:
            device_config_t();
            char device_name[64] = HARDWARE_NAME;      /** @brief device name */

        protected:
            ////////////// Available for overloading: //////////////
            virtual bool onLoad(JsonDocument& document);
            virtual bool onSave(JsonDocument& document);
            virtual bool onDefault( void );
            virtual size_t getJsonBufferSize() { return 1000; }
    };
#endif // _DEVICE_CONFIG_H