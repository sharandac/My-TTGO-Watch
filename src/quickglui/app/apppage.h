/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef APPPAGE_H_
#define APPPAGE_H_

#include "page.h"

/*
* @brief Base class which represent single application page.
* Methods can be overridden in sub-classes
*/
class AppPage : public Page {
public:
  AppPage(){};
  
  AppPage& init(lv_obj_t* handle, bool defaultExitBtn = true);
  AppPage& addSettingsButton(WidgetAction onSettingsBtnClick);
  AppPage& addRefreshButton(WidgetAction onRefreshBtnClick);

  virtual Container& createChildContainer(lv_layout_t autoLayoutOptios);
  bool hasChildContainer() { return childContainer.handle() != nullptr; }

protected:
  virtual void onChildAdded(Widget& child, const TypeInfo& type);

protected:
  Widget btnExit;
  Widget btnSettings;
  Widget btnRefresh;
  Container childContainer;
};

#endif