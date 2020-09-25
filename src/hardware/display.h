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
#ifndef _DISPLAY_H
    #define _DISPLAY_H

    #include "callback.h"

    #define DISPLAYCTL_BRIGHTNESS       _BV(0)          /** @brief event mask display brightness, callback arg is (uint32_t*) */
    #define DISPLAYCTL_TIMEOUT          _BV(1)          /** @brief event mask display brightness, callback arg is (uint32_t*) */

    #define DISPLAY_JSON_CONFIG_FILE    "/display.json" /** @brief defines json config file name */
    
    #define DISPLAY_MIN_TIMEOUT         15              /** @brief min display timeout */
    #define DISPLAY_MAX_TIMEOUT         300             /** @brief max display timeout */
    #define DISPLAY_MIN_BRIGHTNESS      8               /** @brief min display brightness */
    #define DISPLAY_MAX_BRIGHTNESS      255             /** @brief max display brightness */
    #define DISPLAY_MIN_ROTATE          0               /** @brief min display rotation */
    #define DISPLAY_MAX_ROTATE          270             /** @brief max display rotation */

    /**
     * @brief display config structure
     */
    typedef struct {
        uint32_t brightness = DISPLAY_MAX_BRIGHTNESS;   /** @brief display brightness */
        uint32_t timeout = DISPLAY_MIN_TIMEOUT;         /** @brief display time out */
        uint32_t rotation = 0;                          /** @brief display rotation */
        bool block_return_maintile = false;             /** @brief block back to main tile on standby */
        uint32_t background_image = 2;                  /** @brief background image */
    } display_config_t;

    /**
     * @brief setup display
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void display_setup( void );
    /**
     * @brief display loop
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void display_loop( void );
    /**
     * @brief save config for display to spiffs
     */
    void display_save_config( void );
    /**
     * @brief read config for display from spiffs
     */
    void display_read_config( void );
    /**
     * @brief read the timeout from config
     * 
     * @return  timeout in seconds
     */
    uint32_t display_get_timeout( void );
    /**
     * @brief set timeout for the display
     * 
     * @param timeout in seconds
     */
    void display_set_timeout( uint32_t timeout );
    /**
     * @brief read the brightness from config
     * 
     * @return  brightness from 0-255
     */
    uint32_t display_get_brightness( void );
    /**
     * @brief set brightness for the display
     * 
     * @param brightness brightness from 0-255
     */
    void display_set_brightness( uint32_t brightness );
    /**
     * @brief read the rotate from the display
     * 
     * @return  rotation from 0-270 degree in 90 degree steps
     */
    uint32_t display_get_rotation( void );
    /**
     * @brief set the rotate for the display
     * 
     * @param rotation from 0-270 in 90 degree steps
     */
    void display_set_rotation( uint32_t rotation );
    /**
     * @brief read the block_return_maintile while wakeup
     * 
     * @param rotation from 0-270 in 90 degree steps
     */
    bool display_get_block_return_maintile( void );
    /**
     * @brief set the block_return_maintile while wakeup
     * 
     * @param block_return_maintile true or false, true means no autoreturn zu maintile
     */
    void display_set_block_return_maintile( bool block_return_maintile );
    /**
     * @brief get the background for the display
     * 
     * @return image number
     */
    uint32_t display_get_background_image( void );
    /**
     * @brief set the background for the display
     * 
     * @param background_image image number
     */
    void display_set_background_image( uint32_t background_image );
    /**
     * @brief set display into standby
     */
    void display_standby( void );
    /**
     * @brief set display into normal mode or leave it in standby if a silence wakeup occur
     */
    void display_wakeup( bool silence );
    /**
     * @brief registers a callback function which is called on a corresponding event
     * 
     * @param   event           possible values: DISPLAYCTL_BRIGHTNESS and DISPLAYCTL_TIMEOUT
     * @param   callback_func   pointer to the callback function
     * @param   id              program id
     * 
     * @return  true if success, false if failed
     */
    bool display_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );


#endif // _DISPLAY_H
