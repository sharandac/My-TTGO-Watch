#ifndef _OSM_HELPER_H
    #define _OSM_HELPER_H

//    #define OSM_TILE_SERVER     "http://a.tile.openstreetmap.org"   /** @brief osm tile map server */
//    #define OSM_TILE_SERVER     "http://b.tile.openstreetmap.org"   /** @brief osm tile map server */
    #define OSM_TILE_SERVER     "http://c.tile.openstreetmap.org"   /** @brief osm tile map server */

    /**
     * @brief osm tile calculation structure
     */
    typedef struct {
        bool zoom_valid = false;        /** @brief osm zoom level */
        uint32_t zoom = 0;              /** @brief osm zoom level */
        bool lonlat_update = false;     /** @brief lon/lat update flag */
        double lon = 0;                 /** @brief lon for update calculation*/
        double lat = 0;                 /** @brief lat for update calculation*/
        bool tile_update = false;       /** @brief tile update flag */
        uint32_t tilex = 0;             /** @brief corresponding osm tilex from long */
        uint32_t tiley = 0;             /** @brief corresponding osm tilex from lat */
        double tilex_left_top_edge = 0;
        double tiley_left_top_edge = 0;
        double tilex_right_bottom_edge = 0;
        double tiley_right_bottom_edge = 0;
        double tilex_res = 0;
        double tiley_res = 0;
        double tilex_px_res = 0;
        double tiley_px_res = 0;
        double tilex_dest_px_res = 240;
        double tiley_dest_px_res = 240;
        uint16_t tilex_pos = 0;
        uint16_t tiley_pos = 0;
    } osm_location_t;

    /**
     * @brief get osm tilex from long and zoom
     * 
     * @param lon   long
     * @param zoom  zoom
     *
     * @return tilex number
     */
    uint32_t osm_helper_long2tilex(double lon, uint32_t z);
    /**
     * @brief get osm tiley from lat and zoom
     * 
     * @param lon   lat
     * @param zoom  zoom
     *
     * @return tiley number
     */
    uint32_t osm_helper_lat2tiley(double lat, uint32_t z);
    /**
     * @brief get lat from osm tiley number and zoom
     * 
     * @param lon   lat
     * @param zoom  zoom
     *
     * @return lat
     */
    double osm_helper_tilex2long(int x, uint32_t z);
    /**
     * @brief get long from osm tilex number and zoom
     * 
     * @param lon   long
     * @param zoom  zoom
     *
     * @return long
     */
    double osm_helper_tiley2lat(int y, uint32_t z);
    /**
     * @brief check for an location update if a new tile require
     * 
     * @param osm_location  pointer to the osm_location structure
     * @param lon   new long
     * @param lat   new lat
     * @param zoom  new zoom
     * 
     * @return  true if an tile update require
     */
    bool osm_helper_location_update( osm_location_t *osm_location, double lon, double lat, uint32_t zoom );
    /**
     * @brief get an new tile from osm an store it in a lv_img_dsc structure for direct lv_img_set_src use
     * 
     * @param osm_location  pointer to the osm_location structure
     * @param osm_map_data  pointer to an lv_img_sdc structure or NULL for a new lv_img_dsc structure
     * 
     * @return  updated lv_img_dsc structure
     */
    lv_img_dsc_t *osm_helper_get_tile_image( osm_location_t *osm_location, lv_img_dsc_t *osm_map_data );
    /**
     * @brief get an new tile from osm an store it in a lv_img_dsc structure for direct lv_img_set_src use
     * 
     * @param osm_location  pointer to the osm_location structure
     * @param file_name     pointer to a file name like "/osn.png"
     * 
     * @return  true if file download was success
     */
    bool osm_helper_get_tile_image_and_save( osm_location_t *osm_location, char *file_name );

#endif // _OSM_HELPER_H