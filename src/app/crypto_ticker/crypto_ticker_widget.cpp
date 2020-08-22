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

#include "hardware/json_psram_allocator.h"
#include "hardware/wifictl.h"

EventGroupHandle_t crypto_ticker_widget_event_handle = NULL;
TaskHandle_t _crypto_ticker_widget_sync_Task;
void crypto_ticker_widget_sync_Task( void * pvParameters );

crypto_ticker_widget_data_t crypto_ticker_widget_data;

// widget icon container
lv_obj_t *crypto_ticker_widget_cont = NULL;
lv_obj_t *crypto_ticker_widget_icon = NULL;
lv_obj_t *crypto_ticker_widget_icon_info = NULL;
lv_obj_t *crypto_ticker_widget_label = NULL;

static void enter_crypto_ticker_widget_event_cb( lv_obj_t * obj, lv_event_t event );
void crypto_ticker_widget_wifictl_event_cb( EventBits_t event, char* msg );

// declare you images or fonts you need
LV_IMG_DECLARE(info_ok_16px);
LV_IMG_DECLARE(info_fail_16px);

LV_IMG_DECLARE(bitcoin_64px);
LV_IMG_DECLARE(bitcoin_48px);



void crypto_ticker_widget_setup( void ) {
    

    // get an widget container from main_tile
    // remember, an widget icon must have an size of 64x64 pixel
    // total size of the container is 64x80 pixel, the bottom 16 pixel is for your label
    crypto_ticker_widget_cont = main_tile_register_widget();
    crypto_ticker_widget_icon = lv_imgbtn_create( crypto_ticker_widget_cont, NULL );
    lv_imgbtn_set_src( crypto_ticker_widget_icon, LV_BTN_STATE_RELEASED, &bitcoin_48px);
    lv_imgbtn_set_src( crypto_ticker_widget_icon, LV_BTN_STATE_PRESSED, &bitcoin_48px);
    lv_imgbtn_set_src( crypto_ticker_widget_icon, LV_BTN_STATE_CHECKED_RELEASED, &bitcoin_48px);
    lv_imgbtn_set_src( crypto_ticker_widget_icon, LV_BTN_STATE_CHECKED_PRESSED, &bitcoin_48px);
    lv_obj_reset_style_list( crypto_ticker_widget_icon, LV_OBJ_PART_MAIN );
    lv_obj_align( crypto_ticker_widget_icon , crypto_ticker_widget_cont, LV_ALIGN_IN_TOP_LEFT, 0, 0 );
    lv_obj_set_event_cb( crypto_ticker_widget_icon, enter_crypto_ticker_widget_event_cb );

    // make widget icon drag scroll the mainbar
    mainbar_add_slide_element(crypto_ticker_widget_icon);

    // set an small info icon at your widget icon to inform the user about the state or news
    crypto_ticker_widget_icon_info = lv_img_create( crypto_ticker_widget_cont, NULL );
    lv_img_set_src( crypto_ticker_widget_icon_info, &info_ok_16px );
    lv_obj_align( crypto_ticker_widget_icon_info, crypto_ticker_widget_cont, LV_ALIGN_IN_TOP_RIGHT, 0, 0 );
    lv_obj_set_hidden( crypto_ticker_widget_icon_info, true );

    // label your widget
    crypto_ticker_widget_label = lv_label_create( crypto_ticker_widget_cont , NULL);
    lv_label_set_text( crypto_ticker_widget_label, "BTC");
    lv_obj_reset_style_list( crypto_ticker_widget_label, LV_OBJ_PART_MAIN );
    lv_obj_align( crypto_ticker_widget_label, crypto_ticker_widget_cont, LV_ALIGN_IN_BOTTOM_MID, 0, 0);


    crypto_ticker_widget_event_handle = xEventGroupCreate();

    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT, crypto_ticker_widget_wifictl_event_cb );

}





/*
 *
 */
void crypto_ticker_hide_widget_icon_info( bool show ) {
    if ( crypto_ticker_widget_icon_info == NULL )
        return;

    lv_obj_set_hidden( crypto_ticker_widget_icon_info, show );
    lv_obj_invalidate( lv_scr_act() );
}




void crypto_ticker_widget_wifictl_event_cb( EventBits_t event, char* msg ) {
    log_i("crypto_ticker widget wifictl event: %04x", event );

    switch( event ) {
        case WIFICTL_CONNECT:       if ( crypto_ticker_get_config()->autosync ) {
                                        crypto_ticker_widget_sync_request();
                                    }
                                    break;

        case WIFICTL_OFF:           lv_obj_set_hidden( crypto_ticker_widget_icon_info, true );
                                    break;


    }
}


static void enter_crypto_ticker_widget_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( crypto_ticker_get_app_main_tile_num(), LV_ANIM_OFF );
                                        break;
    }    
}



void crypto_ticker_widget_sync_request( void ) {
    if ( xEventGroupGetBits( crypto_ticker_widget_event_handle ) & crypto_ticker_widget_SYNC_REQUEST ) {
        return;
    }
    else {
        xEventGroupSetBits( crypto_ticker_widget_event_handle, crypto_ticker_widget_SYNC_REQUEST );
        lv_obj_set_hidden( crypto_ticker_widget_icon_info, true );
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

    if ( xEventGroupGetBits( crypto_ticker_widget_event_handle ) & crypto_ticker_widget_SYNC_REQUEST ) {       
        uint32_t retval = crypto_ticker_fetch_today(crypto_ticker_get_config() , &crypto_ticker_widget_data );
        if ( retval == 200 ) {
           
            lv_img_set_src( crypto_ticker_widget_icon_info, &info_ok_16px );
            lv_obj_set_hidden( crypto_ticker_widget_icon_info, false );
        }
        else {
            lv_img_set_src( crypto_ticker_widget_icon_info, &info_fail_16px );
            lv_obj_set_hidden( crypto_ticker_widget_icon_info, false );
        }
        lv_obj_invalidate( lv_scr_act() );
    }
    xEventGroupClearBits( crypto_ticker_widget_event_handle, crypto_ticker_widget_SYNC_REQUEST );
    vTaskDelete( NULL );
}

