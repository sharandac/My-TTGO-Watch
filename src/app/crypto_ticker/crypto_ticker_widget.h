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
#ifndef CRYPTO_TICKER_WIDGET_H
    #define CRYPTO_TICKER_WIDGET_H

    #include <TTGO.h>

    #define CRYPTO_TICKER_WIDGET_SYNC_REQUEST    _BV(0)


    typedef struct {
        bool valide = false;
        time_t timestamp = 0;
        char price[50] = "";
    } crypto_ticker_widget_data_t;

    void crypto_ticker_widget_setup( void );
    void crypto_ticker_hide_widget_icon_info( bool show );

    void crypto_ticker_widget_sync_request( void );

#endif // CRYPTO_TICKER_WIDGET_H