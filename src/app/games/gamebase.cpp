/*   July 31 21:38:03 2020
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
#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/app_tile/app_tile.h" //TODO: Swap this with a game app tile
#include "gamebase.h"

GameBase::~GameBase()
{
    FreeAppTiles();
}

bool GameBase::AllocateAppTiles(int xCount, int yCount)
{
    if (xCount < 1 || yCount < 1)
    {
        log_e("Must have at least one tile in each dimension");
        return false;
    }
    if (mTileView || mTiles.size() != 0)
    {
        log_e("App already contains tiles.");
        return false;
    }

    mTileView = lv_tileview_create(lv_scr_act(), NULL);
    if (!mTileView)
    {
        log_e("Couldn't create tile view");
        return false;
    }
    mTilesX = xCount;
    mTilesY = yCount;

    const int numtiles = xCount * yCount;
    mTiles.resize(numtiles);
    mTilePositions.resize(numtiles);

    // Prepare "valid positions" array
    for (int i = 0; i < numtiles; i++)
    {
        mTilePositions[i].x = i % xCount;
        mTilePositions[i].y = i / xCount;
    }

    // Allocate all the tiles.
    for (int i = 0; i < numtiles; i++)
    {
        mTiles[i] = lv_obj_create(mTileView, NULL);
        if (!mTiles[i])
            log_e("Error creating tile %d. Crashing...", i);
        lv_obj_set_size(mTiles[i], LV_HOR_RES, LV_VER_RES);

        lv_obj_set_pos(mTiles[i], mTilePositions[i].x * LV_HOR_RES, mTilePositions[i].y * LV_VER_RES);
        lv_tileview_add_element(mTileView, mTiles[i]);
    }

    lv_tileview_set_valid_positions(mTileView, mTilePositions.data(), mTilePositions.size());
    lv_tileview_set_edge_flash(mTileView, false);

    return true;
}

void GameBase::FreeAppTiles()
{
    if (mTileView)
    {
        lv_obj_clean(mTileView);
        lv_obj_del(mTileView);
    }
    // All the tiles are owned by the tileview, so just dump the pointers.
    mTileView = 0;
    mTiles.clear();
    mTilePositions.clear();
    mTilesX = 0;
    mTilesY = 0;
}
