/****
 * QuickGLUI - Quick Graphical LVLGL-based User Interface development library
 * Copyright  2020  Skurydin Alexey
 * http://github.com/anakod
 * All QuickGLUI project files are provided under the MIT License.
 ****/

#ifndef LABEL_H_
#define LABEL_H_

#include "widget.h"
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

/**
 * @brief Represent Label LVGL widget.
 * Documentation:
 * https://docs.lvgl.io/latest/en/html/widgets/label.html
 */
class Label : public Widget {

public:
    Label(){};
    /**
     * @brief Create a label from lvgl object
     */
    Label(lv_obj_t* handle);
    /**
     * @brief Create a label with text 
     */
    Label(const Widget* parent, const char * txt = NULL);

    virtual void createObject(lv_obj_t* parent);
    /**
     * @brief Set label text
     */
    Label& text(const char * txt);
    /**
     * @brief Set label text
     */
    Label& text(String txt);
    /**
     * @brief Align label text 
     */
    Label& alignText(lv_label_align_t mode);

    constexpr static const char* TypeName = "lv_label";
};

#endif