/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "application.h"
#include "../internal/widgetmanager.h"
#include "../common/jsonconfig.h"
#include "../common/jsonoptions.h"
#include <config.h>
#include <gui/mainbar/mainbar.h>
#include <gui/statusbar.h>

Application& Application::init(const char* name, const lv_img_dsc_t *iconImg, int userPageCount, int settingsPageCount) {
  this->name = name;
  this->userPageCount = userPageCount;
  this->settingsPageCount = settingsPageCount;
  initialTileId = mainbar_add_app_tile( 1, userPageCount + settingsPageCount, name);

  icon().registerAppIcon(name, iconImg);
  onInitializing();
  return *this;
}

Application& Application::useConfig(JsonConfig& externalConfig, bool autoBuildSettingsPage) {
  configuration = &externalConfig;
  configuration->load();

  if (autoBuildSettingsPage)
  {
    log_i("AutoBuilding settings page for %s. Options count: %d", name.c_str(), configuration->totalCount());
    settingsPage().buildFromConfig(externalConfig);
  }
  return *this;
}

void Application::onInitializing()
{
  // Build both main and settings pages
  auto mainTile = mainbar_get_tile_obj(mainTileId());
  onBuildMainPage(mainTile);
  
  if (settingsPageCount > 0)
  {
    auto settingsTile = mainbar_get_tile_obj(settingsTileId());
    onBuildSettingsPage(settingsTile);
  }

  // Build additional application pages if needed
  for (int i = 1; i < userPageCount; i++)
    onBuildUserSubPage(i, mainTile);

  for (int i = 1; i < settingsPageCount; i++)
    onBuildSettingsSubPage(i, mainTile);
}

void Application::onBuildMainPage(lv_obj_t* tile) {
  main.init(tile, true);
  if (settingsPageCount > 0) {
    main.addSettingsButton([this](Widget btn) { onButtonOpenSettingsClicked(); });
  }
}

void Application::onBuildSettingsPage(lv_obj_t* tile) {
  auto title = name + " settings";
  settings.init(tile, title.c_str(), [this](Widget btn) { onButtonCloseSettingsClicked(); });
}

void Application::onButtonOpenSettingsClicked() {
  if (configuration != nullptr)
    configuration->load();
  statusbar_hide(true);
  navigateToSettings(true);
}
void Application::onButtonCloseSettingsClicked() {
  if (configuration != nullptr)
  {
    configuration->applyFromUI();
    configuration->save();
  }
  navigateToMain(true);
}

void Application::onAppIconClicked() {
  statusbar_hide(true);
  icon().hideIndicator();
  navigateToMain(false);
}

void Application::onDesktopWidgetClicked() {
  statusbar_hide(true);
  icon().hideIndicator();
  navigateToMain(false);
}

Application& Application::navigateToMain(bool animate, int id) {
  mainbar_jump_to_tilenumber(initialTileId + id, (animate ? LV_ANIM_ON : LV_ANIM_OFF) );
  return *this;
}

Application& Application::navigateToSettings(bool animate, int id) {
  mainbar_jump_to_tilenumber(initialTileId + userPageCount + id, (animate ? LV_ANIM_ON : LV_ANIM_OFF) );
  return *this;
}