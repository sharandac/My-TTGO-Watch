/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "label.h"

Label::Label(lv_obj_t* handle){
  assign(handle);
  //lv_label_set_long_mode(native, LV_LABEL_LONG_BREAK);
}

Label::Label(const Widget* parent, const char* content) {
  createObject(parent->handle());
  
  if (content != NULL)
    lv_label_set_text(native, content);
  //lv_label_set_long_mode(native, LV_LABEL_LONG_BREAK);
}

void Label::createObject(lv_obj_t* parent) {
  assign(lv_label_create(parent, NULL));
}

Label& Label::text(const char * txt){
  lv_label_set_text(native, txt);
  return *this;
}
Label& Label::text(String txt){
  lv_label_set_text(native, txt.c_str());
  return *this;
}

Label& Label::alignText(lv_label_align_t mode) {
  lv_label_set_align(native, mode);
  return *this;
}

