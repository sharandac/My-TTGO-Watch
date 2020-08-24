/****************************************************************************
 *   Aug 22 16:36:11 2020
 *   Copyright  2020  Chris McNamee
 *   Email: chris.mcna@gmail.com
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
#include "HTTPClient.h"

#include "crypto_ticker.h"
#include "crypto_ticker_main.h"
#include "crypto_ticker_widget.h"
#include "crypto_ticker_fetch.h"

#include "hardware/powermgm.h"
#include "hardware/json_psram_allocator.h"


int crypto_ticker_fetch_price( crypto_ticker_config_t *crypto_ticker_config, crypto_ticker_widget_data_t *crypto_ticker_widget_data ) {
    char url[512]="";
    int httpcode = -1;

    
    snprintf( url, sizeof( url ), "http://%s/api/CryptoTicker/Price/%s", MY_TTGO_WATCH_HOST, crypto_ticker_config->symbol);

    HTTPClient today_client;

    today_client.useHTTP10( true );
    today_client.begin( url );
    today_client.addHeader("force-unsecure","true");
    httpcode = today_client.GET();

    if ( httpcode != 200 ) {
        log_e("HTTPClient error %d", httpcode, url );
        today_client.end();
        return( -1 );
    }

    SpiRamJsonDocument doc( 1000 );

    DeserializationError error = deserializeJson( doc, today_client.getStream() );
    if (error) {
        log_e("crypto_ticker deserializeJson() failed: %s", error.c_str() );
        doc.clear();
        today_client.end();
        return( -1 );
    }

    today_client.end();

    crypto_ticker_widget_data->valide = true;
    strcpy( crypto_ticker_widget_data->price, doc["price"] );


    doc.clear();
    return( httpcode );
}



int crypto_ticker_fetch_statistics( crypto_ticker_config_t *crypto_ticker_config, crypto_ticker_main_data_t *crypto_ticker_main_data ) {
    char url[512]="";
    int httpcode = -1;

    
    snprintf( url, sizeof( url ), "http://%s/api/CryptoTicker/24hrStatistics/%s", MY_TTGO_WATCH_HOST, crypto_ticker_config->symbol);

    HTTPClient today_client;

    today_client.useHTTP10( true );
    today_client.begin( url );
    today_client.addHeader("force-unsecure","true");
    httpcode = today_client.GET();

    if ( httpcode != 200 ) {
        log_e("HTTPClient error %d", httpcode, url );
        today_client.end();
        return( -1 );
    }

    SpiRamJsonDocument doc( 1000 );

    DeserializationError error = deserializeJson( doc, today_client.getStream() );
    if (error) {
        log_e("crypto_ticker deserializeJson() failed: %s", error.c_str() );
        doc.clear();
        today_client.end();
        return( -1 );
    }

    today_client.end();

    crypto_ticker_main_data->valide = true;
    strcpy( crypto_ticker_main_data->lastPrice, doc["lastPrice"] );
    strcpy( crypto_ticker_main_data->priceChangePercent, doc["priceChangePercent"] );
    strcpy( crypto_ticker_main_data->volume, doc["volume"] );

    doc.clear();
    return( httpcode );
}