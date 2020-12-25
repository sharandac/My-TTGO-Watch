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

/*
* @brief Application with syncronization functionality and "refresh" button.
* This type of application will handle refresh button click and launch bacground thread for syncronisation purposes
* internal syncronisation logic provided by user with corresponding callback handler
* See lv_obj_get_type
*/
class SynchronizedApplication : public Application
{
public:
  SynchronizedApplication();

  /*
  * @brief Initialize and register application
  * @param name               application name and icon title
  * @param iconImg            application icon
  * @param userPageCount      count of user pages required for application
  * @param settingsPageCount  count of settings pages required for application
  */
  virtual Application& init(const char* name, const lv_img_dsc_t *iconImg, int userPageCount = 1, int settingsPageCount = 0);
  
  /*
  * @brief Initialize and register application
  * @param name               application name and icon title
  * @param iconImg            application icon
  * @param addSyncButton      add "refresh" button to the UI and assign default handler to it (syncronization start with SyncRequestSource::ApplicationRequest param)
  * @param userPageCount      count of user pages required for application
  * @param settingsPageCount  count of settings pages required for application
  */
  virtual SynchronizedApplication& init(const char* name, const lv_img_dsc_t *iconImg, bool addSyncButton, int userPageCount = 1, int settingsPageCount = 0);

  /*
  * @brief Start synchronization
  * @param callSource      reason of the synchronization starting and/or synchronization caller.
  *                        using this parameter handler can perform different actions depending on how the process was initiated
  */
  void startSynchronization(SyncRequestSource callSource);
  
  /*
  * @brief Set syncronisation handler callback. Method will be executed in background thread.
  */
  SynchronizedApplication& synchronizeActionHandler(SynchronizeAction onSynchronizeHandler);

protected:
  /*
  * @brief This method called before main synchronization action perform
  */
  virtual void onStartSynchronization(SyncRequestSource source) {};

  /*
  * @brief Base low level handler. Don't change it without resons :)
  */
  virtual void onSyncRequest();

private:
  static void SyncTaskHandler(void* pvSelf);

protected:
  EventGroupHandle_t syncEvent = NULL;
  TaskHandle_t syncTask;
  SynchronizeAction synchronize;
  String title;
};

#endif

