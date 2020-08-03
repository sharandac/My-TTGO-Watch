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

display_config_t display_config;

static uint8_t dest_brightness = 0;
static uint8_t brightness = 0;
/*
 *
 */
void display_setup( TTGOClass *ttgo ) {
    display_read_config();

    ttgo->openBL();
    ttgo->bl->adjust( 0 );
    ttgo->tft->setRotation( display_config.rotation / 90 );
}

/*
 * loop routine for handling IRQ in main loop
 */
void display_loop( TTGOClass *ttgo ) {
  if ( !powermgm_get_event( POWERMGM_STANDBY ) && !powermgm_get_event( POWERMGM_SILENCE_WAKEUP )) {
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
}

void display_standby( void ) {
  TTGOClass *ttgo = TTGOClass::getWatch();

  ttgo->bl->adjust( 0 );
  ttgo->displaySleep();
  ttgo->closeBL();
  brightness = 0;
  dest_brightness = 0;
}

void display_wakeup( void ) {
  TTGOClass *ttgo = TTGOClass::getWatch();

  // normal wake up from standby
  if ( powermgm_get_event( POWERMGM_PMU_BUTTON | POWERMGM_PMU_BATTERY | POWERMGM_BMA_WAKEUP ) ) {
    log_i("wakeup");
    ttgo->openBL();
    ttgo->displayWakeup();
    ttgo->bl->adjust( 0 );
    brightness = 0;
    dest_brightness = display_get_brightness();
    motor_vibe( 1 );
  }
  // silence wakeup request from standby
  else if ( powermgm_get_event( POWERMGM_SILENCE_WAKEUP_REQUEST ) ) {
    log_i("silence wakeup");
    ttgo->openBL();
    ttgo->displayWakeup();
    ttgo->bl->adjust( 0 );
    brightness = 0;
    dest_brightness = 0;
    powermgm_set_event( POWERMGM_SILENCE_WAKEUP );
  }
}

/*
 *
 */
void display_save_config( void ) {
  fs::File file = SPIFFS.open( DISPLAY_CONFIG_FILE, FILE_WRITE );

  if ( !file ) {
    log_e("Can't save file: %s", DISPLAY_CONFIG_FILE );
  }
  else {
    file.write( (uint8_t *)&display_config, sizeof( display_config ) );
    file.close();
  }
}

/*
 *
 */
void display_read_config( void ) {
  fs::File file = SPIFFS.open( DISPLAY_CONFIG_FILE, FILE_READ );

  if (!file) {
    log_e("Can't open file: %s!", DISPLAY_CONFIG_FILE );
  }
  else {
    int filesize = file.size();
    if ( filesize > sizeof( display_config ) ) {
      log_e("Failed to read configfile. Wrong filesize!" );
    }
    else {
      file.read( (uint8_t *)&display_config, filesize );
    }
    file.close();
  }
}

uint32_t display_get_timeout( void ) {
    return( display_config.timeout );
}

void display_set_timeout( uint32_t timeout ) {
    display_config.timeout = timeout;
}

uint32_t display_get_brightness( void ) {
    return( display_config.brightness );
}

void display_set_brightness( uint32_t brightness ) {
    display_config.brightness = brightness;
    dest_brightness = brightness;
}

uint32_t display_get_rotation( void ) {
  return( display_config.rotation );
}

void display_set_rotation( uint32_t rotation ) {
  TTGOClass *ttgo = TTGOClass::getWatch();
  display_config.rotation = rotation;
  ttgo->tft->setRotation( rotation / 90 );
  lv_obj_invalidate( lv_scr_act() );
}
