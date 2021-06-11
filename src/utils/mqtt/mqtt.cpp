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

bool mqtt_setup = false;
bool mqtt_run = false;
EventGroupHandle_t mqtt_status = NULL;
portMUX_TYPE DRAM_ATTR mqttMux = portMUX_INITIALIZER_UNLOCKED;
callback_t *mqtt_callback = NULL;
lv_task_t * _mqtt_main_task;

AsyncMqttClient mqtt_client;
char stateTopic[32];
char batteryTopic[32];
char heapTopic[32];
char psramTopic[32];
char sketchTopic[32];
char versionTopic[32];
char ambientTemperatureTopic[32];
char powerTemperatureTopic[32];

bool mqtt_send_event_cb( EventBits_t event, void *arg );
void mqtt_set_event( EventBits_t bits );
bool mqtt_get_event( EventBits_t bits );
void mqtt_clear_event( EventBits_t bits );
std::vector<MqttMessageCallback> _mqtt_message_callbacks;

void _mqtt_connected(bool sessionPresent) {
  log_d("mqtt connected");

  mqtt_publish_state();
  mqtt_publish_battery();
  mqtt_publish_ambient_temperature();
  mqtt_publish_power_temperature();

  mqtt_publish_version();
  mqtt_publish_sketch();
  mqtt_publish_heap();
  mqtt_publish_psram();

  mqtt_set_event( MQTT_CONNECT );
  mqtt_clear_event( MQTT_DISCONNECT | MQTT_DISCONNECTED | MQTT_OFF );
  mqtt_send_event_cb( MQTT_CONNECT, (void *)sessionPresent );
}

void _mqtt_disconnected(AsyncMqttClientDisconnectReason reason) {
  log_d("mqtt disconnected, because %d", reason);

  mqtt_set_event( MQTT_DISCONNECT );
  mqtt_clear_event( MQTT_CONNECT | MQTT_CONNECTED | MQTT_OFF );
  mqtt_send_event_cb( MQTT_DISCONNECT, (void *)reason );
}

void _mqtt_subscribe(uint16_t packetId, uint8_t qos) {
  log_d("mqtt subscribe, package %d", packetId);
}

void _mqtt_unsubscribe(uint16_t packetId) {
  log_d("mqtt unsubscribe, package %d", packetId);
}

void _mqtt_message(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t length, size_t index, size_t total) {
  log_d("mqtt message: %s -> %s", topic, payload);
  for (auto callback : _mqtt_message_callbacks) callback(topic, payload, length);
}

bool mqtt_pmuctl_event_cb( EventBits_t event, void *arg ) {
  if ( !mqtt_client.connected() ) {
    log_e("mqtt not connected");
    return( true );
  }

  switch( event ) {
    case PMUCTL_STATUS:
      mqtt_publish_battery();
      mqtt_publish_ambient_temperature();
      mqtt_publish_power_temperature();

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

void mqtt_loop( lv_task_t * task ) {
  if (!mqtt_setup || !mqtt_run) return;

  if (mqtt_get_event( MQTT_CONNECT )) {
    mqtt_set_event( MQTT_CONNECTED );
    mqtt_clear_event( MQTT_CONNECT );
    mqtt_send_event_cb( MQTT_CONNECTED, (void *)NULL );
  }
  if (mqtt_get_event( MQTT_DISCONNECT )) {
    mqtt_set_event( MQTT_DISCONNECTED );
    mqtt_clear_event( MQTT_DISCONNECT );
    mqtt_send_event_cb( MQTT_DISCONNECTED, (void *)NULL );
  }
}

void mqtt_init( void ) {
  mqtt_status = xEventGroupCreate();
  mqtt_set_event( MQTT_OFF );
  _mqtt_main_task = lv_task_create( mqtt_loop, 250, LV_TASK_PRIO_MID, NULL );
}

void mqtt_start( const char *id, bool ssl, const char *server, int32_t port, const char *user, const char *pass ) {
  if (mqtt_run) return;
  mqtt_run = true;

  if ( !mqtt_client.connected() ) {
    log_i("use mqtt server:port as %s: %s:%d", id, server, port );

    if (!mqtt_setup) snprintf(stateTopic, sizeof(stateTopic), "%s/state", id);
    if (!mqtt_setup) snprintf(batteryTopic, sizeof(batteryTopic), "%s/battery", id);
    if (!mqtt_setup) snprintf(heapTopic, sizeof(heapTopic), "%s/heap", id);
    if (!mqtt_setup) snprintf(psramTopic, sizeof(psramTopic), "%s/psram", id);
    if (!mqtt_setup) snprintf(sketchTopic, sizeof(sketchTopic), "%s/sketch", id);
    if (!mqtt_setup) snprintf(versionTopic, sizeof(versionTopic), "%s/version", id);
    if (!mqtt_setup) snprintf(ambientTemperatureTopic, sizeof(ambientTemperatureTopic), "%s/temp_ambient", id);
    if (!mqtt_setup) snprintf(powerTemperatureTopic, sizeof(powerTemperatureTopic), "%s/temp_power", id);

    mqtt_client.setWill(stateTopic, 1, true, "offline");
    if (!mqtt_setup) mqtt_client.onConnect(_mqtt_connected);
    if (!mqtt_setup) mqtt_client.onDisconnect(_mqtt_disconnected);
    if (!mqtt_setup) mqtt_client.onSubscribe(_mqtt_subscribe);
    if (!mqtt_setup) mqtt_client.onUnsubscribe(_mqtt_unsubscribe);
    if (!mqtt_setup) mqtt_client.onMessage(_mqtt_message);

    mqtt_client.setClientId( id );
    mqtt_client.setServer( server, port );
    mqtt_client.setCredentials( user, pass );
    mqtt_client.connect();

    if (!mqtt_setup) pmu_register_cb( PMUCTL_STATUS, mqtt_pmuctl_event_cb, "mqtt pmu");
  }

  mqtt_setup = true;
}

void mqtt_stop( void ) {
  if (!mqtt_run) return;
  mqtt_run = false;

  if ( mqtt_setup && mqtt_client.connected() ) {
    log_i("stopping mqtt");

    mqtt_set_event( MQTT_DISCONNECT );
    mqtt_clear_event( MQTT_CONNECT | MQTT_OFF );
    mqtt_send_event_cb( MQTT_DISCONNECT, (void *)0 );
    
    mqtt_client.disconnect(true);
  }
}

void mqtt_subscribe(const char* topic) {
  log_d("subscribing to topic '%s'", topic);
  mqtt_client.subscribe(topic, 0);
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

void mqtt_publish_ambient_temperature() {
  char temp[6];
  TTGOClass * ttgo = TTGOClass::getWatch();
  snprintf(temp, sizeof(temp), "%.2f", ttgo->bma->temperature());
  mqtt_client.publish(ambientTemperatureTopic, 0, false, temp);
}

void mqtt_publish_power_temperature() {
  char temp[6];
  TTGOClass * ttgo = TTGOClass::getWatch();
  snprintf(temp, sizeof(temp), "%.2f", ttgo->power->getTemp());
  mqtt_client.publish(powerTemperatureTopic, 0, false, temp);
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

void mqtt_set_event( EventBits_t bits ) {
  /*
   * check if init
   */
  if ( mqtt_setup == false ) {
    log_e("mqtt not set up");
    return;
  }

  portENTER_CRITICAL(&mqttMux);
  xEventGroupSetBits( mqtt_status, bits );
  portEXIT_CRITICAL(&mqttMux);
}

void mqtt_clear_event( EventBits_t bits ) {
  /*
   * check if init
   */
  if ( mqtt_setup == false ) {
    log_e("mqtt not set up");
    return;
  }

  portENTER_CRITICAL(&mqttMux);
  xEventGroupClearBits( mqtt_status, bits );
  portEXIT_CRITICAL(&mqttMux);
}

bool mqtt_get_event( EventBits_t bits ) {
  bool retval = false;
  
  /*
   * check if init
   */
  if ( mqtt_setup == false ) {
    log_e("mqtt not set up");
    return( retval );
  }
  
  portENTER_CRITICAL(&mqttMux);
  EventBits_t temp = xEventGroupGetBits( mqtt_status ) & bits;
  portEXIT_CRITICAL(&mqttMux);
  if ( temp )
    retval = true;

  return( retval );
}

bool mqtt_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
  /*
    * check if an callback table exist, if not allocate a callback table
    */
  if ( mqtt_callback == NULL ) {
      mqtt_callback = callback_init( "mqtt" );
      if ( mqtt_callback == NULL ) {
          log_e("mqtt callback alloc failed");
          while(true);
      }
  }
  /*
    * register an callback entry and return them
    */
  return( callback_register( mqtt_callback, event, callback_func, id ) );
}

bool mqtt_send_event_cb( EventBits_t event, void *arg ) {
  /*
    * call all callbacks with her event mask
    */
  return( callback_send( mqtt_callback, event, arg ) );
}

void mqtt_register_message_cb(MqttMessageCallback callback) {
  _mqtt_message_callbacks.push_back(callback);
}