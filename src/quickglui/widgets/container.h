/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "widget.h"

/*
* @brief Represent Container LVGL widget.
* Documentation:
* https://docs.lvgl.io/latest/en/html/widgets/cont.html
*/
class Container : public Widget {
public:
  Container(){};
  Container(Widget* parent);
  /** Create from lvgl object */
  Container(lv_obj_t* handle);

  void autoLayout(lv_layout_t value);

  constexpr static const char* TypeName = "lv_cont";

protected:
  virtual void createObject(lv_obj_t* parent);
};

#endif