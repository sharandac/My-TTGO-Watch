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
#ifndef _SCREENSHOT_H
    #define _SCREENSHOT_H

    #include "config.h"

    #define SCREENSHOT_FILE_NAME    "/spiffs/screen.png"
    /**
     * @brief rgba pixel structure
     */
    typedef struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } rgb_t;
    /**
     * @brief grey pixel structure
     */
    typedef struct {
        uint8_t grey;
    } grey_t;
    /**
     * @brief rgba image structure
     */
    typedef struct {
        rgb_t data[ RES_X_MAX * RES_Y_MAX ];
    } raw_img_rgb_t;
    /**
     * @brief grey image structure
     */
    typedef struct {
        grey_t data[ RES_X_MAX * RES_Y_MAX ];
    } raw_img_grey_t;
    /**
     * @brief setup screenshot
     */
    void screenshot_setup( void );
    /**
     * @brief take a screenshoot an store it in psram
     */
    void screenshot_take( void );
    /**
     * @brief store a screenshoot from psram to spiffs
     */
    void screenshot_save( void );

#endif // _SCREENSHOT_H