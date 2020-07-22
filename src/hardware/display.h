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

    #define DISPLAY_MIN_TIMEOUT         15
    #define DISPLAY_MAX_TIMEOUT         300

    #define DISPLAY_MIN_BRIGHTNESS      32
    #define DISPLAY_MAX_BRIGHTNESS      255

    #define DISPLAY_MIN_ROTATE          0
    #define DISPLAY_MAX_ROTATE          270

    typedef struct {
        uint32_t brightness = DISPLAY_MIN_BRIGHTNESS;
        uint32_t timeout = DISPLAY_MIN_TIMEOUT;
        uint32_t rotation = 0;
    } display_config_t;

    #define DISPLAY_CONFIG_FILE  "/display.cfg"

    /*
     * @brief setup display
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void display_setup( TTGOClass *ttgo );
    /*
     * @brief display loop
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void display_loop( TTGOClass *ttgo );
    /*
     * @brief save config for display to spiffs
     */
    void display_save_config( void );
    /*
     * @brief read config for display from spiffs
     */
    void display_read_config( void );
    /*
     * @brief read the timeout from config
     * 
     * @return  timeout in seconds
     */
    uint32_t display_get_timeout( void );
    /*
     * @brief set timeout for the display
     * 
     * @param timeout in seconds
     */
    void display_set_timeout( uint32_t timeout );
    /*
     * @brief read the brightness from config
     * 
     * @return  brightness from 0-255
     */
    uint32_t display_get_brightness( void );
    /*
     * @brief set brightness for the display
     * 
     * @param brightness brightness from 0-255
     */
    void display_set_brightness( uint32_t brightness );
    /*
     * @brief read the rotate from the display
     * 
     * @return  rotation from 0-270 degree in 90 degree steps
     */
    uint32_t display_get_rotation( void );
    /*
     * @brief set the rotate for the display
     * 
     * @param rotation from 0-270 in 90 degree steps
     */
    void display_set_rotation( uint32_t rotation );

#endif // _DISPLAY_H