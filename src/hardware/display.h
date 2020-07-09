#ifndef _DISPLAY_H
    #define _DISPLAY_H

    typedef struct {
        uint32_t brightness = 64;
        uint32_t timeout = 15;
    } display_config_t;

    #define DISPLAY_COFIG_FILE  "/display.cfg"

    void display_setup( TTGOClass *ttgo );
    void display_loop( TTGOClass *ttgo );

    void display_save_config( void );

    uint32_t display_get_timeout( void );
    void display_set_timeout( uint32_t timeout );
    uint32_t display_get_brightness( void );
    void display_set_brightness( uint32_t brightness );

#endif // _DISPLAY_H