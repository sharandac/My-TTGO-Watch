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
#ifndef _MAIL_TILE_H
    #define _MAIL_TILE_H

    #include <TTGO.h>
    #include "gui/icon.h"

    #define MAX_WIDGET_NUM      3
    #define WIDGET_X_SIZE       64
    #define WIDGET_Y_SIZE       80
    #define WIDGET_LABEL_Y_SIZE 16
    #define WIDGET_X_CLEARENCE  16

    /**
     * @brief setup the app tile
     */
    void main_tile_setup( void );
    /**
     * @brief register an widget icon an the main tile
     * 
     * @return  pointer to lv_obj_t icon container, here you can set your own icon with imgbtn or NULL if failed
     */
    lv_obj_t *main_tile_register_widget( void );
    /**
     * @brief align all enabled widgets
     */
    void main_tile_align_widgets( void );
    /**
     * @brief get an free widget icon structure
     * 
     * @return a pointer to an free widget icon structure or NULL if failed
     */
    icon_t *main_tile_get_free_widget_icon( void );
    /**
     * @brief get the tile number for the main tile
     * 
     * @return  tile number
     */
    uint32_t main_tile_get_tile_num( void );
    /**
     * @brief update main tile time
     */
    void main_tile_update_time( void );

#endif // _MAIL_TILE_H