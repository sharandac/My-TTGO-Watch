/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "apppage.h"
#include "settings.h"
#include "../os/appicon.h"
#include "../common/events.h"
#include "../common/style.h"
#include "../common/typeinfo.h"

class JsonConfig;

/*
* @brief Application class
*/
class Application {
  friend ApplicationIcon;
public:
  Application() : appIcon(*this) {};

  /*
  * @brief Initialize and register application
  * @param name               application name and icon title
  * @param iconImg            application icon
  * @param userPageCount      count of user pages required for application
  * @param settingsPageCount  count of settings pages required for application
  */
  virtual Application& init(const char* name, const lv_img_dsc_t *iconImg, int userPageCount = 1, int settingsPageCount = 0);

  AppPage& mainPage() { return main; }
  SettingsPage& settingsPage() { return settings; }
  ApplicationIcon& icon() { return appIcon; }

  Application& navigateToMain(bool animate, int id = 0);
  Application& navigateToSettings(bool animate, int id = 0);
  
  uint32_t mainTileId() { return initialTileId; }
  uint32_t settingsTileId() { return initialTileId + userPageCount; }

  /*
  * @brief Assign external configuration for application
  * @param externalConfig             reference to configuration instance. It should be stored by user while application exist (shouldn't be destroyed)
  * @param autoBuildSettingsPage      automatically create settings page widgets based on provided settings options list
  */
  Application& useConfig(JsonConfig& externalConfig, bool autoBuildSettingsPage);

protected:
  /////////////////////////////////////////////////////
  //*** Buttons handlers ***//

  virtual void onButtonOpenSettingsClicked();
  virtual void onButtonCloseSettingsClicked();

  /////////////////////////////////////////////////////
  //*** UI build steps ***//
  virtual void onInitializing();

  // Main application page and main settings page
  virtual void onBuildMainPage(lv_obj_t* tile);
  virtual void onBuildSettingsPage(lv_obj_t* tile);
  
  // Additional application pages
  virtual void onBuildUserSubPage(int id, lv_obj_t* tile) {}
  virtual void onBuildSettingsSubPage(int id, lv_obj_t* tile) {}

  /////////////////////////////////////////////////////
  //*** External handlers - launching application ***//

  virtual void onAppIconClicked();
  virtual void onDesktopWidgetClicked();

protected:
  AppPage main;
  SettingsPage settings;

  uint32_t initialTileId;
  int userPageCount;
  int settingsPageCount;

  ApplicationIcon appIcon;
  String name;
  JsonConfig* configuration = nullptr;
};

#endif