/****************************************************************************
 *   Su Jan 17 23:05:51 2021
 *   Copyright  2021  Dirk Brosswick
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
#ifndef _QUICKBAR_H
    #define _QUICKBAR_H

    #include "config.h"

    /**
     * @brief quickbar style sets
     */
    typedef enum {
        QUICKBAR_STYLE_NORMAL,
        QUICKBAR_STYLE_LIGHT,
        QUICKBAR_STYLE_DARK,
        QUICKBAR_STYLE_TRANS,
        QUICKBAR_STYLE_NUM
    } quickbar_style_t;
    /**
     * @brief setup quickbar
     * 
     */
    void quickbar_setup( void );
    /**
     * @brief hide or show the quickbar
     * 
     * @param hide  TRUE hide the quickbar and FALSE show the quickbar
     */
    void quickbar_hide( bool hide );

#endif // _QUICKBAR_H