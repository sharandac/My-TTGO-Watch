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
#include "config.h"
#include "http_ota.h"
#include "hardware/powermgm.h"
#include "hardware/callback.h"
#include "hardware/blectl.h"
#include "utils/decompress/decompress.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <HTTPClient.h>
    #include <Update.h>
#endif

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
    #ifndef NO_BLUETOOTH
        blectl_off();
    #endif
    powermgm_set_lightsleep( false );
    powermgm_set_perf_mode();
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
#ifdef NATIVE_64BIT

#else
    HTTPClient http;
    WiFiClientSecure *sslclient = NULL;
    http.setUserAgent( "ESP32-UPDATE-" __FIRMWARE__ );
    /**
     * setup http or https
     */
    if( strstr( url, "http://" ) ) {
        /**
         * start get firmware file
         */
        http.begin( url );
    }
    else if( strstr( url, "https://" ) ) {
        /**
         * setup ssl/tls layer
         */
        sslclient = new WiFiClientSecure;
        sslclient->setInsecure();
        /*
         * start get firmware file
         */
        http.begin( *sslclient, url );
    }
    else {
        log_e("url type not supported, only http:// or https://");
        return( false );
    }
    /**
     * start GET request
     */
    int httpCode = http.GET();
    /**
     * check http respone code
     */
    if ( httpCode > 0 && httpCode == HTTP_CODE_OK ) {
        /**
         * send http_ota_start event
         */
        http_ota_send_event_cb( HTTP_OTA_START, (void *)NULL );
        /**
         * start an unpacker instance, reister progress callback and put the stream in
         */
        if( !decompress_stream_into_flash( http.getStreamPtr(), md5, firmwaresize, http_ota_progress_cb ) ) {
            http_ota_send_event_cb( HTTP_OTA_ERROR, (void*)"error ... weak wifi?" );
        }
        else {
            http_ota_send_event_cb( HTTP_OTA_FINISH, (void*)"Flashing ... done!" );
            retval = true;
        }
    }
    else {
        http_ota_send_event_cb( HTTP_OTA_ERROR, (void*)"http error ... weak wifi?" );        
    }
    /**
     * terminate http client
     */
    http.end();
    /**
     * terminate ssl/tls layer if exist
     */
    if( sslclient ) {
        sslclient->stop();
    }
#endif
    return( retval );
}

bool http_ota_start_uncompressed( const char* url, const char* md5 ) {
    bool ret = true;                                    /** @brief return value */
#ifdef NATIVE_64BIT

#else
    float downloaded = 0;                               /** @brief downloaded firmware size in bytes*/
    int32_t total = 1;                                  /** @brief total firmware size in bytes*/
    int32_t written = 0;                                /** @brief written firmware data block in bytes*/
    int32_t len = 1;                                    /** @brief remaining firmware data in bytes*/
    size_t size = sizeof( HTTP_OTA_BUFFER_SIZE );
    uint8_t buff[ HTTP_OTA_BUFFER_SIZE ] = { 0 };       /** @brief firmware write buffer */

    HTTPClient http;
    WiFiClientSecure *sslclient = NULL;
    http.setUserAgent( "ESP32-UPDATE-" __FIRMWARE__ );
    /**
     * setup http or https
     */
    if( strstr( url, "http://" ) ) {
        /**
         * start get firmware file
         */
        http.begin( url );
    }
    else if( strstr( url, "https://" ) ) {
        /**
         * setup ssl/tls layer
         */
        sslclient = new WiFiClientSecure;
        sslclient->setInsecure();
        /*
         * start get firmware file
         */
        http.begin( *sslclient, url );
    }
    else {
        log_e("url type not supported, only http:// or https://");
        return( false );
    }
    /**
     * start GET request
     */
    int httpCode = http.GET();
    /**
     * check return code ok
     */
    if ( httpCode > 0 && httpCode == HTTP_CODE_OK ) {
        http_ota_send_event_cb( HTTP_OTA_START, (void *)NULL );
        /*
         * get the file lenght
         */
        len = http.getSize();
        total = len;
        downloaded = 0;
        /*
         * setup http stream and set nodelay option
         */
        WiFiClient * stream = http.getStreamPtr();
        stream->setNoDelay( true );
        /**
         * start flashing
         */
        if ( Update.begin( total, U_FLASH ) ) {
            /*
             * set md5 and reset downloaded counter
             */
            Update.setMD5( md5 );
            downloaded = 0;
            /**
             * check if update finish
             */
            while ( !Update.isFinished() ) {
                /**
                 * check for broken connection
                 */
                if( http.connected() && ( len > 0 ) ) {
                    /**
                     * get nxt chunk of bytes id available
                     */
                    size = stream->available();
                    if( size > 0 ) {
                        /*
                         * prepare write buffer
                         */
                        int c = stream->readBytes( buff, ( ( size > HTTP_OTA_BUFFER_SIZE ) ? HTTP_OTA_BUFFER_SIZE : size ) );
                        written = Update.write( buff, c );
                        /**
                         * if buffer written check if success and fire callback
                         */
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
    /**
     * terminate http client
     */
    http.end();
    /**
     * terminate ssl/tls layer if exist
     */
    if( sslclient ) {
        sslclient->stop();
    }
    /**
     * check if written bytes equal to downloaded bytes
     */
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
#endif
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