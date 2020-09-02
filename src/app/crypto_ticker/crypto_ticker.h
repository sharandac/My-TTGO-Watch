/****************************************************************************
 *   Aug 22 16:36:11 2020
 *   Copyright  2020  Chris McNamee
 *   Email: chris.mcna@gmail.com
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
#ifndef _CRYPTO_TICKER_H
    #define _CRYPTO_TICKER_H

    #include <TTGO.h>

//    #define CRYPTO_TICKER_WIDGET    // uncomment if an widget need, comment to hide

    #define crypto_ticker_JSON_CONFIG_FILE        "/crypto-ticker.json"

    typedef struct {
            char symbol[10] = "";
            bool autosync = true;
        } crypto_ticker_config_t;


    void crypto_ticker_setup( void );
    crypto_ticker_config_t *crypto_ticker_get_config( void );
    void crypto_ticker_hide_app_icon_info( bool show );
    void crypto_ticker_hide_widget_icon_info( bool show );
    uint32_t crypto_ticker_get_app_setup_tile_num( void );
    uint32_t crypto_ticker_get_app_main_tile_num( void );
    void crypto_ticker_jump_to_main( void ) ;
    void crypto_ticker_jump_to_setup( void );

    void crypto_ticker_save_config( void );

#endif // _CRYPTO_TICKER_H