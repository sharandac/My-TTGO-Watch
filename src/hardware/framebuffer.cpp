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
#include "lvgl.h"
#include "framebuffer.h"
#include "powermgm.h"
#include "utils/alloc.h"
/**
 * device depends includes and inits
 */
#ifdef NATIVE_64BIT
    /*
     * To fix SDL's "undefined reference to WinMain" issue
     */
    #define SDL_MAIN_HANDLED
    #include <unistd.h>
    #include <SDL2/SDL.h>
    #include "display/monitor.h"
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #if defined( M5PAPER )
        #include <M5EPD.h>

        M5EPD_Canvas canvas(&M5.EPD);
        static uint64_t refreshdelay = 0;                                   /** @brief refresh delay counter to mark next screen refresh */
        static lv_coord_t min_x = FRAMEBUFFER_BUFFER_W, max_x = 0;          /** @brief screen area to refresh */
        static lv_coord_t min_y = FRAMEBUFFER_BUFFER_H, max_y = 0;          /** @brief screen area to refresh */
    #elif defined( M5CORE2 )
        #include <utility/In_eSPI.h>

        TFT_eSPI tft = TFT_eSPI();
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <TTGO.h>
    #elif defined( LILYGO_WATCH_2021 )
        #include "TFT_eSPI.h"
        #include <twatch2021_config.h>

        TFT_eSPI tft = TFT_eSPI();
    #elif defined( WT32_SC01 )
        #include "TFT_eSPI.h"

        TFT_eSPI tft = TFT_eSPI();
    #else
        #error "no hardware driver for framebuffer, please setup minimal drivers ( display/framebuffer/touch )"
    #endif
#endif

static bool framebuffer_drawing = true;                             /** @brief disable */
static bool framebuffer_use_dma = false;
lv_color_t *framebuffer_1 = NULL;                                     /** @brief pointer to a full size framebuffer */
lv_color_t *framebuffer_2 = NULL;                                     /** @brief pointer to a full size framebuffer */
uint32_t framebuffer_size = FRAMEBUFFER_BUFFER_SIZE;                /** @brief framebuffer size */

bool framebuffer_powermgm_event_cb( EventBits_t event, void *arg );
bool framebuffer_powermgm_loop_cb( EventBits_t event, void *arg );
static void framebuffer_flush_cb( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p );

void framebuffer_setup( void ) {
    static lv_disp_buf_t disp_buf;
    lv_disp_drv_t disp_drv;

    #ifdef NATIVE_64BIT
        /**
         * setup SDL
         */
        monitor_init();
    #else
        #if defined( M5PAPER )
            /**
             * setup e-ink display and clear it
             */
            M5.EPD.SetRotation( 90 );
            M5.EPD.Clear( true );
            canvas.createCanvas( 540, 960 );
            canvas.pushCanvas( UPDATE_MODE_GLD16 );
            canvas.deleteCanvas();
            /**
             * cleat next screen refresh time
             */
            refreshdelay = 0;
        #elif defined( M5CORE2 )
            tft.init();
            tft.setRotation(1);
            tft.setTextSize(1);
            tft.setTextColor(TFT_GREEN, TFT_BLACK);
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            /**
             * enable DMA only for V1 and V2
             */
            #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 )
                framebuffer_use_dma = true;
            #endif
            /**
             * if dma enabled, initDMA
             */
            if ( framebuffer_use_dma ) {
                TTGOClass *ttgo = TTGOClass::getWatch();
                ttgo->tft->initDMA();
            }
        #elif defined( LILYGO_WATCH_2021 )
            framebuffer_use_dma = true;

            pinMode( TFT_LED, OUTPUT );
            ledcSetup( 0, 4000, 8 );
            ledcAttachPin( TFT_LED, 0 );
            ledcWrite( 0, 0 );

            tft.init();
            tft.fillScreen( TFT_BLACK );
            tft.initDMA();
        #elif defined( WT32_SC01 )
            framebuffer_use_dma = true;
            tft.init();
            tft.setSwapBytes( true );
            tft.fillScreen( TFT_BLACK );
            tft.initDMA();
            tft.setRotation( 1 );
            ledcWrite(0, 0xff );
        #else
            #error "no framebuffer init function implemented, please setup minimal drivers ( display/framebuffer/touch )"
        #endif
    #endif
    /*
     * allocate new framebuffer
     */
    if ( !framebuffer_1 ) {
        if ( framebuffer_use_dma ) {
            framebuffer_1 = (lv_color_t*)calloc( sizeof(lv_color_t), FRAMEBUFFER_BUFFER_W * FRAMEBUFFER_BUFFER_H );
        }
        else {
            framebuffer_1 = (lv_color_t*)CALLOC( sizeof(lv_color_t), FRAMEBUFFER_BUFFER_W * FRAMEBUFFER_BUFFER_H );
        }
        ASSERT( framebuffer_1, "framebuffer malloc failed" );
        /**
         * log info about framebuffer
         */
        #ifdef NATIVE_64BIT
            log_d("framebuffer 1: 0x%p (%ld bytes, %dx%dpx)", framebuffer_1, FRAMEBUFFER_BUFFER_W * FRAMEBUFFER_BUFFER_H * sizeof(lv_color_t), FRAMEBUFFER_BUFFER_W, FRAMEBUFFER_BUFFER_H );
        #else
            log_i("framebuffer 1: 0x%p (%d bytes, %dx%dpx)", framebuffer_1, FRAMEBUFFER_BUFFER_W * FRAMEBUFFER_BUFFER_H * sizeof(lv_color_t), FRAMEBUFFER_BUFFER_W, FRAMEBUFFER_BUFFER_H );
        #endif
    }
    if ( !framebuffer_2 ) {
        if ( framebuffer_use_dma ) {
            framebuffer_2 = (lv_color_t*)calloc( sizeof(lv_color_t), FRAMEBUFFER_BUFFER_W * FRAMEBUFFER_BUFFER_H );
        }
        else {
            framebuffer_2 = (lv_color_t*)CALLOC( sizeof(lv_color_t), FRAMEBUFFER_BUFFER_W * FRAMEBUFFER_BUFFER_H );
        }
        ASSERT( framebuffer_2, "framebuffer malloc failed" );
        /**
         * log info about framebuffer
         */
        #ifdef NATIVE_64BIT
            log_d("framebuffer 2: 0x%p (%ld bytes, %dx%dpx)", framebuffer_2, FRAMEBUFFER_BUFFER_W * FRAMEBUFFER_BUFFER_H * sizeof(lv_color_t), FRAMEBUFFER_BUFFER_W, FRAMEBUFFER_BUFFER_H );
        #else
            log_i("framebuffer 2: 0x%p (%d bytes, %dx%dpx)", framebuffer_2, FRAMEBUFFER_BUFFER_W * FRAMEBUFFER_BUFFER_H * sizeof(lv_color_t), FRAMEBUFFER_BUFFER_W, FRAMEBUFFER_BUFFER_H );
        #endif
    }
    /*
     * set LVGL driver
     */
    lv_disp_buf_init( &disp_buf, framebuffer_1, framebuffer_2, FRAMEBUFFER_BUFFER_W * FRAMEBUFFER_BUFFER_H );
    lv_disp_drv_init( &disp_drv );
    disp_drv.flush_cb = framebuffer_flush_cb;
    disp_drv.buffer = &disp_buf;
    disp_drv.hor_res = RES_X_MAX;
    disp_drv.ver_res = RES_Y_MAX;
    lv_disp_drv_register( &disp_drv );

    /**
     * setup powermgm events and loop
     */
    powermgm_register_cb( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP| POWERMGM_WAKEUP , framebuffer_powermgm_event_cb, "powermgm framebuffer" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP , framebuffer_powermgm_loop_cb, "powermgm framebuffer loop" );
}

bool framebuffer_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_STANDBY:          log_d("go standby, refresh framebuffer");
                                        framebuffer_refresh();
                                        framebuffer_drawing = false;
                                        break;
        case POWERMGM_WAKEUP:           log_d("go wakeup");
                                        framebuffer_drawing = true;
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   log_d("go wakeup");
                                        framebuffer_drawing = false;
                                        break;
    }
    return( true );
}

bool framebuffer_powermgm_loop_cb( EventBits_t event, void *arg ) {
    #ifdef NATIVE_64BIT
    #else
        #if defined( M5PAPER )
            /**
             * check if a refresh delay is set
             */
            if ( refreshdelay != 0 ) {
                /**
                 * check if refresh time is reached
                 */
                if ( refreshdelay < millis() ) {
                    log_d("area: %d.%d / %d.%d", min_x, min_y, max_x, max_y );
                    /**
                     * check if we nee a full screen refresh or a specified area refresh
                     */
                    if ( min_x == FRAMEBUFFER_BUFFER_W && min_y == FRAMEBUFFER_BUFFER_H && max_x == 0 && max_y == 0 ) {
                        log_d("refreshing full display");
                        M5.EPD.UpdateFull( UPDATE_MODE_GC16 );
                    }
                    else {
                        log_d("refreshing specified area display");
                        M5.EPD.UpdateArea( min_x, min_y, max_x - min_x, max_y - min_y, UPDATE_MODE_GC16 );
                    }
                    /**
                     * reset refreshdelay time an area
                     */
                    refreshdelay = 0;
                    min_x = FRAMEBUFFER_BUFFER_W;
                    max_x = 0;
                    min_y = FRAMEBUFFER_BUFFER_H;
                    max_y = 0;
                }
            }
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( LILYGO_WATCH_2021 )
        #elif defined( WT32_SC01 )
        #else
            #error "no framebuffer powermgm loop event function implemented, please setup minimal drivers ( display/framebuffer/touch )"
        #endif
    #endif
    return( true );
}

void framebuffer_refresh( void ) {
    #ifdef NATIVE_64BIT
    #else
        #if defined( M5PAPER )
            /**
             * do a full screen refresh
             */
            log_d("refreshing display");
            log_d("area: %d.%d / %d.%d", min_x, min_y, max_x, max_y );
            M5.EPD.UpdateFull( UPDATE_MODE_GC16 );
            refreshdelay = 0;
            min_x = FRAMEBUFFER_BUFFER_W;
            max_x = 0;
            min_y = FRAMEBUFFER_BUFFER_H;
            max_y = 0;
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( LILYGO_WATCH_2021 )
        #elif defined( WT32_SC01 )
        #else
            #error "no framebuffer refresh function implemented, please setup minimal drivers ( display/framebuffer/touch )"
        #endif
    #endif
}

static void framebuffer_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    if( !framebuffer_drawing )
            lv_disp_flush_ready( disp_drv );

    #ifdef NATIVE_64BIT
        /**
         * flush SDL screen
         */
        #if defined( MONOCHROME ) || defined( MONOCHROME_4BIT ) || defined( MONOCHROME_EINK )
            lv_color_t *color = color_p;

            for( int y = 0 ; y < ( area->y2 - area->y1 + 1 ); y++ ) {
                for( int x = 0 ; x < ( area->x2 - area->x1 + 1 ); x++ ) {
                    #if defined( MONOCHROME_4BIT )
                        uint8_t brightness = ( lv_color_brightness( *color ) & 0xf0 );
                    #elif defined( MONOCHROME_EINK )
                        uint8_t brightness = ( lv_color_brightness( *color ) >> 2 ) * 3 + 64;
                    #else
                        uint8_t brightness = lv_color_brightness( *color );
                    #endif
                    *color = lv_color_make( brightness, brightness, brightness );
                    color++;
                }
            }     
            monitor_flush( disp_drv, area, color_p );
        #else
            monitor_flush( disp_drv, area, color_p );
        #endif
    #else
        #if defined( M5PAPER )
            lv_color_t *color = color_p;
            /**
             * set/update area to freshing
             */
            if ( min_x > area->x1 ) min_x = area->x1;
            if ( min_y > area->y1 ) min_y = area->y1;
            if ( max_x < area->x2 ) max_x = area->x2;
            if ( max_y < area->y2 ) max_y = area->y2;
            /**
             * write buffer to display
             */
            canvas.createCanvas( area->x2 - area->x1, area->y2 - area->y1 );
            /**
             * loop pixel data
             */
            for( int y = 0 ; y < ( area->y2 - area->y1 + 1 ); y++ ) {
                for( int x = 0 ; x < ( area->x2 - area->x1 + 1 ); x++ ) {
                    canvas.drawPixel( x, y, ( 255 - lv_color_brightness( *color ) ) >> 4 );
                    color++;
                }
            }
            /**
             * transfer canvas to the screen without update
             * and delete canvas
             */
            canvas.pushCanvas( area->x1, area->y1, UPDATE_MODE_NONE );
            canvas.deleteCanvas();
            /**
             * set refresh time from now + FRAMEBUFFER_REFRESH_DELAY
             */
            if ( refreshdelay == 0 ) {
                refreshdelay = millis() + FRAMEBUFFER_REFRESH_DELAY;
            }
        #elif defined( M5CORE2 )
            /**
             * get buffer size
             */
            uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) ;
            /**
             * start data trnsmission
             * set the working window
             * and start DMA transfer if enabled
             * stop transmission
             */
            tft.startWrite();
            tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1)); /* set the working window */
            tft.pushColors( ( uint16_t *)color_p, size );
            tft.endWrite();
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            TTGOClass *ttgo = TTGOClass::getWatch();
            /**
             * get buffer size
             */
            uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) ;
            /**
             * start data trnsmission
             * set the working window
             * and start DMA transfer if enabled
             * stop transmission
             */
            ttgo->tft->startWrite();
            ttgo->tft->setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1));
            if ( framebuffer_use_dma )
                ttgo->tft->pushPixelsDMA(( uint16_t *)color_p, size);
            else
                ttgo->tft->pushPixels(( uint16_t *)color_p, size);
            ttgo->tft->endWrite();
        #elif defined( LILYGO_WATCH_2021 )
            /**
             * get buffer size
             */
            uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) ;
            /**
             * start data trnsmission
             * set the working window
             * and start DMA transfer if enabled
             * stop transmission
             */
            if ( framebuffer_use_dma ) {
                tft.endWrite();
                tft.startWrite();
                tft.pushImageDMA( area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1), ( uint16_t *)color_p );
            }
            else {
                tft.startWrite();
                tft.pushImage( area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1), ( uint16_t *)color_p );
                tft.flush();
                tft.endWrite();
            }
        #elif defined( WT32_SC01 )
            /**
             * get buffer size
             */
            uint32_t size = (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) ;
            /**
             * stop/wait last transmission
             * start data trnsmission
             * set the working window
             * and start DMA transfer if enabled
             */
            if ( framebuffer_use_dma ) {
                tft.endWrite();
                tft.startWrite();
                tft.pushImageDMA( area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1), ( uint16_t *)color_p );
            }
            else {
                tft.startWrite();
                tft.pushImage( area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1), ( uint16_t *)color_p );
                tft.flush();
                tft.endWrite();
            }
        #else
            #error "no LVGL display driver function implemented, please setup minimal drivers ( display/framebuffer/touch )"
        #endif
    #endif
    lv_disp_flush_ready( disp_drv );
}
