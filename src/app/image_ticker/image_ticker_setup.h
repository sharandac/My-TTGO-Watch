/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#ifndef _IMAGE_TICKER_SETUP_H
    #define _IMAGE_TICKER_SETUP_H

    #include <TTGO.h>

    void image_ticker_setup_setup( uint32_t tile_num );
    
    #define DEFAULT_URL "http://www.ingvitoabate.it/img.php?img=https://www.webcambiella.it/images/FI9800P_00626E815CE8/snap/current.jpg"
    //#define DEFAULT_URL "http://www.ingvitoabate.it/organigramma/3.png"

#endif // _IMAGE_TICKER_SETUP_H

