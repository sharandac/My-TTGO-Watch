/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "syncapp.h"
#include <config.h>

SynchronizedApplication::SynchronizedApplication() 
{
    syncEvent = xEventGroupCreate();
}

Application& SynchronizedApplication::init(const char* name, const lv_img_dsc_t *iconImg, int userPageCount, int settingsPageCount) {
    this->init(name, iconImg, true, userPageCount, settingsPageCount);
    return *this;
}

SynchronizedApplication& SynchronizedApplication::init(const char* name, const lv_img_dsc_t *iconImg, bool addSyncButton, int userPageCount, int settingsPageCount) {
    Application::init(name, iconImg, userPageCount, settingsPageCount);
    title = name + String(" sync Task");

    if (addSyncButton)
    {
        mainPage().addRefreshButton([this](Widget btn) {
            startSynchronization(SyncRequestSource::ApplicationRequest);
        });
    }
    return *this;
}

SynchronizedApplication& SynchronizedApplication::synchronizeActionHandler(SynchronizeAction onSynchronizeHandler) {
    synchronize = onSynchronizeHandler;
    return *this;
}

void SynchronizedApplication::startSynchronization(SyncRequestSource callSource)
{
    if (xEventGroupGetBits(syncEvent) & SyncRequestSource::IsRequired)
    {
        log_i("Skip startSync() request, %s isn't completed yet", title.c_str());
        return;
    }

    xEventGroupSetBits(syncEvent, callSource);
    auto result = xTaskCreate(&SynchronizedApplication::SyncTaskHandler,      /* Function to implement the task */
                title.c_str(),    /* Name of the task */
                5000,                            /* Stack size in words */
                (void*)this,                            /* Task input parameter */
                1,                               /* Priority of the task */
                &syncTask );  /* Task handle. */
    
    if (result == pdPASS)
        log_d("%s scheduled", title.c_str());
    else if (result == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
    {
        xEventGroupClearBits(syncEvent, callSource);
        log_e("No enough memory to start %s!", title.c_str());
    }
    else
        while (true); // Not possible!!!
}

void SynchronizedApplication::onSyncRequest() {
    log_i("start %s, heap: %d", title.c_str(), ESP.getFreeHeap());

    vTaskDelay(250);

    auto flags = (SyncRequestSource)xEventGroupGetBits(syncEvent);
    if (flags & SyncRequestSource::IsRequired)
    {   
        onStartSynchronization(flags);
        if (synchronize != nullptr)
            synchronize(flags);
    }
    
    xEventGroupClearBits(syncEvent, SyncRequestSource::AllFlagsValues);
    log_i("finsh %s, heap: %d", title.c_str(), ESP.getFreeHeap());
    vTaskDelete( NULL );
}

void SynchronizedApplication::SyncTaskHandler(void* pvSelf)
{
    log_i("SyncTaskHandler: %d", pvSelf);
    auto self = (SynchronizedApplication*)pvSelf;
    self->onSyncRequest();
}