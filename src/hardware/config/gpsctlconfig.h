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
#ifndef _GPS_CONFIG_H
    #define _GPS_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define GPSCTL_JSON_CONFIG_FILE  "/gpsctl.json"           /** @brief defines json config file name */

    /**
     * @brief gpxctl config structure in memory
     */
    class gpsctl_config_t : public BaseJsonConfig {
        public:
        gpsctl_config_t();
        bool autoon = true;                     /** @brief autoon config item, true if autoon enabled, false if disabled */        
        bool enable_on_standby = false;         /** @brief enable on standby on/off */
        bool app_use_gps = false;               /** @brief permission for apps, to get gps location */
        bool gps_over_ip = false;               /** @brief enable gps over ip */
        int32_t TXPin = -1;                     /** @brief enable gps modules on M5stack use PIN as TX*/
        int32_t RXPin = -1;                     /** @brief enable gps modules on M5stack use PIN as RX */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    };

#endif // _GPS_CONFIG_H