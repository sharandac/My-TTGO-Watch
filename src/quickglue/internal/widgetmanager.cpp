#include "widgetmanager.h"
#include <config.h>

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
    
    auto addr = ps_malloc(sizeof(WidgetHandle));
    return new(addr) WidgetHandle();
}

void WidgetManager::Free(WidgetHandle* handle)
{
    free(handle);
    current--;
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
