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

    #define BLECTL_CONNECT               _BV(0)
    #define BLECTL_STANDBY_REQUEST       _BV(1)
    #define BLECTL_ON_REQUEST            _BV(2)
    #define BLECTL_OFF_REQUEST           _BV(3)
    #define BLECTL_ACTIVE                _BV(4)
    #define BLECTL_SCAN                  _BV(5)
    #define BLECTL_PAIRING               _BV(6)

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
    EventBits_t blectl_get_event( EventBits_t bits );
    void blectl_standby( void );
    void blectl_wakeup( void );

#endif // _BLECTL_H