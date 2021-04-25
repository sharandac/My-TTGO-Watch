#ifndef _OSM_HELPER_H
    #define _OSM_HELPER_H

    #define DEFAULT_OSM_TILE_SERVER     "http://c.tile.openstreetmap.org"   /** @brief osm tile map server */

    /**
     * @brief osm tile calculation structure
     */
    typedef struct {
        uint32_t zoom = 16;                     /** @brief osm zoom level */
        double lon = 0;                         /** @brief lon for update calculation*/
        double lat = 0;                         /** @brief lat for update calculation*/
        uint32_t tilex = 0;                     /** @brief corresponding osm tilex from long */
        uint32_t tiley = 0;                     /** @brief corresponding osm tilex from lat */
        double tilex_left_top_edge = 0;         /** @brief max lon position left */
        double tiley_left_top_edge = 0;         /** @brief max lat position top */
        double tilex_right_bottom_edge = 0;     /** @brief max lon position right */
        double tiley_right_bottom_edge = 0;     /** @brief max lat position bottom */
        double tilex_res = 0;                   /** @brief lon range in degree per tile */
        double tiley_res = 0;                   /** @brief lat range in degree per tile  */
        double tilex_px_res = 0;                /** @brief lon range in degree per px */
        double tiley_px_res = 0;                /** @brief lat range in degree per px */
        double tilex_dest_px_res = 256;         /** @brief tile x resolution in px */
        double tiley_dest_px_res = 256;         /** @brief tile y resolution in px */
        uint16_t tilex_pos = 0;                 /** @brief x location on image in px */
        uint16_t tiley_pos = 0;                 /** @brief y location on image in px */
        lv_img_dsc_t osm_map_data;              /** @brief pointer to an lv_img_dsc for lvgl use */
    } osm_location_t;

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
    void osm_map_zoom_in( osm_location_t *osm_location );
    /**
     * @brief decrease the zoom level by one
     * 
     * @param osm_location  pointer to the osm_location structure
     */
    void osm_map_zoom_out( osm_location_t *osm_location );
    /**
     * @brief update all infomations and the tile image if required
     * 
     * @param osm_location  pointer to the osm_location structure
     * 
     * @return true if the tile image was updated
     */
    bool osm_map_update( osm_location_t *osm_location );

#endif // _OSM_HELPER_H