/****************************************************************************
 *   June 04 02:01:00 2021
 *   Copyright  2021  Dirk Sarodnick
 *   Email: programmer@dirk-sarodnick.de
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
lv_task_t * _pong_app_task;

void pong_app_task( lv_task_t * task )
{
    if ( !iconInstance.IsActive ) return;
    iconInstance.Loop();
}

void pong_game_setup()
{
    iconInstance.RegisterAppIcon();
    _pong_app_task = lv_task_create( pong_app_task, 50, LV_TASK_PRIO_HIGH, NULL );
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

PongIcon::PongIcon()
{
    pAppname = "Pong";
    pMenuIcon = &pong_64px;
    pStartFunction = startGame;
}

void PongIcon::OnStartClicked()
{
    motor_vibe(1);

    if(!mGameInstance)
    {
        log_d("Creating game instance.");
        mGameInstance = std::unique_ptr<PongApp>(new PongApp(this));
    }

    log_d("Launching game instance.");
    mGameInstance->OnLaunch();
    IsActive = true;
}

static void DelayedRelease(void* param)
{
    PongIcon *me = reinterpret_cast<PongIcon *>(param);

    me->DoDelayedRelease();
}

void PongIcon::OnExitClicked()
{
    motor_vibe(1);
    mainbar_jump_to_tilenumber(app_tile_get_tile_num(), LV_ANIM_OFF);
    IsActive = false;

    /* Delay this until the next task handler cycle */
    log_d("Queuing async release");
    lv_async_call(DelayedRelease, this);
}

void PongIcon::DoDelayedRelease()
{
    log_d("Triggering async release");
    mGameInstance.release();
}

void PongIcon::Loop()
{
    mGameInstance->Loop();
}