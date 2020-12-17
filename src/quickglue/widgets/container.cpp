#include "container.h"

Container::Container(lv_obj_t* handle){
  assign(handle);
}

void Container::createObject(lv_obj_t* parent) {
  assign(lv_cont_create(parent, NULL));
}

void Container::autoLayout(lv_layout_t value) {
  lv_cont_set_layout(handle(), value);
}