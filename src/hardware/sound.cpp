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

#include "powermgm.h"
#include "sound.h"
#include "timesync.h"
#include "callback.h"
#include "hardware/config/soundconfig.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <SPIFFS.h>
    /*
    * based on https://github.com/earlephilhower/ESP8266Audio
    */
    #if defined( M5PAPER )
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
        #include "TTGO.h"

        #include "AudioFileSourceSPIFFS.h"
        #include "AudioFileSourcePROGMEM.h"
        #include "AudioFileSourceID3.h"
        #include "AudioGeneratorMP3.h"
        #include "AudioGeneratorWAV.h"
        #include <AudioGeneratorMIDI.h>
        #include "AudioOutputI2S.h"
        #include <ESP8266SAM.h>

        AudioFileSourceSPIFFS *spliffs_file;
        AudioOutputI2S *out;
        AudioFileSourceID3 *id3;

        AudioGeneratorMP3 *mp3;
        AudioGeneratorWAV *wav;
        ESP8266SAM *sam;
        AudioFileSourcePROGMEM *progmem_file;
    #elif defined( LILYGO_WATCH_2020_V2 )
    #elif defined( LILYGO_WATCH_2021 )    
    #else
        #warning "no hardware driver for sound"
    #endif
#endif

bool sound_init = false;
bool is_speaking = false;

sound_config_t sound_config;

callback_t *sound_callback = NULL;

bool sound_powermgm_event_cb( EventBits_t event, void *arg );
bool sound_powermgm_loop_cb( EventBits_t event, void *arg );
bool sound_send_event_cb( EventBits_t event, void*arg );
bool sound_is_silenced( void );

void sound_setup( void ) {
    if ( sound_init )
        return;

    /*
     * read config from SPIFFS
     */
    sound_config.load();
    /*
     * config sound driver and interface
     */
    #ifdef NATIVE_64BIT

    #else
        #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
            /*
            * set sound chip voltage on V1
            */
            #if defined( LILYGO_WATCH_2020_V1 )
                    TTGOClass *ttgo = TTGOClass::getWatch();
                    ttgo->power->setLDO3Mode( AXP202_LDO3_MODE_DCIN );
                    ttgo->power->setLDO3Voltage( 3300 );
            #endif
            /**
             * set sound driver
             */
            out = new AudioOutputI2S();
            out->SetPinout( TWATCH_DAC_IIS_BCK, TWATCH_DAC_IIS_WS, TWATCH_DAC_IIS_DOUT );
            sound_set_volume_config( sound_config.volume );
            mp3 = new AudioGeneratorMP3();
            wav = new AudioGeneratorWAV();
            sam = new ESP8266SAM;
            sam->SetVoice(sam->VOICE_SAM);
            /*
            * register all powermgm callback functions
            */
            powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, sound_powermgm_event_cb, "powermgm sound" );
            powermgm_register_loop_cb( POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP | POWERMGM_WAKEUP, sound_powermgm_loop_cb, "powermgm sound loop" );
            sound_set_enabled( sound_config.enable );

            sound_send_event_cb( SOUNDCTL_ENABLED, (void *)&sound_config.enable );
            sound_send_event_cb( SOUNDCTL_VOLUME, (void *)&sound_config.volume );

            sound_init = true;
        #else
            sound_set_enabled( false );
            sound_init = false;
        #endif
    #endif
}

bool sound_get_available( void ) {
    bool retval = false;

    #ifdef NATIVE_64BIT
    #else
        #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
            retval = true;
        #endif
    #endif

   return( retval );
}

bool sound_powermgm_event_cb( EventBits_t event, void *arg ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return( true );
    }

    switch( event ) {
        case POWERMGM_STANDBY:          sound_set_enabled( false );
                                        log_i("go standby");
                                        break;
        case POWERMGM_WAKEUP:           sound_set_enabled( sound_config.enable );
                                        log_i("go wakeup");
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   sound_set_enabled( sound_config.enable );
                                        log_i("go wakeup");
                                        break;
    }
    return( true );
}

bool sound_powermgm_loop_cb( EventBits_t event, void *arg ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return( true );
    }
#ifdef NATIVE_64BIT

#else
    #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
        if ( sound_config.enable && sound_init ) {
            // we call sound_set_enabled(false) to ensure the PMU stops all power
            if ( mp3->isRunning() && !mp3->loop() ) {
                log_i("stop playing mp3 sound");
                mp3->stop();
            }
            if ( wav->isRunning() && !wav->loop() ) {
                log_i("stop playing wav sound");
                wav->stop(); 
            }
        }
    #endif
#endif
    return( true );
}

bool sound_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return( true );
    }

    /*
     * check if an callback table exist, if not allocate a callback table
     */
    if ( sound_callback == NULL ) {
        sound_callback = callback_init( "sound" );
        if ( sound_callback == NULL ) {
            log_e("sound callback alloc failed");
            while(true);
        }
    }
    /*
     * register an callback entry and return them
     */
    return( callback_register( sound_callback, event, callback_func, id ) );
}

bool sound_send_event_cb( EventBits_t event, void *arg ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return( true );
    }
    /*
     * call all callbacks with her event mask
     */
    return( callback_send( sound_callback, event, arg ) );
}

/**
 * @brief enable or disable the power output for AXP202_LDO3 or AXP202_LDO4
 * depending on the current value of: sound_config.enable
 */
void sound_set_enabled( bool enabled ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return;
    }
#ifdef NATIVE_64BIT

#else
    #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
        if ( enabled ) {
            /**
             * ttgo->enableAudio() is not working
             */
            #if     defined( LILYGO_WATCH_2020_V1 )
                    TTGOClass *ttgo = TTGOClass::getWatch();
                    ttgo->power->setPowerOutPut( AXP202_LDO3, AXP202_ON );
            #elif   defined( LILYGO_WATCH_2020_V3 )
                    TTGOClass *ttgo = TTGOClass::getWatch();
                    ttgo->power->setPowerOutPut( AXP202_LDO4, AXP202_ON );
            #endif
            delay( 50 );
        }
        else {
            if ( sound_init ) {
                if ( mp3->isRunning() ) mp3->stop();
                if ( wav->isRunning() ) wav->stop();
            }
            /**
             * ttgo->disableAudio() is not working
             */
            #if     defined( LILYGO_WATCH_2020_V1 )
                    TTGOClass *ttgo = TTGOClass::getWatch();
                    ttgo->power->setPowerOutPut( AXP202_LDO3, AXP202_OFF );
            #elif   defined( LILYGO_WATCH_2020_V3 )
                    TTGOClass *ttgo = TTGOClass::getWatch();
                    ttgo->power->setPowerOutPut( AXP202_LDO4, AXP202_OFF );
            #endif
        }
    #endif
#endif
}

void sound_play_spiffs_mp3( const char *filename ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return;
    }
#ifdef NATIVE_64BIT

#else
    #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
        if ( sound_config.enable && sound_init && !sound_is_silenced() ) {
            sound_set_enabled( sound_config.enable );
            log_i("playing file %s from SPIFFS", filename);
            spliffs_file = new AudioFileSourceSPIFFS(filename);
            id3 = new AudioFileSourceID3(spliffs_file);
            mp3->begin(id3, out);
        } else {
            log_i("Cannot play mp3, sound is disabled");
        }
    #endif
#endif
}

void sound_play_progmem_wav( const void *data, uint32_t len ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return;
    }
#ifdef NATIVE_64BIT

#else
    #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
        if ( sound_config.enable && sound_init && !sound_is_silenced() ) {
            sound_set_enabled( sound_config.enable );
            log_i("playing audio (size %d) from PROGMEM ", len );
            progmem_file = new AudioFileSourcePROGMEM( data, len );
            wav->begin(progmem_file, out);
        } else {
            log_i("Cannot play wav, sound is disabled");
        }
    #endif
#endif
}

void sound_speak( const char *str ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return;
    }
#ifdef NATIVE_64BIT

#else
    #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
        if ( sound_config.enable && sound_init && !sound_is_silenced() ) {
            sound_set_enabled( sound_config.enable );
            log_i("Speaking text", str);
            is_speaking = true;
            sam->Say(out, str);
            is_speaking = false;
        }
        else {
            log_i("Cannot speak, sound is disabled");
        }
    #endif
#endif
}

void sound_save_config( void ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return;
    }

    sound_config.save();
}

void sound_read_config( void ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return;
    }

    sound_config.load();
}

bool sound_get_enabled_config( void ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return( false );
    }

    return sound_config.enable;
}

void sound_set_enabled_config( bool enable ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return;
    }

    sound_config.enable = enable;
    if ( sound_config.enable) {
        sound_set_enabled( true );
    }
    else {
        sound_set_enabled( false );
    }
    sound_send_event_cb( SOUNDCTL_ENABLED, (void *)&sound_config.enable ); 
}

uint8_t sound_get_volume_config( void ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return( 0 );
    }

    return( sound_config.volume );
}

void sound_set_volume_config( uint8_t volume ) {
    /**
     * check if sound available
     */
    if( !sound_get_available() ) {
        return;
    }

    sound_config.volume = volume;
        
#ifdef NATIVE_64BIT

#else
    #if defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V3 )
        if ( sound_config.enable && sound_init ) {
            log_i("Setting sound volume to: %d", volume);
            // limiting max gain to 3.5 (max gain is 4.0)
            out->SetGain(3.5f * ( sound_config.volume / 100.0f ));
        }
    #endif
#endif
    sound_send_event_cb( SOUNDCTL_VOLUME, (void *)&sound_config.volume ); 
}


bool sound_is_silenced( void ) {
    if ( !sound_config.silence_timeframe ) {
        log_i("no silence sound timeframe");
        return( false );
    }

    struct tm start;
    struct tm end;
    start.tm_hour = sound_config.silence_start_hour;
    start.tm_min = sound_config.silence_start_minute;
    end.tm_hour = sound_config.silence_end_hour;
    end.tm_min = sound_config.silence_end_minute;

    return timesync_is_between( start, end );
}