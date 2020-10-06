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
#include <TTGO.h>

#include "image_ticker.h"
#include "image_ticker_main.h"
#include "image_ticker_setup.h"

#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"
#include "gui/widget.h"

#include "image_ticker_fetch.h"

#include "hardware/json_psram_allocator.h"

image_ticker_config_t image_ticker_config;

uint32_t image_ticker_main_tile_num;
uint32_t image_ticker_setup_tile_num;

// app icon
icon_t *image_ticker = NULL;

// widget icon
icon_t *image_ticker_widget = NULL;

// declare you images or fonts you need
LV_IMG_DECLARE(image_ticker_64px);
LV_IMG_DECLARE(info_1_16px);

// declare callback functions for the app and widget icon to enter the app
static void enter_image_ticker_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_image_ticker_widget_event_cb( lv_obj_t * obj, lv_event_t event );

/*
 * setup routine for image_ticker app
 */
void image_ticker_setup( void ) {
    // register 2 vertical tiles and get the first tile number and save it for later use
    image_ticker_main_tile_num = mainbar_add_app_tile( 1, 2, "Image Ticker" );
    image_ticker_setup_tile_num = image_ticker_main_tile_num + 1;

    // register app icon on the app tile
    // set your own icon and register her callback to activate by an click
    // remember, an app icon must have an size of 64x64 pixel with an alpha channel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/image_ticker/images/ and declare it like LV_IMG_DECLARE( your_icon );
    image_ticker = app_register( "ImgTick", &image_ticker_64px, enter_image_ticker_event_cb );
    app_set_indicator( image_ticker, ICON_INDICATOR_OK );

#ifdef IMAGE_TICKER_WIDGET
    // register widget icon on the main tile
    // set your own icon and register her callback to activate by an click
    // remember, an widget icon must have an max size of 64x64 pixel
    // use https://lvgl.io/tools/imageconverter to convert your images and set "true color with alpha" to get fancy images
    // the resulting c-file can put in /app/image_ticker/images/ and declare it like LV_IMG_DECLARE( your_icon );
    image_ticker_widget = widget_register( "ImgTick", &image_ticker_64px, enter_image_ticker_widget_event_cb );
    widget_set_indicator( image_ticker_widget, ICON_INDICATOR_UPDATE );
#endif // IMAGE_TICKER_WIDGET

    // init main and setup tile, see image_ticker_main.cpp and image_ticker_setup.cpp
    image_ticker_main_setup( image_ticker_main_tile_num );
    image_ticker_setup_setup( image_ticker_setup_tile_num );
}

/*
 *
 */
uint32_t image_ticker_get_app_main_tile_num( void ) {
    return( image_ticker_main_tile_num );
}

/*
 *
 */
uint32_t image_ticker_get_app_setup_tile_num( void ) {
    return( image_ticker_setup_tile_num );
}

/*
 *
 */
static void enter_image_ticker_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        app_hide_indicator( image_ticker );
                                        image_ticker_load_config();
                                        image_ticker_fetch_jpg();
                                        mainbar_jump_to_tilenumber( image_ticker_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

/*
 *
 */
static void enter_image_ticker_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        widget_hide_indicator( image_ticker_widget );
                                        mainbar_jump_to_tilenumber( image_ticker_main_tile_num, LV_ANIM_OFF );
                                        break;
    }    
}

image_ticker_config_t *image_ticker_get_config( void ) {
    return( &image_ticker_config );
}

/*
 *
 */
void image_ticker_save_config( void ) {

    fs::File file = SPIFFS.open( image_ticker_JSON_CONFIG_FILE, FILE_WRITE );

    if (!file) {
        log_e("Can't open file: %s!", image_ticker_JSON_CONFIG_FILE );
    }
    else {
        SpiRamJsonDocument doc( 1000 );

        doc["url"] = image_ticker_config.url;
        doc["autosync"] = image_ticker_config.autosync;

        if ( serializeJsonPretty( doc, file ) == 0) {
            log_e("Failed to write config file");
        } else {
            log_e("Config file written");
        }
        doc.clear();
    }
    file.close();
}

/*
 *
 */
void image_ticker_load_config( void ) {
    if ( SPIFFS.exists( image_ticker_JSON_CONFIG_FILE ) ) {        
        fs::File file = SPIFFS.open( image_ticker_JSON_CONFIG_FILE, FILE_READ );
        if (!file) {
            log_e("Can't open file: %s!", image_ticker_JSON_CONFIG_FILE );
        }
        else {
            int filesize = file.size();
            SpiRamJsonDocument doc( filesize * 2 );

            DeserializationError error = deserializeJson( doc, file );
            if ( error ) {
                log_e("update check deserializeJson() failed: %s", error.c_str() );
            }
            else {
                strlcpy( image_ticker_config.url, doc["url"] | DEFAULT_URL, sizeof( image_ticker_config.url ) );
                image_ticker_config.autosync = doc["autosync"].as<bool>();
            }        
            doc.clear();
        }
        file.close();
    }

}