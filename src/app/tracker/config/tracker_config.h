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
#ifndef _TRACKER_CONFIG_H
    #define _TRACKER_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define TRACKER_JSON_COFIG_FILE         "/tracker.json"   /** @brief defines json config file name */

    /**
     * @brief blectl config structure
     */
    class tracker_config_t : public BaseJsonConfig {
        public:
        tracker_config_t();
        bool gps_autoon = true;             /** @brief gps auto on/off */
        int interval = 10;                  /** @brief track point interval */
        bool gps_on_standby = false;        /** @brief enable gps on standby */
        bool track_on_fix = true;           /** @brief track when fix */
        bool vibe_on_fix = true;            /** @brief vibe when fix */
        bool piep_on_fix = true;            /** @brief piep when fix */
        char storage[32] = "/spiffs";      /** @brief file storage prefix */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;

#endif // _TRACKER_CONFIG_H