/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#ifndef _WATCHFACE_APP_TILE_H
    #define _WATCHFACE_APP_TILE_H

    #include <TTGO.h>

    #define WATCHFACE_LOG                       log_i

    #define WATCHFACE_DIAL_IMAGE_FILE           "/spiffs/watchface_dial.png"
    #define WATCHFACE_HOUR_IMAGE_FILE           "/spiffs/watchface_hour.png"
    #define WATCHFACE_MIN_IMAGE_FILE            "/spiffs/watchface_min.png"
    #define WATCHFACE_SEC_IMAGE_FILE            "/spiffs/watchface_sec.png"
    #define WATCHFACE_HOUR_SHADOW_IMAGE_FILE    "/spiffs/watchface_hour_s.png"
    #define WATCHFACE_MIN_SHADOW_IMAGE_FILE     "/spiffs/watchface_min_s.png"
    #define WATCHFACE_SEC_SHADOW_IMAGE_FILE     "/spiffs/watchface_sec_s.png"

    void watchface_app_tile_setup( void );
    void watchface_enable_tile_after_wakeup( bool enable );
    void watchface_reload_theme( void );
    void watchface_reload_and_test( uint32_t return_tile );

#endif // _WATCHFACE_APP_TILE_H