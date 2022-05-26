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
#include "tracker_config.h"

tracker_config_t::tracker_config_t() : BaseJsonConfig( TRACKER_JSON_COFIG_FILE ) {
}

bool tracker_config_t::onSave(JsonDocument& doc) {
    doc["gps_autoon"] = gps_autoon;
    doc["interval"] = interval;
    doc["gps_on_standby"] = gps_on_standby;
    doc["track_on_fix"] = track_on_fix;
    doc["vibe_on_fix"] = vibe_on_fix;
    doc["piep_on_fix"] = piep_on_fix;
    doc["storage"] = storage;
    return true;
}

bool tracker_config_t::onLoad(JsonDocument& doc) {
    gps_autoon = doc["gps_autoon"] | true;
    interval = doc["interval"] | 10;
    gps_on_standby = doc["gps_on_standby"] | false;
    track_on_fix = doc["track_on_fix"] | true;
    vibe_on_fix = doc["vibe_on_fix"] | false;
    piep_on_fix = doc["piep_on_fix"] | false;
    strncpy( storage, doc["storage"] | "/spiffs", sizeof( storage ) );
    return true;
}

bool tracker_config_t::onDefault( void ) {
    gps_autoon = true;
    interval = 10;
    gps_on_standby = false;
    track_on_fix = true;
    vibe_on_fix = false;
    piep_on_fix = false;
    strncpy( storage, "/spiffs", sizeof( storage ) );
    return true;
}