/****************************************************************************
 *   Sep 21 12:13:51 2020
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
#ifndef _DECOMPRESS_H
    #define _DECOMPRESS_H

    typedef void (*ProgressCallback)( uint8_t progress );

    /**
     * @brief decompress a file from spiffs to spiffs
     * 
     * @param filename      pointer to a path/filename
     * @param dest          pointer to a destination path
     * @param cb            callback function for progress
     * @return true 
     * @return false 
     */
    bool decompress_file_into_spiffs( const char*filename, const char *dest, ProgressCallback cb );
    #ifdef NATIVE_64BIT
        bool decompress_stream_into_flash( void *stream, const char* md5, int32_t firmwaresize, ProgressCallback cb );
    #else
        bool decompress_stream_into_flash( Stream *stream, const char* md5, int32_t firmwaresize, ProgressCallback cb );
    #endif

#endif /* _DECOMPRESS_H */