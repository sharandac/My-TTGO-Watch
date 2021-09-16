/****************************************************************************
 *   Tu May 22 21:23:51 2020
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
#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #include "esp_bt.h"
    #include "esp_task_wdt.h"
    #include "sdcard.h"
#endif

#include "powermgm.h"

bool sdcard_powermgm_event_cb( EventBits_t event, void *arg );

void sdcard_setup( void ) {
    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, sdcard_powermgm_event_cb, "sdcard powermgm" );
}

bool sdcard_powermgm_event_cb( EventBits_t event, void *arg ) {
    bool retval = false;

    switch( event ) {
        case POWERMGM_SILENCE_WAKEUP:
            log_i("go silence wakeup");
            retval = true;
            break;
        case POWERMGM_STANDBY:
            log_i("go standby");
            retval = true;
            break;
        case POWERMGM_WAKEUP:
            log_i("go wakeup");
            retval = true;
            break;
    }
    return( retval );
}