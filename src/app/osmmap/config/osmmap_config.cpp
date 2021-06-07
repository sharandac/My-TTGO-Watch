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
#include "osmmap_config.h"

osmmap_config_t::osmmap_config_t() : BaseJsonConfig( OSMMAP_JSON_COFIG_FILE ) {
}

bool osmmap_config_t::onSave(JsonDocument& doc) {
    doc["gps_autoon"] = gps_autoon;
    doc["gps_on_standby"] = gps_on_standby;
    doc["wifi_autoon"] = wifi_autoon;
    doc["load_ahead"] = load_ahead;
    doc["left_right_hand"] = left_right_hand;
    doc["osmmap"] = osmmap;
    return true;
}

bool osmmap_config_t::onLoad(JsonDocument& doc) {
    gps_autoon = doc["gps_autoon"] | true;
    gps_on_standby = doc["gps_on_standby"] | false;
    wifi_autoon = doc["wifi_autoon"] | true;
    load_ahead = doc["load_ahead"] | false;
    left_right_hand = doc["left_right_hand"] | false;
    strncpy( osmmap, doc["osmmap"] | "OSM Standard", sizeof( osmmap ) );
    return true;
}

bool osmmap_config_t::onDefault( void ) {
    gps_autoon = true;
    gps_on_standby = false;
    wifi_autoon = true;
    load_ahead = false;
    left_right_hand = false;
    strncpy( osmmap, "OSM Standard", sizeof( osmmap ) );
    return true;
}