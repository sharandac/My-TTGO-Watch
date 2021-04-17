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
#ifndef _TIME_SYNC_CONFIG_H
    #define _TIME_SYNC_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define TIMESYNC_JSON_CONFIG_FILE   "/timesync.json"    /** @brief defines json config file name */
    #define TIMEZONE_NAME_DEFAULT       "Etc/GMT"           /** @brief defines default time zone name */
    #define TIMEZONE_RULE_DEFAULT       "GMT0"              /** @brief defines default time zone rule */

    /**
     * @brief time sync config structure
     */
    class timesync_config_t : public BaseJsonConfig {
        public:
        timesync_config_t();
        bool timesync = true;                               /** @brief time sync on/off */
        bool daylightsave = false;                          /** @brief day light save on/off */
        int32_t timezone = 0;                               /** @brief time zone from 0..24, 0 means -12 */
        bool use_24hr_clock = true;                         /** @brief 12h/24h time format */
        char timezone_name[32] = TIMEZONE_NAME_DEFAULT;     /** @brief name of the time zone to use */
        char timezone_rule[48] = TIMEZONE_RULE_DEFAULT;     /** @brief time zone rule to use */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }

    } ;

#endif // _TIME_SYNC_CONFIG_H
