/****************************************************************************
 *   July 28 00:23:05 2020
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
#include "update_check_version.h"
#include "utils/json_psram_allocator.h"
#include "utils/alloc.h"
#include "utils/uri_load/uri_load.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#endif

char *firmwarehost = NULL;
char *firmwarefile = NULL;
char *firmwareurl = NULL;
char *firmwaremd5 = NULL;
char *firmwarecomment = NULL;
int64_t firmwareversion = -1;
int32_t firmwaresize = 0;

int64_t update_check_new_version( char *url ) {
    int httpcode = -1;
    /**
     * load uri file into ram
     */
    uri_load_dsc_t *uri_load_dsc = uri_load_to_ram( url );
    log_i("load update information from: %s", url );
    /**
     * if was success, pharse the json
     */
    if ( uri_load_dsc ) {
        SpiRamJsonDocument doc( uri_load_dsc->size * 4 );

        DeserializationError error = deserializeJson( doc, uri_load_dsc->data );
        if (error) {
            log_e("weather today deserializeJson() failed: %s", error.c_str() );
            doc.clear();
            uri_load_free_all( uri_load_dsc );
            return( httpcode );
        }

        if ( doc["host"] ) {
            if ( firmwarehost == NULL ) {
                firmwarehost = (char*)CALLOC( strlen( doc["host"] ) + 1, 1 );
                if ( firmwarehost == NULL ) {
                    log_e("calloc error");
                    while(true);
                }
            }
            else {
                char * tmp_firmwarehost = (char*)REALLOC( firmwarehost, strlen( doc["host"] ) + 1 );
                if ( tmp_firmwarehost == NULL ) {
                    log_e("realloc error");
                    while(true);
                }
                firmwarehost = tmp_firmwarehost;
            }
            strcpy( firmwarehost, doc["host"] );
            log_d("firmwarehost: %s", firmwarehost );
        }

        if ( doc["file"] ) {
            if ( firmwarefile == NULL ) {
                firmwarefile = (char*)CALLOC( strlen( doc["file"] ) + 1, 1 );
                if ( firmwarefile == NULL ) {
                    log_e("calloc error");
                    while(true);
                }
            }
            else {
                char * tmp_firmwarefile = (char*)REALLOC( firmwarefile, strlen( doc["file"] ) + 1 );
                if ( tmp_firmwarefile == NULL ) {
                    log_e("realloc error");
                    while(true);
                }
                firmwarefile = tmp_firmwarefile;
            }
            strcpy( firmwarefile, doc["file"] );
            log_d("firmwarefile: %s", firmwarefile );
        }

        if ( doc["gzipfile"] ) {
            if ( firmwarefile == NULL ) {
                firmwarefile = (char*)CALLOC( strlen( doc["gzipfile"] ) + 1, 1 );
                if ( firmwarefile == NULL ) {
                    log_e("calloc error");
                    while(true);
                }
            }
            else {
                char * tmp_firmwarefile = (char*)REALLOC( firmwarefile, strlen( doc["gzipfile"] ) + 1 );
                if ( tmp_firmwarefile == NULL ) {
                    log_e("realloc error");
                    while(true);
                }
                firmwarefile = tmp_firmwarefile;
            }
            strcpy( firmwarefile, doc["gzipfile"] );
            log_d("firmwarefile: %s", firmwarefile );
        }

        if ( firmwarehost != NULL && firmwarefile != NULL ) {
            if ( firmwareurl == NULL ) {
                firmwareurl = (char*)CALLOC( strlen( firmwarehost ) + strlen( firmwarefile ) + 5, 1 );
                if ( firmwareurl == NULL ) {
                    log_e("calloc error");
                    while(true);
                }
            }
            else {
                char * tmp_firmwareurl = (char*)REALLOC( firmwareurl, strlen( firmwarehost ) + strlen( firmwarefile ) + 5 );
                if ( tmp_firmwareurl == NULL ) {
                    log_e("realloc error");
                    while(true);
                }
                firmwareurl = tmp_firmwareurl;            
            }
            snprintf( firmwareurl, strlen( firmwarehost ) + strlen( firmwarefile ) + 5, "%s/%s", firmwarehost, firmwarefile );
            log_d("firmwareurl: %s", firmwareurl );
        }

        if ( doc["version"] ) {
            firmwareversion = atoll( doc["version"] );
            log_d("firmwareversion: %ld", firmwareversion );
        }

        if ( doc["size"] ) {
            firmwaresize = atoi( doc["size"] );
            log_d("firmwaresize: %d", firmwaresize );
        }

        if ( doc["md5"] ) {
            if ( firmwaremd5 == NULL ) {
                firmwaremd5 = (char*)CALLOC( strlen( doc["md5"] ) + 1, 1 );
                if ( firmwaremd5 == NULL ) {
                    log_e("calloc error");
                    while(true);
                }
            }
            else {
                char * tmp_firmwaremd5 = (char*)REALLOC( firmwaremd5, strlen( doc["md5"] ) + 1 );
                if ( tmp_firmwaremd5 == NULL ) {
                    log_e("realloc error");
                    while(true);
                }
                firmwaremd5 = tmp_firmwaremd5;
            }
            strcpy( firmwaremd5, doc["md5"] );
            log_d("md5: %s", firmwaremd5 );
        }

        if ( doc["comment"] ) {
            if ( firmwarecomment == NULL ) {
                firmwarecomment = (char*)CALLOC( strlen( doc["comment"] ) + 1, 1 );
                if ( firmwarecomment == NULL ) {
                    log_e("calloc error");
                    while(true);
                }
            }
            else {
                char * tmp_firmwarecomment = (char*)REALLOC( firmwarecomment, strlen( doc["comment"] ) + 1 );
                if ( tmp_firmwarecomment == NULL ) {
                    log_e("realloc error");
                    while(true);
                }
                firmwarecomment = tmp_firmwarecomment;
            }
            strcpy( firmwarecomment, doc["comment"] );
            log_d("comment: %s", firmwarecomment );
        }
        /**
         * clear json
         */
        doc.clear();
        httpcode = 200;
    }
    else {
        httpcode = -1;
    }
    /**
     * clear uri dsc
     */
    uri_load_free_all( uri_load_dsc );

    return( firmwareversion );
}

const char* update_get_url( void ) {
    if ( firmwareversion > 0 ) {
        return( (const char*)firmwareurl );
    }
    return( NULL );
}

const char* update_get_md5( void ) {
    if ( firmwareversion > 0 ) {
        return( (const char*)firmwaremd5 );
    }
    return( NULL );
}

const char* update_get_comment( void ) {
    if ( firmwareversion > 0 ) {
        return( (const char*)firmwarecomment );
    }
    return( NULL );
}

int32_t update_get_size( void ) {
    return( firmwaresize );
}