#ifndef _MAINBAR_H
    #define _MAINBAR_H

    #include <TTGO.h>

    typedef void ( * TILE_CALLBACK_FUNC ) ( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );
    
    typedef enum {
        MAIN_TILE,
        SETUP_TILE,
        NOTE_TILE,
        APP_TILE,
        WLAN_SETTINGS_TILE,
        WLAN_PASSWORD_TILE,
        MOVE_SETTINGS_TILE,
        DISPLAY_SETTINGS_TILE,
        BATTERY_SETTINGS_TILE,
        TIME_SETTINGS_TILE,
        TILE_NUM
    } lv_tile_number;

    typedef struct {
        lv_obj_t *tile;
        lv_tile_number tile_number;
        TILE_CALLBACK_FUNC tilecallback;
        lv_point_t pos;
    } lv_tile_entry_t;

    /*
     * @brief mainbar setup funktion
     * @param none
     */
    void mainbar_setup( void );
    /*
     * @brief jump to the given tile
     * @param   x   x coordinate
     * @param   y   y coordinate
     */
    void mainbar_jump_to_tile( lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim );
    /*
     * @brief jump to the given tile
     * @param   tile    tile number
     * @param   anim    LV_ANIM_ON or LV_ANIM_OFF for animated switch
     */
    void mainbar_jump_to_tilenumber( lv_tile_number tile_number, lv_anim_enable_t anim );
    /*
     * @brief jump direct to main tile
     * @param   anim    LV_ANIM_ON or LV_ANIM_OFF for animated switch
     */
    void mainbar_jump_to_maintile( lv_anim_enable_t anim );


#endif // _MAINBAR_H