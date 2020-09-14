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

bool callback_init( callback_t *callback, const char *name ) {
    bool retval = false;
    
    if ( callback == NULL ) {
        callback = (callback_t*)ps_calloc( sizeof( callback_t ), 1 );
        if ( callback == NULL ) {
            log_e("callback_t structure calloc faild");
        }
        else {
            callback->entrys = 0;
            callback->table = NULL;
            callback->name = name;
            retval = true;
        }
    }
    return( retval );
}

bool callback_register( callback_t *callback, EventBits_t event, CALLBACK_FUNC callback_func, const char *id ) {
    bool retval = false;

    if ( callback == NULL ) {
        log_w("no callback_t structure found");
        return( retval );
    }

    callback->entrys++;

    if ( callback->table == NULL ) {
        callback->table = ( callback_table_t * )ps_malloc( sizeof( callback_table_t ) * callback->entrys );
        if ( callback->table == NULL ) {
            log_e("callback_table_t malloc faild");
            return( retval );
        }
        retval = true;
    }
    else {
        callback_table_t *new_callback_table = NULL;

        new_callback_table = ( callback_table_t * )ps_realloc( callback->table, sizeof( callback_table_t ) * callback->entrys );
        if ( new_callback_table == NULL ) {
            log_e("callback_table_t realloc faild");
            return( retval );
        }
        callback->table = new_callback_table;
        retval = true;
    }

    callback->table[ callback->entrys - 1 ].event = event;
    callback->table[ callback->entrys - 1 ].callback_func = callback_func;
    callback->table[ callback->entrys - 1 ].id = id;
    callback->table[ callback->entrys - 1 ].counter = 0;
    log_i("register callback_func success (%p:%s)", callback->table[ callback->entrys - 1 ].callback_func, callback->table[ callback->entrys - 1 ].id );
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
      
    for ( int entry = 0 ; entry < callback->entrys ; entry++ ) {
        yield();
        if ( event & callback->table[ entry ].event ) {
            log_i("call %s cb (%p:%04x:%s)", callback->name, callback->table[ entry ].callback_func, event, callback->table[ entry ].id );
            callback->table[ entry ].counter++;
            if ( callback->table[ entry ].callback_func( event, arg ) ) {
                retval = true;
                log_w("standby blocked by: %s", callback->table[ entry ].id );
            }
        }
    }
    return( retval );
}