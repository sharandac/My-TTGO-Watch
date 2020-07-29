#ifndef _WEATHER_FORECAST_H
    #define _WEATHER_FORECAST_H

    #include <TTGO.h>

    #define WEATHER_FORECAST_SYNC_REQUEST   _BV(0)
    #define WEATHER_MAX_FORECAST            16

    void weather_forecast_tile_setup( uint32_t tile_num );
    void weather_forecast_sync_request( void );

#endif // _WEATHER_FORECAST_H