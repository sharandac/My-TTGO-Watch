/****************************************************************************
 *   Aug 11 17:13:51 2020
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

/*
 *  inspire by https://github.com/bburky/t-watch-2020-project
 *
 */
#include "config.h"
#include "blectl.h"
#include "pmu.h"
#include "powermgm.h"
#include "callback.h"
#include "device.h"
#include "utils/charbuffer.h"
#include "utils/alloc.h"
#include "utils/bluejsonrequest.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"

    static EventBits_t blectl_status;
#else
    #if defined( M5PAPER )
    #elif defined( M5CORE2 )
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #elif defined( LILYGO_WATCH_2021 )
    #elif defined( WT32_SC01 )
    #else
        #warning "no hardware driver for blectl"
    #endif

    #include <Arduino.h>
    #include "ble/blebatctl.h"
    #include "ble/blestepctl.h"
    #include "ble/gadgetbridge.h"
    #include "ble/deviceinfo.h"
    #include "ble/parkinsense_ble.h"
    #include "esp_system.h"
    #include "NimBLEDescriptor.h"
    #include "freertos/FreeRTOS.h"
    #include "freertos/task.h"
    #include <vector>

    EventGroupHandle_t blectl_status = NULL;
    portMUX_TYPE DRAM_ATTR blectlMux = portMUX_INITIALIZER_UNLOCKED;
#endif

blectl_config_t blectl_config;              /** @brief blectl config structure */
callback_t *blectl_callback = NULL;         /** @brief blectl callback structure */

static bool blectl_send_event_cb( EventBits_t event, void *arg );
static bool blectl_powermgm_event_cb( EventBits_t event, void *arg );

#ifdef NATIVE_64BIT
#else
    NimBLEServer *pServer = NULL;                          
    NimBLEAdvertising *pAdvertising = NULL;
    /** Set while we expect onDisconnect to follow blectl_before_light_sleep. */
    static bool blectl_light_sleep_disconnect_in_progress = false;
    /**
     * If false, firmware must not start advertising (onDisconnect, etc.). Set true only by:
     *   user BLE on / visibility in settings, POWERMGM WAKEUP (device use), blectl_on_hourly_advertise_window.
     * Cleared by blectl_off and blectl_before_light_sleep.
     */
    static bool blectl_may_start_ad = false;

    static void blectl_apply_advertising_from_config( void ) {
        if ( !pServer || !pAdvertising ) {
            return;
        }
        if ( blectl_config.advertising ) {
            pAdvertising->start();
            log_i( "[PWR_BLE] advertising->start (visibility on)" );
        }
        else {
            pAdvertising->stop();
            log_i( "[PWR_BLE] advertising->stop (visibility off)" );
        }
    }

    static void blectl_privileged_start_advertising_if_runnable( const char *reason ) {
        if ( !blectl_may_start_ad || !blectl_get_autoon() || !blectl_get_advertising() ) {
            return;
        }
        if ( !pServer || !pAdvertising || pServer->getConnectedCount() > 0 ) {
            return;
        }
        if ( !pAdvertising->isAdvertising() ) {
            pAdvertising->start();
            log_i( "[PWR_BLE] %s: advertising on (may_start=1, no link)", reason );
        }
    }

    static void blectl_disconnect_all_or_log( const char *ctx ) {
        if ( !pServer )
            return;
        const int max_rounds = 8;
        for ( int round = 0; round < max_rounds && pServer->getConnectedCount() > 0; round++ ) {
            const std::vector<uint16_t> peers = pServer->getPeerDevices();
            if ( peers.empty() ) {
                log_w( "[PWR_BLE] %s: stack reports %d link(s) but no peer ids; waiting", ctx,
                    (int)pServer->getConnectedCount() );
                vTaskDelay( pdMS_TO_TICKS( 30 ) );
                continue;
            }
            if ( round == 0 )
                log_i( "[PWR_BLE] %s: terminating %d link(s)", ctx, (int)pServer->getConnectedCount() );
            const int rc = pServer->disconnect( peers[ 0 ] );
            if ( rc != 0 )
                vTaskDelay( pdMS_TO_TICKS( 50 ) );
            else
                vTaskDelay( pdMS_TO_TICKS( 15 ) );
        }
        if ( pServer->getConnectedCount() > 0 )
            log_w( "[PWR_BLE] %s: %d link(s) still up after retries (may clear asynchronously)", ctx,
                (int)pServer->getConnectedCount() );
    }

    class ServerCallbacks: public NimBLEServerCallbacks {
        void onConnect(NimBLEServer* pServer ) {
            log_i( "Client connected" );
            if ( blectl_may_start_ad && blectl_get_autoon() && blectl_get_advertising() ) {
                NimBLEDevice::startAdvertising();
            }
        };

        void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
            pServer->updateConnParams(desc->conn_handle, blectl_config.minInterval, blectl_config.maxInterval, blectl_config.latency, blectl_config.timeout );
            blectl_set_event( BLECTL_AUTHWAIT );
            blectl_clear_event( BLECTL_DISCONNECT | BLECTL_CONNECT );
            powermgm_resume_from_ISR();
            log_d("BLE authwait");
            blectl_send_event_cb( BLECTL_AUTHWAIT, (void *)"authwait" );
            log_i("Client address: %s", NimBLEAddress(desc->peer_ota_addr).toString().c_str() );
        };

        void onDisconnect(NimBLEServer* pServer) {
            log_d("BLE disconnected");
            blectl_set_event( BLECTL_DISCONNECT );
            blectl_clear_event( BLECTL_CONNECT | BLECTL_AUTHWAIT );
            blectl_send_event_cb( BLECTL_DISCONNECT, (void *)"disconnected" );
            powermgm_resume_from_ISR();

            if ( blectl_light_sleep_disconnect_in_progress ) {
                blectl_light_sleep_disconnect_in_progress = false;
                log_i( "[PWR_BLE] onDisconnect: post light-sleep drop — no auto advertising (may_start=%d)", (int)blectl_may_start_ad );
            }
            else if ( blectl_may_start_ad && blectl_get_advertising() && blectl_get_autoon() && !pServer->getAdvertising()->isAdvertising() ) {
                pServer->getAdvertising()->start();
                log_i( "[PWR_BLE] onDisconnect: advertising restarted (may_start=1)" );
            }
            else {
                log_i( "[PWR_BLE] onDisconnect: advertising not restarted (mode or may_start=0 or BLE/visibility off)" );
            }
        };

        void onMTUChange( uint16_t MTU, ble_gap_conn_desc* desc ) {
            log_i("MTU updated: %u for connection ID: %u\n", MTU, desc->conn_handle);
        };
        
        uint32_t onPassKeyRequest(){
            uint32_t pass_key = random( 0,999999 );
            char pin[16]="";
            snprintf( pin, sizeof( pin ), "%06d", pass_key );
            log_d("BLECTL pairing request, PIN: %s", pin );

            blectl_set_event( BLECTL_PIN_AUTH );
            blectl_send_event_cb( BLECTL_PIN_AUTH, (void *)pin );

            powermgm_resume_from_ISR();
            return( pass_key );
        };

        void onPassKeyNotify( uint32_t pass_key ){
            char pin[16]="";
            snprintf( pin, sizeof( pin ), "%06d", pass_key );
            log_d("BLECTL pairing request, PIN: %s", pin );

            blectl_set_event( BLECTL_PIN_AUTH );
            blectl_send_event_cb( BLECTL_PIN_AUTH, (void *)pin );

            powermgm_resume_from_ISR();
        };
        
        bool onConfirmPIN( uint32_t pass_key ) {
            char pin[16]="";
            snprintf( pin, sizeof( pin ), "%06d", pass_key );
            log_d("BLECTL confirm PIN: %s", pin );

            powermgm_resume_from_ISR();

            return( true );
        };

        void onAuthenticationComplete(ble_gap_conn_desc* desc){
            if(!desc->sec_state.encrypted) {
                if ( blectl_get_event( BLECTL_PIN_AUTH ) ) {
                    log_d("BLECTL pairing abort, reason: %02x", cmpl.fail_reason );
                    blectl_clear_event( BLECTL_PIN_AUTH );
                    blectl_send_event_cb( BLECTL_PAIRING_ABORT, (void *)"abort" );
                    NimBLEDevice::getServer()->disconnect( desc->conn_handle );
                    return;
                }
                if ( blectl_get_event( BLECTL_AUTHWAIT | BLECTL_CONNECT ) ) {
                    log_d("BLECTL authentication unsuccessful, client disconnected, reason: %02x", cmpl.fail_reason );
                    blectl_clear_event( BLECTL_AUTHWAIT | BLECTL_CONNECT );
                    blectl_set_event( BLECTL_DISCONNECT );
                    blectl_send_event_cb( BLECTL_DISCONNECT, (void *) "disconnected" );
                    NimBLEDevice::getServer()->disconnect(desc->conn_handle);
                    return;
                }
            }
            else {
                if ( blectl_get_event( BLECTL_PIN_AUTH ) ) {
                    log_d("BLECTL pairing successful");
                    blectl_clear_event( BLECTL_PIN_AUTH );
                    blectl_send_event_cb( BLECTL_PAIRING_SUCCESS, (void *)"success" );
                    return;
                }
                if ( blectl_get_event( BLECTL_AUTHWAIT ) ) {
                    log_d("BLECTL authentication successful, client connected");
                    blectl_clear_event( BLECTL_AUTHWAIT | BLECTL_DISCONNECT );
                    blectl_set_event( BLECTL_CONNECT );
                    blectl_send_event_cb( BLECTL_CONNECT, (void *) "connected" );
                    return;
                }
            }
            powermgm_resume_from_ISR();
        };
    };
#endif

void blectl_setup( void ) {
    #ifdef NATIVE_64BIT
    #else
        /**
         * allocate event group
         */
        blectl_status = xEventGroupCreate();
        ASSERT( blectl_status, "Failed to allocate event group" );
        /**
         *  Create the BLE Device
         */
        char deviceName[ 64 ];
        uint8_t mac[6];
        esp_read_mac(mac, ESP_MAC_BT);
        snprintf(deviceName, sizeof(deviceName), "Espruino (%s-%02X%02X)", 
        device_get_name(), mac[4], mac[5]);
        NimBLEDevice::init( deviceName );
        /*
         * set power level from config
         */
        switch( blectl_config.txpower ) {
            case 0:             NimBLEDevice::setPower( ESP_PWR_LVL_N12 );
                                break;
            case 1:             NimBLEDevice::setPower( ESP_PWR_LVL_N9 );
                                break;
            case 2:             NimBLEDevice::setPower( ESP_PWR_LVL_N6 );
                                break;
            case 3:             NimBLEDevice::setPower( ESP_PWR_LVL_N3 );
                                break;
            case 4:             NimBLEDevice::setPower( ESP_PWR_LVL_N0 );
                                break;
            default:            NimBLEDevice::setPower( ESP_PWR_LVL_N9 );
                                break;
        }
        /*
         * Enable encryption and pairing options
         */
        NimBLEDevice::setSecurityAuth( true, true, true );
        NimBLEDevice::setSecurityIOCap( BLE_HS_IO_DISPLAY_ONLY );
        /*
         * Create the BLE Server
         */
        pServer = NimBLEDevice::createServer();
        pServer->setCallbacks( new ServerCallbacks() );
        pServer->advertiseOnDisconnect( false );
        pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->setAppearance( 0x00c0 );
        /**
         * add services
         */
        deviceinfo_setup();
        gadgetbridge_setup();
        parkinsense_ble_setup();
        blebatctl_setup();
        blestepctl_setup();
        /*
         * Start advertising with power-efficient intervals
         */
        pAdvertising->setMinInterval(800);
        pAdvertising->setMaxInterval(1600);
        log_i( "[PWR_BLE] setup: intervals set; host will not auto-advertise on every disconnect" );
    #endif

    log_i("[PWR_BLE] setup: config autoon=%d advertising_cfg=%d enable_on_standby=%d disable_only_disc=%d",
        (int)blectl_get_autoon(), (int)blectl_get_advertising(),
        (int)blectl_get_enable_on_standby(), (int)blectl_get_disable_only_disconnected() );

    #ifdef NATIVE_64BIT
        if( blectl_get_autoon() ) {
            blectl_set_event( BLECTL_ON );
            blectl_clear_event( BLECTL_OFF );
        } else
            blectl_off();
    #else
        if( blectl_get_autoon() ) {
            blectl_may_start_ad = false;
            blectl_config.autoon = true;
            blectl_set_event( BLECTL_ON );
            blectl_clear_event( BLECTL_OFF );
            pAdvertising->stop();
            log_i( "[PWR_BLE] setup: autoon in config; advertising off until WAKEUP, BLE control, or hourly hook" );
        } else
            blectl_off();
    #endif

    powermgm_register_cb_with_prio( POWERMGM_STANDBY, blectl_powermgm_event_cb, "powermgm blectl", CALL_CB_FIRST );
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP, blectl_powermgm_event_cb, "powermgm blectl" );
}

bool blectl_powermgm_event_cb( EventBits_t event, void *arg ) {
    bool retval = true;

    switch( event ) {
        case POWERMGM_STANDBY: {
            #ifndef NATIVE_64BIT
                int nconn = pServer ? (int)pServer->getConnectedCount() : -1;
            #else
                int nconn = -1;
            #endif
            const bool on = blectl_get_event( BLECTL_ON );
            const bool conn = blectl_get_event( BLECTL_CONNECT );
            const bool en_stby = blectl_get_enable_on_standby();
            const bool only_disc = blectl_get_disable_only_disconnected();

            if ( en_stby && on ) {
                retval = false;
                log_w("standby blocked by \"enable_on_standby\" option");
            }
            else if ( only_disc && conn ) {
                retval = false;
                log_w("standby blocked by \"disable_only_disconnected\" option");
            }
            else {
                log_d("go standby");
            }
            log_i("[PWR_BLE] STANDBY cb: BLECTL_ON=%d BLECTL_CONNECT=%d nconn=%d enable_on_standby=%d stayon_if_conn=%d -> powermgm_allow_sleep=%d (note: BLE stack not stopped here)",
                (int)on, (int)conn, nconn, (int)en_stby, (int)only_disc, (int)retval );
            break;
        }
        case POWERMGM_WAKEUP:
            #ifndef NATIVE_64BIT
                if ( pServer && pAdvertising ) {
                    blectl_may_start_ad = true;
                    blectl_privileged_start_advertising_if_runnable( "user WAKEUP (device in use)" );
                }
            #endif
            log_i( "[PWR_BLE] WAKEUP: may_start applied on device; ad only if autoon+visibility" );
            break;
        case POWERMGM_SILENCE_WAKEUP:
            log_i("[PWR_BLE] SILENCE_WAKEUP cb (BLE unchanged by this callback)");
            break;
    }
    return( retval );
}

void blectl_set_event( EventBits_t bits ) {
    #ifdef NATIVE_64BIT
        blectl_status |= bits;
    #else
        portENTER_CRITICAL( &blectlMux );
        xEventGroupSetBits( blectl_status, bits );
        portEXIT_CRITICAL( &blectlMux );
    #endif
}

void blectl_clear_event( EventBits_t bits ) {
    #ifdef NATIVE_64BIT
        blectl_status &= ~bits;
    #else
        portENTER_CRITICAL( &blectlMux );
        xEventGroupClearBits( blectl_status, bits );
        portEXIT_CRITICAL( &blectlMux );
    #endif
}

bool blectl_get_event( EventBits_t bits ) {
    EventBits_t temp;
    
    #ifdef NATIVE_64BIT
        temp = blectl_status & bits;
    #else
        portENTER_CRITICAL( &blectlMux );
        temp = xEventGroupGetBits( blectl_status ) & bits;
        portEXIT_CRITICAL( &blectlMux );
    #endif

    return( temp );
}

bool blectl_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( blectl_callback == NULL ) {
        blectl_callback = callback_init( "blectl" );
        ASSERT( blectl_callback, "blectl callback alloc failed" );
    }    
    return( callback_register( blectl_callback, event, callback_func, id ) );
}

static bool blectl_send_event_cb( EventBits_t event, void *arg ) {
    return( callback_send( blectl_callback, event, arg ) );
}

void blectl_set_enable_on_standby( bool enable_on_standby ) {        
    blectl_config.enable_on_standby = enable_on_standby;
    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_set_disable_only_disconnected( bool disable_only_disconnected ) {        
    blectl_config.disable_only_disconnected = disable_only_disconnected;
    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_set_wakeup_on_notification( bool wakeup_on_notification ) {        
    blectl_config.wakeup_on_notification = wakeup_on_notification;
    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_set_show_notification( bool show_notification ) {        
    blectl_config.show_notification = show_notification;
    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_set_vibe_notification( bool vibe_notification ) {        
    blectl_config.vibe_notification = vibe_notification;
    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_set_sound_notification( bool sound_notification ) {        
    blectl_config.sound_notification = sound_notification;
    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_set_media_notification( bool media_notification ) {        
    blectl_config.media_notification = media_notification;
    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_set_timesync( bool timesync ) {
    blectl_config.timesync = timesync;
    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_set_advertising( bool advertising ) {  
    blectl_config.advertising = advertising;
    blectl_config.save();

    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );

    if ( blectl_get_event( BLECTL_CONNECT ) )
        return;

    #ifdef NATIVE_64BIT
    #else
        if ( advertising ) {
            blectl_may_start_ad = true;
            pServer->getAdvertising()->start();
        }
        else {
            pServer->getAdvertising()->stop();
        }
    #endif
}

void blectl_set_txpower( int32_t txpower ) {
    /**
     * check if tx power setting in range
     */
    if ( txpower >= 0 && txpower <= 4 )
        blectl_config.txpower = txpower;
    /**
     * set tx power
     */
    #ifdef NATIVE_64BIT
    #else
        switch( blectl_config.txpower ) {
            case 0:             NimBLEDevice::setPower( ESP_PWR_LVL_N12 );
                                break;
            case 1:             NimBLEDevice::setPower( ESP_PWR_LVL_N9 );
                                break;
            case 2:             NimBLEDevice::setPower( ESP_PWR_LVL_N6 );
                                break;
            case 3:             NimBLEDevice::setPower( ESP_PWR_LVL_N3 );
                                break;
            case 4:             NimBLEDevice::setPower( ESP_PWR_LVL_N0 );
                                break;
            default:            NimBLEDevice::setPower( ESP_PWR_LVL_N9 );
                                break;
        }
    #endif

    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_set_autoon( bool autoon ) {
    blectl_config.autoon = autoon;

    if( autoon )
        blectl_on();
    else
        blectl_off();

    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

int32_t blectl_get_txpower( void ) {
    return( blectl_config.txpower );
}

bool blectl_get_enable_on_standby( void ) {
    return( blectl_config.enable_on_standby );
}

bool blectl_get_disable_only_disconnected( void ) {
    return( blectl_config.disable_only_disconnected );
}

bool blectl_get_wakeup_on_notification( void ) {
    return( blectl_config.wakeup_on_notification );
}

bool blectl_get_show_notification( void ) {
    return( blectl_config.show_notification );
}

bool blectl_get_vibe_notification( void ) {
    return( blectl_config.vibe_notification );
}

bool blectl_get_sound_notification( void ) {
    return( blectl_config.sound_notification );
}

bool blectl_get_media_notification( void ) {
    return( blectl_config.media_notification );
}

bool blectl_get_autoon( void ) {
    return( blectl_config.autoon );
}

bool blectl_get_timesync( void ) {
    return( blectl_config.timesync );
}

bool blectl_get_advertising( void ) {
    return( blectl_config.advertising );
}

blectl_custom_audio* blectl_get_custom_audio_notifications( void ) {
    return( blectl_config.custom_audio_notifications );
}

void blectl_save_config( void ) {
    blectl_config.save();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_read_config( void ) {
    blectl_config.load();
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_on( void ) {
    blectl_config.autoon = true;

    #ifdef NATIVE_64BIT
    #else
        blectl_may_start_ad = true;
        blectl_apply_advertising_from_config();
    #endif

    blectl_set_event( BLECTL_ON );
    blectl_clear_event( BLECTL_OFF );
    blectl_send_event_cb( BLECTL_ON, (void *)NULL );
}

void blectl_off( void ) {
    blectl_config.autoon = false;

    #ifdef NATIVE_64BIT
    #else
        blectl_may_start_ad = false;
        blectl_light_sleep_disconnect_in_progress = false;
        blectl_disconnect_all_or_log( "blectl_off" );
        if ( pAdvertising ) {
            pAdvertising->stop();
        }
        log_i( "[PWR_BLE] blectl_off: advertising stopped; NimBLE still initialized" );
    #endif

    blectl_set_event( BLECTL_OFF );
    blectl_clear_event( BLECTL_ON );
    blectl_send_event_cb( BLECTL_OFF, (void *)NULL );
    blectl_send_event_cb( BLECTL_CONFIG_UPDATE, NULL );
}

void blectl_before_light_sleep( void ) {
    #ifdef NATIVE_64BIT
        return;
    #else
        blectl_may_start_ad = false;
        blectl_light_sleep_disconnect_in_progress = true;
        blectl_disconnect_all_or_log( "light sleep" );
        if ( pAdvertising ) {
            pAdvertising->stop();
        }
        log_i( "[PWR_BLE] before_light_sleep: disconnect + ad stopped; may_start=0 (no ad until WAKEUP/toggle/hourly)" );
    #endif
}

void blectl_after_light_sleep( void ) {
    #ifdef NATIVE_64BIT
        return;
    #else
        if ( !pServer || !pAdvertising ) {
            return;
        }
        const size_t npeers = pServer->getConnectedCount();
        if ( npeers > 0 ) {
            log_i( "[PWR_BLE] after_light_sleep: %u link(s) still up (onDisconnect will clear wait flags; no ad until permitted)", (unsigned)npeers );
            return;
        }
        if ( blectl_light_sleep_disconnect_in_progress && pServer->getConnectedCount() == 0 ) {
            blectl_light_sleep_disconnect_in_progress = false;
        }
    #endif
}

void blectl_on_hourly_advertise_window( void ) {
    #ifdef NATIVE_64BIT
        return;
    #else
        blectl_may_start_ad = true;
        blectl_privileged_start_advertising_if_runnable( "hourly window (hook)" );
    #endif
}

#ifdef NATIVE_64BIT
#else
    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #elif defined( WT32_SC01 )
    #endif
    NimBLEServer *blectl_get_ble_server( void ) {
        return pServer;
    }

    NimBLEAdvertising *blectl_get_ble_advertising( void ) {
    return pAdvertising;
    }
#endif
