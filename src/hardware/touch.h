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
#ifndef _TOUCH_H
    #define _TOUCH_H

    #include "hardware/config/touchconfig.h"
    #include "callback.h"

    #define TOUCH_UPDATE        _BV(0)      /** @brief event mask for touch update information */
    #define TOUCH_CONFIG_CHANGE _BV(1)      /** @brief event mask for touch config change */
    /**
     * @brief touch info structure 
     */
    typedef struct {
        bool touched;           /** @brief true if touched or false if not */
        int16_t x_coor;         /** @brief x coordinate when touched */
        int16_t y_coor;         /** @brief y coordinate when touched */
    } touch_t;
    /**
     * @brief setup touch
     */
    void touch_setup( void );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event  possible values: TOUCH_UPDATE
     * @param   callback_func   pointer to the callback function 
     * @param   id      program id
     */
    bool touch_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief lock the touch interface
     */
    bool touch_lock_take( void );
    /**
     * @brief unlock the touch interface
     */
    void touch_lock_give( void );
    /**
     * @brief get the current x scale value
     * 
     * @return float 
     */
    float touch_get_x_scale( void );
    /**
     * @brief get the current y scale value
     * 
     * @return float 
     */
    float touch_get_y_scale( void );
    /**
     * @brief set a new x scale value
     * 
     * @param value 
     */
    void touch_set_x_scale( float value );
    /**
     * @brief set a new y scale value
     * 
     * @param value 
     */
    void touch_set_y_scale( float value );
    /**
     * @brief get the current touch pos
     * 
     * @param   x   pointer to an int16_t variable thats holds the x pos
     * @param   y   pointer to an int16_t variable thats holds the y pos
     * 
     * @return  true if position valid and touch pressed, false if not valid
     */
    bool touch_getXY( int16_t &x, int16_t &y );
    
#endif // _TOUCH_H