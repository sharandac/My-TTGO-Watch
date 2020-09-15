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
#include "crypto_ticker_widget.h"

#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/widget.h"

#include "hardware/json_psram_allocator.h"
#include "hardware/wifictl.h"

EventGroupHandle_t crypto_ticker_widget_event_handle = NULL;
TaskHandle_t _crypto_ticker_widget_sync_Task;
void crypto_ticker_widget_sync_Task( void * pvParameters );

crypto_ticker_widget_data_t crypto_ticker_widget_data;

// widget icon
icon_t *crypto_ticker_widget = NULL;

static void enter_crypto_ticker_widget_event_cb( lv_obj_t * obj, lv_event_t event );
bool crypto_ticker_widget_wifictl_event_cb( EventBits_t event, void *arg );

LV_IMG_DECLARE(bitcoin_64px);

void crypto_ticker_widget_setup( void ) {
    
    crypto_ticker_widget = widget_register( "BTC", &bitcoin_64px, enter_crypto_ticker_widget_event_cb );

    crypto_ticker_widget_event_handle = xEventGroupCreate();

    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT, crypto_ticker_widget_wifictl_event_cb, "crypto ticker widget" );
}

void crypto_ticker_hide_widget_icon_info( bool show ) {
    if ( !show ) {
        widget_set_indicator( crypto_ticker_widget, ICON_INDICATOR_1 );
    }
    else {
        widget_hide_indicator( crypto_ticker_widget );
    }
}

bool crypto_ticker_widget_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_CONNECT:       if ( crypto_ticker_get_config()->autosync ) {
                                        crypto_ticker_widget_sync_request();
                                    }
                                    break;

        case WIFICTL_OFF:           widget_hide_indicator( crypto_ticker_widget );
                                    break;


    }
    return( true );
}

static void enter_crypto_ticker_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( crypto_ticker_get_app_main_tile_num(), LV_ANIM_OFF );
                                        break;
    }    
}

void crypto_ticker_widget_sync_request( void ) {
    if ( xEventGroupGetBits( crypto_ticker_widget_event_handle ) & CRYPTO_TICKER_WIDGET_SYNC_REQUEST ) {
        return;
    }
    else {
        xEventGroupSetBits( crypto_ticker_widget_event_handle, CRYPTO_TICKER_WIDGET_SYNC_REQUEST );
        widget_hide_indicator( crypto_ticker_widget );
        xTaskCreate(    crypto_ticker_widget_sync_Task,       /* Function to implement the task */
                        "crypto_ticker widget sync Task",     /* Name of the task */
                        5000,                           /* Stack size in words */
                        NULL,                           /* Task input parameter */
                        1,                              /* Priority of the task */
                        &_crypto_ticker_widget_sync_Task );   /* Task handle. */
    }
}

void crypto_ticker_widget_sync_Task( void * pvParameters ) {
    log_i("start crypto_ticker widget task");

    vTaskDelay( 250 );

    if ( xEventGroupGetBits( crypto_ticker_widget_event_handle ) & CRYPTO_TICKER_WIDGET_SYNC_REQUEST ) {       
        uint32_t retval = crypto_ticker_fetch_price(crypto_ticker_get_config() , &crypto_ticker_widget_data );
        if ( retval == 200 ) {
            widget_set_indicator( crypto_ticker_widget, ICON_INDICATOR_OK );
            widget_set_label( crypto_ticker_widget, crypto_ticker_widget_data.price );
        }
        else {
            widget_set_indicator( crypto_ticker_widget, ICON_INDICATOR_FAIL );
        }
    }
    xEventGroupClearBits( crypto_ticker_widget_event_handle, CRYPTO_TICKER_WIDGET_SYNC_REQUEST );
    vTaskDelete( NULL );
}

