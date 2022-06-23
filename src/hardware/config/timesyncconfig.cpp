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
#include "timesyncconfig.h"

timesync_config_t::timesync_config_t() : BaseJsonConfig(TIMESYNC_JSON_CONFIG_FILE) {
}

bool timesync_config_t::onSave(JsonDocument& doc) {
    doc["daylightsave"] = daylightsave;
    doc["timesync"] = timesync;
    doc["timezone"] = timezone;
    doc["use_24hr_clock"] = use_24hr_clock;
    doc["timezone_name"] = timezone_name;
    doc["timezone_rule"] = timezone_rule;

    return true;
}

bool timesync_config_t::onLoad(JsonDocument& doc) {
    daylightsave = doc["daylightsave"] | false;
    timesync = doc["timesync"] | true;
    timezone = doc["timezone"] | 0;
    use_24hr_clock = doc["use_24hr_clock"] | true;
    if( doc.containsKey("timezone_name") && doc.containsKey("timezone_rule") ) {
        strncpy( timezone_name, doc["timezone_name"], sizeof( timezone_name ) );
        strncpy( timezone_rule, doc["timezone_rule"], sizeof( timezone_rule ) );
    }
    else {
        strncpy( timezone_name, TIMEZONE_NAME_DEFAULT, sizeof( timezone_name ) );
        strncpy( timezone_rule, TIMEZONE_RULE_DEFAULT, sizeof( timezone_rule ) );
    }
    setenv("TZ", timezone_rule, 1);
    tzset();
    
    return true;
}

bool timesync_config_t::onDefault( void ) {
    daylightsave = false;
    timesync = true;
    timezone = 0;
    use_24hr_clock = true;
    strncpy( timezone_name, TIMEZONE_NAME_DEFAULT, sizeof( timezone_name ) );
    strncpy( timezone_rule, TIMEZONE_RULE_DEFAULT, sizeof( timezone_rule ) );
    setenv("TZ", timezone_rule, 1);
    tzset();
    return true;
}