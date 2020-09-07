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

#include "powermgm.h"

#include "sound.h"

// based on https://github.com/earlephilhower/ESP8266Audio
#include <SPIFFS.h>
#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourcePROGMEM.h"

#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioGeneratorWAV.h"
#include <AudioGeneratorMIDI.h>

#include "AudioOutputI2S.h"

AudioFileSourceSPIFFS *spliffs_file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

AudioGeneratorMP3 *mp3;
AudioGeneratorWAV *wav;
AudioFileSourcePROGMEM *progmem_file;

#include "json_psram_allocator.h"

bool sound_init = false;

sound_config_t sound_config;

static uint8_t dest_brightness = 0;
static uint8_t brightness = 0;

void sound_setup( void ) {
    if ( sound_init == true )
        return;
    sound_read_config();

    TTGOClass *ttgo = TTGOClass::getWatch();


    //!Turn on the audio power
    ttgo->enableLDO3();

    //out->SetPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    out = new AudioOutputI2S();
    out->SetPinout(TWATCH_DAC_IIS_BCK, TWATCH_DAC_IIS_WS, TWATCH_DAC_IIS_DOUT);
    mp3 = new AudioGeneratorMP3();
    wav = new AudioGeneratorWAV();

    sound_init = true;
}

void sound_loop( void ) {
    if (sound_config.enable ) {
        if ( mp3->isRunning() && !mp3->loop() ) mp3->stop();
        if ( wav->isRunning() && !wav->loop() ) wav->stop();
    }
}

void sound_play_spiffs_mp3( const char *filename ) {
    if ( sound_config.enable ) {
        log_i("playing file %s from SPIFFS", filename);
        spliffs_file = new AudioFileSourceSPIFFS(filename);
        id3 = new AudioFileSourceID3(spliffs_file);
        mp3->begin(id3, out);
    }
}

void sound_play_progmem_wav( const void *data, uint32_t len ) {
    if ( sound_config.enable ) {
        log_i("playing audio (size %d) from PROGMEM ", len );
        progmem_file = new AudioFileSourcePROGMEM( data, len );
        wav->begin(progmem_file, out);
    }
}

void sound_save_config( void ) {
    if ( SPIFFS.exists( SOUND_CONFIG_FILE ) ) {
        SPIFFS.remove( SOUND_CONFIG_FILE );
        log_i("remove old binary sound config");
    }

    fs::File file = SPIFFS.open( SOUND_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", SOUND_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["enable"] = sound_config.enable;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

void sound_read_config( void ) {
    if ( SPIFFS.exists( SOUND_JSON_CONFIG_FILE ) ) {        
        fs::File file = SPIFFS.open( SOUND_JSON_CONFIG_FILE, FILE_READ );
        if (!file) {
            log_e("Can't open file: %s!", SOUND_JSON_CONFIG_FILE );
        }
        else {
            int filesize = file.size();
            SpiRamJsonDocument doc( filesize * 2 );

            DeserializationError error = deserializeJson( doc, file );
            if ( error ) {
                log_e("update check deserializeJson() failed: %s", error.c_str() );
            }
            else {
                sound_config.enable = doc["enable"].as<bool>();
            }        
            doc.clear();
        }
        file.close();
    }
    else {
        log_i("no json config exists, read from binary");
        fs::File file = SPIFFS.open( SOUND_CONFIG_FILE, FILE_READ );

        if (!file) {
            log_e("Can't open file: %s!", SOUND_CONFIG_FILE );
        }
        else {
            int filesize = file.size();
            if ( filesize > sizeof( sound_config ) ) {
                log_e("Failed to read configfile. Wrong filesize!" );
            }
            else {
                file.read( (uint8_t *)&sound_config, filesize );
                file.close();
                sound_save_config();
                return; 
            }
        file.close();
        }
    }
}
