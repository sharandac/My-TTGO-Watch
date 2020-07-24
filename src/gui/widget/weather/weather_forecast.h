#ifndef _WEATHER_FORECAST_H
    #define _WEATHER_FORECAST_H

    #include <TTGO.h>

    #define WEATHER_FORECAST_SYNC_REQUEST    _BV(0)
    #define WEATHER_MAX_FORECAST            16

    void weather_widget_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );
    void weather_forecast_sync_request( void );

#endif // _WEATHER_FORECAST_H