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
#include "gui/sound/piep_higher.h"
#include "gui/sound/piep_high.h"
#include "gui/sound/piep_lower.h"
#include "gui/sound/piep_low.h"
#include "hardware/display.h"
#include "hardware/motor.h"
#include "hardware/sound.h"

#include "pong_app.h"
#include "pong_game.h"

//TODO: Placeholder background images
LV_IMG_DECLARE(bg1);
LV_IMG_DECLARE(bg2);
LV_FONT_DECLARE(Ubuntu_48px);

#define FIELD_WIDTH 240
#define FIELD_HEIGHT 240
#define PLAYER1_X 0
#define PLAYER2_X 230
#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 40
#define PLAYER_BOUNDARY ((FIELD_HEIGHT / 2) - (PLAYER_HEIGHT / 2))
#define BALL_WIDTH 8
#define BALL_HEIGHT 8

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

        ClearBoard();
        ResetBall();
    }

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
    pong_active = true;
    lv_tileview_set_tile_act(GetTileView(), 0, 0, LV_ANIM_OFF);
}

void PongApp::OnExitClicked()
{
    pong_active = false;
    log_d("Exiting...");
    // Pass along the message for differed deletion and return to main menu
    mParentIcon->OnExitClicked();
    FreeAppTiles();
}

void PongApp::Loop()
{
    if ( !pong_inited || !pong_active ) return;
    UpdatePlayer();
    //TODO: implement ball movement
    //TODO: implement ball boundary and collision check
    //TODO: implement p2 (cpu) behavior -> tracking the ball with speed limit and small delay/velocity
}

void PongApp::UpdatePlayer()
{
    TTGOClass * ttgo = TTGOClass::getWatch();

    Accel acc;
    ttgo->bma->getAccel(acc);

    int16_t y = acc.y * 0.2;
    if (y > 0 + PLAYER_BOUNDARY) y = 0 + PLAYER_BOUNDARY;
    if (y < 0 - PLAYER_BOUNDARY) y = 0 - PLAYER_BOUNDARY;

	lv_obj_set_pos(bar_player1, PLAYER1_X, PLAYER_BOUNDARY + y);
    lv_disp_trig_activity( NULL );
}

void PongApp::UpdateBoard()
{
    log_i("Updating Board to %d : %d", score_p1, score_p2);

    char temp[10];
    snprintf(temp, sizeof(temp), "%d : %d", score_p1, score_p2);
    lv_label_set_text(label_scoreboard, temp);
    lv_event_send_refresh(label_scoreboard);
}

void PongApp::ClearBoard()
{
    log_i("Clearing Board to 0 : 0");
    score_p1 = 0;
    score_p2 = 0;
    UpdateBoard();
}

void PongApp::ResetBall()
{
    log_d("Resetting Ball...");
	lv_obj_set_pos(bar_ball, (FIELD_WIDTH / 2) - (BALL_WIDTH / 2), (FIELD_HEIGHT / 2) - (BALL_HEIGHT / 2));
}

void PongApp::OnMenuClicked(MenuItem item)
{
    switch (item)
    {
        case Reset:
            ClearBoard();
            lv_tileview_set_tile_act(GetTileView(), 1, 0, LV_ANIM_ON);
            break;
        case Exit:
            OnExitClicked();
            break;
        default:
            log_e("Unknown menu command %d", item);
    }
}