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

#include "app\games\gamebase.h"

#define FIELD_WIDTH 240
#define FIELD_HEIGHT 240
#define PLAYER1_X 0
#define PLAYER2_X 230
#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 40
#define PLAYER_BOUNDARY ((FIELD_HEIGHT / 2) - (PLAYER_HEIGHT / 2))
#define PLAYER_SPEED_MAX 10
#define BALL_WIDTH 8
#define BALL_HEIGHT 8
#define BALL_SPEED_MIN 3
#define BALL_SPEED_MAX 15

void pong_app_setup();

class PongIcon;

class PongApp : public GameBase
{

private:
    PongIcon *mParentIcon = 0;
    bool pong_inited = false;
    bool pong_active = false;
    
    // Gameplay data
    uint8_t ball_speed = BALL_SPEED_MIN;
    uint16_t ball_bounce = 0;
    uint16_t ball_degree = 0;
    float ball_x = (FIELD_WIDTH / 2);
    float ball_y = (FIELD_HEIGHT / 2);
    int16_t player1_y = (FIELD_HEIGHT / 2);
    int16_t player2_y = (FIELD_HEIGHT / 2);
    int8_t cpu_velocity = 0;
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

    bool CheckCollision();

    bool TurnDegree(uint16_t base_degree, int8_t altered_degree);

    void UpdateBall();

    void UpdatePlayer1();

    void UpdatePlayer2();

    void UpdateBoard();

    bool BounceWallTop();

    bool BounceWallBottom();

    bool BouncePlayer1();

    bool BouncePlayer2();

    bool ScorePlayer1();

    bool ScorePlayer2();

    void ResetBall();

    void ResetPlayer1();

    void ResetPlayer2();

    void ResetBoard();

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

    // Loops some typical game logic
    void Loop();

    // Resets the whole game
    void ResetGame();

    // Launch from watch mainbar
    void OnLaunch();

    // A menu item was clicked.
    void OnMenuClicked(MenuItem item);

    // A menu item was clicked.
    void OnTileChanged();

    // Watch goes into standby.
    void OnStandby();
};