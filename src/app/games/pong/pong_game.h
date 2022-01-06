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

#pragma once

#include "app/games/GameIconBase.h"
#include "memory"

/* plain function for main app to configure. */
void pong_game_setup();

class PongApp;

class PongIcon : public GameIconBase
{
private:
    std::unique_ptr<PongApp> mGameInstance;

public:
    PongIcon();
    void OnStartClicked();
    void OnExitClicked();
    void DoDelayedRelease();
    void Loop();
    bool IsActive = false;
};
