/****************************************************************************
 *   Sep 11 10:11:10 2021
 *   Copyright  2021  Dirk Brosswick
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
#include "powermgm.h"
#include "sensor.h"
#include "callback.h"

#ifdef NATIVE_64BIT
    #include "utils/millis.h"
    #include "utils/logging.h"
#else
    #include <Arduino.h>
    #ifdef M5PAPER
        #include <M5EPD.h>
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #elif defined( LILYGO_WATCH_2021 )    
    #elif defined( WT32_SC01 )
    #else
        #warning "no hardware driver for sensor"
    #endif
#endif

callback_t *sensor_callback = NULL;

bool sensor_powermgm_loop_cb( EventBits_t event, void *arg );
bool sensor_powermgm_event_cb( EventBits_t event, void *arg );
bool sensor_send_cb( EventBits_t event, void *arg );

void sensor_setup( void ) {
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
            /**
             * setup SHT30 sensor
             */
            M5.SHT30.Begin();
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( WT32_SC01 )
        #endif
    #endif
    /**
     * setup powermgm
     */
    powermgm_register_cb( POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, sensor_powermgm_event_cb, "sensor powermgm event" );
    powermgm_register_loop_cb( POWERMGM_STANDBY | POWERMGM_WAKEUP | POWERMGM_SILENCE_WAKEUP, sensor_powermgm_loop_cb, "sensor powermgm loop" );
}

bool sensor_powermgm_loop_cb( EventBits_t event, void *arg ) {
    bool retval = false;

    #ifdef NATIVE_64BIT
        retval = true;
    #else
        #ifdef M5PAPER
            static uint64_t nextmillis = millis();
            /**
             * check if an update
             */
            if ( nextmillis < millis() ) {
                /**
                 * set next update time
                 */
                nextmillis = millis() + ( SENSOR_UPDATE_INTERVAL * 1000 );
                /**
                 * update sensor
                 */
                M5.SHT30.UpdateData();
                /**
                 * update all values
                 */
                float value = 0.0f;
                value = sensor_get_temperature();
                sensor_send_cb( SENSOR_TEMPERATURE, (void *)&value );
                value = sensor_get_humidity();
                sensor_send_cb( SENSOR_HUMIDITY, (void *)&value );
                value = sensor_get_relativ_humidity();
                sensor_send_cb( SENSOR_RELHUMIDITY, (void *)&value );
                value = sensor_get_pressure();
                sensor_send_cb( SENSOR_PRESSURE, (void *)&value );
            }
            retval = true;
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            retval = true;
        #elif defined( WT32_SC01 )
            retval = true;
        #else
            #warning "not sensor powermgm loop"
            retval = true;
        #endif
    #endif

    return( retval );
}

bool sensor_powermgm_event_cb( EventBits_t event, void *arg ) {
    bool retval = false;

    #ifdef NATIVE_64BIT
        retval = true;
    #else
        #ifdef M5PAPER
            switch( event ) {
                case POWERMGM_STANDBY:              log_d("go standby");
                                                    retval = true;
                                                    break;
                case POWERMGM_WAKEUP:               log_d("go wakeup");
                                                    M5.SHT30.UpdateData();
                                                    retval = true;
                                                    break;
                case POWERMGM_SILENCE_WAKEUP:       log_d("go silence wakeup");
                                                    M5.SHT30.UpdateData();
                                                    retval = true;
                                                    break;
                case POWERMGM_ENABLE_INTERRUPTS:    log_d("enable interrupts");
                                                    retval = true;
                                                    break;
                case POWERMGM_DISABLE_INTERRUPTS:   log_d("disable interrupts");
                                                    retval = true;
                                                    break;
            }
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            retval = true;
        #elif defined( WT32_SC01 )
            retval = true;
        #else
            #warning "not sensor powermgm"
            retval = true;
        #endif
    #endif

    return( retval );
}

bool sensor_get_available( void ) {
    bool retval = false;
    
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
            retval = true;
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( WT32_SC01 )
        #endif
    #endif

    return( retval );
}

float sensor_get_temperature( void ) {
    float temperature = 0.0f;
    /**
     * check if sensor available
     */
    if( !sensor_get_available() ) {
        return( temperature );
    }
    /**
     * read sensor
     */
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
            temperature = M5.SHT30.GetTemperature();
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( WT32_SC01 )
        #endif
    #endif

    return( temperature );
}

float sensor_get_humidity( void ) {
    float humidity = 0.0f;
    /**
     * check if sensor available
     */
    if( !sensor_get_available() ) {
        return( 0.0f );
    }
    /**
     * read sensor
     */
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
            humidity = M5.SHT30.GetAbsHumidity();
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( WT32_SC01 )
        #endif
    #endif

    return( humidity );
}

float sensor_get_relativ_humidity( void ) {
    float relativ_humidity = 0.0f;
    /**
     * check if sensor available
     */
    if( !sensor_get_available() ) {
        return( 0.0f );
    }
    /**
     * read sensor
     */
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
            relativ_humidity = M5.SHT30.GetRelHumidity();
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( WT32_SC01 )
        #endif
    #endif

    return( relativ_humidity );
}

float sensor_get_pressure( void ) {
    float pressure = 0.0f;
    /**
     * check if sensor available
     */
    if( !sensor_get_available() ) {
        return( 0.0f );
    } 
    /**
     * read sensor
     */
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #elif defined( WT32_SC01 )
        #endif
    #endif

    return( pressure );
}

bool sensor_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    /*
     * check if an callback table exist, if not allocate a callback table
     */
    if ( sensor_callback == NULL ) {
        sensor_callback = callback_init( "sensor" );
        if ( sensor_callback == NULL ) {
            log_e("sensor_callback alloc failed");
            while( true );
        }
    }
    /*
     * register an callback entry and return them
     */
    return( callback_register( sensor_callback, event, callback_func, id ) );
}

bool sensor_send_cb( EventBits_t event, void *arg ) {
    /*
     * call all callbacks with her event mask
     */
    return( callback_send( sensor_callback, event, arg ) );
}