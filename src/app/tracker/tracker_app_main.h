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
#ifndef _TRACKER_APP_MAIN_H
    #define _TRACKER_APP_MAIN_H

    #include "config.h"

    /**
     * @brief gpx file definitions
     */
    #define     GPX_LOGFILE                         "/%Y-%m-%d-%H%M%S.gpx"
    #define     GPX_HEADER                          "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\r\n"
    #define     GPX_START                           "<gpx version=\"1.1\" creator=\"gps tracker\">\r\n"
    #define     GPX_METADATA                        " <metadata> <!-- Metadaten --> </metadata>\r\n"
    #define     GPX_TRACK_START                     " <trk>\r\n"
    #define     GPX_TRACK_SEGMENT_START             "  <trkseg>\r\n"
    #define     GPX_TRACK_SEGMENT_POINT_START       "   <trkpt lat=\"%f\" lon=\"%f\">\r\n"
    #define     GPX_TRACK_SEGMENT_POINT_ELE         "    <ele>%f</ele>\r\n"
    #define     GPX_TRACK_SEGMENT_POINT_TIME_SRF    "%Y-%m-%dT%H:%M:%SZ"
    #define     GPX_TRACK_SEGMENT_POINT_TIME        "    <time>%s</time>\r\n"
    #define     GPX_TRACK_SEGMENT_POINT_END         "   </trkpt>\r\n"
    #define     GPX_TRACK_SEGMENT_END               "  </trkseg>\r\n"
    #define     GPX_TRACK_END                       " </trk>\r\n"
    #define     GPX_END                             "</gpx>\r\n"
    /**
     * @brief tracker main setup function
     */
    void tracker_app_main_setup( uint32_t tile );

#endif // _TRACKER_APP_MAIN_H