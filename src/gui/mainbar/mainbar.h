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

    typedef void ( * MAINBAR_CALLBACK_FUNC ) ( void );

    typedef struct {
        lv_obj_t *tile;
        MAINBAR_CALLBACK_FUNC activate_cb;
        MAINBAR_CALLBACK_FUNC hibernate_cb;
        uint16_t x;
        uint16_t y;
        const char *id;
    } lv_tile_t;

    #define MAINBAR_APP_TILE_X_START     0
    #define MAINBAR_APP_TILE_Y_START     4

    /**
     * @brief mainbar setup funktion
     */
    void mainbar_setup( void );
    /**
     * @brief jump to the given tile
     * 
     * @param   tile    tile number
     * @param   anim    LV_ANIM_ON or LV_ANIM_OFF for animated switch
     */
    void mainbar_jump_to_tilenumber( uint32_t tile_number, lv_anim_enable_t anim );
    /**
     * @brief jump direct to main tile
     * @param   anim    LV_ANIM_ON or LV_ANIM_OFF for animated switch
     */
    void mainbar_jump_to_maintile( lv_anim_enable_t anim );
    /**
     * @brief add a tile at a specific position
     * 
     * @param   x   x position
     * @param   y   y position
     * 
     * @return  tile number
     */
    uint32_t mainbar_add_tile( uint16_t x, uint16_t y, const char *id );
    /**
     * @brief add a independent app tile formation
     * 
     *  predefine tiles
     * 
     *  +---+---+       1 = main tile
     *  | 1 | 2 |       2 = app tile
     *  +---+---+       3 = note tile
     *  | 3 | 4 |       4 = setup tile
     *  +---+---+
     * 
     *  app tile
     * 
     *  +---+---+    +---+   +---+---+
     *  | n |n+1|    | n |   | n |n+1|
     *  +---+---+    +---+   +---+---+
     *  |n+2|n+3|    |n+1|
     *  +---+---+    +---+
     * 
     * @param   x   x size in tiles
     * @param   y   y size in tiles
     * 
     * @return  tile number, if get more than 1 tile it is the first tile number
     */
    uint32_t mainbar_add_app_tile( uint16_t x, uint16_t y, const char *id );
    /**
     * @brief get the lv_obj_t for a specific tile number
     *
     * @param   tile_number   tile number
     * 
     * @return  lv_obj_t
     */
    lv_obj_t * mainbar_get_tile_obj( uint32_t tile_number );
    /**
     * @brief register an hibernate callback function when leave the tile
     * 
     * @param   tile_number     tile number
     * @param   hibernate_cb    pointer to the hibernate callback function
     * 
     * @return  true or false, true means registration was success
     */
    bool mainbar_add_tile_hibernate_cb( uint32_t tile_number, MAINBAR_CALLBACK_FUNC hibernate_cb );
    /**
     * @brief register an activate callback function when enter the tile
     * 
     * @param   tile_number     tile number
     * @param   activate_cb     pointer to the activate callback function
     * 
     * @return  true or false, true means registration was success
     */
    bool mainbar_add_tile_activate_cb( uint32_t tile_number, MAINBAR_CALLBACK_FUNC activate_cb );
    /**
     * @brief get main tile style
     * 
     * @return  pointer to the v_style_t object
     */
    lv_style_t *mainbar_get_style( void );
    /**
     * @brief get main tile switch style
     * 
     * @return  pointer to the v_style_t object
     */
    lv_style_t *mainbar_get_switch_style( void );
    /**
     * @brief get main tile slider style
     * 
     * @return  pointer to the v_style_t object
     */
    lv_style_t *mainbar_get_slider_style( void );
    /**
     * @brief get main tile button style
     * 
     * @return  pointer to the v_style_t object
     */
    lv_style_t *mainbar_get_button_style( void );
    /**
     * @brief
     */
    lv_obj_t *mainbar_obj_create( lv_obj_t *parent );
    /**
     * @brief
     */
    void mainbar_add_slide_element( lv_obj_t *element );

#endif // _MAINBAR_H