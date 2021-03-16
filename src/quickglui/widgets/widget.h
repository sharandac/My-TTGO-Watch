/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef WIDGET_H_
#define WIDGET_H_

#include "lvgl/lvgl.h"
#include "../common/events.h"

class Style;

/*
* @brief Represent base LVGL object.
* Documentation:
* https://docs.lvgl.io/latest/en/html/widgets/obj.html
*/
class Widget
{
public:
  Widget() {};
  Widget(const Widget* parent);
  Widget(lv_obj_t* handle);

  lv_obj_t* handle() const;
  bool isCreated() const;
  virtual void free();
  //void parent(const Widget& parent);
  //Widget parent();

  Widget& size(uint16_t width, uint16_t height);
  Widget& width(uint16_t width);
  Widget& height(uint16_t height);
  Widget& position(uint16_t width, uint16_t height);
  Widget& align(const Widget& base, lv_align_t align_mode, lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  //Widget& align(Widget* base, lv_align_t align_mode, lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignx(const Widget& base, lv_align_t align_mode, lv_coord_t x_offset = 0);
  Widget& aligny(const Widget& base, lv_align_t align_mode, lv_coord_t y_offset = 0);
  
  Widget& style(const Style& addStyle, bool resetStyleList);
  Widget& style(const Style& addStyle, uint8_t part = LV_OBJ_PART_MAIN, bool resetStyleList=false);
  Widget& style(lv_style_t* addStyle, uint8_t part = LV_OBJ_PART_MAIN, bool resetStyleList=false);

  Widget& childAddedHandler(OnChildAddedHandler handler);

  /*
  * Inside and outside align helpers
  **/
  Widget& alignInParentCenter(lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignInParentLeftMid(lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignInParentTopLeft(lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignInParentRightMid(lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignInParentTopRight(lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignOutsideRightMid(const Widget& base, lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignOutsideBottomMid(const Widget& base, lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignOutsideBottomLeft(const Widget& base, lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);

  //template<class T> T add(T child);
  
  constexpr static const char* TypeName = "lv_obj";

protected:
  virtual void createObject(lv_obj_t* parent);
  virtual void assign(lv_obj_t* style);
  void fireParentHandlers();

  lv_obj_t *native = nullptr;
};

#endif