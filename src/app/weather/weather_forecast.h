#ifndef _WEATHER_FORECAST_H
    #define _WEATHER_FORECAST_H

    #define WEATHER_FORECAST_SYNC_REQUEST   _BV(0)
    /**
     * @brief calculate weather icon aligning
     */
    #define WEATHER_ICON_SIZE               64                                                                      /** @brief weather icon x/y size in px */
    #define WEATHER_MAX_FORECAST_ICON       RES_X_MAX / WEATHER_ICON_SIZE                                           /** @brief max icon in a row */
    #define WEATHER_FORCAST_ICON_SPACE      ( RES_X_MAX % WEATHER_ICON_SIZE ) / ( RES_X_MAX / WEATHER_ICON_SIZE )   /** @brief space between two icons in px */
    #define WEATHER_MAX_FORECAST            WEATHER_MAX_FORECAST_ICON * 2

    void weather_forecast_tile_setup( uint32_t tile_num );
    void weather_forecast_sync( void );

#endif // _WEATHER_FORECAST_H