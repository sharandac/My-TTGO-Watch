#ifndef TYPEINFO_H_
#define TYPEINFO_H_

#include "lvgl/lvgl.h"
#include "../widgets/widget.h"
#include "../widgets/widget.h"
#include "../widgets/label.h"
#include "../widgets/textarea.h"
#include "../widgets/switch.h"
#include "../widgets/button.h"

class TypeInfo {
public:
  TypeInfo(Widget* widget)
  {
    lv_obj_get_type(widget->handle(), &native);
  }
  TypeInfo(lv_obj_t* handle)
  {
    lv_obj_get_type(handle, &native);
  }

  const char* name() const { return native.type[0] != NULL ? native.type[0] : "lv_obj"; }
  const char* name(int id) const { return native.type[id]; }

  bool isButton() const { return strcmp(name(), Button::TypeName) == 0; }
  bool isLabel() const { return strcmp(name(), Label::TypeName) == 0; }
  bool isSwitch() const { return strcmp(name(), Switch::TypeName) == 0; }
  bool isTextArea() const { return strcmp(name(), TextArea::TypeName) == 0; }
  bool isWidget() const { return strcmp(name(), Widget::TypeName) == 0; }
  
  //lv_style_t* handle() const;

private:
  lv_obj_type_t native;
};

#endif