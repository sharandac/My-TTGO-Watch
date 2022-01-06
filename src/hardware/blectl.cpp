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
#include "utils/charbuffer.h"
#include "utils/alloc.h"
#include "utils/bluejsonrequest.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"

    static EventBits_t blectl_status;
#else
    #ifdef M5PAPER

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )

    #else
        #warning "no hardware driver for blectl"
    #endif
    #include <Arduino.h>
    #include <BLEDevice.h>
    #include <BLEServer.h>
    #include <BLEUtils.h>
    #include <BLE2902.h>
    #include "blebatctl.h"
    #include "blestepctl.h"

    EventGroupHandle_t blectl_status = NULL;
    portMUX_TYPE DRAM_ATTR blectlMux = portMUX_INITIALIZER_UNLOCKED;
    QueueHandle_t blectl_msg_queue;
#endif

blectl_config_t blectl_config;
blectl_msg_t blectl_msg;
callback_t *blectl_callback = NULL;
uint8_t txValue = 0;

bool blectl_send_event_cb( EventBits_t event, void *arg );
bool blectl_powermgm_event_cb( EventBits_t event, void *arg );
bool blectl_powermgm_loop_cb( EventBits_t event, void *arg );
bool blectl_pmu_event_cb( EventBits_t event, void *arg );
void blectl_send_next_msg( char *msg );
void blectl_loop( void );

#ifdef NATIVE_64BIT
#else
    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #endif
    BLEServer *pServer = NULL;
    BLECharacteristic *pTxCharacteristic;
    BLECharacteristic *pRxCharacteristic;

    static CharBuffer gadgetbridge_msg;

    class BleCtlServerCallbacks: public BLEServerCallbacks {
        void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t* param ) {
            pServer->updateConnParams( param->connect.remote_bda, 1450, 1500, 0, 10000 );
            blectl_set_event( BLECTL_AUTHWAIT );
            blectl_clear_event( BLECTL_DISCONNECT | BLECTL_CONNECT );
            xQueueReset( blectl_msg_queue );
            log_i("BLE authwait");
            blectl_send_event_cb( BLECTL_AUTHWAIT, (void *)"authwait" );
            pServer->getAdvertising()->stop();
        };

        void onDisconnect(BLEServer* pServer) {
            log_i("BLE disconnected");
            blectl_set_event( BLECTL_DISCONNECT );
            blectl_clear_event( BLECTL_CONNECT | BLECTL_AUTHWAIT );
            blectl_send_event_cb( BLECTL_DISCONNECT, (void *)"disconnected" );
            xQueueReset( blectl_msg_queue );
            blectl_msg.active = false;

            if ( blectl_get_advertising() ) {
                pServer->getAdvertising()->start();
                log_i("BLE advertising...");
            }
        }
    };

    class BtlCtlSecurity : public BLESecurityCallbacks {

        uint32_t onPassKeyRequest() {
            log_i("BLECTL pass key request");
            return 123456;
        }
        void onPassKeyNotify( uint32_t pass_key ){
            char pin[16]="";
            snprintf( pin, sizeof( pin ), "%06d", pass_key );
            log_i("BLECTL pairing request, PIN: %s", pin );
            blectl_set_event( BLECTL_PIN_AUTH );
            blectl_send_event_cb( BLECTL_PIN_AUTH, (void *)pin );
        }
        bool onConfirmPIN( uint32_t pass_key ) {
            char pin[16]="";
            snprintf( pin, sizeof( pin ), "%06d", pass_key );
            log_i("BLECTL confirm PIN: %s", pin );
            return false;
        }
        bool onSecurityRequest() {
            log_i("BLECTL security request");
            return true;
        }

        void onAuthenticationComplete( esp_ble_auth_cmpl_t cmpl ){

            if( cmpl.success ){
                if ( blectl_get_event( BLECTL_PIN_AUTH ) ) {
                    log_i("BLECTL pairing successful");
                    blectl_clear_event( BLECTL_PIN_AUTH );
                    blectl_send_event_cb( BLECTL_PAIRING_SUCCESS, (void *)"success" );
                    return;
                }
                if ( blectl_get_event( BLECTL_AUTHWAIT ) ) {
                    log_i("BLECTL authentication successful, client connected");
                    blectl_clear_event( BLECTL_AUTHWAIT | BLECTL_DISCONNECT );
                    blectl_set_event( BLECTL_CONNECT );
                    blectl_send_event_cb( BLECTL_CONNECT, (void *) "connected" );
                    return;
                }
            }
            else {
                if ( blectl_get_event( BLECTL_PIN_AUTH ) ) {
                    log_i("BLECTL pairing abort, reason: %02x", cmpl.fail_reason );
                    blectl_clear_event( BLECTL_PIN_AUTH );
                    blectl_send_event_cb( BLECTL_PAIRING_ABORT, (void *)"abort" );
                    pServer->startAdvertising();
                    return;
                }
                if ( blectl_get_event( BLECTL_AUTHWAIT | BLECTL_CONNECT ) ) {
                    log_i("BLECTL authentication unsuccessful, client disconnected, reason: %02x", cmpl.fail_reason );
                    blectl_clear_event( BLECTL_AUTHWAIT | BLECTL_CONNECT );
                    blectl_set_event( BLECTL_DISCONNECT );
                    blectl_send_event_cb( BLECTL_DISCONNECT, (void *) "disconnected" );
                    pServer->startAdvertising();
                    return;
                }
            }
            
            log_e("authentication not handle but %s. reason: %02x", cmpl.success ? "successful" : "not successful", cmpl.fail_reason );
        }
    };

    class BleCtlCallbacks : public BLECharacteristicCallbacks
    {
        void onWrite( BLECharacteristic *pCharacteristic ) {
            size_t msgLen = pCharacteristic->getValue().length();
            const char *msg = pCharacteristic->getValue().c_str();

            log_i("receive %d bytes msg chunk", msgLen );

            for ( int i = 0 ; i < msgLen ; i++ ) {
                switch( msg[ i ] ) {
                    case EndofText:         gadgetbridge_msg.clear();
                                            log_i("attention, new link establish");
                                            blectl_send_event_cb( BLECTL_CONNECT, (void *)"connected" );
                                            break;
                    case DataLinkEscape:    gadgetbridge_msg.clear();
                                            log_i("attention, new message");
                                            break;
                    case LineFeed:          {
                                                log_i("attention, message complete");
                                                const char *gbmsg = gadgetbridge_msg.c_str();
                                                log_i( "msg: %s", gbmsg );
                                                if( gbmsg[ 0 ] == 'G' && gbmsg[ 1 ] == 'B' ) {
                                                    log_i("gadgetbridge message identified, cut down to json");
                                                    gadgetbridge_msg.erase( gadgetbridge_msg.length() - 1 );
                                                    gbmsg += 3;
                                                    BluetoothJsonRequest request( gbmsg, strlen( gbmsg ) * 4 );
                                                    if ( request.isValid() ) {
                                                        blectl_send_event_cb( BLECTL_MSG_JSON, (void *)&request );
                                                    }
                                                    else {
                                                        blectl_send_event_cb( BLECTL_MSG, (void *)gbmsg );
                                                    }        
                                                    request.clear();
                                                }
                                                else {
                                                    blectl_send_event_cb( BLECTL_MSG, (void *)gbmsg );
                                                }
                                                break;
                                            }
                    default:                gadgetbridge_msg.append( msg[ i ] );
                }
            }
        }

        void onRead( BLECharacteristic* pCharacteristic ) {
            std::string msg = pCharacteristic->getValue();
            log_i("BLE received: %s, %i\n", msg.c_str(), msg.length() );
        }
    };
#endif

void blectl_setup( void ) {
    blectl_msg.active = false;
    blectl_msg.msg = NULL;
    blectl_msg.msglen = 0;
    blectl_msg.msgpos = 0;

    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        blectl_status = xEventGroupCreate();

        esp_bt_controller_enable( ESP_BT_MODE_BLE );
        esp_bt_controller_mem_release( ESP_BT_MODE_CLASSIC_BT );
        esp_bt_mem_release( ESP_BT_MODE_CLASSIC_BT );
        blectl_msg_queue = xQueueCreate( 5, sizeof( char * ) );
        if ( blectl_msg_queue == NULL ) {
            log_e("Failed to allocate msg queue");
            while(true);
        }

        /**
         *  Create the BLE Device
         * Name needs to match filter in Gadgetbridge's banglejs getSupportedType() function.
         * This is too long I think:
         * BLEDevice::init("Espruino Gadgetbridge Compatible Device");
         */
        BLEDevice::init("Espruino (" HARDWARE_NAME ")" );
        /*
         * The minimum power level (-12dbm) ESP_PWR_LVL_N12 was too low
         */
        switch( blectl_config.txpower ) {
            case 0:             BLEDevice::setPower( ESP_PWR_LVL_N12 );
                                break;
            case 1:             BLEDevice::setPower( ESP_PWR_LVL_N9 );
                                break;
            case 2:             BLEDevice::setPower( ESP_PWR_LVL_N6 );
                                break;
            case 3:             BLEDevice::setPower( ESP_PWR_LVL_N3 );
                                break;
            case 4:             BLEDevice::setPower( ESP_PWR_LVL_N0 );
                                break;
            default:            BLEDevice::setPower( ESP_PWR_LVL_N9 );
                                break;
        }
        /*
         * Enable encryption
         */
        BLEDevice::setEncryptionLevel( ESP_BLE_SEC_ENCRYPT_NO_MITM );
        BLEDevice::setSecurityCallbacks( new BtlCtlSecurity() );
        /*
         * Enable authentication
         */
        BLESecurity *pSecurity = new BLESecurity();
        pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
        pSecurity->setCapability(ESP_IO_CAP_OUT);
        pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
        pSecurity->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
        /*
         * Create the BLE Server
         */
        pServer = BLEDevice::createServer();
        pServer->setCallbacks( new BleCtlServerCallbacks() );
        /*
         * Create the BLE Service
         */
        BLEService *pService = pServer->createService(SERVICE_UUID);
        /*
         * Create a BLE Characteristic
         */
        pTxCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY );
        pTxCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
        pTxCharacteristic->addDescriptor( new BLE2902() );
        pRxCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE );
        pRxCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
        pRxCharacteristic->setCallbacks( new BleCtlCallbacks() );
        /*
         * Start the service
         */
        pService->start();
        /*
         * Start advertising
         * ESP_BLE_APPEARANCE_GENERIC_WATCH
         */
        BLEAdvertising *pAdvertising = pServer->getAdvertising();
        pAdvertising->addServiceUUID( pService->getUUID() );
        pAdvertising->setAppearance( ESP_BLE_APPEARANCE_GENERIC_WATCH );
        /*
         * Create device information service
         */
        BLEService *pDeviceInformationService = pServer->createService(DEVICE_INFORMATION_SERVICE_UUID);
        /*
         * Create manufacturer name string Characteristic - 
         */
        BLECharacteristic* pManufacturerNameStringCharacteristic = pDeviceInformationService->createCharacteristic( MANUFACTURER_NAME_STRING_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ );
        pManufacturerNameStringCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
        pManufacturerNameStringCharacteristic->addDescriptor( new BLE2902() );
        pManufacturerNameStringCharacteristic->setValue("Lily Go");
        /*
         * Create manufacturer name string Characteristic - 
         */
        BLECharacteristic* pFirmwareRevisionStringCharacteristic = pDeviceInformationService->createCharacteristic( FIRMWARE_REVISION_STRING_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ );
        pFirmwareRevisionStringCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
        pFirmwareRevisionStringCharacteristic->addDescriptor( new BLE2902() );
        pFirmwareRevisionStringCharacteristic->setValue(__FIRMWARE__);
        /*
         * Start battery service
         */
        pDeviceInformationService->start();
        /*
         * Start advertising battery service
         */
        pAdvertising->addServiceUUID( pDeviceInformationService->getUUID() );
        blebatctl_setup(pServer);
        blestepctl_setup();
        /*
         * Slow advertising interval for battery life
         */
        pAdvertising->setMinInterval( 700 );
        pAdvertising->setMaxInterval( 800 );
    #endif

    if ( blectl_get_autoon() ) {
        blectl_on();
    }
    powermgm_register_cb_with_prio( POWERMGM_STANDBY, blectl_powermgm_event_cb, "powermgm blectl", CALL_CB_FIRST );
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP, blectl_powermgm_event_cb, "powermgm blectl" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, blectl_powermgm_loop_cb, "powermgm blectl loop" );
}

bool blectl_powermgm_event_cb( EventBits_t event, void *arg ) {
    bool retval = true;

    switch( event ) {
        case POWERMGM_STANDBY:          
            if ( blectl_get_enable_on_standby() && blectl_get_event( BLECTL_ON ) ) {
                retval = false;
                log_w("standby blocked by \"enable_on_standby\" option");
            }
            else if ( blectl_get_disable_only_disconnected() && blectl_get_event( BLECTL_CONNECT ) ) {
                retval = false;
                log_w("standby blocked by \"disable_only_disconnected\" option");
            }
            else {
                log_i("go standby");
            }
            break;
        case POWERMGM_WAKEUP:           
            log_i("go wakeup");
            break;
        case POWERMGM_SILENCE_WAKEUP:   
            log_i("go silence wakeup");
            break;
    }
    return( retval );
}

bool blectl_powermgm_loop_cb( EventBits_t event, void *arg ) {
    blectl_loop();
    return( true );
}

void blectl_set_event( EventBits_t bits ) {
    #ifdef NATIVE_64BIT
        blectl_status |= bits;
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        portENTER_CRITICAL(&blectlMux);
        xEventGroupSetBits( blectl_status, bits );
        portEXIT_CRITICAL(&blectlMux);
    #endif
}

void blectl_clear_event( EventBits_t bits ) {
    #ifdef NATIVE_64BIT
        blectl_status &= ~bits;
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        portENTER_CRITICAL(&blectlMux);
        xEventGroupClearBits( blectl_status, bits );
        portEXIT_CRITICAL(&blectlMux);
    #endif
}

bool blectl_get_event( EventBits_t bits ) {
    EventBits_t temp;
    
    #ifdef NATIVE_64BIT
        temp = blectl_status & bits;
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        portENTER_CRITICAL(&blectlMux);
        temp = xEventGroupGetBits( blectl_status ) & bits;
        portEXIT_CRITICAL(&blectlMux);
    #endif

    return( temp );
}

bool blectl_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( blectl_callback == NULL ) {
        blectl_callback = callback_init( "blectl" );
        if ( blectl_callback == NULL ) {
            log_e("blectl callback alloc failed");
            while(true);
        }
    }    
    return( callback_register( blectl_callback, event, callback_func, id ) );
}

bool blectl_send_event_cb( EventBits_t event, void *arg ) {
    return( callback_send( blectl_callback, event, arg ) );
}

void blectl_set_enable_on_standby( bool enable_on_standby ) {        
    blectl_config.enable_on_standby = enable_on_standby;
    blectl_config.save();
}

void blectl_set_disable_only_disconnected( bool disable_only_disconnected ) {        
    blectl_config.disable_only_disconnected = disable_only_disconnected;
    blectl_config.save();
}

void blectl_set_show_notification( bool show_notification ) {        
    blectl_config.show_notification = show_notification;
    blectl_config.save();
}

void blectl_set_advertising( bool advertising ) {  
    blectl_config.advertising = advertising;
    blectl_config.save();
    if ( blectl_get_event( BLECTL_CONNECT ) )
        return;

    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        if ( advertising ) {
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
    if ( txpower >= 0 && txpower <= 4 ) {
        blectl_config.txpower = txpower;
    }
    /**
     * set tx power
     */
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        switch( blectl_config.txpower ) {
            case 0:             BLEDevice::setPower( ESP_PWR_LVL_N12 );
                                break;
            case 1:             BLEDevice::setPower( ESP_PWR_LVL_N9 );
                                break;
            case 2:             BLEDevice::setPower( ESP_PWR_LVL_N6 );
                                break;
            case 3:             BLEDevice::setPower( ESP_PWR_LVL_N3 );
                                break;
            case 4:             BLEDevice::setPower( ESP_PWR_LVL_N0 );
                                break;
            default:            BLEDevice::setPower( ESP_PWR_LVL_N9 );
                                break;
        }
    #endif
    blectl_config.save();
}

void blectl_set_autoon( bool autoon ) {
    blectl_config.autoon = autoon;

    if( autoon ) {
        blectl_on();
    }
    else {
        blectl_off();
    }
    blectl_config.save();
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

bool blectl_get_show_notification( void ) {
    return( blectl_config.show_notification );
}

bool blectl_get_autoon( void ) {
    return( blectl_config.autoon );
}

bool blectl_get_advertising( void ) {
    return( blectl_config.advertising );
}

blectl_custom_audio* blectl_get_custom_audio_notifications( void ) {
    return( blectl_config.custom_audio_notifications );
}

void blectl_save_config( void ) {
    blectl_config.save();
}

void blectl_read_config( void ) {
    blectl_config.load();
}

bool blectl_send_msg( const char *msg ) {
    #ifdef NATIVE_64BIT
        return( false );
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        if ( blectl_get_event( BLECTL_CONNECT | BLECTL_AUTHWAIT ) ) {
            /*
            * Duplicate message
            */
            size_t len = strlen( msg );
            char *buff = (char *)CALLOC( len + 1, 1 );
            if ( buff == NULL ) {
                log_e("buff calloc failed");
                while( true );
            }
            strcpy( buff, msg );
            /*
            * Send message
            */
            BaseType_t ret;
            ret = xQueueSend( blectl_msg_queue, &buff, 0);
            /*
            * buff will be freeed on the receive part
            */
            buff = NULL;
            if ( ret != pdTRUE ) {
                log_e("fail to send msg");
                return false;
            }
            return true;
        }
        else {
            log_e("msg can't send while bluetooth is not connected");
            return false;
        }
    #endif
}

void blectl_send_next_msg( char *msg ) {
    if ( !blectl_msg.active && blectl_get_event( BLECTL_CONNECT ) ) {

        if ( blectl_msg.msg == NULL ) { 
            blectl_msg.msg = (char *)CALLOC( BLECTL_MSG_MTU, 1 );
            if ( blectl_msg.msg == NULL ) {
                log_e("blectl_msg.msg calloc failed");
                while( true );
            }
        }

        strncpy( blectl_msg.msg, msg, BLECTL_MSG_MTU );
        blectl_msg.active = true;
        blectl_msg.msglen = strlen( (const char*)msg ) ;
        blectl_msg.msgpos = 0;
    }
    else {
        log_e("blectl is send another msg or not connected");
        blectl_send_event_cb( BLECTL_MSG_SEND_ABORT , (char*)"msg send abort, blectl is send another msg or not connected" );
        return;
    }
}

void blectl_on( void ) {
    blectl_config.autoon = true;

    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        if ( blectl_config.advertising ) {
            pServer->getAdvertising()->start();
        }
        else {
            pServer->getAdvertising()->stop();
        }
    #endif

    blectl_set_event( BLECTL_ON );
    blectl_clear_event( BLECTL_OFF );
    blectl_send_event_cb( BLECTL_ON, (void *)NULL );
}

void blectl_off( void ) {
    blectl_config.autoon = false;

    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        pServer->getAdvertising()->stop();
    #endif

    blectl_set_event( BLECTL_OFF );
    blectl_clear_event( BLECTL_ON );
    blectl_send_event_cb( BLECTL_OFF, (void *)NULL );
}

static void blectl_send_chunk ( int32_t len ) {
    /*
     * send msg chunk
     */
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        pTxCharacteristic->setValue( (unsigned char*)&blectl_msg.msg[ blectl_msg.msgpos ], len );
        pTxCharacteristic->notify();
    #endif

    char chunk_msg[ 64 ] = "";
    for( int i = 0 ; i < len ; i++ ) {
        if ( blectl_msg.msg[ blectl_msg.msgpos + i ] > 0x1F ) {
            chunk_msg[ i ] = blectl_msg.msg[ blectl_msg.msgpos + i ];
        }
        else {
            chunk_msg[ i ] = '?';
        }
    }
    chunk_msg[ len ] = '\0';
    log_i("send %2dbyte [ \"%s\" ] chunk", len, chunk_msg );
}

void blectl_loop ( void ) {
    static uint64_t nextmillis = 0;

    if ( !blectl_get_event( BLECTL_CONNECT ) ) {
        return;
    }

    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
        if ( !blectl_msg.active ) {
            // Retrieve next message
            char *msg;
            BaseType_t available;
            available = xQueueReceive( blectl_msg_queue, &msg, 0);
            if ( available == pdTRUE ) {
                blectl_send_next_msg( msg );
                free( msg );
            }
        }
    #endif

    if ( blectl_msg.active && nextmillis < millis() ) {
        nextmillis = millis() + BLECTL_CHUNKDELAY;
        if ( blectl_msg.msgpos < blectl_msg.msglen ) {
            if ( ( blectl_msg.msglen - blectl_msg.msgpos ) > BLECTL_CHUNKSIZE ) {
                blectl_send_chunk ( BLECTL_CHUNKSIZE );
                blectl_msg.msgpos += BLECTL_CHUNKSIZE;
            }
            else if ( ( blectl_msg.msglen - blectl_msg.msgpos ) > 0 ) {
                blectl_send_chunk ( blectl_msg.msglen - blectl_msg.msgpos );
                blectl_send_event_cb( BLECTL_MSG_SEND_SUCCESS , (char*)"msg send success" );
                blectl_msg.active = false;
                blectl_msg.msglen = 0;
                blectl_msg.msgpos = 0;
            }
            else {
                log_e("malformed chunksize");
                blectl_send_event_cb( BLECTL_MSG_SEND_ABORT , (char*)"msg send abort, malformed chunksize" );
                blectl_msg.active = false;
                blectl_msg.msglen = 0;
                blectl_msg.msgpos = 0;
            }
        }
        else {
            blectl_msg.active = false;
            blectl_msg.msglen = 0;
            blectl_msg.msgpos = 0;
        }
    }
}

#ifdef NATIVE_64BIT
#else
    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #endif
    BLEServer *blectl_get_ble_server( void ) {
        return pServer;
    }

    BLEAdvertising *blectl_get_ble_advertising( void ) {
    return pServer->getAdvertising();
    }
#endif
