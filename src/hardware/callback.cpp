/****************************************************************************
 *   Sep 14 08:11:10 2020
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

#include "callback.h"

void  display_record_event( callback_t *callback, EventBits_t event );

static bool display_event_logging = false;

callback_t *callback_init( const char *name ) {
    callback_t *callback = NULL;
    
#if defined( BOARD_HAS_PSRAM )
    callback = (callback_t*)ps_calloc( sizeof( callback_t ), 1 );
#else
    callback = (callback_t*)calloc( sizeof( callback_t ), 1 );
#endif // BOARD_HAS_PSRAM
    if ( callback == NULL ) {
        log_e("callback_t structure calloc faild for: %s", name );
    }
    else {
        callback->entrys = 0;
        callback->table = NULL;
        callback->name = name;
        log_i("init callback_t structure success for: %s", name );
    }
    return( callback );
}

bool callback_register( callback_t *callback, EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    bool retval = false;

    if ( callback == NULL ) {
        log_w("no callback_t structure found for: %s", id );
        return( retval );
    }

    callback->entrys++;

    if ( callback->table == NULL ) {

#if defined( BOARD_HAS_PSRAM )
        callback->table = ( callback_table_t * )ps_malloc( sizeof( callback_table_t ) * callback->entrys );
#else
        callback->table = ( callback_table_t * )malloc( sizeof( callback_table_t ) * callback->entrys );
#endif // BOARD_HAS_PSRAM

        if ( callback->table == NULL ) {
            log_e("callback_table_t malloc faild for: %s", id );
            return( retval );
        }
        retval = true;
    }
    else {
        callback_table_t *new_callback_table = NULL;

#if defined( BOARD_HAS_PSRAM )
            new_callback_table = ( callback_table_t * )ps_realloc( callback->table, sizeof( callback_table_t ) * callback->entrys );
#else
            new_callback_table = ( callback_table_t * )realloc( callback->table, sizeof( callback_table_t ) * callback->entrys );
#endif // BOARD_HAS_PSRAM

        if ( new_callback_table == NULL ) {
            log_e("callback_table_t realloc faild for: %s", id );
            return( retval );
        }

        callback->table = new_callback_table;
        retval = true;
    }

    callback->table[ callback->entrys - 1 ].event = event;
    callback->table[ callback->entrys - 1 ].callback_func = callback_func;
    callback->table[ callback->entrys - 1 ].id = id;
    callback->table[ callback->entrys - 1 ].counter = 0;
    log_i("register callback_func for %s success (%p:%s)", callback->name, callback->table[ callback->entrys - 1 ].callback_func, callback->table[ callback->entrys - 1 ].id );
    return( retval );
}

void display_record_event( callback_t *callback, EventBits_t event ) {
    time_t now;
    struct tm info;

    time( &now );
    localtime_r( &now, &info );

    char event_log_filename[32];
    size_t written = strftime(event_log_filename, sizeof(event_log_filename), "/event_log_%Y-%m-%d.csv", &info);
    if(written == 0){
        log_e("Can't convert time to filename" );
        return;
    }

    bool write_header = !(SPIFFS.exists(event_log_filename));

    fs::File file = SPIFFS.open( event_log_filename, FILE_APPEND );

    if (!file) {
        log_e("Can't open file: %s!", event_log_filename );
        return;
    }

    if (write_header) {
        file.println("Date\tTime\tFirmware\tUptime_ms\tCallback\tEvent\tFreeHeap\tBatt_V\tCharge_C\tDischarge_C\tBatt_%\tCharging_mA\tDischarging_mA\tPower_mW\tAXP_Temp_degC\tBMA_Temp_degC" );
    }

    if (! file.print( &info, "%F%t%T%t" ) ) {
        log_e("Failed to append to event log file: %s!", event_log_filename );
    } else {
        AXP20X_Class *power = TTGOClass::getWatch()->power;
        BMA *bma = TTGOClass::getWatch()->bma;
        char log_line[256]="";
        snprintf( log_line, sizeof( log_line ), "%s\t%lu\t%s\t%04x\t%u\t%0.2f\t%u\t%u\t%d\t%0.1f\t%0.1f\t%0.1f\t%0.1f\t%0.1f",
            __FIRMWARE__,
            millis(), 
            callback->name,
            event,
            ESP.getFreeHeap(),
            power->getBattVoltage() / 1000.0,
            power->getBattChargeCoulomb(),
            power->getBattDischargeCoulomb(),
            power->getBattPercentage(),
            power->getBattChargeCurrent(),
            power->getBattDischargeCurrent(),
            power->getBattInpower(),
            power->getTemp(), // need to subtract 144.7 till this is resolved: https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/issues/76
            bma->temperature()
        );
        if (! file.println(log_line) ) {
            log_e("Failed to append to event log file: %s!", event_log_filename );
        }
    }
    file.close();
}

bool callback_send( callback_t *callback, EventBits_t event, void *arg ) {
    bool retval = false;

    if ( callback == NULL ) {
        log_e("no callback structure found");
        return( retval );
    }

    if ( callback->entrys == 0 ) {
        log_w("no callback found");
        return( retval );
    }

    if( display_event_logging ) {
        display_record_event( callback, event );
    }

    retval = true;

    for ( int entry = 0 ; entry < callback->entrys ; entry++ ) {
        yield();
        if ( event & callback->table[ entry ].event ) {
            log_i("call %s cb (%p:%04x:%s)", callback->name, callback->table[ entry ].callback_func, event, callback->table[ entry ].id );
            callback->table[ entry ].counter++;
            if ( !callback->table[ entry ].callback_func( event, arg ) ) {
                retval = false;
            }
        }
    }
    return( retval );
}

bool callback_send_no_log( callback_t *callback, EventBits_t event, void *arg ) {
    bool retval = false;

    if ( callback == NULL ) {
        log_e("no callback structure found");
        return( retval );
    }

    if ( callback->entrys == 0 ) {
        log_w("no callback found");
        return( retval );
    }

    retval = true;

    for ( int entry = 0 ; entry < callback->entrys ; entry++ ) {
        yield();
        if ( event & callback->table[ entry ].event ) {
            callback->table[ entry ].counter++;
            if ( !callback->table[ entry ].callback_func( event, arg ) ) {
                retval = false;
            }
        }
    }
    return( retval );
}

void display_event_logging_enable( bool enable ) {
    display_event_logging = enable;
}
