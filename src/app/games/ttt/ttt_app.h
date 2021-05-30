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

void tic_tac_toe_app_setup();

class TicTacToeIcon;

class TicTacToeApp : public GameBase
{

private:
    static constexpr int NUM_SQUARES = 9;

    TicTacToeIcon *mParentIcon = 0;

    enum Owner : uint8_t
    {
        None = 0,
        Red = 'X',
        Blue = 'O',
    };

    // Gameplay data
    Owner mBoard[NUM_SQUARES];
    Owner mCurrentPlayer = Red;

    // Visual data
    lv_style_t mStyleApp;
    lv_style_t mStyleMenu;
    lv_style_t mStyleRed;
    lv_style_t mStyleBlue;
    lv_style_t mStyleBlank;
    lv_obj_t *mButtons[NUM_SQUARES] = {0};

    void NextPlayer() { mCurrentPlayer = (mCurrentPlayer == Red) ? Blue : Red; };

    void OnExitClicked();

public:
    enum MenuItem : uint8_t
    {
        Reset,
        Exit,
        NumMenuItems
    };

    TicTacToeApp(TicTacToeIcon *callingIcon);
    ~TicTacToeApp();

    void ClearBoard();

    // Launch from watch mainbar
    void OnLaunch();

    // TTT square pressed
    void OnTileClicked(int index);

    // A menu item was clicked.
    void OnMenuClicked(MenuItem item);
};