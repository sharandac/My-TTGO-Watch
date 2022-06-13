/****************************************************************************
 *   Aug 11 17:13:51 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef _WATCHFACE_THEME_CONFIG_H
    #define _WATCHFACE_THEME_CONFIG_H

    #include "config.h"
    #include "utils/alloc.h"
    #include "utils/basejsonconfig.h"
    #include "utils/tinyexpr/tinyexpr.h"

    #define WATCHFACE_LABEL_NUM                 20
    #define WATCHFACE_IMAGE_NUM                 20
    #define API_LEVEL                           3
    /**
     * define all decompressed theme file
     */
    #define WATCHFACE_THEME_JSON_CONFIG_FILE    "/watchface/watchface_theme.json"               /** @brief defines json config file name */
    #define WATCHFACE_DIAL_IMAGE_FILE           "/spiffs/watchface/watchface_dial.png"
    #define WATCHFACE_HOUR_IMAGE_FILE           "/spiffs/watchface/watchface_hour.png"
    #define WATCHFACE_MIN_IMAGE_FILE            "/spiffs/watchface/watchface_min.png"
    #define WATCHFACE_SEC_IMAGE_FILE            "/spiffs/watchface/watchface_sec.png"
    #define WATCHFACE_HOUR_SHADOW_IMAGE_FILE    "/spiffs/watchface/watchface_hour_s.png"
    #define WATCHFACE_MIN_SHADOW_IMAGE_FILE     "/spiffs/watchface/watchface_min_s.png"
    #define WATCHFACE_SEC_SHADOW_IMAGE_FILE     "/spiffs/watchface/watchface_sec_s.png"

    #define WATCHFACE_EXPR_MAX_SIZE             128

    /**
     * @brief dial image control structure
     */
    typedef struct {
        bool enable = true;                             /** @brief enable image */
        int32_t x_offset = 0;                           /** @brief x offset of the image relative to the center */
        int32_t y_offset = 0;                           /** @brief x offset of the image relative to the center */
    } watchface_dial_t;

    /**
     * @brief index image control structure
     */
    typedef struct {
        bool enable = true;                             /** @brief enable image */
        bool smooth = true;                             /** @brief smoothing the move between two min */
        int32_t x_offset = 0;                           /** @brief x offset of the image relative to the center */
        int32_t y_offset = 0;                           /** @brief x offset of the image relative to the center */
    } watchface_index_t;
    typedef struct {
        te_expr *enable = NULL;                         /** @brief enable the widget */
        char enable_expr[WATCHFACE_EXPR_MAX_SIZE] = ""; /** @brief expression for enable flag */
        char type[32] = "";                             /** @brief type of the widget */
        int32_t hide_interval = 0;
        int32_t x_offset = 0;                           /** @brief x offset for the widget*/
        int32_t y_offset = 0;                           /** @brief y offset for the widget*/
        int32_t x_size = 0;                             /** @brief x size for the widget*/
        int32_t y_size = 0;                             /** @brief y size for the widget*/
    } watchface_widget_t;
    /**
     * @brief label control structure
     */
    typedef struct : watchface_widget_t {
        char raw_expr[WATCHFACE_EXPR_MAX_SIZE] = "";    /** @brief raw expression */
        te_expr *expr = NULL;                           /** @brief expression value of the label */
        char label[32] = "";                            /** @brief text for the label */
        char font[32] = "";                             /** @brief font name */
        int32_t font_size = 0;                          /** @brief font size: 12,16,32,48 and 72 */
        char font_color[32] = "";                       /** @brief label color in format '#000000' */
        char align[32] = "";                            /** @brief align the label, default is 'center' */
    } watchface_label_t;
    /**
     * @brief label control structure
     */
    typedef struct : watchface_widget_t {
        char file[32] = "";                             /** @brief filename of the image */
        int32_t stages = 0;
        int32_t rotation_range = 0;                     /** @brief number of vertical stages */
        int32_t rotation_start = 0;                     /** @brief number of vertical stages */
        int32_t rotation_x_origin = 0;
        int32_t rotation_y_origin = 0;
    } watchface_image_t;
    /**
     * @brief watchface theme control structure
     */
    typedef struct {
        uint32_t api_level;                             /** @brief api level */
        watchface_dial_t dial;                          /** @brief dial image struct */
        watchface_index_t hour;                         /** @brief hour image struct */
        watchface_index_t min;                          /** @brief min image struct */
        watchface_index_t sec;                          /** @brief sec image struct */
        watchface_index_t hour_shadow;                  /** @brief hour image shadow struct */
        watchface_index_t min_shadow;                   /** @brief min image shadow struct */
        watchface_index_t sec_shadow;                   /** @brief sec image shadow struct */
        watchface_label_t label[ WATCHFACE_LABEL_NUM ]; /** @brief label struct */
        watchface_image_t image[ WATCHFACE_IMAGE_NUM ]; /** @brief image struct */
    } watchface_t;

    /**
     * @brief blectl config structure
     */
    class watchface_theme_config_t : public BaseJsonConfig {
        public:
        watchface_theme_config_t();
        /**
         * @brief watchface theme config
         */
        watchface_t dial;

        static void *operator new(size_t sz) {
            void* m = MALLOC_ASSERT( sz,"new operator allocation failed" );
            return m;
        }

        static void operator delete( void *m ){
            free( m );
        }

        protected:
        ////////////// Available for overloading: //////////////
        virtual bool onLoad(JsonDocument& document);
        virtual bool onSave(JsonDocument& document);
        virtual bool onDefault( void );
        virtual size_t getJsonBufferSize() { return 1000; }
    } ;

#endif // _WATCHFACE_THEME_CONFIG_H