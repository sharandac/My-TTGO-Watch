#ifndef _MAINBAR_H
    #define _MAINBAR_H

    #include <TTGO.h>

    typedef void ( * TILE_CALLBACK_FUNC ) ( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );
    
    typedef enum {
        TILE_TYPE_MAIN_TILE,
        TILE_TYPE_APP_TILE,
        TILE_TYPE_NOTE_TILE,
        TILE_TYPE_SETUP_TILE,
        TILE_TYPE_SETUP,
        TILE_TYPE_WIDGET_TILE,
        TILE_TYPE_WIDGET_SETUP,
        TILE_TYPE_NUM
    } lv_tile_type;
    
    typedef enum {
        NO_TILE = -1,
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
        UPDATE_SETTINGS_TILE,
        WIDGET1_1_TILE,
        WIDGET1_2_TILE,
        WIDGET2_1_TILE,
        WIDGET2_2_TILE,
        TILE_NUM
    } lv_tile_number;

    typedef struct {
        lv_obj_t *tile;
        lv_tile_type tile_type;
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

    lv_obj_t * mainbar_get_tile_obj( lv_tile_number tile_number );
    lv_tile_number mainbar_get_next_free_tile( lv_tile_type tile_type );
    void mainbar_set_tile_setup_cb( lv_tile_number tile_number, TILE_CALLBACK_FUNC callback );


#endif // _MAINBAR_H