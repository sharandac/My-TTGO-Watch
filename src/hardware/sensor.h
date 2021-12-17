/****************************************************************************
 *   Sep 11 10:11:10 2021
 *   Copyright  2021  Dirk Brosswick
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
#ifndef _SENSOR_H
    #define _SENSOR_H

    #include "callback.h"

    #define SENSOR_UPDATE_INTERVAL      60              /** @brief sensir update interval in seconds */
	/**
     * buttons events mask
     */
    #define     SENSOR_TEMPERATURE      _BV(0)          /** @brief event mask for temperature */
    #define     SENSOR_HUMIDITY         _BV(1)          /** @brief event mask for humidity */
    #define     SENSOR_RELHUMIDITY      _BV(2)          /** @brief event mask for relative humidity */
    #define     SENSOR_PRESSURE         _BV(3)          /** @brief event mask for pressure */
    /**
     * @brief button setup function
     */
    void sensor_setup( void );
    /**
     * @brief check if an sensor available
     * 
     * @return  true means available, false means no sensor available
     */
    bool sensor_get_available( void );
    /**
     * @brief read the current temperature
     * 
     * @return  temerature in degree
     */
    float sensor_get_temperature( void );
    /**
     * @brief read the current humidity
     * 
     * @return  humidity in percent
     */
    float sensor_get_humidity( void );
    /**
     * @brief read the current relativ humidity
     * 
     * @return  relativ humidity in percent
     */
    float sensor_get_relativ_humidity( void );
    /**
     * @brief read the current temperature
     * 
     * @return  pressure in bar
     */
    float sensor_get_pressure( void );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event           possible values: SENSOR_TEMPERATURE, SENSOR_HUMIDITY and SENSOR_PRESSURE
     * @param   callback_func   pointer to the callback function
     * @param   id              program id
     * 
     * @return  true if success, false if failed
     */
    bool sensor_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );

#endif // _SENSOR_H