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
#include <endian.h>
#include "config.h"
#include "screenshot.h"

#include "utils/alloc.h"
#include "gui/png_decoder/lv_png.h"

static RAW_RGB *raw_rgb;

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p );

void screenshot_setup( void ) {
    raw_rgb = NULL;
}

void screenshot_take( void ) {
    lv_disp_drv_t driver;
    lv_disp_t *system_disp;

    log_i("take screenshot");
    /**
     * allocate screenshot memory
     */
    raw_rgb = (RAW_RGB*)MALLOC( sizeof( RAW_RGB ) );
    if ( raw_rgb == NULL ) {
        log_e("screenshot malloc failed");
        return;
    }
    /**
     * redirect display driver
     */
    system_disp = lv_disp_get_default();
    driver.flush_cb = system_disp->driver.flush_cb;
    system_disp->driver.flush_cb = screenshot_disp_flush;
    lv_obj_invalidate( lv_scr_act() );
    lv_refr_now( system_disp );
    system_disp->driver.flush_cb = driver.flush_cb;
}

void screenshot_save( void ) {
    /**
     * check if screenshot memory allocated
     */
    if ( raw_rgb == NULL ) {
        log_e("no screenshot memory allocated");
        return;
    }
    /**
     * delete old screenshot
     */
    remove( SCREENSHOT_FILE_NAME );
    /**
     * open new screenshoot file and write them
     */
    lv_rgba_as_png( SCREENSHOT_FILE_NAME, (const uint8_t*)raw_rgb, 240, 240 );
    log_i("save screenshot");
    /**
     * free screenshot memory
     */
    free( raw_rgb );
}

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p ) {

    uint32_t x, y;
    uint8_t r,g,b;
    uint16_t *data = (uint16_t *)color_p;
    /**
     * check if screenshot memory allocated
     */
    if ( raw_rgb == NULL ) {
        log_e("no screenshot memory allocated");
        return;
    }
    /**
     * copy data into screenshot memory as RGBA
     */
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            r = ( bswap16( *data ) & 0xf800 ) >> 8;
            g = ( bswap16( *data ) & 0x07E0 ) >> 3;
            b = ( bswap16( *data ) & 0x001F ) << 3;
            raw_rgb->data[ y * lv_disp_get_hor_res( NULL ) + x ].r = r;
            raw_rgb->data[ y * lv_disp_get_hor_res( NULL ) + x ].g = g;
            raw_rgb->data[ y * lv_disp_get_hor_res( NULL ) + x ].b = b;
            raw_rgb->data[ y * lv_disp_get_hor_res( NULL ) + x ].a = 255;
            data++;
        }
    } 
    lv_disp_flush_ready(disp_drv);
}
