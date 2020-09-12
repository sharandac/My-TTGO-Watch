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

lv_obj_t * wf_add_container(
    lv_obj_t *parent, lv_obj_t * alignment_base, lv_align_t alignment, int offset_x, int offset_y, int width, int height
);

lv_obj_t * wf_add_label(
    lv_obj_t *parent, char const * text, lv_obj_t * alignment_base, lv_align_t alignment, int offset_x, int offset_y
);

lv_obj_t * wf_add_roller(
    lv_obj_t *parent, char const * data, lv_roller_mode_t mode, lv_obj_t * alignment_base, lv_align_t alignment
);

lv_obj_t * wf_add_labeled_switch(
    lv_obj_t *parent, char const * text, lv_obj_t * alignment_base, lv_align_t alignment, int offset_x, int offset_y, lv_obj_t ** ret_switch_obj
);

lv_obj_t * wf_add_image_button(
    lv_obj_t *parent, lv_img_dsc_t const &image, lv_obj_t * alignment_base, lv_align_t alignment, int offset_x, int offset_y, lv_event_cb_t event_cb
);

lv_obj_t * wf_add_settings_header(lv_obj_t *parent, char const * title, lv_obj_t ** ret_back_btn);
lv_obj_t * wf_add_image(
    lv_obj_t *parent, lv_img_dsc_t const &image, lv_obj_t * alignment_base, lv_align_t alignment, int offset_x, int offset_y
);

