/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "button.h"
#include <config.h>
#include "../internal/widgetmanager.h"

Button::Button(lv_obj_t * btn){
  assign(btn);
  // lv_cont_set_fit(native, true, true);
  
  auto subLabelhandle = lv_obj_get_child(btn, NULL);
  if (subLabelhandle != nullptr)
    label = Label(subLabelhandle);
}


Button::Button(const Widget* parent, const lv_img_dsc_t& image, WidgetAction onClick){
  assign(lv_imgbtn_create(parent->handle(), NULL));
  
  lv_imgbtn_set_src(native, LV_BTN_STATE_RELEASED, &image);
  lv_imgbtn_set_src(native, LV_BTN_STATE_PRESSED, &image);
  lv_imgbtn_set_src(native, LV_BTN_STATE_CHECKED_RELEASED, &image);
  lv_imgbtn_set_src(native, LV_BTN_STATE_CHECKED_PRESSED, &image);
  
  if (onClick != nullptr)
    clicked(onClick);
}

Button::Button(const Widget* parent, const char * txt, WidgetAction onClick){
  assign(lv_btn_create(parent->handle(), NULL));
  lv_obj_t * lbl = lv_label_create(native, NULL);
  label = Label(lbl);
  if (txt != nullptr)
  {
    label.text(txt);
    label.alignText(LV_LABEL_ALIGN_CENTER);
  }

  if (onClick != nullptr)
    clicked(onClick);
}

Button& Button::text(const char * txt) {
  if (label.isCreated())
    label.text(txt);
  return *this;
};

Button& Button::clicked(WidgetAction onClick){
    auto wh = DefaultWidgetManager.GetOrCreate(native);
    wh->Action = onClick;
    return *this;
}

void Button::createObject(lv_obj_t* parent){
  assign(lv_obj_create(parent, NULL));
}

void Button::assign(lv_obj_t* newHandle)
{
    Widget::assign(newHandle);
    if (lv_obj_get_event_cb(native) == NULL)
      lv_obj_set_event_cb(native, &Button::Action);
}

// void Button::size(uint16_t width, uint16_t height){
//   // lv_cont_set_fit(native, false, false);
//   lv_obj_set_size(native, width, height);
//   label.size(width, height);
// }

Button& Button::disable() {
  lv_btn_set_state(native, LV_BTN_STATE_DISABLED);
  lv_obj_set_click(native, false);
  return *this;
}
Button& Button::enable() {
  lv_obj_set_click(native, true);
  lv_btn_set_state(native, LV_BTN_STATE_RELEASED);
  return *this;
}

void Button::Action(lv_obj_t* obj, lv_event_t event)
{
    auto handle = DefaultWidgetManager.GetIfExists(obj);
    Widget target(obj);
    switch (event) {
        case LV_EVENT_CLICKED:
            if (handle->Action != NULL)
              handle->Action(target);
            break;
    }
}