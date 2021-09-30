/****************************************************************************
 *   Aug 11 17:13:51 2020
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
#ifndef _UPDATE_CONFIG_H
    #define _UPDATE_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define UPDATE_JSON_COFIG_FILE              "/update.json"          /** @brief defines json config file name */
    #define FIRMWARE_UPDATE_URL_LEN             512

    #if defined( M5PAPER )
        #define FIRMWARE_UPDATE_URL            "http://www.neo-guerillaz.de/m5stack-fire.version.json"
    #elif defined( LILYGO_WATCH_2020_V1 )
        #define FIRMWARE_UPDATE_URL            "http://www.neo-guerillaz.de/ttgo-t-watch2020_v1.version.json"
    #elif defined( LILYGO_WATCH_2020_V2 )
        #define FIRMWARE_UPDATE_URL            "http://www.neo-guerillaz.de/ttgo-t-watch2020_v2.version.json"
    #elif defined( LILYGO_WATCH_2020_V3 )
        #define FIRMWARE_UPDATE_URL            "http://www.neo-guerillaz.de/ttgo-t-watch2020_v3.version.json"
    #elif defined( LILYGO_WATCH_2021 )
        #define FIRMWARE_UPDATE_URL            "http://www.neo-guerillaz.de/ttgo-t-watch2021.version.json"
    #elif defined( M5CORE2 )
        #define FIRMWARE_UPDATE_URL            "http://www.neo-guerillaz.de/m5stack-core2.version.json"
    #endif

    #if !defined( FIRMWARE_UPDATE_URL )
        #error "nohardware version defined"
    #endif

    /**
     * @brief update config structure
     */
    class update_config_t : public BaseJsonConfig {
        public:
        update_config_t();
        bool autosync = true;
        bool autorestart = false;
        char *updateurl = NULL;

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;

#endif // _UPDATE_CONFIG_H