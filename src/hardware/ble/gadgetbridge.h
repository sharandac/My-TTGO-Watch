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
#ifndef _GADEGTBRIDGE_H
    #define _GADEGTBRIDGE_H

    #include "hardware/callback.h"
    #include "hardware/blectl.h"

    /**
     *  See the following for generating UUIDs:
     * https://www.uuidgenerator.net/
     */
    #define GADGETBRIDGE_SERVICE_UUID                           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"     /** @brief UART service UUID */
    #define GADGETBRIDGE_CHARACTERISTIC_UUID_RX                 "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
    #define GADGETBRIDGE_CHARACTERISTIC_UUID_TX                 "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
    /**
     * connection state
     */
    #define GADGETBRIDGE_CONNECT                                _BV(0)          /** @brief event mask for blectl connect to an client */
    #define GADGETBRIDGE_DISCONNECT                             _BV(1)          /** @brief event mask for blectl disconnect */
    #define GADGETBRIDGE_MSG                                    _BV(2)          /** @brief event mask for blectl wait for auth to get connect */
    #define GADGETBRIDGE_JSON_MSG                               _BV(3)          /** @brief event mask for blectl wait for auth to get connect */
    #define GADGETBRIDGE_MSG_SEND_ABORT                         _BV(4)          /** @brief event mask for blectl wait for auth to get connect */
    #define GADGETBRIDGE_MSG_SEND_SUCCESS                       _BV(5)  

    #define EndofText                                           0x03            /** @brief gadgetbridge EndOfText marker */
    #define LineFeed                                            0x0a            /** @brief gadgetbridge LineFeed marker */
    #define DataLinkEscape                                      0x10            /** @brief gadgetbridge DataLinkEscape marker */

    #define BLECTL_CHUNKSIZE                                    20              /** @brief chunksize for send msg */
    #define BLECTL_CHUNKDELAY                                   50              /** @brief chunk delay in ms for each msg chunk */
    /**
     * @brief blectl send msg structure
     */
    typedef struct {
        char *msg;                                                              /** @brief pointer to an sending msg */
        bool active;                                                            /** @brief send msg structure active */
        int32_t msglen;                                                         /** @brief msg lenght */
        int32_t msgpos;                                                         /** @brief msg postition for next send */
    } blectl_msg_t;
    /**
     * @brief setup gadgetbridge transmit/recieve over ble
     */
    void gadgetbridge_setup( void );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event  possible values:     GADGETBRIDGE_CONNECT,
     *                                      GADGETBRIDGE_DISCONNECT,
     *                                      GADGETBRIDGE_MSG,
     *                                      GADGETBRIDGE_JSON_MSG,
     *                                      GADGETBRIDGE_MSG_SEND_ABORT       
     * @param   blectl_event_cb     pointer to the callback function
     * @param   id                  pointer to an string
     */
    bool gadgetbridge_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief send an message and loop back as printf formatted 
     * 
     * @param   format  pointer to a string
     * @param   ...     printf options
     */
    bool gadgetbridge_send_loop_msg( const char *format, ... );
    /**
     * @brief send an message over bluettoth to gadgetbridge
     * 
     * @param   msg     pointer to a string
     */
    bool gadgetbridge_send_msg( const char *format, ... );


#endif // _GADEGTBRIDGE_H