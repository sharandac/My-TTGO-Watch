#ifndef _OSM_HELPER_H
    #define _OSM_HELPER_H

    #include "utils/uri_load/uri_load.h"
    #ifdef NATIVE_64BIT
        #include "utils/logging.h"
    #else
        #include <Arduino.h>
    #endif

    #define OSM_MAP_LOG                 log_d
    #define OSM_MAP_ERROR_LOG           log_e

    #define MAX_CURRENT_TILE_URL_LEN    256
    #define DEFAULT_OSM_CACHE_SIZE      32
    #define DEFAULT_OSM_TILE_SERVER     "http://a.tile.openstreetmap.org/$z/$x/$y.png"   /** @brief osm tile map server */

    /**
     * @brief osm tile calculation structure
     */
    typedef struct {
        uint32_t zoom = 16;                             /** @brief osm zoom level */
        double lon = 0;                                 /** @brief lon for update calculation*/
        double lat = 0;                                 /** @brief lat for update calculation*/
        uint32_t tilex = 0;                             /** @brief corresponding osm tilex from long */
        uint32_t tiley = 0;                             /** @brief corresponding osm tilex from lat */
        bool manual_nav = false;                        /** @brief flag to inform if we are in manual nav mode */
        bool manual_nav_update = false;                 /** @brief manual nav update flag */
        uint32_t tilex_manual_nav = 0;                  /** @brief current osm tilex from manual nav */
        uint32_t tiley_manual_nav = 0;                  /** @brief current osm tiley from manual nav */
        double tilex_left_top_edge = 0;                 /** @brief max lon position left */
        double tiley_left_top_edge = 0;                 /** @brief max lat position top */
        double tilex_right_bottom_edge = 0;             /** @brief max lon position right */
        double tiley_right_bottom_edge = 0;             /** @brief max lat position bottom */
        double tilex_res = 0;                           /** @brief lon range in degree per tile */
        double tiley_res = 0;                           /** @brief lat range in degree per tile  */
        double tilex_px_res = 0;                        /** @brief lon range in degree per px */
        double tiley_px_res = 0;                        /** @brief lat range in degree per px */
        double tilex_dest_px_res = 256;                 /** @brief tile x resolution in px */
        double tiley_dest_px_res = 256;                 /** @brief tile y resolution in px */
        bool tilexy_pos_valid = false;                  /** @brief indicates if the current lon/lat in view of the current tile image */
        uint16_t tilex_pos = 0;                         /** @brief x location on image in px */
        uint16_t tiley_pos = 0;                         /** @brief y location on image in px */
        bool tile_server_source_update = false;         /** @brief indicates a tile server uri has change */
        char *tile_server = NULL;                       /** @brief the current tile server uri */
        char *current_tile_url = NULL;                  /** @brief the current tile image uri */
        bool load_ahead = false;                        /** @brief enable load ahead feature */
        uint32_t cache_size = 0;
        uint32_t cached_fies = 0;
        lv_img_dsc_t osm_map_data;                      /** @brief pointer to an lv_img_dsc for lvgl use */
        uri_load_dsc_t *uri_load_dsc[ DEFAULT_OSM_CACHE_SIZE ];
#ifndef NATIVE_64BIT
        SemaphoreHandle_t xSemaphoreMutex;
#endif
    } osm_location_t;

    /**
     * @brief the possible move direction in manual nav
     */
    typedef enum {
        north = 0,
        south,
        west,
        east,
        zoom_northwest,
        zoom_northeast,
        zoom_southwest,
        zoom_southeast
    } osm_map_nav_direction_t;

    /**
     * @brief create an osm_location object
     * 
     * @return pointer to osm_location object
     */
    osm_location_t *osm_map_create_location_obj( void );
    /**
     * @brief get the current lv_img_dsc_t of the current position
     * 
     * @param osm_location  pointer to osm_location structure
     *
     * @return lv_img_dsc_t for use with lvgl, NULL mean no tile image available
     */
    lv_img_dsc_t *osm_map_get_tile_image( osm_location_t *osm_location );
    /**
     * @brief get the default 'no data'
     * 
     * @return lv_img_dsc_t for use with lvgl, NULL mean no tile image available
     */
    lv_img_dsc_t *osm_map_get_no_data_image( void );
    /**
     * @brief set a new lon/lat location
     * 
     * @param osm_location  pointer to the osm_location structure
     * @param lon   new long
     * @param lat   new lat
     */
    void osm_map_set_lon_lat( osm_location_t *osm_location, double lon, double lat );
    /**
     * @brief set a new zoom level
     * 
     * @param osm_location  pointer to the osm_location structure
     * @param zoom  new zoom
     */
    void osm_map_set_zoom( osm_location_t *osm_location, uint32_t zoom );
    /**
     * @brief get the current zoom level
     * 
     * @param osm_location  pointer to the osm_location structure
     * 
     * @return zoom level
     */
    uint32_t osm_map_get_zoom( osm_location_t *osm_location );
    /**
     * @brief increase the zoom level by one
     * 
     * @param osm_location  pointer to the osm_location structure
     */
    bool osm_map_zoom_in( osm_location_t *osm_location );
    /**
     * @brief decrease the zoom level by one
     * 
     * @param osm_location  pointer to the osm_location structure
     */
    bool osm_map_zoom_out( osm_location_t *osm_location );
    /**
     * @brief update all infomations and the tile image if required
     * 
     * @param osm_location  pointer to the osm_location structure
     * 
     * @return true if the tile image was updated
     */
    bool osm_map_update( osm_location_t *osm_location );
    /**
     * @brief set a custom tile server
     * 
     * @param tile_server pointer t a tile server uri
     */ 
    void osm_map_set_tile_server( osm_location_t *osm_location, const char* tile_server );
    /**
     * @brief navigate the current tile view one step in a direction
     * 
     * @param osm_location  pointer to the osm_location structure
     * @param direction direction to move
     */
    void osm_map_nav_direction( osm_location_t *osm_location, osm_map_nav_direction_t direction );
    /**
     * @brief center the current view to the current lon/lat location
     * 
     * @param osm_location  pointer to the osm_location structure
     */
    void osm_map_center_location( osm_location_t *osm_location );
    bool osm_map_load_tiles_ahead( osm_location_t *osm_location );
    /**
     * @brief get the numbers of bytes in the cache
     * 
     * @param osm_location  pointer to the osm_location structure
     * 
     * @return number of bytes in the cache
     */
    uint32_t osm_map_get_used_cache_size( osm_location_t *osm_location );
    /**
     * @brief get the numbers of tile image are cached
     * 
     * @param osm_location  pointer to the osm_location structure
     * 
     * @return number of cached tile images
     */
    uint32_t osm_map_get_cache_files( osm_location_t *osm_location );
    /**
     * @brief get the load ahead config flag
     * 
     * @return true if load ahead enable
     */
    bool osm_map_get_load_ahead( osm_location_t *osm_location );
    /**
     * @brief set the load ahead config flag, enabled/disable tile image load ahead
     * 
     * @param pointer pointer to the osm_location_t structure
     */
    void osm_map_set_load_ahead( osm_location_t *osm_location, bool load_ahead );
    /**
     * @brief get the current tile image source uri
     * 
     * @param pointer pointer to the osm_location_t structure
     * 
     * @return pointer the to uri string
     */
    char *osm_map_get_current_uri( osm_location_t *osm_location );
    /**
     * @brief get the current tile image uri
     * 
     * @param pointer pointer to the osm_location_t structure
     * 
     * @return pointer the to uri string
     */
    char *osm_map_get_current_tile_uri( osm_location_t *osm_location );
    void osm_map_clear_cache( osm_location_t *osm_location );

#endif // _OSM_HELPER_H