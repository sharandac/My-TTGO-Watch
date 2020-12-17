#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "apppage.h"
#include "settings.h"
#include "../common/style.h"
#include "../common/typeinfo.h"

#include <gui/icon.h>

enum IndicatorType {
  AppIcon = 1,
  AppWidget = 2,
  BothItems
};

class Application {
public:
  Application(){};

  void init(const char* name, const lv_img_dsc_t *icon, int userPageCount = 1, int settingsPageCount = 1);
  void init(const char* name, const lv_img_dsc_t *icon, bool supportRefresing, int userPageCount = 1, int settingsPageCount = 1);

  AppPage mainPage() { return main; }
  SettingsPage settingsPage() { return settings; }

  void navigateToMain(bool animate, int subItemOffset = 0);
  void navigateToSettings(bool animate, int subItemOffset = 0);
  
  uint32_t mainTileId() { return initialTileId; }
  uint32_t settingsTileId() { return initialTileId + userPageCount; }

  void showIndicator(IndicatorType type, icon_indicator_t value);
  void hideIndicator(IndicatorType type = IndicatorType::BothItems);

  void registerAppWidget(const char* title, const lv_img_dsc_t *icon);

protected:
  virtual void onInitializing();
  virtual void onAppIconClicked();
  virtual void onAppWidgetClicked();

  // Main application page and main settings page
  virtual void onBuildMainPage(lv_obj_t* tile);
  virtual void onBuildSettingsPage(lv_obj_t* tile);
  
  // Additional application pages
  virtual void onBuildUserSubPage(int id, lv_obj_t* tile) {}
  virtual void onBuildSettingsSubPage(int id, lv_obj_t* tile) {}

  void registerAppIcon(const char* title, const lv_img_dsc_t *icon);

protected:
  AppPage main;
  SettingsPage settings;

  uint32_t initialTileId;
  int userPageCount;
  int settingsPageCount;

  icon_t* appHandle;
  icon_t* widgetHandle;
};

#endif