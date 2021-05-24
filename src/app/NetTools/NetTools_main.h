/****************************************************************************
 *  NetTools_main.h
 *  Copyright  2020  David Stewart / NorthernDIY
 *  Email: genericsoftwaredeveloper@gmail.com
 *
 *  Requires Libraries: 
 *      WakeOnLan by a7md0      https://github.com/a7md0/WakeOnLan
 *
 *  Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  Example_App"
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
#ifndef _NETTOOLS_MAIN_H
    #define _NETTOOLS_MAIN_H

    #include <TTGO.h>

    void NetTools_main_setup( uint32_t tile_num );

#endif // _NETTOOLS_MAIN_H

//These are button labels, replace as needed to suit your setup   
#define WOL_NAME "WakePC"
#define TOGGLE1_NAME "Tasmota 1"
#define TOGGLE2_NAME "Tasmota 2"

