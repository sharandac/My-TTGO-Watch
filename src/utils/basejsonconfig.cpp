/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "basejsonconfig.h"

//#include <hardware/json_psram_allocator.h>
#include "json_psram_allocator.h"
#include "alloc.h"
#include <FS.h>

BaseJsonConfig::BaseJsonConfig(const char* configFileName) {
  if (configFileName[0] == '/')
    strlcpy(fileName, configFileName, MAX_CONFIG_FILE_NAME_LENGTH);
  else
  {
    fileName[0] = '/';
    strlcpy(fileName+1, configFileName, MAX_CONFIG_FILE_NAME_LENGTH);
  }
}

bool BaseJsonConfig::load() {
    bool result = false;
    /*
     * load config if exsits
     */
    if ( SPIFFS.exists(fileName) ) {
        /*
         * open file
         */
        fs::File file = SPIFFS.open(fileName, FILE_READ);
        /*
         * check if open was success
         */
        if (!file) {
            log_e("Can't open file: %s!", fileName);
        }
        else {
            /*
             * get filesize
             */
            int filesize = file.size();
            /*
             * create json structure
             */
            SpiRamJsonDocument doc( filesize*4 );
            DeserializationError error = deserializeJson( doc, file );
            /*
             * check if create json structure was successfull
             */
            if ( error || filesize == 0 ) {
                log_e("json config deserializeJson() failed: %s, file: %s", error.c_str(), fileName );
            }
            else {
                log_i("json config deserializeJson() success: %s, file: %s", error.c_str(), fileName );
                result = onLoad(doc);
            }
            doc.clear();
        }
        file.close();
    }
    /*
     * check if read from json is failed
     */
    if ( !result ) {
        log_i("reading json failed, call defaults, file: %s", fileName );
        result = onDefault();
    }

    return result;
}

bool BaseJsonConfig::save() {
    bool result = false;
    fs::File file = SPIFFS.open(fileName, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", fileName);
    }
    else {
        auto size = getJsonBufferSize();
        SpiRamJsonDocument doc(size);
        result = onSave(doc);
        
        size_t outSize = 0;
        if (prettyJson)
        outSize = serializeJsonPretty(doc, file);
        else
        outSize = serializeJson(doc, file);

        if (result == true && outSize == 0) {
            log_e("Failed to write config file %s", fileName);
            result = false;
        }
        
        doc.clear();
    }
    file.close();

    return result;
}

void BaseJsonConfig::debugPrint() {
    auto size = getJsonBufferSize();
    SpiRamJsonDocument doc(size);
    bool result = onSave(doc);
    if ( result ) {
        serializeJsonPretty(doc, Serial);
    }
}
