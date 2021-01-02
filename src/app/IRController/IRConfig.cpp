#include <IRremoteESP8266.h>
#include "IRConfig.h"
#include "hardware/alloc.h"

IRConfig::IRConfig() : BaseJsonConfig("ir-remote.json") {
    count = 0;
    // This file is too big for that!
    prettyJson = false;
}

InfraButton* IRConfig::add(const char* name) {
    void* pointer = MALLOC(sizeof(InfraButton));
    InfraButton* btn = new (pointer) InfraButton();
    btn->name = name;
    buttons[count++] = btn;
    return btn;
}

void IRConfig::del(const char* name) {
  bool found = false;
  for (int i = 0; i < count; i++) {
    if (buttons[i]->name == name) {
      if (buttons[i]->uiButton.isCreated()) {
        buttons[i]->uiButton.free();
      }
      delete buttons[i];
      found = true;
    }
    if (found)
      buttons[i] = buttons[i+1];
  }
  if (found)
    count--;
}

InfraButton* IRConfig::get(const char* name) {
  for (int i = 0; i < count; i++) {
    if (buttons[i]->name == name)
      return buttons[i];
  }
  return nullptr;
}

void IRConfig::sendListNames(BluetoothJsonResponse& response) {
  auto nestedArray = response.createNestedArray("v");
  for (int i = 0; i < count; i++) {
    nestedArray.add(buttons[i]->name);
  }
  response.send();
}

void IRConfig::sendButtonEdit(BluetoothJsonResponse& response, const char* name) {
  auto btn = get(name);
  if (btn != nullptr) {
    response["v"] = btn->name;
    response["m"] = (int)btn->mode;
    if (btn->mode == decode_type_t::RAW) {
      auto nestedArray = response.createNestedArray("raw");
      for (int i = 0; i < btn->rawLength; i++)
        nestedArray.add(btn->raw[i]);
    } else {
      response["hex"] = String(btn->code, 16);
    }
  }
  response.send();
}

bool IRConfig::onSave(JsonDocument& document) {
  auto pagesArray = document.createNestedArray("pages");
  auto main = pagesArray.createNestedObject();
  for (int i = 0; i < count; i++) {
    JsonObject btnRecord = main.createNestedObject(buttons[i]->name);
    btnRecord["m"] = buttons[i]->mode;
    String hex((uint32_t)buttons[i]->code, 16);
    btnRecord["hex"] = hex;
    if (buttons[i]->bits > 0 && buttons[i]->mode == decode_type_t::SONY) {
      btnRecord["bits"] = buttons[i]->bits;
    }
    if (buttons[i]->mode == decode_type_t::RAW) {
      auto rawArray = btnRecord.createNestedArray("raw");
      for (int j = 0; j < buttons[i]->rawLength; j++)
        rawArray.add(buttons[i]->raw[j]);
    }
  }
  document["defBtnHeight"] = defBtnHeight;
  document["defBtnWidth"] = defBtnWidth;
  document["defSpacing"] = defSpacing;

  return true;
}

bool IRConfig::onLoad(JsonDocument& document) {
  JsonArray pages = document["pages"].as<JsonArray>();
  if (pages.isNull() || pages.size() < 1) return false;
  
  JsonObject main = pages[0].as<JsonObject>(); // For now only first page supported
  if (main.isNull()) return false;

  for (JsonPair record : main) {
    if (record.value().isNull()) continue;
    // Create and load button
    log_d("Loading ir button: %s", record.key().c_str());
    auto btn = add(record.key().c_str());
    JsonObject configuration = record.value().as<JsonObject>();
    btn->loadFrom(configuration);
  }

  if (document.containsKey("defBtnHeight"))
    defBtnHeight = document["defBtnHeight"];
  if (document.containsKey("defBtnWidth"))
    defBtnWidth = document["defBtnWidth"];
  if (document.containsKey("defSpacing"))
    defSpacing = document["defSpacing"];
  if (defBtnHeight < 12 || defBtnHeight > 200)
    defBtnHeight = 12;
  if (defBtnWidth < 50 || defBtnWidth > LV_HOR_RES-10)
    defBtnWidth = 50;
  if (defSpacing < 0 || defSpacing > 100)
    defSpacing = 5;

  return true;
}