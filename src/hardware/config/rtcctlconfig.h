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
#ifndef _RTCCTLCONFIG_H
    #define _RTCCTLCONFIG_H    

    #include "utils/basejsonconfig.h"    

    #define CONFIG_FILE_PATH         "/rtcctr.json"

    #define VERSION_KEY "version"
    #define ENABLED_KEY "enabled"
    #define HOUR_KEY "hour"
    #define MINUTE_KEY "minute"
    #define WEEK_DAYS_KEY "week_days" 

    #define DAYS_IN_WEEK 7
    #define RTCCTL_ALARM_NOT_SET -1

    class rtcctl_alarm_t : public BaseJsonConfig {
        public:
        rtcctl_alarm_t();
        bool enabled;                       /** @brief alarm enabled */
        uint8_t hour;                       /** @brief alarm hour */
        uint8_t minute;                     /** @brief alarm minute */
        bool week_days[DAYS_IN_WEEK];       /** @brief starting from sunday to be aligned with tm */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;

#endif // _RTCCTLCONFIG_H