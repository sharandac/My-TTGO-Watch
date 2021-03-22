/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * Copyright  2021  Guilhem Bonnefille <guilhem.bonnefille@gmail.com>
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "arc.h"

Arc::Arc(lv_obj_t* handle){
  assign(handle);
}

Arc::Arc(const Widget* parent, uint16_t start, uint16_t end) {
  createObject(parent->handle());
  
  lv_arc_set_bg_angles(native, start, end);
}

void Arc::createObject(lv_obj_t* parent) {
  assign(lv_arc_create(parent, NULL));
}

Arc& Arc::start(uint16_t start){
  lv_arc_set_start_angle(native, start);
  return *this;
}

Arc& Arc::end(uint16_t end){
  lv_arc_set_end_angle(native, end);
  return *this;
}

Arc& Arc::rotation(uint16_t rotation_angle){
  lv_arc_set_rotation(native, rotation_angle);
  return *this;
}
