/****************************************************************************
 *   Copyright  2021  Guilhem Bonnefille <guilhem.bonnefille@gmail.com>
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
#include "blehid.h"
#include "blectl.h"

#include "hardware/powermgm.h"
#include "hardware/callback.h"
#include "utils/alloc.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #include <BleKeyboard.h>

    BleKeyboard bleKeyboard;
#endif

static bool blehid_connected = false;
callback_t *blehid_callback = NULL;

static bool blehid_send_event_cb( EventBits_t event, void *arg );
static bool blehid_powermgm_loop_event_cb( EventBits_t event, void *arg );

void blehid_setup( void ) {
#ifdef NATIVE_64BIT

#else
    bleKeyboard.begin();
#endif
    powermgm_register_loop_cb( POWERMGM_WAKEUP | POWERMGM_STANDBY | POWERMGM_SILENCE_WAKEUP, blehid_powermgm_loop_event_cb, "blehid loop cb" );

    blehid_connected = false;
}

static bool blehid_powermgm_loop_event_cb( EventBits_t event, void *arg ) {
#ifdef NATIVE_64BIT

#else
    if( bleKeyboard.isConnected() != blehid_connected ) {
        if( bleKeyboard.isConnected() ) {
            blehid_send_event_cb( BLEHID_CONNECT, NULL );
        }
        else {
            blehid_send_event_cb( BLEHID_DISCONNECT, NULL );
        }
        blehid_connected = bleKeyboard.isConnected();
    }
#endif
    return( true );
}

bool blehid_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( blehid_callback == NULL ) {
        blehid_callback = callback_init( "blehid" );
        ASSERT( blehid_callback, "blehid callback alloc failed" );
    }    
    return( callback_register( blehid_callback, event, callback_func, id ) );
}

static bool blehid_send_event_cb( EventBits_t event, void *arg ) {
    return( callback_send( blehid_callback, event, arg ) );
}
