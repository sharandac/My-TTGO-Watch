#ifndef APPPAGE_H_
#define APPPAGE_H_

#include "page.h"

class AppPage : public Page {
public:
  AppPage(){};
  
  AppPage& init(lv_obj_t* handle, bool defaultExitBtn = true);
  AppPage& addSettingsButton(WidgetAction onSettingsBtnClick);
  AppPage& addRefreshButton(WidgetAction onRefreshBtnClick);

protected:
  virtual void onChildAdded(Widget& target, const TypeInfo& type);

protected:
  Widget btnExit;
  Widget btnSettings;
  Widget btnRefresh;
};

#endif