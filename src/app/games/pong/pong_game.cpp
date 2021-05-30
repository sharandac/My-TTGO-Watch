/*   Aug 5 16:03:06 2020
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
#include <Arduino.h>
#include <memory>
#include <utility>

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/statusbar.h"
#include "hardware/display.h"
#include "hardware/motor.h"

#include "pong_game.h"
#include "pong_app.h"

// Use this icon image
LV_IMG_DECLARE(pong_64px);

// The one and only.
static PongIcon iconInstance;

void pong_game_setup()
{
    iconInstance.RegisterAppIcon();
}

static void startGame(struct _lv_obj_t *obj, lv_event_t event)
{
}

PongIcon::PongIcon()
{
    pAppname = "Pong";
    pMenuIcon = &pong_64px;
    pStartFunction = startGame;
}