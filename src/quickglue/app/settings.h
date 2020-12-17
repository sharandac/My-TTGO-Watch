#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "page.h"

class SettingsPage : public Page {
public:
  SettingsPage() {};
  
  SettingsPage& init(lv_obj_t* handle, const char* title, WidgetAction onExitBtnClick);
  const Widget& header() { return headerContainer; }

protected:
  virtual void constructHeader(const char* title, WidgetAction onExitBtnClick);
  virtual void onInitializing();
  virtual void onChildAdded(Widget& target, const TypeInfo& type);

protected:
  Widget headerContainer;
};

#endif