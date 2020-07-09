#include "config.h"
#include <TTGO.h>

#include "display.h"

display_config_t display_config;

void display_read_config( void );

/*
 *
 */
void display_setup( TTGOClass *ttgo ) {
    display_read_config();

    ttgo->openBL();
    ttgo->bl->adjust( 0 );
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
  fs::File file = SPIFFS.open( DISPLAY_COFIG_FILE, FILE_WRITE );

  if ( !file ) {
    Serial.printf("Can't save file: %s\r\n", DISPLAY_COFIG_FILE );
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
  fs::File file = SPIFFS.open( DISPLAY_COFIG_FILE, FILE_READ );

  if (!file) {
    Serial.printf("Can't open file: %s!\r\n", DISPLAY_COFIG_FILE );
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