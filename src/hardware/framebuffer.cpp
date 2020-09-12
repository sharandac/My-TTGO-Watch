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

#include "framebuffer.h"

lv_color_t *framebuffer;

static lv_disp_buf_t disp_buf;
lv_disp_drv_t *framebuffer_disp_drv = NULL;
const lv_area_t *framebuffer_area = NULL;
lv_color_t *framebuffer_color_p = NULL;

volatile bool DRAM_ATTR framebuffer_flag = false;
portMUX_TYPE DRAM_ATTR FRAMEBUFFER_Mux = portMUX_INITIALIZER_UNLOCKED;
TaskHandle_t _framebuffer_Task;

void framebuffer_Task( void * pvParameters );
static void framebuffer_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

void framebuffer_setup( void ) {
    framebuffer = (lv_color_t*)ps_malloc( lv_disp_get_hor_res( NULL ) * lv_disp_get_ver_res( NULL ) * sizeof( lv_color_t ) );
    if ( framebuffer == NULL ) {
        log_e("framebuffer 1 malloc failed");
        while(1);
    }
    lv_disp_buf_init( &disp_buf, framebuffer, NULL, lv_disp_get_hor_res( NULL ) * lv_disp_get_ver_res( NULL ) );

    lv_disp_t *system_disp;
    system_disp = lv_disp_get_default();
    system_disp->driver.flush_cb = framebuffer_flush;
    system_disp->driver.hor_res = lv_disp_get_hor_res( NULL );
    system_disp->driver.ver_res = lv_disp_get_ver_res( NULL );
    system_disp->driver.buffer = &disp_buf;

    xTaskCreatePinnedToCore(  framebuffer_Task,     /* Function to implement the task */
                              "framebuffer Task",   /* Name of the task */
                              3000,             /* Stack size in words */
                              NULL,             /* Task input parameter */
                              1,                /* Priority of the task */
                              &_framebuffer_Task,   /* Task handle. */
                              0 );

    log_i("framebuffer enable");
}

static void framebuffer_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    while( framebuffer_flag );

    portENTER_CRITICAL(&FRAMEBUFFER_Mux);
    framebuffer_flag = true;
    framebuffer_disp_drv = disp_drv;
    framebuffer_area = area;
    framebuffer_color_p = color_p;
    portEXIT_CRITICAL(&FRAMEBUFFER_Mux);
}

void framebuffer_Task( void * pvParameters ) {
    static uint64_t nextmillis = 0;
    static int32_t frame = 0;
    TTGOClass *ttgo = TTGOClass::getWatch();

    log_i("start framebuffer refresh task");

    while ( true ) {
        if ( nextmillis < millis() ) {
            nextmillis = millis() + 1000;
            log_i("framerate: %d", frame );
            frame = 0;
        }
        if ( framebuffer_flag ) {
            uint32_t size = (framebuffer_area->x2 - framebuffer_area->x1 + 1) * (framebuffer_area->y2 - framebuffer_area->y1 + 1) ;
            ttgo->tft->setAddrWindow(framebuffer_area->x1, framebuffer_area->y1, (framebuffer_area->x2 - framebuffer_area->x1 + 1), (framebuffer_area->y2 - framebuffer_area->y1 + 1)); /* set the working window */
            ttgo->tft->pushColors(( uint16_t *)framebuffer_color_p, size, false);
            portENTER_CRITICAL(&FRAMEBUFFER_Mux);
            framebuffer_flag = false;
            portEXIT_CRITICAL(&FRAMEBUFFER_Mux);
            lv_disp_flush_ready(framebuffer_disp_drv);
            frame++;
       }
    }
}