/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef WIDGETMANAGER_H_
#define WIDGETMANAGER_H_

#include "lvgl/lvgl.h"
#include "widgethandle.h"

// #ifndef MAX_WIDGET_HANDLES
// #define MAX_WIDGET_HANDLES 128
// #endif

//TODO: both modes - static and dynamic allocation
class WidgetManager
{
public:
    WidgetManager(/* args */);
    ~WidgetManager();

    WidgetHandle* Allocate(lv_obj_t* obj);
    WidgetHandle* GetIfExists(lv_obj_t* obj);
    WidgetHandle* GetOrCreate(lv_obj_t* obj);
    void Free(lv_obj_t* obj);

private:
    int current = 0;
    //WidgetHandle handles[MAX_WIDGET_HANDLES];
};

extern WidgetManager DefaultWidgetManager;

#endif