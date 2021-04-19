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
#include <TTGO.h>

#include "gpsctl.h"
#include "powermgm.h"
#include "callback.h"

static bool gpsctl_init = false;

gpsctl_config_t gpsctl_config;
callback_t *gpsctl_callback = NULL;
gps_data_t gps_data;

#if defined( LILYGO_WATCH_HAS_GPS )
    TinyGPSPlus *gps = nullptr;
#endif

bool gpsctl_powermgm_loop_cb( EventBits_t event, void *arg );
bool gpsctl_powermgm_event_cb( EventBits_t event, void *arg );
bool gpsctl_send_cb( EventBits_t event, void *arg );
void gpsctl_autoon_on( void );
void gpsctl_autoon_off( void );

void gpsctl_setup( void ) {
    /*
     * check if gpsctl already init
     */
    if ( gpsctl_init ) {
        return;
    }
    /*
     * load config from json
     */
    gpsctl_config.load();

    #if defined( LILYGO_WATCH_HAS_GPS )
        /*
         * init tinygps
         */
        TTGOClass *ttgo = TTGOClass::getWatch();
        ttgo->trunOnGPS();
        ttgo->gps_begin();
        gps = ttgo->gps;
    #endif

    powermgm_register_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, gpsctl_powermgm_event_cb, "powermgm gpsctl" );
    powermgm_register_loop_cb( POWERMGM_SILENCE_WAKEUP | POWERMGM_STANDBY | POWERMGM_WAKEUP, gpsctl_powermgm_loop_cb, "powermgm gpsctl loop" );

    gpsctl_init = true;

    gpsctl_send_cb( GPSCTL_UPDATE_CONFIG, NULL );
    gpsctl_autoon_on();
}

bool gpsctl_get_available( void ) {
    #if defined( LILYGO_WATCH_HAS_GPS )
        return( true );
    #else
        return( false );
    #endif
}

bool gpsctl_powermgm_loop_cb( EventBits_t event, void *arg ) {
    /*
     * check if gpsctl already init
     */
    if ( !gpsctl_init ) {
        return( true );
    }
    static uint64_t lastmillis = millis();

    if ( millis() - lastmillis > GPSCTL_INTERVAL ) {
        /*
         * check if the last update is more than 2 times away
         * to avoid callback bombing
         */
        if ( ( millis() - lastmillis ) > GPSCTL_INTERVAL * 2 ) {
            lastmillis = millis();
        }
        else {
            lastmillis =+ GPSCTL_INTERVAL;
        }

        #if defined( LILYGO_WATCH_HAS_GPS )
            TTGOClass *ttgo = TTGOClass::getWatch();
            ttgo->gpsHandler();
            /*
             * store valid state
             */
            gps_data.valid_location = gps->location.isValid();
            gps_data.valid_speed = gps->speed.isValid();
            gps_data.valid_satellite = gps->satellites.isValid();
            gps_data.valid_altitude = gps->altitude.isValid();
             /*
             * send FIX, UPDATE_SOURCE and UPDATE_LOCATION
             */
            if ( gps_data.valid_location != gps_data.gpsfix ) {
                gps_data.gpsfix = gps_data.valid_location;
                if ( gps_data.gpsfix ) {
                    /*
                     * send FIX and SET_APP_LOCATION event 
                     */
                    gpsctl_send_cb( GPSCTL_FIX, NULL );
                    if ( gpsctl_get_app_use_gps() ) {
                        gps_data.lat = gps->location.lat();
                        gps_data.lon = gps->location.lng();
                        gpsctl_send_cb( GPSCTL_SET_APP_LOCATION, (void*)&gps_data );
                    }
                    gpsctl_send_cb( GPSCTL_UPDATE_SOURCE, (void*)&gps_data );
                }
                else {
                    /*
                     * send NOFIX event
                     */
                    gpsctl_send_cb( GPSCTL_NOFIX, NULL );
                }
            }
            /*
             * check for data updates
             */
            if ( gps->location.isUpdated() ) {
                gps_data.gps_source = GPS_SOURCE_GPS;
                gps_data.lat = gps->location.lat();
                gps_data.lon = gps->location.lng();
                gpsctl_send_cb( GPSCTL_UPDATE_LOCATION, (void*)&gps_data );
            }
            if ( gps->speed.isUpdated() ) {
                gps_data.gps_source = GPS_SOURCE_GPS;
                gps_data.speed_mph = gps->speed.mph();
                gps_data.speed_mps = gps->speed.mps();
                gps_data.speed_kmh = gps->speed.kmph();
                gpsctl_send_cb( GPSCTL_UPDATE_SPEED, (void*)&gps_data );
            }
            if ( gps->altitude.isUpdated()) {
                gps_data.gps_source = GPS_SOURCE_GPS;
                gps_data.altitude_feed = gps->altitude.feet();
                gps_data.altitude_meters = gps->altitude.meters();
                gpsctl_send_cb( GPSCTL_UPDATE_ALTITUDE, (void*)&gps_data );
            }
            if ( gps->satellites.isUpdated() ) {
                gps_data.gps_source = GPS_SOURCE_GPS;
                gps_data.satellites = gps->satellites.value();
                gpsctl_send_cb( GPSCTL_UPDATE_SATELLITE, (void*)&gps_data );
            }

        #else
            /*
             * send only when valid_location and gpsfix not equal
             */
            if ( gps_data.valid_location != gps_data.gpsfix ) {
                gps_data.gpsfix = gps_data.valid_location;
                if ( gps_data.gpsfix ) {
                    /*
                     * send FIX, UPDATE_SOURCE and UPDATE_LOCATION
                     */
                    gpsctl_send_cb( GPSCTL_FIX, NULL );
                    gpsctl_send_cb( GPSCTL_UPDATE_LOCATION, (void*)&gps_data );
                    gpsctl_send_cb( GPSCTL_UPDATE_SOURCE, (void*)&gps_data );
                    /*
                     * send SET_APP_LOCATION if enabled
                     */
                    if ( gpsctl_get_app_use_gps() ) {
                        gpsctl_send_cb( GPSCTL_SET_APP_LOCATION, (void*)&gps_data );
                    }
                }
                else {
                    /*
                     * send NOFIX event
                     */
                    gpsctl_send_cb( GPSCTL_NOFIX, NULL );
                }
            }
        #endif
    }

    return( true );
}

bool gpsctl_powermgm_event_cb( EventBits_t event, void *arg ) {
    /*
     * check if gpsctl already init
     */
    if ( !gpsctl_init ) {
        return( true );
    }

    bool retval = false;

    switch( event ) {
        case POWERMGM_STANDBY:          log_i("go standby");
                                        if ( !gpsctl_config.enable_on_standby ) {
                                            gpsctl_autoon_off();
                                        }
                                        retval = true;
                                        break;
        case POWERMGM_WAKEUP:           log_i("go wakeup");
                                        gpsctl_autoon_on();
                                        retval = true;
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   log_i("go silence wakeup");
                                        retval = true;
                                        break;
    }

    return( retval );    
}

bool gpsctl_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    /*
     * check if an callback table exist, if not allocate a callback table
     */
    if ( gpsctl_callback == NULL ) {
        gpsctl_callback = callback_init( "pmu" );
        if ( gpsctl_callback == NULL ) {
            log_e("pmu_callback alloc failed");
            while( true );
        }
    }
    /*
     * register an callback entry and return them
     */
    return( callback_register( gpsctl_callback, event, callback_func, id ) );
}

bool gpsctl_send_cb( EventBits_t event, void *arg ) {
    /*
     * call all callbacks with her event mask
     */
    return( callback_send( gpsctl_callback, event, arg ) );
}

void gpsctl_on( void ) {
    if ( gpsctl_config.app_control_permission ) {
        #if defined( LILYGO_WATCH_HAS_GPS )
	        TTGOClass *ttgo = TTGOClass::getWatch();
	        ttgo->trunOnGPS();
        #endif

    }
        gps_data.gpsfix = false;
	    gps_data.valid_location = false;
	    gps_data.valid_speed = false;
	    gps_data.valid_altitude = false;
	    gps_data.valid_satellite = false;
	    gpsctl_send_cb( GPSCTL_ENABLE, NULL );
	    gpsctl_send_cb( GPSCTL_NOFIX, NULL );
}

void gpsctl_off( void ) {
    if ( gpsctl_config.app_control_permission ) {
        #if defined( LILYGO_WATCH_HAS_GPS )
	        TTGOClass *ttgo = TTGOClass::getWatch();
	        ttgo->turnOffGPS();
        #endif
	    gps_data.gpsfix = false;
	    gps_data.valid_location = false;
	    gps_data.valid_speed = false;
	    gps_data.valid_altitude = false;
	    gps_data.valid_satellite = false;
	    gpsctl_send_cb( GPSCTL_NOFIX, NULL );
	    gpsctl_send_cb( GPSCTL_DISABLE, NULL );
    }
}

void gpsctl_autoon_on( void ) {
    if ( gpsctl_config.autoon ) {
        #if defined( LILYGO_WATCH_HAS_GPS )
            TTGOClass *ttgo = TTGOClass::getWatch();
            ttgo->trunOnGPS();
        #endif
        gps_data.gpsfix = false;
        gps_data.valid_location = false;
        gps_data.valid_speed = false;
        gps_data.valid_altitude = false;
        gps_data.valid_satellite = false;
        gpsctl_send_cb( GPSCTL_ENABLE, NULL );
        gpsctl_send_cb( GPSCTL_NOFIX, NULL );
    }
    else {
        gps_data.gpsfix = false;
        gps_data.valid_location = false;
        gps_data.valid_speed = false;
        gps_data.valid_altitude = false;
        gps_data.valid_satellite = false;
        gpsctl_send_cb( GPSCTL_NOFIX, NULL );
        gpsctl_send_cb( GPSCTL_DISABLE, NULL );
    }
}

void gpsctl_autoon_off( void ) {
    #if defined( LILYGO_WATCH_HAS_GPS )
        TTGOClass *ttgo = TTGOClass::getWatch();
        ttgo->turnOffGPS();
    #endif
    gps_data.gpsfix = false;
    gps_data.valid_location = false;
    gps_data.valid_speed = false;
    gps_data.valid_altitude = false;
    gps_data.valid_satellite = false;
    gpsctl_send_cb( GPSCTL_NOFIX, NULL );
    gpsctl_send_cb( GPSCTL_DISABLE, NULL );
}

bool gpsctl_get_app_use_gps( void ) {
    return( gpsctl_config.app_use_permission );
}

bool gpsctl_get_app_control_gps( void ) {
    return( gpsctl_config.app_control_permission );
}

void gpsctl_set_app_use_gps( bool app_use_permission ) {
    gpsctl_config.app_use_permission = app_use_permission;
    gpsctl_config.save();
}

void gpsctl_set_app_control_gps( bool app_control_permission ) {
    gpsctl_config.app_control_permission = app_control_permission;
    gpsctl_config.save();
    gpsctl_send_cb( GPSCTL_UPDATE_CONFIG, NULL );
}

bool gpsctl_get_autoon( void ) {
    return( gpsctl_config.autoon );
}

void gpsctl_set_autoon( bool autoon ) {
    gpsctl_config.autoon = autoon;
    gpsctl_config.save();
    gpsctl_send_cb( GPSCTL_UPDATE_CONFIG, NULL );
}

bool gpsctl_get_gps_over_ip( void ) {
    return( gpsctl_config.gps_over_ip );
}

void gpsctl_set_gps_over_ip( bool gps_over_ip ) {
    gpsctl_config.gps_over_ip = gps_over_ip;
    gpsctl_config.save();
    gpsctl_send_cb( GPSCTL_UPDATE_CONFIG, NULL );
}

bool gpsctl_get_enable_on_standby( void ) {
    return( gpsctl_config.enable_on_standby );
}

void gpsctl_set_enable_on_standby( bool enable_on_standby ) {
    gpsctl_config.enable_on_standby = enable_on_standby;
    gpsctl_config.save();
    gpsctl_send_cb( GPSCTL_UPDATE_CONFIG, NULL );
}

void gpsctl_set_location( double lat, double lon, gps_source_t gps_source ) {
    /*
     * setup gps_data structure and send events
     */
    gps_data.gps_source = gps_source;
    gps_data.gpsfix = false;
    gps_data.valid_location = true;
    gps_data.valid_speed = false;
    gps_data.valid_satellite = false;
    gps_data.valid_altitude = false;
    gps_data.lat = lat;
    gps_data.lon = lon;
    /*
     * send FIX, UPDATE_SOURCE and UPDATE_LOCATION
     */
    gpsctl_send_cb( GPSCTL_FIX, NULL );
    gpsctl_send_cb( GPSCTL_UPDATE_LOCATION, (void*)&gps_data );
    gpsctl_send_cb( GPSCTL_UPDATE_SOURCE, (void*)&gps_data );
    /*
     * send SET_APP_LOCATION if enabled
     */
    if ( gpsctl_get_app_use_gps() ) {
        gpsctl_send_cb( GPSCTL_SET_APP_LOCATION, (void*)&gps_data );
    }
}

const char *gpsctl_get_source_str( gps_source_t gps_source ) {
    const char *ret_val = NULL;

    switch( gps_source ) {
        case GPS_SOURCE_UNKNOWN:
            ret_val = "unknown gps";
            break;
        case GPS_SOURCE_FAKE:
            ret_val = "fake gps";
            break;
        case GPS_SOURCE_IP:
            ret_val = "ip location";
            break;
        case GPS_SOURCE_USER:
            ret_val = "user gps";
            break;
        case GPS_SOURCE_GPS:
            ret_val = "gps receiver";
            break;
        default:
            ret_val = "no source";
    }

    return( ret_val );
}