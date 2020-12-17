#include "Label.h"

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

void Label::text(const char * txt){
  lv_label_set_text(native, txt);
}

void Label::alignText(lv_label_align_t mode) {
  lv_label_set_align(native, mode);
}

