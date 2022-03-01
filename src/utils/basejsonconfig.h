/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef BASEJSONCONFIG_H_
#define BASEJSONCONFIG_H_

#define MAX_CONFIG_FILE_NAME_LENGTH 128

#include "ArduinoJson.h"

/**
 * @brief JSON configuration storage with bindings for variables and UI widgets
 */
class BaseJsonConfig {
public:
  BaseJsonConfig(const char* configFileName);
  /**
   * @brief Load settings from file with a custom json size, use 0 for automatic sizing
   */
  bool load( uint32_t size = 0 );
  /**
   * @brief Save settings to file with a custom json size, use 0 for automatic sizing
   */
  bool save( uint32_t size = 0 );
  /**
   * @brief print out json
   */
  void debugPrint();
  
protected:
  ////////////// Available for overloading: //////////////
  virtual bool onSave(JsonDocument& document) = 0;
  virtual bool onLoad(JsonDocument& document) = 0;
  virtual bool onDefault( void ) = 0;
  virtual size_t getJsonBufferSize() { return 8192; }

protected:
  char fileName[MAX_CONFIG_FILE_NAME_LENGTH];
  bool prettyJson = true;
};

#endif