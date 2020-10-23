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
#include "Arduino.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "blectl.h"
#include "pmu.h"
#include "powermgm.h"
#include "callback.h"
#include "json_psram_allocator.h"
#include "alloc.h"
#include "msg_chain.h"

#include "gui/statusbar.h"

EventGroupHandle_t blectl_status = NULL;
portMUX_TYPE DRAM_ATTR blectlMux = portMUX_INITIALIZER_UNLOCKED;

blectl_config_t blectl_config;
blectl_msg_t blectl_msg;
msg_chain_t *blectl_msg_chain = NULL;

callback_t *blectl_callback = NULL;

bool blectl_send_event_cb( EventBits_t event, void *arg );
bool blectl_powermgm_event_cb( EventBits_t event, void *arg );
bool blectl_powermgm_loop_cb( EventBits_t event, void *arg );
bool blectl_pmu_event_cb( EventBits_t event, void *arg );
void blectl_send_next_msg( char *msg );
void blectl_loop( void );

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
BLECharacteristic *pRxCharacteristic;
uint8_t txValue = 0;

BLECharacteristic *pBatteryLevelCharacteristic;
BLECharacteristic *pBatteryPowerStateCharacteristic;

char *gadgetbridge_msg = NULL;
uint32_t gadgetbridge_msg_size = 0;

class BleCtlServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t* param ) {
        blectl_set_event( BLECTL_CONNECT );
        blectl_clear_event( BLECTL_DISCONNECT );
        log_i("BLE connected");

        pServer->getAdvertising()->stop();
    };

    void onDisconnect(BLEServer* pServer) {
        blectl_set_event( BLECTL_DISCONNECT );
        blectl_clear_event( BLECTL_CONNECT );
        blectl_send_event_cb( BLECTL_DISCONNECT, (void *)"disconnected" );
        blectl_msg.active = false;
        log_i("BLE disconnected");

        if ( blectl_get_advertising() ) {
            pServer->getAdvertising()->start();
            log_i("BLE advertising...");
        }
    }
};

class BtlCtlSecurity : public BLESecurityCallbacks {

    uint32_t onPassKeyRequest(){
        return 123456;
    }
    void onPassKeyNotify(uint32_t pass_key){
        char pin[16]="";
        snprintf( pin, sizeof( pin ), "%06d", pass_key );
        blectl_set_event( BLECTL_PIN_AUTH );
        blectl_send_event_cb( BLECTL_PIN_AUTH, (void *)pin );
        log_i("Bluetooth Pairing Request\r\nPIN: %s", pin );
    }
    bool onConfirmPIN(uint32_t pass_key){
        return false;
    }
    bool onSecurityRequest(){
        return true;
    }

    void onAuthenticationComplete( esp_ble_auth_cmpl_t cmpl ){
        log_i("Bluetooth pairing %s", cmpl.success ? "successful" : "unsuccessful");

        if( cmpl.success ){
            if ( blectl_get_event( BLECTL_PIN_AUTH ) ) {
                blectl_send_event_cb( BLECTL_PAIRING_SUCCESS, (void *)"success" );
            }
        }
        else {
            if ( blectl_get_event( BLECTL_PIN_AUTH ) ) {
                blectl_send_event_cb( BLECTL_PAIRING_ABORT, (void *)"abort" );
            }
            pServer->startAdvertising();
        }

        if ( blectl_get_event( BLECTL_PIN_AUTH ) ) {
            blectl_clear_event( BLECTL_PIN_AUTH );
        }
    }
};

void blectl_add_char_to_gadgetbridge_msg( char msg_char ) {
    gadgetbridge_msg_size++;

    if ( gadgetbridge_msg == NULL ) {
        gadgetbridge_msg = (char *)CALLOC( gadgetbridge_msg_size + 1, 1 );
        if ( gadgetbridge_msg == NULL ) {
            log_e("gadgetbridge_msg alloc fail");
            while(true);
        }
    }
    else {
        char *new_gadgetbridge_msg = NULL;
        new_gadgetbridge_msg = (char *)REALLOC( gadgetbridge_msg, gadgetbridge_msg_size + 1 );
        if ( new_gadgetbridge_msg == NULL ) {
            log_e("gadgetbridge_msg realloc fail");
            while(true);            
        }
        gadgetbridge_msg = new_gadgetbridge_msg;
    }
    gadgetbridge_msg[ gadgetbridge_msg_size - 1 ] = msg_char;
    gadgetbridge_msg[ gadgetbridge_msg_size ] = '\0';
}

void blectl_delete_gadgetbridge_msg ( void ) {
    gadgetbridge_msg_size = 0;

    if ( gadgetbridge_msg == NULL ) {
        gadgetbridge_msg = (char *)CALLOC( gadgetbridge_msg_size + 1, 1 );
        if ( gadgetbridge_msg == NULL ) {
            log_e("gadgetbridge_msg alloc fail");
            while(true);
        }
    }
    else {
        char *new_gadgetbridge_msg = NULL;
        new_gadgetbridge_msg = (char *)REALLOC( gadgetbridge_msg, gadgetbridge_msg_size + 1 );
        if ( new_gadgetbridge_msg == NULL ) {
            log_e("gadgetbridge_msg realloc fail");
            while(true);            
        }
        gadgetbridge_msg = new_gadgetbridge_msg;
    }
    gadgetbridge_msg[ gadgetbridge_msg_size ] = '\0';
}

class BleCtlCallbacks : public BLECharacteristicCallbacks
{
    void onWrite( BLECharacteristic *pCharacteristic ) {
        char *msg = (char *)CALLOC( pCharacteristic->getValue().length() + 1, 1 );
        if ( msg == NULL ) {
            log_e("calloc fail");
            return;
        }
        else {
            strlcpy( msg, pCharacteristic->getValue().c_str(), pCharacteristic->getValue().length() + 1 );
            for ( int i = 0 ; i < pCharacteristic->getValue().length(); i++ ) {
                switch( msg[ i ] ) {
                    case EndofText:         blectl_delete_gadgetbridge_msg();
                                            log_i("attention, new link establish");
                                            blectl_send_event_cb( BLECTL_CONNECT, (void *)"connected" );
                                            break;
                    case DataLinkEscape:    blectl_delete_gadgetbridge_msg();
                                            log_i("attention, new message");
                                            break;
                    case LineFeed:          log_i("message complete, fire BLTCTL_MSG callback");
                                            if( gadgetbridge_msg[ 0 ] == 'G' && gadgetbridge_msg[ 1 ] == 'B' ) {
                                                log_i("gadgetbridge message identified, cut down to json");
                                                gadgetbridge_msg[ gadgetbridge_msg_size - 1 ] = '\0';
                                                log_i("msg: %s", &gadgetbridge_msg[ 3 ] );
                                                blectl_send_event_cb( BLECTL_MSG, (void *)&gadgetbridge_msg[ 3 ] );
                                            }
                                            else {
                                                log_i("msg: %s", gadgetbridge_msg );
                                                blectl_send_event_cb( BLECTL_MSG, (void *)&gadgetbridge_msg[ 0 ] );
                                            }
                                            break;
                    default:                blectl_add_char_to_gadgetbridge_msg( msg[ i ] );
                }
            }
            free(msg);
        }
    }

    void onRead( BLECharacteristic* pCharacteristic ) {
        std::string msg = pCharacteristic->getValue();
        Serial.printf("BLE received: %s, %i\n", msg.c_str(), msg.length());
    }
};

void blectl_setup( void ) {

    blectl_status = xEventGroupCreate();

    esp_bt_controller_enable( ESP_BT_MODE_BLE );
    esp_bt_controller_mem_release( ESP_BT_MODE_CLASSIC_BT );
    esp_bt_mem_release( ESP_BT_MODE_CLASSIC_BT );
    esp_bt_controller_mem_release( ESP_BT_MODE_IDLE );
    esp_bt_mem_release( ESP_BT_MODE_IDLE );

    blectl_msg.active = false;
    blectl_msg.msg = NULL;
    blectl_msg.msglen = 0;
    blectl_msg.msgpos = 0;

    // Create the BLE Device
    // Name needs to match filter in Gadgetbridge's banglejs getSupportedType() function.
    // This is too long I think:
    // BLEDevice::init("Espruino Gadgetbridge Compatible Device");
    BLEDevice::init("Espruino (T-Watch2020)");
    // The minimum power level (-12dbm) ESP_PWR_LVL_N12 was too low
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

    // Enable encryption
    BLEServer* pServer = BLEDevice::createServer();
    BLEDevice::setEncryptionLevel( ESP_BLE_SEC_ENCRYPT_NO_MITM );
    BLEDevice::setSecurityCallbacks( new BtlCtlSecurity() );

    // Enable authentication
    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    pSecurity->setCapability(ESP_IO_CAP_OUT);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    pSecurity->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks( new BleCtlServerCallbacks() );

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);
    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY );
    pTxCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pTxCharacteristic->addDescriptor( new BLE2902() );
    pRxCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE );
    pRxCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pRxCharacteristic->setCallbacks( new BleCtlCallbacks() );
    // Start the service
    pService->start();
    // Start advertising
    pServer->getAdvertising()->addServiceUUID( pService->getUUID() );

    // Create device information service
    BLEService *pDeviceInformationService = pServer->createService(DEVICE_INFORMATION_SERVICE_UUID);
    // Create manufacturer name string Characteristic - 
    BLECharacteristic* pManufacturerNameStringCharacteristic = pDeviceInformationService->createCharacteristic( MANUFACTURER_NAME_STRING_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ );
    pManufacturerNameStringCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pManufacturerNameStringCharacteristic->addDescriptor( new BLE2902() );
    pManufacturerNameStringCharacteristic->setValue("Lily Go");
    // Create manufacturer name string Characteristic - 
    BLECharacteristic* pFirmwareRevisionStringCharacteristic = pDeviceInformationService->createCharacteristic( FIRMWARE_REVISION_STRING_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ );
    pFirmwareRevisionStringCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pFirmwareRevisionStringCharacteristic->addDescriptor( new BLE2902() );
    pFirmwareRevisionStringCharacteristic->setValue(__FIRMWARE__);
    // Start battery service
    pDeviceInformationService->start();
    // Start advertising battery service
    pServer->getAdvertising()->addServiceUUID( pDeviceInformationService->getUUID() );

    // Create battery service
    BLEService *pBatteryService = pServer->createService(BATTERY_SERVICE_UUID);
    // Create a BLE battery service, batttery level Characteristic - 
    pBatteryLevelCharacteristic = pBatteryService->createCharacteristic( BATTERY_LEVEL_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY );
    pBatteryLevelCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pBatteryLevelCharacteristic->addDescriptor( new BLEDescriptor(BATTERY_LEVEL_DESCRIPTOR_UUID) );
    pBatteryLevelCharacteristic->addDescriptor( new BLE2902() );
    pBatteryPowerStateCharacteristic = pBatteryService->createCharacteristic( BATTERY_POWER_STATE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY );
    pBatteryPowerStateCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pBatteryPowerStateCharacteristic->addDescriptor( new BLE2902() );
    // Start battery service
    pBatteryService->start();
    // Start advertising battery service
    pServer->getAdvertising()->addServiceUUID( pBatteryService->getUUID() );

    // Slow advertising interval for battery life
    pServer->getAdvertising()->setMinInterval( 200 );
    pServer->getAdvertising()->setMaxInterval( 300 );

    if ( blectl_get_autoon() ) {
        blectl_on();
    }
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, blectl_powermgm_event_cb, "blectl" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, blectl_powermgm_loop_cb, "blectl loop" );
    pmu_register_cb( PMUCTL_BATTERY_PERCENT | PMUCTL_CHARGING | PMUCTL_VBUS_PLUG, blectl_pmu_event_cb, "bluetooth battery");
}

bool blectl_powermgm_event_cb( EventBits_t event, void *arg ) {
    bool retval = true;

    switch( event ) {
        case POWERMGM_STANDBY:          
            if ( blectl_get_enable_on_standby() && blectl_get_event( BLECTL_ON ) ) {
                retval = false;
                log_w("standby blocked by \"enable_on_standby\" option");
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
    portENTER_CRITICAL(&blectlMux);
    xEventGroupSetBits( blectl_status, bits );
    portEXIT_CRITICAL(&blectlMux);
}

void blectl_clear_event( EventBits_t bits ) {
    portENTER_CRITICAL(&blectlMux);
    xEventGroupClearBits( blectl_status, bits );
    portEXIT_CRITICAL(&blectlMux);
}

bool blectl_get_event( EventBits_t bits ) {
    portENTER_CRITICAL(&blectlMux);
    EventBits_t temp = xEventGroupGetBits( blectl_status ) & bits;
    portEXIT_CRITICAL(&blectlMux);
    if ( temp )
        return( true );

    return( false );
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
    blectl_save_config();
}

void blectl_set_advertising( bool advertising ) {  
    blectl_config.advertising = advertising;
    blectl_save_config();
    if ( blectl_get_event( BLECTL_CONNECT ) )
        return;

    if ( advertising ) {
        pServer->getAdvertising()->start();
    }
    else {
        pServer->getAdvertising()->stop();
    }
}

void blectl_set_txpower( int32_t txpower ) {
    if ( txpower >= 0 && txpower <= 4 ) {
        blectl_config.txpower = txpower;
    }
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
    blectl_save_config();
}

void blectl_set_autoon( bool autoon ) {
    blectl_config.autoon = autoon;

    if( autoon ) {
        blectl_on();
    }
    else {
        blectl_off();
    }
    blectl_save_config();
}

int32_t blectl_get_txpower( void ) {
    return( blectl_config.txpower );
}

bool blectl_get_enable_on_standby( void ) {
    return( blectl_config.enable_on_standby );
}

bool blectl_get_autoon( void ) {
    return( blectl_config.autoon );
}

bool blectl_get_advertising( void ) {
    return( blectl_config.advertising );
}

void blectl_save_config( void ) {
    fs::File file = SPIFFS.open( BLECTL_JSON_COFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", BLECTL_JSON_COFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["autoon"] = blectl_config.autoon;
        doc["advertising"] = blectl_config.advertising;
        doc["enable_on_standby"] = blectl_config.enable_on_standby;
        doc["tx_power"] = blectl_config.txpower;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void blectl_read_config( void ) {
    fs::File file = SPIFFS.open( BLECTL_JSON_COFIG_FILE, FILE_READ );

    if (!file) {
        log_e("Can't open file: %s!", BLECTL_JSON_COFIG_FILE );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 2 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("blectl deserializeJson() failed: %s", error.c_str() );
        }
        else {                
            blectl_config.autoon = doc["autoon"] | true;
            blectl_config.advertising = doc["advertising"] | true;
            blectl_config.enable_on_standby = doc["enable_on_standby"] | false;
            blectl_config.txpower = doc["tx_power"] | 1;
        }        
        doc.clear();
    }
    file.close();
}

bool blectl_pmu_event_cb( EventBits_t event, void *arg ) {
    static int32_t percent = 0;
    static bool charging = false;
    static bool plug = false;

    switch( event ) {
        case PMUCTL_BATTERY_PERCENT:
            percent = *(int32_t*)arg;
			if ( blectl_get_event( BLECTL_CONNECT ) ) {
				blectl_update_battery( percent, charging, plug );
			}
            break;
        case PMUCTL_CHARGING:
            charging = *(bool*)arg;
            break;
        case PMUCTL_VBUS_PLUG:
            plug = *(bool*)arg;
            break;
    }
    return( true );
}

void blectl_update_battery( int32_t percent, bool charging, bool plug ) {
    uint8_t level = (uint8_t)percent;
    if (level > 100) level = 100;

    pBatteryLevelCharacteristic->setValue(&level, 1);
    pBatteryLevelCharacteristic->notify();

    uint8_t batteryPowerState = BATTERY_POWER_STATE_BATTERY_PRESENT | 
        (plug ? BATTERY_POWER_STATE_DISCHARGE_NOT_DISCHARING : BATTERY_POWER_STATE_DISCHARGE_DISCHARING) |
        (charging? BATTERY_POWER_STATE_CHARGE_CHARING : BATTERY_POWER_STATE_CHARGE_NOT_CHARING) | 
        (percent > 10 ? BATTERY_POWER_STATE_LEVEL_GOOD : BATTERY_POWER_STATE_LEVEL_CRITICALLY_LOW );
    pBatteryPowerStateCharacteristic->setValue(&batteryPowerState, 1);
    pBatteryPowerStateCharacteristic->notify();
}

void blectl_send_msg( char *msg ) {
    blectl_msg_chain = msg_chain_add_msg( blectl_msg_chain, msg );
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
    if ( blectl_config.advertising ) {
        pServer->getAdvertising()->start();
    }
    else {
        pServer->getAdvertising()->stop();
    }
    blectl_set_event( BLECTL_ON );
    blectl_clear_event( BLECTL_OFF );
    blectl_send_event_cb( BLECTL_ON, (void *)NULL );
}

void blectl_off( void ) {
    blectl_config.autoon = false;
    pServer->getAdvertising()->stop();
    blectl_set_event( BLECTL_OFF );
    blectl_clear_event( BLECTL_ON );
    blectl_send_event_cb( BLECTL_OFF, (void *)NULL );
}

void blectl_loop ( void ) {
    static uint64_t nextmillis = 0;
    char chunk_msg[ 64 ] = "";

    if ( !blectl_get_event( BLECTL_CONNECT ) ) {
        return;
    }

    if ( !blectl_msg.active && msg_chain_get_entrys( blectl_msg_chain ) > 0 ) {
        blectl_send_next_msg( (char *)msg_chain_get_msg_entry( blectl_msg_chain, 0 ) );
        msg_chain_delete_msg_entry( blectl_msg_chain, 0 );
    }

    if ( nextmillis < millis() ) {
        nextmillis = millis() + BLECTL_CHUNKDELAY;
        if ( blectl_msg.active ) {
            if ( blectl_msg.msgpos < blectl_msg.msglen ) {
                if ( ( blectl_msg.msglen - blectl_msg.msgpos ) > BLECTL_CHUNKSIZE ) {
                    chunk_msg[ 0 ] ='\0';
                    for( int i = 0 ; i < BLECTL_CHUNKSIZE ; i++ ) {
                        char tmp_str[16]="";
                        if ( blectl_msg.msg[ blectl_msg.msgpos + i ] > 0x1F ) {
                            snprintf( tmp_str, sizeof( tmp_str ),"%c", blectl_msg.msg[ blectl_msg.msgpos + i ] );
                        }
                        else {
                            snprintf( tmp_str, sizeof( tmp_str ),"?" );
                        }
                        strcat( chunk_msg, tmp_str );
                    }
                    pTxCharacteristic->setValue( (unsigned char*)&blectl_msg.msg[ blectl_msg.msgpos ], BLECTL_CHUNKSIZE );
                    pTxCharacteristic->notify();
                    log_i("send %2dbyte [ \"%s\" ] chunk", BLECTL_CHUNKSIZE, chunk_msg );
                    blectl_msg.msgpos += BLECTL_CHUNKSIZE;
                }
                else if ( ( blectl_msg.msglen - blectl_msg.msgpos ) > 0 ) {
                    chunk_msg[ 0 ] ='\0';
                    for( int i = 0 ; i < ( blectl_msg.msglen - blectl_msg.msgpos ) ; i++ ) {
                        char tmp_str[16]="";
                        if ( blectl_msg.msg[ blectl_msg.msgpos + i ] > 0x1F ) {
                            snprintf( tmp_str, sizeof( tmp_str ),"%c", blectl_msg.msg[ blectl_msg.msgpos + i ] );
                        }
                        else {
                            snprintf( tmp_str, sizeof( tmp_str ),"?" );
                        }
                        strcat( chunk_msg, tmp_str );
                    }
                    
                    pTxCharacteristic->setValue( (unsigned char*)&blectl_msg.msg[ blectl_msg.msgpos ], blectl_msg.msglen - blectl_msg.msgpos );
                    pTxCharacteristic->notify();
                    log_i("send %2dbyte [ \"%s\" ] chunk", blectl_msg.msglen - blectl_msg.msgpos, chunk_msg );
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
}