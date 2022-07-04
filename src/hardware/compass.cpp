/****************************************************************************
 *   Mo July 4 21:17:51 2022
 *   Copyright  2022  Dirk Brosswick
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
#include "compass.h"
#include "powermgm.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #if defined( M5PAPER )
        #include <M5EPD.h>
    #elif defined( M5CORE2 )
        #include <M5Core2.h>
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <TTGO.h>
    #elif defined( LILYGO_WATCH_2021 )
        #include <twatch2021_config.h>
        #include <QMC5883LCompass.h>
        QMC5883LCompass compass;
    #else
        #error "no hardware driver for display"
    #endif
#endif

static bool compass_powermgm_event_cb( EventBits_t event, void *arg );
static bool compass_powermgm_loop_event_cb( EventBits_t event, void *arg );

void compass_setup( void ) {
    #if defined( M5PAPER )
    #elif defined( M5CORE2 )
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #elif defined( LILYGO_WATCH_2021 )
        compass.init();
    #else
    #endif

    powermgm_register_loop_cb( POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, compass_powermgm_event_cb, "compass powermgm event" );
    powermgm_register_cb( POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, compass_powermgm_loop_event_cb, "compass powermgm loop event" );
}

static bool compass_powermgm_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_STANDBY:
            break;
        case POWERMGM_WAKEUP:
            break;
        case POWERMGM_SILENCE_WAKEUP:
            break;
        default:
            break;
    }

    return( true );
}

static bool compass_powermgm_loop_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case POWERMGM_STANDBY:
            break;
        case POWERMGM_WAKEUP:
            break;
        case POWERMGM_SILENCE_WAKEUP:
            break;
        default:
            break;
    }

    return( true );
}

bool compass_enable( void ) {
    bool retval = false;
    
    #if defined( M5PAPER )
    #elif defined( M5CORE2 )
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #elif defined( LILYGO_WATCH_2021 )
        retval = true;
    #else
    #endif

    return( retval );
}