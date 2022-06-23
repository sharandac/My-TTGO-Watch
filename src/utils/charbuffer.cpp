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
#include "charbuffer.h"
#include "utils/alloc.h"

CharBuffer::CharBuffer( void ) : filter(true), msg(NULL), capacity(0), size(0) {}

void CharBuffer::append(char c) {
    if ( capacity == 0 )
        clear();
    
    if ( size + 2 > capacity ) {
        msg = (char *)REALLOC_ASSERT( msg, capacity + CHUNK_CAPACITY, "msg realloc fail" );
        capacity += CHUNK_CAPACITY;
    }
    size++;

    if( filter ) {
        if( !( ( c >= ' ' && c <= '~' ) || c == '\r' || c == '\n' || c == '\t') ) {
            c = '?';
        }
    }

    msg[ size - 1 ] = c;
    msg[ size ] = '\0';
}

void CharBuffer::clear( void ) {
    if( msg )
        free( msg );

    msg = (char *)CALLOC_ASSERT( INITIAL_CAPACITY, 1, "msg calloc fail" );
    capacity = INITIAL_CAPACITY;
    size = 0;

    msg[0] = '\0';
}

void CharBuffer::erase( size_t pos ) {
    if ( pos >= size )
        return;

    for ( int i = pos ; i < size ; i++ )
        msg[i] = '\0';

    size = pos;
}