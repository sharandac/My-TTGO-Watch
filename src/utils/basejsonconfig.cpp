/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/
#ifdef NATIVE_64BIT
    #include <iostream>
    #include <fstream>
    #include <string.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <pwd.h>
#else
    #include <SPIFFS.h>
    #include <FS.h>
#endif

#include "basejsonconfig.h"
#include "utils/filepath_convert.h"
#include "json_psram_allocator.h"
#include "alloc.h"

BaseJsonConfig::BaseJsonConfig(const char* configFileName) {

#ifdef NATIVE_64BIT
    const char *homedir;
    char localpath[128] = "";
    /**
     * get local device path for config files
     */
    if ( configFileName[0] == '/') {
        snprintf( localpath, sizeof( localpath ), "spiffs%s", configFileName );
    }
    else {
        snprintf( localpath, sizeof( localpath ), "spiffs/%s", configFileName );
    }
    /**
     * convert from local device path to machine path
     */
    filepath_convert( fileName, sizeof( fileName ), localpath );
#else
    if (configFileName[0] == '/')
        strncpy( fileName, configFileName, MAX_CONFIG_FILE_NAME_LENGTH);
    else
    {
        fileName[0] = '/';
        strncpy(fileName+1, configFileName, MAX_CONFIG_FILE_NAME_LENGTH);
    }
#endif
}

bool BaseJsonConfig::load( uint32_t size ) {
    bool result = false;
    /*
     * load config if exist
     */
    /*
     * open file
     */
#ifdef NATIVE_64BIT
    std::fstream file;

    file.open( fileName, std::fstream::in );
#else
    fs::File file = SPIFFS.open(fileName, FILE_READ);
#endif
    /*
    * check if open was success
    */
    if (!file) {
        log_e("Can't open file: %s!", fileName);
    }
    else {
        if (size == 0) {
            /*
            * get filesize
            */
#ifdef NATIVE_64BIT
            file.seekg( 0, file.end );
            int filesize = file.tellg();
            file.seekg( 0, file.beg );
#else
            int filesize = file.size();
#endif
            /*
             * compute size to allocate
             */
            size = filesize*4;
        }
        /*
        * create json structure
        */
        SpiRamJsonDocument doc( size );
        DeserializationError error = deserializeJson( doc, file );
        /*
        * check if create json structure was successfull
        */
        if ( error || size == 0 ) {
            log_d("json config deserializeJson() failed: %s, file: %s", error.c_str(), fileName );
        }
        else {
            log_d("json config deserializeJson() success: %s, file: %s", error.c_str(), fileName );
            result = onLoad(doc);
        }
        doc.clear();
    }
    file.close();
    /*
     * check if read from json is failed
     */
    if ( !result ) {
        log_w("reading json failed, call defaults, file: %s", fileName );
        result = onDefault();
    }
    return result;
}

bool BaseJsonConfig::save( uint32_t size ) {
    bool result = false;
#ifdef NATIVE_64BIT
    std::fstream file;

    file.open(fileName, std::fstream::out );
#else
    fs::File file = SPIFFS.open(fileName, FILE_WRITE );
#endif
    if (!file) {
        log_e("Can't open file: %s!", fileName);
    }
    else {
        if (size == 0) {
            size = getJsonBufferSize();
        }
        SpiRamJsonDocument doc( size );
        result = onSave(doc);

        if ( doc.overflowed() ) {
            log_e("json (%s) to large, some value are missing. use another size", fileName );
        }
        
        size_t outSize = 0;
        if (prettyJson)
            outSize = serializeJsonPretty(doc, file);
        else
            outSize = serializeJson(doc, file);

        if (result == true && outSize == 0) {
            log_e("Failed to write config file %s", fileName);
            result = false;
        }
        else {
            log_d("json config serializeJson() success: %s", fileName );
        }
        
        doc.clear();
    }
    file.close();
    return result;
}

void BaseJsonConfig::debugPrint() {
/*
    auto size = getJsonBufferSize();
    SpiRamJsonDocument doc(size);
    bool result = onSave(doc);

    if ( result ) {
        serializeJsonPretty(doc, Serial );
    }
*/
}
