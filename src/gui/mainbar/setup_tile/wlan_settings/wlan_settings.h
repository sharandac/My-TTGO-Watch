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
#ifndef _WLAN_SETTINGS_H
    #define _WLAN_SETTINGS_H

    void wlan_settings_tile_setup( void );
    void wlan_password_tile_setup( uint32_t wifi_password_tile_num );
    void wlan_setup_tile_setup( uint32_t wifi_setup_tile_num );
    /**
     * @brief get the wlan setup tile number
     * 
     * @return  tile number
     */
    uint32_t wifi_get_setup_tile_num( void );

#endif // _WLAN_SETTINGS_H