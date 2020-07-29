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
#include <Arduino.h>
#include <WiFi.h>
#include "ArduinoJson.h"
#include "HTTPClient.h"

#include "update_check_version.h"

uint64_t update_check_new_version( void ) {
    
    WiFiClient check_version_client;
    uint64_t retval = -1;

	if ( !check_version_client.connect( FIRMWARE_HOST, FIRMWARE_HOST_PORT ) ) {
        log_e("connection failed");
        return( -1 );
	}

	check_version_client.printf(    "GET /ttgo-t-watch2020_v1.version.json HTTP/1.1\r\n"
                                    "Host: %s\r\n"
                                    "Connection: close\r\n"
                                    "Pragma: no-cache\r\n"
                                    "Cache-Control: no-cache\r\n"
                                    "User-Agent: ESP32\r\n"
                                    "Accept: text/html,application/json\r\n\r\n", FIRMWARE_HOST );

	uint64_t startMillis = millis();
	while ( check_version_client.available() == 0 ) {
		if ( millis() - startMillis > 5000 ) {
            log_e("connection timeout");
			check_version_client.stop();
			return( retval );
		}
	}

    char *json = (char *)ps_malloc( 200 );
    if ( json == NULL ) {
        log_e("memory alloc failed");
        check_version_client.stop();
        return( retval );
    }
    char *ptr = json;

    bool data_begin = false;
    while( check_version_client.available() ) {
        if ( data_begin ) {
            ptr[ check_version_client.readBytes( ptr, 100 - 1 ) ] = '\0';
        }
		else if ( check_version_client.read() == '{' ) {
            data_begin = true;
            *ptr = '{';
            ptr++;
        }
	}

    check_version_client.stop();

    if ( data_begin == false ) {
        free( json );
        return( retval );
    }
    check_version_client.stop();

    DynamicJsonDocument doc( 400 );

    DeserializationError error = deserializeJson( doc, json);
    if (error) {
        log_e("update version deserializeJson() failed: ", error.c_str() );
        doc.clear();
        free( json );
        return( retval );
    }

    retval = atoll( doc["version"] );

    doc.clear();
    free( json );
    return( retval );
}