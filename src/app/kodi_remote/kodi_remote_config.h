/****************************************************************************
 *   June 14 02:01:00 2021
 *   Copyright  2021  Dirk Sarodnick
 *   Email: programmer@dirk-sarodnick.de
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
#ifndef _KODI_REMOTE_CONFIG_H
    #define _KODI_REMOTE_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define KODI_REMOTE_JSON_CONFIG_FILE        "/kodi_remote.json"

    /**
     * @brief kodi remote config structure
     */
    class kodi_remote_config_t : public BaseJsonConfig {
        public:
        kodi_remote_config_t();
        char host[32] = "";
        uint16_t port = 8080;
        char user[32] = "";
        char pass[32] = "";

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;
    
#endif // _KODI_REMOTE_CONFIG_H