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
#include "gpsctlconfig.h"

gpsctl_config_t::gpsctl_config_t() : BaseJsonConfig( GPSCTL_JSON_CONFIG_FILE ) {
}

bool gpsctl_config_t::onSave(JsonDocument& doc) {
    doc["autoon"] = autoon;
    doc["enable_on_standby"] = enable_on_standby;
    doc["gps_over_ip"] = gps_over_ip;
    doc["app_use_gps"] = app_use_gps;
    doc["TXPin"] = TXPin;
    doc["RXPin"] = RXPin;

    return true;
}

bool gpsctl_config_t::onLoad(JsonDocument& doc) {
    autoon = doc["autoon"] | false;
    enable_on_standby = doc["enable_on_standby"] | false;
    gps_over_ip = doc["gps_over_ip"] | false;
    app_use_gps = doc["app_use_gps"] | false;
    TXPin = doc["TXPin"] | -1;
    RXPin = doc["RXPin"] | -1;

    return true;
}

bool gpsctl_config_t::onDefault( void ) {
    autoon = false;
    enable_on_standby = false;
    gps_over_ip = false;
    app_use_gps = false;
    TXPin = -1;
    RXPin = -1;

    return true;
}