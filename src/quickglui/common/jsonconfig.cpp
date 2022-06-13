/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "jsonconfig.h"
#include "jsonoptions.h"

#include "utils/json_psram_allocator.h"
#include "utils/alloc.h"

JsonConfig::JsonConfig(const char* configFileName) : BaseJsonConfig(configFileName) {
  count = 0;
}

JsonConfig::~JsonConfig() {
  for (int i = 0; i < count; i++) {
    delete options[i];
  }
  count = 0;
}

JsonBoolOption& JsonConfig::addBoolean(const char* optionName, bool defValue) {
  auto ptr = MALLOC_ASSERT(sizeof(JsonBoolOption),"JsonConfig::addBoolean allocation failed");
  options[count] = new(ptr) JsonBoolOption(optionName, defValue);
  return *(JsonBoolOption*)options[count++];
}

JsonStringOption& JsonConfig::addString(const char* optionName, int maxValueLength, const char* defValue) {
  auto ptr = MALLOC_ASSERT(sizeof(JsonStringOption),"JsonConfig::addString allocation failed");
  options[count] = new(ptr) JsonStringOption(optionName, maxValueLength, defValue);
  return *(JsonStringOption*)options[count++];
}


bool JsonConfig::getBoolean(const char* optionName, bool defValue) {
  for (int i = 0; i < count; i++) {
    if (options[i]->type() == OptionDataType::BoolOption && strcmp(options[i]->name, optionName) == 0)
      return ((JsonBoolOption*)options[i])->value;
  }
  return defValue;
}

const char* JsonConfig::getString(const char* optionName, const char* defValue) {
  for (int i = 0; i < count; i++) {
    if (options[i]->type() == OptionDataType::StringOption && strcmp(options[i]->name, optionName) == 0)
      return ((JsonStringOption*)options[i])->value;
  }
  return defValue;
}

void JsonConfig::applyFromUI() {
  for (int i = 0; i < count; i++) {
    options[i]->applyFromUI();
  }
}

bool JsonConfig::onSave(JsonDocument& document) {
  for (int i = 0; i < count; i++) {
    options[i]->save(document);
  }
  
  if (processHandler != nullptr)
    processHandler(*this);

  return true;
}

bool JsonConfig::onLoad(JsonDocument& document) {
  for (int i = 0; i < count; i++) {
    options[i]->load(document);
  }
  
  if (processHandler != nullptr)
    processHandler(*this);
  
  return true;
}

bool JsonConfig::onDefault( void ) {
  return true;
}

void JsonConfig::onLoadSaveHandler(SettingsAction saveSettingsHandler) {
  processHandler = saveSettingsHandler;
}
