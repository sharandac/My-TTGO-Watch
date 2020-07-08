#ifndef _KEYBOARD_H
    #define _KEYBOARD_H

    #include <TTGO.h>

    /*
     * @brief setup onscreen keyboard
     */
    void keyboard_setup( void );
    /*
     * @brief activate onscreen keyboard and set output to an lv_obj aka textarea
     * 
     * @ param  textarea    point to an lv_obj
     */
    void keyboard_set_textarea( lv_obj_t *textarea );
    /*
     *  @brief hide onscreen keyboard
     */
    void keyboard_hide( void );
    /*
     *  @brief show onscreen keyboard
     */
    void keyboard_show( void );

#endif // _KEYBOARD_H