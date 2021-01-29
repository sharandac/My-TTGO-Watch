/****************************************************************************
 *   Sep 21 12:13:51 2020
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

/*
 *  inspire by https://github.com/nhatuan84/esp32-http-firmware-update-over-the-air
 *
 */
#include "config.h"
#include "hardware/ESP32-targz/ESP32-targz.h"
#include <HTTPClient.h>
#include <Update.h>

#include "callback.h"
#include "http_ota.h"
#include "alloc.h"
#include "powermgm.h"
#include "pmu.h"
#include "blectl.h"

callback_t *http_ota_callback = NULL;
bool http_ota_start_compressed( const char* url, const char* md5, int32_t firmwaresize );
bool http_ota_start_uncompressed( const char* url, const char* md5 );
bool http_ota_send_event_cb( EventBits_t event, void *arg );

void http_ota_progress_cb( uint8_t progress ) {
    float tmp_progress = progress;
    http_ota_send_event_cb( HTTP_OTA_PROGRESS, (void*)&tmp_progress );
}

bool http_ota_start( const char* url, const char* md5, int32_t firmwaresize ) {
    bool retval = false;
    /*
     * disable ble and set esp32 voltage to 3.3V to
     * prevent some issues
     */
    blectl_off();
    pmu_set_safe_voltage_for_update();
    /*
     * if firmware an .gz file, take compressed ota otherwise
     * take a normal uncompressed firmware
     */
    if ( strstr( url, ".gz") ) {
        http_ota_send_event_cb( HTTP_OTA_START, (void*)"get compressed firmware ..." );
        retval = http_ota_start_compressed( url, md5, firmwaresize );
    }
    else {
        http_ota_send_event_cb( HTTP_OTA_START, (void*)"get uncompressed firmware ..." );
        retval = http_ota_start_uncompressed( url, md5 );
    }

    return( retval );
}

bool http_ota_start_compressed( const char* url, const char* md5, int32_t firmwaresize ) {
    bool retval = false;
    int32_t size = UPDATE_SIZE_UNKNOWN;

    HTTPClient http;

    /*
     * start get firmware file
     */
    http.setUserAgent( "ESP32-UPDATE-" __FIRMWARE__ );
    http.begin( url );
    int httpCode = http.GET();

    if ( httpCode > 0 && httpCode == HTTP_CODE_OK ) {
        /*
         * send http_ota_start event
         */
        http_ota_send_event_cb( HTTP_OTA_START, (void *)NULL );
        /*
         * start an unpacker instance, reister progress callback and put the stream in
         */
        GzUnpacker *GZUnpacker = new GzUnpacker();

        GZUnpacker->setGzProgressCallback( http_ota_progress_cb );

        if ( firmwaresize != 0 )
            size = firmwaresize;

        if( !GZUnpacker->gzStreamUpdater( http.getStreamPtr(), size, 0, false ) ) {
            log_e("gzStreamUpdater failed with return code #%d\n", GZUnpacker->tarGzGetError() );
            http_ota_send_event_cb( HTTP_OTA_ERROR, (void*)"Flashing ... failed!" );
        }
        else {
            http_ota_send_event_cb( HTTP_OTA_FINISH, (void*)"Flashing ... done!" );
            retval = true;
        }
    }
    http.end();

    return( retval );
}

bool http_ota_start_uncompressed( const char* url, const char* md5 ) {
    float downloaded = 0;                               /** @brief downloaded firmware size in bytes*/
    int32_t total = 1;                                  /** @brief total firmware size in bytes*/
    int32_t written = 0;                                /** @brief written firmware data block in bytes*/
    int32_t len = 1;                                    /** @brief remaining firmware data in bytes*/
    size_t size = sizeof( HTTP_OTA_BUFFER_SIZE );
    bool ret = true;                                    /** @brief return value */
    uint8_t buff[ HTTP_OTA_BUFFER_SIZE ] = { 0 };       /** @brief firmware write buffer */

    HTTPClient http;

    http.setUserAgent( "ESP32-UPDATE-" __FIRMWARE__ );
    http.begin( url );
    int httpCode = http.GET();

    if ( httpCode > 0 && httpCode == HTTP_CODE_OK ) {
        http_ota_send_event_cb( HTTP_OTA_START, (void *)NULL );

        len = http.getSize();
        total = len;
        downloaded = 0;

        WiFiClient * stream = http.getStreamPtr();
        stream->setNoDelay( true );

        if ( Update.begin( total, U_FLASH ) ) {
            Update.setMD5( md5 );
            downloaded = 0;
            while ( !Update.isFinished() ) {
                if( http.connected() && ( len > 0 ) ) {
                    size = stream->available();
                    if( size > 0 ) {
                        int c = stream->readBytes( buff, ( ( size > HTTP_OTA_BUFFER_SIZE ) ? HTTP_OTA_BUFFER_SIZE : size ) );
                        written = Update.write( buff, c );
                        if ( written > 0 ) {
                            if( written != c ) {
                                http_ota_send_event_cb( HTTP_OTA_ERROR, (void*)"Flashing chunk not full ... warning!" );
                                log_w("Flashing chunk not full ... warning!");
                            }
                            downloaded += written;
                            static float old_progress = -1;
                            float progress = ( 100 * downloaded ) / total ;
                            if ( old_progress != progress ) {
                                http_ota_send_event_cb( HTTP_OTA_PROGRESS, (void*)&progress );
                                old_progress = progress;
                            }
                        } else {
                            http_ota_send_event_cb( HTTP_OTA_ERROR, (void*)"Flashing ... failed!" );
                            log_e("Flashing ... failed!");
                            ret = false;
                            break;
                        }
                        if(len > 0) {
                            len -= c;
                        }
                    }
                }
            }
        }
        else {
            http_ota_send_event_cb( HTTP_OTA_ERROR, (void*)"Flashing init ... failed!" );
            log_e("Flashing init ... failed!");
        }
        http_ota_send_event_cb( HTTP_OTA_FINISH, (void*)NULL );
    }
    else {
        http_ota_send_event_cb( HTTP_OTA_ERROR, (void*)"[HTTP] GET... failed!" );
        log_e("[HTTP] GET... failed!");
        ret = false;        
    }
    http.end();

    if( downloaded == total && len == 0 ) {
        if( Update.end() ) {
            http_ota_send_event_cb( HTTP_OTA_FINISH, (void*)"Flashing ... done!" );
            log_i("Flashing ... done!");
        } else {
            http_ota_send_event_cb( HTTP_OTA_ERROR, (void*)"Flashing md5 ... failed!" );
            log_e("Flashing md5 ... failed!");
            ret = false;
        }
    } else {
        http_ota_send_event_cb( HTTP_OTA_ERROR, (void*)"Download firmware ... failed!" );
        log_e("Download firmware ... failed!");
        ret = false;
    }

    return( ret );
}

bool http_ota_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( http_ota_callback == NULL ) {
        http_ota_callback = callback_init( "http ota" );
        if ( http_ota_callback == NULL ) {
            log_e("http http_ota_callback alloc failed");
            while(true);
        }
    }
    return( callback_register( http_ota_callback, event, callback_func, id ) );
}

bool http_ota_send_event_cb( EventBits_t event, void *arg ) {
    return( callback_send_no_log( http_ota_callback, event, arg ) );
}