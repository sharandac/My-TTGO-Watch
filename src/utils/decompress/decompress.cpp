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
#include "config.h"
#ifdef NATIVE_64BIT

#else

    #include <HTTPClient.h>
    #include <Update.h>
    #define DEST_FS_USES_SPIFFS
    #include <ESP32-targz.h>

#endif

#include "decompress.h"

bool decompress_file_into_spiffs( const char*filename, const char *dest, ProgressCallback cb ) {
    bool retval = false;
#ifdef NATIVE_64BIT

#else
    TarGzUnpacker *TARGZUnpacker = new TarGzUnpacker();

    TARGZUnpacker->haltOnError( false ); // stop on fail (manual restart/reset required)
    TARGZUnpacker->setTarVerify( false ); // true = enables health checks but slows down the overall process
    TARGZUnpacker->setupFSCallbacks( targzTotalBytesFn, targzFreeBytesFn ); // prevent the partition from exploding, recommended
    TARGZUnpacker->setGzProgressCallback( BaseUnpacker::defaultProgressCallback ); // targzNullProgressCallback or defaultProgressCallback
    TARGZUnpacker->setLoggerCallback( BaseUnpacker::targzPrintLoggerCallback  );    // gz log verbosity
    TARGZUnpacker->setTarProgressCallback( BaseUnpacker::defaultProgressCallback ); // prints the untarring progress for each individual file
    TARGZUnpacker->setTarStatusProgressCallback( BaseUnpacker::defaultTarStatusProgressCallback ); // print the filenames as they're expanded
    TARGZUnpacker->setTarMessageCallback( BaseUnpacker::targzPrintLoggerCallback ); // tar log verbosity
    TARGZUnpacker->setPsram( true );

    if( !TARGZUnpacker->tarGzExpander(tarGzFS, filename, tarGzFS, dest, nullptr ) ) {
        log_e("tarGzExpander+intermediate file failed with return code #%d\n", TARGZUnpacker->tarGzGetError() );
    }
    else {
        log_i("success!");
        retval = true;
    }
#endif
    return( retval );
}

#ifdef NATIVE_64BIT
    bool decompress_stream_into_flash( void *stream, const char* md5, int32_t firmwaresize, ProgressCallback cb ) {
        return( false );
    }
#else
    bool decompress_stream_into_flash( Stream *stream, const char* md5, int32_t firmwaresize, ProgressCallback cb ) {
        int32_t size = UPDATE_SIZE_UNKNOWN;
        bool retval = false;
        /**
         * start an unpacker instance, reister progress callback and put the stream in
         */
        GzUnpacker *GZUnpacker = new GzUnpacker();
        GZUnpacker->setGzProgressCallback( cb );
        GZUnpacker->setPsram( true );
        /**
         * if firmware size known set the right value
         */
        if ( firmwaresize != 0 )
            size = firmwaresize;
        /**
         * progress the stream
         */
        if( !GZUnpacker->gzStreamUpdater( stream, size, 0, false ) ) {
            log_e("gzStreamUpdater failed with return code #%d\n", GZUnpacker->tarGzGetError() );
        }
        else {
            retval = true;
        }
        return( retval );
    }
#endif
