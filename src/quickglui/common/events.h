/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef EVENTS_H_
#define EVENTS_H_

#include "lvgl/lvgl.h"
#include <functional>

enum SyncRequestSource {
  ApplicationRequest = 1,
  ConnectionEvent = 2,
  UserDefinedRequest = 4,
  IsRequired = ApplicationRequest | ConnectionEvent | UserDefinedRequest,
  AllFlagsValues = UINT8_MAX
};

class Widget;
class TypeInfo;
class JsonConfig;

typedef std::function<void(Widget& source)> WidgetAction;
typedef std::function<void(Widget& source, lv_event_t event)> WidgetEventHandler;
typedef std::function<void(Widget& child, const TypeInfo& type)> OnChildAddedHandler;
typedef std::function<void(SyncRequestSource source)> SynchronizeAction;
typedef std::function<void(JsonConfig& conffig)> SettingsAction;

#endif