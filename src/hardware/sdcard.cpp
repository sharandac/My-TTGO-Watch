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

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
    #include "esp_bt.h"
    #include "esp_task_wdt.h"
    #include "sdcard.h"
    #ifdef M5PAPER

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <TTGO.h>
        
        #if defined( LILYGO_WATCH_HAS_SDCARD )
            SPIClass *sdhander = nullptr;
        #endif
    #elif defined( LILYGO_WATCH_2021 )    
    #else
        #warning "no hardware driver for sd_card"
    #endif
#endif

#include "powermgm.h"

static bool sdcard_mounted = false;
static bool sdcard_block_unmount = false;
bool sdcard_powermgm_event_cb( EventBits_t event, void *arg );

void sdcard_setup( void ) {
#ifdef NATIVE_64BIT

#else
    #ifdef M5PAPER

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #if defined( LILYGO_WATCH_HAS_SDCARD )
            /**
             * as small hack to reduce internal heap memory
             * consumption from 27k to 5k while using sd lib
             */
            heap_caps_malloc_extmem_enable( 1 );
            if (!sdhander) {
                sdhander = new SPIClass(HSPI);
                sdhander->begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
            }
            if (!SD.begin(SD_CS, *sdhander)) {
                log_e("SD Card Mount Failed");
            }
            heap_caps_malloc_extmem_enable( 16 * 1024 );

            sdcard_mounted = true;
        #endif    
    #endif
#endif
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, sdcard_powermgm_event_cb, "sdcard powermgm" );
}

bool sdcard_powermgm_event_cb( EventBits_t event, void *arg ) {
    bool retval = false;

    #if defined( LILYGO_WATCH_HAS_SDCARD )
        switch( event ) {
            case POWERMGM_SILENCE_WAKEUP:
                log_d("go silence wakeup");
                if( !sdcard_mounted ) {
                    heap_caps_malloc_extmem_enable( 1 );
                    if ( !SD.begin( SD_CS, *sdhander) ) {
                        log_e("SD Card Mount Failed");
                    }
                    heap_caps_malloc_extmem_enable( 16 * 1024 );
                    sdcard_mounted = true;
                }
                retval = true;
                break;
            case POWERMGM_STANDBY:
                if( sdcard_mounted && !sdcard_block_unmount ) {
                    log_d("go standby");
                    SD.end();
                    sdcard_mounted = false;
                }
                else {
                    log_w("go standby without unmount");
                }
                retval = true;
                break;
            case POWERMGM_WAKEUP:
                log_d("go wakeup");
                if( !sdcard_mounted ) {
                    heap_caps_malloc_extmem_enable( 1 );
                    if (!SD.begin( SD_CS, *sdhander)) {
                        log_e("SD Card Mount Failed");
                    }
                    heap_caps_malloc_extmem_enable( 16 * 1024 );
                    sdcard_mounted = true;
                }
                retval = true;
                break;
        }
    #else
        switch( event ) {
            case POWERMGM_SILENCE_WAKEUP:
                log_d("go silence wakeup");
                retval = true;
                break;
            case POWERMGM_STANDBY:
                log_d("go standby");
                retval = true;
                break;
            case POWERMGM_WAKEUP:
                log_d("go wakeup");
                retval = true;
                break;
        }
    #endif
    return( retval );
}

void sdcard_block_unmounting( bool block_unmount ) {
    sdcard_block_unmount = block_unmount;
}