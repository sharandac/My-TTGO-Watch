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