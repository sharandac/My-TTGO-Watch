/****************************************************************************
 *   Copyright  2020  Jakub Vesely
 *   Email: jakub_vesely@seznam.cz
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

#pragma once

    #include "config.h"
    #ifdef LV_LVGL_H_INCLUDE_SIMPLE
        #include "lv_core/lv_obj.h"
    #else
        #include "lvgl/src/lv_core/lv_obj.h"
    #endif

    #include "widget_styles.h"

    #if defined( BIG_THEME )
        #define THEME_ICON_SIZE         112
        #define THEME_ICON_PADDING      10
        #define THEME_PADDING           10
        #define THEME_CONT_HEIGHT       60
    #elif defined( MID_THEME )
        #define THEME_ICON_SIZE         72
        #define THEME_ICON_PADDING      10
        #define THEME_PADDING           10
        #define THEME_CONT_HEIGHT       45
    #else
        #define THEME_ICON_SIZE         42
        #define THEME_ICON_PADDING      5
        #define THEME_PADDING           5
        #define THEME_CONT_HEIGHT       35
    #endif

    #define     BACKGROUND_STYLE            ws_get_background_style()
    #define     MAINBAR_STYLE               ws_get_mainbar_style()
    #define     APP_STYLE                   ws_get_app_style()
    #define     APP_ICON_STYLE              ws_get_app_icon_style()
    #define     APP_ICON_LABEL_STYLE        ws_get_app_icon_label_style()
    #define     SETUP_STYLE                 ws_get_setup_tile_style()
    #define     SYSTEM_ICON_STYLE           ws_get_system_icon_style()
    #define     SYSTEM_ICON_LABEL_STYLE     ws_get_system_icon_label_style()

/**
 * @brief   Creates and adds container (lv_cont) to parent object as a layout member.
 *
 * @param   parent  pointer to parent object which the new container will be added in
 * @param   layout  defines container's object layout
 * @param   hor_fit defines how the container fit parent in width
 * @param   ver_fit defines how the container fit parent in height
 * @param   add_padding true means that CLICKABLE_PADDING will be added among the container borders and its internal objects
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_container(lv_obj_t *parent, lv_layout_t layout,lv_fit_t hor_fit=LV_FIT_TIGHT, lv_fit_t ver_fit=LV_FIT_TIGHT, bool add_padding=false, lv_style_t *style=NULL );

/**
 * @brief   Creates and adds a main container to a tile
 *
 * @param   parent_tile pointer to tile object
 * @param   layout  defines container's object layout
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_tile_container(lv_obj_t *parent_tile, lv_layout_t layout);
lv_obj_t * wf_add_tile_container(lv_obj_t *parent_tile, lv_layout_t layout, lv_style_t *style );

/**
 * @brief   Creates and add a container placed on on a tile bottom (out of main tile container)
 *
 * @param   parent_tile pointer to tile object
 * @param   layout  defines container's object layout
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_tile_footer_container(lv_obj_t *parent_tile, lv_layout_t layout);

/**
 * @brief   Creates and adds a label object to a container
 *
 * @param   parent  pointer to a parent container
 * @param   text    label content
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_label(lv_obj_t *parent, char const * text);
/**
 * @brief   Creates and adds a label object to a container
 *
 * @param   parent  pointer to a parent container
 * @param   text    label content
 * @param   style   pointer to a style
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_label(lv_obj_t *parent, char const * text, lv_style_t *style );
lv_obj_t * wf_add_label_container(lv_obj_t *parent, char const * text);
lv_obj_t * wf_add_label_container(lv_obj_t *parent, char const * text, lv_style_t *style );

/**
 * @brief   Creates and adds a roller object to a container
 *
 * @param   parent  pointer to a parent container
 * @param   data    pointer to data (entries are separated by \\n)
 * @param   mode    LV_ROLLER_MODE_NORMAL or LV_ROLLER_MODE_INIFINITE
 * @param   row_count   number of visible rows
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_roller(lv_obj_t *parent, char const * data, lv_roller_mode_t mode, int row_count);

/**
 * @brief   Creates and adds a switch object to a container
 *
 * @param   parent  pointer to a parent container
 * @param   on      default value
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_switch(lv_obj_t *parent, bool on = false);

/**
 * @brief   Creates and adds a container with a label and a switch to a container
 *
 * @param   parent  pointer to a parent container
 * @param   text    the label content
 * @param   ret_switch_obj returns pointer to the switch object
 *
 * @return  returns pointer to the added container
 */
lv_obj_t * wf_add_labeled_switch(lv_obj_t *parent, char const * text, lv_obj_t ** ret_switch_obj);
lv_obj_t * wf_add_labeled_switch(lv_obj_t *parent, char const * text, lv_obj_t ** ret_switch_obj, bool state, lv_event_cb_t event_cb );
lv_obj_t * wf_add_labeled_switch(lv_obj_t *parent, char const * text, lv_obj_t ** ret_switch_obj, bool state, lv_event_cb_t event_cb, lv_style_t *style  );
lv_obj_t * wf_add_labeled_list(lv_obj_t *parent, char const * text, lv_obj_t ** ret_list_obj, const char *options, lv_event_cb_t event_cb, lv_style_t *style, lv_style_t *dropdown_style );

/**
 * @brief   Creates and adds an image button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   image    reference to image description
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_image_button(lv_obj_t *parent, lv_img_dsc_t const &image, lv_event_cb_t event_cb, lv_style_t *style=NULL);

/**
 * @brief   Creates and adds an exit button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_exit_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style=NULL);

/**
 * @brief   Creates and adds an exit button to a container with default exit callback
 *
 * @param   parent   pointer to a parent container
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_exit_button(lv_obj_t *parent, lv_style_t *style=NULL);
lv_img_dsc_t const &wf_get_exit_img( void );

/**
 * @brief   Creates and adds an add button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_add_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL );
lv_img_dsc_t const &wf_get_add_img( void );

/**
 * @brief   Creates and adds an trash button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_trash_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL );
lv_img_dsc_t const &wf_get_trash_img( void );

/**
 * @brief   Creates and adds an menu button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_menu_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL );
lv_img_dsc_t const &wf_get_menu_img( void );

/**
 * @brief   Creates and adds an refresh button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_refresh_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL );
lv_img_dsc_t const &wf_get_refresh_img( void );

/**
 * @brief   Creates and adds an setup button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_setup_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL);
lv_img_dsc_t const &wf_get_setup_img( void );

/**
 * @brief   Creates and adds an close button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_close_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL);
lv_img_dsc_t const &wf_get_close_img( void );

/**
 * @brief   Creates and adds an check button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_check_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL);
lv_img_dsc_t const &wf_get_check_img( void );

/**
 * @brief   Creates and adds an download button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_download_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL);
lv_img_dsc_t const &wf_get_download_img( void );

/**
 * @brief   Creates and adds an equal button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_equal_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL);
lv_img_dsc_t const &wf_get_equal_img( void );

/**
 * @brief   Creates and adds an zoom in button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */

lv_obj_t * wf_add_zoom_in_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL );
lv_img_dsc_t const &wf_get_zoom_in_img( void );

/**
 * @brief   Creates and adds an zoom out button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_zoom_out_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *styl = NULL );
lv_img_dsc_t const &wf_get_zoom_out_img( void );

/**
 * @brief   Creates and adds an right button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_right_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL );
lv_img_dsc_t const &wf_get_right_img( void );

/**
 * @brief   Creates and adds an left button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_left_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL );
lv_img_dsc_t const &wf_get_left_img( void );

/**
 * @brief   Creates and adds an up button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_up_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL );
lv_img_dsc_t const &wf_get_up_img( void );

/**
 * @brief   Creates and adds an down button to a container
 *
 * @param   parent   pointer to a parent container
 * @param   event_cb the button click event function
 * @param   style    style to apply
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_down_button(lv_obj_t *parent, lv_event_cb_t event_cb, lv_style_t *style = NULL );
lv_img_dsc_t const &wf_get_down_img( void );

/**
 * @brief   Creates and adds an labeled button to a container
 *
 * @param   parent  pointer to a parent container
 * @param   label   pointer to the button text
 * @param   width   width of the button
 * @param   height  height of the button
 * @param   event_cb    the button click event function
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_button(lv_obj_t *parent, char const * label, int width, int height, lv_event_cb_t event_cb);

/**
 * @brief   Creates and adds header container for settings dialogs
 *
 * @param   parent  pointer to a parent container
 * @param   title   dialog name (usually the app name)
 * @param   ret_back_btn poiter to back button of the tile (it is expected that the click event will be managed externally)
 * @param   style   pointer to a style
 *
 * @return  returns pointer to the added container
 */
lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_obj_t ** ret_back_btn, lv_style_t *style );

/**
 * @brief   Creates and adds header container for settings dialogs
 *
 * @param   parent  pointer to a parent container
 * @param   title   dialog name (usually the app name)
 * @param   ret_back_btn poiter to back button of the tile (it is expected that the click event will be managed externally)
 *
 * @return  returns pointer to the added container
 */
lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_obj_t ** ret_back_btn);
lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_event_cb_t event_cb);

/**
 * @brief   Creates and adds header container for settings dialogs with a default exit callback
 *
 * @param   parent  pointer to a parent container
 * @param   title   dialog name (usually the app name)
 *
 * @return  returns pointer to the added container
 */
lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title);
lv_obj_t *wf_get_settings_header_title(lv_obj_t *parent);
lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_style_t *style );

/**
 * @brief   Creates and adds an image to a container
 *
 * @param   image  reference to the image descriptor
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_image(lv_obj_t *parent, lv_img_dsc_t const &image);

/**
 * @brief create and adds a list to a container
 * 
 * @param   parent  pointer to a parent container
 * @param   options pointer to a option list
 * @param   style   pointer to a lv_style strcuture
 *
 * @return pointer to the new list object
 */
lv_obj_t * wf_add_list(lv_obj_t *parent, const char* _options );
lv_obj_t * wf_add_list(lv_obj_t *parent, const char* _options, lv_style_t *style );

/**
 * @brief create and adds a labled list to a container
 * 
 * @param   parent          pointer to a parent container
 * @param   text            label text
 * @param   ret_list_obj    pointer to a list pointer
 * @param   options         pointer to a option list
 * @param   event_cb        pointer to a call back function when a list entry is clicked
 * @param   style           pointer to a lv_style strcuture
 *
 * @return pointer to the new list object
 */
lv_obj_t * wf_add_labeled_list(lv_obj_t *parent, char const * text, lv_obj_t ** ret_list_obj, const char *options, lv_event_cb_t event_cb );
lv_obj_t * wf_add_labeled_list(lv_obj_t *parent, char const * text, lv_obj_t ** ret_list_obj, const char *options, lv_event_cb_t event_cb, lv_style_t *style );