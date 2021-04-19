/****************************************************************************
 *   Copyright  2021  Guilhem Bonnefille <guilhem.bonnefille@gmail.com>
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
#ifndef _FAKEGPS_H
    #define _FAKEGPS_H

    #define   GEOIP_URL     "http://ip-api.com/json/"
    #define   FAKEGPS_SYNC_REQUEST      _BV(1)

    /**
     * @brief get gps via ip-api.com and set it in gpsctl to fake gps
     */
    void fakegps_setup( void );
    /**
     * @brief get the lastest lat
     * 
     * @return lat
     */
    double fakegps_get_last_lat( void );
    /**
     * @brief get the lastest lon
     * 
     * @return lon
     */
    double fakegps_get_last_lon( void );
#endif // _FAKEGPS_H