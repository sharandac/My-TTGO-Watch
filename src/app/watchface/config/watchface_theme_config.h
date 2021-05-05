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
#ifndef _WATCHFACE_THEME_CONFIG_H
    #define _WATCHFACE_THEME_CONFIG_H

    #include <TTGO.h>
    #include "config.h"
    #include "utils/basejsonconfig.h"

    #define WATCHFACE_THEME_JSON_COFIG_FILE         "/watchface_theme.json"   /** @brief defines json config file name */
    #define WATCHFACE_LABEL_NUM                     3

    typedef struct {
        bool enable = true;
        bool smooth = true;
        int32_t x_offset = 0;
        int32_t y_offset = 0;
    } watchface_index_t;

    typedef struct {
        bool enable = true;
        char type[32] = "";
        char label[32] = "";
        char font_color[32] = "";
        char align[32] = "";
        int32_t font_size = 0;
        int32_t x_offset = 0;
        int32_t y_offset = 0;
        int32_t x_size = 0;
        int32_t y_size = 0;
    } watchface_label_t;

    typedef struct {
        watchface_index_t hour;
        watchface_index_t min;
        watchface_index_t sec;
        watchface_index_t hour_shadow;
        watchface_index_t min_shadow;
        watchface_index_t sec_shadow;
        watchface_label_t label[3];
    } watchface_dial_t;

    /**
     * @brief blectl config structure
     */
    class watchface_theme_config_t : public BaseJsonConfig {
        public:
        watchface_theme_config_t();
        watchface_dial_t dial;

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;

#endif // _WATCHFACE_THEME_CONFIG_H