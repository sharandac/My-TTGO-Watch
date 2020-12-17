#include "application.h"
#include <config.h>
#include <gui/mainbar/mainbar.h>
#include <gui/statusbar.h>
#include <gui/app.h>
#include <gui/widget.h>

LV_IMG_DECLARE(example_app_64px);

void Application::init(const char* name, const lv_img_dsc_t *icon, int userPageCount, int settingsPageCount) {
  init(name, icon, false, userPageCount, settingsPageCount);
}

void Application::init(const char* name, const lv_img_dsc_t *icon, bool supportRefresing, int userPageCount, int settingsPageCount) {
    this->userPageCount = userPageCount;
    this->settingsPageCount = settingsPageCount;
    initialTileId = mainbar_add_app_tile( 1, userPageCount + settingsPageCount, name);

    registerAppIcon(name, icon);
    onInitializing();
}

void Application::registerAppIcon(const char* title, const lv_img_dsc_t *icon) {
  appHandle = app_register(title, icon, NULL);
  showIndicator(IndicatorType::AppIcon, ICON_INDICATOR_2);

  Button iconBtn(appHandle->icon_img);
  iconBtn.clicked([this](Widget btn) { onAppIconClicked(); } );
}

void Application::registerAppWidget(const char* title, const lv_img_dsc_t *icon) {
  widgetHandle = widget_register(title, icon, NULL);
  showIndicator(IndicatorType::AppWidget, ICON_INDICATOR_N);

  Button widgetIconBtn(widgetHandle->icon_img);
  widgetIconBtn.clicked([this](Widget btn) { onAppWidgetClicked(); } );
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
  if (settingsPageCount > 0)
  {
    main.addSettingsButton([this](Widget btn) {
        statusbar_hide( true );
        navigateToSettings(true);
    });
  }
}

void Application::onBuildSettingsPage(lv_obj_t* tile) {
  settings.init(tile, "my app setuper", [this](Widget btn) {
    navigateToMain(true);
  });
}

void Application::showIndicator(IndicatorType type, icon_indicator_t value) {
  if (type == IndicatorType::AppIcon)
    app_set_indicator(appHandle, value);
  else if (type == IndicatorType::AppWidget)
    widget_set_indicator(widgetHandle, value);
  else if (type == IndicatorType::BothItems)
  {
    app_set_indicator(appHandle, value);
    widget_set_indicator(widgetHandle, value);
  }
}
void Application::hideIndicator(IndicatorType type) {
  if (type == IndicatorType::AppIcon)
    app_hide_indicator(appHandle);
  else if (type == IndicatorType::AppWidget)
    widget_hide_indicator(widgetHandle);
  else if (type == IndicatorType::BothItems)
  {
    app_hide_indicator(appHandle);
    widget_hide_indicator(widgetHandle);
  }
}

void Application::onAppIconClicked() {
  statusbar_hide(true);
  hideIndicator();
  navigateToMain(false);
}

void Application::onAppWidgetClicked() {
  statusbar_hide(true);
  hideIndicator();
  navigateToMain(false);
}

void Application::navigateToMain(bool animate, int subItemOffset) {
  mainbar_jump_to_tilenumber(initialTileId + subItemOffset, (animate ? LV_ANIM_ON : LV_ANIM_OFF) );
}

void Application::navigateToSettings(bool animate, int subItemOffset) {
  mainbar_jump_to_tilenumber(initialTileId + userPageCount + subItemOffset, (animate ? LV_ANIM_ON : LV_ANIM_OFF) );
}