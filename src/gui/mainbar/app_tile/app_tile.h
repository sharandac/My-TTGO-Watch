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
#ifndef _APP_TILE_H
    #define _APP_TILE_H

    #include <TTGO.h>
    #include "gui/icon.h"

    #define MAX_APPS_ICON_HORZ      3
    #define MAX_APPS_ICON_VERT      2
    #define MAX_APPS_TILES          2
    #define MAX_APPS_ICON           ( MAX_APPS_ICON_HORZ * MAX_APPS_ICON_VERT * MAX_APPS_TILES )

    #define APP_ICON_X_SIZE         64
    #define APP_ICON_Y_SIZE         64
    #define APP_ICON_X_CLEARENCE    8
    #define APP_ICON_Y_CLEARENCE    36
    #define APP_LABEL_X_SIZE        APP_ICON_X_SIZE + APP_ICON_X_CLEARENCE
    #define APP_LABEL_Y_SIZE        APP_ICON_Y_CLEARENCE / 2

    #define APP_FIRST_X_POS         ( 240 - ( APP_ICON_X_SIZE * MAX_APPS_ICON_HORZ + APP_ICON_X_CLEARENCE * ( MAX_APPS_ICON_HORZ - 1 ) ) ) / 2
    #define APP_FIRST_Y_POS         ( 240 - ( APP_ICON_Y_SIZE * MAX_APPS_ICON_VERT + APP_ICON_Y_CLEARENCE * ( MAX_APPS_ICON_VERT - 1 ) ) ) / 2

    /**
     * @brief setup the app tile
     */
    void app_tile_setup( void );
    /**
     * @brief register an app icon an the app tile
     * 
     * @param   appname     app name
     * 
     * @return  pointer to lv_obj_t icon container, here you can set your own icon with imgbtn
     */
    lv_obj_t *app_tile_register_app( const char* appname );
    /**
     * @brief   get the an free app icon stucture
     * 
     * @return  pointer to the free lv_app_icon_t structure
     */
    icon_t *app_tile_get_free_app_icon( void );
    /**
     * @brief get the tile number for the app tile
     * 
     * @return  tile number
     */
    uint32_t app_tile_get_tile_num( void );

#endif // _APP_TILE_H