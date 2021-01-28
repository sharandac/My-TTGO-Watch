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
#include "powermgm.h"
#include "callback.h"
#include "json_psram_allocator.h"
#include "alloc.h"

#include "gui/statusbar.h"
#ifdef ENABLE_WEBSERVER
#include "webserver/webserver.h"
#endif
#ifdef ENABLE_FTPSERVER
#include "ftpserver/ftpserver.h"
#endif

bool wifi_init = false;
EventGroupHandle_t wifictl_status = NULL;
portMUX_TYPE DRAM_ATTR wifictlMux = portMUX_INITIALIZER_UNLOCKED;
callback_t *wifictl_callback = NULL;

void wifictl_send_event_cb( EventBits_t event, char *msg );
bool wifictl_powermgm_event_cb( EventBits_t event, void *arg );

void wifictl_StartTask( void );
void wifictl_Task( void * pvParameters );
TaskHandle_t _wifictl_Task;

char *wifiname=NULL;
char *wifipassword=NULL;

static networklist *wifictl_networklist = NULL;
wifictl_config_t wifictl_config;

static esp_wps_config_t esp_wps_config;

bool wifictl_send_event_cb( EventBits_t event, void *arg );
void wifictl_set_event( EventBits_t bits );
bool wifictl_get_event( EventBits_t bits );
void wifictl_clear_event( EventBits_t bits );
void wifictl_save_network( void );
void wifictl_load_network( void );
void wifictl_save_config( void );
void wifictl_load_config( void );
void wifictl_Task( void * pvParameters );

void wifictl_setup( void ) {
    /*
     * prevent wifictl init if already init
     */
    if ( wifi_init == true )
        return;
    wifi_init = true;
    /*
     * create wifictl event group
     */
    wifictl_status = xEventGroupCreate();
    /*
     * allocate networklist and clean networklist
     */
    wifictl_networklist = (networklist*)CALLOC( sizeof( networklist ) * NETWORKLIST_ENTRYS, 1 );
    if( !wifictl_networklist ) {
      log_e("wifictl_networklist calloc faild");
      while(true);
    }
    for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
      wifictl_networklist[ entry ].ssid[ 0 ] = '\0';
      wifictl_networklist[ entry ].password[ 0 ] = '\0';
    }
    /*
     * load config from spiff
     */
    wifictl_load_config();
    /*
     * limit wifi bandwidth to 20Mhz channel width
     */
    esp_wifi_set_bandwidth( ESP_IF_WIFI_STA, WIFI_BW_HT20 );
    /*
     * register WiFi events
     */
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifictl_set_event( WIFICTL_ACTIVE );
        wifictl_clear_event( WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST | WIFICTL_SCAN | WIFICTL_CONNECT );
        if ( wifictl_get_event( WIFICTL_WPS_REQUEST ) )
          wifictl_send_event_cb( WIFICTL_DISCONNECT, (void *)"wait for WPS" );
        else {
          wifictl_set_event( WIFICTL_SCAN );
          wifictl_send_event_cb( WIFICTL_DISCONNECT, (void *)"scan ..." );
          WiFi.scanNetworks( true );
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
              wifictl_send_event_cb( WIFICTL_SCAN, (void *)"connecting ..." );
              WiFi.begin( wifiname, wifipassword );
              return;
            }
          }
        }
        wifictl_send_event_cb( WIFICTL_SCAN, (void *)"scan done" );
    }, WiFiEvent_t::SYSTEM_EVENT_SCAN_DONE );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifictl_set_event( WIFICTL_CONNECT | WIFICTL_ACTIVE );
        if ( wifictl_get_event( WIFICTL_WPS_REQUEST ) ) {
          log_i("store new SSID and psk from WPS");
          wifictl_insert_network( WiFi.SSID().c_str(), WiFi.psk().c_str() );
          wifictl_save_config();
        }
        wifictl_clear_event( WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST | WIFICTL_SCAN | WIFICTL_WPS_REQUEST  );
        wifictl_send_event_cb( WIFICTL_CONNECT, (void *)WiFi.SSID().c_str() );
        wifictl_send_event_cb( WIFICTL_CONNECT_IP, (void *)WiFi.localIP().toString().c_str() );
        #ifdef ENABLE_WEBSERVER
        if ( wifictl_config.webserver ) {
          asyncwebserver_start();
        }
        #endif
        #ifdef ENABLE_FTPSERVER
        if ( wifictl_config.ftpserver ) {
          ftpserver_start( wifictl_config.ftpuser , wifictl_config.ftppass );
        }
        # endif
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifictl_set_event( WIFICTL_ACTIVE );
        wifictl_clear_event( WIFICTL_CONNECT | WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST );
        if ( wifictl_get_event( WIFICTL_WPS_REQUEST ) )
          wifictl_send_event_cb( WIFICTL_ON, (void *)"wait for WPS" );
        else {
          wifictl_set_event( WIFICTL_SCAN );
          wifictl_send_event_cb( WIFICTL_ON, (void *)"scan ..." );
          WiFi.scanNetworks( true );
        }
    }, WiFiEvent_t::SYSTEM_EVENT_WIFI_READY );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        #ifdef ENABLE_WEBSERVER
        asyncwebserver_end();
        #endif
        wifictl_clear_event( WIFICTL_ACTIVE | WIFICTL_CONNECT | WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST | WIFICTL_SCAN | WIFICTL_WPS_REQUEST );
        wifictl_send_event_cb( WIFICTL_OFF, (void *)"" );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_STOP );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
      esp_wifi_wps_disable();
      WiFi.begin();
      wifictl_send_event_cb( WIFICTL_WPS_SUCCESS, (void *)"wps success" );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_WPS_ER_SUCCESS );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
      esp_wifi_wps_disable();
      wifictl_send_event_cb( WIFICTL_WPS_SUCCESS, (void *)"wps failed" );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_WPS_ER_FAILED );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
      esp_wifi_wps_disable();
      wifictl_send_event_cb( WIFICTL_WPS_SUCCESS, (void *)"wps timeout" );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_WPS_ER_TIMEOUT );
    /*
     * Add wifictl task
     */
    xTaskCreatePinnedToCore(  wifictl_Task,     /* Function to implement the task */
                              "wifictl Task",   /* Name of the task */
                              3000,             /* Stack size in words */
                              NULL,             /* Task input parameter */
                              1,                /* Priority of the task */
                              &_wifictl_Task,   /* Task handle. */
                              0 );
    vTaskSuspend( _wifictl_Task );
    /*
     * register powermgm callback function
     */
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, wifictl_powermgm_event_cb, "powermgm wifictl" );
    /*
     * set default state after init
     */
    wifictl_set_event( WIFICTL_OFF );
}

bool wifictl_powermgm_event_cb( EventBits_t event, void *arg ) {
  bool retval = false;
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return( retval );
  }
  
  switch( event ) {
      case POWERMGM_STANDBY:          if ( !wifictl_config.enable_on_standby || wifictl_get_event( WIFICTL_OFF ) ) {
                                        wifictl_standby();
                                        retval = true;
                                      }
                                      else {
                                        log_w("standby blocked by \"enable on standby\" option");
                                        retval = false;
                                      }
                                      break;
      case POWERMGM_WAKEUP:           wifictl_wakeup();
                                      retval = true;
                                      break;
      case POWERMGM_SILENCE_WAKEUP:   wifictl_wakeup();
                                      retval = true;
                                      break;
  }
  return( retval );
}

void wifictl_save_config( void ) {
    fs::File file = SPIFFS.open( WIFICTL_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", WIFICTL_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 10000 );

        doc["autoon"] = wifictl_config.autoon;
        #ifdef ENABLE_WEBSERVER
        doc["webserver"] = wifictl_config.webserver;
        #endif
        #ifdef ENABLE_FTPSERVER
        doc["ftpserver"] = wifictl_config.ftpserver;
        doc["ftpuser"] = wifictl_config.ftpuser;
        doc["ftppass"] = wifictl_config.ftppass;
        #endif
        doc["enable_on_standby"] = wifictl_config.enable_on_standby;
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
            wifictl_config.autoon = doc["autoon"] | true;
            #ifdef ENABLE_WEBSERVER
            wifictl_config.webserver = doc["webserver"] | false;
            #endif
            #ifdef ENABLE_FTPSERVER
            wifictl_config.ftpserver = doc["ftpserver"] | false;
            
            if ( doc["ftpuser"] )
              strlcpy( wifictl_config.ftpuser, doc["ftpuser"], sizeof( wifictl_config.ftpuser ) );
            else
              strlcpy( wifictl_config.ftpuser, FTPSERVER_USER, sizeof( wifictl_config.ftpuser ) );
            if ( doc["ftppass"] )
              strlcpy( wifictl_config.ftppass, doc["ftppass"], sizeof( wifictl_config.ftppass ) );
            else
              strlcpy( wifictl_config.ftppass, FTPSERVER_PASSWORD, sizeof( wifictl_config.ftppass ) );
            #endif

            wifictl_config.enable_on_standby = doc["enable_on_standby"] | false;
            for ( int i = 0 ; i < NETWORKLIST_ENTRYS ; i++ ) {
                if ( doc["networklist"][ i ]["ssid"] && doc["networklist"][ i ]["psk"] ) {
                    strlcpy( wifictl_networklist[ i ].ssid    , doc["networklist"][ i ]["ssid"], sizeof( wifictl_networklist[ i ].ssid ) );
                    strlcpy( wifictl_networklist[ i ].password, doc["networklist"][ i ]["psk"], sizeof( wifictl_networklist[ i ].password ) );
                }
            }
        }        
        doc.clear();
    }
    file.close();
}

bool wifictl_get_autoon( void ) {
  return( wifictl_config.autoon );
}

bool wifictl_get_enable_on_standby( void ) {
  return( wifictl_config.enable_on_standby );
}

void wifictl_set_autoon( bool autoon ) {
  wifictl_config.autoon = autoon;
  wifictl_send_event_cb( WIFICTL_AUTOON, (void*)&autoon );
  wifictl_save_config();
}

void wifictl_set_enable_on_standby( bool enable ) {
  wifictl_config.enable_on_standby = enable;
  wifictl_save_config();
}

#ifdef ENABLE_WEBSERVER
bool wifictl_get_webserver( void ) {
  return( wifictl_config.webserver );
}

void wifictl_set_webserver( bool webserver ) {
  wifictl_config.webserver = webserver;
  wifictl_save_config();
}
#endif

#ifdef ENABLE_FTPSERVER
bool wifictl_get_ftpserver( void ) {
return( wifictl_config.ftpserver );
}

void wifictl_set_ftpserver( bool ftpserver ) {
  wifictl_config.ftpserver = ftpserver;
  wifictl_save_config();
}
#endif

void wifictl_set_event( EventBits_t bits ) {
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return;
  }

  portENTER_CRITICAL(&wifictlMux);
  xEventGroupSetBits( wifictl_status, bits );
  portEXIT_CRITICAL(&wifictlMux);
}

void wifictl_clear_event( EventBits_t bits ) {
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return;
  }

  portENTER_CRITICAL(&wifictlMux);
  xEventGroupClearBits( wifictl_status, bits );
  portEXIT_CRITICAL(&wifictlMux);
}

bool wifictl_get_event( EventBits_t bits ) {
  bool retval = false;
  
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return( retval );
  }
  
  portENTER_CRITICAL(&wifictlMux);
  EventBits_t temp = xEventGroupGetBits( wifictl_status ) & bits;
  portEXIT_CRITICAL(&wifictlMux);
  if ( temp )
    retval = true;

  return( retval );
}

bool wifictl_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
  /*
    * check if an callback table exist, if not allocate a callback table
    */
  if ( wifictl_callback == NULL ) {
      wifictl_callback = callback_init( "wifictl" );
      if ( wifictl_callback == NULL ) {
          log_e("wifictl callback alloc failed");
          while(true);
      }
  }
  /*
    * register an callback entry and return them
    */
  return( callback_register( wifictl_callback, event, callback_func, id ) );
}

bool wifictl_send_event_cb( EventBits_t event, void *arg ) {
  /*
    * call all callbacks with her event mask
    */
  return( callback_send( wifictl_callback, event, arg ) );
}

bool wifictl_is_known( const char* networkname ) {
  bool retval = false;
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return( retval );
  }
  /*
   * check if network already in the networklist
   */
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( networkname, wifictl_networklist[ entry ].ssid ) ) {
      retval = true;
      return( retval );
    }
  }
  return( retval );
}

bool wifictl_delete_network( const char *ssid ) {
  bool retval = false;
  
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return( retval );
  }
  /*
   * search networklist entry and zero them
   */
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( ssid, wifictl_networklist[ entry ].ssid ) ) {
      wifictl_networklist[ entry ].ssid[ 0 ] = '\0';
      wifictl_networklist[ entry ].password[ 0 ] = '\0';
      wifictl_save_config();
      retval = true;
      return( retval );
    }
  }
  return( retval );
}

bool wifictl_insert_network( const char *ssid, const char *password ) {
  bool retval = false;
  
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return( retval );
  }
  /*
   * check if network exist
   */
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( ssid, wifictl_networklist[ entry ].ssid ) ) {
      strlcpy( wifictl_networklist[ entry ].password, password, sizeof( wifictl_networklist[ entry ].password ) );
      wifictl_save_config();
      WiFi.scanNetworks( true );
      wifictl_set_event( WIFICTL_SCAN );
      retval = true;
      return( retval );
    }
  }
  /*
   * check for an emty entry and insert
   */
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( strlen( wifictl_networklist[ entry ].ssid ) == 0 ) {
      strlcpy( wifictl_networklist[ entry ].ssid, ssid, sizeof( wifictl_networklist[ entry ].ssid ) );
      strlcpy( wifictl_networklist[ entry ].password, password, sizeof( wifictl_networklist[ entry ].password ) );
      wifictl_save_config();
      WiFi.scanNetworks( true );
      wifictl_set_event( WIFICTL_SCAN );
      retval = true;
      return( retval );
    }
  }
  return( retval ); 
}

void wifictl_on( void ) {
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return;
  }

  log_i("request wifictl on");
  while( wifictl_get_event( WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST ) ) { 
    yield();
  }
  wifictl_set_event( WIFICTL_ON_REQUEST | WIFICTL_FIRST_RUN );
  vTaskResume( _wifictl_Task );
}

void wifictl_off( void ) {
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return;
  }

  
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
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return;
  }

  log_i("request wifictl standby");
  wifictl_off();
  while( wifictl_get_event( WIFICTL_ACTIVE | WIFICTL_CONNECT | WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST | WIFICTL_SCAN | WIFICTL_WPS_REQUEST ) ) { 
    yield();
  }
  log_i("request wifictl standby done");
}

void wifictl_wakeup( void ) {
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return;
  }
  
  if ( wifictl_config.autoon ) {
    log_i("request wifictl wakeup");
    wifictl_on();
    log_i("request wifictl wakeup done");
  }
}

void wifictl_start_wps( void ) {
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise");
    return;
  }
  
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
  /*
   * check if init
   */
  if ( wifi_init == false ) {
    log_e("wifictl not initialise, start task failed");
    while( true );
  }

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
      wifictl_set_event( WIFICTL_OFF );
      wifictl_clear_event( WIFICTL_ON );
    }
    else if ( wifictl_get_event( WIFICTL_ON_REQUEST ) ) {
      esp_wifi_start();
      WiFi.mode( WIFI_STA );
      log_i("request wifictl on done");
      wifictl_set_event( WIFICTL_ON );
      wifictl_clear_event( WIFICTL_OFF );
    }
    wifictl_clear_event( WIFICTL_OFF_REQUEST | WIFICTL_ACTIVE | WIFICTL_CONNECT | WIFICTL_SCAN | WIFICTL_ON_REQUEST );
    vTaskSuspend( _wifictl_Task );
  }
}