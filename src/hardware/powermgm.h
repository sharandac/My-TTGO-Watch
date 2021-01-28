/****************************************************************************
 *   Tu May 22 21:23:51 2020
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
#ifndef _POWERMGM_H
    #define _POWERMGM_H

    #include "TTGO.h"
    #include "callback.h"

    #define POWERMGM_STANDBY                    _BV(0)         /** @brief event mask for powermgm standby */
    #define POWERMGM_STANDBY_REQUEST            _BV(1)         /** @brief event mask for powermgm standby request */
    #define POWERMGM_SILENCE_WAKEUP             _BV(2)         /** @brief event mask for powermgm silence wakeup */
    #define POWERMGM_SILENCE_WAKEUP_REQUEST     _BV(3)         /** @brief event mask for powermgm wakeup silence request */
    #define POWERMGM_WAKEUP                     _BV(4)         /** @brief event mask for powermgm wakeup */
    #define POWERMGM_WAKEUP_REQUEST             _BV(5)         /** @brief event mask for powermgm wakeup request */
    #define POWERMGM_POWER_BUTTON               _BV(6)         /** @brief event mask for powermgm pmu button is pressed */
    #define POWERMGM_SHUTDOWN                   _BV(12)        /** @brief event mask for powermgm shutdown */
    #define POWERMGM_RESET                      _BV(13)        /** @brief event mask for powermgm reset */
    #define POWERMGM_DISABLE_INTERRUPTS         _BV(15)        
    #define POWERMGM_ENABLE_INTERRUPTS          _BV(16)        
    
    /**
     * @brief setp power managment, coordinate managment beween CPU, wifictl, pmu, bma, display, backlight and lvgl
     */
    void powermgm_setup( void );
    /**
     * @brief power managment loop routine, call from loop. not for user use
     */
    void powermgm_loop( void );
    /**
     * @brief trigger a power managemt event
     * 
     * @param   bits    event to trigger, example: POWERMGM_WIFI_ON_REQUEST for switch an WiFi
     */
    void powermgm_set_event( EventBits_t bits );
    /**
     * @brief clear a power managemt event
     * 
     * @param   bits    event to trigger, example: POWERMGM_WIFI_ON_REQUEST for switch an WiFi
     */
    void powermgm_clear_event( EventBits_t bits );
    /**
     * @brief get a power managemt event state
     * 
     * @param   bits    event state, example: POWERMGM_STANDBY to evaluate if the system in standby
     * 
     * @return  EventBits_t    event state
     */
    EventBits_t powermgm_get_event( EventBits_t bits );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event               possible values: POWERMGM_STANDBY, POWERMGM_SILENCE_WAKEUP, POWERMGM_WAKEUP and POWERMGM_RTC_ALARM
     * @param   callback_func       pointer to the callback function 
     * @param   id                  pointer to an string
     * 
     * @return  TRUE if successful, FALSE if not successful
     * 
     * @note  Your callback function return TRUE if all fine, FALSE when you want break, by example into to standby.
     */
    bool powermgm_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief registers a callback function which is called on a corresponding loop event
     * 
     * @param   event               possible values: POWERMGM_STANDBY, POWERMGM_SILENCE_WAKEUP, POWERMGM_WAKEUP
     * @param   callback_func       pointer to the callback function 
     * @param   id                  pointer to an string
     */
    bool powermgm_register_loop_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief send an interrupt disable request
     */
    void powermgm_disable_interrupts( void );
    /**
     * @brief send an interrupt enable request
     */
    void powermgm_enable_interrupts( void );

#endif // _POWERMGM_H
