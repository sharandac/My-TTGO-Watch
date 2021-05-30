/*   Aug 5 15:57:34 2020
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

/* Handles the common code for putting a game on a menu. 
    This serves as a container for a game application, and could be adapted to other applications.

    Designed to reduce memory needs on the device.
    The game instance is created after the icon is tapped, and destroyed at some point after the game has completed.
    This eliminates the need for all the gameplay data from existing all the time.

    USE:
    * Derive from this class. Initialize the values for the icon to use.
    * Create a static function for the watch's menubar to call for setup
    * Call that function in my-ttgo-watch.ino
    
    See TicTacToe icon for simple example.  It has a pointer instance to the real application.
    When the watch's menubar calls the icon's start function, the game's app instance is created.
    When the application is done, it uses a delayed release to deallocate the app instance.
*/

#pragma once

class GameIconBase
{
private:
protected:
    // Set these during construction, used in DoRegisterApp
    const lv_img_dsc_t *pMenuIcon = 0;
    const char *pAppname = 0;
    lv_event_cb_t pStartFunction = 0;

public:
    /* Register the icon with with the watch. */
    bool RegisterAppIcon();

    /* Return to watch menu. Does not release resources. */
    void ReturnToMenu();
};