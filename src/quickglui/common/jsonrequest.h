/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef JSONREQUEST_H
#define JSONREQUEST_H

#include "ArduinoJson.h"
#include "hardware/json_psram_allocator.h"

class JsonRequest 
{
public:
  JsonRequest(size_t maxJsonBufferSize);
  ~JsonRequest();

  bool process(const char* url);

  JsonDocument& result();

  int httpCode() { return httpcode; }
  DeserializationError getDeserializationError() { return dsError; }

  tm completedAt() { return timeStamp; }
  String fromatCompletedAt(const char* format);

protected:
  int httpcode = -1;
  SpiRamJsonDocument document;
  time_t now;
  struct tm timeStamp;
  DeserializationError dsError;
};

#endif

