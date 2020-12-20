/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef SYNCAPP_H
#define SYNCAPP_H

#include "application.h"
#include <FreeRTOS.h>
// #include <freertos/task.h>
// #include <freertos/event_groups.h>

#define WEATHER_FORECAST_SYNC_REQUEST   _BV(0)

class SynchronizedApplication : public Application
{
public:
  SynchronizedApplication();

  virtual Application& init(const char* name, const lv_img_dsc_t *iconImg, int userPageCount = 1, int settingsPageCount = 1);
  virtual SynchronizedApplication& init(const char* name, const lv_img_dsc_t *iconImg, bool addSyncButton, int userPageCount = 1, int settingsPageCount = 1);

  void startSynchronization(SyncRequestSource callSource);
  SynchronizedApplication& synchronizeActionHandler(SynchronizeAction onSynchronizeHandler);

protected:
  virtual void onSyncRequest();
  virtual void onStartSynchronization(SyncRequestSource source) {};

private:
  static void SyncTaskHandler(void* pvSelf);

protected:
  EventGroupHandle_t syncEvent = NULL;
  TaskHandle_t syncTask;
  SynchronizeAction synchronize;
  String title;
};

#endif

