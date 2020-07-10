#include "config.h"
#include <Arduino.h>
#include <WiFi.h>

#include "powermgm.h"
#include "wifictl.h"

#include "gui/statusbar.h"
#include "webserver/webserver.h"

bool wifi_init = false;
TaskHandle_t _WIFICTL_Task;

void wifictl_StartTask( void );
void wifictl_Task( void * pvParameters );
TaskHandle_t _wifictl_Task;

char *wifiname=NULL;
char *wifipassword=NULL;

struct networklist wifictl_networklist[ NETWORKLIST_ENTRYS ];

void wifictl_save_network( void );
void wifictl_load_network( void );
void wifictl_Task( void * pvParameters );

/*
 *
 */
void wifictl_setup( void ) {
    if ( wifi_init == true )
        return;
    wifi_init = true;
    powermgm_clear_event( POWERMGM_WIFI_ACTIVE | POWERMGM_WIFI_OFF_REQUEST | POWERMGM_WIFI_ON_REQUEST | POWERMGM_WIFI_CONNECTED | POWERMGM_WIFI_SCAN );

    // clean network list table
    for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
      wifictl_networklist[ entry ].ssid[ 0 ] = '\0';
      wifictl_networklist[ entry ].password[ 0 ] = '\0';
    }

    // load network list from spiff
    wifictl_load_network();

    // register WiFi events
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        powermgm_set_event( POWERMGM_WIFI_ACTIVE );
        powermgm_clear_event( POWERMGM_WIFI_OFF_REQUEST | POWERMGM_WIFI_ON_REQUEST | POWERMGM_WIFI_SCAN | POWERMGM_WIFI_CONNECTED );
        statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_GRAY );
        statusbar_show_icon( STATUSBAR_WIFI );
        statusbar_wifi_set_state( true, "scan ..." );
        WiFi.scanNetworks();
    }, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        powermgm_set_event( POWERMGM_WIFI_ACTIVE );
        powermgm_clear_event( POWERMGM_WIFI_OFF_REQUEST | POWERMGM_WIFI_ON_REQUEST | POWERMGM_WIFI_SCAN | POWERMGM_WIFI_CONNECTED );
        statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_GRAY );
        statusbar_show_icon( STATUSBAR_WIFI );
        int len = WiFi.scanComplete();
        for( int i = 0 ; i < len ; i++ ) {
          for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
            if ( !strcmp( wifictl_networklist[ entry ].ssid,  WiFi.SSID(i).c_str() ) ) {
              wifiname = wifictl_networklist[ entry ].ssid;
              wifipassword = wifictl_networklist[ entry ].password;
              statusbar_wifi_set_state( true, "connecting ..." );
              WiFi.begin( wifiname, wifipassword );
              return;
            }
          }
        }
    }, WiFiEvent_t::SYSTEM_EVENT_SCAN_DONE );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        powermgm_set_event( POWERMGM_WIFI_CONNECTED | POWERMGM_WIFI_ACTIVE );
        powermgm_clear_event( POWERMGM_WIFI_OFF_REQUEST | POWERMGM_WIFI_ON_REQUEST | POWERMGM_WIFI_SCAN );
        statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_WHITE );
        statusbar_show_icon( STATUSBAR_WIFI );
        statusbar_wifi_set_state( true, wifiname );
        asyncwebserver_setup();
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        powermgm_set_event( POWERMGM_WIFI_ACTIVE | POWERMGM_WIFI_SCAN );
        powermgm_clear_event( POWERMGM_WIFI_CONNECTED | POWERMGM_WIFI_OFF_REQUEST | POWERMGM_WIFI_ON_REQUEST );
        statusbar_style_icon( STATUSBAR_WIFI, STATUSBAR_STYLE_GRAY );
        statusbar_show_icon( STATUSBAR_WIFI );
        statusbar_wifi_set_state( true, "scan ..." );
        WiFi.scanNetworks();
    }, WiFiEvent_t::SYSTEM_EVENT_WIFI_READY );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        powermgm_clear_event( POWERMGM_WIFI_ACTIVE | POWERMGM_WIFI_CONNECTED | POWERMGM_WIFI_OFF_REQUEST | POWERMGM_WIFI_ON_REQUEST | POWERMGM_WIFI_SCAN );
        statusbar_hide_icon( STATUSBAR_WIFI );
        statusbar_wifi_set_state( false, "" );
    }, WiFiEvent_t::SYSTEM_EVENT_STA_STOP );

  // start Wifo controll task
  xTaskCreatePinnedToCore(
                            wifictl_Task,    /* Function to implement the task */
                            "wifictl Task",       /* Name of the task */
                            10000,                  /* Stack size in words */
                            NULL,                   /* Task input parameter */
                            1,                      /* Priority of the task */
                            &_wifictl_Task,       /* Task handle. */
                            1 );  /* Core where the task should run */ 
}

/*
 *
 */
void wifictl_save_network( void ) {
  fs::File file = SPIFFS.open( WIFICTL_CONFIG_FILE, FILE_WRITE );

  if ( !file ) {
    Serial.printf("Can't save file: %s\r\n", WIFICTL_CONFIG_FILE );
  }
  else {
    file.write( (uint8_t *)wifictl_networklist, sizeof( wifictl_networklist  ) );
    file.close();
  }
}

/*
 *
 */
void wifictl_load_network( void ) {
  fs::File file = SPIFFS.open( WIFICTL_CONFIG_FILE, FILE_READ );

  if (!file) {
    Serial.printf("Can't open file: %s\r\n", WIFICTL_CONFIG_FILE );
  }
  else {
    int filesize = file.size();
    if ( filesize > sizeof( wifictl_networklist  ) ) {
      Serial.printf("Failed to read configfile. Wrong filesize!\r\n" );
    }
    else {
      file.read( (uint8_t *)wifictl_networklist, filesize );
    }
    file.close();
  }
}

/*
 *
 */
bool wifictl_is_known( const char* networkname ) {
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( networkname, wifictl_networklist[ entry ].ssid ) ) {
      return( true );
    }
  }
  return( false );
}

/*
 *
 */
bool wifictl_delete_network( const char *ssid ) {
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( ssid, wifictl_networklist[ entry ].ssid ) ) {
      wifictl_networklist[ entry ].ssid[ 0 ] = '\0';
      wifictl_networklist[ entry ].password[ 0 ] = '\0';
      wifictl_save_network();
      return( true );
    }
  }
  return( false );
}

/*
 *
 */
bool wifictl_insert_network( const char *ssid, const char *password ) {
  // check if existin
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( !strcmp( ssid, wifictl_networklist[ entry ].ssid ) ) {
      strncpy( wifictl_networklist[ entry ].ssid, ssid, sizeof( wifictl_networklist[ entry ].ssid ) );
      wifictl_save_network();
      WiFi.scanNetworks();
      powermgm_set_event( POWERMGM_WIFI_SCAN );
      return( true );
    }
  }
  // check for an emty entry
  for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
    if( strlen( wifictl_networklist[ entry ].ssid ) == 0 ) {
      Serial.printf("wifictl: insert network\r\n");
      strncpy( wifictl_networklist[ entry ].ssid, ssid, sizeof( wifictl_networklist[ entry ].ssid ) );
      strncpy( wifictl_networklist[ entry ].password, password, sizeof( wifictl_networklist[ entry ].password ) );
      wifictl_save_network();
      WiFi.scanNetworks();
      powermgm_set_event( POWERMGM_WIFI_SCAN );
      return( true );
    }
  }
  return( false ); 
}

/*
 *
 */
void wifictl_on( void ) {
  if ( wifi_init == false )
    return;
  vTaskResume( _wifictl_Task );
  powermgm_set_event( POWERMGM_WIFI_ON_REQUEST );
}

/*
 *
 */
void wifictl_off( void ) {
  if ( wifi_init == false )
    return;
  powermgm_set_event( POWERMGM_WIFI_OFF_REQUEST );
}

/*
 * 
 */
void wifictl_Task( void * pvParameters ) {
  if ( wifi_init == false )
    return;

  while( true ) {
    vTaskDelay( 50 );
    if ( powermgm_get_event( POWERMGM_STANDBY ) ) {   
        vTaskSuspend( _wifictl_Task );
    }
    else {
      if ( powermgm_get_event( POWERMGM_WIFI_ON_REQUEST ) ) {
        Serial.printf("wlan on request\r\n");
        statusbar_wifi_set_state( true, "activate" );
        WiFi.mode( WIFI_STA );
        powermgm_clear_event( POWERMGM_WIFI_OFF_REQUEST | POWERMGM_WIFI_ACTIVE | POWERMGM_WIFI_CONNECTED | POWERMGM_WIFI_SCAN | POWERMGM_WIFI_ON_REQUEST );
      }
      else if ( powermgm_get_event( POWERMGM_WIFI_OFF_REQUEST ) ) {
        Serial.printf("wlan of request\r\n");
        statusbar_wifi_set_state( false, "" );
        WiFi.mode( WIFI_OFF );
        powermgm_clear_event( POWERMGM_WIFI_OFF_REQUEST | POWERMGM_WIFI_ACTIVE | POWERMGM_WIFI_CONNECTED | POWERMGM_WIFI_SCAN | POWERMGM_WIFI_ON_REQUEST );
      }
    }
  }
}