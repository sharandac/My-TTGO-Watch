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
#ifndef _BLECTL_CONFIG_H
    #define _BLECTL_CONFIG_H

    #include "utils/basejsonconfig.h"

    #define CUSTOM_AUDIO_ENTRYS            5
    #define BLECTL_JSON_COFIG_FILE         "/blectl.json"   /** @brief defines json config file name */

    /**
     * @brief custom audio notification structure
     */
    typedef struct {
        char text[64]="";
        char value[32]="";
    } blectl_custom_audio;

    /**
     * @brief blectl config structure
     */
    class blectl_config_t : public BaseJsonConfig {
        public:
        blectl_config_t();
        bool autoon = true;                                     /** @brief auto on/off */
        bool advertising = true;                                /** @brief advertising on/off */
        bool enable_on_standby = false;                         /** @brief enable on standby on/off */
        bool disable_only_disconnected = false;                 /** @brief disable only when disconnected on/off */
        bool wakeup_on_notification = true;                     /** @brief wakes up on notifications */
        bool show_notification = true;                          /** @brief enable show notifications */
        bool vibe_notification = true;                          /** @brief enable vibe on notifications */
        bool sound_notification = true;                         /** @brief enable sound on notifications */
        bool media_notification = true;                         /** @brief enable media on notifications */
        int txpower = 1;                                        /** @brief tx power, valide values are from 0 to 4 */
        uint16_t minInterval = 0x06;                            /** @brief connParams for minInterval  0x10*1.25ms = 20ms */
        uint16_t maxInterval = 0x20;                            /** @brief connParams for maxInterval  0x20*1.25ms = 40ms */
        uint16_t latency = 20;                                  /** @brief skipped events */
        uint16_t timeout = 400;                                 /** @brief connParams for minInterval  0x10*1.25ms = 20ms */
        blectl_custom_audio* custom_audio_notifications = NULL; /** @brief timeout = 400*10ms = 4000ms */

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 12000; }
    } ;

#endif // _BLECTL_CONFIG_H