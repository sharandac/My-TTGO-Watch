/****************************************************************************
 *   Oct 05 23:37:31 2020
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
#include "alloc.h"

#include "msg_chain.h"

msg_chain_t * msg_chain_add_msg( msg_chain_t *msg_chain, const char *msg ) {
    /*
     * add msg_chain_t head structure if not exsist
     */
    if ( msg_chain == NULL ) {
        msg_chain = (msg_chain_t *)CALLOC( sizeof( msg_chain_t ), 1 );
        if ( msg_chain == NULL ) {
            log_e("msg_chain_t alloc failed");
            while( true );
        }

        msg_chain->entrys = 0;
        msg_chain->current_entry = 0;
        msg_chain->current_msg_chain_entry = NULL;
        msg_chain->first_msg_chain_entry = NULL;
    }
 
    /*
     * add new msg_chain_entry_t structure and add msg
     */
    msg_chain_entry_t *msg_chain_entry = (msg_chain_entry_t *)CALLOC( sizeof( msg_chain_entry_t ), 1 );
    if ( msg_chain_entry == NULL ) {
        log_e("msg_chain_entry_t alloc failed");
        while( true );
    }
    else {
        msg_chain_entry->msg = (const char *)CALLOC( strlen( msg ) + 1, 1 );
        if ( msg_chain_entry->msg == NULL ) {
            log_e("msg calloc failed");
            while( true );
        }
        msg_chain_entry->prev_msg = NULL;
        msg_chain_entry->next_msg = NULL;
        time( &msg_chain_entry->timestamp );
        strcpy( (char*)msg_chain_entry->msg, msg );
    }
    
    /*
     * add msg_chain_entry to the chain
     */
    if ( msg_chain->first_msg_chain_entry == NULL ) {
        msg_chain->first_msg_chain_entry = msg_chain_entry;
        msg_chain->entrys++;
    }
    else {
        msg_chain_entry_t *current_msg_chain_entry = msg_chain->first_msg_chain_entry;

        /*
         * find the last entry in the chain
         */
        while( current_msg_chain_entry->next_msg != NULL ) {
            current_msg_chain_entry = current_msg_chain_entry->next_msg;
        }
        /*
         * add a new msg_chain_entry
         */
        current_msg_chain_entry->next_msg = msg_chain_entry;
        msg_chain_entry->prev_msg = current_msg_chain_entry;
        msg_chain->entrys++;
    }
    return( msg_chain );
}

bool msg_chain_delete_msg_entry( msg_chain_t *msg_chain, int32_t entry ) {
    int32_t msg_counter = 0;
    bool retval = false;

    /*
     * check if msg chain exist
     */
    if ( msg_chain == NULL ) {
        return( retval );
    }
    /*
     * check if msg has an entry
     */
    if ( msg_chain->first_msg_chain_entry == NULL ) {
        return( retval );
    }
    /*
     * check if entry exist
     */
    if ( entry > msg_chain->entrys ) {
        return( retval );
    }

    msg_chain_entry_t *msg_chain_entry = msg_chain->first_msg_chain_entry;
    msg_chain_entry_t *prev_msg_chain_entry = NULL;
    msg_chain_entry_t *next_msg_chain_entry = NULL;

    /*
     * find the entry in the chain
     */
    do {
        if ( entry == msg_counter ) {
            /*
             * get the prev and next msg chain entry if exsist
             */
            if ( msg_chain_entry->prev_msg != NULL ) {
                prev_msg_chain_entry = msg_chain_entry->prev_msg;
            }
            if ( msg_chain_entry->next_msg != NULL ) {
                next_msg_chain_entry = msg_chain_entry->next_msg;
            }

            /*
             * free allocated msg
             */
            free( (void *)msg_chain_entry->msg );

            /*
             * delete chain entry in all conditions
             */
            if ( prev_msg_chain_entry && next_msg_chain_entry ) {
                prev_msg_chain_entry->next_msg = next_msg_chain_entry;
                next_msg_chain_entry->prev_msg = prev_msg_chain_entry;
            }
            else if( !prev_msg_chain_entry && next_msg_chain_entry ) {
                next_msg_chain_entry->prev_msg = NULL;
                msg_chain->first_msg_chain_entry = next_msg_chain_entry;
            }
            else if( prev_msg_chain_entry && !next_msg_chain_entry ) {
                prev_msg_chain_entry->next_msg = NULL;
            }
            else {
                msg_chain->first_msg_chain_entry = NULL;
            }

            /*
             * free allocated msg chain entry
             */
            free( msg_chain_entry );
            msg_chain->entrys--;
            retval = true;
            break;
        }

        /*
         * get next chain entry
         */
        if ( msg_chain_entry->next_msg != NULL ) {
            msg_counter++;
            msg_chain_entry = msg_chain_entry->next_msg;
        }
        else {
            retval = false;
            break;
        }

    } while ( true );

    return( retval );
}

time_t* msg_chain_get_msg_timestamp_entry( msg_chain_t *msg_chain, int32_t entry ) {
    time_t* retval = NULL;
    int32_t msg_counter = 0;

    /*
     * check if msg chain exist
     */
    if ( msg_chain == NULL ) {
        return( retval );
    }
    /*
     * check if msg has an entry
     */
    if ( msg_chain->first_msg_chain_entry == NULL ) {
        return( retval );
    }
    /*
     * check if entry exist
     */
    if ( entry > msg_chain->entrys ) {
        return( retval );
    }

    /*
     * get the first entry in the chain
     */
    msg_chain_entry_t *msg_chain_entry = msg_chain->first_msg_chain_entry;
    /*
     * find the entry in the chain
     */
    do {
        if ( entry == msg_counter ) {
            retval = &msg_chain_entry->timestamp;
            break;
        }
        if ( msg_chain_entry->next_msg != NULL ) {
            msg_counter++;
            msg_chain_entry = msg_chain_entry->next_msg;
        }
        else {
            break;
        }
    } while ( true );

    return( retval );
}

const char* msg_chain_get_msg_entry( msg_chain_t *msg_chain, int32_t entry ) {
    const char* retval = NULL;
    int32_t msg_counter = 0;

    /*
     * check if msg chain exist
     */
    if ( msg_chain == NULL ) {
        return( retval );
    }
    /*
     * check if msg has an entry
     */
    if ( msg_chain->first_msg_chain_entry == NULL ) {
        return( retval );
    }
    /*
     * check if entry exist
     */
    if ( entry > msg_chain->entrys ) {
        return( retval );
    }
    /*
     * get the first entry in the chain
     */
    msg_chain_entry_t *msg_chain_entry = msg_chain->first_msg_chain_entry;
    /*
     * find the entry in the chain
     */
    do {
        if ( entry == msg_counter ) {
            retval = msg_chain_entry->msg;
            break;
        }
        if ( msg_chain_entry->next_msg != NULL ) {
            msg_counter++;
            msg_chain_entry = msg_chain_entry->next_msg;
        }
        else {
            break;
        }
    } while ( true );

    return( retval );
}

int32_t msg_chain_get_entrys( msg_chain_t *msg_chain ) {
    int32_t msg_counter = 0;
    /*
     * check if msg chain exist
     */
    if ( msg_chain == NULL ) {
        return( msg_counter );
    }
    /*
     * check if msg has an entry
     */
    if ( msg_chain->first_msg_chain_entry == NULL ) {
        return( msg_counter );
    }

    msg_counter++;
    
    msg_chain_entry_t *msg_chain_entry = msg_chain->first_msg_chain_entry;

    /*
     * count the entry in the chain
     */
    while( msg_chain_entry->next_msg != NULL ) {
        msg_counter++;
        msg_chain_entry = msg_chain_entry->next_msg;
    }
    return( msg_counter );
}

msg_chain_t *msg_chain_delete( msg_chain_t *msg_chain ) {
    int32_t entrys = 0;

    /*
     * check if msg chain exist
     */
    if ( msg_chain == NULL ) {
        return( NULL );
    }
    /*
     * get number of entrys
     */
    entrys = msg_chain_get_entrys( msg_chain );

    /*
     * delete all msg_chain entrys
     */
    for ( int32_t i = 0 ; i < entrys ; i++ ) {
        if ( !msg_chain_delete_msg_entry( msg_chain, 0 ) ) {
            log_e("delete msg from msg_chain failed");
            return( msg_chain );
        }
    }

    free( msg_chain );

    return( NULL );
}

void msg_chain_printf_msg_chain( msg_chain_t *msg_chain ) {
    /*
     * check if msg chain exist
     */
    if ( msg_chain == NULL ) {
        return;
    }
    /*
     * check if msg has an entry
     */
    if ( msg_chain->first_msg_chain_entry == NULL ) {
        return;
    }
    
    int32_t msg_counter = 0;
    msg_chain_entry_t *msg_chain_entry = msg_chain->first_msg_chain_entry;

    /*
     * count and printf entry in the chain
     */
    while( true ) {
        log_i("msg %d: %p < \"%s\" > %p", msg_counter, msg_chain_entry->prev_msg, msg_chain_entry->msg, msg_chain_entry->next_msg );
        if ( msg_chain_entry->next_msg == NULL ) {
            break;
        }
        msg_counter++;
        msg_chain_entry = msg_chain_entry->next_msg;
    };
}