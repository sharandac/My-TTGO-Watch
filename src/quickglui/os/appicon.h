/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef APPICON_H
#define APPICON_H

#include "lvgl/lvgl.h"
#include <gui/icon.h>

enum IndicatorType {
  AppIcon = 1,
  DesktopWidget = 2,
  BothItems
};

class Application;

class ApplicationIcon
{
public:
  ApplicationIcon(Application& app);

  ApplicationIcon& showIndicator(icon_indicator_t value);
  ApplicationIcon& showIndicator(IndicatorType type, icon_indicator_t value);
  ApplicationIcon& hideIndicator(IndicatorType type = IndicatorType::BothItems);
  ApplicationIcon& widgetText(const char* text, const char* extendedText = nullptr);
  ApplicationIcon& widgetText(String text);
  ApplicationIcon& widgetText(String text, String extendedText);

  ApplicationIcon& registerAppIcon(const char* title, const lv_img_dsc_t *icon);

  ApplicationIcon& registerDesktopWidget(const char* title, const lv_img_dsc_t *icon);
  ApplicationIcon& unregisterDesktopWidget();

protected:
  icon_t* appHandle = nullptr;
  icon_t* widgetHandle = nullptr;

  Application& application;
};

#endif

