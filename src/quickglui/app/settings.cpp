/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "settings.h"
#include "../common/jsonconfig.h"
#include "../common/jsonoptions.h"
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
    exitBtn.alignInParentLeftMid(10, 0);

    Label titleLabel(&headerContainer, title);
    titleLabel.alignOutsideRightMid(exitBtn, 5, 0);
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

void SettingsPage::onChildAdded(Widget& child, const TypeInfo& type)
{
  log_d("Style processind for %s", type.name());

  if (type.isLabel() || type.isWidget())
      child.style(pageStyle);
  else if (type.isSwitch())
      child.style(mainbar_get_switch_style(), LV_SWITCH_PART_INDIC);
}

void SettingsPage::buildFromConfig(JsonConfig& config)
{
  for (int i = 0; i < config.totalCount(); i++)
  {
      auto item = config.getOption(i);

      Widget line(this);
      line.size(LV_HOR_RES, 38);

      Label label(&line, item->name);
      label.alignInParentLeftMid(5, 0);

      switch (item->type())
      {            
          case OptionDataType::BoolOption:
          {
              auto option = (JsonBoolOption*)item;

              Switch switcher(&line, false);
              switcher.alignInParentRightMid(-5, 0);

              // Option value will be updated on applyFromUI() call
              option->assign(switcher);
              break;
          }
          case OptionDataType::StringOption:
          {
              auto option = (JsonStringOption*)item;

              TextArea editor(&line, "");
              editor.width(LV_HOR_RES/2)
                  .alignInParentRightMid(-5, 0);

              // Option value will be updated on applyFromUI() call
              option->assign(editor);
              break;
          }
      }
  }
}
