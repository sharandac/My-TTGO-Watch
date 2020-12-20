/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "jsonconfig.h"
#include "jsonoptions.h"

//#include <hardware/json_psram_allocator.h>
#include "hardware/json_psram_allocator.h"
#include <FS.h>

JsonConfig::JsonConfig(const char* configFileName) {
  if (configFileName[0] == '/')
    strlcpy(fileName, configFileName, MAX_CONFIG_FILE_NAME_LENGTH);
  else
  {
    fileName[0] = '/';
    strlcpy(fileName+1, configFileName, MAX_CONFIG_FILE_NAME_LENGTH);
  }
  
  count = 0;
}

JsonConfig::~JsonConfig() {
  for (int i = 0; i < count; i++) {
    delete options[i];
  }
  count = 0;
}

JsonBoolOption& JsonConfig::addBoolean(const char* optionName, bool defValue) {
  auto ptr = ps_malloc(sizeof(JsonBoolOption));
  options[count] = new(ptr) JsonBoolOption(optionName, defValue);
  return *(JsonBoolOption*)options[count++];
}

JsonStringOption& JsonConfig::addString(const char* optionName, int maxValueLength, const char* defValue) {
  auto ptr = ps_malloc(sizeof(JsonStringOption));
  options[count] = new(ptr) JsonStringOption(optionName, maxValueLength, defValue);
  return *(JsonStringOption*)options[count++];
}


bool JsonConfig::getBoolean(const char* optionName, bool defValue) {
  for (int i = 0; i < count; i++) {
    if (strcmp(options[i]->name, optionName) == 0 && options[i]->type() == OptionDataType::BoolOption)
      return ((JsonBoolOption*)options[i])->value;
  }
  return defValue;
}

const char* JsonConfig::getString(const char* optionName, const char* defValue) {
  for (int i = 0; i < count; i++) {
    if (strcmp(options[i]->name, optionName) == 0 && options[i]->type() == OptionDataType::StringOption)
      return ((JsonStringOption*)options[i])->value;
  }
  return defValue;
}

void JsonConfig::applyFromUI() {
  for (int i = 0; i < count; i++) {
    options[i]->applyFromUI();
  }
}

bool JsonConfig::save() {
  bool result = false;
  fs::File file = SPIFFS.open(fileName, FILE_WRITE );

  if (!file) {
      log_e("Can't open file: %s!", fileName);
  }
  else {
      auto size = getJsonBufferSize();
      SpiRamJsonDocument doc(size);
      result = onSave(doc);
      
      if (result == true && serializeJsonPretty( doc, file ) == 0) {
          log_e("Failed to write config file %s", fileName);
          result = false;
      }
      
      doc.clear();
  }
  file.close();
  if (result == true && processHandler != nullptr)
    processHandler(*this);
  
  return result;
}

bool JsonConfig::load() {
  bool result = false;
  if (SPIFFS.exists(fileName)) {
      fs::File file = SPIFFS.open(fileName, FILE_READ);
      if (!file) {
          log_e("Can't open file: %s!", fileName);
      }
      else {
          int filesize = file.size();
          SpiRamJsonDocument doc( filesize*4 );

          DeserializationError error = deserializeJson( doc, file );
          if ( error ) {
              log_e("update check deserializeJson() failed: %s, file: %s", error.c_str(), fileName );
          } else {
              result = onLoad(doc);

            if (result == true && processHandler != nullptr)
              processHandler(*this);
          }        
          doc.clear();
      }
      file.close();
  }
  return false;
}

bool JsonConfig::onSave(JsonDocument& document) {
  for (int i = 0; i < count; i++) {
    options[i]->save(document);
  }
  return true;
}
bool JsonConfig::onLoad(JsonDocument& document) {
  for (int i = 0; i < count; i++) {
    options[i]->load(document);
  }
  return true;
}

void JsonConfig::onLoadSaveHandler(SettingsAction saveSettingsHandler) {
  processHandler = saveSettingsHandler;
}