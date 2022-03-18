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
#include "kodi_remote_config.h"

kodi_remote_config_t::kodi_remote_config_t() : BaseJsonConfig( KODI_REMOTE_JSON_CONFIG_FILE ) {}

bool kodi_remote_config_t::onSave(JsonDocument& doc) {
    doc["host"] = host;
    doc["user"] = user;
    doc["pass"] = pass;
    doc["port"] = port;

    return true;
}

bool kodi_remote_config_t::onLoad(JsonDocument& doc) {
    strncpy( host, doc["host"], sizeof( host ) );
    strncpy( user, doc["user"], sizeof( user ) );
    strncpy( pass, doc["pass"], sizeof( pass ) );
    port = (uint16_t)doc["port"];

    return true;
}

bool kodi_remote_config_t::onDefault() {
    return true;
}