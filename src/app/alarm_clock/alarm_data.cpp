/****************************************************************************
 *   Copyright  2020  Jakub Vesely
 *   Email: jakub_vesely@seznam.cz
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

#include "alarm_data.h"
#include <hardware/rtcctl.h>
#include <SPIFFS.h>
#include "hardware/json_psram_allocator.h"

#define CONFIG_FILE_PATH "/alarm.json"

static bool vibe = 1;
static bool fade = 1;

static void load_data(){
    if (! SPIFFS.exists( CONFIG_FILE_PATH ) ) {
        return; //wil be used default values set during theier creation
    }

    fs::File file = SPIFFS.open( CONFIG_FILE_PATH, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", CONFIG_FILE_PATH );
        return;
    }

    int filesize = file.size();
    SpiRamJsonDocument doc( filesize * 2 );

    DeserializationError error = deserializeJson( doc, file );
    if ( error ) {
        log_e("update check deserializeJson() failed: %s", error.c_str() );
        return;
    }

    //vibe and fade are feature of alarm and is set directly
    //enabled, hour and minute are features of rtc and are set via registered event
    vibe = doc["vibe"].as<bool>();
    fade = doc["fade"].as<bool>();
    alarm_set_term(doc["hour"].as<int>(), doc["minute"].as<int>());
    alarm_set_enabled(doc["enabled"].as<bool>());

    doc.clear();
    file.close();
}

void alarm_data_store_data(bool enabled){
    if ( SPIFFS.exists( CONFIG_FILE_PATH ) ) {
        SPIFFS.remove( CONFIG_FILE_PATH );
        log_i("remove old binary weather config");
    }

    fs::File file = SPIFFS.open( CONFIG_FILE_PATH, FILE_WRITE );
    if (!file) {
        log_e("Can't open file: %s!", CONFIG_FILE_PATH );
        return;
    }

    SpiRamJsonDocument doc( 1000 );

    doc["version"] = 1;
    doc["vibe"] = vibe;
    doc["fade"] = fade;
    doc["hour"] = alarm_get_hour();
    doc["minute"] = alarm_get_minute();

    //FIXME: Workaround: alarm have to be disabled when it is stored. When was enabled and the alarm time was now it caused crashes (opening the file crashed)
    doc["enabled"] = enabled; //alarm_is_enabled();

    if ( serializeJsonPretty( doc, file ) == 0) {
        log_e("Failed to write config file");
    }

    doc.clear();
    file.close();
}

void alarm_set_term(uint8_t hour, uint8_t minute){
    log_i("alarm_set term %d:%.2d", hour, minute);
    rtcctl_set_alarm_term( hour, minute );
}

uint8_t alarm_get_hour(){
    return rtcctl_get_alarm_hour();
}

uint8_t alarm_get_minute(){
    return rtcctl_get_alarm_minute();
}

void alarm_set_enabled(bool enable){
    if (enable){
        rtcctl_enable_alarm();
    }
    else{
        rtcctl_disable_alarm();
    }
}

bool alarm_is_enabled(){
    return rtcctl_is_alarm_enabled();
}

void alarm_set_vibe_allowed(bool _vibe){
    vibe = _vibe;
}

bool alarm_is_vibe_allowed(){
    return vibe;
}

void alarm_set_fade_allowed(bool _fade){
    fade = _fade;
}

bool alarm_is_fade_allowed(){
    return fade;
}

void alarm_setup(){
    load_data();
}

bool alarm_is_time()
{
    return rtcctl_is_alarm_time();
}
