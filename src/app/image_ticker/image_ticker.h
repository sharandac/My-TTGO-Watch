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
#ifndef _IMAGE_TICKER_H
    #define _IMAGE_TICKER_H

    #include <TTGO.h>

    #define image_ticker_JSON_CONFIG_FILE        "/image-ticker.json"

    void image_ticker_setup( void );
    uint32_t image_ticker_get_app_setup_tile_num( void );
    uint32_t image_ticker_get_app_main_tile_num( void );

    typedef struct {
            char url[250] = "";
            bool autosync = true;
        } image_ticker_config_t;

    image_ticker_config_t *image_ticker_get_config( void );
    void image_ticker_load_config( void );
    void image_ticker_save_config( void );

//    #define IMAGE_TICKER_WIDGET    // uncomment if an widget need

#endif // _IMAGE_TICKER_H


