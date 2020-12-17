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
    WidgetEventHandler Event = NULL;
    OnChildAddedHandler OnChildAdded = NULL;

private:
    uint16_t flags = 0;
};

#endif