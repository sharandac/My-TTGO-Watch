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

/**
 * @brief   returns a common container style
 */
lv_style_t *ws_get_container_style();

/**
 * @brief   returns a common setup_tile style
 */
lv_style_t *ws_get_setup_tile_style();

/**
 * @brief   returns a common button style
 */
lv_style_t *ws_get_button_style();

/**
 * @brief   returns a common image button style
 */
lv_style_t *ws_get_img_button_style();

/**
 * @brief   returns a common label style
 */
lv_style_t *ws_get_label_style();

/**
 * @brief   returns a common switch style
 */
lv_style_t *ws_get_switch_style();

/**
 * @brief   returns a common roller style for LV_ROLLER_PART_BG
 */
lv_style_t *ws_get_roller_bg_style();

/**
 * @brief   returns a common roller style for LV_ROLLER_PART_SELECTED
 */
lv_style_t *ws_get_roller_part_selected_style();

/**
 * @brief   returns a common style for popup tile
 */
lv_style_t *ws_get_popup_style();
