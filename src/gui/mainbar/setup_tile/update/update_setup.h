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
    
    /**
     * @brief setup update setup tile
     * 
     * @param tile_num tile number
     */
    void update_setup_tile_setup( uint32_t tile_num );
    /**
     * @brief get autosync config state
     * 
     * @return true if sync enablled, false if not
     */
    bool update_setup_get_autosync( void );
    /**
     * @brief get autorestart state
     * 
     * @return true if reset after success update, false if not
     */
    bool update_setup_get_autorestart( void );
    /**
     * @brief get update file url
     * 
     * @return pointer to url string
     */
    char* update_setup_get_url( void );

#endif // _UPDATE_SETUP_H