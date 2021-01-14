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
#include "config.h"
#include <TTGO.h>
#include <esp_ipc.h>

#include "framebuffer.h"
#include "powermgm.h"
#include "alloc.h"

lv_color_t *framebuffer1 = NULL;
lv_color_t *framebuffer2 = NULL;

static lv_disp_buf_t disp_buf;

static void framebuffer_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

void framebuffer_setup( void ) {

#if defined( TWATCH_USE_PSRAM_ALLOC_LVGL ) && defined( CONFIG_SPIRAM_SPEED_80M )
    TTGOClass *ttgo = TTGOClass::getWatch();
    ttgo->tft->initDMA();

    framebuffer1 = (lv_color_t*)calloc( sizeof(lv_color_t), FRAMEBUFFER_BUFFER_SIZE );
    if ( framebuffer1 == NULL ) {
        log_e("framebuffer 1 malloc failed");
        return;
    }

    framebuffer2 = (lv_color_t*)calloc( sizeof(lv_color_t), FRAMEBUFFER_BUFFER_SIZE );
    if ( framebuffer2 == NULL ) {
        log_e("framebuffer 2 malloc failed");
        return;
    }

    lv_disp_buf_init( &disp_buf, framebuffer1, framebuffer2, FRAMEBUFFER_BUFFER_SIZE );

    lv_disp_t *system_disp;
    system_disp = lv_disp_get_default();
    system_disp->driver.flush_cb = framebuffer_flush;
    system_disp->driver.hor_res = lv_disp_get_hor_res( NULL );
    system_disp->driver.ver_res = lv_disp_get_ver_res( NULL );
    system_disp->driver.buffer = &disp_buf;

    log_i("custom arduino-esp32 framework detected, double DMA framebuffer enable");
#else
    return;
#endif

}

static void framebuffer_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) ;
    ttgo->tft->startWrite();
    ttgo->tft->setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
    ttgo->tft->pushPixelsDMA(( uint16_t *)color_p, size);
    ttgo->tft->endWrite();
    lv_disp_flush_ready( disp_drv );
}
