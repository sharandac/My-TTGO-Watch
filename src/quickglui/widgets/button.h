/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef BUTTON_H_
#define BUTTON_H_

#include "widget.h"
#include "label.h"

class Button : public Widget {
public:
    Button() {};
    /** Create a button based on lvgl object */
    Button(lv_obj_t * btn);
    // Create image button
    Button(const Widget* parent, const lv_img_dsc_t& image, WidgetAction onClick = nullptr);
    Button(const Widget* parent, const char * txt, WidgetAction onClick = nullptr);

    Button& clicked(WidgetAction onClick);

    Button& text(const char * txt);

    // Disables the button and makes it inactive
    Button& disable();
    // Enables the button and makes it active
    Button& enable();

    constexpr static const char* TypeNameWithImage = "lv_imgbtn";
    constexpr static const char* TypeNameWithText = "lv_btn";

protected:
    virtual void createObject(lv_obj_t* parent);
    void assign(lv_obj_t* newHandle);

private:
    static void Action(lv_obj_t* obj, lv_event_t event);
    Label label;
};

#endif