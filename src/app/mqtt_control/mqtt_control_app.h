/****************************************************************************
 *   June 14 02:01:00 2021
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
#ifndef _MQTT_CONTROL_H
    #define _MQTT_CONTROL_H

    #include <TTGO.h>
    #include "gui/icon.h"

    #define MQTT_CONTROL_ITEMS             20
    #define MQTT_CONTROL_JSON_CONFIG_FILE  "/mqtt_control.json"

    enum {
        MQTT_CONTROL_TYPE_NONE,
        MQTT_CONTROL_TYPE_LABEL,
        MQTT_CONTROL_TYPE_BUTTON,
        MQTT_CONTROL_TYPE_SWITCH
    };

    typedef struct {
        uint8_t type = MQTT_CONTROL_TYPE_NONE;
        char label[32]="";
        char topic[64]="";
        lv_obj_t *gui_label;
        lv_obj_t *gui_object;
    } mqtt_control_item_t;

    typedef struct {
        mqtt_control_item_t* items = NULL;
    } mqtt_control_config_t;

    void mqtt_control_app_setup( void );
    uint32_t mqtt_control_get_app_main_tile_num( void );

    void mqtt_control_app_set_indicator(icon_indicator_t indicator);
    void mqtt_control_app_hide_indicator();

    mqtt_control_config_t *mqtt_control_get_config( void );
    void mqtt_control_load_config( void );

#endif // _MQTT_CONTROL_H