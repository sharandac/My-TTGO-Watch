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

lv_color_t *framebuffer;

static lv_disp_buf_t disp_buf;

lv_disp_drv_t *framebuffer_disp_drv = NULL;
const lv_area_t *framebuffer_area = NULL;
lv_color_t *framebuffer_color_p = NULL;

volatile bool DRAM_ATTR framebuffer_flag = false;
portMUX_TYPE DRAM_ATTR FRAMEBUFFER_Mux = portMUX_INITIALIZER_UNLOCKED;

static int32_t frame = 0;
static int32_t framerate = 0;

bool framebuffer_powermgm_event_cb( EventBits_t event, void *arg );
void framebuffer_ipc_call( void * arg );
static void framebuffer_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

void framebuffer_setup( void ) {
    framebuffer = (lv_color_t*)ps_malloc( lv_disp_get_hor_res( NULL ) * lv_disp_get_ver_res( NULL ) * sizeof( lv_color_t ) );
    if ( framebuffer == NULL ) {
        log_e("framebuffer 1 malloc failed");
        return;
    }
    lv_disp_buf_init( &disp_buf, framebuffer, NULL, lv_disp_get_hor_res( NULL ) * lv_disp_get_ver_res( NULL ) );

    powermgm_register_cb( POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, framebuffer_powermgm_event_cb, "framebuffer" );

    lv_disp_t *system_disp;
    system_disp = lv_disp_get_default();
    system_disp->driver.flush_cb = framebuffer_flush;
    system_disp->driver.hor_res = lv_disp_get_hor_res( NULL );
    system_disp->driver.ver_res = lv_disp_get_ver_res( NULL );
    system_disp->driver.buffer = &disp_buf;

    log_i("framebuffer enable");
}

bool framebuffer_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case    POWERMGM_STANDBY:           framebuffer_flag = true;
                                            log_i("go standby");
                                            break;
        case    POWERMGM_SILENCE_WAKEUP:    framebuffer_flag = false;
                                            log_i("go silence wakeup");
                                            break;
        case    POWERMGM_WAKEUP:            framebuffer_flag = false;
                                            log_i("go wakeup");
                                            break;
    }
    return( true );
}

static void framebuffer_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    static uint64_t nextmillis = 0;

    if ( framebuffer_flag )
        return;

    portENTER_CRITICAL(&FRAMEBUFFER_Mux);
    framebuffer_disp_drv = disp_drv;
    framebuffer_area = area;
    framebuffer_color_p = color_p;
    framebuffer_flag = true;

    if ( nextmillis < millis() ) {
        nextmillis = millis() + 1000;
        framerate = frame;
        frame = 0;
    }
    portEXIT_CRITICAL(&FRAMEBUFFER_Mux);

    esp_ipc_call( 0, framebuffer_ipc_call, NULL );
}

void framebuffer_ipc_call( void * arg ) {
    TTGOClass *ttgo = TTGOClass::getWatch();

    uint32_t size = (framebuffer_area->x2 - framebuffer_area->x1 + 1) * (framebuffer_area->y2 - framebuffer_area->y1 + 1) ;
    ttgo->tft->setAddrWindow(framebuffer_area->x1, framebuffer_area->y1, (framebuffer_area->x2 - framebuffer_area->x1 + 1), (framebuffer_area->y2 - framebuffer_area->y1 + 1)); /* set the working window */
    ttgo->tft->pushColors(( uint16_t *)framebuffer_color_p, size, false);
    portENTER_CRITICAL(&FRAMEBUFFER_Mux);
    frame++;
    framebuffer_flag = false;
    portEXIT_CRITICAL(&FRAMEBUFFER_Mux);
    lv_disp_flush_ready(framebuffer_disp_drv);
}