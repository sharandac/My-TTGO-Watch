/****************************************************************************
 *   Apr 17 00:28:11 2021
 *   Copyright  2021  Federico Liuzzi
 *   Email: f.liuzzi02@gmail.com
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

#include "sailing.h"
#include "sailing_setup.h"

#include "hardware/display.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else
    #include <Arduino.h>
#endif

lv_obj_t *sailing_setup_tile = NULL;

lv_obj_t *sailing_foobar_switch = NULL;
lv_obj_t *sailing_track_switch = NULL;

bool tracking = false;

static void sailing_foobar_switch_event_cb( lv_obj_t * obj, lv_event_t event );
static void sailing_track_switch_event_cb( lv_obj_t * obj, lv_event_t event );

void sailing_setup_setup( uint32_t tile_num ) {
    /**
     * get tile obj from tile number
     */
    sailing_setup_tile = mainbar_get_tile_obj( tile_num );
    /**
     * add setup header with exit button
     */
    lv_obj_t *header = wf_add_settings_header( sailing_setup_tile, "Exit setup" );
    lv_obj_align( header, sailing_setup_tile, LV_ALIGN_IN_TOP_LEFT, 10, 10 );
    /**
     * setup always on display switch
     */
    lv_obj_t *sailing_foobar_switch_cont = wf_add_labeled_switch( sailing_setup_tile, "Always on display", &sailing_foobar_switch, false, sailing_foobar_switch_event_cb, SETUP_STYLE );
    lv_obj_align( sailing_foobar_switch_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
    /**
     * setup show track switch
     */
    lv_obj_t *sailing_track_switch_cont = wf_add_labeled_switch( sailing_setup_tile, "Show track", &sailing_track_switch, false, sailing_track_switch_event_cb, SETUP_STYLE );
    lv_obj_align( sailing_track_switch_cont, sailing_foobar_switch_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 5 );
}

static void sailing_foobar_switch_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): SAILING_INFO_LOG( "switch value = %d", lv_switch_get_state( obj ) );
                                        if( lv_switch_get_state( obj ) == 1 ) display_set_timeout( 300 );
                                        else display_set_timeout( 15 );
                                        break;
    }
}

static void sailing_track_switch_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): SAILING_INFO_LOG( "switch value = %d", lv_switch_get_state( obj ) );
                                        if( lv_switch_get_state( obj ) == 1 ) tracking = true;
                                        else tracking = false;
                                        break;
    }
}
