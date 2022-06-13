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

#include "osm_map.h"
#include "utils/alloc.h"
#include "utils/uri_load/uri_load.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
    #include <iostream>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include <string.h>
    #include <string>
    #include <math.h>
    
    using namespace std;
    #define String string
#else
    #include <HTTPClient.h>
    #include <Update.h>
    #include <SPIFFS.h>
#endif

/**
 * @brief osm_map default tile image
 */
LV_IMG_DECLARE(osm_no_data_256px);

bool osm_map_take( osm_location_t *osm_location );
void osm_map_give( osm_location_t *osm_location );
uint32_t osm_map_long2tilex(double lon, uint32_t z);
uint32_t osm_map_lat2tiley(double lat, uint32_t z);
double osm_map_tilex2long(int x, uint32_t z);
double osm_map_tiley2lat(int y, uint32_t z);
osm_location_t *osm_map_update_tile_image( osm_location_t *osm_location );
uri_load_dsc_t *osm_map_get_cache_tile_image( osm_location_t *osm_location );
void osm_map_gen_url( osm_location_t *osm_location );

osm_location_t *osm_map_create_location_obj( void ) {
    /**
     * allocate osm_location structure
     */
    osm_location_t *osm_location = (osm_location_t*)MALLOC_ASSERT( sizeof( osm_location_t ), "osm location obj allocation failed" );
    /**
     * if allocation was successfull, set to default
     */
    if ( osm_location ) {
        osm_location->zoom = 16;                      
        osm_location->lon = 0;                         
        osm_location->lat = 51.5285582;                         
        osm_location->tilex = 0;                     
        osm_location->tiley = 0;
        osm_location->manual_nav = false;
        osm_location->manual_nav_update = false;
        osm_location->tilex_manual_nav = 0;                     
        osm_location->tiley_manual_nav = 0;
        osm_location->tilex_left_top_edge = 0;         
        osm_location->tiley_left_top_edge = 0;         
        osm_location->tilex_right_bottom_edge = 0;     
        osm_location->tiley_right_bottom_edge = 0;     
        osm_location->tilex_res = 0;                  
        osm_location->tiley_res = 0;                   
        osm_location->tilex_px_res = 0;                
        osm_location->tiley_px_res = 0;                
        osm_location->tilex_dest_px_res = 240;         
        osm_location->tiley_dest_px_res = 240;
        osm_location->tilexy_pos_valid = false;
        osm_location->tilex_pos = 0;                 
        osm_location->tiley_pos = 0;    
        osm_location->tile_server_source_update = false;    
        osm_location->tile_server = NULL;
        osm_location->current_tile_url = NULL;
        osm_location->osm_map_data.header.always_zero = 0;
        osm_location->osm_map_data.header.cf = LV_IMG_CF_RAW_ALPHA;
        osm_location->osm_map_data.header.w = 256;
        osm_location->osm_map_data.header.h = 256;
        osm_location->osm_map_data.data = NULL;
        osm_location->osm_map_data.data_size = 0;
        osm_location->load_ahead = false;
        osm_location->cache_size = 0;
        osm_location->cached_fies = 0;
        for( int i = 0 ; i < DEFAULT_OSM_CACHE_SIZE ; i++ ) {
            osm_location->uri_load_dsc[ i ] = NULL;
        }
#ifndef NATIVE_64BIT
        osm_location->xSemaphoreMutex = xSemaphoreCreateMutex();;
#endif
    }
#ifdef NATIVE_64BIT
    OSM_MAP_LOG("osm_location: alloc %ld bytes at %p", sizeof( osm_location_t ), osm_location );
#else
    OSM_MAP_LOG("osm_location: alloc %d bytes at %p", sizeof( osm_location_t ), osm_location );
#endif
    return( osm_location );
}
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

bool osm_map_take( osm_location_t *osm_location ) {
    /**
     * enter critical section
     */
#ifdef NATIVE_64BIT
    return( true );
#else
    return xSemaphoreTake( osm_location->xSemaphoreMutex, portMAX_DELAY ) == pdTRUE;
#endif
}

void osm_map_give( osm_location_t *osm_location ) {
    /**
     * leave critical section
     */
#ifdef NATIVE_64BIT

#else
    xSemaphoreGive( osm_location->xSemaphoreMutex );
#endif
}

void osm_map_set_lon_lat( osm_location_t *osm_location, double lon, double lat ) {
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return;
    }
    
    osm_location->lat = lat;
    osm_location->lon = lon;
}

uint32_t osm_map_get_zoom( osm_location_t *osm_location ) {
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return( 4 );
    }
    
    return( osm_location->zoom );
}

bool osm_map_zoom_in( osm_location_t *osm_location ) {
    bool retval = false;
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return( retval );
    }
    /**
     * enter critical section
     */
    osm_map_take( osm_location );
    if ( osm_location->zoom < 18 ) {
        osm_location->zoom++;
        if ( osm_location->manual_nav ) {
            osm_location->tilex_manual_nav = osm_location->tilex_manual_nav * 2;
            osm_location->tiley_manual_nav = osm_location->tiley_manual_nav * 2;
            osm_location->manual_nav_update = true;
        }
        retval = true;
    }
    /**
     * leave critical section
     */
    osm_map_give( osm_location );

    return( retval );
}
bool osm_map_zoom_out( osm_location_t *osm_location ) {
    bool retval = false;
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return( retval );
    }
    /**
     * enter critical section
     */
    osm_map_take( osm_location );
    if ( osm_location->zoom > 2 ) {
        osm_location->zoom--;
        if ( osm_location->manual_nav ) {
            osm_location->tilex_manual_nav = osm_location->tilex_manual_nav / 2;
            osm_location->tiley_manual_nav = osm_location->tiley_manual_nav / 2;
            osm_location->manual_nav_update = true;
        }
        retval = true;
    }
    /**
     * leave critical section
     */
    osm_map_give( osm_location );
    return( retval );
}

void osm_map_set_zoom( osm_location_t *osm_location, uint32_t zoom ) {
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return;
    }
    osm_location->zoom = zoom;
}

lv_img_dsc_t *osm_map_get_tile_image( osm_location_t *osm_location ) {
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return( NULL );
    }

    if ( osm_location->osm_map_data.data ) {
        return( &osm_location->osm_map_data );
    }
    else {
        return( (lv_img_dsc_t*)&osm_no_data_256px );
    }
}

lv_img_dsc_t *osm_map_get_no_data_image( void ) {
    return( (lv_img_dsc_t*)&osm_no_data_256px );
}

void osm_map_center_location( osm_location_t *osm_location ) {
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return;
    }
    osm_location->manual_nav = false;
}

void osm_map_nav_direction( osm_location_t *osm_location, osm_map_nav_direction_t direction ) {
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return;
    }
    /**
     * enter critical section
     */
    osm_map_take( osm_location );
    /**
     * if we are not in manual nav, set current tile
     */
    if ( !osm_location->manual_nav ) {
        osm_location->tiley_manual_nav = osm_location->tiley;
        osm_location->tilex_manual_nav = osm_location->tilex;
        osm_location->manual_nav = true;
    }
    /**
     * move tilex/tilex
     */
    switch( direction ) {
        case( north ):
            OSM_MAP_LOG("move one tile north");
            osm_location->tiley_manual_nav--;
            osm_location->manual_nav_update = true;
            break;
        case( south ):
            OSM_MAP_LOG("move one tile south");
            osm_location->tiley_manual_nav++;
            osm_location->manual_nav_update = true;
            break;
        case( west ):
            OSM_MAP_LOG("move one tile west");
            osm_location->tilex_manual_nav--;
            osm_location->manual_nav_update = true;
            break;
        case( east ):
            OSM_MAP_LOG("move one tile east");
            osm_location->tilex_manual_nav++;
            osm_location->manual_nav_update = true;
            break;
        case( zoom_northwest ):
            OSM_MAP_LOG("zoom into northwest");
            osm_map_give( osm_location );
            osm_map_zoom_in( osm_location );
            osm_map_take( osm_location );
            break;
        case( zoom_northeast ):
            OSM_MAP_LOG("zoom into northeast");
            osm_map_give( osm_location );
            if ( osm_map_zoom_in( osm_location ) ) {
                osm_map_take( osm_location );
                osm_location->tilex_manual_nav++;
                osm_map_give( osm_location );
            }
            osm_map_take( osm_location );
            break;
        case( zoom_southwest ):
            OSM_MAP_LOG("zoom into southwest");
            osm_map_give( osm_location );
            if ( osm_map_zoom_in( osm_location ) ) {
                osm_map_take( osm_location );
                osm_location->tiley_manual_nav++;
                osm_map_give( osm_location );
            }
            osm_map_take( osm_location );
            break;
        case( zoom_southeast ):
            OSM_MAP_LOG("zoom into southeast");
            osm_map_give( osm_location );
            if ( osm_map_zoom_in( osm_location ) ) {
                osm_map_take( osm_location );
                osm_location->tilex_manual_nav++;
                osm_location->tiley_manual_nav++;
                osm_map_give( osm_location );
            }
            osm_map_take( osm_location );
            break;
    }

    /**
     * check for tile wraping
     */
    osm_location->tilex_manual_nav &= ( 1 << osm_location->zoom ) - 1;
    osm_location->tiley_manual_nav &= ( 1 << osm_location->zoom ) - 1;
    /**
     * leave critical section
     */
    osm_map_give( osm_location );
}

bool osm_map_update( osm_location_t *osm_location ) {
    bool tile_update = false;
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return( false );
    }
    /**
     * enter critical section
     */
    osm_map_take( osm_location );
    /**
     * check if the user set manual navigation
     */
    if ( !osm_location->manual_nav ) {
        if ( osm_location->tilex != osm_helper_long2tilex( osm_location->lon, osm_location->zoom ) ) {
            osm_location->tilex = osm_helper_long2tilex( osm_location->lon, osm_location->zoom );
            tile_update = true;
        }
        if ( osm_location->tiley != osm_helper_lat2tiley( osm_location->lat, osm_location->zoom ) ) {
            osm_location->tiley = osm_helper_lat2tiley( osm_location->lat, osm_location->zoom );
            tile_update = true;
        }
    }
    else {
        osm_location->tiley = osm_location->tiley_manual_nav;
        osm_location->tilex = osm_location->tilex_manual_nav;
    }
    /**
     * if tile, source or nav change -> update tile image
     */
    if ( tile_update || osm_location->tile_server_source_update || osm_location->manual_nav_update ) {
        tile_update = true;
        osm_location->tile_server_source_update = false;
        osm_location->manual_nav_update = false;
        /**
         * leave critical section
         */
        osm_map_give( osm_location );
        osm_location = osm_map_update_tile_image( osm_location );
        /**
         * enter critical section
         */
        osm_map_take( osm_location );
        /**
         * calculate new tile infomations
         */
        osm_location->tiley_left_top_edge = osm_helper_tiley2lat( osm_location->tiley, osm_location->zoom );
        osm_location->tilex_left_top_edge = osm_helper_tilex2long( osm_location->tilex, osm_location->zoom );
        osm_location->tiley_right_bottom_edge = osm_helper_tiley2lat( osm_location->tiley + 1, osm_location->zoom );
        osm_location->tilex_right_bottom_edge = osm_helper_tilex2long( osm_location->tilex + 1, osm_location->zoom );
        osm_location->tiley_res = abs( osm_helper_tiley2lat( osm_location->tiley, osm_location->zoom ) - osm_helper_tiley2lat( osm_location->tiley + 1, osm_location->zoom ) );
        osm_location->tilex_res = abs( osm_helper_tiley2lat( osm_location->tilex, osm_location->zoom ) - osm_helper_tiley2lat( osm_location->tilex + 1, osm_location->zoom ) );
        osm_location->tiley_px_res = osm_location->tiley_res / osm_location->tiley_dest_px_res;
        osm_location->tilex_px_res = osm_location->tilex_res / osm_location->tilex_dest_px_res;
        OSM_MAP_LOG("left/top: %f / %f", osm_location->tiley_left_top_edge, osm_location->tilex_left_top_edge );
        OSM_MAP_LOG("right/bottom: %f / %f", osm_location->tiley_right_bottom_edge, osm_location->tilex_right_bottom_edge );
        OSM_MAP_LOG("tilex/tiley resolution in degree: %f° / %f°", osm_location->tilex_res, osm_location->tiley_res );
        OSM_MAP_LOG("tilex/tiley pixel resultion in degree: %f° /%f°", osm_location->tilex_px_res, osm_location->tiley_px_res );
    }
    /**
     * check if current lon/lat on tile
     */
    if ( osm_location->tiley == osm_helper_lat2tiley( osm_location->lat, osm_location->zoom ) && osm_location->tilex == osm_helper_long2tilex( osm_location->lon, osm_location->zoom ) ) {
        osm_location->tilexy_pos_valid = true;
        osm_location->tiley_pos = abs( osm_location->tiley_left_top_edge - osm_location->lat ) / osm_location->tiley_px_res;
        osm_location->tilex_pos = abs( osm_location->tilex_left_top_edge - osm_location->lon ) / osm_location->tilex_px_res;
        OSM_MAP_LOG("current lon/lat is in view");
    }
    else {
        osm_location->tilexy_pos_valid = false;
        OSM_MAP_LOG("current lon/lat is not in view");
    }
    /**
     * leave critical section
     */
    osm_map_give( osm_location );
    return( tile_update );
}

/**
 * @brief get an new tile from osm an store it in a lv_img_dsc structure for direct lv_img_set_src use
 * 
 * @param osm_location  pointer to the osm_location structure
 * 
 * @return  updated lv_img_dsc structure
 */
osm_location_t *osm_map_update_tile_image( osm_location_t *osm_location ) {
    uri_load_dsc_t *uri_load_dsc = NULL;
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return( NULL );
    }
    /**
     * download file into RAM
     */
    uri_load_dsc = osm_map_get_cache_tile_image( osm_location );
    /**
     * enter critical section
     */
    osm_map_take( osm_location );
    /**
     * check if download was success
     */
    if ( uri_load_dsc ) {
        /**
         * set new image data
         */
        osm_location->osm_map_data.data = uri_load_dsc->data;
        osm_location->osm_map_data.data_size = uri_load_dsc->size;
        lv_img_cache_invalidate_src( &osm_location->osm_map_data );
    }
    else {
        /**
         * set default no data image
         */
        osm_location->osm_map_data.data = osm_no_data_256px.data;
        osm_location->osm_map_data.data_size = osm_no_data_256px.data_size;
        lv_img_cache_invalidate_src( &osm_location->osm_map_data );
    }
    /**
     * leave critical section
     */
    osm_map_give( osm_location );
    return( osm_location );
}

bool osm_map_load_tiles_ahead( osm_location_t *osm_location ) {
    static uint32_t tilex = 0, tiley = 0;
    static uint32_t load_ahead_progress = 0;
    static bool load_ahead_in_progress = false;
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return( false );
    }
    /**
     * enter critical section
     */
    osm_map_take( osm_location );
    if ( osm_location->load_ahead ) {
        /**
         * check if viewed tile has change
         */
        if ( tilex != osm_location->tilex || tiley != osm_location->tiley ) {
            tilex = osm_location->tilex;
            tiley = osm_location->tiley;
            load_ahead_in_progress = true;
            load_ahead_progress = 0;
            OSM_MAP_LOG("start load 4 tiles ahead");
        }
        /**
         * if a load ahead need?
         */
        if ( load_ahead_in_progress ) {
            switch( load_ahead_progress ) {
                case 0:
                    osm_location->tiley = tiley;
                    osm_location->tilex = tilex - 1;
                    break;
                case 1:
                    osm_location->tiley = tiley;
                    osm_location->tilex = tilex + 1;
                    break;
                case 2:
                    osm_location->tilex = tilex;
                    osm_location->tiley = tiley - 1;
                    break;
                case 3:
                    osm_location->tilex = tilex;
                    osm_location->tiley = tiley + 1;
                    break;
                default:
                    load_ahead_in_progress = false;
            }
            if ( load_ahead_in_progress ) {
                load_ahead_progress++;
                    /**
                     * leave critical section
                     */
                    osm_map_give( osm_location );
                    osm_map_get_cache_tile_image( osm_location );
                    /**
                     * enter critical section
                     */
                    osm_map_take( osm_location );
            }
            osm_location->tilex = tilex;
            osm_location->tiley = tiley;
        }
    }
    /**
     * leave critical section
     */
    osm_map_give( osm_location );
    return( load_ahead_in_progress );
}

uint32_t osm_map_get_used_cache_size( osm_location_t *osm_location ) {
    uint32_t cache_size = 0;
    if ( osm_location )
        cache_size = osm_location->cache_size;
    
    return( cache_size );
}

uint32_t osm_map_get_cache_files( osm_location_t *osm_location ) {
    uint32_t cached_file = 0;
    if ( osm_location )
        cached_file = osm_location->cached_fies;
    
    return( cached_file );
}

bool osm_map_get_load_ahead( osm_location_t *osm_location ) {
    bool load_ahead = false;
    
    if ( osm_location )
        load_ahead = osm_location->load_ahead;

    return( load_ahead );
}

void osm_map_set_load_ahead( osm_location_t *osm_location, bool load_ahead ) {
    if ( osm_location )
        osm_location->load_ahead = load_ahead;
}

char *osm_map_get_current_uri( osm_location_t *osm_location ) {
    char *uri = NULL;
    
    if ( osm_location )
        uri = osm_location->tile_server;

    return( uri );
}

char *osm_map_get_current_tile_uri( osm_location_t *osm_location ) {
    char *uri = NULL;
    
    if ( osm_location )
        uri = osm_location->current_tile_url;

    return( uri );
}

void osm_map_clear_cache( osm_location_t *osm_location ) {
    uint32_t cache_size = 0;
    uint32_t cache_files = 0;
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return;
    }
    /**
     * enter critical section
     */
    osm_map_take( osm_location );
    /**
     * clear cache
     * leave the current used tile image in memory
     */
    for( int i = 0 ; i < DEFAULT_OSM_CACHE_SIZE ; i++ ) {
        if ( osm_location->uri_load_dsc[ i ] ) {
            if ( osm_location->uri_load_dsc[ i ]->data != osm_location->osm_map_data.data ) {
                uri_load_free_all( osm_location->uri_load_dsc[ i ] );
                osm_location->uri_load_dsc[ i ] = NULL;
            }
        }
    }
    /**
     * get cache size
     */
    for( int i = 0 ; i < DEFAULT_OSM_CACHE_SIZE ; i++ ) {
        if ( osm_location->uri_load_dsc[ i ] ) {
            cache_size += osm_location->uri_load_dsc[ i ]->size;
            cache_files++;
        }
    }
    osm_location->cache_size = cache_size;
    osm_location->cached_fies = cache_files;
    /**
     * leave critical section
     */
    osm_map_give( osm_location );
}

uri_load_dsc_t *osm_map_get_cache_tile_image( osm_location_t *osm_location ) {
    size_t tile = -1;
    size_t cache_size = 0;
    size_t cache_file = 0;
    uint64_t timestamp = millis();
    uri_load_dsc_t *uri_load_dsc = NULL;
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return( NULL );
    }
    /**
     * generate tile image utl/uri
     */
    osm_map_gen_url( osm_location );
    /**
     * enter critical section
     */
    osm_map_take( osm_location );
    /**
     * check if tile image exist
     */
    for( int i = 0 ; i < DEFAULT_OSM_CACHE_SIZE ; i++ ) {
        if ( osm_location->uri_load_dsc[ i ] ) {
            if ( !strcmp( osm_location->current_tile_url, osm_location->uri_load_dsc[ i ]->uri ) ) {
                tile = i;
                break;
            }
        }
    }
    /**
     * check for a cache hit
     */
    if ( tile != -1 ) {
        OSM_MAP_LOG("url cache hit: %s", osm_location->uri_load_dsc[ tile ]->uri );
        uri_load_dsc = osm_location->uri_load_dsc[ tile ];
        uri_load_dsc->timestamp = millis();
    }
    else {
        /**
         * multi threading optimizing
         * -
         * 1st stage
         * load file into ram ahead
         * and give semaphore away in the time to download
         * and take it back after that
         */
        char *uri = (char*)MALLOC_ASSERT( strlen( osm_location->current_tile_url ) + 1, "error while uri alloc failed" );
        strncpy( uri, osm_location->current_tile_url, strlen( osm_location->current_tile_url ) + 1 );
        osm_map_give( osm_location );
        uri_load_dsc = uri_load_to_ram( (const char*)uri );
        osm_map_take( osm_location );
        free( uri );
        /**
         * 2nd stage
         * seek for a free tile cache
         */
        tile = -1;
        for( int i = 0 ; i < DEFAULT_OSM_CACHE_SIZE ; i++ ) {
            if ( !osm_location->uri_load_dsc[ i ] ) {
                tile = i;
                break;
            }
        }
        /**
         * 3rd stage
         * if ne free tile, search for the oldest tile
         */
        if ( tile == -1 ) {
            /**
             * search the oldest one
             */
            for( int i = 0 ; i < DEFAULT_OSM_CACHE_SIZE ; i++ ) {
                if ( osm_location->uri_load_dsc[ i ]->timestamp <= timestamp && osm_location->uri_load_dsc[ i ] ) {
                    timestamp = osm_location->uri_load_dsc[ i ]->timestamp;
                    tile = i;
                }
            }
            /**
             * delete the oldest one
             */
            OSM_MAP_LOG("cache full, delete the oldest: %s", osm_location->uri_load_dsc[ tile ]->uri );
            uri_load_free_all( osm_location->uri_load_dsc[ tile ] );
            osm_location->uri_load_dsc[ tile ] = NULL;
        }
        osm_location->uri_load_dsc[ tile ] = uri_load_dsc;
#ifdef NATIVE_64BIT
        OSM_MAP_LOG("use tile cache %ld", tile );
#else
        OSM_MAP_LOG("use tile cache %d", tile );
#endif
        /**
         * get cache size
         */
        for( int i = 0 ; i < DEFAULT_OSM_CACHE_SIZE ; i++ ) {
            if ( osm_location->uri_load_dsc[ i ] ) {
                cache_size += osm_location->uri_load_dsc[ i ]->size;
                cache_file++;
            }
        }
        osm_location->cache_size = cache_size;
        osm_location->cached_fies = cache_file;
#ifdef NATIVE_64BIT
        OSM_MAP_LOG("cached files: %ld, cachesize = %ld bytes", cache_file, cache_size );
#else
        OSM_MAP_LOG("cached files: %d, cachesize = %d bytes", cache_file, cache_size );
#endif
    }
    /**
     * leave critical section
     */
    osm_map_give( osm_location );
    return( uri_load_dsc );
}

void osm_map_set_tile_server( osm_location_t *osm_location, const char* tile_server ) {
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return;
    }
    /**
     * enter critical section
     */
    osm_map_take( osm_location );
    /**
     * free old tile server entry
     */
    if( osm_location->tile_server ) {
        free( (void *)osm_location->tile_server );
        osm_location->tile_server = NULL;
    }
    /**
     * allocate new memory and copy new tile server
     */
    osm_location->tile_server = (char *)MALLOC_ASSERT( strlen( tile_server ) + 1, "tile server alloc failed" );
    strcpy( osm_location->tile_server, tile_server );
    OSM_MAP_LOG("osm_location->tile_server: %s", osm_location->tile_server );
    osm_location->tile_server_source_update = true;
    /**
     * leave critical section
     */
    osm_map_give( osm_location );
}

void osm_map_gen_url( osm_location_t *osm_location ) {
    char *tile_server_p = NULL;
    char *current_tile_url_p = NULL;
    char temp_str[32] = "";
    char *temp_str_p = NULL;
    /**
     * check if osm_location set
     */
    if ( !osm_location ) {
        return;
    }
    /**
     * enter critical section
     */
    osm_map_take( osm_location );
    /**
     * is a tile server set?
     */
    if ( !osm_location->tile_server ) {
        OSM_MAP_LOG("set default osm tile server");
        osm_location->tile_server = (char*)MALLOC_ASSERT( sizeof( DEFAULT_OSM_TILE_SERVER ), "tile server alloc failed" );
#ifdef NATIVE_64BIT
        OSM_MAP_LOG("osm_location->tile_server: alloc %ld bytes at %p", sizeof( DEFAULT_OSM_TILE_SERVER ), osm_location->tile_server );
#else
        OSM_MAP_LOG("osm_location->tile_server: alloc %d bytes at %p", sizeof( DEFAULT_OSM_TILE_SERVER ), osm_location->tile_server );
#endif
        strcpy( osm_location->tile_server, DEFAULT_OSM_TILE_SERVER );
    }
    /**
     * alloc current tile url
     */
    if ( !osm_location->current_tile_url ) {
        osm_location->current_tile_url = (char *)MALLOC_ASSERT( MAX_CURRENT_TILE_URL_LEN, "current tile url alloc failed" );
        OSM_MAP_LOG("osm_location->current_tile_url: alloc %d bytes at %p", MAX_CURRENT_TILE_URL_LEN, osm_location->current_tile_url );
        *osm_location->current_tile_url = '\0';
    }
    /**
     * generate current tile url from tile server
     */
    tile_server_p = osm_location->tile_server;
    current_tile_url_p = osm_location->current_tile_url;

    while( *tile_server_p ) {
        if ( *tile_server_p == '$' ) {
            tile_server_p++;
            switch ( *tile_server_p ) {
                case 'z':
                    snprintf( temp_str, sizeof( temp_str ), "%d", osm_location->zoom );
                    break;
                case 'x':
                    snprintf( temp_str, sizeof( temp_str ), "%d", osm_location->tilex );
                    break;
                case 'y':
                    snprintf( temp_str, sizeof( temp_str ), "%d", osm_location->tiley );
                    break;
                default:
                    snprintf( temp_str, sizeof( temp_str ), "$%c", *tile_server_p );
                    break;
            }
            temp_str_p = temp_str;
            while( *temp_str_p ) {
                *current_tile_url_p = *temp_str_p;
                current_tile_url_p++;
                temp_str_p++;
            }
        }
        else {
            *current_tile_url_p = *tile_server_p;
            current_tile_url_p++;
        }
        tile_server_p++;
        *current_tile_url_p = '\0';
        if ( strlen( osm_location->current_tile_url ) >= MAX_CURRENT_TILE_URL_LEN ) {
            OSM_MAP_ERROR_LOG("osm_location->current_tile_url: MAX_CURRENT_TILE_URL_LEN reached");
            *osm_location->current_tile_url = '\0';
            break;
        }
        log_d("current url: %s", osm_location->current_tile_url );
    }
    OSM_MAP_LOG("tile server: %s -> %s", osm_location->tile_server, osm_location->current_tile_url );
    /**
     * leave critical section
     */
    osm_map_give( osm_location );
}