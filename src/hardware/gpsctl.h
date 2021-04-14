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
#ifndef _GPSCTL_H
    #define _GPSCTL_H

    #include "TTGO.h"
    #include "callback.h"
    #include "hardware/config/gpsctlconfig.h"

    #define GPSCTL_INTERVAL             1000

    #define GPSCTL_ENABLE               _BV(0)         /** @brief event mask for GPS enabled/disable, callback arg is (bool*) */
    #define GPSCTL_DISABLE              _BV(1)         /** @brief event mask for GPS enabled/disable, callback arg is (bool*) */
    #define GPSCTL_FIX                  _BV(2)         /** @brief event mask for GPS enabled/disable, callback arg is (bool*) */
    #define GPSCTL_NOFIX                _BV(3)         /** @brief event mask for GPS enabled/disable, callback arg is (bool*) */
    #define GPSCTL_UPDATE_LOCATION      _BV(4)         /** @brief event mask for GPS location update */
    #define GPSCTL_UPDATE_DATE          _BV(5)         /** @brief event mask for GPS date update */
    #define GPSCTL_UPDATE_TIME          _BV(6)         /** @brief event mask for GPS time update*/
    #define GPSCTL_UPDATE_SPEED         _BV(7)         /** @brief event mask for GPS speed update*/
    #define GPSCTL_UPDATE_ALTITUDE      _BV(8)         /** @brief event mask for GPS altitude update*/
    #define GPSCTL_UPDATE_SATELLITE     _BV(9)         /** @brief event mask for GPS satellite update*/
    #define GPSCTL_UPDATE_HDOP          _BV(10)        /** @brief event mask for GPS hdop update*/

    /**
     * @brief gps data structure
     */
    typedef struct {
        bool valid = false;
        double lat = 0;
        double lon = 0;
        double speed_mph = 0;
        double speed_mps = 0;
        double speed_kmh = 0;
        double altitude_feed = 0;
        double altitude_meters = 0;
        uint32_t satellites = 0;
        uint32_t hdop = 0;
    } gps_data_t;

    /**
     * @brief setup gps
     */
    void gpsctl_setup( void );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event           possible values: GPSCTL_*
     * @param   callback_func   pointer to the callback function
     * @param   id              program id
     * 
     * @return  true if success, false if failed
     */
    bool gpsctl_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief enabled GPS
     */
    void gpsctl_on( void );
    /**
     * @brief disabled GPS
     */
    void gpsctl_off( void );
    /**
     * @brief get autoon config
     *
     * @return true if enable
     */
    bool gpsctl_get_autoon( void );
    /**
     * @brief set autoon config
     * 
     * @param autoon    true enabed, false disable
     */
    void gpsctl_set_autoon( bool autoon );
    /**
     * @brief get geoip fake gps config
     * 
     * @return  true if enable
     */
    bool gpsctl_get_gps_over_ip( void );
    /**
     * @brief set geoip fake gps config
     * 
     * @param   gps_over_ip true enable, false disable
     */
    void gpsctl_set_gps_over_ip( bool gps_over_ip );
    /**
     * @brief get gps an standby config
     * 
     * @return  true if enable
     */
    bool gpsctl_get_enable_on_standby( void );
    /**
     * @brief set gps on standby config
     * 
     * @param enable_on_standby true enable, false disable
     */
    void gpsctl_set_enable_on_standby( bool enable_on_standby );
    /**
     * @brief set a new location
     * 
     * @param   lat set lat
     * @param   lon set lon
     */
    void gpsctl_set_location( double lat, double lon );

#endif // _GPSCTL_H