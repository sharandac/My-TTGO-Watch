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
#include "config.h"
#include "gps_logger.h"
#include "hardware/gpsctl.h"
#include "utils/alloc.h"

gps_logger_t *gps_logger_init( void ) {
    gps_logger_t *gps_logger = (gps_logger_t*)MALLOC( sizeof( gps_logger_t) );
    
    if( gps_logger ) {
        gps_logger->fix = false;
        gps_logger->lon = 0.0;
        gps_logger->lat = 0.0;
        gps_logger->altitude = 0.0;
        gps_logger->filename = NULL;
    }

    return( gps_logger );
}

void gps_logger_set_filename( gps_logger_t *gps_logger, const char *filename ) {
    char fname[128] = "";
    time_t now;
    struct tm info;
    /*
     * copy current time into now and convert it local time info
     */
    time( &now );
    localtime_r( &now, &info );
    
    if( !gps_logger )
        return;

    if( !filename ) {
        strftime( fname, sizeof( fname ), "%y-%m-%d-%H%M%S", &info );
        filename = (const char*)&fname;
    }

    gps_logger->filename = (char *)MALLOC( strlen( filename ) + 1 );
    if( gps_logger->filename )
        strncpy( gps_logger->filename, filename, strlen( gps_logger->filename ) );
    else {
        while( 1 ){};
    }

}
