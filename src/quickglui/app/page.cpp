/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "page.h"
#include <config.h>
#include <gui/mainbar/mainbar.h>
#include <gui/statusbar.h>

void Page::init(lv_obj_t* handle) {
  assign(handle);
}

void Page::onInitializing()
{
  style(pageStyle);
}

void Page::onChildAdded(Widget& child, const TypeInfo& type) {
}

void Page::assign(lv_obj_t* newHandle) {  
    native = newHandle;

    pageStyle = Style(mainbar_get_style(), true);
    onInitializing();

    //onChildAddedHandler(std::bind(&Page::onChildAdded, this, std::placeholders::_1, std::placeholders::_2));
    childAddedHandler([this](Widget& target, const TypeInfo& type){ onChildAdded(target, type); });
}