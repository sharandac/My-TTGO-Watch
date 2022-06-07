/****************************************************************************
 *   Aug 3 12:17:11 2020
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
#include "mail_app.h"
#include "mail_app_main.h"
#include "mail_app_setup.h"
#include "config/mail_config.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

uint32_t mail_app_main_tile_num = 0;
uint32_t mail_app_setup_tile_num = 0;
/*
 * app icon
 */
icon_t *mail_app = NULL;
/*
 * declare you images or fonts you need
 */
LV_IMG_DECLARE(mail_64px);
/*
 * declare callback functions
 */
static void enter_mail_app_event_cb( lv_obj_t * obj, lv_event_t event );
/*
 *
 */
mail_config_t mail_config;
/*
 * setup routine for example app
 */
void mail_app_setup( void ) {
    #if defined( ONLY_ESSENTIAL )
        return;
    #endif

    #ifndef NO_BLUETOOTH
        return;
    #endif

    mail_app_main_tile_num = mainbar_add_app_tile( 1, 1, "mail" );
    mail_app_setup_tile_num = mainbar_add_setup_tile( 1, 1, "mail setup" );

    mail_app = app_register( "mail\n(alpha)", &mail_64px, enter_mail_app_event_cb );

    mail_config.load();

    mail_app_main_setup( mail_app_main_tile_num );
    mail_app_setup( mail_app_setup_tile_num );
}

uint32_t mail_app_get_app_main_tile_num( void ) {
    return( mail_app_main_tile_num );
}

uint32_t mail_app_get_app_setup_tile_num( void ) {
    return( mail_app_setup_tile_num );
}

static void enter_mail_app_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_tilenumber( mail_app_main_tile_num, LV_ANIM_OFF, true );
                                        break;
    }    
}

mail_config_t *mail_app_get_config( void ) {
    return( &mail_config );
}