/*   July 31 21:33:35 2020
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

// Set logging level for this file
#include "config.h"
#include <Arduino.h>
#include <memory>
#include <utility>

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/statusbar.h"
#include "hardware/display.h"
#include "hardware/motor.h"

#include "pong_app.h"
#include "pong_game.h"

//TODO: Placeholder background images
LV_IMG_DECLARE(bg1);

/* These would be unnecessary if LVGL supported a data param... */

static PongApp *gameInstance = 0;
static void OnExit(struct _lv_obj_t *obj, lv_event_t event)
{
}

static void OnReset(struct _lv_obj_t *obj, lv_event_t event)
{
}

PongApp::PongApp(PongIcon *icon)
{
    gameInstance = this;
    mParentIcon = icon;
}

PongApp::~PongApp()
{
    // LVGL Objects parented to the tiles should be deleted when the tiles are destroyed.
    FreeAppTiles();
    gameInstance = nullptr;
}

// Open from watch menu
void PongApp::OnLaunch()
{
}