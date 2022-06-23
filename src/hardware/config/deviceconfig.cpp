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
#include "deviceconfig.h"

device_config_t::device_config_t() : BaseJsonConfig( DEVICE_JSON_CONFIG_FILE ) {}

bool device_config_t::onSave(JsonDocument& doc) {
    doc["device_name"] = device_name;
    return true;
}

bool device_config_t::onLoad(JsonDocument& doc) {
    if ( doc.containsKey("device_name") )
        strncpy( device_name, doc["device_name"], sizeof( device_name ) );
    else
        strncpy( device_name, HARDWARE_NAME, sizeof( device_name ) );

    return true;
}

bool device_config_t::onDefault( void ) {
    return true;
}