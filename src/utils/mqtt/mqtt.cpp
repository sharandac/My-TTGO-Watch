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
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"
#include "utils/mqtt/mqtt.h"
#include "hardware/pmu.h"
#include "hardware/powermgm.h"

bool mqtt_setup = false;
bool mqtt_run = false;
bool mqtt_connected = false;
bool mqtt_needs_publish = false;
EventGroupHandle_t mqtt_status = NULL;
portMUX_TYPE DRAM_ATTR mqttMux = portMUX_INITIALIZER_UNLOCKED;
callback_t *mqtt_callback = NULL;
lv_task_t * mqtt_main_task;

WiFiClient espClient;
PubSubClient mqtt_client( espClient );
char clientId[24];

bool mqtt_send_event_cb( EventBits_t event, void *arg );
void mqtt_set_event( EventBits_t bits );
bool mqtt_get_event( EventBits_t bits );
void mqtt_clear_event( EventBits_t bits );
std::vector<MqttMessageCallback> _mqtt_message_callbacks;

void mqtt_message_event_cb(char* topic, byte* payload, unsigned int length) {
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
      mqtt_needs_publish = true;
      break;
  }
  return( true );
}

void mqtt_loop( lv_task_t * task ) {
  if (!mqtt_setup) return;

  if (!mqtt_connected && mqtt_client.connected()) {
    mqtt_connected = true;
    mqtt_needs_publish = true;
    mqtt_publish_state();

    mqtt_set_event( MQTTCTL_CONNECT );
    mqtt_clear_event( MQTTCTL_DISCONNECT );
    mqtt_send_event_cb( MQTTCTL_CONNECT, (void *)NULL );
  }
  
  if (mqtt_connected && !mqtt_client.connected()) {
    mqtt_connected = false;
    mqtt_set_event( MQTTCTL_DISCONNECT );
    mqtt_clear_event( MQTTCTL_CONNECT );
    mqtt_send_event_cb( MQTTCTL_DISCONNECT, (void *)NULL );
  }

  mqtt_client.loop();

  if (mqtt_run && mqtt_connected && mqtt_needs_publish) {
    mqtt_needs_publish = false;

    mqtt_publish_battery();
    mqtt_publish_ambient_temperature();
    mqtt_publish_power_temperature();

    if (!powermgm_get_event( POWERMGM_STANDBY )) {
        mqtt_publish_version();
        mqtt_publish_sketch();
        mqtt_publish_heap();
        mqtt_publish_psram();
    }
  }
}

void mqtt_init( void ) {
  mqtt_status = xEventGroupCreate();
  mqtt_set_event( MQTTCTL_OFF );
  mqtt_main_task = lv_task_create( mqtt_loop, 100, LV_TASK_PRIO_MID, NULL );
}

void mqtt_start() {
  if (!mqtt_setup) return;
  mqtt_run = true;

  if ( !mqtt_client.connected() ) {
    mqtt_client.connect( clientId );
  }
}

void mqtt_start( const char *id, bool ssl, const char *server, int32_t port, const char *user, const char *pass ) {
  if (mqtt_run) return;
  mqtt_run = true;

  if ( !mqtt_client.connected() ) {
    log_i("use mqtt server:port as %s: %s:%d", id, server, port );

    if (!mqtt_setup) mqtt_client.setCallback(mqtt_message_event_cb);

    char topic[64];
    strlcpy( clientId, id, strlen( id ) + 1 );
    snprintf(topic, sizeof(topic), "%s/state", clientId);
    
    mqtt_client.setServer( server, port );
    mqtt_client.connect( clientId, user, pass, topic, 1, true, "offline" );

    if (!mqtt_setup) pmu_register_cb( PMUCTL_STATUS, mqtt_pmuctl_event_cb, "mqtt pmu");
  }

  mqtt_setup = true;
}

void mqtt_stop( void ) {
  if (!mqtt_run) return;
  mqtt_run = false;

  if ( mqtt_setup && mqtt_client.connected() ) {
    log_i("stopping mqtt");

    mqtt_set_event( MQTTCTL_DISCONNECT );
    mqtt_clear_event( MQTTCTL_CONNECT | MQTTCTL_OFF );
    mqtt_send_event_cb( MQTTCTL_DISCONNECT, (void *)0 );
    
    mqtt_client.disconnect();
  }
}

void mqtt_subscribe(const char* topic) {
  log_d("subscribing to topic '%s'", topic);
  mqtt_client.subscribe(topic, 0);
}

void mqtt_unsubscribe(const char* topic) {
  log_d("subscribing to topic '%s'", topic);
  mqtt_client.unsubscribe(topic);
}

void mqtt_publish(const char* topic, bool retain, const char* payload) {
  mqtt_client.publish(topic, payload, retain);
}

void mqtt_publish_state() {
  char topic[64];
  snprintf(topic, sizeof(topic), "%s/state", clientId);
  mqtt_client.publish(topic, "online", true);
}

void mqtt_publish_battery() {
  int32_t voltage = pmu_get_battery_voltage();
  if (voltage > 3000) {
    char topic[64];
    snprintf(topic, sizeof(topic), "%s/battery", clientId);

    char payload[5];
    snprintf(payload, sizeof(payload), "%d", voltage);

    mqtt_client.publish(topic, payload, true);
  }
}

void mqtt_publish_version() {
    char topic[64];
    snprintf(topic, sizeof(topic), "%s/version", clientId);

  char payload[11];
  snprintf(payload, sizeof(payload), "%s", __FIRMWARE__);

  mqtt_client.publish(topic, payload, true);
}

void mqtt_publish_ambient_temperature() {
  TTGOClass * ttgo = TTGOClass::getWatch();

  char topic[64];
  snprintf(topic, sizeof(topic), "%s/temp_ambient", clientId);
  
  char payload[6];
  snprintf(payload, sizeof(payload), "%.2f", ttgo->bma->temperature());

  mqtt_client.publish(topic, payload, false);
}

void mqtt_publish_power_temperature() {
  TTGOClass * ttgo = TTGOClass::getWatch();

  char topic[64];
  snprintf(topic, sizeof(topic), "%s/temp_power", clientId);
  
  char payload[6];
  snprintf(payload, sizeof(payload), "%.2f", ttgo->power->getTemp());

  mqtt_client.publish(topic, payload, false);
}

void mqtt_publish_heap() {
  char topic[64];
  snprintf(topic, sizeof(topic), "%s/heap", clientId);
  
  char payload[22];
  snprintf(payload, sizeof(payload), "%d/%d", ESP.getFreeHeap(), ESP.getHeapSize());

  mqtt_client.publish(topic, payload, false);
}

void mqtt_publish_psram() {
  char topic[64];
  snprintf(topic, sizeof(topic), "%s/psram", clientId);
  
  char payload[22];
  snprintf(payload, sizeof(payload), "%d/%d", ESP.getFreePsram(), ESP.getPsramSize());

  mqtt_client.publish(topic, payload, false);
}

void mqtt_publish_sketch() {
  char topic[64];
  snprintf(topic, sizeof(topic), "%s/sketch", clientId);
  
  char payload[22];
  snprintf(payload, sizeof(payload), "%d/%d", ESP.getFreeSketchSpace(), ESP.getSketchSize());

  mqtt_client.publish(topic, payload, false);
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