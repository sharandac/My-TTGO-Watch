/****************************************************************************
 *  watchface_tile.h
 *  Neuroplant  11/2020
 *  Email: m.roych@gmail.com
 *
 *
 *  Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  note-tile"
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
#ifndef _WATCHFACE_TILE_H
    #define _WATCHFACE_TILE_H

    #include <TTGO.h>
	
	#define SHADOW_X 4
	#define SHADOW_Y 5

	struct Analog_Parts_t
	{
		lv_obj_t* Marks; 	//use transparent Image with Marks to use general Background
		lv_obj_t* Second;
		lv_obj_t* Minute;
		lv_obj_t* Hour;
	//	lv_obj_t* Moon;		//you can add more complications
		lv_style_t Hands_Style;
	};

    void watchface_tile_setup( void );
	bool watchface_tile_powermgm_event_cb( EventBits_t event, void *arg );
	void watchface_tile_update_time( void );
	void watchface_tile_update_task( lv_task_t * task ) ;


#endif // _WATCHFACE_TILE_H
