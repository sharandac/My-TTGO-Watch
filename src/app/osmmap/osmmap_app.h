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
#ifndef _OSM_APP_H
    #define _OSM_APP_H

    #define OSMMAP_APP_INFO_LOG     log_i
    #define OSMMAP_APP_LOG          log_d
    #define OSMMAP_APP_ERROR_LOG    log_e

    /**
     * @brief osmmap app setup
     */
    void osmmap_app_setup( void );
    /**
     * @brief show/hide osmmap app icons
     * 
     * @param show true means hide and false means show
     */
    void osmmap_app_hide_app_icon_info( bool show );
    /**
     * @brief show/hide osmmap widget icon
     * 
     * @param show true means hide and false means show
     */
    void osmmap_app_hide_widget_icon_info( bool show );
    /**
     * @brief get osmmap setup tile number
     * 
     * @return uint32_t
     */
    uint32_t osmmap_app_get_app_setup_tile_num( void );
    /**
     * @brief get osmmap app tile number
     * 
     * @return uint32_t 
     */
    uint32_t osmmap_app_get_app_main_tile_num( void );

#endif // _OSM_APP_H