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
#ifndef _KEYBOARD_H
    #define _KEYBOARD_H

    #include <TTGO.h>

    /**
     * @brief setup onscreen keyboard
     */
    void keyboard_setup( void );
    void num_keyboard_setup( void );
    /**
     * @brief   activate onscreen keyboard and set output to an lv_obj aka textarea
     * 
     * @param   textarea    point to an lv_obj
     */
    void keyboard_set_textarea( lv_obj_t *textarea );
    void num_keyboard_set_textarea( lv_obj_t *textarea );
    /**
     * @brief   hide onscreen keyboard
     */
    void keyboard_hide( void );
    /**
     * @brief   show onscreen keyboard
     */
    void keyboard_show( void );
    void num_keyboard_show( void );

#endif // _KEYBOARD_H
