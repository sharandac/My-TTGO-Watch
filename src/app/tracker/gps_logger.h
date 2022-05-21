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
#ifndef _TRACKER_UTIL_LOGER_H
    #define _TRACKER_UTIL_LOGER_H

    typedef struct gps_logger_t {
        bool fix = true;
        double lon = 0.0;
        double lat = 0.0;
        double altitude = 0.0;
        char *filename = NULL;
    } gps_logger_t;

    /**
     * @brief init gps logger structure
     * 
     * @return pointer to a new gps logger structure
     */
    gps_logger_t *gps_logger_init( void );
    void gps_logger_set_filename( gps_logger_t *gps_logger, const char *filename );

#endif // _TRACKER_UTIL_LOGER_H