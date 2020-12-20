/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "switch.h"

Switch::Switch(lv_obj_t* handle) {
  assign(handle);
}

Switch::Switch(const Widget* parent) {
  createObject(parent->handle());
}

Switch::Switch(const Widget* parent, bool state) {
  createObject(parent->handle());
  value(state);
}

void Switch::createObject(lv_obj_t* parent) {
  assign(lv_switch_create(parent, NULL));
  lv_obj_add_protect(native, LV_PROTECT_CLICK_FOCUS);
}

void Switch::value(bool val) {
    if (val)
        lv_switch_on(native, LV_ANIM_OFF);
    else
        lv_switch_off(native, LV_ANIM_OFF);
}
bool Switch::value() {
  return lv_switch_get_state(native);
}
