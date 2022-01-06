/****************************************************************************
 *   Aug 11 17:13:51 2020
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
#include "powermeter_config.h"

powermeter_config_t::powermeter_config_t() : BaseJsonConfig( POWERMETER_JSON_CONFIG_FILE ) {}

bool powermeter_config_t::onSave(JsonDocument& doc) {
    doc["powermeter"]["server"] = server;
    doc["powermeter"]["port"] = port;
    doc["powermeter"]["ssl"] = ssl;
    doc["powermeter"]["user"] = user;
    doc["powermeter"]["password"] = password;
    doc["powermeter"]["topic"] = topic;
    doc["powermeter"]["autoconnect"] = autoconnect;
    doc["powermeter"]["widget"] = widget;

    return true;
}

bool powermeter_config_t::onLoad(JsonDocument& doc) {
    strncpy( server, doc["powermeter"]["server"] | "", sizeof( server ) );
    port = doc["powermeter"]["port"] | 1883;
    ssl = doc["powermeter"]["ssl"] | false;
    strncpy( user, doc["powermeter"]["user"] | "", sizeof( user ) );
    strncpy( password, doc["powermeter"]["password"] | "", sizeof( password ) );
    strncpy( topic, doc["powermeter"]["topic"] | "", sizeof( topic ) );
    autoconnect = doc["powermeter"]["autoconnect"] | false;
    widget = doc["powermeter"]["widget"] | false;
    
    return true;
}

bool powermeter_config_t::onDefault( void ) {
    strncpy( server, "", sizeof( server ) );
    port = 1883;
    ssl = false;
    strncpy( user, "", sizeof( user ) );
    strncpy( password, "", sizeof( password ) );
    strncpy( topic, "", sizeof( topic ) );
    autoconnect = false;
    widget = false;

    return true;
}