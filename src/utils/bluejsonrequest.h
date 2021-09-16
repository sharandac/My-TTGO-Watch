/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/
 
#ifndef BTJSONREQUEST_H
#define BTJSONREQUEST_H

#include "ArduinoJson.h"
#include "utils/json_psram_allocator.h"

#ifdef NATIVE_64BIT
    #include <string>
    using namespace std;
    #define String string
#endif

#define BLUETOOTH_MAX_JSON_BUFFER_SIZE 4096

class BluetoothJsonRequest : public SpiRamJsonDocument {
public:
    BluetoothJsonRequest(const char* message) : SpiRamJsonDocument(BLUETOOTH_MAX_JSON_BUFFER_SIZE) {
        dsError = deserializeJson(*this, message);
        if (dsError) {
            log_e("deserializeJson() failed: %s", dsError.c_str());
            clear();
        }
    }

    BluetoothJsonRequest(const char* message, size_t jsonBufferSize) : SpiRamJsonDocument(jsonBufferSize) {
        dsError = deserializeJson(*this, message);
        if (dsError) {
            log_e("deserializeJson() failed: %s", dsError.c_str());
            clear();
        }
    }

    bool isValid() { return !dsError; }
    DeserializationError getDeserializationError() { return dsError; }

    bool isConfiguration() { return isEqualKeyValue("t", "conf"); }
    bool isRequest() { return isEqualKeyValue("t", "req");  }

    bool isForApplication(const char* appName) { return isEqualKeyValue("app", appName); }
    String command() { return (*this)["r"]; } // What requested

    bool isEqualKeyValue(const char* key, const char* value) { return isValid() && containsKey(key) && strcmp((*this)[key], value) == 0; }
    bool isEqualKeyValue(const char* key, bool value) { return isValid() && containsKey(key) && (*this)[key] == value; }
  
protected:
    DeserializationError dsError;
};

#endif

