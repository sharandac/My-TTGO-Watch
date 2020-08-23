/****************************************************************************
 *   Aug 22 16:36:11 2020
 *   Copyright  2020  Chris McNamee
 *   Email: chris.mcna@gmail.com
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
#include <TTGO.h>

#include "crypto_ticker.h"
#include "crypto_ticker_fetch.h"
#include "crypto_ticker_main.h"
#include "crypto_ticker_setup.h"

#ifdef CRYPTO_TICKER_WIDGET

#include "crypto_ticker_widget.h"

#endif // CRYPTO_TICKER_WIDGET

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

#include "hardware/json_psram_allocator.h"




crypto_ticker_config_t crypto_ticker_config;


uint32_t crypto_ticker_main_tile_num;
uint32_t crypto_ticker_setup_tile_num;

// app icon container
lv_obj_t *crypto_ticker_icon_cont = NULL;
lv_obj_t *crypto_ticker_icon = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(bitcoin_64px);
LV_IMG_DECLARE(bitcoin_48px);
LV_IMG_DECLARE(info_1_16px);

// declare callback functions
static void enter_crypto_ticker_event_cb( lv_obj_t * obj, lv_event_t event );

void crypto_ticker_load_config( void );

// setup routine for example app
void crypto_ticker_setup( void ) {

    crypto_ticker_load_config();

    // register 2 vertical tiles and get the first tile number and save it for later use
    crypto_ticker_main_tile_num = mainbar_add_app_tile( 1, 2 );
    crypto_ticker_setup_tile_num = crypto_ticker_main_tile_num + 1;

    // create an app icon, label it and get the lv_obj_t icon container
    crypto_ticker_icon_cont = app_tile_register_app( "crypto\nticker");
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/examples/images/
    crypto_ticker_icon = lv_imgbtn_create( crypto_ticker_icon_cont, NULL );
    lv_imgbtn_set_src( crypto_ticker_icon, LV_BTN_STATE_RELEASED, &bitcoin_64px);
    lv_imgbtn_set_src( crypto_ticker_icon, LV_BTN_STATE_PRESSED, &bitcoin_64px);
    lv_imgbtn_set_src( crypto_ticker_icon, LV_BTN_STATE_CHECKED_RELEASED, &bitcoin_64px);
    lv_imgbtn_set_src( crypto_ticker_icon, LV_BTN_STATE_CHECKED_PRESSED, &bitcoin_64px);
    lv_obj_reset_style_list( crypto_ticker_icon, LV_OBJ_PART_MAIN );
    lv_obj_align( crypto_ticker_icon , crypto_ticker_icon_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( crypto_ticker_icon, enter_crypto_ticker_event_cb );

    // make app icon drag scroll the mainbar
    mainbar_add_slide_element(crypto_ticker_icon);

    // init main and setup tile, see crypto_ticker_main.cpp and crypto_ticker_setup.cpp
    crypto_ticker_main_setup( crypto_ticker_main_tile_num );
    crypto_ticker_setup_setup( crypto_ticker_setup_tile_num );


    #ifdef CRYPTO_TICKER_WIDGET

    crypto_ticker_widget_setup();

    #endif // CRYPTO_TICKER_WIDGET
}

uint32_t crypto_ticker_get_app_main_tile_num( void ) {
    return( crypto_ticker_main_tile_num );
}

uint32_t crypto_ticker_get_app_setup_tile_num( void ) {
    return( crypto_ticker_setup_tile_num );
}

void crypto_ticker_jump_to_main( void ) {
    statusbar_hide( true );
    mainbar_jump_to_tilenumber( crypto_ticker_main_tile_num, LV_ANIM_ON );
}

void crypto_ticker_jump_to_setup( void ) {
    statusbar_hide( true );
    mainbar_jump_to_tilenumber( crypto_ticker_setup_tile_num, LV_ANIM_ON );    
}

/*
 *
 */
static void enter_crypto_ticker_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       crypto_ticker_jump_to_main();
                                        break;
    }    
}



crypto_ticker_config_t *crypto_ticker_get_config( void ) {
    return( &crypto_ticker_config );
}

/*
 *
 */
void crypto_ticker_save_config( void ) {
  

    fs::File file = SPIFFS.open( crypto_ticker_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", crypto_ticker_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["symbol"] = crypto_ticker_config.symbol;
        doc["autosync"] = crypto_ticker_config.autosync;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        }
        doc.clear();
    }
    file.close();
}

/*
 *
 */
void crypto_ticker_load_config( void ) {
    if ( SPIFFS.exists( crypto_ticker_JSON_CONFIG_FILE ) ) {        
        fs::File file = SPIFFS.open( crypto_ticker_JSON_CONFIG_FILE, FILE_READ );
        if (!file) {
            log_e("Can't open file: %s!", crypto_ticker_JSON_CONFIG_FILE );
        }
        else {
            int filesize = file.size();
            SpiRamJsonDocument doc( filesize * 2 );

            DeserializationError error = deserializeJson( doc, file );
            if ( error ) {
                log_e("update check deserializeJson() failed: %s", error.c_str() );
            }
            else {
                strlcpy( crypto_ticker_config.symbol, doc["symbol"], sizeof( crypto_ticker_config.symbol ) );
                crypto_ticker_config.autosync = doc["autosync"].as<bool>();
            }        
            doc.clear();
        }
        file.close();
    }

}