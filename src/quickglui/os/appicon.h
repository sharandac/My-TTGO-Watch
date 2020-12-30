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


/*
* @brief Control application icon and desktop widget registration
*/
class ApplicationIcon
{
  friend Application;
public:
  /*
  * @brief Show notification indicator (small icon) for both app icon and application desktop widget
  */
  ApplicationIcon& showIndicator(icon_indicator_t value);
  /*
  * @brief Show notification indicator (small icon) for app icon and/or application desktop widget
  */
  ApplicationIcon& showIndicator(IndicatorType type, icon_indicator_t value);
  
  /*
  * @brief Hide notification indicator (small icon) for app icon and/or application desktop widget
  */
  ApplicationIcon& hideIndicator(IndicatorType type = IndicatorType::BothItems);


  ApplicationIcon& widgetText(const char* text, const char* extendedText = nullptr);
  ApplicationIcon& widgetText(String text);
  ApplicationIcon& widgetText(String text, String extendedText);

  /*
  * @brief Create and add application icon to the desktop
  */
  ApplicationIcon& registerAppIcon(const char* title, const lv_img_dsc_t *icon);

  /*
  * @brief Create and add application widget to the desktop
  */
  ApplicationIcon& registerDesktopWidget(const char* title, const lv_img_dsc_t *icon);
  /*
  * @brief Remove application widget from the desktop
  */
  ApplicationIcon& unregisterDesktopWidget();

protected:
  ApplicationIcon(Application& app);

protected:
  icon_t* appHandle = nullptr;
  icon_t* widgetHandle = nullptr;

  Application& application;
};

#endif

