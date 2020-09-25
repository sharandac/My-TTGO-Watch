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

#include "display.h"
#include "powermgm.h"
#include "motor.h"
#include "bma.h"
#include "gui/gui.h"

#include "json_psram_allocator.h"

display_config_t display_config;
callback_t *display_callback = NULL;

static uint8_t dest_brightness = 0;
static uint8_t brightness = 0;

bool display_powermgm_event_cb( EventBits_t event, void *arg );
bool display_powermgm_loop_cb( EventBits_t event, void *arg );
bool display_send_event_cb( EventBits_t event, void *arg );

void display_setup( void ) {
    display_read_config();

    TTGOClass *ttgo = TTGOClass::getWatch();

    ttgo->openBL();
    ttgo->bl->adjust( 0 );
    ttgo->tft->setRotation( display_config.rotation / 90 );
    bma_set_rotate_tilt( display_config.rotation );

    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, display_powermgm_event_cb, "display" );
    powermgm_register_loop_cb( POWERMGM_WAKEUP, display_powermgm_loop_cb, "display loop" );
}

bool display_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_STANDBY:          display_standby();
                                        break;
        case POWERMGM_WAKEUP:           display_wakeup( false );
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   display_wakeup( true );
                                        break;
    }
    return( true );
}

bool display_powermgm_loop_cb( EventBits_t event, void *arg ) {
    display_loop();
    return( true );
}

void display_loop( void ) {
  TTGOClass *ttgo = TTGOClass::getWatch();

  if ( dest_brightness != brightness ) {
    if ( brightness < dest_brightness ) {
      brightness++;
      ttgo->bl->adjust( brightness );
    }
    else {
      brightness--;
      ttgo->bl->adjust( brightness );
    }
  }
  if ( display_get_timeout() != DISPLAY_MAX_TIMEOUT ) {
    if ( lv_disp_get_inactive_time(NULL) > ( ( display_get_timeout() * 1000 ) - display_get_brightness() * 8 ) ) {
        dest_brightness = ( ( display_get_timeout() * 1000 ) - lv_disp_get_inactive_time( NULL ) ) / 8 ;
    }
    else {
        dest_brightness = display_get_brightness();
    }
  }
}

bool display_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( display_callback == NULL ) {
        display_callback = callback_init( "display" );
        if ( display_callback == NULL ) {
            log_e("display_callback_callback alloc failed");
            while(true);
        }
    }
    return( callback_register( display_callback, event, callback_func, id ) );
}

bool display_send_event_cb( EventBits_t event, void *arg ) {
    return( callback_send( display_callback, event, arg ) );
}

void display_standby( void ) {
  TTGOClass *ttgo = TTGOClass::getWatch();
  log_i("go standby");
  ttgo->bl->adjust( 0 );
  ttgo->displaySleep();
  ttgo->closeBL();
  brightness = 0;
  dest_brightness = 0;
}

void display_wakeup( bool silence ) {
  TTGOClass *ttgo = TTGOClass::getWatch();

  // wakeup without display
  if ( silence ) {
    log_i("go silence wakeup");
    ttgo->openBL();
    ttgo->displayWakeup();
    ttgo->bl->adjust( 0 );
    brightness = 0;
    dest_brightness = 0;
  }
  // wakeup with display
  else {
    log_i("go wakeup");
    ttgo->openBL();
    ttgo->displayWakeup();
    ttgo->bl->adjust( 0 );
    brightness = 0;
    dest_brightness = display_get_brightness();
  }
}

void display_save_config( void ) {
    fs::File file = SPIFFS.open( DISPLAY_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", DISPLAY_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["brightness"] = display_config.brightness;
        doc["rotation"] = display_config.rotation;
        doc["timeout"] = display_config.timeout;
        doc["block_return_maintile"] = display_config.block_return_maintile;
        doc["background_image"] = display_config.background_image;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void display_read_config( void ) {
    fs::File file = SPIFFS.open( DISPLAY_JSON_CONFIG_FILE, FILE_READ );
    if (!file) {
        log_e("Can't open file: %s!", DISPLAY_JSON_CONFIG_FILE );
    }
    else {
        int filesize = file.size();
        SpiRamJsonDocument doc( filesize * 2 );

        DeserializationError error = deserializeJson( doc, file );
        if ( error ) {
            log_e("update check deserializeJson() failed: %s", error.c_str() );
        }
        else {
            display_config.brightness = doc["brightness"] | DISPLAY_MAX_BRIGHTNESS / 2;
            display_config.rotation = doc["rotation"] | DISPLAY_MIN_ROTATE;
            display_config.timeout = doc["timeout"] | DISPLAY_MIN_TIMEOUT;
            display_config.block_return_maintile = doc["block_return_maintile"] | false;
            display_config.background_image = doc["background_image"] | 2;
        }        
        doc.clear();
    }
    file.close();
}

uint32_t display_get_timeout( void ) {
    return( display_config.timeout );
}

void display_set_timeout( uint32_t timeout ) {
    display_config.timeout = timeout;
    display_send_event_cb( DISPLAYCTL_TIMEOUT, (void *)timeout );
}

uint32_t display_get_brightness( void ) {
    return( display_config.brightness );
}

void display_set_brightness( uint32_t brightness ) {
    display_config.brightness = brightness;
    dest_brightness = brightness;
    display_send_event_cb( DISPLAYCTL_BRIGHTNESS, (void *)brightness );
}

uint32_t display_get_rotation( void ) {
  return( display_config.rotation );
}

bool display_get_block_return_maintile( void ) {
  return( display_config.block_return_maintile );
}

void display_set_block_return_maintile( bool block_return_maintile ) {
  display_config.block_return_maintile = block_return_maintile;
}

void display_set_rotation( uint32_t rotation ) {
  TTGOClass *ttgo = TTGOClass::getWatch();
  display_config.rotation = rotation;
  ttgo->tft->setRotation( rotation / 90 );
  lv_obj_invalidate( lv_scr_act() );
}

uint32_t display_get_background_image( void ) {
    return( display_config.background_image );
}

void display_set_background_image( uint32_t background_image ) {
    display_config.background_image = background_image;
}
