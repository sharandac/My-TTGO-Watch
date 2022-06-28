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
#ifndef _BLEBATCTL_H
    #define _BLEBATCTL_H

    #define BATTERY_SERVICE_UUID                            (uint16_t)0x180F                           /** @brief Battery service UUID */
    #define BATTERY_LEVEL_CHARACTERISTIC_UUID               (uint16_t)0x2A19                           /** @brief battery level characteristic UUID */
    #define BATTERY_LEVEL_DESCRIPTOR_UUID                   (uint16_t)0x2901                           /** @brief battery level descriptor UUID */
    #define BATTERY_POWER_STATE_CHARACTERISTIC_UUID         (uint16_t)0x2A1A                           /** @brief battery power state characteristic UUID */

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

    #ifdef NATIVE_64BIT
    #else
        #include "hardware/blectl.h"

        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #else
        #endif
            /**
             * @brief ble bat setup function
             * 
             * @param pServer   pointer to an BLEServer
             */
            void blebatctl_setup( void );
    #endif
#endif // _BLEBATCTL_H