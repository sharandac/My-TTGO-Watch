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
#include "wifictl.h"
#include "powermgm.h"
#include "callback.h"
#include "config/wifictlconfig.h"
#include "utils/webserver/webserver.h"
#include "utils/ftpserver/ftpserver.h"

#ifdef NATIVE_64BIT
    #include <unistd.h>
    #include "utils/logging.h"

    lv_task_t * wifictl_lv_task;
    void wifictl_Task( lv_task_t * task );

    static EventBits_t wifictl_status;
#else
    #include <Arduino.h>
    #include <WiFi.h>
    #include <esp_wifi.h>
    #include <esp_wps.h>

    EventGroupHandle_t wifictl_status = NULL;
    portMUX_TYPE DRAM_ATTR wifictlMux = portMUX_INITIALIZER_UNLOCKED;

    TaskHandle_t _wifictl_Task;
    static esp_wps_config_t esp_wps_config;

    void wifictl_Task( void * pvParameters );
#endif

bool wifi_init = false;
callback_t *wifictl_callback = NULL;

void wifictl_send_event_cb( EventBits_t event, char *msg );
bool wifictl_powermgm_event_cb( EventBits_t event, void *arg );

char *wifiname=NULL;
char *wifipassword=NULL;

wifictl_config_t wifictl_config;

bool wifictl_send_event_cb( EventBits_t event, void *arg );
void wifictl_set_event( EventBits_t bits );
bool wifictl_get_event( EventBits_t bits );
void wifictl_clear_event( EventBits_t bits );
void wifictl_save_network( void );
void wifictl_load_network( void );
void wifictl_save_config( void );
void wifictl_load_config( void );

void wifictl_setup( void ) {
    /*
     * prevent wifictl init if already init
     */
    if ( wifi_init == true )
        return;
    wifi_init = true;
    /*
     * load config from spiff
     */
    wifictl_config.load();
#ifdef NATIVE_64BIT
    wifictl_lv_task = lv_task_create( wifictl_Task, 500, LV_TASK_PRIO_MID, NULL );
#else
    /*
     * create wifictl event group
     */
    wifictl_status = xEventGroupCreate();
    /*
     * limit wifi bandwidth to 20Mhz channel width
     */
    esp_wifi_set_bandwidth( ESP_IF_WIFI_STA, WIFI_BW_HT20 );
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);
    /*
     * register WiFi events
     */
    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifictl_set_event( WIFICTL_ACTIVE );
        wifictl_clear_event( WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST | WIFICTL_SCAN | WIFICTL_CONNECT );
        if ( wifictl_get_event( WIFICTL_WPS_REQUEST ) ) {
            wifictl_send_event_cb( WIFICTL_DISCONNECT, (void *)"wait for WPS" );
        }
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
        /**
         * send scan done event
         */
        wifictl_send_event_cb( WIFICTL_MSG, (void *)"scan done" );
        wifictl_send_event_cb( WIFICTL_SCAN_DONE, (void *)NULL );
        /**
         * send all entry via event
         */
        for( int i = 0 ; i < len ; i++ ) {
            wifictl_send_event_cb( WIFICTL_SCAN_ENTRY, (void *)WiFi.SSID(i).c_str() );
            log_i("found network entry %s with %d rssi", WiFi.SSID(i).c_str(), WiFi.RSSI(i) );
        }
        /**
         * connect if we discover a known network, but skip the ones that were already tried
         */
        for( int i = 0 ; i < len ; i++ ) {
            for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
                if ( !strcmp( wifictl_config.networklist[ entry ].ssid,  WiFi.SSID(i).c_str() ) && strcmp( wifictl_config.networklist[ entry ].ssid,  wifictl_config.networklist_tried[ entry ].ssid ) ) {
                    wifictl_send_event_cb( WIFICTL_MSG, (void *)"connecting ..." );
                    WiFi.setHostname(wifictl_config.hostname);
                    WiFi.begin( wifictl_config.networklist[ entry ].ssid, wifictl_config.networklist[ entry ].password );
                    log_d("try to connect to network entry %s with %d rssi", WiFi.SSID(i).c_str(), WiFi.RSSI(i) );
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
        /*
         * clean tried networklist
         */
        for ( int entry = 0 ; entry < NETWORKLIST_ENTRYS ; entry++ ) {
            wifictl_config.networklist_tried[ entry ].ssid[ 0 ] = '\0';
            wifictl_config.networklist_tried[ entry ].password[ 0 ] = '\0';
        }
    }, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP );

    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
        wifictl_set_event( WIFICTL_ACTIVE );
        wifictl_clear_event( WIFICTL_CONNECT | WIFICTL_OFF_REQUEST | WIFICTL_ON_REQUEST );
        if ( wifictl_get_event( WIFICTL_WPS_REQUEST ) ) {
            wifictl_send_event_cb( WIFICTL_ON, (void *)"wait for WPS" );
        }
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
#endif
    /*
     * register powermgm callback function
     */
    powermgm_register_cb_with_prio( POWERMGM_STANDBY, wifictl_powermgm_event_cb, "powermgm wifictl", CALL_CB_FIRST );
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP, wifictl_powermgm_event_cb, "powermgm wifictl" );
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
        case POWERMGM_STANDBY:          
            if ( !wifictl_config.enable_on_standby || wifictl_get_event( WIFICTL_OFF ) ) {
                wifictl_standby();
                retval = true;
            }
            else {
                log_w("standby blocked by \"enable on standby\" option");
                retval = false;
            }
            break;
        case POWERMGM_WAKEUP:
            wifictl_wakeup();
            retval = true;
            break;
        case POWERMGM_SILENCE_WAKEUP:   
            wifictl_wakeup();
            retval = true;
            break;
    }
    return( retval );
}

void wifictl_save_config( void ) {
    wifictl_config.save();
}

void wifictl_load_config( void ) {
    wifictl_config.load();
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

bool wifictl_get_webserver( void ) {
    return( wifictl_config.webserver );
}

void wifictl_set_webserver( bool webserver ) {
    wifictl_config.webserver = webserver;
    wifictl_save_config();
}

bool wifictl_get_ftpserver( void ) {
    return( wifictl_config.ftpserver );
}

void wifictl_set_ftpserver( bool ftpserver ) {
    wifictl_config.ftpserver = ftpserver;
    wifictl_save_config();
}

void wifictl_set_event( EventBits_t bits ) {
    /*
    * check if init
    */
    if ( wifi_init == false ) {
        log_e("wifictl not initialise");
        return;
    }
#ifdef NATIVE_64BIT
    wifictl_status |= bits;
#else
    portENTER_CRITICAL(&wifictlMux);
    xEventGroupSetBits( wifictl_status, bits );
    portEXIT_CRITICAL(&wifictlMux);
#endif
}

void wifictl_clear_event( EventBits_t bits ) {
    /*
    * check if init
    */
    if ( wifi_init == false ) {
        log_e("wifictl not initialise");
        return;
    }
#ifdef NATIVE_64BIT
    wifictl_status &= ~bits;
#else
    portENTER_CRITICAL(&wifictlMux);
    xEventGroupClearBits( wifictl_status, bits );
    portEXIT_CRITICAL(&wifictlMux);
#endif
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
#ifdef NATIVE_64BIT
    if( wifictl_status & bits )
        retval = true;
#else
    portENTER_CRITICAL(&wifictlMux);
    EventBits_t temp = xEventGroupGetBits( wifictl_status ) & bits;
    portEXIT_CRITICAL(&wifictlMux);
    if ( temp )
        retval = true;
#endif
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
        if( !strcmp( networkname, wifictl_config.networklist[ entry ].ssid ) ) {
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
        if( !strcmp( ssid, wifictl_config.networklist[ entry ].ssid ) ) {
        wifictl_config.networklist[ entry ].ssid[ 0 ] = '\0';
        wifictl_config.networklist[ entry ].password[ 0 ] = '\0';
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
        if( !strcmp( ssid, wifictl_config.networklist[ entry ].ssid ) ) {
        strncpy( wifictl_config.networklist[ entry ].password, password, sizeof( wifictl_config.networklist[ entry ].password ) );
        wifictl_save_config();
#ifndef NATIVE_64BIT
        WiFi.scanNetworks( true );
        wifictl_set_event( WIFICTL_SCAN );
#endif
        retval = true;
        return( retval );
        }
    }
    /*
    * check for an emty entry and insert
    */
    for( int entry = 0 ; entry < NETWORKLIST_ENTRYS; entry++ ) {
        if( strlen( wifictl_config.networklist[ entry ].ssid ) == 0 ) {
        strncpy( wifictl_config.networklist[ entry ].ssid, ssid, sizeof( wifictl_config.networklist[ entry ].ssid ) );
        strncpy( wifictl_config.networklist[ entry ].password, password, sizeof( wifictl_config.networklist[ entry ].password ) );
        wifictl_save_config();
#ifndef NATIVE_64BIT
        WiFi.scanNetworks( true );
        wifictl_set_event( WIFICTL_SCAN );
#endif
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
#ifndef NATIVE_64BIT
        yield();
#endif
    }
    wifictl_set_event( WIFICTL_ON_REQUEST | WIFICTL_FIRST_RUN );
#ifndef NATIVE_64BIT
    vTaskResume( _wifictl_Task );
#endif
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
#ifndef NATIVE_64BIT
        yield();
#endif
    }

    if ( !wifictl_get_event( WIFICTL_FIRST_RUN ) ) {
        log_i("wifictl not active, prevent first run crash");
        return;
    }

    wifictl_set_event( WIFICTL_OFF_REQUEST );
#ifndef NATIVE_64BIT
    vTaskResume( _wifictl_Task );
#endif
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
#ifndef NATIVE_64BIT
        yield();
#endif
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
#ifndef NATIVE_64BIT
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
#endif
}

#ifdef NATIVE_64BIT
void wifictl_Task( lv_task_t * task ) {
    /*
    * check if init
    */
    if ( wifi_init == false ) {
        log_e("wifictl not initialise, start task failed");
        while( true );
    }

    if ( wifictl_get_event( WIFICTL_OFF_REQUEST ) && wifictl_get_event( WIFICTL_ON_REQUEST ) ) {
        log_w("confused by wifictl on/off at the same time. off request accept");
    }

    if ( wifictl_get_event( WIFICTL_OFF_REQUEST ) ) {
        log_i("request wifictl off done");
        wifictl_set_event( WIFICTL_OFF );
        wifictl_clear_event( WIFICTL_ON );
        wifictl_send_event_cb( WIFICTL_DISCONNECT, (void *)"disconnected" );
        wifictl_send_event_cb( WIFICTL_OFF, (void *)"" );
    }
    else if ( wifictl_get_event( WIFICTL_ON_REQUEST ) ) {
        log_i("request wifictl on done");
        wifictl_set_event( WIFICTL_ON );
        wifictl_clear_event( WIFICTL_OFF );
        wifictl_send_event_cb( WIFICTL_ON, (void *)"scan ..." );
        wifictl_send_event_cb( WIFICTL_SCAN, (void *)"connecting ..." );
        wifictl_send_event_cb( WIFICTL_CONNECT, (void *)"connect to wifi" );
        wifictl_send_event_cb( WIFICTL_CONNECT_IP, (void *)"x.x.x.x" );
        wifictl_send_event_cb( WIFICTL_SCAN_ENTRY, (void *)"foobar" );
        wifictl_send_event_cb( WIFICTL_SCAN_ENTRY, (void *)"fnord" );
        wifictl_send_event_cb( WIFICTL_SCAN_ENTRY, (void *)"23" );
    }

    wifictl_clear_event( WIFICTL_OFF_REQUEST | WIFICTL_ACTIVE | WIFICTL_CONNECT | WIFICTL_SCAN | WIFICTL_ON_REQUEST );
}
#else
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
#endif
