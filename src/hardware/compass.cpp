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
#include "callback.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
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
    #elif defined( WT32_SC01 )
    #else
        #warning "no hardware driver for compass"
    #endif
#endif

static bool compass_init = false;
static bool compass_active = false;
static bool compass_calibrated = false;
static int64_t compass_calibration = 0;
callback_t *compass_callback = NULL;
static int calibrationData[3][2];

static void compass_calibration_loop( compass_data_t *compass_data );
static bool compass_get_data( compass_data_t *compass_data );
static bool compass_powermgm_event_cb( EventBits_t event, void *arg );
static bool compass_powermgm_loop_event_cb( EventBits_t event, void *arg );
static bool compass_send_event_cb( EventBits_t event, void *arg );

void compass_setup( void ) {

    #ifdef NATIVE_64BIT
    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( LILYGO_WATCH_2021 )
            compass.init();
            compass_off();
        #elif defined( WT32_SC01 )
        #else
        #endif
    #endif
    powermgm_register_loop_cb( POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, compass_powermgm_loop_event_cb, "compass powermgm event" );
    powermgm_register_cb( POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, compass_powermgm_event_cb, "compass powermgm loop event" );

    compass_init = true;
}

static bool compass_powermgm_event_cb( EventBits_t event, void *arg ) {
    #ifdef NATIVE_64BIT
    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( LILYGO_WATCH_2021 )
            switch( event ) {
                case POWERMGM_STANDBY:
                    compass_off();
                    break;
                case POWERMGM_WAKEUP:
                    compass_off();
                    break;
                case POWERMGM_SILENCE_WAKEUP:
                    compass_off();
                    break;
                default:
                    break;
            }
        #elif defined( WT32_SC01 )
        #else
        #endif
    #endif

    return( true );
}

static bool compass_powermgm_loop_event_cb( EventBits_t event, void *arg ) {
    static int64_t nextmillis = millis() + COMPASS_UPDATE_INTERVAL;

    if( compass_calibration != 0 ) {
        if( compass_calibration < millis() ) {
            compass_calibration = 0;
            log_i("x = %d/%d", calibrationData[0][0], calibrationData[0][1] );
            log_i("y = %d/%d", calibrationData[1][0], calibrationData[1][1] );
            log_i("z = %d/%d", calibrationData[2][0], calibrationData[2][1] );
            #ifdef NATIVE_64BIT
            #else
                #if defined( M5PAPER )
                #elif defined( M5CORE2 )
                #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                #elif defined( LILYGO_WATCH_2021 )
                        compass.setCalibration( calibrationData[0][0], calibrationData[0][1], calibrationData[1][0], calibrationData[1][1], calibrationData[2][0], calibrationData[2][1] );
                #elif defined( WT32_SC01 )
                #endif
            #endif
            compass_calibrated = true;
            log_i("stop calibration");
        }
        else {
            compass_data_t compass_data;
            compass_get_data( &compass_data );
            compass_calibration_loop( &compass_data );
        }
    }
    else if( nextmillis < millis() && compass_active ) {
        nextmillis = millis() + COMPASS_UPDATE_INTERVAL;

        compass_data_t compass_data;

        switch( event ) {
            case POWERMGM_STANDBY:
                break;
            case POWERMGM_WAKEUP:
                compass_get_data( &compass_data );
                compass_send_event_cb( COMPASS_UPDATE, (void*)&compass_data );
                break;
            case POWERMGM_SILENCE_WAKEUP:
                break;
            default:
                break;
        }
    }

    return( true );
}

static bool compass_get_data( compass_data_t *compass_data ) {
    #ifdef NATIVE_64BIT
    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( LILYGO_WATCH_2021 )
            compass.read();
            compass_data->x = compass.getX();
            compass_data->y = compass.getY();
            compass_data->z = compass.getZ();
            compass_data->azimuth = compass.getAzimuth();
            compass_data->bearing = compass.getBearing( compass_data->azimuth );
            compass.getDirection( compass_data->direction, compass_data->azimuth );
            compass_data->direction[3] = '\0';
            log_d("x=%d, y=%d, z=%d, azimuth=%d, bearing=%d, direction=%s", compass_data->x, compass_data->y, compass_data->z, compass_data->azimuth, compass_data->bearing, compass_data->direction );
        #elif defined( WT32_SC01 )
        #else
        #endif
    #endif

    return( true );
}

bool compass_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    if ( compass_callback == NULL ) {
        compass_callback = callback_init( "compass" );
        if ( compass_callback == NULL ) {
            log_e("compass__callback alloc failed");
            while(true);
        }
    }
    return( callback_register( compass_callback, event, callback_func, id ) );
}

static bool compass_send_event_cb( EventBits_t event, void *arg ) {
    return( callback_send( compass_callback, event, arg ) );
}

static void compass_calibration_loop( compass_data_t *compass_data ) {
    bool change = false;
    
    if( compass_data->x < calibrationData[0][0] ) {
        calibrationData[0][0] = compass_data->x;
        change = true;
    }

    if( compass_data->x > calibrationData[0][1] ) {
        calibrationData[0][1] = compass_data->x;
        change = true;
    }

    if( compass_data->y < calibrationData[1][0] ) {
        calibrationData[1][0] = compass_data->y;
        change = true;
    }

    if( compass_data->y > calibrationData[1][1] ) {
        calibrationData[1][1] = compass_data->y;
        change = true;
    }

    if( compass_data->z < calibrationData[2][0] ) {
        calibrationData[2][0] = compass_data->z;
        change = true;
    }

    if( compass_data->z > calibrationData[2][1] ) {
        calibrationData[2][1] = compass_data->z;
        change = true;
    }

    if( change )
        compass_calibration = millis() + 5000;
}

void compass_on( void ) {
    compass_active = true;

    #ifdef NATIVE_64BIT
    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( LILYGO_WATCH_2021 )
            compass.init();
            compass.setReset();
            compass.setMode( 0x01, 0x0C, 0x10, 0x00 );
            if( compass_calibrated )
                compass.setCalibration( calibrationData[0][0], calibrationData[0][1], calibrationData[1][0], calibrationData[1][1], calibrationData[2][0], calibrationData[2][1] );
        #elif defined( WT32_SC01 )
        #else
        #endif
    #endif
}

void compass_off( void ) {
    compass_active = false;
    
    #ifdef NATIVE_64BIT
    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( LILYGO_WATCH_2021 )
            compass.init();
            compass.setReset();
            compass.setMode( 0x00, 0x0C, 0x10, 0x00 );
        #elif defined( WT32_SC01 )
        #else
        #endif
    #endif
}

bool compass_start_calibration( void ) {
    if( compass_calibrated )
        return( true );
    /**
     * reset calibration data
     */
    calibrationData[0][0] = 0;
    calibrationData[0][1] = 0;
    calibrationData[1][0] = 0;
    calibrationData[1][1] = 0;
    calibrationData[2][0] = 0;
    calibrationData[2][1] = 0;
    /**
     * enable compass
     */
    compass_on();
    /**
     * set calibration active
     */
    compass_calibration = millis() + 20000;
    log_i("start calibration");

    return( true );
}

bool compass_available( void ) {
    bool retval = false;

    #ifdef NATIVE_64BIT
        retval = true;
    #else
        #if defined( M5PAPER )
        #elif defined( M5CORE2 )
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( LILYGO_WATCH_2021 )
            retval = true;
        #elif defined( WT32_SC01 )
        #else
        #endif
    #endif
    
    return( retval );
}