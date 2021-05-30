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

#include "ttt_game.h"
#include "ttt_app.h"

// Use this icon image
LV_IMG_DECLARE(ttt_64px);

// The one and only.
static TicTacToeIcon iconInstance;

void tic_tac_toe_game_setup()
{
    iconInstance.RegisterAppIcon();
}

static void startGame(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        iconInstance.OnStartClicked();
        break;
    }
}

TicTacToeIcon::TicTacToeIcon()
{
    pAppname = "Tic Tac Toe";
    pMenuIcon = &ttt_64px;
    pStartFunction = startGame;
}

void TicTacToeIcon::OnStartClicked()
{
    motor_vibe(1);

    if(!mGameInstance)
    {
        log_d("Creating game instance.");
        mGameInstance = std::unique_ptr<TicTacToeApp>(new TicTacToeApp(this));
    }

    log_d("Launching game instance.");
    mGameInstance->OnLaunch();
}

static void DelayedRelease(void* param)
{
    TicTacToeIcon *me = reinterpret_cast<TicTacToeIcon *>(param);

    me->DoDelayedRelease();
}

void TicTacToeIcon::OnExitClicked()
{
    motor_vibe(1);
    mainbar_jump_to_tilenumber(app_tile_get_tile_num(), LV_ANIM_OFF);

    /* Delay this until the next task handler cycle */
    log_d("Queuing async release");
    lv_async_call(DelayedRelease, this);
}

void TicTacToeIcon::DoDelayedRelease()
{
    log_d("Triggering async release");
    mGameInstance.release();
}