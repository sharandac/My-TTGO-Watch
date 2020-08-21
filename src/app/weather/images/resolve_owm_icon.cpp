/****************************************************************************
 *   Thu July 23 12:17:01 2020
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
#include <TTGO.h>

#include "resolve_owm_icon.h"

LV_IMG_DECLARE(owm_01d_64px);
LV_IMG_DECLARE(owm_02d_64px);
LV_IMG_DECLARE(owm_03d_64px);
LV_IMG_DECLARE(owm_04d_64px);
LV_IMG_DECLARE(owm_09d_64px);
LV_IMG_DECLARE(owm_10d_64px);
LV_IMG_DECLARE(owm_11d_64px);
LV_IMG_DECLARE(owm_13d_64px);
LV_IMG_DECLARE(owm_50d_64px);
LV_IMG_DECLARE(owm_01n_64px);
LV_IMG_DECLARE(owm_02n_64px);
LV_IMG_DECLARE(owm_03n_64px);
LV_IMG_DECLARE(owm_04n_64px);
LV_IMG_DECLARE(owm_09n_64px);
LV_IMG_DECLARE(owm_10n_64px);
LV_IMG_DECLARE(owm_11n_64px);
LV_IMG_DECLARE(owm_13n_64px);
LV_IMG_DECLARE(owm_50n_64px);

struct owm_icon owm_icon[ 18 ] = {
    { "01d", &owm_01d_64px },
    { "02d", &owm_02d_64px },
    { "03d", &owm_03d_64px },
    { "04d", &owm_04d_64px },
    { "09d", &owm_09d_64px },
    { "10d", &owm_10d_64px },
    { "11d", &owm_11d_64px },
    { "12d", &owm_13d_64px },
    { "50d", &owm_50d_64px },
    { "01n", &owm_01n_64px },
    { "02n", &owm_02n_64px },
    { "03n", &owm_03n_64px },
    { "04n", &owm_04n_64px },
    { "09n", &owm_09n_64px },
    { "10n", &owm_10n_64px },
    { "11n", &owm_11n_64px },
    { "12n", &owm_13n_64px },
    { "50n", &owm_50n_64px }
};

const void * resolve_owm_icon( char *iconname ) {
    for ( uint32_t icon = 0 ; icon < 18 ; icon++ ) {
        if ( !strcmp( (const char*)owm_icon[ icon ].iconname , (const char*)iconname ) ){
            return( owm_icon[ icon ].icon );
        }
    }
    return( &owm_01d_64px );
}

