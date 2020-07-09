#ifndef _SPLASHSCREEN_H
    #define _SPLASHSCREEN_H

    /*
     * @brief start splashscreen
     * 
     * @param ttgo  pointer to TTGOClass
     */
    void splash_screen_stage_one( TTGOClass *ttgo );
    /*
     * @brief update spash screen text and bar
     * 
     * @param   msg   splash screen text
     * @param   value splash screen bar value (0-100)
     */
    void splash_screen_stage_update( const char* msg, int value );
    /*
     * @brief finish splashscreen
     * 
     * @param   ttgo    pointer to TTGOClass
     */
    void splash_screen_stage_finish( TTGOClass *ttgo );

#endif // _SPLASHSCREEN_H