/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef APPICON_H
#define APPICON_H

#include "lvgl.h"
#include <gui/icon.h>

#ifdef NATIVE_64BIT
    #include <string>
    using namespace std;
    #define String string
#else
        #include <Arduino.h>
    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #endif
#endif

enum IndicatorType {
  AppIcon = 1,
  DesktopWidget = 2,
  BothItems
};

class Application;


/**
 * @brief Control application icon and desktop widget registration
 */
class ApplicationIcon {
    friend Application;
public:
    /**
     * @brief Show notification indicator (small icon) for both app icon and application desktop widget
     */
    ApplicationIcon& showIndicator(icon_indicator_t value);
    /**
     * @brief Show notification indicator (small icon) for app icon and/or application desktop widget
     */
    ApplicationIcon& showIndicator(IndicatorType type, icon_indicator_t value);
    /**
     * @brief Hide notification indicator (small icon) for app icon and/or application desktop widget
     */
    ApplicationIcon& hideIndicator(IndicatorType type = IndicatorType::BothItems);
    /**
     * @brief Set Application text
     */
    ApplicationIcon& widgetText(const char* text, const char* extendedText = nullptr);
    /**
     * @brief Set Application text
     */
    ApplicationIcon& widgetText(String text);
    /**
     * @brief Set Application text
     */
    ApplicationIcon& widgetText(String text, String extendedText);
    /**
     * @brief Create and add application icon to the desktop
     */
    ApplicationIcon& registerAppIcon(const char* title, const lv_img_dsc_t *icon);
    /**
     * @brief Create and add application widget to the desktop
     */
    ApplicationIcon& registerDesktopWidget(const char* title, const lv_img_dsc_t *icon);
    /**
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

