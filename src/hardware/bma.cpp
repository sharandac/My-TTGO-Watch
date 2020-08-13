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
#include <soc/rtc.h>

#include "bma.h"
#include "powermgm.h"
#include "json_psram_allocator.h"

#include "gui/statusbar.h"

EventGroupHandle_t bma_event_handle = NULL;
bma_config_t bma_config[ BMA_CONFIG_NUM ];

__NOINIT_ATTR uint32_t stepcounter_valid;
__NOINIT_ATTR uint32_t stepcounter_before_reset;
__NOINIT_ATTR uint32_t stepcounter;

void IRAM_ATTR bma_irq( void );

/*
 *
 */
void bma_setup( TTGOClass *ttgo ) {

    bma_event_handle = xEventGroupCreate();

    for ( int i = 0 ; i < BMA_CONFIG_NUM ; i++ ) {
        bma_config[ i ].enable = true;
    }

    if ( stepcounter_valid != 0xa5a5a5a5 ) {
      stepcounter = 0;
      stepcounter_before_reset = 0;
      stepcounter_valid = 0xa5a5a5a5;
      log_i("stepcounter not valid. reset");
    }

    stepcounter = stepcounter + stepcounter_before_reset;

    bma_read_config();

    ttgo->bma->begin();
    ttgo->bma->attachInterrupt();
    ttgo->bma->direction();

    pinMode( BMA423_INT1, INPUT );
    attachInterrupt( BMA423_INT1, bma_irq, RISING );

    bma_reload_settings();
}

void bma_standby( void ) {
  TTGOClass *ttgo = TTGOClass::getWatch();

  log_i("go standby");

  if ( bma_get_config( BMA_STEPCOUNTER ) )
      ttgo->bma->enableStepCountInterrupt( false );

}

void bma_wakeup( void ) {
  TTGOClass *ttgo = TTGOClass::getWatch();

  log_i("go wakeup");

  if ( bma_get_config( BMA_STEPCOUNTER ) )
    ttgo->bma->enableStepCountInterrupt( true );

  stepcounter_before_reset = ttgo->bma->getCounter();
  statusbar_update_stepcounter( stepcounter + ttgo->bma->getCounter() );
}

/*
 *
 */
void bma_reload_settings( void ) {

    TTGOClass *ttgo = TTGOClass::getWatch();

    ttgo->bma->enableStepCountInterrupt( bma_config[ BMA_STEPCOUNTER ].enable );
    ttgo->bma->enableWakeupInterrupt( bma_config[ BMA_DOUBLECLICK ].enable );
}

/*
 *
 */
void IRAM_ATTR  bma_irq( void ) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xEventGroupSetBitsFromISR( bma_event_handle, BMA_EVENT_INT, &xHigherPriorityTaskWoken );

    if ( xHigherPriorityTaskWoken )
    {
        portYIELD_FROM_ISR ();
    }
}

/*
 * loop routine for handling IRQ in main loop
 */
void bma_loop( TTGOClass *ttgo ) {
    /*
     * handle IRQ event
     */
    if ( xEventGroupGetBitsFromISR( bma_event_handle ) & BMA_EVENT_INT ) {                
      while( !ttgo->bma->readInterrupt() );
        if ( ttgo->bma->isDoubleClick() ) {
            powermgm_set_event( POWERMGM_BMA_WAKEUP );
            xEventGroupClearBitsFromISR( bma_event_handle, BMA_EVENT_INT );
            return;
        }
    }

    if ( !powermgm_get_event( POWERMGM_STANDBY ) && xEventGroupGetBitsFromISR( bma_event_handle ) & BMA_EVENT_INT ) {
        stepcounter_before_reset = ttgo->bma->getCounter();
        statusbar_update_stepcounter( stepcounter + ttgo->bma->getCounter() );
        xEventGroupClearBitsFromISR( bma_event_handle, BMA_EVENT_INT );
    }
}

/*
 *
 */
void bma_save_config( void ) {
    if ( SPIFFS.exists( BMA_COFIG_FILE ) ) {
        SPIFFS.remove( BMA_COFIG_FILE );
        log_i("remove old binary bma config");
    }

    fs::File file = SPIFFS.open( BMA_JSON_COFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", BMA_JSON_COFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["stepcounter"] = bma_config[ BMA_STEPCOUNTER ].enable;
        doc["doubleclick"] = bma_config[ BMA_DOUBLECLICK ].enable;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

/*
 *
 */
void bma_read_config( void ) {
    if ( SPIFFS.exists( BMA_JSON_COFIG_FILE ) ) {        
        fs::File file = SPIFFS.open( BMA_JSON_COFIG_FILE, FILE_READ );
        if (!file) {
            log_e("Can't open file: %s!", BMA_JSON_COFIG_FILE );
        }
        else {
            int filesize = file.size();
            SpiRamJsonDocument doc( filesize * 2 );

            DeserializationError error = deserializeJson( doc, file );
            if ( error ) {
                log_e("update check deserializeJson() failed: %s", error.c_str() );
            }
            else {
                bma_config[ BMA_STEPCOUNTER ].enable = doc["stepcounter"].as<bool>();
                bma_config[ BMA_DOUBLECLICK ].enable = doc["doubleclick"].as<bool>();
            }        
            doc.clear();
        }
        file.close();
    }
    else {
        log_i("no json config exists, read from binary");
        fs::File file = SPIFFS.open( BMA_COFIG_FILE, FILE_READ );

        if (!file) {
            log_e("Can't open file: %s!", BMA_COFIG_FILE );
        }
        else {
            int filesize = file.size();
            if ( filesize > sizeof( bma_config ) ) {
                log_e("Failed to read configfile. Wrong filesize!" );
            }
            else {
                file.read( (uint8_t *)bma_config, filesize );
                file.close();
                bma_save_config();
                return; 
            }
        file.close();
        }
    }
}

/*
 *
 */
bool bma_get_config( int config ) {
    if ( config < BMA_CONFIG_NUM ) {
        return( bma_config[ config ].enable );
    }
    return false;
}

/*
 *
 */
void bma_set_config( int config, bool enable ) {
    if ( config < BMA_CONFIG_NUM ) {
        bma_config[ config ].enable = enable;
        bma_save_config();
        bma_reload_settings();
    }
}