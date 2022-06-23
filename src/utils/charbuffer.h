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
#ifndef _CHAR_BUFFER_H
    #define _CHAR_BUFFER_H

    #include "config.h"

    #define INITIAL_CAPACITY 64
    #define CHUNK_CAPACITY 16

    /**
     * @brief A resizable char based buffer
     * 
     * This class try to reduce memory fragmentation when we need a resisable buffer.
     * Memory is allocated by chunks.
     */
    class CharBuffer {
        public:
            /**
             * @brief Default constructor
             */
            CharBuffer();
            /**
             * @brief Append a char to the buffer
             * 
             * @param c char to append
             */
            void append(char c);
            /**
             * @brief enabled char filter
             * 
             * @param enable  true means enabled
             */
            void setFilter( bool enable ) { filter = enable; }
            /**
             * @brief Clear the buffer
             */
            void clear();
            /**
             * @brief Erase trailing part of the buffer
             * 
             * @param pos starting position to erase
             */
            void erase(size_t pos =  0);
            /**
             * @brief C string buffer
             * 
             * @return pointer to C string
             */
            inline const char *c_str() { return msg; }
            /**
             * @brief length of the effective content
             * 
             * @return the size
             */
            inline size_t length() { return size; }
        private:
            /**
             * @brief if true, all non printable char filtered out
             */
            bool filter;
            /**
             * @brief pointer to the buffer
             */
            char* msg;
            /**
             * @brief current capacity
             */
            size_t capacity;
            /**
             * @brief current size
             */
            size_t size;
    };

#endif // _CHAR_BUFFER_H