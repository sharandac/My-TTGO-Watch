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
#ifndef _FRAMEBUFFER_H
    #define _FRAMEBUFFER_H
    
    #include "lvgl.h"
    #include "config.h"

    #ifdef NATIVE_64BIT
            #define FRAMEBUFFER_BUFFER_W        LV_HOR_RES_MAX
            #define FRAMEBUFFER_BUFFER_H        LV_VER_RES_MAX
    #else
        #if defined( M5PAPER )
            #define FRAMEBUFFER_BUFFER_W        RES_X_MAX
            #define FRAMEBUFFER_BUFFER_H        RES_Y_MAX
            #define FRAMEBUFFER_REFRESH_DELAY   100
        #elif defined( M5CORE2 )
            #define FRAMEBUFFER_BUFFER_W        RES_X_MAX
            #define FRAMEBUFFER_BUFFER_H        10
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            #define FRAMEBUFFER_BUFFER_W        RES_X_MAX
            #define FRAMEBUFFER_BUFFER_H        10
        #elif defined( LILYGO_WATCH_2021 )
            #define FRAMEBUFFER_BUFFER_W        RES_X_MAX
            #define FRAMEBUFFER_BUFFER_H        10
        #elif defined( WT32_SC01 )
            #define FRAMEBUFFER_BUFFER_W        RES_X_MAX
            #define FRAMEBUFFER_BUFFER_H        40
        #endif
    #endif

    #define FRAMEBUFFER_BUFFER_SIZE     ( FRAMEBUFFER_BUFFER_W * FRAMEBUFFER_BUFFER_H )

    /**
     * @brief setup framebuffer
     */
    void framebuffer_setup( void );
    /**
     * @brief force framebuffer refresh to screen/display
     */
    void framebuffer_refresh( void );
#endif // _FRAMEBUFFER_H