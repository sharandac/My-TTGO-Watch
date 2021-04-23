#ifndef _OSM_HELPER_H
    #define _OSM_HELPER_H

//    #define OSM_TILE_SERVER     "http://a.tile.openstreetmap.org"
//    #define OSM_TILE_SERVER     "http://b.tile.openstreetmap.org"
    #define OSM_TILE_SERVER     "http://c.tile.openstreetmap.org"

    typedef struct {
        uint32_t zoom = 0;
        bool lonlat_update = false;
        double lat = 0;
        double lon = 0;
        bool tile_update = false;
        uint32_t tilex = 0;
        uint32_t tiley = 0;
    } osm_location_t;

    uint32_t osm_helper_long2tilex(double lon, uint32_t z);
    uint32_t osm_helper_lat2tiley(double lat, uint32_t z);
    double osm_helper_tilex2long(int x, int z);
    double osm_helper_tiley2lat(int y, uint32_t z);
    bool osm_helper_location_update( osm_location_t *osm_location, double lon, double lat, uint32_t zoom );
    lv_img_dsc_t *osm_helper_get_tile_image( osm_location_t *osm_location, lv_img_dsc_t *osm_map_data );
    bool osm_helper_get_tile_image_and_save( osm_location_t *osm_location, char *file_name );

#endif // _OSM_HELPER_H