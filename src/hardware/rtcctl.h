/****************************************************************************
 *   Copyright  2020  Jakub Vesely
 *   Email: jakub_vesely@seznam.cz
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

#ifndef _RTCCTL_H
    #define _RTCCTL_H

    #include "TTGO.h"
    #include "callback.h"

    #define RTCCTL_ALARM_OCCURRED    _BV(0)
    #define RTCCTL_ALARM_TERM_SET    _BV(1)
    #define RTCCTL_ALARM_DISABLED    _BV(2)
    #define RTCCTL_ALARM_ENABLED     _BV(3)

    /**
     * @brief setup rtc controller routine
     */
    void rtcctl_setup( void );
    /**
     * @brief rtc controller loop routine
     */
    void rtcctl_loop( void );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event           possible values: RTCCTL_ALARM, RTCCTL_ALARM_SET, RTCCTL_ALARM_ENABLE and RTCCTL_ALARM_DISABLE
     * @param   callback_func   pointer to the callback function 
     * @param   id              programm id
     * 
     * @return  true if success, false if failed
     */
    bool rtcctl_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief set an alarm time
     *
     * @param   hour    hour to set
     * @param   minute  minute to set
     */
    void rtcctl_set_alarm_term( uint8_t hour, uint8_t minute );
    /**
     * @brief   enable alarm
     */
    void rtcctl_enable_alarm( void );
    /**
     * @brief   disable alarm
     */
    void rtcctl_disable_alarm( void );
    /**
     * @brief   check rtc time
     *
     * @return  true if equal, otherwise false
     */
    bool rtcctl_is_alarm_time();
    /**
     * @brief   get the current alarm state
     * 
     * @return  true if enable, false is disable
     */
    bool rtcctl_is_alarm_enabled( void );

    /*
     * @brief   returns currently set alarm hour - a value can be set when alarm is currently disabled as well
     */
    uint8_t rtcctl_get_alarm_hour();

    /*
     * @brief   returns currently set alarm minute - a value can be set when alarm is currently disabled as well
     */
    uint8_t rtcctl_get_alarm_minute();

#endif // _RTCCTL_H
