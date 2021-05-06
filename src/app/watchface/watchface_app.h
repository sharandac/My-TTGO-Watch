/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#ifndef _WATCHFACE_APP_H
    #define _WATCHFACE_APP_H

    #include <TTGO.h>

    #define WATCHFACE_APP_INFO_LOG     log_i
    #define WATCHFACE_APP_LOG          log_d
    #define WATCHFACE_APP_ERROR_LOG    log_e

    void watchface_app_setup( void );
    uint32_t watchface_app_get_app_main_tile_num( void );

#endif // _WATCHFACE_APP_H