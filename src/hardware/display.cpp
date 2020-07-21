#include "config.h"
#include <TTGO.h>

#include "display.h"

display_config_t display_config;

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
}

/*
 *
 */
void display_save_config( void ) {
  fs::File file = SPIFFS.open( DISPLAY_CONFIG_FILE, FILE_WRITE );

  if ( !file ) {
    Serial.printf("Can't save file: %s\r\n", DISPLAY_CONFIG_FILE );
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
    Serial.printf("Can't open file: %s!\r\n", DISPLAY_CONFIG_FILE );
  }
  else {
    int filesize = file.size();
    if ( filesize > sizeof( display_config ) ) {
      Serial.printf("Failed to read configfile. Wrong filesize!\r\n" );
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
    TTGOClass *ttgo = TTGOClass::getWatch();

    display_config.brightness = brightness;
    ttgo->bl->adjust( brightness );
}

uint32_t display_get_rotation( void ) {
  TTGOClass *ttgo = TTGOClass::getWatch();
  return( display_config.rotation );
}

void display_set_rotation( uint32_t rotation ) {
  TTGOClass *ttgo = TTGOClass::getWatch();
  display_config.rotation = rotation;
  ttgo->tft->setRotation( rotation / 90 );
  lv_obj_invalidate( lv_scr_act() );
}
