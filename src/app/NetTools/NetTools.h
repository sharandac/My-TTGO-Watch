/****************************************************************************
 *  NetTools.h
 *  Copyright  2020  David Stewart / NorthernDIY
 *  Email: genericsoftwaredeveloper@gmail.com
 *
 *  Requires Libraries: 
 *      WakeOnLan by a7md0      https://github.com/a7md0/WakeOnLan
 *
 *  Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  Example_App"
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
#ifndef _NETTOOLS_H
    #define _NETTOOLS_H

    #include <TTGO.h>

//    #define EXAMPLE_WIDGET    // uncomment if an widget need
    #define NetTools_JSON_CONFIG_FILE  "/NetTools.json"
    #define TASMOTA_SUFFIX  "/cm?cmnd=Power%20Toggle"
    #define HTTP_PREFIX  "http://"
    
    typedef struct {
        char mac_address[21] = "00:00:00:00:00:00:00"; //A default value in typical MAC address format
        bool ping_status = false;
        char tasmota1_ip[16] = "000.000.000.000"; //A default value in typical IP address format
        char tasmota2_ip[16] = "000.000.000.000"; //A default value in typical IP address format
    } NetTools_config_t;

    void NetTools_setup( void );
    uint32_t NetTools_get_app_setup_tile_num( void );
    uint32_t NetTools_get_app_main_tile_num( void );
    NetTools_config_t *NetTools_get_config( void );
    void NetTools_save_config( void );
#endif // NETTOOLS_H
