#include "filepath_convert.h"

#ifdef NATIVE_64BIT
    #include <iostream>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
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
        char hedge_config_path[512] = "";
        struct stat st = {0};

        /**
         * check config path
         */
        if ( getenv("HOME") )
            snprintf( hedge_config_path, sizeof( hedge_config_path ), "%s/.hedge", getpwuid(getuid())->pw_dir );
        /**
         * create config dir if not exist
         */
        if ( stat( hedge_config_path, &st ) == -1) {
            log_i("create config path and dir");
            mkdir( hedge_config_path, 0700 );
            snprintf( hedge_config_path, sizeof( hedge_config_path ), "%s/.hedge/spiffs", getpwuid(getuid())->pw_dir );
            mkdir( hedge_config_path, 0700 );
            snprintf( hedge_config_path, sizeof( hedge_config_path ), "%s/.hedge/sd", getpwuid(getuid())->pw_dir );
            mkdir( hedge_config_path, 0700 );
        }

        if ( getenv("HOME") )
            snprintf( dst_str, max_len, "%s/.hedge/%s", getpwuid(getuid())->pw_dir, local_path );
    #else
        snprintf( dst_str, max_len, "%s", local_path );
    #endif

    return( dst_str );
}
