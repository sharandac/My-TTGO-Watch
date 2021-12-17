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
#ifndef _MAIL_CONFIG_H
    #define _MAIL_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define MAIL_JSON_COFIG_FILE         "/mail.json"   /** @brief defines json config file name */

    /**
     * @brief mail config structure
     */
    class mail_config_t : public BaseJsonConfig {
        public:
        mail_config_t();
        char imap_server[128] = "";                 /** @brief gps auto on/off */
        uint32_t imap_port = 993;                   /** @brief enable gps on standby */
        bool use_ssl = true;                        /** @brief wifi auto on/off */
        char inbox_folder[128] = "INBOX";           /** @brief osm map name */
        uint32_t max_msg = 5;                       /** @brief max mail to fetch */
        uint32_t max_msg_size = 512;                /** @brief max mail buffer size to fetch */
        char username[128] = "";                    /** @brief load ahead on/off */
        char password[128] = "";                    /** @brief left/right hand icons, false mean left hand */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;

#endif // _MAIL_CONFIG_H