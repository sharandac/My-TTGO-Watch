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
#ifndef _CALENDAR_CREATE_H
    #define _CALENDAR_CREATE_H

    #
    #define CALENDAR_CREATE_INFO_LOG               log_i
    #define CALENDAR_CREATE_DEBUG_LOG              log_d
    #define CALENDAR_CREATE_ERROR_LOG              log_e
    /**
     * @brief setup calendar create tile
     */
    void calendar_create_setup( void );
    /**
     * @brief get calendar overview tile number
     * 
     * @return  calendar overview tile number
     */
    uint32_t calendar_create_get_tile( void );
    /**
     * @brief set year, month and day to create a date entry
     * 
     * @param year      create year
     * @param month     create month
     * @param day       create day
     */
    void calendar_create_set_date( int year, int month, int day );
    /**
     * @brief set hour and min to create a date entry
     * 
     * @param year      create year
     * @param month     create month
     * @param day       create day
     */
    void calendar_create_set_time( int hour, int min );
    /**
     * @brief set the content is a date is exist on given year, month, day, hour and min
     */
    void calendar_create_set_content( void );
    /**
     * @brief clear content
     */
    void calendar_create_clear_content( void );

#endif // _CALENDAR_CREATE_H