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
#ifndef _TIME_SYNC_H
    #define _TIME_SYNC_H

    #include <TTGO.h>
    #include "callback.h"

    #define TIME_SYNC_REQUEST       _BV(0)
    #define TIME_SYNC_OK            _BV(1)

    #define TIMESYNC_JSON_CONFIG_FILE   "/timesync.json"    /** @brief defines json config file name */
    #define TIMEZONE_NAME_DEFAULT       "Etc/GMT"           /** @brief defines default time zone name */
    #define TIMEZONE_RULE_DEFAULT       "GMT0"              /** @brief defines default time zone rule */

    /**
     * @brief time sync config structure
     */
    typedef struct {
        bool timesync = true;               /** @brief time sync on/off */
        bool daylightsave = false;          /** @brief day light save on/off */
        int32_t timezone = 0;               /** @brief time zone from 0..24, 0 means -12 */
        bool use_24hr_clock = true;         /** @brief 12h/24h time format */
        char timezone_name[32] = TIMEZONE_NAME_DEFAULT; /** @brief name of the time zone to use */
        char timezone_rule[48] = TIMEZONE_RULE_DEFAULT; /** @brief time zone rule to use */
    } timesync_config_t;

    /**
     * @brief setup display
     */
    void timesync_setup( void );
    /**
     * @brief save config for timesync to spiffs
     */
    void timesync_save_config( void );
    /**
     * @brief read config for timesync from spiffs
     */
    void timesync_read_config( void );
    /**
     * @brief get the status if timesync enable/disable
     * 
     * @return true or false
     */
    bool timesync_get_timesync( void );
    /**
     * @brief enable/disable automatic timesync when wifi is connected
     * 
     * @param timesync  true or false
     */
    void timesync_set_timesync( bool timesync );
    /**
     * @brief get the status if daylightsave enable/disable
     * 
     * @return true or false
     */
    bool timesync_get_daylightsave( void );
    /**
     * @brief enable/disable daylight saving
     * 
     * @param daylightsave  true means daylight save enable, false means disable
     */
    void timesync_set_daylightsave( bool daylightsave );
    /**
     * @brief get the current timezone
     * 
     * @return  timezone from UTC-12 to UTC+12
     */
    int32_t timesync_get_timezone( void );
    /**
     * @brief set the current timezone
     * 
     * @param timezone  timezone from UTC-12 to UTC+12
     */
    void timesync_set_timezone( int32_t timezone );
    /**
     * @brief get the 24hr clock setting
     * 
     * @return  true if 24 clock enabled
     */
    bool timesync_get_24hr(void);
    /**
     * @brief enable/disable 24 hour clock format
     * 
     * @param use24  true or false
     */
    void timesync_set_24hr( bool use24 );
    char* timesync_get_timezone_name( void );
    void timesync_set_timezone_name( char * timezone_name );
    char* timesync_get_timezone_rule( void );
    void timesync_set_timezone_rule( const char * timezone_rule );
    /**
     * @brief wrapper function to sync the system with rtc
     */
    void timesyncToSystem( void );
    /**
     * @brief wrapper function to sync the rtc with system
     */
    void timesyncToRTC( void );
   /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event           possible values: TIME_SYNC_OK
     * @param   callback_func   pointer to the callback function 
     * @param   id              program id
     * 
     * @return  true if success, false if failed
     */
    bool timesync_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );

#endif // _TIME_SYNC_H
