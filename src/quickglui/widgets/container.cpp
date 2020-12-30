/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "container.h"

Container::Container(lv_obj_t* handle){
  assign(handle);
}
Container::Container(Widget* parent) {
  createObject(parent->handle());
}

void Container::createObject(lv_obj_t* parent) {
  assign(lv_cont_create(parent, NULL));
}

void Container::autoLayout(lv_layout_t value) {
  lv_cont_set_layout(handle(), value);
}