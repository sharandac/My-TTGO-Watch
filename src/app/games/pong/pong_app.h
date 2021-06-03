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

#pragma once

#include "app\games\gamebase.h"

void pong_app_setup();

class PongIcon;

class PongApp : public GameBase
{

private:
    PongIcon *mParentIcon = 0;
    bool pong_inited = false;
    bool pong_active = false;
    
    // Gameplay data
    uint8_t score_p1 = 0;
    uint8_t score_p2 = 0;
    
    // Visual data
    lv_style_t mStyleApp;
    lv_style_t mStyleMenu;
    lv_style_t style_ball;
    lv_style_t style_player1;
    lv_style_t style_player2;
    lv_style_t style_scoreboard;
    lv_obj_t* bar_ball;
    lv_obj_t* bar_player1;
    lv_obj_t* bar_player2;
    lv_obj_t* label_scoreboard;

    void OnExitClicked();

public:
    enum MenuItem : uint8_t
    {
        Reset,
        Exit,
        NumMenuItems
    };

    PongApp(PongIcon *callingIcon);
    ~PongApp();

    void Loop();

    void UpdatePlayer();

    void UpdateBoard();

    void ClearBoard();

    void ResetBall();

    // Launch from watch mainbar
    void OnLaunch();

    // A menu item was clicked.
    void OnMenuClicked(MenuItem item);
};