#ifndef _WEATHER_FORECAST_H
    #define _WEATHER_FORECAST_H

    #define WEATHER_FORECAST_SYNC_REQUEST   _BV(0)
    #define WEATHER_MAX_FORECAST            16

    #if defined( M5PAPER )
        #define WEATHER_MAX_FORECAST_DIV        2
        #define WEATHER_FORCAST_ICON_SPACE      9
    #else
        #define WEATHER_MAX_FORECAST_DIV        4
        #define WEATHER_FORCAST_ICON_SPACE      -4
    #endif

    void weather_forecast_tile_setup( uint32_t tile_num );
    void weather_forecast_sync_request( void );

#endif // _WEATHER_FORECAST_H