/****************************************************************************
 *   Apr 17 00:28:11 2021
 *   Copyright  2021  Federico Liuzzi
 *   Email: f.liuzzi02@gmail.com
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
#ifndef _SAILING_H
    #define _SAILING_H

    #define SAILING_INFO_LOG        log_i
    #define SAILING_ERROR_LOG       log_e
    #define SAILING_UDP_PORT        1234

    void sailing_setup( void );
    uint32_t sailing_get_app_setup_tile_num( void );
    uint32_t sailing_get_app_main_tile_num( void );

#endif // _SAILING_H

extern bool tracking;
