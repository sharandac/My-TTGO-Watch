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
#ifndef _ALARM_CLOCK_CONFIG_H
    #define _ALARM_CLOCK_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define ALARM_CLOCK_JSON_CONFIG_FILE   "/alarm.json"   /** @brief defines json config file name */

    #define VERSION_KEY "version"
    #define BEEP_KEY "beep"
    #define FADE_KEY "fade"
    #define VIBE_KEY "vibe"
    #define SHOW_ON_MAIN_TILE_KEY "show_on_main_tile_key"

    #define AM "AM"
    #define PM "PM"

    #define AM_ONE "A"
    #define PM_ONE "P"

    /**
     * @brief alarm properties structure
     */
    class alarm_properties_t : public BaseJsonConfig {
        public:
        alarm_properties_t();
        bool beep;
        bool fade;
        bool vibe;
        bool show_on_main_tile;

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 2000; }
    } ;

#endif // _ALARM_CLOCK_CONFIG_H
