/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "widgetmanager.h"
#include <config.h>
#include "utils/alloc.h"
#include "lvgl.h"
#include "lv_core/lv_obj.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include <string>
    using namespace std;
    #define String string
#else
    #include <Arduino.h>
        #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #endif
#endif


WidgetManager DefaultWidgetManager;

WidgetManager::WidgetManager() {}

WidgetManager::~WidgetManager() {}

WidgetHandle* WidgetManager::Allocate(lv_obj_t* obj) {
    /**
     * Static allocation
     * TODO: checks MAX_WIDGET_HANDLES
     * @return &handles[current++];
     */
    lv_obj_type_t buf;
    lv_obj_get_type(obj, &buf);
    auto type = buf.type[0] != NULL ? buf.type[0] : "lv_obj";
    log_d("WidgetHandle allocated for %s. Total count: %d", type, ++current);
    
    auto addr = MALLOC_ASSERT(sizeof(WidgetHandle),"WidgetManager::Allocate allocation failed");
    return new(addr) WidgetHandle();
}

void WidgetManager::Free(lv_obj_t* obj) {
    auto handle = GetIfExists(obj);
    lv_obj_set_user_data(obj, NULL);
    free(handle);
    current--;
    log_d("WidgetHandle was destroyed. Total count: %d", current);
}

WidgetHandle* WidgetManager::GetIfExists(lv_obj_t* obj) {
    auto handle = (WidgetHandle*)lv_obj_get_user_data(obj);
    return handle;
}

WidgetHandle* WidgetManager::GetOrCreate(lv_obj_t* obj) {
    auto handle = GetIfExists(obj);
    if (handle == NULL) {
        handle = Allocate(obj);
        lv_obj_set_user_data(obj, handle);
    }
    return handle;
}
