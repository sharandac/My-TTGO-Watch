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
#ifndef _SPLASHSCREEN_H
    #define _SPLASHSCREEN_H

    #define SPLASHSCREENLOGO    "/spiffs/logo.png"

    /**
     * @brief start splashscreen
     */
    void splash_screen_stage_one( void );
    /**
     * @brief update spash screen text and bar
     * 
     * @param   msg   splash screen text
     * @param   value splash screen bar value (0-100)
     */
    void splash_screen_stage_update( const char* msg, int value );
    /**
     * @brief finish splashscreen
     */
    void splash_screen_stage_finish( void );

#endif // _SPLASHSCREEN_H