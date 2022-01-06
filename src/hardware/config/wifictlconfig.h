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

#ifndef _WIFICTLCONFIG_H
    #define _WIFICTLCONFIG_H

    #include "config.h"
    #include "utils/basejsonconfig.h"
    #include "utils/webserver/webserver.h"
    #include "utils/ftpserver/ftpserver.h"
    #define NETWORKLIST_ENTRYS          20
    #define WIFICTL_JSON_CONFIG_FILE    "/wificfg.json"

    /**
     * @brief network list structure
     */
    typedef struct {
        char ssid[64]="";
        char password[64]="";
    } wifictl_networklist;

    /**
     * @brief wifictl config structure
     */
    class wifictl_config_t : public BaseJsonConfig {
        public:
        wifictl_config_t();
        bool autoon = true;                                 /** @brief enable on auto on/off an wakeup and standby */
        char hostname[32] = "T-Watch";                      /** @brief default hostname */
        bool enable_on_standby = false;                     /** @brief enable on standby */
        bool webserver = false;                             /** @brief enable on webserver */
        bool ftpserver = false;                             /** @brief enable on ftpserver */
        char ftpuser[32] = FTPSERVER_USER;                  /** @brief ftpserver username*/
        char ftppass[32] = FTPSERVER_PASSWORD;              /** @brief ftpserver password*/
        wifictl_networklist* networklist = NULL;            /** @brief network list config pointer */
        wifictl_networklist* networklist_tried = NULL;      /** @brief network list config pointer of networks a connection was tried */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 2000; }
    };

#endif // _WIFICTLCONFIG_H