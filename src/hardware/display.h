#ifndef _DISPLAY_H
    #define _DISPLAY_H

    #define DISPLAY_MIN_TIMEOUT         15
    #define DISPLAY_MAX_TIMEOUT         300

    #define DISPLAY_MIN_BRIGHTNESS      32
    #define DISPLAY_MAX_BRIGHTNESS      255

    typedef struct {
        uint32_t brightness = DISPLAY_MIN_BRIGHTNESS;
        uint32_t timeout = DISPLAY_MIN_TIMEOUT;
    } display_config_t;

    #define DISPLAY_COFIG_FILE  "/display.cfg"

    /*
     * @brief setup display
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void display_setup( TTGOClass *ttgo );
    /*
     * @brief display loop
     * 
     * @param   ttgo    pointer to an TTGOClass
     */
    void display_loop( TTGOClass *ttgo );
    /*
     * @brief save config for display to spiff
     */
    void display_save_config( void );
    /*
     * @brief read the timeout from config
     * 
     * @return  timeout in seconds
     */
    uint32_t display_get_timeout( void );
    /*
     * @brief set timeout for the display
     * 
     * @param timeout in seconds
     */
    void display_set_timeout( uint32_t timeout );
    /*
     * @brief read the brightness from config
     * 
     * @return  brightness from 0-255
     */
    uint32_t display_get_brightness( void );
    /*
     * @brief set brightness for the display
     * 
     * @param brightness brightness from 0-255
     */
    void display_set_brightness( uint32_t brightness );

#endif // _DISPLAY_H