/****************************************************************************
 *   Sep 2 08:21:51 2020
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
#ifndef _WIDGET_H
    #define _WIDGET_H
    
    #include "gui/mainbar/main_tile/main_tile.h"

    typedef enum {
        WIDGET_ICON_INDICATOR_OK = 0,
        WIDGET_ICON_INDICATOR_FAIL,
        WIDGET_ICON_INDICATOR_UPDATE,
        WIDGET_ICON_INDICATOR_1,
        WIDGET_ICON_INDICATOR_2,
        WIDGET_ICON_INDICATOR_3,
        WIDGET_ICON_INDICATOR_N
    } widget_icon_indicator_t;

    /*
     * @brief register an widget icon
     * 
     * @param   widgetname  widget name
     * @param   icon        pointer to an 48x48px icon
     * @param   event_cb    pointer to an callback function
     */
    widget_icon_t *widget_register( const char* widgetname, const lv_img_dsc_t *icon, lv_event_cb_t event_cb );
    /*
     * @brief   set the icon indicator
     * 
     * @param   widget      pointer to an widget_icon_t structure
     * @param   indicator   possible values are:    WIDGET_ICON_INDICATOR_OK, WIDGET_ICON_INDICATOR_FAIL, WIDGET_ICON_INDICATOR_UPDATE ....
     */
    void widget_set_indicator( widget_icon_t *widget, widget_icon_indicator_t indicator );
    /*
     * @brief   hide the icon indicator
     * 
     * @param   widget      pointer to an widget_icon_t structure
     */
    void widget_hide_indicator( widget_icon_t *widget );
    /*
     * @brief   set the widget icon
     * 
     * @param   widget      pointer to an widget_icon_t structure
     * @param   icon        pointer to an 48x48px icon
     */
    void widget_set_icon( widget_icon_t *widget, lv_obj_t *icon );
    /*
     * @brief   set the widget label
     * 
     * @param   widget      pointer to an widget_icon_t structure
     * @param   text        text for the label
     */
    void widget_set_label( widget_icon_t *widget, const char* text );
    /*
     * @brief   set the widget extended label
     * 
     * @param   widget      pointer to an widget_icon_t structure
     * @param   text        text for the extended label
     */
    void widget_set_extended_label( widget_icon_t *widget, const char* text );

#endif // _WIDGET_H