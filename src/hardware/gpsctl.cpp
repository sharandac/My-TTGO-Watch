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
#include "gpsctl.h"
#include "powermgm.h"
#include "callback.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
        #include <TTGO.h>

        #if defined( LILYGO_WATCH_HAS_GPS )
            TinyGPSPlus *gps = nullptr;
            TinyGPSCustom TGC_sats_in_view_gps;
            TinyGPSCustom TGC_sats_in_view_glonass;
            TinyGPSCustom TGC_sats_in_view_baidou;
        #endif
    #elif defined( LILYGO_WATCH_2021 ) 
    #else
        #warning "no hardware driver for gpsctl"
    #endif
#endif

static bool gpsctl_init = false;
static bool gpsctl_enable = false;

gpsctl_config_t gpsctl_config;
callback_t *gpsctl_callback = NULL;
gps_data_t gps_data;

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

    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            #if defined( LILYGO_WATCH_HAS_GPS )
                /*
                * init tinygps
                */
                TTGOClass *ttgo = TTGOClass::getWatch();
                ttgo->gps_begin();
                gps = ttgo->gps;
                TGC_sats_in_view_gps.begin(*gps, "GPGSV", 3);
                TGC_sats_in_view_glonass.begin(*gps, "GLGSV", 3);
                TGC_sats_in_view_baidou.begin(*gps, "BDGSV", 3);
            #endif
        #endif
    #endif
    /**
     * register powermgm call back routine
     */
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
    static uint64_t lastmillis = millis();
    /*
     * check if gpsctl already init or turn off
     */
    if ( !gpsctl_init || !gpsctl_enable ) {
        return( true );
    }
    /**
     * run any second
     */
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
        #ifdef NATIVE_64BIT
        #else
            #ifdef M5PAPER
            #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
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
                        GPSCTL_DEBUG_LOG("new lat/lon: %f/%f", gps_data.lat, gps_data.lon );
                    }
                    if ( gps->speed.isUpdated() ) {
                        gps_data.gps_source = GPS_SOURCE_GPS;
                        gps_data.speed_mph = gps->speed.mph();
                        gps_data.speed_mps = gps->speed.mps();
                        gps_data.speed_kmh = gps->speed.kmph();
                        gpsctl_send_cb( GPSCTL_UPDATE_SPEED, (void*)&gps_data );
                        GPSCTL_DEBUG_LOG("new speed: %fkmh / %fmph / %mps", gps_data.speed_kmh, gps_data.speed_mph, gps_data.speed_mps );
                    }
                    if ( gps->altitude.isUpdated()) {
                        gps_data.gps_source = GPS_SOURCE_GPS;
                        gps_data.altitude_feed = gps->altitude.feet();
                        gps_data.altitude_meters = gps->altitude.meters();
                        gpsctl_send_cb( GPSCTL_UPDATE_ALTITUDE, (void*)&gps_data );
                        GPSCTL_DEBUG_LOG("new altitude: %fmeters / %ffeed", gps_data.altitude_meters, gps_data.altitude_feed );
                    }
                    if ( gps->satellites.isUpdated() ) {
                        if ( gps_data.satellites != gps->satellites.value() ) {
                            gps_data.gps_source = GPS_SOURCE_GPS;
                            gps_data.satellites = gps->satellites.value();
                            gpsctl_send_cb( GPSCTL_UPDATE_SATELLITE, (void*)&gps_data );
                            GPSCTL_DEBUG_LOG("new satellites: %d", gps_data.satellites );
                        }
                    }
                    /*
                    * Update Custom GNSS values
                    */
                    if ( TGC_sats_in_view_gps.isUpdated() )
                    {
                        if ( gps_data.satellite_types.gps_satellites != atoi( TGC_sats_in_view_gps.value() ) ) {
                            gps_data.gps_source = GPS_SOURCE_GPS;
                            gps_data.satellite_types.gps_satellites = atoi( TGC_sats_in_view_gps.value() );
                            gpsctl_send_cb( GPSCTL_UPDATE_SATELLITE_TYPE, (void *)&gps_data );
                            GPSCTL_DEBUG_LOG("gps satellites: %d", gps_data.satellite_types.gps_satellites );
                        }
                    }
                    if ( TGC_sats_in_view_glonass.isUpdated() )
                    {
                        if ( gps_data.satellite_types.glonass_satellites != atoi( TGC_sats_in_view_glonass.value() ) ) {
                            gps_data.gps_source = GPS_SOURCE_GPS;
                            gps_data.satellite_types.glonass_satellites = atoi( TGC_sats_in_view_glonass.value() );
                            gpsctl_send_cb( GPSCTL_UPDATE_SATELLITE_TYPE, (void *)&gps_data );
                            GPSCTL_DEBUG_LOG("glosnass satellites: %d", gps_data.satellite_types.glonass_satellites );
                        }
                    }
                    if ( TGC_sats_in_view_baidou.isUpdated() )
                    {
                        if ( gps_data.satellite_types.baidou_satellites != atoi( TGC_sats_in_view_baidou.value() ) ) {
                            gps_data.gps_source = GPS_SOURCE_GPS;
                            gps_data.satellite_types.baidou_satellites = atoi( TGC_sats_in_view_baidou.value() );
                            gpsctl_send_cb( GPSCTL_UPDATE_SATELLITE_TYPE, (void *)&gps_data );
                            GPSCTL_DEBUG_LOG("baidou satellites: %d", gps_data.satellite_types.baidou_satellites );
                        }
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
            #endif
        #endif // NATIVE_64BIT
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
        case POWERMGM_STANDBY:          if ( gpsctl_config.enable_on_standby && gpsctl_enable ) {
                                            GPSCTL_INFO_LOG("standby blocked by \"enable on standby\" option");
                                        }
                                        else {
                                            GPSCTL_INFO_LOG("go standby");
                                            gpsctl_autoon_off();
                                            retval = true;
                                        }
                                        break;
        case POWERMGM_WAKEUP:           GPSCTL_INFO_LOG("go wakeup");
                                        gpsctl_autoon_on();
                                        retval = true;
                                        break;
        case POWERMGM_SILENCE_WAKEUP:   GPSCTL_INFO_LOG("go silence wakeup");
                                        gpsctl_autoon_on();
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
            GPSCTL_ERROR_LOG("pmu_callback alloc failed");
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
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            #if defined( LILYGO_WATCH_HAS_GPS )
                TTGOClass *ttgo = TTGOClass::getWatch();
                ttgo->trunOnGPS();
            #endif
        #endif
    #endif
    gps_data.gpsfix = false;
    gps_data.valid_location = false;
    gps_data.valid_speed = false;
    gps_data.valid_altitude = false;
    gps_data.valid_satellite = false;
    gps_data.satellite_types.gps_satellites = 0;
    gps_data.satellite_types.glonass_satellites = 0;
    gps_data.satellite_types.baidou_satellites = 0;
    gpsctl_config.autoon = true;
    gpsctl_config.save();
    gpsctl_enable = true;
    gpsctl_send_cb( GPSCTL_UPDATE_CONFIG, NULL );
    gpsctl_send_cb( GPSCTL_ENABLE, NULL );
    gpsctl_send_cb( GPSCTL_NOFIX, NULL );
}

void gpsctl_off( void ) {
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            #if defined( LILYGO_WATCH_HAS_GPS )
                TTGOClass *ttgo = TTGOClass::getWatch();
                ttgo->turnOffGPS();
            #endif
        #endif
    #endif
    gps_data.gpsfix = false;
    gps_data.valid_location = false;
    gps_data.valid_speed = false;
    gps_data.valid_altitude = false;
    gps_data.valid_satellite = false;
    gps_data.satellite_types.gps_satellites = 0;
    gps_data.satellite_types.glonass_satellites = 0;
    gps_data.satellite_types.baidou_satellites = 0;
    gpsctl_config.autoon = false;
    gpsctl_config.save();
    gpsctl_enable = false;
    gpsctl_send_cb( GPSCTL_UPDATE_CONFIG, NULL );
    gpsctl_send_cb( GPSCTL_NOFIX, NULL );
    gpsctl_send_cb( GPSCTL_DISABLE, NULL );
}

void gpsctl_autoon_on( void ) {

    gps_data.gpsfix = false;
    gps_data.valid_location = false;
    gps_data.valid_speed = false;
    gps_data.valid_altitude = false;
    gps_data.valid_satellite = false;
    gps_data.satellite_types.gps_satellites = 0;
    gps_data.satellite_types.glonass_satellites = 0;
    gps_data.satellite_types.baidou_satellites = 0;

    if ( gpsctl_config.autoon ) {
        if ( !gpsctl_enable ) {
            #ifdef NATIVE_64BIT
            #else
                #ifdef M5PAPER
                #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
                    #if defined( LILYGO_WATCH_HAS_GPS )
                        TTGOClass *ttgo = TTGOClass::getWatch();
                        ttgo->trunOnGPS();
                    #endif
                #endif
            #endif
            gpsctl_enable = true;
            gpsctl_send_cb( GPSCTL_ENABLE, NULL );
            gpsctl_send_cb( GPSCTL_NOFIX, NULL );
        }
    }
    else {
        gpsctl_enable = false;
        gpsctl_send_cb( GPSCTL_NOFIX, NULL );
        gpsctl_send_cb( GPSCTL_DISABLE, NULL );
    }
}

void gpsctl_autoon_off( void ) {
    #ifdef NATIVE_64BIT
    #else
        #ifdef M5PAPER
        #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
            #if defined( LILYGO_WATCH_HAS_GPS )
                TTGOClass *ttgo = TTGOClass::getWatch();
                ttgo->turnOffGPS();
            #endif
        #endif
    #endif
    gpsctl_enable = false;
    gps_data.gpsfix = false;
    gps_data.valid_location = false;
    gps_data.valid_speed = false;
    gps_data.valid_altitude = false;
    gps_data.valid_satellite = false;
    gps_data.satellite_types.gps_satellites = 0;
    gps_data.satellite_types.glonass_satellites = 0;
    gps_data.satellite_types.baidou_satellites = 0;
    gpsctl_send_cb( GPSCTL_NOFIX, NULL );
    gpsctl_send_cb( GPSCTL_DISABLE, NULL );
}

bool gpsctl_get_app_use_gps( void ) {
    return( gpsctl_config.app_use_gps );
}

void gpsctl_set_app_use_gps( bool app_use_gps ) {
    gpsctl_config.app_use_gps = app_use_gps;
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

void gpsctl_set_location( double lat, double lon, double altitude, gps_source_t gps_source , bool app_location ) {
    /*
     * setup gps_data structure and send events
     */
    if ( !gps_data.gpsfix ) {
        gps_data.gpsfix = true;
        gpsctl_send_cb( GPSCTL_FIX, NULL );
    }
    if ( gps_data.gps_source != gps_source ) {
        gps_data.gps_source = gps_source;
        gpsctl_send_cb( GPSCTL_UPDATE_SOURCE, (void*)&gps_data );        
    }
    gps_data.valid_location = true;
    gps_data.valid_speed = false;
    gps_data.valid_satellite = false;
    gps_data.valid_altitude = true;
    gps_data.lat = lat;
    gps_data.lon = lon;
    gps_data.altitude_meters = altitude;
    /*
     * send FIX, UPDATE_SOURCE and UPDATE_LOCATION
     */
    gpsctl_send_cb( GPSCTL_UPDATE_LOCATION, (void*)&gps_data );
    gpsctl_send_cb( GPSCTL_UPDATE_ALTITUDE, (void*)&gps_data );
    /*
     * send SET_APP_LOCATION if enabled
     */
    if ( gpsctl_get_app_use_gps() && app_location ) {
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