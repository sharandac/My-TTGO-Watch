#ifndef WIDGET_H_
#define WIDGET_H_

#include "lvgl/lvgl.h"
#include "../common/events.h"

class Style;

class Widget
{
public:
  Widget() {};
  Widget(const Widget* parent);
  Widget(lv_obj_t* handle);

  lv_obj_t* handle() const;
  void parent(const Widget& parent);
  //Widget parent();

  Widget& size(uint16_t width, uint16_t height);
  Widget& width(uint16_t width);
  Widget& height(uint16_t height);
  Widget& position(uint16_t width, uint16_t height);
  Widget& align(const Widget& base, lv_align_t align_mode, lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  //Widget& align(Widget* base, lv_align_t align_mode, lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  
  Widget& style(const Style& style, uint8_t part = LV_OBJ_PART_MAIN);
  Widget& style(lv_style_t* style, uint8_t part = LV_OBJ_PART_MAIN);

  Widget& childAddedHandler(OnChildAddedHandler handler);

  /*
  * Inside and outside align helpers
  **/
  Widget& alignInParentLeft(lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignInParentRight(lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignOutsideRight(const Widget& base, lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);
  Widget& alignOutsideBottom(const Widget& base, lv_coord_t x_offset = 0, lv_coord_t y_offset = 0);

  //template<class T> T add(T child);
  
  constexpr static const char* TypeName = "lv_obj";

protected:
  virtual void createObject(lv_obj_t* parent);
  virtual void assign(lv_obj_t* style);
  void fireParentHandlers();

  lv_obj_t *native = NULL;
};

#endif