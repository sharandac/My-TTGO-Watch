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

#include "gui/statusbar.h"

EventGroupHandle_t blectl_status = NULL;
portMUX_TYPE blectlMux = portMUX_INITIALIZER_UNLOCKED;

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
uint8_t txValue = 0;

#define MAX_MESSAGE_SIZE 512
String message;

/*
 *
 */
class BleCtlServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        blectl_set_event( BLECTL_CONNECT );
        statusbar_style_icon( STATUSBAR_BLUETOOTH, STATUSBAR_STYLE_WHITE );
        log_i("BLE connected");
    };

    void onDisconnect(BLEServer* pServer) {
        blectl_clear_event( BLECTL_CONNECT );
        statusbar_style_icon( STATUSBAR_BLUETOOTH, STATUSBAR_STYLE_GRAY );
        log_i("BLE disconnected");
        delay(500);
        pServer->getAdvertising()->start();
        log_i("BLE advertising...");
    }
};

/*
 *
 */
class BtlCtlSecurity : public BLESecurityCallbacks {

    uint32_t onPassKeyRequest(){
        log_i("BLE: PassKeyRequest");
        // TODO: when is this used?
        return 123456;
    }
    void onPassKeyNotify(uint32_t pass_key){
        blectl_set_event( BLECTL_PAIRING );
        log_i("Bluetooth Pairing Request\r\nPIN: %06d", pass_key);
    }
    bool onConfirmPIN(uint32_t pass_key){
        log_i("BLE: The passkey YES/NO number :%06d", pass_key);
        // vTaskDelay(5000);
        // return true;
        // TODO: when is this used?
        return false;
    }
    bool onSecurityRequest(){
        log_i("BLE: SecurityRequest");
        // TODO: when is this used?
        return true;
    }

    void onAuthenticationComplete( esp_ble_auth_cmpl_t cmpl ){
        log_i("Bluetooth pairing %s", cmpl.success ? "successful" : "unsuccessful");

        if( cmpl.success ){
            uint16_t length;
            esp_ble_gap_get_whitelist_size( &length );
            log_i("size: %d", length );
        } else {
            // Restart advertising
            pServer->startAdvertising();
        }

        if ( blectl_get_event( BLECTL_PAIRING )) {
            blectl_clear_event( BLECTL_PAIRING );
        }
    }
};

class BleCtlCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();

        if (rxValue.length() > 0)
        {
            for (int i = 0; i < rxValue.length(); i++) {
                if (rxValue[i] == 0x10) {
                    if ( message.length() ) {
                        log_i("BLE: Discarding %d bytes", message.length());
                    }
                    message.clear();
                } else if (rxValue[i] == '\n') {
                   if (message.length()+1 > MAX_MESSAGE_SIZE) {
                        message.clear();
                        log_e("BLE Error: Message too long");
                        return;
                    }
                    message[message.length()] = 0;
                    Serial.println("BLE message: " + message );
                    message.clear();
                } else {
                    message += rxValue[i];
                    if (message.length() > MAX_MESSAGE_SIZE) {
                        message.clear();
                        log_e("BLE Error: Message too long");
                        return;
                    }
                }
            }
        }
    }
};

/*
 *
 */
void blectl_setup( void ) {
    blectl_status = xEventGroupCreate();
    blectl_set_event( BLECTL_CONNECT | BLECTL_OFF_REQUEST | BLECTL_ON_REQUEST | BLECTL_PAIRING | BLECTL_STANDBY_REQUEST | BLECTL_ACTIVE | BLECTL_SCAN );

    esp_bt_controller_enable( ESP_BT_MODE_BLE );
    esp_bt_controller_mem_release( ESP_BT_MODE_CLASSIC_BT );
    esp_bt_mem_release( ESP_BT_MODE_CLASSIC_BT );
    esp_bt_controller_mem_release( ESP_BT_MODE_IDLE );
    esp_bt_mem_release( ESP_BT_MODE_IDLE );

    // Create the BLE Device
    // Name needs to match filter in Gadgetbridge's banglejs getSupportedType() function.
    // This is too long I think:
    // BLEDevice::init("Espruino Gadgetbridge Compatible Device");
    BLEDevice::init("Espruino (T-Watch2020)");
    // The minimum power level (-12dbm) ESP_PWR_LVL_N12 was too low
    BLEDevice::setPower( ESP_PWR_LVL_N9 );

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

    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic( CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE );
    pRxCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
    pRxCharacteristic->setCallbacks( new BleCtlCallbacks() );

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->addServiceUUID( pService->getUUID() );
    // Slow advertising interval for battery life
    // The maximum 0x4000 interval of ~16 sec was too slow, I could not reliably connect
    pServer->getAdvertising()->setMinInterval( 100 );
    pServer->getAdvertising()->setMaxInterval( 200 );
    pServer->getAdvertising()->start();
    log_i("BLE advertising...");
}

/*
 *
 */
void blectl_set_event( EventBits_t bits ) {
    portENTER_CRITICAL(&blectlMux);
    xEventGroupSetBits( blectl_status, bits );
    portEXIT_CRITICAL(&blectlMux);
}

/*
 *
 */
void blectl_clear_event( EventBits_t bits ) {
    portENTER_CRITICAL(&blectlMux);
    xEventGroupClearBits( blectl_status, bits );
    portEXIT_CRITICAL(&blectlMux);
}

/*
 *
 */
EventBits_t blectl_get_event( EventBits_t bits ) {
    portENTER_CRITICAL(&blectlMux);
    EventBits_t temp = xEventGroupGetBits( blectl_status ) & bits;
    portEXIT_CRITICAL(&blectlMux);
    return( temp );
}

void blectl_standby( void ) {
    statusbar_style_icon( STATUSBAR_BLUETOOTH, STATUSBAR_STYLE_GRAY );
}

void blectl_wakeup( void ) {
    statusbar_style_icon( STATUSBAR_BLUETOOTH, STATUSBAR_STYLE_GRAY );
}