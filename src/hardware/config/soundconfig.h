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
#ifndef _SOUND_CONFIG_H
    #define _SOUND_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define SOUND_JSON_CONFIG_FILE    "/sound.json"     /** @brief defines json config file name */

    /**
     * @brief sound config structure
     */
    class sound_config_t : public BaseJsonConfig {
        public:
        sound_config_t();
        uint8_t volume = 50;                            /** @brief sound value from 0..100 */
        bool enable = true;                             /** @brief sound on/off */

        bool silence_timeframe = false;                 /** @brief silence sound time frame enabled/disabled */
        int silence_start_hour = 0;                     /** @brief silence sound frame start hour */
        int silence_start_minute = 0;                   /** @brief silence sound frame start min */
        int silence_end_hour = 0;                       /** @brief silence sound frame end hour */
        int silence_end_minute = 0;                     /** @brief silence sound frame end min */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    };

#endif // _SOUND_CONFIG_H
