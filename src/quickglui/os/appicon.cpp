/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "appicon.h"
#include "../internal/widgetmanager.h"
#include "../widgets/button.h"
#include "../app/application.h"

#include <gui/app.h>
#include <gui/widget.h>

ApplicationIcon::ApplicationIcon(Application& app) : application(app)
{
}

ApplicationIcon& ApplicationIcon::showIndicator(icon_indicator_t value) {
  return showIndicator(IndicatorType::BothItems, value);
}
ApplicationIcon& ApplicationIcon::showIndicator(IndicatorType type, icon_indicator_t value) {
  if (type == IndicatorType::AppIcon && appHandle != nullptr)
    app_set_indicator(appHandle, value);
  else if (type == IndicatorType::DesktopWidget && widgetHandle != nullptr)
    widget_set_indicator(widgetHandle, value);
  else if (type == IndicatorType::BothItems)
  {
    if (appHandle != nullptr)
        app_set_indicator(appHandle, value);
    if (widgetHandle != nullptr)
        widget_set_indicator(widgetHandle, value);
  }
  return *this;
}

ApplicationIcon& ApplicationIcon::hideIndicator(IndicatorType type) {
  if (type == IndicatorType::AppIcon && appHandle != nullptr)
    app_hide_indicator(appHandle);
  else if (type == IndicatorType::DesktopWidget && widgetHandle != nullptr)
    widget_hide_indicator(widgetHandle);
  else if (type == IndicatorType::BothItems)
  {
    if (appHandle != nullptr)
        app_hide_indicator(appHandle);
    if (widgetHandle != nullptr)
        widget_hide_indicator(widgetHandle);
  }
  return *this;
}

ApplicationIcon& ApplicationIcon::widgetText(String text) {
  widgetText(text.c_str());
  return *this;
}
ApplicationIcon& ApplicationIcon::widgetText(String text, String extendedText) {
  widgetText(text.c_str(), extendedText.c_str());
  return *this;
}
ApplicationIcon& ApplicationIcon::widgetText(const char* text, const char* extendedText) {
    if (widgetHandle == nullptr)
        return *this;

    if (text != nullptr)
      widget_set_label(widgetHandle, text);
    
    if (extendedText != nullptr)
      widget_set_extended_label(widgetHandle, extendedText);
  return *this;
}

ApplicationIcon& ApplicationIcon::registerAppIcon(const char* title, const lv_img_dsc_t *icon) {
    if (appHandle != nullptr) return *this;

    // Replace spaces with new line break to fit space in case of long text
    char buf[64] = {0};
    strlcpy(buf, title, sizeof(buf)-1);
    if (strlen(buf) > 6)
    {
      char* s = buf; int k = 0;
      while (*s != 0) {
          if (*s == ' ' && k > 4) {
              *s = '\n';
              k=0;
          }
          s++; k++;
      }
    }

    appHandle = app_register(buf, icon, NULL);

    Button iconBtn(appHandle->icon_img);
    iconBtn.clicked([this](Widget btn) { application.onAppIconClicked(); } );
    return *this;
}

ApplicationIcon& ApplicationIcon::registerDesktopWidget(const char* title, const lv_img_dsc_t *icon) {
    if (widgetHandle != nullptr) return *this;

    widgetHandle = widget_register(title, icon, NULL);
    //showIndicator(IndicatorType::DesktopWidget, ICON_INDICATOR_OK);

    Button widgetIconBtn(widgetHandle->icon_img);
    widgetIconBtn.clicked([this](Widget btn) { application.onDesktopWidgetClicked(); } );
    return *this;
}

ApplicationIcon& ApplicationIcon::unregisterDesktopWidget() {
    if (widgetHandle == nullptr) return *this;

    DefaultWidgetManager.Free(widgetHandle->icon_img);
    widgetHandle = widget_remove(widgetHandle);
    return *this;
}
