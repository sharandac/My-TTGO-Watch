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

    #include "TTGO.h"

    // See the following for generating UUIDs:
    // https://www.uuidgenerator.net/
    #define SERVICE_UUID BLEUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E") // UART service UUID
    #define CHARACTERISTIC_UUID_RX BLEUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E")
    #define CHARACTERISTIC_UUID_TX BLEUUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E")

    #define BATTERY_SERVICE_UUID  BLEUUID((uint16_t)0x180F) // Battery service UUID
    #define BATTERY_SERVICE_BATTERY_LEVEL_CHARACTERISTIC_UUID  BLEUUID((uint16_t)0x2A19) // Battery service - battery level characteristic  UUID
    #define BATTERY_SERVICE_BATTERY_LEVEL_DESCRIPTOR_UUID  BLEUUID((uint16_t)0x2901) // Battery service - battery level descriptor UUID

    #define BLECTL_JSON_COFIG_FILE         "/blectl.json"

    #define EndofText               0x03
    #define LineFeed                0x0a
    #define DataLinkEscape          0x10

    typedef struct {
        bool advertising = true;
        bool enable_on_standby = false;
    } blectl_config_t;

    typedef void ( * BLECTL_CALLBACK_FUNC ) ( EventBits_t event, char *msg );

    typedef struct {
        EventBits_t event;
        BLECTL_CALLBACK_FUNC event_cb;
    } blectl_event_t;

    #define BLECTL_CONNECT               _BV(0)
    #define BLECTL_DISCONNECT            _BV(1)
    #define BLECTL_STANDBY               _BV(2)
    #define BLECTL_ON                    _BV(3)
    #define BLECTL_OFF                   _BV(4)
    #define BLECTL_ACTIVE                _BV(5)
    #define BLECTL_MSG                   _BV(6)
    #define BLECTL_PIN_AUTH              _BV(7)
    #define BLECTL_PAIRING               _BV(8)
    #define BLECTL_PAIRING_SUCCESS       _BV(9)
    #define BLECTL_PAIRING_ABORT         _BV(10)

    void blectl_setup( void );

    /*
     * @brief trigger a blectl managemt event
     * 
     * @param   bits    event to trigger
     */
    void blectl_set_event( EventBits_t bits );
    /*
     * @brief clear a blectl managemt event
     * 
     * @param   bits    event to clear
     */
    void blectl_clear_event( EventBits_t bits );
    /*
     * @brief get a blectl managemt event state
     * 
     * @param   bits    event state, example: POWERMGM_STANDBY to evaluate if the system in standby
     */
    bool blectl_get_event( EventBits_t bits );

    void blectl_register_cb( EventBits_t event, BLECTL_CALLBACK_FUNC blectl_event_cb );
    void blectl_standby( void );
    void blectl_wakeup( void );
    void blectl_set_enable_on_standby( bool enable_on_standby );
    void blectl_set_advertising( bool advertising );
    bool blectl_get_enable_on_standby( void );
    bool blectl_get_advertising( void );
    void blectl_save_config( void );
    void blectl_read_config( void );

    void blectl_update_battery( int32_t percent, bool charging, bool plug );

#endif // _BLECTL_H