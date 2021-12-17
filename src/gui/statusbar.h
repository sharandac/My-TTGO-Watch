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
#ifndef _STATUSBAR_H

    #define _STATUSBAR_H

    #include "config.h"

    #if defined( BIG_THEME )
        #define STATUSBAR_HEIGHT                52
        #define STATUSBAR_EXPAND_HEIGHT         300
    #elif defined ( MID_THEME )
        #define STATUSBAR_HEIGHT                26
        #define STATUSBAR_EXPAND_HEIGHT         200
    #else
        #define STATUSBAR_HEIGHT                26
        #define STATUSBAR_EXPAND_HEIGHT         200
    #endif

    /**
     * @brief statusbar item config stucture
     */
    typedef struct {
        lv_obj_t *icon;
        const void *symbol;
        lv_align_t align;
        lv_style_t *style;
    } lv_status_bar_t;

    /**
     * @brief statusbar icon set
     */
    typedef enum {
        STATUSBAR_BATTERY_PERCENT = 0,
        STATUSBAR_BATTERY,
        STATUSBAR_BLUETOOTH,
        STATUSBAR_WIFI,
        STATUSBAR_VOLUME,
        STATUSBAR_GPS,
        STATUSBAR_BELL,
        STATUSBAR_WARNING,
        STATUSBAR_ALARM,
        STATUSBAR_NUM
    } statusbar_icon_t;
    
    /**
     * @brief statusbar style set
     */
    typedef enum {
        STATUSBAR_STYLE_NORMAL = 0,
        STATUSBAR_STYLE_BLACK,
        STATUSBAR_STYLE_WHITE,
        STATUSBAR_STYLE_RED,
        STATUSBAR_STYLE_GRAY,
        STATUSBAR_STYLE_YELLOW,
        STATUSBAR_STYLE_GREEN,
        STATUSBAR_STYLE_BLUE,
        STATUSBAR_STYLE_NUM
    } statusbar_style_t;

    /**
     * @brief setup statusbar
     */
    void statusbar_setup( void );
    /**
     * @brief hide an icon from statusbar
     * 
     * @param icon  icon name
     */
    void statusbar_hide_icon( statusbar_icon_t icon );
    /**
     * @brief show an icon from statusbar
     * 
     * @param icon  icon name
     */
    void statusbar_show_icon( statusbar_icon_t icon );
    /**
     * @brief hide an icon from statusbar
     * 
     * @param icon  icon name
     * @param style style name
     */
    void statusbar_style_icon( statusbar_icon_t icon, statusbar_style_t style );
    /**
     * @brief refresh/redraw statusbar
     */
    void statusbar_refresh( void );
    /**
     * @brief hide the statusbar
     * 
     * @param   hide    true or false, ture means statusbar is hide
     */
    void statusbar_hide( bool hide );
    /**
     * @brief expand the statusbar
     * 
     * @param   expand  true if expand of false if not
     */
    void statusbar_expand( bool expand );
    /**
     * @brief force dark mode
     * 
     * @param force_dark_mode   true to force dark mode
     */
    void statusbar_set_force_dark( bool dark_mode );
    /**
     * @brief get force dark mode status
     * 
     * @return  true if dark mode forced
     */
    bool statusbar_get_force_dark( void );
    /**
     * @brief geth the current hidden state
     * 
     * @return true if hidden or false is visible
     */
    bool statusbar_get_hidden_state( void );

#endif // _STATUSBAR_H

