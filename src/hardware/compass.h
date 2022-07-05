/****************************************************************************
 *   Mo July 4 21:17:51 2022
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
#ifndef _COMPASS_H
    #define _COMPASS_H

    #include "callback.h"

    #ifdef NATIVE_64BIT
        #include "utils/io.h"
    #else

    #endif

    #define COMPASS_UPDATE_INTERVAL         100
    #define COMPASS_CALIBRATION_INTERVAL    10
    /**
     * 
     */
    #define COMPASS_UPDATE                  _BV(0)              /** @brief event mask for data updatet */
    #define COMPASS_CALIBRATION_START       _BV(1)              /** @brief event mask for data updatet */
    #define COMPASS_CALIBRATION_DONE        _BV(2)              /** @brief event mask for data updatet */
    #define COMPASS_CALIBRATION_FAILED      _BV(3)              /** @brief event mask for data updatet */
    /**
     * @brief 
     */
    typedef struct {
        int x = 0;
        int y = 0;
        int z = 0;
        int azimuth = 0;
        uint8_t bearing = 0;
        char direction[4] = "";
    } compass_data_t;
    /**
     * @brief setup compass
     */
    void compass_setup( void );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event           possible values: COMPASS_UPDATE
     * @param   callback_func   pointer to the callback function
     * @param   id              program id
     * 
     * @return  true if success, false if failed
     */
    bool compass_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );
    /**
     * @brief start compass calibration
     * 
     * @return true 
     * @return false 
     */
    bool compass_start_calibration( void );

    void compass_on( void );

    void compass_off( void );
    /**
     * @brief check if compass available
     * 
     * @return true 
     * @return false 
     */
    bool compass_available( void );

#endif // _COMPASS_H