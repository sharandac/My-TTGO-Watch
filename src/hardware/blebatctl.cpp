/****************************************************************************
 *   Copyright  2021  Guilhem Bonnefille <guilhem.bonnefille@gmail.com>
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
#include "blebatctl.h"
#include "blectl.h"
#include "pmu.h"
#include "bleupdater.h"

#ifdef NATIVE_64BIT

#else
    #include <Arduino.h>
    #include <BLEServer.h>
    #include <BLE2902.h>

    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #else
        #warning "no hardware driver for blebatctl"
    #endif

    class BleBattLevelUpdater : public BleUpdater<uint8_t> {
        public:
        BleBattLevelUpdater(BLECharacteristic *charac, time_t timeout) : BleUpdater(timeout), characteristic(charac) {}
        void set(uint8_t power) {
            characteristic->setValue(&power, 1);
        }
        
        bool notify( uint8_t level ) {
            bool retval = false;
            /*
            * TODO and fix me: add a threshold to notify only significant changes
            *
            * Do not notify on same value
            */
            if ( last_value == level ) {
                return( retval );
            }
            /*
            * Send battery level via standard characteristic
            */
            characteristic->notify();
            /*
            * Send battery percent via BangleJS protocol
            */
            char msg[64]="";
            snprintf( msg, sizeof(msg), "\r\n{t:\"status\", bat:%d}\r\n", level );
            bool ret = blectl_send_msg( msg );
            log_i("Notified batt level: new=%d last=%d => %d", level, last_value, ret);
            return ret;
        }
        BLECharacteristic *characteristic;
    };

    class BleBattPowerUpdater : public BleUpdater<uint8_t> {
        public:
        BleBattPowerUpdater(BLECharacteristic *charac, time_t timeout) : BleUpdater(timeout), characteristic(charac) {}
        void set(uint8_t power) {
            characteristic->setValue(&power, 1);
        }

        bool notify(uint8_t power) {
            /*
            * Do not notify on same value
            */
            if ( last_value == power ) {
                return( false );
            }
            /*
            * update power
            */
            characteristic->notify();
            return( true );
        }
        BLECharacteristic *characteristic;
    };

    static bool blebatctl_pmu_event_cb( EventBits_t event, void *arg );
    static void blebatctl_update_battery( int32_t percent, bool charging, bool plug );
    static bool blebatctl_bluetooth_event_cb( EventBits_t event, void *arg );

    static BLECharacteristic *pBatteryLevelCharacteristic;
    static BLECharacteristic *pBatteryPowerStateCharacteristic;

    static BleBattLevelUpdater *blebatctl_level_updater;
    static BleBattPowerUpdater *blebatctl_power_updater;

    void blebatctl_setup(BLEServer *pServer) {
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

        blebatctl_level_updater = new BleBattLevelUpdater( pBatteryLevelCharacteristic, 60 * 5 );
        blebatctl_power_updater = new BleBattPowerUpdater( pBatteryPowerStateCharacteristic, 60 * 5 );

        pmu_register_cb( PMUCTL_STATUS, blebatctl_pmu_event_cb, "ble battery" );
        blectl_register_cb( BLECTL_CONNECT, blebatctl_bluetooth_event_cb, "ble battery" );
    }

    static bool blebatctl_pmu_event_cb( EventBits_t event, void *arg ) {
        bool retval = false;

        switch( event ) {
            case PMUCTL_STATUS:
                bool charging = *(bool*)arg & PMUCTL_STATUS_CHARGING;
                bool plug = *(bool*)arg & PMUCTL_STATUS_PLUG;
                int32_t percent = *(int32_t*)arg & PMUCTL_STATUS_PERCENT;
                blebatctl_update_battery( percent, charging, plug );
                retval = true;
                break;
        }
        return( retval );
    }

    static void blebatctl_update_battery( int32_t percent, bool charging, bool plug ) {
        /*
        * update battery level
        */
        uint8_t level = (uint8_t)percent;
        if ( level > 100 ) {
            level = 100;
        }
        blebatctl_level_updater->update( level );
        /*
        * Send powerstate via standard caracteristic
        */
        uint8_t batteryPowerState = BATTERY_POWER_STATE_BATTERY_PRESENT
                                    | ( plug ? BATTERY_POWER_STATE_DISCHARGE_NOT_DISCHARING : BATTERY_POWER_STATE_DISCHARGE_DISCHARING )
                                    | ( charging ? BATTERY_POWER_STATE_CHARGE_CHARING : BATTERY_POWER_STATE_CHARGE_NOT_CHARING )
                                    | ( percent > 10 ? BATTERY_POWER_STATE_LEVEL_GOOD : BATTERY_POWER_STATE_LEVEL_CRITICALLY_LOW );
        blebatctl_power_updater->update( batteryPowerState );
    }

    static bool blebatctl_bluetooth_event_cb(EventBits_t event, void *arg) {
        bool retval = false;

        switch( event ) {
            case BLECTL_CONNECT:        
                    /*
                    * Try to refresh values on (re)connect
                    */
                    bool charging = pmu_is_charging();
                    bool plug = pmu_is_vbus_plug();
                    int32_t percent = pmu_get_battery_percent();
                    blebatctl_update_battery( percent, charging, plug );
                    retval = true;
                    break;
        }

        return( retval );
    }
#endif

