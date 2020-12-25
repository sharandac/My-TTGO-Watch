/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef PAGE_H_
#define PAGE_H_

#include "../widgets/container.h"
#include "../common/style.h"
#include "../common/typeinfo.h"

/*
* @brief Base application page class. Parent for all specialized application pages
*/
class Page : public Container {
public:
  Page(){};

  // This class keep references so we should store it
  Page & operator=(const Page&) = delete;
  Page(const Page&) = delete;

  void init(lv_obj_t* handle);

  virtual void assign(lv_obj_t* newHandle);

protected:
  virtual void onInitializing();
  virtual void onChildAdded(Widget& child, const TypeInfo& type);

protected:
  Style pageStyle;
};

#endif