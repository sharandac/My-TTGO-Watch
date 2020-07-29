/****************************************************************************
 *   Tu May 22 21:23:51 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef _MAINBAR_H
    #define _MAINBAR_H

    #include <TTGO.h>

    typedef void ( * TILE_CALLBACK_FUNC ) ( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );
    
    typedef enum {
        TILE_TYPE_MAIN_TILE,
        TILE_TYPE_APP_TILE,
        TILE_TYPE_NOTE_TILE,
        TILE_TYPE_KEYBOARD_TILE,
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
        KEYBOARD_TILE,
        WLAN_SETTINGS_TILE,
        WLAN_PASSWORD_TILE,
        MOVE_SETTINGS_TILE,
        DISPLAY_SETTINGS_TILE,
        BATTERY_OVERVIEW_TILE,
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

    typedef struct {
        lv_obj_t *tile;
    } lv_tile_t;

    #define MAINBAR_APP_TILE_X_START     0
    #define MAINBAR_APP_TILE_Y_START     4

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
    void mainbar_jump_to_tilenumber( uint32_t tile_number, lv_anim_enable_t anim );
    /*
     * @brief jump direct to main tile
     * @param   anim    LV_ANIM_ON or LV_ANIM_OFF for animated switch
     */
    void mainbar_jump_to_maintile( lv_anim_enable_t anim );

    lv_obj_t * mainbar_get_tile_obj( uint32_t tile_number );
    uint32_t mainbar_add_tile( uint16_t x, uint16_t y );
    lv_style_t *mainbar_get_style( void );
    void mainbar_print_cont( void );
    uint32_t mainbar_add_app_tile( uint16_t x, uint16_t y );

    lv_tile_number mainbar_get_next_free_tile( lv_tile_type tile_type );
    void mainbar_set_tile_setup_cb( lv_tile_number tile_number, TILE_CALLBACK_FUNC callback );


#endif // _MAINBAR_H