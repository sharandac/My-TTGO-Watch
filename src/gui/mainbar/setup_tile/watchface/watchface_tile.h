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
#ifndef _WATCHFACE_APP_TILE_H
    #define _WATCHFACE_APP_TILE_H

    #define WATCHFACE_LOG                       log_d
    /**
     * @brief watchface tile setup
     */
    void watchface_tile_setup( void );
    /**
     * @brief watchface enable after wakeup config
     * 
     * @param enable    true enable jump into watchface after wakeup
     */
    void watchface_enable_tile_after_wakeup( bool enable );
    /**
     * @brief get watchface enable after wakeup config
     * 
     * @return true enable jump into watchface after wakeup
     */
    bool watchface_get_enable_tile_after_wakeup( void );
    /**
     * @brief reload watchface theme from /spiffs or setup default if no
     * watchface_theme.json founf in /watchface/
     */
    void watchface_reload_theme( void );
    /**
     * @brief clear watchface theme to default
     * 
     * @param return_tile   return tile after preview
     */
    void watchface_default_theme( void );
    /**
     * @brief reload watchface theme and show a preview
     * 
     * @param return_tile   return tile after preview
     */
    void watchface_reload_and_test( void );
    /**
     * @brief unzip watchface.tar.gz from /spiffs, install theme
     * and show a preview
     */
    void watchface_decompress_theme( void );
    /**
     * @brief setup antialias
     * 
     * @param   enable  true enable antialias
     */
    void watchface_tile_set_antialias( bool enable );

#endif // _WATCHFACE_APP_TILE_H