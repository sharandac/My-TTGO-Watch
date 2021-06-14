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
#ifndef _MQTT_PLAYER_H
    #define _MQTT_PLAYER_H

    #include <TTGO.h>
    #include "gui/icon.h"

    #define MQTT_PLAYER_JSON_CONFIG_FILE        "/mqtt_player.json"

    typedef struct {
        char topic_base[32] = "";
        
        char topic_state[32] = "";
        char topic_artist[32] = "";
        char topic_title[32] = "";

        char topic_cmd_play[32] = "";
        char topic_cmd_pause[32] = "";
        char topic_cmd_next[32] = "";
        char topic_cmd_prev[32] = "";
        char topic_cmd_volumeup[32] = "";
        char topic_cmd_volumedown[32] = "";
    } mqtt_player_config_t;

    void mqtt_player_app_setup( void );
    uint32_t mqtt_player_get_app_setup_tile_num( void );
    uint32_t mqtt_player_get_app_main_tile_num( void );

    void mqtt_player_app_set_indicator(icon_indicator_t indicator);
    void mqtt_player_app_hide_indicator();

    mqtt_player_config_t *mqtt_player_get_config( void );
    void mqtt_player_save_config( void );
    void mqtt_player_load_config( void );

#endif // _MQTT_PLAYER_H