#ifndef EVENTS_H_
#define EVENTS_H_

#include "lvgl/lvgl.h"
#include <functional>

class Widget;
class TypeInfo;

typedef std::function<void(Widget& source)> WidgetAction;
typedef std::function<void(Widget& source, lv_event_t event)> WidgetEventHandler;
typedef std::function<void(Widget& target, const TypeInfo& type)> OnChildAddedHandler;

#endif