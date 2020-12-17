#include "settings.h"
#include <config.h>
#include <gui/mainbar/mainbar.h>
#include <gui/statusbar.h>

LV_IMG_DECLARE(exit_32px);

SettingsPage& SettingsPage::init(lv_obj_t* handle, const char* title, WidgetAction onExitBtnClick)
{
  Page::assign(handle);

  if (title != nullptr && onExitBtnClick != nullptr)
    constructHeader(title, onExitBtnClick);

  return *this;
}

void SettingsPage::constructHeader(const char* title, WidgetAction onExitBtnClick)
{
    headerContainer = Widget(this);
    headerContainer.size(LV_HOR_RES, 40);

    Button exitBtn(&headerContainer, exit_32px, onExitBtnClick);
    exitBtn.alignInParentLeft(10, 0);

    Label titleLabel(&headerContainer, title);
    titleLabel.alignOutsideRight(exitBtn, 5, 0);
}

void SettingsPage::onInitializing()
{
  autoLayout(LV_LAYOUT_PRETTY_MID);

  // Modify style for settings page
  pageStyle
    .background(LV_COLOR_GRAY)
    .border(0)
    .paddingInner(0);

  // Aplly it
  style(pageStyle);
}

void SettingsPage::onChildAdded(Widget& target, const TypeInfo& type)
{
  log_i("Style processind for %s", type.name());

  if (type.isLabel() || type.isWidget())
      target.style(pageStyle);
  else if (type.isSwitch())
      target.style(mainbar_get_switch_style(), LV_SWITCH_PART_INDIC);
}