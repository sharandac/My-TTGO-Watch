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

#pragma once

#include <vector>

class GameBase
{
private:
    // Tile management
    lv_obj_t *mTileView = 0;
    std::vector<lv_obj_t *> mTiles;
    std::vector<lv_point_t> mTilePositions; // pointer kept inside LVGM after tiles created.
    int mTilesX = 0;
    int mTilesY = 0;

protected:
    /* Create tiles for this application
    @param xCount number of tiles wide, must be at least 1
    @param yCount number of tiles tall, must be at least 1
    @return True on successful creation. False means errors with registration, check the log.
    */
    bool AllocateAppTiles(int xCount, int yCount);
    /* Release app tiles */
    void FreeAppTiles();
    /* Get allocated tile by index */
    lv_obj_t *GetTile(int idx) { return mTiles[idx]; }
    /* Get allocated tile by sliding position */
    lv_obj_t *GetTile(int x, int y) { return mTiles[y * mTilesX + x]; };
    /* Get the view associated with the application tiles */
    lv_obj_t *GetTileView() { return mTileView; }

public:
    GameBase(){};
    virtual ~GameBase();

};
