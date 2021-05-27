/****************************************************************************
 *   Mo May 23 00:08:51 2021
 *   Copyright  2021  Dirk Sarodnick
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
#include <Arduino.h>
#include <AsyncMqttClient.h>

#include "config.h"
#include "utils/mqtt/mqtt.h"
#include "hardware/pmu.h"
#include "hardware/powermgm.h"

bool mqtt_init = false;
bool mqtt_run = false;
AsyncMqttClient mqtt_client;
char stateTopic[32];
char batteryTopic[32];
char heapTopic[32];
char psramTopic[32];
char sketchTopic[32];
char versionTopic[32];
char temperatureTopic[32];

void _mqtt_connected(bool sessionPresent) {
    mqtt_publish_state();
    mqtt_publish_battery();
    mqtt_publish_temperature();

    mqtt_publish_version();
    mqtt_publish_sketch();
    mqtt_publish_heap();
    mqtt_publish_psram();
}

bool mqtt_pmuctl_event_cb( EventBits_t event, void *arg ) {
    if ( !mqtt_client.connected() ) {
        log_e("mqtt not connected");
        return( true );
    }

    switch( event ) {
        case PMUCTL_STATUS:
            mqtt_publish_battery();
            mqtt_publish_temperature();

            if (!powermgm_get_event( POWERMGM_STANDBY )) {
                mqtt_publish_version();
                mqtt_publish_sketch();
                mqtt_publish_heap();
                mqtt_publish_psram();
            }
            break;
    }
    return( true );
}

void mqtt_start( const char *id, bool ssl, const char *server, int32_t port, const char *user, const char *pass ) {
    if (mqtt_run) return;
    mqtt_run = true;

    if ( !mqtt_client.connected() ) {
        log_i("use mqtt server:port as %s: %s:%d", id, server, port );

        if (!mqtt_init) snprintf(stateTopic, sizeof(stateTopic), "%s/state", id);
        if (!mqtt_init) snprintf(batteryTopic, sizeof(batteryTopic), "%s/battery", id);
        if (!mqtt_init) snprintf(heapTopic, sizeof(heapTopic), "%s/heap", id);
        if (!mqtt_init) snprintf(psramTopic, sizeof(psramTopic), "%s/psram", id);
        if (!mqtt_init) snprintf(sketchTopic, sizeof(sketchTopic), "%s/sketch", id);
        if (!mqtt_init) snprintf(versionTopic, sizeof(versionTopic), "%s/version", id);
        if (!mqtt_init) snprintf(temperatureTopic, sizeof(temperatureTopic), "%s/temp", id);

        mqtt_client.setWill(stateTopic, 1, true, "offline");
        if (!mqtt_init) mqtt_client.onConnect(_mqtt_connected);

        mqtt_client.setClientId( id );
        mqtt_client.setServer( server, port );
        mqtt_client.setCredentials( user, pass );
        mqtt_client.connect();

        if (!mqtt_init) pmu_register_cb( PMUCTL_STATUS, mqtt_pmuctl_event_cb, "mqtt pmu");
    }

    mqtt_init = true;
}

void mqtt_stop() {
    if (!mqtt_run) return;
    mqtt_run = false;

    if ( mqtt_client.connected() ) {
        log_i("stop mqtt");
        mqtt_client.disconnect(true);
    }
}

void mqtt_publish_state() {
    mqtt_client.publish(stateTopic, 1, true, "online");
}

void mqtt_publish_battery() {
    int32_t voltage = pmu_get_battery_voltage();
    if (voltage > 3000) {
        char temp[5];
        snprintf(temp, sizeof(temp), "%d", voltage);
        mqtt_client.publish(batteryTopic, 0, true, temp);
    }
}

void mqtt_publish_version() {
    char temp[11];
    snprintf(temp, sizeof(temp), "%s", __FIRMWARE__);
    mqtt_client.publish(versionTopic, 0, true, temp);
}

void mqtt_publish_temperature() {
    char temp[6];
    TTGOClass * ttgo = TTGOClass::getWatch();
    snprintf(temp, sizeof(temp), "%.2f", ttgo->power->getTemp());
    mqtt_client.publish(temperatureTopic, 0, false, temp);
}

void mqtt_publish_heap() {
    char temp[22];
    snprintf(temp, sizeof(temp), "%d/%d", ESP.getFreeHeap(), ESP.getHeapSize());
    mqtt_client.publish(heapTopic, 0, false, temp);
}

void mqtt_publish_psram() {
    char temp[22];
    snprintf(temp, sizeof(temp), "%d/%d", ESP.getFreePsram(), ESP.getPsramSize());
    mqtt_client.publish(psramTopic, 0, false, temp);
}

void mqtt_publish_sketch() {
    char temp[22];
    snprintf(temp, sizeof(temp), "%d/%d", ESP.getFreeSketchSpace(), ESP.getSketchSize());
    mqtt_client.publish(sketchTopic, 0, false, temp);
}

bool mqtt_get_connected() {
    return mqtt_client.connected();
}

AsyncMqttClient mqtt_get_client() {
    return mqtt_client;
}