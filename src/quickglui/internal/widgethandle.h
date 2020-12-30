/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef WIDGETHANDLE_H_
#define WIDGETMANAGER_H_

#include "lvgl/lvgl.h"
#include "../widgets/widget.h"

class WidgetHandle
{
public:
    bool IsFlagSet(uint16_t check)
    {
        return (flags & check) == check;
    }
    void SetFlag(uint16_t id, bool val)
    {
        if (val)
            flags = flags | val;
        else
            flags = flags & (~val);
    }

    WidgetAction Action = NULL;
    //WidgetEventHandler Event = NULL; //TODO: Add it in future
    OnChildAddedHandler OnChildAdded = NULL;

private:
    uint16_t flags = 0;
};

#endif