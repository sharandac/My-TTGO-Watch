#include "filepath_convert.h"

#ifdef NATIVE_64BIT
    #include <iostream>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
    #include "utils/logging.h"
#else
    #include <Arduino.h>

    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #endif
#endif

char *filepath_convert( char * dst_str, int max_len, const char* local_path ) {

    #ifdef NATIVE_64BIT
        if ( getenv("HOME") )
            snprintf( dst_str, max_len, "%s/.hedge/%s", getpwuid(getuid())->pw_dir, local_path );
    #else
        snprintf( dst_str, max_len, "%s", local_path );
    #endif

    return( dst_str );
}
