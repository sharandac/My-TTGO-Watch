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

#ifdef CRYPTO_TICKER_WIDGET

#include "crypto_ticker_widget.h"

#endif // CRYPTO_TICKER_WIDGET

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

#include "hardware/wifictl.h"

EventGroupHandle_t crypto_ticker_main_event_handle = NULL;
TaskHandle_t _crypto_ticker_main_sync_Task;

lv_obj_t *crypto_ticker_main_tile = NULL;
lv_style_t crypto_ticker_main_style;

lv_obj_t *crypto_ticker_main_update_label = NULL;
lv_obj_t *crypto_ticker_main_last_price_value_label = NULL;
lv_obj_t *crypto_ticker_main_price_change_value_label = NULL;
lv_obj_t *crypto_ticker_main_volume_value_label = NULL;

crypto_ticker_main_data_t crypto_ticker_main_data;


void crypto_ticker_main_sync_Task( void * pvParameters );
bool crypto_ticker_main_wifictl_event_cb( EventBits_t event, void *arg );

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_crypto_ticker_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_crypto_ticker_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void refresh_crypto_ticker_main_event_cb( lv_obj_t * obj, lv_event_t event );

void crypto_ticker_main_setup( uint32_t tile_num ) {

    crypto_ticker_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &crypto_ticker_main_style, mainbar_get_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( crypto_ticker_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &crypto_ticker_main_style );
    lv_obj_align(exit_btn, crypto_ticker_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_crypto_ticker_main_event_cb );

    lv_obj_t * reload_btn = lv_imgbtn_create( crypto_ticker_main_tile, NULL);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_PRESSED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_PRESSED, &refresh_32px);
    lv_obj_add_style(reload_btn, LV_IMGBTN_PART_MAIN, &crypto_ticker_main_style );
    lv_obj_align(reload_btn, crypto_ticker_main_tile, LV_ALIGN_IN_TOP_RIGHT, -10 , 10 );
    lv_obj_set_event_cb( reload_btn, refresh_crypto_ticker_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( crypto_ticker_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &crypto_ticker_main_style );
    lv_obj_align(setup_btn, crypto_ticker_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_crypto_ticker_setup_event_cb );

    crypto_ticker_main_update_label = lv_label_create( crypto_ticker_main_tile , NULL);
    lv_label_set_text( crypto_ticker_main_update_label, "");
    lv_obj_reset_style_list( crypto_ticker_main_update_label, LV_OBJ_PART_MAIN );
    lv_obj_align( crypto_ticker_main_update_label, crypto_ticker_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 0 );


    lv_obj_t *crypto_ticker_main_last_price_cont = lv_obj_create( crypto_ticker_main_tile, NULL );
    lv_obj_set_size(crypto_ticker_main_last_price_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( crypto_ticker_main_last_price_cont, LV_OBJ_PART_MAIN, &crypto_ticker_main_style  );
    lv_obj_align( crypto_ticker_main_last_price_cont, crypto_ticker_main_update_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20 );
    lv_obj_t *crypto_ticker_main_last_price_label = lv_label_create( crypto_ticker_main_last_price_cont, NULL);
    lv_obj_add_style( crypto_ticker_main_last_price_label, LV_OBJ_PART_MAIN, &crypto_ticker_main_style  );
    lv_label_set_text( crypto_ticker_main_last_price_label, "Last Price");
    lv_obj_align( crypto_ticker_main_last_price_label, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    crypto_ticker_main_last_price_value_label = lv_label_create( crypto_ticker_main_last_price_cont , NULL);
    lv_label_set_text( crypto_ticker_main_last_price_value_label, "");
    lv_obj_reset_style_list( crypto_ticker_main_last_price_value_label, LV_OBJ_PART_MAIN );
    lv_obj_set_width( crypto_ticker_main_last_price_value_label, LV_HOR_RES /4 * 2 );
    lv_obj_align( crypto_ticker_main_last_price_value_label, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *crypto_ticker_main_price_change_cont = lv_obj_create( crypto_ticker_main_tile, NULL );
    lv_obj_set_size(crypto_ticker_main_price_change_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( crypto_ticker_main_price_change_cont, LV_OBJ_PART_MAIN, &crypto_ticker_main_style  );
    lv_obj_align( crypto_ticker_main_price_change_cont, crypto_ticker_main_last_price_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5 );
    lv_obj_t *crypto_ticker_main_price_change_label = lv_label_create( crypto_ticker_main_price_change_cont, NULL);
    lv_obj_add_style( crypto_ticker_main_price_change_label, LV_OBJ_PART_MAIN, &crypto_ticker_main_style  );
    lv_label_set_text( crypto_ticker_main_price_change_label, "Price Change");
    lv_obj_align( crypto_ticker_main_price_change_label, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    crypto_ticker_main_price_change_value_label = lv_label_create( crypto_ticker_main_price_change_cont , NULL);
    lv_label_set_text( crypto_ticker_main_price_change_value_label, "");
    lv_obj_reset_style_list( crypto_ticker_main_price_change_value_label, LV_OBJ_PART_MAIN );
    lv_obj_set_width( crypto_ticker_main_price_change_value_label, LV_HOR_RES /4 * 2 );
    lv_obj_align( crypto_ticker_main_price_change_value_label, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

    lv_obj_t *crypto_ticker_main_volume_cont = lv_obj_create( crypto_ticker_main_tile, NULL );
    lv_obj_set_size(crypto_ticker_main_volume_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_add_style( crypto_ticker_main_volume_cont, LV_OBJ_PART_MAIN, &crypto_ticker_main_style  );
    lv_obj_align( crypto_ticker_main_volume_cont, crypto_ticker_main_price_change_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5 );
    lv_obj_t *crypto_ticker_main_volume_label = lv_label_create( crypto_ticker_main_volume_cont, NULL);
    lv_obj_add_style( crypto_ticker_main_volume_label, LV_OBJ_PART_MAIN, &crypto_ticker_main_style  );
    lv_label_set_text( crypto_ticker_main_volume_label, "Volume");
    lv_obj_align( crypto_ticker_main_volume_label, NULL, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    crypto_ticker_main_volume_value_label = lv_label_create( crypto_ticker_main_volume_cont , NULL);
    lv_label_set_text( crypto_ticker_main_volume_value_label, "");
    lv_obj_reset_style_list( crypto_ticker_main_volume_value_label, LV_OBJ_PART_MAIN );
    lv_obj_set_width( crypto_ticker_main_volume_value_label, lv_disp_get_hor_res( NULL ) /4 * 2 );
    lv_obj_align( crypto_ticker_main_volume_value_label, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 0 );


    crypto_ticker_main_event_handle = xEventGroupCreate();

    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT, crypto_ticker_main_wifictl_event_cb, "crypto ticker main" );
}

bool crypto_ticker_main_wifictl_event_cb( EventBits_t event, void *arg ) {    
    switch( event ) {
        case WIFICTL_CONNECT:       crypto_ticker_config_t *crypto_ticker_config = crypto_ticker_get_config();
                                    if ( crypto_ticker_config->autosync ) {
                                        crypto_ticker_main_sync_request();
                                    }
                                    break;
    }
    return( true );
}

static void enter_crypto_ticker_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       crypto_ticker_jump_to_setup();
                                        break;
    }
}

static void exit_crypto_ticker_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}


static void refresh_crypto_ticker_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       crypto_ticker_main_sync_request();

#ifdef CRYPTO_TICKER_WIDGET

                                        crypto_ticker_widget_sync_request();

#endif // CRYPTO_TICKER_WIDGET
                                        break;
    }
}


void crypto_ticker_main_sync_request( void ) {
    if ( xEventGroupGetBits( crypto_ticker_main_event_handle ) & CRYPTO_TICKER_MAIN_SYNC_REQUEST ) {
        return;
    }
    else {
        xEventGroupSetBits( crypto_ticker_main_event_handle, CRYPTO_TICKER_MAIN_SYNC_REQUEST );
        xTaskCreate(    crypto_ticker_main_sync_Task,      /* Function to implement the task */
                        "crypto ticker main sync Task",    /* Name of the task */
                        5000,                            /* Stack size in words */
                        NULL,                            /* Task input parameter */
                        1,                               /* Priority of the task */
                        &_crypto_ticker_main_sync_Task );  /* Task handle. */ 
    }
}

void crypto_ticker_main_sync_Task( void * pvParameters ) {
    crypto_ticker_config_t *crypto_ticker_config = crypto_ticker_get_config();
    int32_t retval = -1;

    log_i("start crypto ticker main task, heap: %d", ESP.getFreeHeap() );

    vTaskDelay( 250 );

    if ( xEventGroupGetBits( crypto_ticker_main_event_handle ) & CRYPTO_TICKER_MAIN_SYNC_REQUEST ) {   
        if ( crypto_ticker_config->autosync ) {
            retval = crypto_ticker_fetch_statistics( crypto_ticker_config , &crypto_ticker_main_data );
            if ( retval == 200 ) {
                time_t now;
                struct tm info;
                char buf[64];


                lv_label_set_text( crypto_ticker_main_last_price_value_label, crypto_ticker_main_data.lastPrice );
                lv_obj_align( crypto_ticker_main_last_price_value_label, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

                lv_label_set_text( crypto_ticker_main_price_change_value_label, crypto_ticker_main_data.priceChangePercent );
                lv_obj_align( crypto_ticker_main_price_change_value_label, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 0 );

                lv_label_set_text( crypto_ticker_main_volume_value_label, crypto_ticker_main_data.volume );
                lv_obj_align( crypto_ticker_main_volume_value_label, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 0 );
               

                time( &now );
                localtime_r( &now, &info );
                strftime( buf, sizeof(buf), "updated: %d.%b %H:%M", &info );
                lv_label_set_text( crypto_ticker_main_update_label, buf );
                lv_obj_invalidate( lv_scr_act() );
            }
        }
    }
    xEventGroupClearBits( crypto_ticker_main_event_handle, CRYPTO_TICKER_MAIN_SYNC_REQUEST );
    log_i("finish crypto ticker main task, heap: %d", ESP.getFreeHeap() );
    vTaskDelete( NULL );
}