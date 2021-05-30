/*   Aug 5 15:57:34 2020
 *   Copyright  2020  Bryan Wagstaff
 *   Email: programmer@bryanwagstaff.com
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

#include "config.h"
#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/app_tile/app_tile.h" //TODO: Swap this with a game app tile
#include "gui/widget_styles.h"

#include "app/games/GameIconBase.h"

bool GameIconBase::RegisterAppIcon()
{
    if (!pAppname || !pMenuIcon || !pStartFunction)
    {
        log_e("Class is not properly configured.");
        return false;
    }
    //TODO: have a game tile instead of app tile
    lv_obj_t *app = app_tile_register_app(pAppname);
    if (!app)
    {
        log_e("Could not register");
        return false;
    }
    lv_obj_t *image = lv_imgbtn_create(app, NULL);
    if (!image)
    {
        log_e("Could not create icon image");
        return false;
    }
    lv_imgbtn_set_src(image, LV_BTN_STATE_RELEASED, pMenuIcon);
    lv_imgbtn_set_src(image, LV_BTN_STATE_PRESSED, pMenuIcon);
    lv_imgbtn_set_src(image, LV_BTN_STATE_CHECKED_RELEASED, pMenuIcon);
    lv_imgbtn_set_src(image, LV_BTN_STATE_CHECKED_PRESSED, pMenuIcon);
    lv_obj_add_style(image, LV_IMGBTN_PART_MAIN, ws_get_mainbar_style());
    lv_obj_align(image, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(image, pStartFunction);

    return true;
}

void GameIconBase::ReturnToMenu()
{
    // Return to the watch's main tile
    mainbar_jump_to_tilenumber(app_tile_get_tile_num(), LV_ANIM_OFF); //TODO: have a game tile instead of app tile
}