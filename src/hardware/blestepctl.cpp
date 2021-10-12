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
#include "blestepctl.h"
#include "blectl.h"
#include "motion.h"
#include "bleupdater.h"
#include "utils/bluejsonrequest.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #include <BLEServer.h>
    #include <BLE2902.h>
    #ifdef M5PAPER

    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )

    #else
        #warning "no hardware driver for blestepctl"
    #endif
#endif

class StepcounterBleUpdater : public BleUpdater<int32_t> {
    public:
    /*
     * Update every 1800 as GadgetBidge uses such value by default
     */
    StepcounterBleUpdater() : BleUpdater(1800){}
    protected:
    bool notify(int32_t stepcounter) {
        /*
         * Take care of daily reset
         */
        uint32_t delta = stepcounter < last_value ? stepcounter : stepcounter - last_value;
        /*
         * Cf. https://www.espruino.com/Gadgetbridge
         */
        char msg[64]="";
        snprintf( msg, sizeof( msg ),"\r\n{t:\"act\", stp:%d}\r\n", delta );
        bool ret = blectl_send_msg( msg );
        log_i("Notified stepcounter: new=%d last=%d -> delta=%d => %d", stepcounter, last_value, delta, ret);
        return ret;
    }
};

static StepcounterBleUpdater stepcounter_ble_updater;
static int32_t stepcounter = 0;

static bool blestepctl_bma_event_cb( EventBits_t event, void *arg );
static bool blestepctl_bluetooth_event_cb(EventBits_t event, void *arg);

void blestepctl_setup( void ) {
    bma_register_cb( BMACTL_STEPCOUNTER, blestepctl_bma_event_cb, "ble step counter");
    blectl_register_cb( BLECTL_CONNECT | BLECTL_MSG_JSON, blestepctl_bluetooth_event_cb, "ble step counter" );
}

static bool blestepctl_bma_event_cb( EventBits_t event, void *arg ) {
    bool retval = false;
    
    switch( event ) {
        case BMACTL_STEPCOUNTER:
            stepcounter = *(int32_t*)arg;
            stepcounter_ble_updater.update( stepcounter );
            retval = true;
            break;
    }
    return( retval );
}

static bool blestepctl_bluetooth_event_cb(EventBits_t event, void *arg) {
    bool retval = false;
    
    switch( event ) {
        case BLECTL_CONNECT: 
                /*
                 * Try to refresh step counter value on (re)connect
                 */
                stepcounter_ble_updater.update( stepcounter );
                retval = true;
                break;
        case BLECTL_MSG_JSON:
                BluetoothJsonRequest &request = *(BluetoothJsonRequest*)arg;

                if (request.isEqualKeyValue("t","act") && request.containsKey("stp") && request["stp"].as<bool>() && request.containsKey("int")) {
                    /*
                     * get requested timeout, in seconds
                     * TODO_ affect power loop rate
                     */
                    time_t timeout = request["int"].as<time_t>(); // Requested timeout, in seconds
                    log_i("RECEIVED timeout: %d seconds", timeout);
                    stepcounter_ble_updater.setTimeout(timeout);
                }
                retval = true;
                break;
    }


    return( retval );
}

void blestepctl_update(bool force) {
    stepcounter_ble_updater.update( stepcounter, force );
}