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

    #define SCREENSHOT_FILE_NAME    "/screen.data"

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

/*
    struct PNG_IMAGE {
        uint8_t     png[ 8 ];
        uint32_t    IHDR_len;
        uint8_t     IHDR[ 4 ];
        uint32_t    width;
        uint32_t    height;
        uint8_t     bitdepth;
        uint8_t     colortype;
        uint8_t     compression;
        uint8_t     filter;
        uint8_t     interlace_method;
        uint32_t    IHDR_crc;
        uint32_t    IDAT_len = LV_HOR_RES_MAX * LV_VER_RES_MAX * 3;
        uint8_t     IDAT[ 4 ];
        uint8_t     data[ LV_HOR_RES_MAX * LV_VER_RES_MAX * 3 ];
        uint32_t    IDAT_crc;
        uint32_t    IEND_len;
        uint8_t     IEND[ 4 ];
        uint32_t    IEND_crc;
    } __attribute__((packed));
*/

#endif // _SCREENSHOT_H