/****************************************************************************
 *   Sep 11 10:11:10 2021
 *   Copyright  2021  Dirk Brosswick
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
#ifndef _BUTTON_H
    #define _BUTTON_H

    #include "callback.h"

    #ifdef NATIVE_64BIT
        #include "utils/io.h"
    #else
        #include <Arduino.h>
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #endif
    #endif
	/**
     * @brief buttons events mask
     */
    #define     BUTTON_PWR              _BV(0)          /** @brief event mask for power button */
    #define     BUTTON_QUICKBAR         _BV(1)          /** @brief event mask for quickbar button */
    #define     BUTTON_EXIT             _BV(2)          /** @brief event mask for exit button */
    #define     BUTTON_MENU             _BV(3)          /** @brief event mask for menu button */
    #define     BUTTON_ENTER            _BV(4)          /** @brief event mask for enter button */
    #define     BUTTON_REFRESH          _BV(5)          /** @brief event mask for refresh button */
    #define     BUTTON_SETUP            _BV(6)          /** @brief event mask for setup button */
    #define     BUTTON_UP               _BV(7)          /** @brief event mask for up button */
    #define     BUTTON_DOWN             _BV(8)          /** @brief event mask for down button */
    #define     BUTTON_LEFT             _BV(9)          /** @brief event mask for left button */
    #define     BUTTON_RIGHT            _BV(10)         /** @brief event mask for right button */
    #define     BUTTON_KEYBOARD         _BV(11)         /** @brief event mask for keyboard button, data as uint32_t */
    /**
     * @brief button setup function
     */
    void button_setup( void );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event           possible values: PMUCTL_STATUS
     * @param   callback_func   pointer to the callback function
     * @param   id              program id
     * 
     * @return  true if success, false if failed
     */
    bool button_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );

#endif // _BUTTON_H