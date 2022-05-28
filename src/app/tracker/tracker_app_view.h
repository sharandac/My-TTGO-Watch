/****************************************************************************
 *   Aug 11 17:13:51 2020
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
#ifndef _TRACKER_APP_VIEW_H
    #define _TRACKER_APP_VIEW_H

    #include "config.h"
    #include "hardware/gpsctl.h"
    /**
     * @brief tracker main setup function
     */
    void tracker_app_view_setup( uint32_t tile );
    void tracker_app_view_add_data( gps_data_t *gps_data );
    void tracker_app_view_clean_data( void );

#endif // _TRACKER_APP_VIEW_H