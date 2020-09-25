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
#ifndef _MOTOR_H
    #define _MOTOR_H

    #include "TTGO.h"

    #define MOTOR_JSON_CONFIG_FILE  "/motor.json"           /** @brief defines binary config file name */

    /**
     * @brief motor config structure in memory
     */
    typedef struct {
        bool vibe = true;           /** @brief vibe config item, true if vibe enabled, false if disabled */
    } motor_config_t;

    /**
     * @brief setup motor I/O
     */
    void motor_setup( void );
    /**
     * @brief let vibe motor for n * 10ms
     * 
     * @param   time    time in 10ms
     * @enforce motor will vibrate even if "vibe feedback" option is deactivated
     *  It is usefull for alrm or notifications which can be set independently
     */
    void motor_vibe( int time, bool enforced = false );
    /*
     * @brief   get the current vibe configuration
     * 
     * @return  bool    true means vibe enable, false means disable
     */
    bool motor_get_vibe_config( void );
    /**
     * @brief   set the current vibe configuration
     * 
     * @param   enable    true means vibe enable, false means disable
     */
    void motor_set_vibe_config( bool enable );
    /**
     * @brief  store the current configuration to SPIFFS
     */
    void motor_save_config( void );
    /**
     * @brief   read the configuration from SPIFFS
     */
    void motor_read_config( void );

#endif // _MOTOR_H