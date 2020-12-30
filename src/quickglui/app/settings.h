/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "page.h"

class JsonConfig;

/*
* @brief Base class which represent single application settings page.
* Methods can be overridden in sub-classes
*/
class SettingsPage : public Page {
public:
  SettingsPage() {};
  
  SettingsPage& init(lv_obj_t* handle, const char* title, WidgetAction onExitBtnClick);
  const Widget& header() { return headerContainer; }

  void buildFromConfig(JsonConfig& config);

protected:
  virtual void constructHeader(const char* title, WidgetAction onExitBtnClick);
  virtual void onInitializing();
  virtual void onChildAdded(Widget& child, const TypeInfo& type);

protected:
  Widget headerContainer;
};

#endif