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
#ifndef _CALENDAR_DAY_H
    #define _CALENDAR_DAY_H

    #
    #define CALENDAR_DAY_INFO_LOG               log_i
    #define CALENDAR_DAY_DEBUG_LOG              log_d
    #define CALENDAR_DAY_ERROR_LOG              log_e
    /**
     * @brief setup calendar overview tile
     */
    void calendar_day_setup();
    /**
     * @brief get calendar overview tile number
     * 
     * @return  calendar overview tile number
     */
    uint32_t calendar_day_get_tile( void );
    void calendar_day_overview_refresh( int year, int month, int day );

#endif // _CALENDAR_DAY_H