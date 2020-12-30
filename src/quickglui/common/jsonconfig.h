/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef JSONCONFIG_H_
#define JSONCONFIG_H_

#define MAX_OPTION_NAME_LENGTH 12
#define MAX_OPTIONS_COUNT 24
#define MAX_CONFIG_FILE_NAME_LENGTH 32

#include "events.h"
#include "ArduinoJson.h"

class JsonOption;
class JsonBoolOption;
class JsonStringOption;

/*
* @brief JSON configuration storage with bindings for variables and UI widgets
*/
class BaseJsonConfig {
public:
  BaseJsonConfig(const char* configFileName);

  /*
  * @brief Load settings from file
  */
  bool load();

  /*
  * @brief Save settings to file
  */
  bool save();
  
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

class JsonConfig : public BaseJsonConfig  {
public:
  JsonConfig(const char* configFileName);
  ~JsonConfig();

  /*
  * @brief Add bool option to the settings list 
  */
  JsonBoolOption& addBoolean(const char* optionName, bool defValue = false);

  /*
  * @brief Add string option to the settings list 
  */
  JsonStringOption& addString(const char* optionName, int maxValueLength, const char* defValue = "");

  bool getBoolean(const char* optionName, bool defValue = false);
  const char* getString(const char* optionName, const char* defValue = "");

  /*
  * @brief Load option values from assigned UI widgets 
  */
  void applyFromUI();

  int totalCount() { return count; }
  JsonOption* getOption(int id) { return options[id]; }

  /*
  * @brief Set handler which will be called every time on load() ans save() actions
  */
  void onLoadSaveHandler(SettingsAction saveSettingsHandler);

protected:
  ////////////// Available for overloading: //////////////
  virtual bool onLoad(JsonDocument& document);
  virtual bool onSave(JsonDocument& document);
  virtual size_t getJsonBufferSize() { return (count+1)*128; }

protected:
  JsonOption* options[MAX_OPTIONS_COUNT];
  int count = 0;
  SettingsAction processHandler;
};

#endif