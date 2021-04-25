/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#include <HTTPClient.h>
#include <SPIFFS.h>

#include "http_download.h"
#include "utils/alloc.h"

void http_download_get_filename_from_url( http_download_dsc_t *http_download, const char *url );

http_download_dsc_t *http_download_to_ram( const char *url ) {
    /**
     * alloc http_download_dsc structure
     */
    http_download_dsc_t *http_download_dsc = (http_download_dsc_t *)CALLOC( sizeof( http_download_dsc_t ), 1 );
    log_d("http_download_dsc: alloc %d bytes at %p-%p", sizeof( http_download_dsc_t), http_download_dsc, http_download_dsc + sizeof( http_download_dsc_t) );
    /**
     * check if alloc was failed
     */
    if ( http_download_dsc ) {
        /**
         * set filename in the http_download_dsc
         */
        http_download_get_filename_from_url( http_download_dsc, url );
        /**
         * open http connection
         */
        HTTPClient download_client;                     /** @brief http download client */
        download_client.begin( url );
        int httpCode = download_client.GET();
        /**
         * request successfull?
         */
        if ( httpCode > 0 && httpCode == HTTP_CODE_OK  ) {
            /**
             * get file size and alloc memory for the file
             */
            http_download_dsc->size = download_client.getSize();
            http_download_dsc->data = (uint8_t*)CALLOC( http_download_dsc->size , 1 );
            log_d("http_download_dsc->data: alloc %d bytes at %p-%p", http_download_dsc->size, http_download_dsc->data, http_download_dsc->data + http_download_dsc->size );
            /**
             * check if alloc success
             */
            if ( http_download_dsc->data ) {
                /**
                 * setup data write counter/pointer/buffer and data stream
                 */
                uint32_t bytes_left = http_download_dsc->size;                          /** @brief download left byte counter */
                uint8_t *data_write_p = http_download_dsc->data;                        /** @brief write pointer for the raw file download */

                WiFiClient *download_stream = download_client.getStreamPtr();
                /**
                 * get download data
                 */
                while( download_client.connected() && ( bytes_left > 0 ) ) {
                    /**
                     * get bytes in buffer and store them
                     */
                    size_t size = download_stream->available();
                    if ( size > 0 ) {
                        size_t c = download_stream->readBytes( data_write_p, size < bytes_left ? size : bytes_left );
                        bytes_left -= c;
                        data_write_p = data_write_p + c;
                    }
                }
                if ( bytes_left != 0 ) {
                    log_e("download failed");
                    download_client.end();
                    http_download_free_all( http_download_dsc );
                    return( NULL );
                }
            }
            else {
                log_e("data alloc failed");
                download_client.end();
                http_download_free_all( http_download_dsc );
                return( NULL );
            }
        }
        else {
            log_e("http connection abort");
            download_client.end();
            http_download_free_all( http_download_dsc );
            return( NULL );
        }
        /**
         * close http connection
         */
        download_client.end();
    }
    else {
        log_e("http_download_dsc alloc failed");
    }
    return( http_download_dsc );
}

void http_download_free_all( http_download_dsc_t *http_download ) {
    if ( http_download ) {
        /**
         * free name momory
         */
        http_download_free_name( http_download );
        /**
         * free data momory
         */
        http_download_free_data( http_download );
        /**
         * free http_download_dsc memory
         */
        http_download_free_dsc( http_download );
    }
}

void http_download_free_name( http_download_dsc_t *http_download ) {
    if ( http_download->filename ) {
        log_d("free: %p", http_download->filename );
        free( http_download->filename );
        http_download->filename = NULL;
    }
}

void http_download_free_data( http_download_dsc_t *http_download ) {
    if ( http_download->data ) {
        log_d("free: %p", http_download->data );
        free( http_download->data );
        http_download->data = NULL;
    }
}

void http_download_free_dsc( http_download_dsc_t *http_download ) {
    if ( http_download ) {
        /**
         * free http_download_dsc memory
         */
        log_d("free: %p", http_download );
        free( http_download );
    }
}

void http_download_free_without_data( http_download_dsc_t *http_download ) {
    if ( http_download ) {
        /**
         * free name momory
         */
        http_download_free_name( http_download );
        /**
         * free http_download_dsc memory
         */
        http_download_free_dsc( http_download );
    }
}

void http_download_get_filename_from_url( http_download_dsc_t *http_download, const char *url ) {
    const char *filename_from_url = NULL;
    const char *tld = NULL;
    const char index[] = "index.html";
    /**
     * check for http or https url
     */
    if ( http_download ) {
        if ( strstr( url, "http://" ) || strstr( url, "https://" ) ) {
            tld = strstr( url, "://" ) + 3;
            /**
             * reverse search for the lash
             */
            filename_from_url = (const char*)strrchr( tld, '/' );
            /**
             * if not lash found, set index.html as filename
             */
            if ( !filename_from_url ) {
                filename_from_url = index;
            }
            /**
             * alloc filename momory
             */
            http_download->filename = (char *)CALLOC( strlen( filename_from_url ) + 2 , 1 );
            if ( http_download->filename ) {
                /**
                 * copy filename into alloc mamory
                 */
                log_d("http_download_dsc->filename: alloc %d bytes at %p-%p", strlen( filename_from_url ), http_download->filename, http_download->filename + strlen( filename_from_url ) );
                strncpy( http_download->filename, filename_from_url, strlen( filename_from_url ) );
            }
            else {
                log_e("http_download->filename: alloc failed");
            }
        }
    }
}