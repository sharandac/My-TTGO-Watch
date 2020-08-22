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

#include "crypto_ticker_widget.h"

#ifndef _crypto_ticker_FETCH_H
    #define _crypto_ticker_FETCH_H

    #define OWM_HOST    "api.binance.com"
    #define OWM_PORT    443

    #define crypto_ticker_PRICE_BUFFER_SIZE       10000

    int crypto_ticker_fetch_today( crypto_ticker_config_t * crypto_ticker_config, crypto_ticker_widget_data_t * crypto_ticker_today );

#endif // _crypto_ticker_FETCH_H