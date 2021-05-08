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

    #define BLOCK_SIZE              8192
    #define SCREENSHOT_FILE_NAME    "/spiffs/screen.png"

    typedef struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    } rgba_t;

    struct RAW_RGB {
        rgba_t data[ 240 * 240 ];
    };

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