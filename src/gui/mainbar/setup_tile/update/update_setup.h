/****************************************************************************
 *   July 31 07:31:12 2020
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
#ifndef _UPDATE_SETUP_H
    #define _UPDATE_SETUP_H

    #include <TTGO.h>

    #define UPDATE_CONFIG_FILE             "/update.cfg"
    #define UPDATE_JSON_CONFIG_FILE        "/update.json"

    #define FIRMWARE_UPDATE_URL            "http://www.neo-guerillaz.de/ttgo-t-watch2020_v1.version.json"

    typedef struct {
        bool autosync = true;
        char updateurl[512] = FIRMWARE_UPDATE_URL;
    } update_config_t;

    void update_setup_tile_setup( uint32_t tile_num );
    bool update_setup_get_autosync( void );
    char* update_setup_get_url( void );

#endif // _UPDATE_SETUP_H