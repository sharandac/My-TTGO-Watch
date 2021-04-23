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
#include "config.h"
#include <TTGO.h>
#include <HTTPClient.h>
#include <Update.h>
#include <SPIFFS.h>

#include "osm_helper.h"
#include "utils/alloc.h"

/*
 * https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#C.2FC.2B.2B
 */
uint32_t osm_helper_long2tilex(double lon, uint32_t z){ 
	return (uint32_t)(floor((lon + 180.0) / 360.0 * (1 << z))); 
}

uint32_t osm_helper_lat2tiley(double lat, uint32_t z) { 
    double latrad = lat * M_PI/180.0;
	return (uint32_t)(floor((1.0 - asinh(tan(latrad)) / M_PI) / 2.0 * (1 << z))); 
}

double osm_helper_tilex2long(uint32_t x, uint32_t z) {
	return x / (double)(1 << z) * 360.0 - 180;
}

double osm_helper_tiley2lat(uint32_t y, uint32_t z) {
	double n = M_PI - 2.0 * M_PI * y / (double)(1 << z);
	return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

bool osm_helper_location_update( osm_location_t *osm_location, double lon, double lat, uint32_t zoom ) {
    /**
     * set osm_location structure
     */
    osm_location->zoom = zoom;
    osm_location->lat = lat;
    osm_location->lon = lon;
    /**
     * check if osm tile change
     */
    osm_location->tile_update = false;

    if ( osm_location->tilex != osm_helper_long2tilex( lon, osm_location->zoom ) ) {
        osm_location->tilex = osm_helper_long2tilex( lon, osm_location->zoom );
        osm_location->tile_update = true;
    }

    if ( osm_location->tiley != osm_helper_lat2tiley( lat, osm_location->zoom ) ) {
        osm_location->tiley = osm_helper_lat2tiley( lat, osm_location->zoom );
        osm_location->tile_update = true;
    }

    if ( osm_location->tile_update ) {
        log_d("tile update: %s/%d/%d/%d.png", OSM_TILE_SERVER, osm_location->zoom, osm_location->tilex, osm_location->tiley );
    }

    osm_location->tiley_left_top_edge = osm_helper_tiley2lat( osm_location->tiley, osm_location->zoom );
    osm_location->tilex_left_top_edge = osm_helper_tilex2long( osm_location->tilex, osm_location->zoom );
    osm_location->tiley_right_bottom_edge = osm_helper_tiley2lat( osm_location->tiley + 1, osm_location->zoom );
    osm_location->tilex_right_bottom_edge = osm_helper_tilex2long( osm_location->tilex + 1, osm_location->zoom );
    osm_location->tiley_res = abs( osm_helper_tiley2lat( osm_location->tiley, osm_location->zoom ) - osm_helper_tiley2lat( osm_location->tiley + 1, osm_location->zoom ) );
    osm_location->tilex_res = abs( osm_helper_tiley2lat( osm_location->tilex, osm_location->zoom ) - osm_helper_tiley2lat( osm_location->tilex + 1, osm_location->zoom ) );
    osm_location->tiley_px_res = osm_location->tiley_res / 240;
    osm_location->tilex_px_res = osm_location->tilex_res / 240;
    osm_location->tiley_pos = abs( osm_location->tiley_left_top_edge - osm_location->lat ) / osm_location->tiley_px_res;
    osm_location->tilex_pos = abs( osm_location->tilex_left_top_edge - osm_location->lon ) / osm_location->tilex_px_res;
    log_d("tile resolution: %f°/%f°", osm_location->tiley_res, osm_location->tilex_res );
    log_d("pixel resolution: %f°/%f°", osm_location->tiley_px_res, osm_location->tilex_px_res );
    log_d("pixel pos: x%d/y%d", osm_location->tiley_pos, osm_location->tilex_pos );

    return( osm_location->tile_update );
}

lv_img_dsc_t *osm_helper_get_tile_image( osm_location_t *osm_location, lv_img_dsc_t *osm_map_data ) {
    char url[128] = "";                             /** @brief url buffer */
    char buffer[1024];                              /** @brief http stream buffer for downloading */
    uint32_t len = 0;                               /** @brief http file size */
    uint32_t downloaded_len = 0;                    /** @brief http downloaded file size */

    uint8_t *image = NULL;                          /** @brief pointer to the raw file data */
    uint8_t *image_write_p = NULL;                  /** @brief write pointer for the raw file download */
    HTTPClient osm_client;                          /** @brief http download client */

    /**
     * alloc lv_img_dsc_t structure if not exist
     */
    if ( !osm_map_data ) {
        osm_map_data = (lv_img_dsc_t *)CALLOC( sizeof( lv_img_dsc_t ), 1 );
        if ( !osm_map_data ) {
            log_e("alloc error");
            while(1);
        }
        osm_map_data->header.always_zero = 0;
        osm_map_data->header.w = 256;
        osm_map_data->header.h = 256;
        osm_map_data->data_size = downloaded_len;
        osm_map_data->header.cf = LV_IMG_CF_RAW_ALPHA;
        osm_map_data->data = NULL;
    }
    /**
     * setup http connection
     */
    snprintf( url, sizeof( url ), "%s/%d/%d/%d.png", OSM_TILE_SERVER, osm_location->zoom, osm_location->tilex, osm_location->tiley );
    log_i("tile url: %s", url );
    /**
     * setup user agent and get connect
     */
    osm_client.begin( url );
    int httpCode = osm_client.GET();
    /**
     * check return code ok
     */
    if ( httpCode > 0 && httpCode == HTTP_CODE_OK ) {
        len = osm_client.getSize();
        log_i("tile size: %d bytes", len );
        image = (uint8_t*)CALLOC( len, 1 );
        /**
         * check if allocation/reallocation was successfull
         */
        if ( !image ) {
            log_e("allocation failed");
            while(1);
        }
        /**
         * setup writepointer to the start position
         */
        image_write_p = image;
        WiFiClient * osm_stream = osm_client.getStreamPtr();
        /**
         * get download data
         */
		while( osm_client.connected() && ( len > 0 ) ) {
			/**
             * get bytes in buffer and store them
             */
            size_t size = osm_stream->available();
			if ( size > 0 ) {
				size_t c = osm_stream->readBytes( image_write_p, ( ( size > sizeof( buffer ) ) ? sizeof( buffer ) : size ) );
                downloaded_len += c;
                image_write_p = image_write_p + c;
                /**
                 * all data downloaded?
                 */
				if ( len > 0 ) {
                    len -= c;
                }
                else {
                    break;
                }
			}
		}
        /**
         * setup lv_img_dsc_t structure
         */
        osm_map_data->header.always_zero = 0;
        osm_map_data->header.cf = LV_IMG_CF_RAW_ALPHA;
        osm_map_data->header.w = 256;
        osm_map_data->header.h = 256;
        if ( osm_map_data->data )
            free( (void*)osm_map_data->data );
        osm_map_data->data = image;
        osm_map_data->data_size = downloaded_len;
        lv_img_cache_invalidate_src( osm_map_data );
        log_i("downloaded tile size: %d bytes", downloaded_len );
    }
    else {
        /**
         * clear old osm_map_data strcuture
         */
        if ( osm_map_data->data )
            free( (void*)osm_map_data->data );
        free( (void*)osm_map_data );
        osm_map_data = NULL;
    }
    osm_client.end();

    return( osm_map_data );
}

bool osm_helper_get_tile_image_and_save( osm_location_t *osm_location, char *file_name ) {
    char url[128] = "";
    bool retval = false;
    HTTPClient osm_client;

    snprintf( url, sizeof( url ), "%s/%d/%d/%d.png", OSM_TILE_SERVER, osm_location->zoom, osm_location->tilex, osm_location->tiley );
    log_i("tile url: %s", url );
    /**
     * setup user agent and get connect
     */
    osm_client.begin( url );
    int httpCode = osm_client.GET();
    /**
     * check return code ok
     */
    if ( httpCode > 0 && httpCode == HTTP_CODE_OK ) {
        fs::File f = SPIFFS.open( file_name, "w");
        if (!f) {
            osm_client.end();
            return false;
        }
        uint8_t buff[512] = { 0 };
        int total = osm_client.getSize();
        int len = total;
        WiFiClient *stream = osm_client.getStreamPtr();
        while ( osm_client.connected() && (len > 0 || len == -1 ) ) {
            size_t size = stream->available();
            if (size) {
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                f.write(buff, c);
                if (len > 0) {
                    len -= c;
                }
            }
            delay(1);
        }
        log_i("Size: %u\n", f.size());
        f.close();
        retval = true;
    }
    osm_client.end();

    return( retval );
}