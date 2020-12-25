/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef STYLE_H_
#define STYLE_H_

#include "../widgets/widget.h"

/*
* @brief Represent LVGL Style. Styles are used to set the appearance of the objects. Styles in lvgl are heavily inspired by CSS.
* Documentation:
* https://docs.lvgl.io/latest/en/html/overview/style.html
*/
class Style {
public:
  Style() {}
  Style(lv_style_t* handle, bool copy);
  //~Style();
  
  static Style Create(lv_style_t* handle, bool copy);

  Style(Style&& rightValue) = default;
  Style& operator=(Style&& rightValue) = default;

  // Style object should be stored by user. Disable implicit copy semantics:
  Style & operator=(const Style&) = delete;
  Style(const Style&) = delete;

  lv_style_t* handle() const;

  Style& background(lv_color_t color, lv_opa_t opacity = LV_OPA_100);
  Style& border(int width);
  Style& border(int width, lv_color_t color, lv_opa_t opacity = LV_OPA_100);
  
  Style& padding(int top, int right, int bottom, int left);
  Style& paddingInner(int inner);
  
  Style& textFont(lv_font_t* font);
  Style& textOpacity(lv_opa_t value);
  
private:
  lv_style_t native = {0};
};

#endif