/****************************************************************************
 *   July 23 00:23:05 2020
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

#include "weather.h"
#include "weather_fetch.h"

void weather_fetch_today( weather_config_t *weather_config, weather_forcast_t *weather_today ) {
    
    WiFiClient client;
    char *ptr = NULL;
    char json[2000] = "";
    StaticJsonDocument<2000> doc;

    weather_today->valide = false;

	if ( !client.connect( OWM_HOST, OWM_PORT ) ) {
    	Serial.println("Connection failed");
        return;
	}

	client.printf(  "GET /data/2.5/weather?lat=%s&lon=%s&appid=%s HTTP/1.1\r\n"
                    "Host: %s\r\n"
		            "Connection: close\r\n"
		            "Pragma: no-cache\r\n"
		            "Cache-Control: no-cache\r\n"
		            "User-Agent: ESP32\r\n"
		            "Accept: text/html,application/json\r\n\r\n", weather_config->lat, weather_config->lon, weather_config->apikey, OWM_HOST );

	uint64_t startMillis = millis();
	while ( client.available() == 0 ) {
		if ( millis() - startMillis > 5000 ) {
			client.stop();
			return;
		}
	}

    ptr = json;
    bool data_begin = false;
    while( client.available() ) {
        if ( data_begin ) {
            *ptr = client.read();
            ptr++;
        }
		else if ( client.read() == '{' ) {
            data_begin = true;
            *ptr = '{';
            ptr++;
        }
	}
    *ptr = '\0';
    if ( data_begin == false ) {
        return;
    }


    DeserializationError error = deserializeJson( doc, json);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }

    weather_today->valide = true;
    snprintf( weather_today->temp, sizeof( weather_today->temp ),"%0.1f°C", doc["main"]["temp"].as<float>() - 273.15 );
    snprintf( weather_today->humidity, sizeof( weather_today->humidity ),"%f%%", doc["main"]["humidity"].as<float>() );
    snprintf( weather_today->pressure, sizeof( weather_today->pressure ),"%fpha", doc["main"]["ressure"].as<float>() );
    strcpy( weather_today->icon, doc["weather"][0]["icon"] );
    strcpy( weather_today->name, doc["name"] );

    client.stop();
}