/****************************************************************************
 *   Jun 27 21:38:51 2020
 *   Copyright  2022  Dirk Brosswick
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
#ifndef _DEVICEINFO_H
    #define _DEVICEINFO_H

    #include "hardware/callback.h"
    #include "hardware/blectl.h"

    #define DEVICE_INFORMATION_SERVICE_UUID                 (uint16_t)0x180A                           /** @brief Device Information server UUID */
    #define MANUFACTURER_NAME_STRING_CHARACTERISTIC_UUID    (uint16_t)0x2A29                           /** @brief Device Information - manufacturer name string UUID */
    #define FIRMWARE_REVISION_STRING_CHARACTERISTIC_UUID    (uint16_t)0x2A26                           /** @brief Device Information - firmware revision UUID */
    /**
     * @brief setup gadgetbridge transmit/recieve over ble
     */
    void deviceinfo_setup( void );

#endif // _DEVICEINFO_H