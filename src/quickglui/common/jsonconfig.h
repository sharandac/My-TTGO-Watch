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

class BaseJsonConfig {
public:
  BaseJsonConfig(const char* configFileName);

  bool load();
  bool save();
  
  void debugPrint();
  
protected:
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

  JsonBoolOption& addBoolean(const char* optionName, bool defValue = false);
  JsonStringOption& addString(const char* optionName, int maxValueLength, const char* defValue = "");

  bool getBoolean(const char* optionName, bool defValue = false);
  const char* getString(const char* optionName, const char* defValue = "");

  void applyFromUI();

  int totalCount() { return count; }
  JsonOption* getOption(int id) { return options[id]; }

  void onLoadSaveHandler(SettingsAction saveSettingsHandler);

protected:
  virtual bool onLoad(JsonDocument& document);
  virtual bool onSave(JsonDocument& document);
  virtual size_t getJsonBufferSize() { return (count+1)*128; }

protected:
  JsonOption* options[MAX_OPTIONS_COUNT];
  int count = 0;
  SettingsAction processHandler;
};

#endif