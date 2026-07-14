#ifndef _HARDWARE_H
    #define _HARDWARE_H

    /**
     * @brief hardware init
     */
    void hardware_setup( void );
    /**
     * @brief hardware post init function
     */
    void hardware_post_setup( void );
    /**
     * @brief attach lvgl ticker function
     */
    void hardware_attach_lvgl_ticker( void );
    /**
     * @brief detach lvgl ticker function
     */
    void hardware_attach_lvgl_ticker_slow( void );
    /**
     * @brief detach lvgl ticker function
     */
    void hardware_detach_lvgl_ticker( void );
    /**
     * @brief minimal init for tests that need BMA (watch + bma_setup only)
     */
    void hardware_test_init_bma( void );

#endif // _HARDWARE_H