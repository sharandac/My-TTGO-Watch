/****************************************************************************
 *   Tu May 22 21:23:51 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
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
#ifndef _WEATHER_H
    #define _WEATHER_H

    #include <TTGO.h>

    #define WEATHER_CONFIG_FILE             "/weather.cfg"

    #define WEATHER_WIDGET_SYNC_REQUEST    _BV(0)

    #define WEATHER_MAX_FORECAST            4

    typedef struct {
        char apikey[64] = "";
        char lon[16] = "";
        char lat[16] = "";
        bool autosync = true;
    } weather_config_t;

    typedef struct {
        bool valide = false;
        char temp[8] = "";
        char pressure[8] = "";
        char humidity[8] = "";
        char name[32] = "";
        char icon[8] = "";
    } weather_forcast_t;

    void weather_widget_setup( void );

    void weather_widget_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );

    void weather_widget_setup_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );

    weather_config_t *weather_get_config( void );

    void weather_jump_to_forecast( void );

    void weather_jump_to_setup( void );

    void weather_sync_request( void );

    void weather_save_config( void );

    void weather_load_config( void );
    
#endif // _WEATHER_H
