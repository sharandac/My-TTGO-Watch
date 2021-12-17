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
#ifndef _WATCHFACE_CONFIG_H
    #define _WATCHFACE_CONFIG_H

    #include "utils/basejsonconfig.h"

#ifdef NATIVE_64BIT
    #include "utils/io.h"
    #include <string>

    using namespace std;
    #define String string
#else
    #include <Arduino.h>
#endif

    #define WATCHFACE_JSON_COFIG_FILE               "/watchface.json"   /** @brief defines json config file name */
    /**
     * define all theme download and compressed theme file
     */
    #define WATCHFACE_THEME_URL                     "https://raw.githubusercontent.com/sharandac/My-TTGO-Watchfaces/main"
    #define WATCHFACE_THEME_LIST_FILE               "/watchface_theme_list.json"
    #define WATCHFACE_THEME_FILE                    "/watchface.tar.gz"
    #define WATCHFACE_THEME_PREV                    "/watchface_theme_prev_120px.png"

    #define WATCHFACE_MAX_ENTRYS                    50
    /**
     * @brief blectl config structure
     */
    class watchface_config_t : public BaseJsonConfig {
        public:
        watchface_config_t();
        bool watchface_enable = false;              /** @brief enable the watchface on wakeup */
        bool watchface_antialias = true;            /** @brief setup antialias */
        String watchface_theme_url = "";            /** @brief theme url */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;

#endif // _WATCHFACE_CONFIG_H