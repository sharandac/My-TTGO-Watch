/****************************************************************************
 *   Tu May 22 21:23:51 2020
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
#ifndef _ICON_H
    #define _ICON_H

    #include <TTGO.h>

    LV_IMG_DECLARE(info_ok_16px);
    LV_IMG_DECLARE(info_fail_16px);
    LV_IMG_DECLARE(info_update_16px);
    LV_IMG_DECLARE(info_1_16px);
    LV_IMG_DECLARE(info_2_16px);
    LV_IMG_DECLARE(info_3_16px);
    LV_IMG_DECLARE(info_n_16px);

    typedef enum {
        ICON_INDICATOR_OK = 0,
        ICON_INDICATOR_FAIL,
        ICON_INDICATOR_UPDATE,
        ICON_INDICATOR_1,
        ICON_INDICATOR_2,
        ICON_INDICATOR_3,
        ICON_INDICATOR_N
    } icon_indicator_t;

    typedef enum {
        ICON_BTN_SETUP = 0,
        ICON_BTN_REFRESH,
        ICON_BTN_EXIT
    } icon_btn_t;
    
    typedef struct {
        lv_obj_t *icon_cont;
        lv_obj_t *icon_img;
        lv_obj_t *icon_indicator;
        lv_obj_t *label;
        lv_obj_t *ext_label;
        lv_event_cb_t *icon_event_cb;
        lv_coord_t x;
        lv_coord_t y;
        bool active;
    } icon_t;

#endif // _ICON_H