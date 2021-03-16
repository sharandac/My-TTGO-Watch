/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "widget.h"
#include "../internal/widgetmanager.h"
#include "../common/style.h"
#include "../common/typeinfo.h"
#include <config.h>

Widget::Widget(lv_obj_t* handle)
{
  assign(handle);
}

Widget::Widget(const Widget* parent)
{
  createObject(parent->handle());
}

void Widget::createObject(lv_obj_t* parent){
  assign(lv_obj_create(parent, NULL));
}

void Widget::assign(lv_obj_t* newHandle)
{
  native = newHandle;
  fireParentHandlers();
}

lv_obj_t* Widget::handle() const{
  return native;
}
bool Widget::isCreated() const {
  return handle() != nullptr;
}
void Widget::free() {
  DefaultWidgetManager.Free(handle());
  lv_obj_del(handle());
  native = nullptr;
}

Widget& Widget::size(uint16_t width, uint16_t height){
  lv_obj_set_size(native, width, height);
  return *this;
}
Widget& Widget::width(uint16_t width){
  lv_obj_set_width(native, width);
  return *this;
}
Widget& Widget::height(uint16_t height){
  lv_obj_set_height(native, height);
  return *this;
}

Widget& Widget::style(const Style& addStyle, bool resetStyleList){
  return style(addStyle.handle(), LV_OBJ_PART_MAIN, resetStyleList);
}
Widget& Widget::style(const Style& addStyle, uint8_t part, bool resetStyleList){
  return style(addStyle.handle(), part, resetStyleList);
}
Widget& Widget::style(lv_style_t* addStyle, uint8_t part, bool resetStyleList){
  if (resetStyleList)
    lv_obj_reset_style_list(native, part);
  lv_obj_add_style(native, part, addStyle);
  return *this;
}

Widget& Widget::childAddedHandler(OnChildAddedHandler handler) {
  auto handle = DefaultWidgetManager.GetOrCreate(native);
  handle->OnChildAdded = handler;
  return *this;
}

void Widget::fireParentHandlers()
{
  TypeInfo ti(this);

  lv_obj_t* parent = native;
  while(true)
  {
    parent = lv_obj_get_parent(parent);
    if (parent == NULL) break;

    auto handle = DefaultWidgetManager.GetIfExists(parent);
    if (handle != NULL && handle->OnChildAdded != NULL)
      handle->OnChildAdded(*this, ti);
  }
}

// Widget Widget::parent() {
//   auto parHandle = lv_obj_get_parent(native);
//   auto par = (Widget*)lv_obj_get_user_data(parHandle);
//   if (par != NULL)
//     return *par;
//   else
//     return Widget(parHandle);
// }

// void Widget::parent(const Widget& parent){
//   lv_obj_set_parent(native, parent.handle());
// }

Widget& Widget::position(uint16_t x, uint16_t y){
  lv_obj_set_pos(native, x, y);
  return *this;
}

Widget& Widget::align(const Widget& base, lv_align_t align_mode, lv_coord_t x_offset, lv_coord_t y_offset) {
  lv_obj_align(native, base.handle(), align_mode, x_offset, y_offset);
  return *this;
  //return align(&base, align_mode, x_offset, y_offset);
}

// Widget& Widget::align(Widget* base, lv_align_t align_mode, lv_coord_t x_offset, lv_coord_t y_offset) {
//   lv_obj_align(native, base->handle(), align_mode, x_offset, y_offset);
//   return *this;
// }

Widget& Widget::alignx(const Widget& base, lv_align_t align_mode, lv_coord_t x_offset) {
  lv_obj_align_x(native, base.handle(), align_mode, x_offset);
  return *this;
}

Widget& Widget::aligny(const Widget& base, lv_align_t align_mode, lv_coord_t y_offset) {
  lv_obj_align_y(native, base.handle(), align_mode, y_offset);
  return *this;
}

Widget& Widget::alignInParentCenter(lv_coord_t x_offset, lv_coord_t y_offset)
{
  auto parHandle = lv_obj_get_parent(native);
  align(parHandle, LV_ALIGN_CENTER, x_offset, y_offset);
  return *this;
}
Widget& Widget::alignInParentLeftMid(lv_coord_t x_offset, lv_coord_t y_offset)
{
  auto parHandle = lv_obj_get_parent(native);
  align(parHandle, LV_ALIGN_IN_LEFT_MID, x_offset, y_offset);
  return *this;
}
Widget& Widget::alignInParentTopLeft(lv_coord_t x_offset, lv_coord_t y_offset)
{
  auto parHandle = lv_obj_get_parent(native);
  align(parHandle, LV_ALIGN_IN_TOP_LEFT, x_offset, y_offset);
  return *this;
}
Widget& Widget::alignInParentRightMid(lv_coord_t x_offset, lv_coord_t y_offset)
{
  auto parHandle = lv_obj_get_parent(native);
  align(parHandle, LV_ALIGN_IN_RIGHT_MID, x_offset, y_offset);
  return *this;
}
Widget& Widget::alignInParentTopRight(lv_coord_t x_offset, lv_coord_t y_offset)
{
  auto parHandle = lv_obj_get_parent(native);
  align(parHandle, LV_ALIGN_IN_TOP_RIGHT, x_offset, y_offset);
  return *this;
}
Widget& Widget::alignOutsideRightMid(const Widget& base, lv_coord_t x_offset, lv_coord_t y_offset)
{
  align(base, LV_ALIGN_OUT_RIGHT_MID, x_offset, y_offset);
  return *this;
}
Widget& Widget::alignOutsideBottomMid(const Widget& base, lv_coord_t x_offset, lv_coord_t y_offset)
{
  align(base, LV_ALIGN_OUT_BOTTOM_MID, x_offset, y_offset);
  return *this;
}
Widget& Widget::alignOutsideBottomLeft(const Widget& base, lv_coord_t x_offset, lv_coord_t y_offset)
{
  align(base, LV_ALIGN_OUT_BOTTOM_LEFT, x_offset, y_offset);
  return *this;
}

// template<class T>
// T Widget::add(T child)
// {
//   if (child.handle() == NULL)
//     child.createObject(this->handle());
//   else
//     child.setParent(*this);

//   return child;
// }