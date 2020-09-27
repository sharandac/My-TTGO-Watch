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
#include <lvgl/src/lv_core/lv_obj.h>
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
lv_obj_t * wf_add_container(lv_obj_t *parent, lv_layout_t layout, lv_fit_t hor_fit=LV_FIT_TIGHT, lv_fit_t ver_fit=LV_FIT_TIGHT, bool add_padding=false);

/**
 * @brief   Creates and adds a main container to a tile
 *
 * @param   parent_tile pointer to tile object
 * @param   layout  defines container's object layout
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_tile_container(lv_obj_t *parent_tile, lv_layout_t layout);

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
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_switch(lv_obj_t *parent);

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

/**
 * @brief   Creates and adds an image button to a container
 *
 * @param   parent  pointer to a parent container
 * @param   image reference to image description
 * @param   event_cb    the button click event function
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_image_button(lv_obj_t *parent, lv_img_dsc_t const &image, lv_event_cb_t event_cb);

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
 *
 * @return  returns pointer to the added container
 */
lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_obj_t ** ret_back_btn);

/**
 * @brief   Creates and adds an image to a container
 *
 * @param   image  reference to the image descriptor
 *
 * @return  returns pointer to the added object
 */
lv_obj_t * wf_add_image(lv_obj_t *parent, lv_img_dsc_t const &image);
