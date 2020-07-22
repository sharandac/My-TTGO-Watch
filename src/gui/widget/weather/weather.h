#ifndef _WEATHER_H
    #define _WEATHER_H

    #include <TTGO.h>

    void weather_widget_setup( void );

    void weather_widget_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );

    void weather_widget_setup_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );
    
#endif // _WEATHER_H
