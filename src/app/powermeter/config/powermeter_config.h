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
#ifndef _POWERMETER_CONFIG_H
    #define _POWERMETER_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define POWERMETER_JSON_CONFIG_FILE        "/powermeter.json"
    
    /**
     * @brief blectl config structure
     */
    class powermeter_config_t : public BaseJsonConfig {
        public:
        powermeter_config_t();
        char server[64] = "";
        int32_t port = 1883;
        bool ssl = false;
        char user[32] = "";
        char password[32] = "";
        char topic[64] = "";
        bool autoconnect = false;
        bool widget = false;

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;

#endif // _POWERMETER_CONFIG_H