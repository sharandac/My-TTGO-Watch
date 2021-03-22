/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * Copyright  2021  Guilhem Bonnefille <guilhem.bonnefille@gmail.com>
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef ARC_H_
#define ARC_H_

#include "widget.h"
#include <Arduino.h>

/*
* @brief Represent Arc LVGL widget.
* Documentation:
* https://docs.lvgl.io/latest/en/html/widgets/arc.html
*/
class Arc : public Widget {
public:
  Arc(){};
  /** Create an arc from lvgl object */
  Arc(lv_obj_t* handle);
  /** Create an arc with text */
  Arc(const Widget* parent,  uint16_t start, uint16_t end);

  // Set arc values
  Arc& start(uint16_t start);
  Arc& end(uint16_t end);

  Arc& rotation(uint16_t rotation_angle);

  constexpr static const char* TypeName = "lv_arc";

protected:
  virtual void createObject(lv_obj_t* parent);
};

#endif