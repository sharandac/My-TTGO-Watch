/****************************************************************************
 *   Mo May 23 00:08:51 2021
 *   Copyright  2021  Dirk Sarodnick
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

#ifndef _MQTT_H
    #define _MQTT_H
    
    #include <AsyncMqttClient.h>
    #include "stdint.h"

    /**
     *  @brief setup builtin mqtt, call after first wifi-connection.
     */
    void mqtt_start( const char *id, bool ssl, const char *server, int32_t port, const char *user, const char *pass );

    /**
     *  @brief stop builtin mqtt.
     */
    void mqtt_stop();

    bool mqtt_get_connected();
    AsyncMqttClient mqtt_get_client();

#endif // _MQTT_H