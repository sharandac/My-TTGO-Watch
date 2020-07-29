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

#include "hardware/powermgm.h"

/* Utility function to convert numbers to directions */
static void weather_wind_to_string( weather_forcast_t* container, int speed, int directionDegree);

uint32_t weather_fetch_today( weather_config_t *weather_config, weather_forcast_t *weather_today ) {
    
    WiFiClient today_client;
    uint32_t retval = -1;

	if ( !today_client.connect( OWM_HOST, OWM_PORT ) ) {
        log_e("connection failed");
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
            log_e("connection timeout");
			today_client.stop();
			return( retval );
		}
	}

    char *json = (char *)ps_malloc( WEATHER_TODAY_BUFFER_SIZE );
    if ( json == NULL ) {
        log_e("memory alloc failed");
        today_client.stop();
        return( retval );
    }
    char *ptr = json;

    bool data_begin = false;
    while( today_client.available() ) {
        if ( data_begin ) {
            ptr[ today_client.readBytes( ptr, WEATHER_TODAY_BUFFER_SIZE - 1 ) ] = '\0';
        }
		else if ( today_client.read() == '{' ) {
            data_begin = true;
            *ptr = '{';
            ptr++;
        }
	}

    today_client.stop();

    if ( data_begin == false ) {
        free( json );
        return( retval );
    }
    today_client.stop();

    DynamicJsonDocument doc(20000);

    DeserializationError error = deserializeJson( doc, json);
    if (error) {
        log_e("weather today deserializeJson() failed: %s", error.c_str() );
        doc.clear();
        free( json );
        return( retval );
    }

    retval = doc["cod"].as<int>();

    if ( retval != 200 ) {
        log_e("get weather failed, returncode: %d", retval );
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

    int directionDegree = doc["wind"]["deg"].as<int>();
    int speed = doc["wind"]["speed"].as<int>();
    weather_wind_to_string( weather_today, speed, directionDegree );

    doc.clear();
    free( json );
    return( retval );
}

uint32_t weather_fetch_forecast( weather_config_t *weather_config, weather_forcast_t * weather_forecast ) {
    
    WiFiClient forecast_client;
    uint32_t retval = -1;

	if ( !forecast_client.connect( OWM_HOST, OWM_PORT ) ) {
        log_e("connection failed");
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
            log_e("connection timeout");
			forecast_client.stop();
			return( retval );
		}
	}

    char *json = (char *)ps_malloc( WEATHER_FORECAST_BUFFER_SIZE );
    if ( json == NULL ) {
        log_e("memory alloc failed");
        forecast_client.stop();
        return( retval );
    }
    char *ptr = json;

    bool data_begin = false;
    while( forecast_client.available() ) {
        if ( data_begin ) {
            ptr[ forecast_client.readBytes( ptr, WEATHER_FORECAST_BUFFER_SIZE - 1 ) ] = '\0';
        }
		else if ( forecast_client.read() == '{' ) {
            data_begin = true;
            *ptr = '{';
            ptr++;
        }
	}

    forecast_client.stop();

    if ( data_begin == false ) {
        free( json );
        return( retval );
    }

    DynamicJsonDocument doc(20000);
    DeserializationError error = deserializeJson( doc, json );
    if (error) {
        log_e("weather forecast deserializeJson() failed: %s", error.c_str() );
        doc.clear();
        free( json );
        return( retval );
    }

    retval = doc["cod"].as<int>();

    if ( retval != 200 ) {
        log_e("get weather forecast failed, returncode: %d", retval );
        doc.clear();
        free( json );
        return( retval );
    }

    weather_forecast[0].valide = true;
    for ( int i = 0 ; i < WEATHER_MAX_FORECAST ; i++ ) {
        weather_forecast[ i ].timestamp = doc["list"][i]["dt"].as<long>();
        snprintf( weather_forecast[ i ].temp, sizeof( weather_forecast[ i ].temp ),"%0.1f°C", doc["list"][i]["main"]["temp"].as<float>() - 273.15 );
        snprintf( weather_forecast[ i ].humidity, sizeof( weather_forecast[ i ].humidity ),"%f%%", doc["list"][i]["main"]["humidity"].as<float>() );
        snprintf( weather_forecast[ i ].pressure, sizeof( weather_forecast[ i ].pressure ),"%fpha", doc["list"][i]["main"]["pressure"].as<float>() );
        strcpy( weather_forecast[ i ].icon, doc["list"][i]["weather"][0]["icon"] );
        strcpy( weather_forecast[ i ].name, doc["city"]["name"] );

        int directionDegree = doc["list"][i]["wind"]["deg"].as<int>();
        int speed = doc["list"][i]["wind"]["speed"].as<int>();
        weather_wind_to_string( &weather_forecast[i], speed, directionDegree );
    }

    doc.clear();
    free( json );
    return( 200 );
}

void weather_wind_to_string( weather_forcast_t* container, int speed, int directionDegree )
{
    const char *dir = "N";
    if ( directionDegree > 348 )
        ; // already set to "N"
    else if ( directionDegree > 326 )
        dir = "NNW";
    else if ( directionDegree > 303 )
        dir = "NW";
    else if ( directionDegree > 281 )
        dir = "WNW";
    else if ( directionDegree > 258 )
        dir = "W";
    else if ( directionDegree > 236 )
        dir = "WSW";
    else if ( directionDegree > 213 )
        dir = "SW";
    else if ( directionDegree > 191 )
        dir = "SSW";
    else if ( directionDegree > 168 )
        dir = "S";
    else if ( directionDegree > 146 )
        dir = "SSE";
    else if ( directionDegree > 123 )
        dir = "SE";
    else if ( directionDegree > 101 )
        dir = "ESE";
    else if ( directionDegree > 78 )
        dir = "E";
    else if ( directionDegree > 56 )
        dir = "ENE";
    else if ( directionDegree > 33 )
        dir = "NE";
    else if ( directionDegree > 11 )
        dir = "NNE";
    snprintf( container->wind, sizeof(container->wind), "%d %s", speed, dir);
    return;
}