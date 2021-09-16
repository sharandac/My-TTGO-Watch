#ifndef _WATCHFACE_MANAGER_H
    #define _WATCHFACE_MANAGER_H

    /**
     * @brief setup watchface manager app, setup and watchface tile
     */
    void watchface_manager_setup( void );
    /**
     * @brief get the watchface manager app tile number
     * 
     * @return  watchface manager app tile number
     */
    uint32_t watchface_manager_get_app_tile_num( void );
    /**
     * @brief get the watchface setup tile number
     * 
     * @return  watchface setup tile number
     */
    uint32_t watchface_manager_get_setup_tile_num( void );

#endif // _WATCHFACE_MANAGER_H