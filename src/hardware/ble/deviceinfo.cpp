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
#include "config.h"
#include "deviceinfo.h"
#include "hardware/blectl.h"

void deviceinfo_setup( void ) {
    #ifdef NATIVE_64BIT

    #else
        NimBLEServer *pServer = blectl_get_ble_server();
        NimBLEAdvertising *pAdvertising = blectl_get_ble_advertising();
        /*
         * Create device information service
         */
        NimBLEService *pDeviceInformationService = pServer->createService( NimBLEUUID( DEVICE_INFORMATION_SERVICE_UUID ) );
        NimBLECharacteristic* pManufacturerNameStringCharacteristic = pDeviceInformationService->createCharacteristic( MANUFACTURER_NAME_STRING_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::READ );
        pManufacturerNameStringCharacteristic->setValue("Lily Go");
        NimBLECharacteristic* pFirmwareRevisionStringCharacteristic = pDeviceInformationService->createCharacteristic( FIRMWARE_REVISION_STRING_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::READ );
        pFirmwareRevisionStringCharacteristic->setValue(__FIRMWARE__);
        pDeviceInformationService->start();
        pAdvertising->addServiceUUID( pDeviceInformationService->getUUID() );
    #endif
}