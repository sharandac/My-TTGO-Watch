/****************************************************************************
 *   Jun 14 17:13:51 2022
 *   Copyright  2022  Dirk Brosswick
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
#ifndef _PRESENTER_APP_H
    #define _PRESENTER_APP_H

    #include "utils/logging.h"
    /**
     * @brief presenter main setup function
     */
    void presenter_app_setup( void );
    /**
     * @brief get the presenter app main tile numnber
     * 
     * @return uint32_t 
     */
    uint32_t presenter_app_get_app_main_tile_num( void );
    /**
     * @brief get the presenter app setup tile numnber
     * 
     * @return uint32_t 
     */
    uint32_t presenter_app_get_app_setup_tile_num( void );

#endif // _PRESENTER_APP_H