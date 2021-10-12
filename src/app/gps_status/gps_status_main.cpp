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
#include "gps_status_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/gpsctl.h"
#include "hardware/display.h"
#include "gui/mainbar/mainbar.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
#else

#endif
/*
 * tile  and style objects
 */
lv_obj_t *gps_status_main_tile = NULL;
lv_style_t gps_status_main_style;
lv_style_t gps_status_value_style;
/*
 * objects
 */
static lv_style_t style_led_green;
static lv_style_t style_led_red;
lv_obj_t *satfix_value_on = NULL;
lv_obj_t *satfix_value_off = NULL;
lv_obj_t *num_satellites_value = NULL;
lv_obj_t *satellite_type = NULL;
lv_obj_t *pos_longlat_value = NULL;
lv_obj_t *altitude_value = NULL;
lv_obj_t *speed_value = NULL;
lv_obj_t *source_value = NULL;
static bool gps_status_block_return_maintile = false;
/*
 * images
 */
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_32px);
LV_FONT_DECLARE(Ubuntu_16px);

bool style_change_event_cb( EventBits_t event, void *arg );
bool gpsctl_gps_status_event_cb( EventBits_t event, void *arg );
static void enter_gps_status_setup_event_cb(lv_obj_t *obj, lv_event_t event);
void gps_status_task(lv_task_t *task);
void gps_status_hibernate_cb(void);
void gps_status_activate_cb(void);

void gps_status_main_setup(uint32_t tile_num) {
    gps_status_main_tile = mainbar_get_tile_obj(tile_num);
    lv_style_copy(&gps_status_main_style, ws_get_mainbar_style());

    lv_obj_t * exit_btn = wf_add_exit_button( gps_status_main_tile );
    lv_obj_align(exit_btn, gps_status_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, THEME_PADDING, -THEME_PADDING);

    lv_obj_t *setup_btn = wf_add_setup_button(gps_status_main_tile, enter_gps_status_setup_event_cb );
    lv_obj_align(setup_btn, gps_status_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -THEME_PADDING, -THEME_PADDING);
    lv_obj_set_hidden(setup_btn, true);

    lv_style_copy(&gps_status_value_style, ws_get_mainbar_style());
    lv_style_set_bg_color(&gps_status_value_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_bg_opa(&gps_status_value_style, LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_border_width(&gps_status_value_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font(&gps_status_value_style, LV_STATE_DEFAULT, &Ubuntu_16px);
    /*
     * led style
     */
    lv_style_init(&style_led_green);
    lv_style_set_bg_color(&style_led_green, LV_STATE_DEFAULT, lv_color_hex(0x00d000));
    lv_style_set_border_color(&style_led_green, LV_STATE_DEFAULT, lv_color_hex(0x00d000));
    lv_style_set_shadow_color(&style_led_green, LV_STATE_DEFAULT, lv_color_hex(0x00d000));
    lv_style_set_shadow_spread(&style_led_green, LV_STATE_DEFAULT, 4);
    lv_style_init(&style_led_red);
    lv_style_set_bg_color(&style_led_red, LV_STATE_DEFAULT, lv_color_hex(0x900000));
    lv_style_set_border_color(&style_led_red, LV_STATE_DEFAULT, lv_color_hex(0x900000));
    lv_style_set_shadow_color(&style_led_red, LV_STATE_DEFAULT, lv_color_hex(0x900000));
    lv_style_set_shadow_spread(&style_led_red, LV_STATE_DEFAULT, 4);
    /*
     * num satfix
     */
    lv_obj_t *satfix_cont = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(satfix_cont, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(satfix_cont, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(satfix_cont, gps_status_main_tile, LV_ALIGN_IN_TOP_MID, 0, STATUSBAR_HEIGHT );
    lv_obj_t *satfix_label = lv_label_create(satfix_cont, NULL);
    lv_obj_add_style(satfix_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(satfix_label, "SatFix");
    lv_obj_align(satfix_label, satfix_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);
    satfix_value_on = lv_led_create(satfix_cont, NULL);
    lv_obj_add_style(satfix_value_on, LV_LED_PART_MAIN, &style_led_green);
    lv_obj_set_size(satfix_value_on, 15, 15);
    lv_obj_align(satfix_value_on, satfix_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_led_on(satfix_value_on);
    lv_obj_set_hidden(satfix_value_on, true);
    satfix_value_off = lv_led_create(satfix_cont, NULL);
    lv_obj_add_style(satfix_value_off, LV_LED_PART_MAIN, &style_led_red);
    lv_obj_set_size(satfix_value_off, 15, 15);
    lv_obj_align(satfix_value_off, satfix_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_led_on(satfix_value_off);
    lv_obj_set_hidden(satfix_value_off, false);
    /*
     * num satellites
     */
    lv_obj_t *num_satellites_cont = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(num_satellites_cont, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(num_satellites_cont, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(num_satellites_cont, satfix_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_t *num_satellites_label = lv_label_create(num_satellites_cont, NULL);
    lv_obj_add_style(num_satellites_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(num_satellites_label, "Num satellites");
    lv_obj_align(num_satellites_label, num_satellites_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);
    num_satellites_value = lv_label_create(num_satellites_cont, NULL);
    lv_obj_add_style(num_satellites_value, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(num_satellites_value, "n/a");
    lv_obj_align(num_satellites_value, num_satellites_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    /*
     * satellite type
     */
    lv_obj_t *satellite_type_cont = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(satellite_type_cont, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(satellite_type_cont, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(satellite_type_cont, num_satellites_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_t *satellite_type_label = lv_label_create(satellite_type_cont, NULL);
    lv_obj_add_style(satellite_type_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(satellite_type_label, "Sat type:");
    lv_obj_align(satellite_type_label, satellite_type_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);
    satellite_type = lv_label_create(satellite_type_cont, NULL);
    lv_obj_add_style(satellite_type, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(satellite_type, "n/a");
    lv_obj_align(satellite_type, satellite_type_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    /*
     * altitude
     */
    lv_obj_t *altitude_cont = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(altitude_cont, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(altitude_cont, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(altitude_cont, satellite_type_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_t *altitude_info_label = lv_label_create(altitude_cont, NULL);
    lv_obj_add_style(altitude_info_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(altitude_info_label, "Altitude");
    lv_obj_align(altitude_info_label, altitude_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);
    altitude_value = lv_label_create(altitude_cont, NULL);
    lv_obj_add_style(altitude_value, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(altitude_value, "n/a");
    lv_obj_align(altitude_value, altitude_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    /*
     * long lat
     */
    lv_obj_t *pos_longlat_cont = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(pos_longlat_cont, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(pos_longlat_cont, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(pos_longlat_cont, altitude_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_t *pos_longlat_label = lv_label_create(pos_longlat_cont, NULL);
    lv_obj_add_style(pos_longlat_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(pos_longlat_label, "Long/Lat");
    lv_obj_align(pos_longlat_label, pos_longlat_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);
    pos_longlat_value = lv_label_create(pos_longlat_cont, NULL);
    lv_obj_add_style(pos_longlat_value, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(pos_longlat_value, "n/a");
    lv_obj_align(pos_longlat_value, pos_longlat_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    /*
     * speed
     */
    lv_obj_t *speed_cont = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(speed_cont, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(speed_cont, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(speed_cont, pos_longlat_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_t *speed_label = lv_label_create(speed_cont, NULL);
    lv_obj_add_style(speed_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(speed_label, "Speed");
    lv_obj_align(speed_label, speed_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);
    speed_value = lv_label_create(speed_cont, NULL);
    lv_obj_add_style(speed_value, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(speed_value, "n/a");
    lv_obj_align(speed_value, speed_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    /*
     * source label
     */
    lv_obj_t *source_cont = lv_obj_create(gps_status_main_tile, NULL);
    lv_obj_set_size(source_cont, lv_disp_get_hor_res(NULL), STATUS_HEIGHT);
    lv_obj_add_style(source_cont, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_obj_align(source_cont, speed_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_obj_t *source_label = lv_label_create(source_cont, NULL);
    lv_obj_add_style(source_label, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(source_label, "Source");
    lv_obj_align(source_label, source_cont, LV_ALIGN_IN_LEFT_MID, 5, 0);
    source_value = lv_label_create(source_cont, NULL);
    lv_obj_add_style(source_value, LV_OBJ_PART_MAIN, &gps_status_value_style);
    lv_label_set_text(source_value, "n/a");
    lv_obj_align(source_value, source_cont, LV_ALIGN_IN_RIGHT_MID, -5, 0);
    /*
     * create callback
     */
    gpsctl_register_cb(     GPSCTL_FIX 
                          | GPSCTL_NOFIX
                          | GPSCTL_UPDATE_LOCATION
                          | GPSCTL_UPDATE_SATELLITE
                          | GPSCTL_UPDATE_SATELLITE_TYPE
                          | GPSCTL_UPDATE_SPEED
                          | GPSCTL_UPDATE_ALTITUDE
                          | GPSCTL_UPDATE_SOURCE
                          , gpsctl_gps_status_event_cb
                          , "gpsctl gps status" );
    /** register avtivate and hibernate callback function */
    gps_status_block_return_maintile = display_get_block_return_maintile();
    mainbar_add_tile_activate_cb( tile_num, gps_status_activate_cb );
    mainbar_add_tile_hibernate_cb( tile_num, gps_status_hibernate_cb );
    styles_register_cb( STYLE_CHANGE, style_change_event_cb, "gps status style");
}

bool style_change_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case STYLE_CHANGE:  lv_style_copy(&gps_status_value_style, ws_get_mainbar_style());
                            lv_style_set_bg_color(&gps_status_value_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
                            lv_style_set_bg_opa(&gps_status_value_style, LV_OBJ_PART_MAIN, LV_OPA_0);
                            lv_style_set_border_width(&gps_status_value_style, LV_OBJ_PART_MAIN, 0);
                            lv_style_set_text_font(&gps_status_value_style, LV_STATE_DEFAULT, &Ubuntu_16px);
                            break;
    }
    return( true );
}

static void enter_gps_status_setup_event_cb(lv_obj_t *obj, lv_event_t event) {
    
    switch (event) {
        case (LV_EVENT_CLICKED):
            statusbar_hide(true);
            mainbar_jump_to_tilenumber(gps_status_get_app_setup_tile_num(), LV_ANIM_OFF );
            break;
    }
}

bool gpsctl_gps_status_event_cb( EventBits_t event, void *arg ) {
    char temp[30] = "";
    gps_data_t *gps_data = (gps_data_t*)arg;

    switch( event ) {
        case GPSCTL_FIX:
            lv_obj_set_hidden( satfix_value_on, false );
            lv_obj_set_hidden( satfix_value_off, true );
            break;
        case GPSCTL_NOFIX:
            lv_obj_set_hidden( satfix_value_on, true );
            lv_obj_set_hidden( satfix_value_off, false );
            lv_label_set_text( pos_longlat_value, "n/a" );
            lv_label_set_text( num_satellites_value, "n/a" );
            lv_label_set_text( altitude_value, "n/a" );
            lv_label_set_text( speed_value, "n/a" );
            lv_label_set_text( source_value, "n/a" );
            break;
        case GPSCTL_UPDATE_LOCATION:
            if( gps_data->valid_location )
                snprintf( temp, sizeof( temp ), "%.4f/%.4f", gps_data->lat, gps_data->lon );
            else
                snprintf( temp, sizeof( temp ), "n/a" );
            lv_label_set_text( pos_longlat_value, temp );
            break;
        case GPSCTL_UPDATE_SATELLITE:
            if ( gps_data->valid_satellite )
                snprintf( temp, sizeof( temp ), "%d", gps_data->satellites );
            else
                snprintf( temp, sizeof( temp ), "n/a" );
            lv_label_set_text( num_satellites_value, temp );
            break;
        case GPSCTL_UPDATE_SATELLITE_TYPE:
            snprintf( temp, sizeof( temp ), "GP %d, GL %d, BD %d", gps_data->satellite_types.gps_satellites,
                                                                   gps_data->satellite_types.glonass_satellites,
                                                                   gps_data->satellite_types.baidou_satellites );
            lv_label_set_text( satellite_type, temp );
            break;
        case GPSCTL_UPDATE_SPEED:
            if ( gps_data->valid_speed )
                snprintf( temp, sizeof( temp ), "%.2fkm/h", gps_data->speed_kmh );
            else
                snprintf( temp, sizeof( temp ), "n/a" );
            lv_label_set_text( speed_value, temp );
            break;
        case GPSCTL_UPDATE_ALTITUDE:
            if ( gps_data->valid_altitude )
                snprintf( temp, sizeof( temp ), "%.1fm", gps_data->altitude_meters );
            else
                snprintf( temp, sizeof( temp ), "n/a" );
            lv_label_set_text( altitude_value, temp);
            break;
        case GPSCTL_UPDATE_SOURCE:
            lv_label_set_text( source_value, gpsctl_get_source_str( gps_data->gps_source ) );
            break;
    }

    lv_obj_align( pos_longlat_value, lv_obj_get_parent( pos_longlat_value ), LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_align( num_satellites_value, lv_obj_get_parent( num_satellites_value ), LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_align( satellite_type, lv_obj_get_parent( satellite_type ), LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_align( speed_value, lv_obj_get_parent( speed_value ), LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_align( altitude_value, lv_obj_get_parent( altitude_value ), LV_ALIGN_IN_RIGHT_MID, -5, 0);
    lv_obj_align( source_value, lv_obj_get_parent( source_value ), LV_ALIGN_IN_RIGHT_MID, -5, 0);

    return( true );
}

void gps_status_hibernate_cb(void)
{
    /** restore old "block the maintile value */
    display_set_block_return_maintile( gps_status_block_return_maintile );
}
void gps_status_activate_cb(void)
{
    /** save "block the maintile" value */
    gps_status_block_return_maintile = display_get_block_return_maintile();
    /** overwrite "block the maintile" value */
    display_set_block_return_maintile( true );
}