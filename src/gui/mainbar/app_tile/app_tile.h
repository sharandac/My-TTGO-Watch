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

    #define MAX_APPS_ICON_HORZ      3
    #define MAX_APPS_ICON_VERT      2
    #define MAX_APPS_ICON           ( MAX_APPS_ICON_HORZ * MAX_APPS_ICON_VERT )

    #define APP_ICON_X_SIZE         48
    #define APP_ICON_Y_SIZE         48
    #define APP_ICON_X_CLEARENCE    32
    #define APP_ICON_Y_CLEARENCE    32
    #define APP_LABEL_X_SIZE        APP_ICON_X_SIZE + APP_ICON_X_CLEARENCE
    #define APP_LABEL_Y_SIZE        APP_ICON_Y_CLEARENCE / 2

    #define APP_FIRST_X_POS         ( 240 - ( APP_ICON_X_SIZE * MAX_APPS_ICON_HORZ + APP_ICON_X_CLEARENCE * ( MAX_APPS_ICON_HORZ - 1 ) ) ) / 2
    #define APP_FIRST_Y_POS         ( 240 - ( APP_ICON_Y_SIZE * MAX_APPS_ICON_VERT + APP_ICON_Y_CLEARENCE * ( MAX_APPS_ICON_VERT - 1 ) ) ) / 2

    typedef struct {
        lv_obj_t *app;
        lv_obj_t *label;
        lv_coord_t x;
        lv_coord_t y;
        bool active;
    } lv_app_entry_t;

    void app_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );

#endif // _APP_TILE_H