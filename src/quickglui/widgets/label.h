/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef LABEL_H_
#define LABEL_H_

#include "widget.h"
#include <Arduino.h>

/*
* @brief Represent Label LVGL widget.
* Documentation:
* https://docs.lvgl.io/latest/en/html/widgets/label.html
*/
class Label : public Widget {
public:
  Label(){};
  /** Create a label from lvgl object */
  Label(lv_obj_t* handle);
  /** Create a label with text */
  Label(const Widget* parent, const char * txt = NULL);

  virtual void createObject(lv_obj_t* parent);
  
  // Set label text
  Label& text(const char * txt);
  Label& text(String txt);
  
  // Align label text 
  Label& alignText(lv_label_align_t mode);

  constexpr static const char* TypeName = "lv_label";
};

#endif