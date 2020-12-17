#include "style.h"

Style::Style(lv_style_t* handle, bool copy) {
  if (copy)
  {
    lv_style_init(&native);
    lv_style_copy(&native, handle);
  }
  else
  {
    native.map = handle->map;
    #if LV_USE_ASSERT_STYLE
      native.sentinel = handle->sentinel;
    #endif
  }
}

lv_style_t* Style::handle() const {
  return (lv_style_t*)&native;
}

Style& Style::background(lv_color_t color, lv_opa_t opacity) {
  lv_style_set_bg_color(&native, LV_OBJ_PART_MAIN, color);
  lv_style_set_bg_opa(&native, LV_OBJ_PART_MAIN, opacity);
  return *this;
}

Style& Style::border(int width) {
  lv_style_set_border_width(&native, LV_OBJ_PART_MAIN, 0);
  return *this;
}

Style& Style::border(int width, lv_color_t color, lv_opa_t opacity) {
  lv_style_set_border_width(&native, LV_OBJ_PART_MAIN, 0);
  lv_style_set_border_color(&native, LV_OBJ_PART_MAIN, color);
  lv_style_set_border_opa(&native, LV_OBJ_PART_MAIN, opacity);
  return *this;
}

Style& Style::padding(int top, int right, int bottom, int left) {
  lv_style_set_pad_top(&native, LV_STATE_DEFAULT, top);
  lv_style_set_pad_bottom(&native, LV_STATE_DEFAULT, bottom);
  lv_style_set_pad_left(&native, LV_STATE_DEFAULT, left);
  lv_style_set_pad_right(&native, LV_STATE_DEFAULT, right);
  return *this;
}

Style& Style::paddingInner(int inner) {
  lv_style_set_pad_inner(&native, LV_STATE_DEFAULT, inner);
  return *this;
}
