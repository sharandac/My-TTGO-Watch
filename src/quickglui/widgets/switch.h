/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef SWITCH_H_
#define SWITCH_H_

#include "widget.h"

/*
* @brief Represent Switch LVGL widget.
* Documentation:
* https://docs.lvgl.io/latest/en/html/widgets/switch.html
*/
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