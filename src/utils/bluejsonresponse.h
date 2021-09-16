/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/
 
#ifndef BTJSONRESPONSE_H
#define BTJSONRESPONSE_H

#include "ArduinoJson.h"
#include "utils/bluejsonrequest.h"
#include "hardware/blectl.h"
#include "utils/json_psram_allocator.h"

#define BLUETOOTH_MAX_RESPONSE_SIZE 512
#define BLUETOOTH_MAX_TRANSMISSION_SIZE 240

class BluetoothJsonResponse : public SpiRamJsonDocument
{
public:
    BluetoothJsonResponse() : SpiRamJsonDocument(BLUETOOTH_MAX_JSON_BUFFER_SIZE) {}

    BluetoothJsonResponse(const char* app, const char* command = nullptr) : SpiRamJsonDocument(BLUETOOTH_MAX_JSON_BUFFER_SIZE) {
        if (app != nullptr) {
            (*this)["app"] = app;
        }
        if (command != nullptr) {
            (*this)["r"] = command;
        }
    }

    BluetoothJsonResponse(BluetoothJsonRequest& sourceRequest, const char* command = nullptr) : SpiRamJsonDocument(BLUETOOTH_MAX_JSON_BUFFER_SIZE) {
        (*this)["app"] = sourceRequest["app"];
        if (command != nullptr) {
            (*this)["r"] = command;
        }
        else {
            (*this)["r"] = sourceRequest["r"];
        }
    }

    void send() {
        char buf[BLUETOOTH_MAX_RESPONSE_SIZE+2];
        auto len = serializeJson(*this, buf + 1, BLUETOOTH_MAX_RESPONSE_SIZE) + 1;
        buf[0] = buf[len++] = '\x03';
        buf[len] = '\0';
        int maxp = BLUETOOTH_MAX_TRANSMISSION_SIZE;

        if (len < maxp) {
            blectl_send_msg(buf);
        }
        else if (BLUETOOTH_MAX_RESPONSE_SIZE > maxp) {
            // Probably should be resolved in blectl_send_msg:
            char saved = buf[maxp];
            buf[maxp] = '\0';
            blectl_send_msg(buf);
            
            buf[maxp] = saved;
            blectl_send_msg(buf + maxp);
        }
    }

    // template <typename T>
    // void BluetoothJsonResponse::addArray(const char* keyName, T* array) {
    //   JsonArray& target = root.createNestedArray(keyName);
    //   copyArray(array, target);
    // }
};

#endif

