/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef BASEJSONCONFIG_H_
#define BASEJSONCONFIG_H_

#define MAX_CONFIG_FILE_NAME_LENGTH 32

#include "ArduinoJson.h"

/**
 * @brief JSON configuration storage with bindings for variables and UI widgets
 */
class BaseJsonConfig {
public:
  BaseJsonConfig(const char* configFileName);
  /**
   * @brief Load settings from file
   */
  bool load();
  /**
   * @brief Save settings to file
   */
  bool save();
  /**
   * @brief print out json
   */
  void debugPrint();
  
protected:
  ////////////// Available for overloading: //////////////
  virtual bool onSave(JsonDocument& document) = 0;
  virtual bool onLoad(JsonDocument& document) = 0;
  virtual size_t getJsonBufferSize() { return 4096; }

protected:
  char fileName[MAX_CONFIG_FILE_NAME_LENGTH];
  bool prettyJson = true;
};

#endif