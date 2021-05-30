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
#define CORE_DEBUG_LEVEL 5

#include "config.h"
#include <Arduino.h>
#include <memory>
#include <utility>

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/statusbar.h"
#include "hardware/display.h"
#include "hardware/motor.h"

#include "ttt_app.h"
#include "ttt_game.h"

//TODO: Placeholder background images
LV_IMG_DECLARE(bg1);
LV_IMG_DECLARE(bg2);

/* These would be unnecessary if LVGL supported a data param... */

static TicTacToeApp *gameInstance = 0;
static void OnSquareUL(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnTileClicked(0);
        break;
    }
}
static void OnSquareUC(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnTileClicked(1);
        break;
    }
}
static void OnSquareUR(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnTileClicked(2);
        break;
    }
}
static void OnSquareCL(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnTileClicked(3);
        break;
    }
}
static void OnSquareCC(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnTileClicked(4);
        break;
    }
}
static void OnSquareCR(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnTileClicked(5);
        break;
    }
}
static void OnSquareBL(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnTileClicked(6);
        break;
    }
}
static void OnSquareBC(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnTileClicked(7);
        break;
    }
}
static void OnSquareBR(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnTileClicked(8);
        break;
    }
}

static void OnExit(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnMenuClicked(TicTacToeApp::Exit);
        break;
    }
}

static void OnReset(struct _lv_obj_t *obj, lv_event_t event)
{
    switch (event)
    {
    case (LV_EVENT_CLICKED):
        gameInstance->OnMenuClicked(TicTacToeApp::Reset);
        break;
    }
}

TicTacToeApp::TicTacToeApp(TicTacToeIcon *icon)
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

        log_d("Creating button styles");
        lv_style_init(&mStyleRed);
        lv_style_set_bg_opa(&mStyleRed, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_bg_color(&mStyleRed, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_style_set_bg_grad_color(&mStyleRed, LV_STATE_DEFAULT, LV_COLOR_MAROON);
        lv_style_set_bg_grad_dir(&mStyleRed, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        lv_style_set_bg_color(&mStyleRed, LV_BTN_STATE_DISABLED, LV_COLOR_RED);
        lv_style_set_bg_grad_color(&mStyleRed, LV_BTN_STATE_DISABLED, LV_COLOR_MAROON);
        lv_style_set_bg_grad_dir(&mStyleRed, LV_BTN_STATE_DISABLED, LV_GRAD_DIR_VER);

        lv_style_init(&mStyleBlue);
        lv_style_set_bg_opa(&mStyleBlue, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_bg_color(&mStyleBlue, LV_STATE_DEFAULT, LV_COLOR_BLUE);
        lv_style_set_bg_grad_color(&mStyleBlue, LV_STATE_DEFAULT, LV_COLOR_NAVY);
        lv_style_set_bg_grad_dir(&mStyleBlue, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        lv_style_set_bg_color(&mStyleBlue, LV_BTN_STATE_DISABLED, LV_COLOR_BLUE);
        lv_style_set_bg_grad_color(&mStyleBlue, LV_BTN_STATE_DISABLED, LV_COLOR_NAVY);
        lv_style_set_bg_grad_dir(&mStyleBlue, LV_BTN_STATE_DISABLED, LV_GRAD_DIR_VER);

        lv_style_init(&mStyleBlank);
        lv_style_set_bg_opa(&mStyleBlank, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_bg_color(&mStyleBlank, LV_STATE_DEFAULT, LV_COLOR_SILVER);
        lv_style_set_bg_grad_color(&mStyleBlank, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_style_set_bg_grad_dir(&mStyleBlank, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
        lv_style_set_bg_color(&mStyleBlank, LV_BTN_STATE_DISABLED, LV_COLOR_SILVER);
        lv_style_set_bg_grad_color(&mStyleBlank, LV_BTN_STATE_DISABLED, LV_COLOR_GRAY);
        lv_style_set_bg_grad_dir(&mStyleBlank, LV_BTN_STATE_DISABLED, LV_GRAD_DIR_VER);
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
        lv_label_set_text_static(label, "Tic Tac Toe");
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
        // Display a TTT grid on the gameplay page
        constexpr int tops[3] = {16, 88, 160};
        constexpr int lefts[3] = {16, 88, 160};
        constexpr int width = 64;
        constexpr int height = 64;
        const lv_event_cb_t funcs[NUM_SQUARES] = {
            OnSquareUL, OnSquareUC, OnSquareUR,
            OnSquareCL, OnSquareCC, OnSquareCR,
            OnSquareBL, OnSquareBC, OnSquareBR};

        for (int i = 0; i < NUM_SQUARES; i++)
        {
            mButtons[i] = lv_btn_create(gameplayTile, NULL);
            if (!mButtons[i])
                log_e("Error creating button %d. Crash is immenent.", i);
            lv_obj_set_pos(mButtons[i], tops[i % 3], lefts[i / 3]);
            lv_obj_set_size(mButtons[i], width, height);
            lv_obj_reset_style_list(mButtons[i], LV_BTN_PART_MAIN);
            lv_obj_add_style(mButtons[i], LV_BTN_PART_MAIN, &mStyleBlank);
            lv_obj_set_event_cb(mButtons[i], funcs[i]);
        }

        ClearBoard();
    }

    log_d("Construction complete");
}

TicTacToeApp::~TicTacToeApp()
{
    // LVGL Objects parented to the tiles should be deleted when the tiles are destroyed.
    FreeAppTiles();
    gameInstance = nullptr;
}

// Open from watch menu
void TicTacToeApp::OnLaunch()
{
    lv_tileview_set_tile_act(GetTileView(), 0, 0, LV_ANIM_OFF);
}

void TicTacToeApp::OnExitClicked()
{
    log_d("Exiting...");
    // Pass along the message for differed deletion and return to main menu
    mParentIcon->OnExitClicked();
    FreeAppTiles();
}

void TicTacToeApp::OnTileClicked(int index)
{
    if (index < 0 || index >= NUM_SQUARES)
    {
        log_e("Invaid tile number, expected 0-%d, received %d", NUM_SQUARES, index);
        return;
    }

    if (mBoard[index] == Owner::None)
    {
        lv_style_t *style = (mCurrentPlayer == Owner::Red) ? &mStyleRed : &mStyleBlue;

        mBoard[index] = mCurrentPlayer;

        lv_obj_add_style(mButtons[index], LV_BTN_PART_MAIN, style);
        lv_btn_set_state(mButtons[index], LV_BTN_STATE_DISABLED);

        NextPlayer();
    }
    else
    {
        /* tile already owned, do an error response, beep or something. */
    }
}

void TicTacToeApp::ClearBoard()
{
    for (Owner &c : mBoard)
    {
        c = Owner::None;
    }
    for (lv_obj_t *button : mButtons)
    {
        lv_obj_reset_style_list(button, LV_BTN_PART_MAIN);
        lv_obj_add_style(button, LV_BTN_PART_MAIN, &mStyleBlank);
        lv_btn_set_state(button, LV_BTN_STATE_RELEASED);
    }
    mCurrentPlayer = Red;
}

void TicTacToeApp::OnMenuClicked(MenuItem item)
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