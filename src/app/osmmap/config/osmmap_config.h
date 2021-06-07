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
#ifndef _OSMMAP_CONFIG_H
    #define _OSMMAP_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define OSMMAP_JSON_COFIG_FILE         "/osmmap.json"   /** @brief defines json config file name */

    /**
     * @brief blectl config structure
     */
    class osmmap_config_t : public BaseJsonConfig {
        public:
        osmmap_config_t();
        bool gps_autoon = true;             /** @brief gps auto on/off */
        bool gps_on_standby = false;        /** @brief enable gps on standby */
        bool wifi_autoon = true;            /** @brief wifi auto on/off */
        bool load_ahead = false;            /** @brief load ahead on/off */
        bool left_right_hand = false;       /** @brief left/right hand icons, false mean left hand */
        char osmmap[128] = "";              /** @brief osm map name */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;

#endif // _OSMMAP_CONFIG_H