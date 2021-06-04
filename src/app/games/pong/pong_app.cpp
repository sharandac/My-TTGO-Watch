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
#include "gui/sound/piep_higher.h"
#include "gui/sound/piep_high.h"
#include "gui/sound/piep_lower.h"
#include "gui/sound/piep_low.h"
#include "hardware/display.h"
#include "hardware/motor.h"
#include "hardware/powermgm.h"
#include "hardware/sound.h"

#include "pong_app.h"
#include "pong_game.h"

//TODO: Placeholder background images
LV_IMG_DECLARE(bg1);
LV_IMG_DECLARE(bg2);
LV_FONT_DECLARE(Ubuntu_48px);

/* These would be unnecessary if LVGL supported a data param... */

static PongApp *gameInstance = 0;
static void OnExit(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
        case (LV_EVENT_CLICKED):
            gameInstance->OnMenuClicked(PongApp::Exit);
            break;
    }
}

static void OnReset(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
        case (LV_EVENT_CLICKED):
            gameInstance->OnMenuClicked(PongApp::Reset);
            break;
    }
}

static void OnSwitch(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
        case (LV_EVENT_VALUE_CHANGED):
            gameInstance->OnTileChanged();
            break;
    }
}

static bool OnPower(EventBits_t event, void *arg)
{
    switch( event ) {
        case( POWERMGM_STANDBY ):
            gameInstance->OnStandby();
            break;
    }
    return( true );
}

PongApp::PongApp(PongIcon *icon)
{
    gameInstance = this;
    mParentIcon = icon;

    log_d("Creating game tiles...");
    if (!AllocateAppTiles(2, 1))
    {
        log_e("Could not allocate tiles. Aborting.");
        return;
    }
    lv_obj_t *menuTile = GetTile(0);
    lv_obj_t *gameplayTile = GetTile(1);

    log_d("Initializing styles");
    {
        // Create a general application style for all the app's tiles in the view
        lv_style_init(&mStyleApp);
        lv_style_set_radius(&mStyleApp, LV_OBJ_PART_MAIN, 0);
        lv_style_set_bg_color(&mStyleApp, LV_OBJ_PART_MAIN, LV_COLOR_NAVY);
        lv_style_set_bg_opa(&mStyleApp, LV_OBJ_PART_MAIN, LV_OPA_0);
        lv_style_set_border_width(&mStyleApp, LV_OBJ_PART_MAIN, 0);
        lv_style_set_text_color(&mStyleApp, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
        lv_style_set_image_recolor(&mStyleApp, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);

        lv_tileview_set_edge_flash(GetTileView(), false);
        lv_obj_add_style(GetTileView(), LV_OBJ_PART_MAIN, &mStyleApp);
        lv_page_set_scrlbar_mode(GetTileView(), LV_SCRLBAR_MODE_DRAG);
        lv_obj_set_event_cb(GetTileView(), OnSwitch);

        // Initialize screen backgrounds
        log_d("Creating background for menu tile");
        lv_obj_t *img_bin = lv_img_create(menuTile, NULL);
        lv_img_set_src(img_bin, &bg1);
        lv_obj_set_width(img_bin, LV_HOR_RES);
        lv_obj_set_height(img_bin, LV_VER_RES);
        lv_obj_align(img_bin, NULL, LV_ALIGN_CENTER, 0, 0);

        log_d("Creating background for gameplay tile");
        lv_obj_t *gameplay_img = lv_img_create(gameplayTile, NULL);
        lv_img_set_src(gameplay_img, &bg2);
        lv_obj_set_width(gameplay_img, LV_HOR_RES);
        lv_obj_set_height(gameplay_img, LV_VER_RES);
        lv_obj_align(gameplay_img, NULL, LV_ALIGN_CENTER, 0, 0);

        log_d("Creating menu styles");
        lv_style_init(&mStyleMenu);
        lv_style_copy(&mStyleMenu, &mStyleApp);
        lv_style_set_text_color(&mStyleMenu, LV_LABEL_PART_MAIN, LV_COLOR_WHITE);
        lv_style_set_bg_opa(&mStyleMenu, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_bg_color(&mStyleMenu, LV_STATE_DEFAULT, LV_COLOR_BLUE);
        lv_style_set_bg_grad_color(&mStyleMenu, LV_STATE_DEFAULT, LV_COLOR_NAVY);
        lv_style_set_bg_grad_dir(&mStyleMenu, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        lv_obj_add_style(menuTile, LV_LABEL_PART_MAIN, &mStyleMenu);

        log_d("Creating game styles");
        lv_style_init(&style_ball);
        lv_style_set_bg_opa(&style_ball, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_bg_color(&style_ball, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_style_set_bg_grad_color(&style_ball, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        lv_style_set_bg_grad_dir(&style_ball, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        lv_style_set_border_color(&style_ball, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_style_set_border_width(&style_ball, LV_STATE_DEFAULT, 1);

        lv_style_init(&style_player1);
        lv_style_set_bg_opa(&style_player1, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_bg_color(&style_player1, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_style_set_bg_grad_color(&style_player1, LV_STATE_DEFAULT, LV_COLOR_MAROON);
        lv_style_set_bg_grad_dir(&style_player1, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        lv_style_set_border_color(&style_player1, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_style_set_border_width(&style_player1, LV_STATE_DEFAULT, 1);

        lv_style_init(&style_player2);
        lv_style_set_bg_opa(&style_player2, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_bg_color(&style_player2, LV_STATE_DEFAULT, LV_COLOR_BLUE);
        lv_style_set_bg_grad_color(&style_player2, LV_STATE_DEFAULT, LV_COLOR_NAVY);
        lv_style_set_bg_grad_dir(&style_player2, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        lv_style_set_border_color(&style_player2, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_style_set_border_width(&style_player2, LV_STATE_DEFAULT, 1);

        lv_style_init(&style_scoreboard);
        lv_style_set_text_font(&style_scoreboard, LV_STATE_DEFAULT, &Ubuntu_48px);
        lv_style_set_text_color(&style_scoreboard, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    }

    log_d("Initializing menu");
    {
        lv_obj_t *button;
        lv_obj_t *label;

        int offset = 64; // starting offset down the screen
        constexpr int buttonSpacing = 32;
        constexpr int buttonWidth = 120;
        constexpr int buttonHeight = 24;

        label = lv_label_create(menuTile, NULL);
        lv_label_set_text_static(label, "Pong");
        lv_obj_align(label, menuTile, LV_ALIGN_IN_TOP_MID, 0, 10);
        lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);

        button = lv_btn_create(menuTile, NULL);
        label = lv_label_create(button, NULL);
        lv_label_set_text(label, "New Game");
        lv_obj_set_event_cb(button, OnReset);
        lv_obj_align(button, menuTile, LV_ALIGN_IN_TOP_MID, 0, offset);
        lv_obj_set_size(button, buttonWidth, buttonHeight);
        offset += buttonSpacing;

        button = lv_btn_create(menuTile, NULL);
        label = lv_label_create(button, NULL);
        lv_label_set_text(label, "Exit");
        lv_obj_set_event_cb(button, OnExit);
        lv_obj_align(button, menuTile, LV_ALIGN_IN_TOP_MID, 0, offset);
        lv_obj_set_size(button, buttonWidth, buttonHeight);
        offset += buttonSpacing;

        label = lv_label_create(menuTile, NULL);
        lv_label_set_text_static(label, "Swipe left for the play area.");
        lv_obj_align(label, menuTile, LV_ALIGN_IN_TOP_MID, 0, offset);
        lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
        offset += buttonSpacing;
    }

    log_d("Initializing game board");
    {
        bar_ball = lv_bar_create(gameplayTile, NULL);
        lv_obj_add_style(bar_ball, LV_OBJ_PART_MAIN, &style_ball);
        lv_obj_align(bar_ball, gameplayTile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
        lv_obj_set_click(bar_ball, false);
        lv_obj_set_size(bar_ball, BALL_WIDTH, BALL_HEIGHT);
	    lv_bar_set_anim_time(bar_ball, 50);

        bar_player1 = lv_bar_create(gameplayTile, NULL);
        lv_obj_add_style(bar_player1, LV_OBJ_PART_MAIN, &style_player1);
        lv_obj_align(bar_player1, gameplayTile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
        lv_obj_set_click(bar_player1, false);
        lv_obj_set_size(bar_player1, PLAYER_WIDTH, PLAYER_HEIGHT);
	    lv_obj_set_pos(bar_player1, PLAYER1_X, (FIELD_HEIGHT / 2) - (PLAYER_HEIGHT / 2));
	    lv_bar_set_anim_time(bar_player1, 50);

        bar_player2 = lv_bar_create(gameplayTile, NULL);
        lv_obj_add_style(bar_player2, LV_OBJ_PART_MAIN, &style_player2);
        lv_obj_align(bar_player2, gameplayTile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
        lv_obj_set_click(bar_player2, false);
        lv_obj_set_size(bar_player2, PLAYER_WIDTH, PLAYER_HEIGHT);
	    lv_obj_set_pos(bar_player2, PLAYER2_X, (FIELD_HEIGHT / 2) - (PLAYER_HEIGHT / 2));
	    lv_bar_set_anim_time(bar_player2, 50);

        label_scoreboard = lv_label_create(gameplayTile, NULL);
        lv_obj_add_style(label_scoreboard, LV_OBJ_PART_MAIN, &style_scoreboard);
        lv_obj_align(label_scoreboard, gameplayTile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
        lv_label_set_align(label_scoreboard, LV_LABEL_ALIGN_CENTER);
        lv_label_set_long_mode(label_scoreboard, LV_LABEL_LONG_CROP);
        lv_obj_set_click(label_scoreboard, false);
	    lv_obj_set_size(label_scoreboard, 240, 60);

        ResetGame();
    }
    
    powermgm_register_cb( POWERMGM_STANDBY, OnPower, "pong powermgm");

    pong_inited = true;
    log_d("Construction complete");
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
    lv_tileview_set_tile_act(GetTileView(), 0, 0, LV_ANIM_OFF);
}

void PongApp::OnExitClicked()
{
    log_d("Exiting...");
    // Pass along the message for differed deletion and return to main menu
    mParentIcon->OnExitClicked();
    FreeAppTiles();
}

void PongApp::Loop()
{
    if ( !pong_inited || !pong_active ) return;

    UpdateBall();
    UpdatePlayer1();
    UpdatePlayer2();
    CheckCollision();

    lv_disp_trig_activity( NULL );
}

bool PongApp::CheckCollision()
{
    // check if ball hit p1 or p2
    if (ball_x <= 0 + PLAYER_WIDTH + (BALL_WIDTH / 2) && ball_y >= player1_y - (PLAYER_HEIGHT / 2) + (FIELD_HEIGHT / 2) && ball_y <= player1_y + (PLAYER_HEIGHT / 2) + (FIELD_HEIGHT / 2)) return BouncePlayer1();
    if (ball_x >= FIELD_WIDTH - PLAYER_WIDTH - (BALL_WIDTH / 2) && ball_y >= player2_y - (PLAYER_HEIGHT / 2) + (FIELD_HEIGHT / 2) && ball_y <= player2_y + (PLAYER_HEIGHT / 2) + (FIELD_HEIGHT / 2)) return BouncePlayer2();

    // check if ball hit the left or right wall
    if (ball_x <= 0 + (BALL_WIDTH / 2)) return ScorePlayer2();
    if (ball_x >= FIELD_WIDTH - (BALL_WIDTH / 2)) return ScorePlayer1();

    // check if ball hit top or bottom wall
    if (ball_y <= 0 + (BALL_HEIGHT / 2)) return BounceWallTop(); //TODO: Only bounce, if direction is heading into wall
    if (ball_y >= FIELD_HEIGHT - (BALL_HEIGHT / 2)) return BounceWallBottom(); //TODO: Only bounce, if direction is heading into wall

    return false;
}

bool PongApp::TurnDegree(uint16_t base_degree, int8_t altered_degree)
{
    int16_t new_degree = (base_degree * 2) - 180 - ball_degree + altered_degree;
    while (new_degree < 0) new_degree += 360;
    while (new_degree >= 360) new_degree -= 360;
    //TODO: Add maximum degree difference of less then 90 degree

    log_d("Turn Degree from %d to %d using base of %d", ball_degree, new_degree, base_degree);
    ball_degree = new_degree;

    return true;
}

bool PongApp::BounceWallTop()
{
    log_d("Bounce Wall Top");

    TurnDegree(90, 0);
    motor_vibe(1);

    return true;
}

bool PongApp::BounceWallBottom()
{
    log_d("Bounce Wall Bottom");

    TurnDegree(270, 0);
    motor_vibe(1);

    return true;
}

bool PongApp::BouncePlayer1()
{
    int8_t altered_degree = map(ball_y, player1_y - (PLAYER_HEIGHT / 2) + (FIELD_HEIGHT / 2), player1_y + (PLAYER_HEIGHT / 2) + (FIELD_HEIGHT / 2), -45, 45);
    if (altered_degree < 5 && altered_degree > -5) altered_degree = 0;

    log_d("Bounce Player 1 with altered Degree %d", altered_degree);
    TurnDegree(0, altered_degree);
    
    if (ball_bounce > 0 && ball_bounce % 2 == 0) ball_speed++;
    ball_bounce++;

    sound_play_progmem_wav( piep_high_wav, piep_high_wav_len );
    motor_vibe(3);

    return true;
}

bool PongApp::BouncePlayer2()
{
    int8_t altered_degree = map(ball_y, player2_y - (PLAYER_HEIGHT / 2) + (FIELD_HEIGHT / 2), player2_y + (PLAYER_HEIGHT / 2) + (FIELD_HEIGHT / 2), 45, -45);
    if (altered_degree < 5 && altered_degree > -5) altered_degree = 0;

    log_d("Bounce Player 2 with altered Degree %d", altered_degree);
    TurnDegree(180, altered_degree);

    if (ball_bounce > 0 && ball_bounce % 2 == 0) ball_speed++;
    ball_bounce++;

    sound_play_progmem_wav( piep_low_wav, piep_low_wav_len );
    motor_vibe(3);

    return true;
}

bool PongApp::ScorePlayer1()
{
    log_d("Score Player 1");

    score_p1++;
    UpdateBoard();
    ResetBall();

    sound_play_progmem_wav( piep_higher_wav, piep_higher_wav_len );
    motor_vibe(10);

    return true;
}

bool PongApp::ScorePlayer2()
{
    log_d("Score Player 2");

    score_p2++;
    UpdateBoard();
    ResetBall();

    sound_play_progmem_wav( piep_lower_wav, piep_lower_wav_len );
    motor_vibe(10);

    return true;
}

void PongApp::UpdateBall()
{
    if (ball_speed < BALL_SPEED_MIN) ball_speed = BALL_SPEED_MIN;
    if (ball_speed > BALL_SPEED_MAX) ball_speed = BALL_SPEED_MAX;

    ball_x = ball_x + ((float)ball_speed * cos((float)ball_degree * PI / 180));
    ball_y = ball_y + ((float)ball_speed * sin((float)ball_degree * PI / 180));

	lv_obj_set_pos(bar_ball, ball_x - (BALL_WIDTH / 2), ball_y - (BALL_HEIGHT / 2));
}

void PongApp::UpdatePlayer1()
{
    TTGOClass * ttgo = TTGOClass::getWatch();

    Accel acc;
    ttgo->bma->getAccel(acc);

    // set new position by accelerator
    int16_t new_position = acc.y * 0.2;
    if (new_position > 0 + PLAYER_BOUNDARY) new_position = 0 + PLAYER_BOUNDARY;
    if (new_position < 0 - PLAYER_BOUNDARY) new_position = 0 - PLAYER_BOUNDARY;

    // limit distance by maximum speed
    if (new_position < player1_y && player1_y - new_position > PLAYER_SPEED_MAX) new_position = player1_y - PLAYER_SPEED_MAX;
    if (new_position > player1_y && new_position - player1_y > PLAYER_SPEED_MAX) new_position = player1_y + PLAYER_SPEED_MAX;

    player1_y = new_position;
	lv_obj_set_pos(bar_player1, PLAYER1_X, PLAYER_BOUNDARY + player1_y);
}

void PongApp::UpdatePlayer2()
{
    // determine ball position
    int16_t new_target = ball_y - (FIELD_HEIGHT / 2);

    // determine direction of movement to get to target and slowly increase speed
    if (new_target < player2_y) {
        if (cpu_velocity > 0 - PLAYER_SPEED_MAX && random(0, 2) > 0) cpu_velocity--;
    }
    if (new_target > player2_y) {
        if (cpu_velocity < 0 + PLAYER_SPEED_MAX && random(0, 2) > 0) cpu_velocity++;
    }

    // set new position by ball position
    int16_t new_position = player2_y + cpu_velocity;
    if (new_position > 0 + PLAYER_BOUNDARY) new_position = 0 + PLAYER_BOUNDARY;
    if (new_position < 0 - PLAYER_BOUNDARY) new_position = 0 - PLAYER_BOUNDARY;
    if (new_position < player2_y && player2_y - new_position > PLAYER_SPEED_MAX) new_position = player2_y - PLAYER_SPEED_MAX;
    if (new_position > player2_y && new_position - player2_y > PLAYER_SPEED_MAX) new_position = player2_y + PLAYER_SPEED_MAX;

    player2_y = new_position;
	lv_obj_set_pos(bar_player2, PLAYER2_X, PLAYER_BOUNDARY + player2_y);
}

void PongApp::UpdateBoard()
{
    log_i("Updating Board to %d : %d", score_p1, score_p2);

    char temp[10];
    snprintf(temp, sizeof(temp), "%d : %d", score_p1, score_p2);
    lv_label_set_text(label_scoreboard, temp);
    lv_event_send_refresh(label_scoreboard);
}

void PongApp::ResetBall()
{
    log_d("Resetting Ball...");

    ball_speed = BALL_SPEED_MIN;
    ball_bounce = 0;

    ball_degree = random(-45, 45);
    if (ball_degree < 0) ball_degree += 360;

    ball_x = (FIELD_WIDTH / 2);
    ball_y = (FIELD_WIDTH / 2);

	lv_obj_set_pos(bar_ball, ball_x - (BALL_WIDTH / 2), ball_y - (BALL_HEIGHT / 2));
}

void PongApp::ResetBoard()
{
    log_d("Resetting Board to 0 : 0");
    score_p1 = 0;
    score_p2 = 0;
    UpdateBoard();
}

void PongApp::ResetPlayer1()
{
    log_d("Resetting Player 1");
    player1_y = 0;
}

void PongApp::ResetPlayer2()
{
    log_d("Resetting Player 2");
    player2_y = 0;
    cpu_velocity = 0;
}

void PongApp::ResetGame()
{
    ResetBall();
    ResetBoard();
    ResetPlayer1();
    ResetPlayer2();
}

void PongApp::OnMenuClicked(MenuItem item)
{
    switch (item)
    {
        case Reset:
            ResetGame();
            lv_tileview_set_tile_act(GetTileView(), 1, 0, LV_ANIM_ON);
            pong_active = true;
            break;
        case Exit:
            OnExitClicked();
            pong_active = false;
            break;
        default:
            log_e("Unknown menu command %d", item);
    }
}

void PongApp::OnTileChanged()
{
    lv_coord_t x;
    lv_coord_t y;
    lv_tileview_get_tile_act(GetTileView(), &x, &y);
    log_d("Tile changed to %d, %d", x, y);

    if (x == 1) pong_active = true;
    else pong_active = false;
}

void PongApp::OnStandby()
{
    pong_active = false;
}