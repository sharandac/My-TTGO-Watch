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
    UPDATE_CHECK_VERSION_LOG("load update information from: %s", url );
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

        if ( doc.containsKey("host") ) {
            if ( firmwarehost == NULL ) {
                firmwarehost = (char*)CALLOC( strlen( doc["host"] ) + 1, 1 );
                ASSERT( firmwarehost, "calloc error" );
            }
            else {
                char * tmp_firmwarehost = (char*)REALLOC( firmwarehost, strlen( doc["host"] ) + 1 );
                ASSERT( tmp_firmwarehost, "calloc error" );
                firmwarehost = tmp_firmwarehost;
            }
            strcpy( firmwarehost, doc["host"] );
            UPDATE_CHECK_VERSION_LOG("firmwarehost: %s", firmwarehost );
        }

        if ( doc.containsKey("file") ) {
            if ( firmwarefile == NULL ) {
                firmwarefile = (char*)CALLOC( strlen( doc["file"] ) + 1, 1 );
                ASSERT( firmwarefile, "calloc error" );
            }
            else {
                char * tmp_firmwarefile = (char*)REALLOC( firmwarefile, strlen( doc["file"] ) + 1 );
                ASSERT( tmp_firmwarefile, "calloc error" );
                firmwarefile = tmp_firmwarefile;
            }
            strcpy( firmwarefile, doc["file"] );
            UPDATE_CHECK_VERSION_LOG("firmwarefile: %s", firmwarefile );
        }

        if ( doc.containsKey("gzipfile") ) {
            if ( firmwarefile == NULL ) {
                firmwarefile = (char*)CALLOC( strlen( doc["gzipfile"] ) + 1, 1 );
                ASSERT( firmwarefile, "calloc error" );
            }
            else {
                char * tmp_firmwarefile = (char*)REALLOC( firmwarefile, strlen( doc["gzipfile"] ) + 1 );
                ASSERT( tmp_firmwarefile, "calloc error" );
                firmwarefile = tmp_firmwarefile;
            }
            strcpy( firmwarefile, doc["gzipfile"] );
            UPDATE_CHECK_VERSION_LOG("firmwarefile: %s", firmwarefile );
        }

        if ( firmwarehost != NULL && firmwarefile != NULL ) {
            if ( firmwareurl == NULL ) {
                firmwareurl = (char*)CALLOC( strlen( firmwarehost ) + strlen( firmwarefile ) + 5, 1 );
                ASSERT( firmwareurl, "calloc error" );
            }
            else {
                char * tmp_firmwareurl = (char*)REALLOC( firmwareurl, strlen( firmwarehost ) + strlen( firmwarefile ) + 5 );
                ASSERT( tmp_firmwareurl, "calloc error" );
                firmwareurl = tmp_firmwareurl;            
            }
            snprintf( firmwareurl, strlen( firmwarehost ) + strlen( firmwarefile ) + 5, "%s/%s", firmwarehost, firmwarefile );
            UPDATE_CHECK_VERSION_LOG("firmwareurl: %s", firmwareurl );
        }

        if ( doc.containsKey("version") ) {
            firmwareversion = atoll( doc["version"] );
            UPDATE_CHECK_VERSION_LOG("firmwareversion: %ld", firmwareversion );
        }

        if ( doc.containsKey("size") ) {
            firmwaresize = atoi( doc["size"] );
            UPDATE_CHECK_VERSION_LOG("firmwaresize: %d", firmwaresize );
        }

        if ( doc.containsKey("md5") ) {
            if ( firmwaremd5 == NULL ) {
                firmwaremd5 = (char*)CALLOC( strlen( doc["md5"] ) + 1, 1 );
                ASSERT( firmwaremd5, "calloc error" );
            }
            else {
                char * tmp_firmwaremd5 = (char*)REALLOC( firmwaremd5, strlen( doc["md5"] ) + 1 );
                ASSERT( tmp_firmwaremd5, "calloc error" );
                firmwaremd5 = tmp_firmwaremd5;
            }
            strcpy( firmwaremd5, doc["md5"] );
            UPDATE_CHECK_VERSION_LOG("md5: %s", firmwaremd5 );
        }

        if ( doc.containsKey("comment") ) {
            if ( firmwarecomment == NULL ) {
                firmwarecomment = (char*)CALLOC( strlen( doc["comment"] ) + 1, 1 );
                ASSERT( firmwarecomment, "calloc error" );
            }
            else {
                char * tmp_firmwarecomment = (char*)REALLOC( firmwarecomment, strlen( doc["comment"] ) + 1 );
                ASSERT( tmp_firmwarecomment, "calloc error" );
                firmwarecomment = tmp_firmwarecomment;
            }
            strcpy( firmwarecomment, doc["comment"] );
            UPDATE_CHECK_VERSION_LOG("comment: %s", firmwarecomment );
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
        return( firmwareurl ? (const char*)firmwareurl : "" );
    }
    return( NULL );
}

const char* update_get_md5( void ) {
    if ( firmwareversion > 0 ) {
        return( firmwaremd5 ? (const char*)firmwaremd5 : "" );
    }
    return( NULL );
}

const char* update_get_comment( void ) {
    if ( firmwareversion > 0 ) {
        return( firmwarecomment ? (const char*)firmwarecomment : "" );
    }
    return( NULL );
}

int32_t update_get_size( void ) {
    return( firmwaresize );
}