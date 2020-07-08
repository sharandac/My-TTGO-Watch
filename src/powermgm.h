#ifndef _POWERMGM_H
    #define _POWERMGM_H

    #define POWERMGM_STANDBY            _BV(0)
    #define POWERMGM_PMU_BUTTON         _BV(1)
    #define POWERMGM_PMU_BATTERY        _BV(2)
    #define POWERMGM_BMA_WAKEUP         _BV(3)
    #define POWERMGM_WIFI_ON_REQUEST    _BV(5)
    #define POWERMGM_WIFI_OFF_REQUEST   _BV(6)
    #define POWERMGM_WIFI_ACTIVE        _BV(7)
    #define POWERMGM_WIFI_SCAN          _BV(8)
    #define POWERMGM_WIFI_CONNECTED     _BV(9)

    /*
     * @brief setp power managment, coordinate managment beween CPU, wifictl, pmu, bma, display, backlight and lvgl
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void powermgm_setup( TTGOClass *ttgo );
    /*
     * @brief power managment loop routine, call from loop. not for user use
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void powermgm_loop( TTGOClass *ttgo );
    /*
     * @brief trigger a power managemt event
     * 
     * @param   bits    event to trigger, example: POWERMGM_WIFI_ON_REQUEST for switch an WiFi
     */
    void powermgm_set_event( EventBits_t bits );
    /*
     * @brief clear a power managemt event
     * 
     * @param   bits    event to trigger, example: POWERMGM_WIFI_ON_REQUEST for switch an WiFi
     */
    void powermgm_clear_event( EventBits_t bits );
    /*
     * @brief get a power managemt event state
     * 
     * @param   bits    event state, example: POWERMGM_STANDBY to evaluate if the system in standby
     */
    EventBits_t powermgm_get_event( EventBits_t bits );

#endif // _POWERMGM_H