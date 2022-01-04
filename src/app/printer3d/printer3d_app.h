/****************************************************************************
 *   January 04 19:00:00 2022
 *   Copyright  2021  Dirk Sarodnick
 *   Email: programmer@dirk-sarodnick.de
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
#ifndef _PRINTER3D_APP_H
    #define _PRINTER3D_APP_H

    #include "gui/icon.h"

    #define PRINTER3D_JSON_CONFIG_FILE        "/printer3d.json"

    typedef struct {
        char host[32] = "";
        uint16_t port = 8899;
    } printer3d_config_t;

//    #define PRINTER3D_WIDGET    // uncomment if an widget need

    void printer3d_app_setup( void );
    uint32_t printer3d_app_get_app_setup_tile_num( void );
    uint32_t printer3d_app_get_app_main_tile_num( void );

    void printer3d_app_set_indicator(icon_indicator_t indicator);
    void printer3d_app_hide_indicator();

    printer3d_config_t *printer3d_get_config( void );
    void printer3d_save_config( void );
    void printer3d_load_config( void );

#endif // _PRINTER3D_APP_H