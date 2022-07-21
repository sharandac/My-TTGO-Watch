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

#include "pmuconfig.h"

pmu_config_t::pmu_config_t() : BaseJsonConfig(PMU_JSON_CONFIG_FILE) {}

bool pmu_config_t::onSave(JsonDocument& doc) {
    doc["silence_wakeup"] = silence_wakeup;
    doc["silence_wakeup_interval"] = silence_wakeup_interval;
    doc["silence_wakeup_interval_vbplug"] = silence_wakeup_interval_vbplug;
    doc["experimental_power_save"] = experimental_power_save;
    doc["normal_voltage"] = normal_voltage;
    doc["normal_power_save_voltage"] = normal_power_save_voltage;
    doc["experimental_normal_voltage"] = experimental_normal_voltage;
    doc["experimental_power_save_voltage"] = experimental_power_save_voltage;
    doc["compute_percent"] = compute_percent;
    doc["high_charging_target_voltage"] = high_charging_target_voltage;
    doc["designed_battery_cap"] = designed_battery_cap;
    doc["pmu_logging"] = pmu_logging;
    doc["powermgm_resume_interval"] = powermgm_resume_interval;
    doc["battery_monitor_interval"] = battery_monitor_interval;
    doc["battery_voltage_lowest"] = battery_voltage_lowest;
    doc["battery_voltage_highest"] = battery_voltage_highest;
    doc["battery_chargingoffset"] = battery_chargingoffset;
    doc["battery_calibrated"] = battery_calibrated;

    return true;
}

bool pmu_config_t::onLoad(JsonDocument& doc) {
    silence_wakeup = doc["silence_wakeup"] | false;
    silence_wakeup_interval = doc["silence_wakeup_interval"] | SILENCEWAKEINTERVAL;
    silence_wakeup_interval_vbplug = doc["silence_wakeup_interval_vbplug"] | SILENCEWAKEINTERVAL_PLUG;
    experimental_power_save = doc["experimental_power_save"] | false;
    compute_percent = doc["compute_percent"] | false;
    high_charging_target_voltage = doc["high_charging_target_voltage"] | false;
    designed_battery_cap = doc["designed_battery_cap"] | 300;
    normal_voltage = doc["normal_voltage"] | NORMALVOLTAGE;
    normal_power_save_voltage = doc["normal_power_save_voltage"] | NORMALPOWERSAVEVOLTAGE;
    experimental_normal_voltage = doc["experimental_normal_voltage"] | EXPERIMENTALNORMALVOLTAGE;
    experimental_power_save_voltage = doc["experimental_power_save_voltage"] | EXPERIMENTALPOWERSAVEVOLTAGE;
    powermgm_resume_interval = doc["powermgm_resume_interval"] | POWERMGMRESUMEINTERVAL;
    battery_monitor_interval = doc["battery_monitor_interval"] | BATTERY_MONITOR_INTERVAL;
    battery_voltage_lowest = doc["battery_voltage_lowest"] | BATTERY_LOWEST;
    battery_voltage_highest = doc["battery_voltage_highest"] | BATTERY_HIGHEST;
    battery_chargingoffset = doc["battery_chargingoffset"] | BATTERY_CHARGING_OFFSET;
    battery_calibrated = doc["battery_calibrated"] | false;
    pmu_logging = doc["pmu_logging"] | false;
    
    return true;
}

bool pmu_config_t::onDefault( void ) {
    return true;
}