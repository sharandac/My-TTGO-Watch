/****************************************************************************
 *   July 23 00:23:05 2020
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
#ifndef _WEATHER_FETCH_H
    #define _WEATHER_FETCH_H

    #define OWM_HOST    "api.openweathermap.org"
    #define OWM_PORT    80

    #define WEATHER_TODAY_BUFFER_SIZE       10000
    #define WEATHER_FORECAST_BUFFER_SIZE    40000

    /**
     * @brief fetch today weather information
     * 
     * @param weather_config    pointer to the weather config
     * @param weather_today     pointer to the weather today structure
     *
     * @return  200 ok, otherwise failed
     */
    int weather_fetch_today( weather_config_t * weather_config, weather_forcast_t * weather_today );
    /**
     * @brief fetch forecast weather information
     * 
     * @param weather_config    pointer to the weather config
     * @param weather_forecast  pointer to the weather forecast structure
     *
     * @return  200 ok, otherwise failed
     */
    int weather_fetch_forecast( weather_config_t *weather_config, weather_forcast_t * weather_forecast );

#endif // _WEATHER_FETCH_H