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
#include <HTTPClient.h>
#include <Update.h>

#include "callback.h"
#include "http_ota.h"

callback_t *http_ota_callback = NULL;
bool http_ota_send_event_cb( EventBits_t event, void *arg );

bool http_ota_start( const char* url, const char* md5 ) {
    int downloaded = 0;
    int written = 0;
    int total = 1;
    int len = 1;
    uint8_t buff[ 1024 * 2 ] = { 0 };
    size_t size = sizeof( buff );
    bool ret = true;

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

        if ( Update.begin( total, U_FLASH ) ) {
            Update.setMD5( md5 );
            downloaded = 0;
            while ( !Update.isFinished() ) {
                if( http.connected() && ( len > 0 ) ) {
                    size = stream->available();
                    if( size > 0 ) {
                        int c = stream->readBytes( buff, ( ( size > sizeof( buff ) ) ? sizeof( buff ) : size ) );
                        written = Update.write( buff, c );
                        if ( written > 0 ) {
                            if( written != c ) {
                                http_ota_send_event_cb( HTTP_OTA_ERROR, (void*)"Flashing chunk not full ... warning!" );
                                log_w("Flashing chunk not full ... warning!");
                            }
                            downloaded += written;
                            static int16_t old_progress = 0;
                            int16_t progress = ( 100 * downloaded ) / total ;
                            if ( old_progress != progress ) {
                                http_ota_send_event_cb( HTTP_OTA_PROGRESS, (void*)&progress );
                                log_i("progress: %d", progress );
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
                    delay( 1 );
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