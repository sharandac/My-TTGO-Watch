/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "widget.h"

class Container : public Widget {
public:
  Container(){};
  /** Create a label from lvgl object */
  Container(lv_obj_t* handle);

  virtual void createObject(lv_obj_t* parent);
  void autoLayout(lv_layout_t value);

  constexpr static const char* TypeName = "lv_cont";
};

#endif