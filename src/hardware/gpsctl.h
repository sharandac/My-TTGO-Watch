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

    #include "callback.h"
    #include "hardware/config/gpsctlconfig.h"
    #include "utils/io.h"
    
    #define GPSCTL_INFO_LOG                 log_i
    #define GPSCTL_DEBUG_LOG                log_d
    #define GPSCTL_ERROR_LOG                log_e

    #define GPSCTL_INTERVAL                 1000           /** @brief gps data intervall in milliseconds */

    #define GPSCTL_ENABLE                   _BV(0)         /** @brief event mask for GPS enabled */
    #define GPSCTL_DISABLE                  _BV(1)         /** @brief event mask for GPS disable */
    #define GPSCTL_FIX                      _BV(2)         /** @brief event mask for GPS has an fix */
    #define GPSCTL_NOFIX                    _BV(3)         /** @brief event mask for GPS has no fix */
    #define GPSCTL_SET_APP_LOCATION         _BV(4)         /** @brief event mask for GPS set location for user application like weather-app */
    #define GPSCTL_UPDATE_LOCATION          _BV(5)         /** @brief event mask for GPS location update */
    #define GPSCTL_UPDATE_DATE              _BV(6)         /** @brief event mask for GPS date update */
    #define GPSCTL_UPDATE_TIME              _BV(7)         /** @brief event mask for GPS time update*/
    #define GPSCTL_UPDATE_SPEED             _BV(8)         /** @brief event mask for GPS speed update*/
    #define GPSCTL_UPDATE_ALTITUDE          _BV(9)         /** @brief event mask for GPS altitude update*/
    #define GPSCTL_UPDATE_SATELLITE         _BV(10)        /** @brief event mask for GPS satellite update*/
    #define GPSCTL_UPDATE_SATELLITE_TYPE    _BV(11)        /** @brief event mask for GPS satellite type update*/
    #define GPSCTL_UPDATE_SOURCE            _BV(12)        /** @brief event mask for GPS source update*/
    #define GPSCTL_UPDATE_CONFIG            _BV(13)        /** @brief event mask for GPS configuration*/
    /**
     * @brief gps source types
     */
    typedef enum {
        GPS_SOURCE_UNKNOWN = 0,                         /** @brief unknown source */
        GPS_SOURCE_FAKE,                                /** @brief fake gps source */
        GPS_SOURCE_IP,                                  /** @brief fake gps source from geoip service */
        GPS_SOURCE_USER,                                /** @brief fake gps source from user */
        GPS_SOURCE_GPS,                                 /** @brief gps source */
        GPS_SOURCE_NUM
    } gps_source_t;
    /**
     * @brief gps data structure
     */
    typedef struct {
        gps_source_t gps_source = GPS_SOURCE_UNKNOWN;   /** @brief gps source */
        bool gpsfix = false;                            /** @brief gps fix flag for internal use */
        bool valid_location = false;                    /** @brief true if location valid */
        bool valid_speed = false;                       /** @brief true if speed valid */
        bool valid_altitude = false;                    /** @brief true if altitude valid */
        bool valid_satellite = false;                   /** @brief true if satellite valid */
        double lat = 0;                                 /** @brief gps latitude */
        double lon = 0;                                 /** @brief gps longitude */
        double speed_mph = 0;                           /** @brief speed in miles per hour */
        double speed_mps = 0;                           /** @brief speed in meter per second */
        double speed_kmh = 0;                           /** @brief speed in kilometers per hour */
        double altitude_feed = 0;                       /** @brief altitude in feed */
        double altitude_meters = 0;                     /** @brief altitude in meter */
        uint32_t satellites = 0;                        /** @brief number of seen satellites */
        struct {
            uint32_t gps_satellites = 0;                /** @brief number of gps satellits in view */
            uint32_t glonass_satellites = 0;            /** @brief number of glonass satellits in view */
            uint32_t baidou_satellites = 0;             /** @brief number of baidou satellits in view */
        } satellite_types;
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
     * @brief get app_use_gps config
     * 
     * @return true if use or false if not
     */
    bool gpsctl_get_app_use_gps( void );
    /**
     * @brief set app_use_gps
     * 
     * @param app_use_gps   true if use or false if not
     */
    void gpsctl_set_app_use_gps( bool app_use_gps );
    /**
     * @brief set a new location
     * 
     * @param   lat set lat
     * @param   lon set lon
     * @param   altitude set altitude
     * @param   gps_source gps data source
     * @param   app_location true to inform app for a new location like weather app
     */
    void gpsctl_set_location( double lat, double lon, double altitude, gps_source_t gps_source , bool app_location );
    /**
     * @brief get gps source string
     * 
     * @param gps_source gps source enum
     */
    const char *gpsctl_get_source_str( gps_source_t gps_source );
    /**
     * @brief set rx and tx pin configuration
     * 
     * @param   rx      rx gpio pin ( rx < -1 means disabled )
     * @param   tx      tx gpio pin ( tx < -1 means disabled )
     */
    void gpsctl_set_gps_rx_tx_pin( int8_t rx, int8_t tx );
    /**
     * @brief get rx and tx pin configuration
     * 
     * @param   rx      pointer to a rx variable
     * @param   tx      pointer to a tx variable
     */
    void gpsctl_get_gps_rx_tx_pin( int8_t *rx, int8_t *tx );

#endif // _GPSCTL_H