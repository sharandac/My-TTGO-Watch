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
#include "config.h"
#include "notify_settings.h"

#include "gui/mainbar/mainbar.h"
#include "gui/mainbar/setup_tile/setup_tile.h"
#include "gui/statusbar.h"
#include "gui/setup.h"
#include "gui/widget_factory.h"
#include "gui/widget_styles.h"

#include "hardware/blectl.h"

#ifdef NATIVE_64BIT
    #include "utils/logging.h"
    #include "utils/millis.h"
#else

#endif

icon_t *notify_setup_icon = NULL;

lv_obj_t *notify_settings_tile=NULL;
uint32_t notify_settings_tile_num;

lv_obj_t *notify_wakeup_on_notification_switch = NULL;
lv_obj_t *notify_show_notification_switch = NULL;
lv_obj_t *notify_vibe_notification_switch = NULL;
lv_obj_t *notify_sound_notification_switch = NULL;
lv_obj_t *notify_media_notification_switch = NULL;

LV_IMG_DECLARE(notification_64px);

static void notify_wakeup_switch_event_handler( lv_obj_t * obj, lv_event_t event );
static void notify_show_switch_event_handler( lv_obj_t * obj, lv_event_t event );
static void notify_vibe_switch_event_handler( lv_obj_t * obj, lv_event_t event );
static void notify_sound_switch_event_handler( lv_obj_t * obj, lv_event_t event );
static void notify_media_switch_event_handler( lv_obj_t * obj, lv_event_t event );
static void notify_enter_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void notify_exit_setup_event_cb( lv_obj_t * obj, lv_event_t event );

void notify_settings_tile_setup( void ) {
    // get an app tile and copy mainstyle
    notify_settings_tile_num = mainbar_add_setup_tile( 1, 1, "battery setup" );
    notify_settings_tile = mainbar_get_tile_obj( notify_settings_tile_num );

    notify_setup_icon = setup_register( "notify\nsettings", &notification_64px, notify_enter_setup_event_cb );
    setup_hide_indicator( notify_setup_icon );

    lv_obj_t *notify_header = wf_add_settings_header( notify_settings_tile, "notifications settings", notify_exit_setup_event_cb );
    lv_obj_align( notify_header, notify_settings_tile, LV_ALIGN_IN_TOP_LEFT, THEME_PADDING, STATUSBAR_HEIGHT + THEME_PADDING );

    lv_obj_t *notify_wakeup_on_notification_cont = wf_add_labeled_switch( notify_settings_tile, "wakeup", &notify_wakeup_on_notification_switch, blectl_get_wakeup_on_notification(), notify_wakeup_switch_event_handler, SETUP_STYLE );
    lv_obj_align( notify_wakeup_on_notification_cont, notify_header, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    lv_obj_t *notify_show_notification_cont = wf_add_labeled_switch( notify_settings_tile, "show", &notify_show_notification_switch, blectl_get_show_notification(), notify_show_switch_event_handler, SETUP_STYLE );
    lv_obj_align( notify_show_notification_cont, notify_wakeup_on_notification_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    lv_obj_t *notify_vibe_notification_cont = wf_add_labeled_switch( notify_settings_tile, "vibe", &notify_vibe_notification_switch, blectl_get_vibe_notification(), notify_vibe_switch_event_handler, SETUP_STYLE );
    lv_obj_align( notify_vibe_notification_cont, notify_show_notification_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    lv_obj_t *notify_sound_notification_cont = wf_add_labeled_switch( notify_settings_tile, "sound", &notify_sound_notification_switch, blectl_get_sound_notification(), notify_sound_switch_event_handler, SETUP_STYLE );
    lv_obj_align( notify_sound_notification_cont, notify_vibe_notification_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );

    lv_obj_t *notify_media_notification_cont = wf_add_labeled_switch( notify_settings_tile, "media", &notify_media_notification_switch, blectl_get_media_notification(), notify_media_switch_event_handler, SETUP_STYLE );
    lv_obj_align( notify_media_notification_cont, notify_sound_notification_cont, LV_ALIGN_OUT_BOTTOM_MID, 0, THEME_PADDING );
}

static void notify_wakeup_switch_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): blectl_set_wakeup_on_notification( lv_switch_get_state( obj ) );
                                        break;
    }
}


static void notify_show_switch_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): blectl_set_show_notification( lv_switch_get_state( obj ) );
                                        break;
    }
}

static void notify_vibe_switch_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): blectl_set_vibe_notification( lv_switch_get_state( obj ) );
                                        break;
    }
}

static void notify_sound_switch_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): blectl_set_sound_notification( lv_switch_get_state( obj ) );
                                        break;
    }
}

static void notify_media_switch_event_handler( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_VALUE_CHANGED ): blectl_set_media_notification( lv_switch_get_state( obj ) );
                                        break;
    }
}

static void notify_enter_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( notify_settings_tile_num, LV_ANIM_OFF );
                                        break;
    }

}

static void notify_exit_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_back();
                                        break;
    }
}
