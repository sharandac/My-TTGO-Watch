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

    #define TIME_SYNC_REQUEST    _BV(0)

    #define TIMESYNC_CONFIG_FILE  "/timesync.cfg"

    typedef struct {
        bool timesync = true;
        bool daylightsave = false;
        int32_t timezone = 0;
    } timesync_config_t;

        /*
     * @brief setup display
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void timesync_setup( TTGOClass *ttgo );
    /*
     * @brief save config for timesync to spiffs
     */
    void timesync_save_config( void );
    /*
     * @brief read config for timesync from spiffs
     */
    void timesync_read_config( void );
    /*
     * @brief get the status if timesync enable/disable
     * 
     * @return true or false
     */
    bool timesync_get_timesync( void );
    /*
     * @brief enable/disable automatic timesync when wifi is connected
     * 
     * @param timesync  true or false
     */
    void timesync_set_timesync( bool timesync );
    /*
     * @brief get the status if daylightsave enable/disable
     * 
     * @return true or false
     */
    bool timesync_get_daylightsave( void );
    /*
     * @brief enable/disable daylight saving
     * 
     * @param daylightsave  true or false
     */
    void timesync_set_daylightsave( bool daylightsave );
    /*
     * @brief get the current timezone
     * 
     * @return  timezone from UTC-12 to UTC+12
     */
    int32_t timesync_get_timezone( void );
    /*
     * @brief set the current timezone
     * 
     * @param timezone timezone from UTC-12 to UTC+12
     */
    void timesync_set_timezone( int32_t timezone );

    void timesyncToSystem( void );
    void timesyncToRTC( void );

#endif // _TIME_SYNC_H
