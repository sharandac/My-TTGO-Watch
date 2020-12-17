#ifndef STYLE_H_
#define STYLE_H_

#include "../widgets/widget.h"

class Style {
public:
  Style() {}
  Style(lv_style_t* handle, bool copy);
  
  lv_style_t* handle() const;

  Style& background(lv_color_t color, lv_opa_t opacity = LV_OPA_100);
  Style& border(int width);
  Style& border(int width, lv_color_t color, lv_opa_t opacity = LV_OPA_100);
  
  Style& padding(int top, int right, int bottom, int left);
  Style& paddingInner(int inner);
  
private:
  lv_style_t native;
};

#endif