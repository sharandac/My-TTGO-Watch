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
#include "config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_wps.h>

#include "wifictl.h"
#include "json_psram_allocator.h"

#include "gui/statusbar.h"
#include "webserver/webserver.h"

bool wifi_init = false;
EventGroupHandle_t wifictl_status = NULL;
portMUX_TYPE wifictlMux = portMUX_INITIALIZER_UNLOCKED;

wifictl_event_cb_t *wifictl_event_cb_table = NULL;
uint32_t wifictl_event_cb_entrys = 0;
void wifictl_send_event_cb( EventBits_t event, char *msg );

void wifictl_StartTask( void );
void wifictl_Task( void * pvParameters );
TaskHandle_t _wifictl_Task;

char *wifiname=NULL;
char *wifipassword=NULL;

static networklist *wifictl_networklist = NULL;
wifictl_config_t wifictl_config;

static esp_wps_config_t esp_wps_config;

void wifictl_send_event_cb( EventBits_t event, char *msg );
void wifictl_set_event( EventBits_t bits );
bool wifictl_get_event( EventBits_t bits );
void wifictl_clear_event( EventBits_t bits );
void wifictl_save_network( void );
void wifictl_load_network( void );
void wifictl_save_config( void );
void wifictl_load_config( void );
void wifictl_Task( void * pvParameters );

void wifictl_setup( void ) {
    if ( wifi_init == true )
        return;

    wifictl_status = xEventGroupCreate();

    wifi_init = true;

    wifictl_networklist = (networklist*)ps_calloc( sizeof( networklist ) * NETWORKLIST_ENTRYS, 1 );
    if( !wifictl_networklist ) {
      log_e("wifictl_networklist calloc faild");
      while(true);
    }

    // clean network list table
    for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
      wifictl_networklist[ entry ].ssid[ 0 ] = '\0';
      wifictl_networklist[ entry ].password[ 0 ] = '\0';
    }

    // load config from spiff
    wifictl_load_config();

    // register WiFi events
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifictl_set_event( WIFICTL_ACTIVE );
        wifictl_clear_event( WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST | WIFICTL_SCAN | WIFICTL_CONNECT );
        if ( wifictl_get_event( WIFICTL_WPS_REQUEST ) )
          wifictl_send_event_cb( WIFICTL_DISCONNECT, (char *)"wait for WPS" );
        else {
          wifictl_set_event( WIFICTL_SCAN );
          wifictl_send_event_cb( WIFICTL_DISCONNECT, (char *)"scan ..." );
          WiFi.scanNetworks();
        }
    }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifictl_set_event( WIFICTL_ACTIVE );
        wifictl_clear_event( WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST | WIFICTL_SCAN | WIFICTL_CONNECT | WIFICTL_WPS_REQUEST );
        int len = WiFi.scanComplete();
        for( int i = 0 ; i < len ; i++ ) {
          for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
            if ( !strcmp( wifictl_networklist[ entry ].ssid,  WiFi.SSID(i).c_str() ) ) {
              wifiname = wifictl_networklist[ entry ].ssid;
              wifipassword = wifictl_networklist[ entry ].password;
              wifictl_send_event_cb( WIFICTL_SCAN, (char *)"connecting ..." );
              WiFi.begin( wifiname, wifipassword );
              return;
            }
          }
        }
    }, WiFiEvent_t::SYSTEM_EVENT_SCAN_DONE );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifictl_set_event( WIFICTL_CONNECT | WIFICTL_ACTIVE );
        if ( wifictl_get_event( WIFICTL_WPS_REQUEST ) ) {
          log_i("store new SSID and psk from WPS");
          wifictl_insert_network( WiFi.SSID().c_str(), WiFi.psk().c_str() );
          wifictl_save_config();
        }
        wifictl_clear_event( WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST | WIFICTL_SCAN | WIFICTL_WPS_REQUEST  );
        wifictl_send_event_cb( WIFICTL_CONNECT, (char*)WiFi.SSID().c_str() );
        wifictl_send_event_cb( WIFICTL_CONNECT_IP, (char*)WiFi.localIP().toString().c_str() );
        if ( wifictl_config.webserver ) {
          asyncwebserver_start();
        }
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifictl_set_event( WIFICTL_ACTIVE );
        wifictl_clear_event( WIFICTL_CONNECT | WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST );
        if ( wifictl_get_event( WIFICTL_WPS_REQUEST ) )
          wifictl_send_event_cb( WIFICTL_ON, (char *)"wait for WPS" );
        else {
          wifictl_set_event( WIFICTL_SCAN );
          wifictl_send_event_cb( WIFICTL_ON, (char *)"scan ..." );
          WiFi.scanNetworks();
        }
    }, WiFiEvent_t::SYSTEM_EVENT_WIFI_READY );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        asyncwebserver_end();
        wifictl_clear_event( WIFICTL_ACTIVE | WIFICTL_CONNECT | WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST | WIFICTL_SCAN | WIFICTL_WPS_REQUEST );
        wifictl_send_event_cb( WIFICTL_OFF, (char *)"" );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_STOP );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
      esp_wifi_wps_disable();
      WiFi.begin();
      wifictl_send_event_cb( WIFICTL_WPS_SUCCESS, (char *)"wps success" );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_WPS_ER_SUCCESS );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
      esp_wifi_wps_disable();
      wifictl_send_event_cb( WIFICTL_WPS_SUCCESS, (char *)"wps failed" );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_WPS_ER_FAILED );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
      esp_wifi_wps_disable();
      wifictl_send_event_cb( WIFICTL_WPS_SUCCESS, (char *)"wps timeout" );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_WPS_ER_TIMEOUT );

    xTaskCreate(  wifictl_Task,    /* Function to implement the task */
                  "wifictl Task",       /* Name of the task */
                  3000,                  /* Stack size in words */
                  NULL,                   /* Task input parameter */
                  1,                      /* Priority of the task */
                  &_wifictl_Task );       /* Task handle. */
    vTaskSuspend( _wifictl_Task );
}

void wifictl_save_config( void ) {
    if ( SPIFFS.exists( WIFICTL_CONFIG_FILE ) ) {
        SPIFFS.remove( WIFICTL_CONFIG_FILE );
        log_i("remove old binary wificfg config");
    }
    if ( SPIFFS.exists( WIFICTL_LIST_FILE ) ) {
        SPIFFS.remove( WIFICTL_LIST_FILE );
        log_i("remove old binary wifilist config");
    }

    fs::File file = SPIFFS.open( WIFICTL_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", WIFICTL_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 10000 );

        doc["autoon"] = wifictl_config.autoon;
        doc["webserver"] = wifictl_config.webserver;
        for ( int i = 0 ; i < NETWORKLIST_ENTRYS ; i++ ) {
            doc["networklist"][ i ]["ssid"] = wifictl_networklist[ i ].ssid;
            doc["networklist"][ i ]["psk"] = wifictl_networklist[ i ].password;
        }

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void wifictl_load_config( void ) {
    if ( SPIFFS.exists( WIFICTL_JSON_CONFIG_FILE ) ) {        
        fs::File file = SPIFFS.open( WIFICTL_JSON_CONFIG_FILE, FILE_READ );
        if (!file) {
            log_e("Can't open file: %s!", WIFICTL_JSON_CONFIG_FILE );
        }
        else {
            int filesize = file.size();
            SpiRamJsonDocument doc( filesize * 2 );

            DeserializationError error = deserializeJson( doc, file );
            if ( error ) {
                log_e("update check deserializeJson() failed: %s", error.c_str() );
            }
            else {
                wifictl_config.autoon = doc["autoon"].as<bool>();
                wifictl_config.webserver = doc["webserver"].as<bool>();
                for ( int i = 0 ; i < NETWORKLIST_ENTRYS ; i++ ) {
                    strlcpy( wifictl_networklist[ i ].ssid    , doc["networklist"][ i ]["ssid"], sizeof( wifictl_networklist[ i ].ssid ) );
                    strlcpy( wifictl_networklist[ i ].password, doc["networklist"][ i ]["psk"], sizeof( wifictl_networklist[ i ].password ) );
                }
            }        
            doc.clear();
        }
        file.close();
    }
    else {
        log_i("no json config exists, read from binary");

        wifictl_load_network();

        fs::File file = SPIFFS.open( WIFICTL_CONFIG_FILE, FILE_READ );

        if (!file) {
            log_e("Can't open file: %s!", WIFICTL_CONFIG_FILE );
        }
        else {
            int filesize = file.size();
            if ( filesize > sizeof( wifictl_config ) ) {
                log_e("Failed to read configfile. Wrong filesize!" );
            }
            else {
                file.read( (uint8_t *)&wifictl_config, filesize );
                file.close();
                wifictl_save_config();
                return;
            }
        file.close();
        }
    }
}

bool wifictl_get_autoon( void ) {
  return( wifictl_config.autoon );
}

void wifictl_set_autoon( bool autoon ) {
  wifictl_config.autoon = autoon;
  wifictl_save_config();
}

bool wifictl_get_webserver( void ) {
  return( wifictl_config.webserver );
}

void wifictl_set_webserver( bool webserver ) {
  wifictl_config.webserver = webserver;
  wifictl_save_config();
}

void wifictl_set_event( EventBits_t bits ) {
    portENTER_CRITICAL(&wifictlMux);
    xEventGroupSetBits( wifictl_status, bits );
    portEXIT_CRITICAL(&wifictlMux);
}

void wifictl_clear_event( EventBits_t bits ) {
    portENTER_CRITICAL(&wifictlMux);
    xEventGroupClearBits( wifictl_status, bits );
    portEXIT_CRITICAL(&wifictlMux);
}

bool wifictl_get_event( EventBits_t bits ) {
    portENTER_CRITICAL(&wifictlMux);
    EventBits_t temp = xEventGroupGetBits( wifictl_status ) & bits;
    portEXIT_CRITICAL(&wifictlMux);
    if ( temp )
        return( true );

    return( false );
}

void wifictl_register_cb( EventBits_t event, WIFICTL_CALLBACK_FUNC wifictl_event_cb ) {
    wifictl_event_cb_entrys++;

    if ( wifictl_event_cb_table == NULL ) {
        wifictl_event_cb_table = ( wifictl_event_cb_t * )ps_malloc( sizeof( wifictl_event_cb_t ) * wifictl_event_cb_entrys );
        if ( wifictl_event_cb_table == NULL ) {
            log_e("wifictl_event_cb_table malloc faild");
            while(true);
        }
    }
    else {
        wifictl_event_cb_t *new_wifictl_event_cb_table = NULL;

        new_wifictl_event_cb_table = ( wifictl_event_cb_t * )ps_realloc( wifictl_event_cb_table, sizeof( wifictl_event_cb_t ) * wifictl_event_cb_entrys );
        if ( new_wifictl_event_cb_table == NULL ) {
            log_e("wifictl_event_cb_table realloc faild");
            while(true);
        }
        wifictl_event_cb_table = new_wifictl_event_cb_table;
    }

    wifictl_event_cb_table[ wifictl_event_cb_entrys - 1 ].event = event;
    wifictl_event_cb_table[ wifictl_event_cb_entrys - 1 ].event_cb = wifictl_event_cb;
    log_i("register wifictl_event_cb success (%p)", wifictl_event_cb_table[ wifictl_event_cb_entrys - 1 ].event_cb );
}

void wifictl_send_event_cb( EventBits_t event, char *msg ) {
    if ( wifictl_event_cb_entrys == 0 ) {
      return;
    }
      
    for ( int entry = 0 ; entry < wifictl_event_cb_entrys ; entry++ ) {
        yield();
        if ( event & wifictl_event_cb_table[ entry ].event ) {
            log_i("call wifictl_event_cb (%p)", wifictl_event_cb_table[ entry ].event_cb );
            wifictl_event_cb_table[ entry ].event_cb( event, msg );
        }
    }
}

void wifictl_load_network( void ) {
  fs::File file = SPIFFS.open( WIFICTL_LIST_FILE, FILE_READ );

  if (!file) {
    log_e("Can't open file: %s", WIFICTL_LIST_FILE );
  }
  else {
    int filesize = file.size();
    if ( filesize > sizeof( wifictl_networklist  ) ) {
      log_e("Failed to read configfile. Wrong filesize!" );
    }
    else {
      file.read( (uint8_t *)wifictl_networklist, filesize );
    }
    file.close();
  }
}

bool wifictl_is_known( const char* networkname ) {
  if ( wifi_init == false )
    return( false );

  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( networkname, wifictl_networklist[ entry ].ssid ) ) {
      return( true );
    }
  }
  return( false );
}

bool wifictl_delete_network( const char *ssid ) {
  if ( wifi_init == false )
    return( false );

  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( ssid, wifictl_networklist[ entry ].ssid ) ) {
      wifictl_networklist[ entry ].ssid[ 0 ] = '\0';
      wifictl_networklist[ entry ].password[ 0 ] = '\0';
      wifictl_save_config();
      return( true );
    }
  }
  return( false );
}

bool wifictl_insert_network( const char *ssid, const char *password ) {
  if ( wifi_init == false )
    return( false );

  // check if existin
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( ssid, wifictl_networklist[ entry ].ssid ) ) {
      strlcpy( wifictl_networklist[ entry ].password, password, sizeof( wifictl_networklist[ entry ].password ) );
      wifictl_save_config();
      WiFi.scanNetworks();
      wifictl_set_event( WIFICTL_SCAN );
      return( true );
    }
  }
  // check for an emty entry
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( strlen( wifictl_networklist[ entry ].ssid ) == 0 ) {
      strlcpy( wifictl_networklist[ entry ].ssid, ssid, sizeof( wifictl_networklist[ entry ].ssid ) );
      strlcpy( wifictl_networklist[ entry ].password, password, sizeof( wifictl_networklist[ entry ].password ) );
      wifictl_save_config();
      WiFi.scanNetworks();
      wifictl_set_event( WIFICTL_SCAN );
      return( true );
    }
  }
  return( false ); 
}

void wifictl_on( void ) {
  if ( wifi_init == false )
    return;

  log_i("request wifictl on");
  while( wifictl_get_event( WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST ) ) { 
    yield();
  }
  wifictl_set_event( WIFICTL_ON_REQUEST | WIFICTL_FIRST_RUN );
  vTaskResume( _wifictl_Task );
}

void wifictl_off( void ) {
  if ( wifi_init == false )
    return;
  
  log_i("request wifictl off");
  while( wifictl_get_event( WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST ) ) { 
    yield();
  }

  if ( !wifictl_get_event( WIFICTL_FIRST_RUN ) ) {
    log_i("wifictl not active, prevent first run crash");
    return;
  }

  wifictl_set_event( WIFICTL_OFF_REQUEST );
  vTaskResume( _wifictl_Task );
}

void wifictl_standby( void ) {
  log_i("request wifictl standby");
  wifictl_off();
  while( wifictl_get_event( WIFICTL_ACTIVE | WIFICTL_CONNECT | WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST | WIFICTL_SCAN | WIFICTL_WPS_REQUEST ) ) { 
    yield();
  }
  log_i("request wifictl standby done");
}

void wifictl_wakeup( void ) {
  if ( wifictl_config.autoon ) {
    log_i("request wifictl wakeup");
    wifictl_on();
    log_i("request wifictl wakeup done");
  }
}

void wifictl_start_wps( void ) {
  if ( wifictl_get_event( WIFICTL_WPS_REQUEST ) )
    return;

  log_i("start WPS");

  esp_wps_config.crypto_funcs = &g_wifi_default_wps_crypto_funcs;
  esp_wps_config.wps_type = ESP_WPS_MODE;
  strlcpy( esp_wps_config.factory_info.manufacturer, ESP_MANUFACTURER, sizeof( esp_wps_config.factory_info.manufacturer ) );
  strlcpy( esp_wps_config.factory_info.model_number, ESP_MODEL_NUMBER, sizeof( esp_wps_config.factory_info.model_number ) );
  strlcpy( esp_wps_config.factory_info.model_name, ESP_MODEL_NAME, sizeof( esp_wps_config.factory_info.model_name ) );
  strlcpy( esp_wps_config.factory_info.device_name, ESP_DEVICE_NAME, sizeof( esp_wps_config.factory_info.device_name ) );

  WiFi.mode( WIFI_OFF );
  esp_wifi_stop();

  wifictl_set_event( WIFICTL_WPS_REQUEST );

  ESP_ERROR_CHECK( esp_wifi_set_mode( WIFI_MODE_STA ) );
  ESP_ERROR_CHECK( esp_wifi_start() );

  ESP_ERROR_CHECK( esp_wifi_wps_enable( &esp_wps_config ) );
  ESP_ERROR_CHECK( esp_wifi_wps_start( 120000 ) ); 
}

void wifictl_Task( void * pvParameters ) {
  if ( wifi_init == false )
    return;

  log_i("start wifictl task, heap: %d", ESP.getFreeHeap() );

  while ( true ) {
    vTaskDelay( 500 );
    if ( wifictl_get_event( WIFICTL_OFF_REQUEST ) && wifictl_get_event( WIFICTL_ON_REQUEST ) ) {
      log_w("confused by wifictl on/off at the same time. off request accept");
    }

    if ( wifictl_get_event( WIFICTL_OFF_REQUEST ) ) {
      WiFi.mode( WIFI_OFF );
      esp_wifi_stop();
      log_i("request wifictl off done");
    }
    else if ( wifictl_get_event( WIFICTL_ON_REQUEST ) ) {
      esp_wifi_start();
      WiFi.mode( WIFI_STA );
      log_i("request wifictl on done");
    }
    wifictl_clear_event( WIFICTL_OFF_REQUEST | WIFICTL_ACTIVE | WIFICTL_CONNECT | WIFICTL_SCAN | WIFICTL_ON_REQUEST );
    vTaskSuspend( _wifictl_Task );
  }
}