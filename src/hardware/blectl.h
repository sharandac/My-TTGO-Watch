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
#ifndef _BLECTL_H
    #define _BLECTL_H

    #ifdef NATIVE_64BIT
        #include "utils/io.h"
    #else
        #include <BLEServer.h>
        #include <BLEAdvertising.h>
    #endif

    #include "callback.h"
    #include "hardware/config/blectlconfig.h"

    /**
     * connection state
     */
    #define BLECTL_CONNECT               _BV(0)         /** @brief event mask for blectl connect to an client */
    #define BLECTL_DISCONNECT            _BV(1)         /** @brief event mask for blectl disconnect */
    #define BLECTL_AUTHWAIT              _BV(2)         /** @brief event mask for blectl wait for auth to get connect */
    /**
     * power state
     */
    #define BLECTL_STANDBY               _BV(3)         /** @brief event mask for blectl standby */
    #define BLECTL_ON                    _BV(4)         /** @brief event mask for blectl on */
    #define BLECTL_OFF                   _BV(5)         /** @brief event mask for blectl off */
    /**
     * pairing state
     */
    #define BLECTL_PIN_AUTH              _BV(6)         /** @brief event mask for blectl for pin auth, callback arg is (uint32*) */
    #define BLECTL_PAIRING               _BV(7)         /** @brief event mask for blectl pairing requested */
    #define BLECTL_PAIRING_SUCCESS       _BV(8)         /** @brief event mask for blectl pairing success */
    #define BLECTL_PAIRING_ABORT         _BV(9)         /** @brief event mask for blectl pairing abort */
    /**
     * message state
     */
    #define BLECTL_MSG                   _BV(10)        /** @brief event mask for blectl msg */
    #define BLECTL_MSG_SEND_SUCCESS      _BV(11)        /** @brief event mask msg send success */
    #define BLECTL_MSG_SEND_ABORT        _BV(12)        /** @brief event mask msg send abort */
    #define BLECTL_MSG_JSON              _BV(13)        /** @brief event mask for blectl JSON msg */
    /**
     *  See the following for generating UUIDs:
     * https://www.uuidgenerator.net/
     */
    #define SERVICE_UUID                                    BLEUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E")     /** @brief UART service UUID */
    #define CHARACTERISTIC_UUID_RX                          BLEUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E")
    #define CHARACTERISTIC_UUID_TX                          BLEUUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E")

    #define DEVICE_INFORMATION_SERVICE_UUID                 BLEUUID((uint16_t)0x180A)                           /** @brief Device Information server UUID */
    #define MANUFACTURER_NAME_STRING_CHARACTERISTIC_UUID    BLEUUID((uint16_t)0x2A29)                           /** @brief Device Information - manufacturer name string UUID */
    #define FIRMWARE_REVISION_STRING_CHARACTERISTIC_UUID    BLEUUID((uint16_t)0x2A26)                           /** @brief Device Information - firmware revision UUID */

    #define BATTERY_SERVICE_UUID                            BLEUUID((uint16_t)0x180F)                           /** @brief Battery service UUID */
    #define BATTERY_LEVEL_CHARACTERISTIC_UUID               BLEUUID((uint16_t)0x2A19)                           /** @brief battery level characteristic UUID */
    #define BATTERY_LEVEL_DESCRIPTOR_UUID                   BLEUUID((uint16_t)0x2901)                           /** @brief battery level descriptor UUID */
    #define BATTERY_POWER_STATE_CHARACTERISTIC_UUID         BLEUUID((uint16_t)0x2A1A)                           /** @brief battery power state characteristic UUID */

    #define BATTERY_POWER_STATE_BATTERY_UNKNOWN             0x0
    #define BATTERY_POWER_STATE_BATTERY_NOT_SUPPORTED       0x1
    #define BATTERY_POWER_STATE_BATTERY_NOT_PRESENT         0x2
    #define BATTERY_POWER_STATE_BATTERY_PRESENT             0x3

    #define BATTERY_POWER_STATE_DISCHARGE_UNKNOWN           0x0
    #define BATTERY_POWER_STATE_DISCHARGE_NOT_SUPPORTED     0x4
    #define BATTERY_POWER_STATE_DISCHARGE_NOT_DISCHARING    0x8
    #define BATTERY_POWER_STATE_DISCHARGE_DISCHARING        0xc

    #define BATTERY_POWER_STATE_CHARGE_UNKNOWN              0x0
    #define BATTERY_POWER_STATE_CHARGE_NOT_CHARGEABLE       0x10
    #define BATTERY_POWER_STATE_CHARGE_NOT_CHARING          0x20
    #define BATTERY_POWER_STATE_CHARGE_CHARING              0x30

    #define BATTERY_POWER_STATE_LEVEL_UNKNOWN               0x0
    #define BATTERY_POWER_STATE_LEVEL_NOT_SUPPORTED         0x40
    #define BATTERY_POWER_STATE_LEVEL_GOOD                  0x80
    #define BATTERY_POWER_STATE_LEVEL_CRITICALLY_LOW        0xC0

    #define EndofText               0x03
    #define LineFeed                0x0a
    #define DataLinkEscape          0x10

    #define BLECTL_CHUNKSIZE        20      /** @brief chunksize for send msg */
    #define BLECTL_CHUNKDELAY       50      /** @brief chunk delay in ms for each msg chunk */
    #define BLECTL_MSG_MTU          512     /** @brief max msg size */

    /**
     * @brief blectl send msg structure
     */
    typedef struct {
        char *msg;                      /** @brief pointer to an sending msg */
        bool active;                    /** @brief send msg structure active */
        int32_t msglen;                 /** @brief msg lenght */
        int32_t msgpos;                 /** @brief msg postition for next send */
    } blectl_msg_t;
    /**
     * @brief ble setup function
     */
    void blectl_setup( void );
    /**
     * @brief trigger a blectl managemt event
     * 
     * @param   bits    event to trigger
     */
    void blectl_set_event( EventBits_t bits );
    /**
     * @brief clear a blectl managemt event
     * 
     * @param   bits    event to clear
     */
    void blectl_clear_event( EventBits_t bits );
    /**
     * @brief get a blectl managemt event state
     * 
     * @param   bits    event state, example: POWERMGM_STANDBY to evaluate if the system in standby
     */
    bool blectl_get_event( EventBits_t bits );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event  possible values:     BLECTL_CONNECT,
     *                                      BLECTL_DISCONNECT,
     *                                      BLECTL_STANDBY,
     *                                      BLECTL_ON,
     *                                      BLECTL_OFF,       
     *                                      BLECTL_ACTIVE,    
     *                                      BLECTL_MSG,
     *                                      BLECTL_PIN_AUTH,
     *                                      BLECTL_PAIRING,
     *                                      BLECTL_PAIRING_SUCCESS,
     *                                      BLECTL_PAIRING_ABORT
     * @param   blectl_event_cb     pointer to the callback function
     * @param   id                  pointer to an string
     */
    bool blectl_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief enable blueetooth on standby
     * 
     * @param   enable_on_standby   true means enabled, false means disabled 
     */
    void blectl_set_enable_on_standby( bool enable_on_standby );
    /**
     * @brief disable blueetooth only when disconnected
     * 
     * @param   disable_only_disconnected   true means enabled, false means disabled 
     */
    void blectl_set_disable_only_disconnected( bool disable_only_disconnected );
    /**
     * @brief enable show notification
     * 
     * @param   show_notification   true means enabled, false means disabled 
     */
    void blectl_set_show_notification( bool show_notification );
    /**
     * @brief enable advertising
     * 
     * @param   advertising true means enabled, false means disabled
     */
    void blectl_set_advertising( bool advertising );
    /**
     * @brief get the current enable_on_standby config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_enable_on_standby( void );
    /**
     * @brief get the current disable_only_disconnected config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_disable_only_disconnected( void );
    /**
     * @brief get the current show notification config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_show_notification( void );
    /**
     * @brief get the current advertising config
     * 
     * @return  true means enabled, false means disabled
     */
    bool blectl_get_advertising( void );
    /**
     * @brief get the current custom audio notifications config
     * 
     * @return  the array of custom audio notifications
     */
    blectl_custom_audio* blectl_get_custom_audio_notifications( void );
    /**
     * @brief store the current configuration to SPIFFS
     */
    void blectl_save_config( void );
    /**
     * @brief read the configuration from SPIFFS
     */
    void blectl_read_config( void );
    /**
     * @brief send an battery update over bluetooth to gadgetbridge
     * 
     * @param   percent     battery percent
     * @param   charging    charging state
     * @param   plug        powerplug state
     */
    void blectl_update_battery( int32_t percent, bool charging, bool plug );
    /**
     * @brief send an message over bluettoth to gadgetbridge
     * 
     * @param   msg     pointer to a string
     */
    bool blectl_send_msg( const char *msg );
    /**
     * @brief set the transmission power
     * 
     * @param   txpower power from 0..4, from -12db to 0db in 3db steps
     */
    void blectl_set_txpower( int32_t txpower );
    /**
     * @brief get the current transmission power
     * 
     * @return  power from 0..4, from -12db to 0db in 3db steps
     */
    int32_t blectl_get_txpower( void );
    /**
     * @brief enable the bluettoth stack
     */
    void blectl_on( void );
    /**
     * @brief disable the bluetooth stack
     */
    void blectl_off( void );
    /**
     * @brief get the current enable config
     * 
     * @return true if bl enabled, false if bl disabled
     */
    bool blectl_get_autoon( void );
    /**
     * @brief set the current bl enable config
     * 
     * @param enable    true if enabled, false if disable
     */
    void blectl_set_autoon( bool autoon );
#ifndef NATIVE_64BIT
    /**
     * @brief get the raw BLE Server
     */
    BLEServer *blectl_get_ble_server( void );
    /**
     * @brief get the raw BLE Advertising
     */
    BLEAdvertising *blectl_get_ble_advertising( void );
    /**
     * @brief get the raw BLE Server
     */
    BLEServer *blectl_get_ble_server();
    /**
     * @brief get the raw BLE Advertising
     */
    BLEAdvertising *blectl_get_ble_advertising();
#endif

#endif // _BLECTL_H