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

#include "crypto_ticker_widget.h" //TODO: why is this needed
#include "crypto_ticker_main.h"   //TODO: why is this needed

#ifndef _CRYPTO_TICKER_FETCH_H
    #define _CRYPTO_TICKER_FETCH_H

    #define MY_TTGO_WATCH_HOST    "my-ttgo-watch.co.uk"

    int crypto_ticker_fetch_price( crypto_ticker_config_t * crypto_ticker_config, crypto_ticker_widget_data_t * crypto_ticker_today );
    int crypto_ticker_fetch_statistics( crypto_ticker_config_t *crypto_ticker_config, crypto_ticker_main_data_t *crypto_ticker_main_data );

#endif // _CRYPTO_TICKER_FETCH_H