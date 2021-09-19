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
#include <gui/widget_factory.h>
#include <gui/widget_styles.h>

LV_IMG_DECLARE(refresh_32px);

AppPage& AppPage::init(lv_obj_t* handle, bool defaultExitBtn)
{
  Page::assign(handle);

  if (defaultExitBtn)
  {
    lv_obj_t *btnExitHandle = wf_add_exit_button(handle);
    btnExit = Button(btnExitHandle);
    btnExit.align(*this, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10);
  }
  
  return *this;
}

AppPage& AppPage::addSettingsButton(WidgetAction onSettingsBtnClick)
{
  lv_obj_t *btnSettingsHandle = wf_add_setup_button(this->handle(), NULL, NULL);
  btnSettings = Button(btnSettingsHandle).clicked(onSettingsBtnClick);
  btnSettings.align(*this, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);

  return *this;
}

AppPage& AppPage::addRefreshButton(WidgetAction onRefreshBtnClick)
{
    lv_obj_t *btnRefreshHandle = wf_add_refresh_button(this->handle(), NULL, NULL);
  btnRefresh = Button(btnRefreshHandle).clicked(onRefreshBtnClick);
  btnRefresh.align(*this, LV_ALIGN_IN_TOP_RIGHT, -10, 10);

  return *this;
}

AppPage& AppPage::addAppButton(const lv_img_dsc_t& image, WidgetAction onBtnClick)
{
  log_d("New button");
  btnApp = Button(this, image, onBtnClick);
  btnApp.align(*this, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

  return *this;
}

AppPage& AppPage::moveExitButtonToForeground()
{
  lv_obj_move_foreground(btnExit.handle());
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
    childContainer.style(ws_get_mainbar_style());
  }
  return childContainer;
}

void AppPage::onChildAdded(Widget& child, const TypeInfo& type)
{
  //log_i("Style processind for %s", type.name());

  //if (type.isLabel() || type.isWidget())
  //    target.style(pageStyle);
  //else if (type.isSwitch())
  //    target.style(ws_get_switch_style(), LV_SWITCH_PART_INDIC);
}