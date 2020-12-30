/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "apppage.h"
#include <config.h>
#include <gui/mainbar/mainbar.h>
#include <gui/statusbar.h>

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);

AppPage& AppPage::init(lv_obj_t* handle, bool defaultExitBtn)
{
  Page::assign(handle);

  if (defaultExitBtn)
  {
    btnExit = Button(this, exit_32px, [](Widget target) {
        mainbar_jump_to_maintile(LV_ANIM_OFF);
    });
    btnExit.align(*this, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10);
  }
  
  return *this;
}

AppPage& AppPage::addSettingsButton(WidgetAction onSettingsBtnClick)
{
  btnSettings = Button(this, setup_32px, onSettingsBtnClick);
  btnSettings.align(*this, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);

  return *this;
}

AppPage& AppPage::addRefreshButton(WidgetAction onRefreshBtnClick)
{
  btnRefresh = Button(this, refresh_32px, onRefreshBtnClick);
  btnRefresh.align(*this, LV_ALIGN_IN_TOP_RIGHT, -10, 10);

  return *this;
}

Container& AppPage::createChildContainer(lv_layout_t autoLayoutOptios) {
  if (!hasChildContainer()) {
    childContainer = Container(this);
    // Space top/buttom for action buttons:
    if (btnRefresh.handle() == nullptr)
      childContainer.size(LV_HOR_RES, LV_VER_RES - 42);
    else {
      childContainer.size(LV_HOR_RES, LV_VER_RES - 84);
      childContainer.alignInParentTopLeft(0, 42);
    }
    // Auto-align child widgets on it
    childContainer.autoLayout(autoLayoutOptios);
    childContainer.style(mainbar_get_style());
  }
  return childContainer;
}

void AppPage::onChildAdded(Widget& child, const TypeInfo& type)
{
  //log_i("Style processind for %s", type.name());

  //if (type.isLabel() || type.isWidget())
  //    target.style(pageStyle);
  //else if (type.isSwitch())
  //    target.style(mainbar_get_switch_style(), LV_SWITCH_PART_INDIC);
}