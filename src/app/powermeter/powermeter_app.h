/****************************************************************************
 *   Sep 3 23:05:42 2020
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
#ifndef _POWERMETER_APP_H
    #define _POWERMETER_APP_H

    #include "gui/icon.h"

    #define POWERMETER_JSON_CONFIG_FILE        "/powermeter.json"

    typedef struct {
        char server[64] = "";
        int32_t port = 1883;
        bool ssl = false;
        char user[32] = "";
        char password[32] = "";
        char topic[64] = "";
        bool autoconnect = false;
        bool widget = false;
    } powermeter_config_t;

    void powermeter_app_setup( void );
    uint32_t powermeter_get_app_main_tile_num( void );
    uint32_t powermeter_get_app_setup_tile_num( void );
    powermeter_config_t *powermeter_get_config( void );
    void powermeter_save_config( void );
    void powermeter_load_config( void );
    bool powermeter_add_widget( void );
    bool powermeter_remove_widget( void );
    icon_t *powermeter_get_app_icon( void );
    icon_t *powermeter_get_widget_icon( void );

#endif // _POWERMETER_APP_H