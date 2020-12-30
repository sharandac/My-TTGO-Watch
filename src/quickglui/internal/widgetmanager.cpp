/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#include "widgetmanager.h"
#include <config.h>
#include "hardware/alloc.h"

WidgetManager DefaultWidgetManager;

WidgetManager::WidgetManager()
{
}

WidgetManager::~WidgetManager()
{
}

WidgetHandle* WidgetManager::Allocate(lv_obj_t* obj)
{
    // Static allocation
    //TODO: checks MAX_WIDGET_HANDLES
    //return &handles[current++];
    
    lv_obj_type_t buf;
    lv_obj_get_type(obj, &buf);
    auto type = buf.type[0] != NULL ? buf.type[0] : "lv_obj";
    log_i("WidgetHandle allocated for %s. Total count: %d", type, ++current);
    
    auto addr = MALLOC(sizeof(WidgetHandle));
    return new(addr) WidgetHandle();
}

void WidgetManager::Free(lv_obj_t* obj)
{
    auto handle = GetIfExists(obj);
    lv_obj_set_user_data(obj, NULL);
    free(handle);
    current--;
    log_i("WidgetHandle was destroyed. Total count: %d", current);
}

WidgetHandle* WidgetManager::GetIfExists(lv_obj_t* obj)
{
    auto handle = (WidgetHandle*)lv_obj_get_user_data(obj);
    return handle;
}

WidgetHandle* WidgetManager::GetOrCreate(lv_obj_t* obj)
{
    auto handle = GetIfExists(obj);
    if (handle == NULL)
    {
        handle = Allocate(obj);
        lv_obj_set_user_data(obj, handle);
    }
    return handle;
}
