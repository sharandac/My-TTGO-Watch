/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef TYPEINFO_H_
#define TYPEINFO_H_

#include "lvgl/lvgl.h"
#include "../widgets/widget.h"
#include "../widgets/label.h"
#include "../widgets/textarea.h"
#include "../widgets/switch.h"
#include "../widgets/button.h"

/*
* @brief Represent object's and its ancestors type.
* See lv_obj_get_type
*/
class TypeInfo {
public:
  TypeInfo(Widget* widget) {
    lv_obj_get_type(widget->handle(), &native);
  }
  TypeInfo(lv_obj_t* handle) {
    lv_obj_get_type(handle, &native);
  }

  const char* name() const { return native.type[0]; }
  const char* name(int id) const { return native.type[id]; }

  bool isButton() const { return isTextButton() || isImageButton(); }
  bool isTextButton() const { return strcmp(name(), Button::TypeNameWithText) == 0; }
  bool isImageButton() const { return strcmp(name(), Button::TypeNameWithImage) == 0; }
  bool isLabel() const { return strcmp(name(), Label::TypeName) == 0; }
  bool isSwitch() const { return strcmp(name(), Switch::TypeName) == 0; }
  bool isTextArea() const { return strcmp(name(), TextArea::TypeName) == 0; }
  bool isWidget() const { return strcmp(name(), Widget::TypeName) == 0; }
  
private:
  lv_obj_type_t native;
};

#endif