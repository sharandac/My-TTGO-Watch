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
#include "ArduinoJson.h"
#include "HTTPClient.h"

#include "update_check_version.h"

// arduinoJson allocator for external PSRAM
// see: https://arduinojson.org/v6/how-to/use-external-ram-on-esp32/
struct UpdateSpiRamAllocator {
    void* allocate( size_t size ) { 
        void *psram = ps_calloc( size, 1 );
        if ( psram ) {
            log_i("allocate %dbytes(%p) json psram", size, psram );
            return( psram );
        }
        else {
            log_e("allocate %dbytes(%p) json psram failed", size, psram );
            while(1);
        }
    }
    void deallocate( void* pointer ) {
        log_i("deallocate (%p) json psram", pointer );
        free( pointer );
    }
};
using SpiRamJsonDocument = BasicJsonDocument<UpdateSpiRamAllocator>;

uint64_t update_check_new_version( void ) {
    char url[512]="";
    int httpcode = -1;
    uint64_t version = -1;

    snprintf( url, sizeof( url ), "http://%s/%s", FIRMWARE_HOST, FIRMWARE_VERSION_FILE );

    HTTPClient check_update_client;

    check_update_client.useHTTP10( true );
    check_update_client.setUserAgent( "ESP32-" __FIRMWARE__ );
    check_update_client.begin( url );
    httpcode = check_update_client.GET();

    if ( httpcode != 200 ) {
        log_e("HTTPClient error %d", httpcode );
        check_update_client.end();
        return( -1 );
    }

    SpiRamJsonDocument doc( check_update_client.getSize() * 2 );

    DeserializationError error = deserializeJson( doc, check_update_client.getStream() );
    if (error) {
        log_e("update check deserializeJson() failed: %s", error.c_str() );
        doc.clear();
        check_update_client.end();
        return( -1 );
    }

    check_update_client.end();

    version = atoll( doc["version"] );

    doc.clear();
    return( version );
}