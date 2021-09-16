/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef SWITCH_H_
#define SWITCH_H_

#include "widget.h"

#ifdef NATIVE_64BIT
    #include <string>
    using namespace std;
    #define String string
#else
        #include <Arduino.h>
    #ifdef M5PAPER
    #elif defined( LILYGO_WATCH_2020_V1 ) || defined( LILYGO_WATCH_2020_V2 ) || defined( LILYGO_WATCH_2020_V3 )
    #endif
#endif


/*
* @brief Represent Switch LVGL widget.
* Documentation:
* https://docs.lvgl.io/latest/en/html/widgets/switch.html
*/
class Switch : public Widget {

public:
    Switch(){};
    /**
     * @brief Create a Switch from lvgl object
     */
    Switch(lv_obj_t* handle);
    
    Switch(const Widget* parent);
    Switch(const Widget* parent, bool state);

    virtual void createObject(lv_obj_t* parent);
    
    void value(bool val);
    bool value();

    constexpr static const char* TypeName = "lv_switch";
};

#endif