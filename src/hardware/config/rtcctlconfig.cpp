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

#include "rtcctlconfig.h"

rtcctl_alarm_t::rtcctl_alarm_t() : BaseJsonConfig( CONFIG_FILE_PATH ) {
    enabled = false;
    hour = 0;
    minute = 0;
    for (int i = 0 ; i < DAYS_IN_WEEK ; i++)
    week_days[i] = false;
}

bool rtcctl_alarm_t::onSave(JsonDocument& doc) {
    doc[VERSION_KEY] = 1;
    doc[ENABLED_KEY] = enabled;
    doc[HOUR_KEY] = hour;
    doc[MINUTE_KEY] = minute;

    uint8_t week_days_to_store = 0;
    for (int index = 0; index < DAYS_IN_WEEK; ++index){
        week_days_to_store |= week_days[index] << index; 
    }
    doc[WEEK_DAYS_KEY] = week_days_to_store;
    
    return true;
}

bool rtcctl_alarm_t::onLoad(JsonDocument& doc) {
    enabled = doc[ENABLED_KEY].as<bool>();
    hour = doc[HOUR_KEY].as<uint8_t>();
    minute =  doc[MINUTE_KEY].as<uint8_t>();
    uint8_t stored_week_days = doc[WEEK_DAYS_KEY].as<uint8_t>();
    for (int index = 0; index < DAYS_IN_WEEK; ++index){
        week_days[index] = ((stored_week_days >> index) & 1) != 0;
    }

    return true;
}

bool rtcctl_alarm_t::onDefault( void ) {
    return true;
}