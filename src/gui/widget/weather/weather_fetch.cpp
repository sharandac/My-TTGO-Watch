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
#include "weather_forecast.h"

uint32_t weather_fetch_today( weather_config_t *weather_config, weather_forcast_t *weather_today ) {
    
    WiFiClient today_client;
    uint32_t retval = -1;

	if ( !today_client.connect( OWM_HOST, OWM_PORT ) ) {
    	Serial.println("Connection failed");
        return( -1 );
	}

	today_client.printf(  "GET /data/2.5/weather?lat=%s&lon=%s&appid=%s HTTP/1.1\r\n"
                    "Host: %s\r\n"
		            "Connection: close\r\n"
		            "Pragma: no-cache\r\n"
		            "Cache-Control: no-cache\r\n"
		            "User-Agent: ESP32\r\n"
		            "Accept: text/html,application/json\r\n\r\n", weather_config->lat, weather_config->lon, weather_config->apikey, OWM_HOST );

	uint64_t startMillis = millis();
	while ( today_client.available() == 0 ) {
		if ( millis() - startMillis > 5000 ) {
			today_client.stop();
			return( retval );
		}
	}

    char *json = (char *)ps_malloc( 40000 );
    char *ptr = json;

    bool data_begin = false;
    while( today_client.available() ) {
        if ( data_begin ) {
            *ptr = today_client.read();
            ptr++;
        }
		else if ( today_client.read() == '{' ) {
            data_begin = true;
            *ptr = '{';
            ptr++;
        }
	}
    *ptr = '\0';
    if ( data_begin == false ) {
        return( retval );
    }

    today_client.stop();

    DynamicJsonDocument doc(20000);

    DeserializationError error = deserializeJson( doc, json);
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        doc.clear();
        free( json );
        return( retval );
    }

    retval = doc["cod"].as<int>();

    if ( retval != 200 ) {
        doc.clear();
        free( json );
        return( retval );
    }

    weather_today->valide = true;
    snprintf( weather_today->temp, sizeof( weather_today->temp ),"%0.1f°C", doc["main"]["temp"].as<float>() - 273.15 );
    snprintf( weather_today->humidity, sizeof( weather_today->humidity ),"%f%%", doc["main"]["humidity"].as<float>() );
    snprintf( weather_today->pressure, sizeof( weather_today->pressure ),"%fpha", doc["main"]["pressure"].as<float>() );
    strcpy( weather_today->icon, doc["weather"][0]["icon"] );
    strcpy( weather_today->name, doc["name"] );

    doc.clear();
    free( json );
    return( retval );
}

uint32_t weather_fetch_forecast( weather_config_t *weather_config, weather_forcast_t * weather_forecast ) {
    
    WiFiClient forecast_client;
    uint32_t retval = -1;

    weather_forecast[ 0 ].valide = false;

	if ( !forecast_client.connect( OWM_HOST, OWM_PORT ) ) {
    	Serial.println("Connection failed");
        return( retval );
	}

	forecast_client.printf(  "GET /data/2.5/forecast?cnt=%d&lat=%s&lon=%s&appid=%s HTTP/1.1\r\n"
                    "Host: %s\r\n"
		            "Connection: close\r\n"
		            "Pragma: no-cache\r\n"
		            "Cache-Control: no-cache\r\n"
		            "User-Agent: ESP32\r\n"
		            "Accept: text/html,application/json\r\n\r\n", WEATHER_MAX_FORECAST, weather_config->lat, weather_config->lon, weather_config->apikey, OWM_HOST );

	uint64_t startMillis = millis();
	while ( forecast_client.available() == 0 ) {
		if ( millis() - startMillis > 5000 ) {
			forecast_client.stop();
			return( retval );
		}
	}

    char *json = (char *)ps_malloc( 20000 );
    char *ptr = json;

    bool data_begin = false;
    while( forecast_client.available() ) {
        yield();
        if ( data_begin ) {
            ptr[ forecast_client.readBytes( ptr, 40000 ) ] = '\0';
        }
		else if ( forecast_client.read() == '{' ) {
            data_begin = true;
            *ptr = '{';
            ptr++;
        }
	}

    forecast_client.stop();

    if ( data_begin == false ) {
        Serial.printf("No json data\r\n");
        free( json );
        return( retval );
    }

    DynamicJsonDocument doc(20000);
    DeserializationError error = deserializeJson( doc, json );
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        doc.clear();
        free( json );
        return( retval );
    }

    retval = doc["cod"].as<int>();

    if ( retval != 200 ) {
        doc.clear();
        free( json );
        return( retval );
    }

    weather_forecast[0].valide = true;
    for ( int i = 0 ; i < WEATHER_MAX_FORECAST ; i++ ) {
        snprintf( weather_forecast[ i ].temp, sizeof( weather_forecast[ i ].temp ),"%0.1f°C", doc["list"][i]["main"]["temp"].as<float>() - 273.15 );
        snprintf( weather_forecast[ i ].humidity, sizeof( weather_forecast[ i ].humidity ),"%f%%", doc["list"][i]["main"]["humidity"].as<float>() );
        snprintf( weather_forecast[ i ].pressure, sizeof( weather_forecast[ i ].pressure ),"%fpha", doc["list"][i]["main"]["pressure"].as<float>() );
        strcpy( weather_forecast[ i ].icon, doc["list"][i]["weather"][0]["icon"] );
        strcpy( weather_forecast[ i ].name, doc["city"]["name"] );
    }

    doc.clear();
    free( json );
    return( 200 );
}