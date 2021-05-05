#include "config.h"
#include <HTTPClient.h>
#include <Update.h>

#define DEST_FS_USES_SPIFFS
#include <ESP32-targz.h>

#include "decompress.h"

bool decompress_file_into_spiffs( const char*filename, const char *dest, ProgressCallback cb ) {
    TarGzUnpacker *TARGZUnpacker = new TarGzUnpacker();

    TARGZUnpacker->haltOnError( true ); // stop on fail (manual restart/reset required)
    TARGZUnpacker->setTarVerify( true ); // true = enables health checks but slows down the overall process
    TARGZUnpacker->setupFSCallbacks( targzTotalBytesFn, targzFreeBytesFn ); // prevent the partition from exploding, recommended
    TARGZUnpacker->setGzProgressCallback( BaseUnpacker::defaultProgressCallback ); // targzNullProgressCallback or defaultProgressCallback
    TARGZUnpacker->setLoggerCallback( BaseUnpacker::targzPrintLoggerCallback  );    // gz log verbosity
    TARGZUnpacker->setTarProgressCallback( BaseUnpacker::defaultProgressCallback ); // prints the untarring progress for each individual file
    TARGZUnpacker->setTarStatusProgressCallback( BaseUnpacker::defaultTarStatusProgressCallback ); // print the filenames as they're expanded
    TARGZUnpacker->setTarMessageCallback( BaseUnpacker::targzPrintLoggerCallback ); // tar log verbosity

    // or without intermediate file
    if( !TARGZUnpacker->tarGzExpander(tarGzFS, filename, tarGzFS, dest, nullptr ) ) {
        log_e("tarGzExpander+intermediate file failed with return code #%d\n", TARGZUnpacker->tarGzGetError() );
    }
}

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