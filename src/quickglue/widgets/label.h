#ifndef LABEL_H_
#define LABEL_H_

#include "widget.h"

class Label : public Widget {
public:
  Label(){};
  /** Create a label from lvgl object */
  Label(lv_obj_t* handle);
  /** Create a label with text */
  Label(const Widget* parent, const char * txt = NULL);

  virtual void createObject(lv_obj_t* parent);
  
  // Set label text
  void text(const char * txt);
  
  // Align label text 
  void alignText(lv_label_align_t mode);

  constexpr static const char* TypeName = "lv_label";
};

#endif