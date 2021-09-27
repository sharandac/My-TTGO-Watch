/****************************************************************************
 *   Apr 13 14:17:11 2021
 *   Copyright  2021  Cornelius Wild
 *   Email: tt-watch-code@dervomsee.de
 *   Based on the work of Dirk Brosswick,  sharandac / My-TTGO-Watch  Example_App"
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

#include "gps_status.h"
#include "gps_status_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else
    #include <Arduino.h>
#endif

lv_obj_t *gps_status_setup_tile = NULL;
lv_obj_t *gps_status_foobar_switch = NULL;

static void gps_status_foobar_switch_event_cb( lv_obj_t * obj, lv_event_t event );

void gps_status_setup_setup( uint32_t tile_num ) {

    gps_status_setup_tile = mainbar_get_tile_obj( tile_num );

    lv_obj_t *header = wf_add_settings_header( gps_status_setup_tile, "gps status setup" );
    lv_obj_align( header, gps_status_setup_tile, LV_ALIGN_IN_TOP_MID, 0, 0 );

    lv_obj_t *gps_status_foobar_switch_cont = wf_add_labeled_switch( gps_status_setup_tile, "foo bar", &gps_status_foobar_switch, false, gps_status_foobar_switch_event_cb, SETUP_STYLE );
    lv_obj_align( gps_status_foobar_switch_cont, header, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_ICON_PADDING );
}

static void gps_status_foobar_switch_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): log_i( "switch value = %d\r\n", lv_switch_get_state( obj ) );
                                        break;
    }
}
