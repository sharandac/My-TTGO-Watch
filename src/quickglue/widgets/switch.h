#ifndef SWITCH_H_
#define SWITCH_H_

#include "widget.h"

class Switch : public Widget {
public:
  Switch(){};
  /** Create a Switch from lvgl object */
  Switch(lv_obj_t* handle);
  
  Switch(const Widget* parent);
  Switch(const Widget* parent, bool state);

  virtual void createObject(lv_obj_t* parent);
  
  void value(bool val);
  bool value();

  constexpr static const char* TypeName = "lv_switch";
};

#endif