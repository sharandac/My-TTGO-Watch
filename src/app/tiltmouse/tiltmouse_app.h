/****************************************************************************
 *   June 04 02:01:00 2021
 *   Copyright  2021  Dirk Sarodnick
 *   Email: programmer@dirk-sarodnick.de
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
#ifndef _TILTMOUSE_APP_H
    #define _TILTMOUSE_APP_H

    #include <TTGO.h>

    void tiltmouse_app_setup( void );
    uint32_t tiltmouse_app_get_app_main_tile_num( void );
    
    void enter_tiltmouse_app_event_cb( lv_obj_t * obj, lv_event_t event );
    void exit_tiltmouse_app_event_cb( lv_obj_t * obj, lv_event_t event );

#endif // _TILTMOUSE_APP_H