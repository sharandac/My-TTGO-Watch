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
static bool framebuffer_use_dma = false;

static lv_disp_buf_t disp_buf;

bool framebuffer_setup_dma( bool doubleframebuffer  );
bool framebuffer_setup_nodma( bool doubleframebuffer  );
static void framebuffer_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

void framebuffer_setup( bool dma, bool doubleframebuffer ) {
    if ( dma ) {
        TTGOClass *ttgo = TTGOClass::getWatch();
        ttgo->tft->initDMA();
        framebuffer_setup_dma( doubleframebuffer );
    }
    else {
        framebuffer_setup_nodma( doubleframebuffer );
    }
}

bool framebuffer_setup_nodma( bool doubleframebuffer  ) {
    lv_color_t *tmp_framebuffer1 = NULL;
    lv_color_t *tmp_framebuffer2 = NULL;
    /*
     * allocate new framebuffer
     */
    tmp_framebuffer1 = (lv_color_t*)CALLOC( sizeof(lv_color_t), FRAMEBUFFER_BUFFER_SIZE );
    if ( tmp_framebuffer1 == NULL ) {
        log_e("framebuffer 1 malloc failed");
        return( false );
    }

    if ( doubleframebuffer ) {
        tmp_framebuffer2 = (lv_color_t*)CALLOC( sizeof(lv_color_t), FRAMEBUFFER_BUFFER_SIZE );
        if ( tmp_framebuffer2 == NULL ) {
            log_e("framebuffer 2 malloc failed");
            if ( tmp_framebuffer1 ) {
                free( tmp_framebuffer1 );
            }
            return( false );
        }
    }

    /*
     * free old framebuffer
     */
    if ( framebuffer1 ) {
        free( framebuffer1 );
    }
    if ( framebuffer2 ) {
        free( framebuffer2 );
    }
    /*
     * set new framebuffer
     */
    framebuffer1 = tmp_framebuffer1;
    if ( doubleframebuffer ) {
        framebuffer2 = tmp_framebuffer2;
        log_i("double framebuffer enable ( 2 x %d bytes )", sizeof(lv_color_t) * FRAMEBUFFER_BUFFER_SIZE );
    }
    else {
        framebuffer2 = NULL;
        log_i("single framebuffer enable ( %d bytes )", sizeof(lv_color_t) * FRAMEBUFFER_BUFFER_SIZE );
    }
    /*
     * set LVGL driver
     */
    lv_disp_t *system_disp;
    lv_disp_buf_init( &disp_buf, framebuffer1, framebuffer2, FRAMEBUFFER_BUFFER_SIZE );
    system_disp = lv_disp_get_default();
    system_disp->driver.flush_cb = framebuffer_flush;
    system_disp->driver.hor_res = lv_disp_get_hor_res( NULL );
    system_disp->driver.ver_res = lv_disp_get_ver_res( NULL );
    system_disp->driver.buffer = &disp_buf;
    framebuffer_use_dma = false;

    return( true );
}

bool framebuffer_setup_dma( bool doubleframebuffer  ) {

#if defined( TWATCH_USE_PSRAM_ALLOC_LVGL ) && defined( CONFIG_SPIRAM_SPEED_80M )
    lv_color_t *tmp_framebuffer1 = NULL;
    lv_color_t *tmp_framebuffer2 = NULL;
    /*
     * allocate new framebuffer
     */
    tmp_framebuffer1 = (lv_color_t*)calloc( sizeof(lv_color_t), FRAMEBUFFER_BUFFER_SIZE );
    if ( tmp_framebuffer1 == NULL ) {
        log_e("framebuffer 1 malloc failed");
        return( false );
    }

    if ( doubleframebuffer ) {
        tmp_framebuffer2 = (lv_color_t*)calloc( sizeof(lv_color_t), FRAMEBUFFER_BUFFER_SIZE );
        if ( tmp_framebuffer2 == NULL ) {
            log_e("framebuffer 2 malloc failed");
            if ( tmp_framebuffer1 ) {
                free( tmp_framebuffer1 );
            }
            return( false );
        }
    }

    /*
     * free old framebuffer
     */
    if ( framebuffer1 ) {
        free( framebuffer1 );
    }
    if ( framebuffer2 ) {
        free( framebuffer2 );
    }
    /*
     * set new framebuffer
     */
    framebuffer1 = tmp_framebuffer1;
    if ( doubleframebuffer ) {
        framebuffer2 = tmp_framebuffer2;
        log_i("custom arduino-esp32 framework detected, double DMA framebuffer enable ( 2 x %d bytes )", sizeof(lv_color_t) * FRAMEBUFFER_BUFFER_SIZE );
    }
    else {
        framebuffer2 = NULL;
        log_i("custom arduino-esp32 framework detected, single DMA framebuffer enable ( %d bytes )", sizeof(lv_color_t) * FRAMEBUFFER_BUFFER_SIZE );
    }
    /*
     * set LVGL driver
     */
    lv_disp_t *system_disp;
    lv_disp_buf_init( &disp_buf, framebuffer1, framebuffer2, FRAMEBUFFER_BUFFER_SIZE );
    system_disp = lv_disp_get_default();
    system_disp->driver.flush_cb = framebuffer_flush;
    system_disp->driver.hor_res = lv_disp_get_hor_res( NULL );
    system_disp->driver.ver_res = lv_disp_get_ver_res( NULL );
    system_disp->driver.buffer = &disp_buf;
    framebuffer_use_dma = true;

    return( true );
#else
    log_i("no custom arduino-esp32 framework detected, use normal framebuffer");
    return( false );
#endif
}

static void framebuffer_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) ;
    ttgo->tft->startWrite();
    ttgo->tft->setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
    if ( framebuffer_use_dma ) {
        ttgo->tft->pushPixelsDMA(( uint16_t *)color_p, size);
    }
    else {
        ttgo->tft->pushPixels(( uint16_t *)color_p, size);
    }
    ttgo->tft->endWrite();
    lv_disp_flush_ready( disp_drv );
}
