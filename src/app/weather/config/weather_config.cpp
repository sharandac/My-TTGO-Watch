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
#include "weather_config.h"

weather_config_t::weather_config_t() : BaseJsonConfig( WEATHER_JSON_CONFIG_FILE ) {
}

bool weather_config_t::onSave(JsonDocument& doc) {

    doc["apikey"] = apikey;
    doc["lat"] = lat;
    doc["lon"] = lon;
    doc["autosync"] = autosync;
    doc["showWind"] = showWind;
    doc["imperial"] = imperial;
    doc["widget"] = widget;

    return true;
}

bool weather_config_t::onLoad(JsonDocument& doc) {

    strncpy( apikey, doc["apikey"], sizeof( apikey ) );
    strncpy( lat, doc["lat"], sizeof( lat ) );
    strncpy( lon, doc["lon"], sizeof( lon ) );
    autosync = doc["autosync"] | true;
    showWind = doc["showWind"] | false;
    imperial = doc["imperial"] | false;
    widget = doc["widget"] | true;

    return true;
}

bool weather_config_t::onDefault( void ) {
    return true;
}