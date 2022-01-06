/****************************************************************************
 *   linuxthor 2020
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
#include <lwip/sockets.h>
#include <ESP32Ping.h>

#include "ping_app.h"
#include "ping_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/keyboard.h"
#include "gui/widget_styles.h"

lv_obj_t * ping_result_cont = NULL;
lv_obj_t *ping_app_main_tile = NULL;
lv_obj_t *ping_ip_textfield = NULL;
lv_style_t ping_app_main_style;

lv_task_t * _ping_app_task;

LV_IMG_DECLARE(next_32px);
LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(refresh_32px);
LV_IMG_DECLARE(ping_app_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_ping_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void ping_textarea_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_ping_app_next_event_cb( lv_obj_t * obj, lv_event_t event );
void ping_app_task( lv_task_t * task );

int getIpBlock(int index, String str) {
    char separator = '.';
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = str.length()-1;
  
    for(int i=0; i<=maxIndex && found<=index; i++){
      if(str.charAt(i)==separator || i==maxIndex){
          found++;
          strIndex[0] = strIndex[1]+1;
          strIndex[1] = (i == maxIndex) ? i+1 : i;
      }
    }
    
    return found>index ? str.substring(strIndex[0], strIndex[1]).toInt() : 0;
}

IPAddress str2IP(String str) {
    IPAddress ret( getIpBlock(0,str),getIpBlock(1,str),getIpBlock(2,str),getIpBlock(3,str) );
    return ret;
}

void ping_app_main_scanner_try( void )
{
    static char outstr[15];
    lv_obj_t * label;
 

   if( ping_result_cont != NULL )
    {
       lv_obj_del( ping_result_cont ); 
       ping_result_cont = NULL;           
    }        
    ping_result_cont = lv_cont_create( ping_app_main_tile, NULL);
    lv_obj_set_auto_realign(ping_result_cont, true);                    
    lv_obj_align_origo(ping_result_cont, NULL, LV_ALIGN_CENTER, 0, 0);  
    lv_cont_set_fit(ping_result_cont, LV_FIT_TIGHT);
    lv_cont_set_layout(ping_result_cont, LV_LAYOUT_COLUMN_MID);
    Ping.ping(str2IP(lv_textarea_get_text(ping_ip_textfield)));
    float avg_time_ms = Ping.averageTime();
    if(avg_time_ms != 0)
    {
        dtostrf(avg_time_ms,7, 2, outstr);
        label = lv_label_create(ping_result_cont, NULL);
        lv_label_set_text_fmt(label, "Send 5 pkts\nAvg: %s ms", outstr);
    }
    else
    {
        label = lv_label_create(ping_result_cont, NULL);
        lv_label_set_text(label, "Send ???\nAvg: ??? ms");
    }  
}

void ping_app_main_setup( uint32_t tile_num ) {

    ping_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &ping_app_main_style, ws_get_mainbar_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( ping_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &ping_app_main_style );
    lv_obj_align(exit_btn, ping_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_ping_app_main_event_cb );

    lv_obj_t * next_btn = lv_imgbtn_create( ping_app_main_tile, NULL);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_RELEASED, &ping_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_PRESSED, &ping_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_CHECKED_RELEASED, &ping_app_32px);
    lv_imgbtn_set_src(next_btn, LV_BTN_STATE_CHECKED_PRESSED, &ping_app_32px);
    lv_obj_add_style(next_btn, LV_IMGBTN_PART_MAIN, &ping_app_main_style );
    lv_obj_align(next_btn, ping_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, (LV_HOR_RES / 2) -15 , -10 );
    lv_obj_set_event_cb( next_btn, enter_ping_app_next_event_cb );

    // text entry 
    lv_obj_t *ping_ip_cont = lv_obj_create( ping_app_main_tile, NULL );
    lv_obj_set_size(ping_ip_cont, lv_disp_get_hor_res( NULL ) , 40);
    lv_obj_align( ping_ip_cont, ping_app_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 20 );
    lv_obj_t *ping_ip_label = lv_label_create( ping_ip_cont, NULL);
    lv_label_set_text( ping_ip_label, "IP");
    lv_obj_align( ping_ip_label, ping_ip_cont, LV_ALIGN_IN_LEFT_MID, 5, 0 );
    ping_ip_textfield = lv_textarea_create( ping_ip_cont, NULL);
    lv_textarea_set_text( ping_ip_textfield, "127.0.0.1" );
    lv_textarea_set_accepted_chars(ping_ip_textfield, "0123456789.");
    lv_textarea_set_pwd_mode( ping_ip_textfield, false);
    lv_textarea_set_one_line( ping_ip_textfield, true);
    lv_textarea_set_cursor_hidden( ping_ip_textfield, true);
    lv_obj_set_width( ping_ip_textfield, LV_HOR_RES /4 * 2 );
    lv_obj_align( ping_ip_textfield, ping_ip_cont, LV_ALIGN_IN_LEFT_MID, 55, 0 );
    lv_obj_set_event_cb( ping_ip_textfield, ping_textarea_event_cb );
}

static void ping_textarea_event_cb( lv_obj_t * obj, lv_event_t event ) {
    if( event == LV_EVENT_CLICKED ) {
        num_keyboard_set_textarea( obj );
    }
}

static void enter_ping_app_next_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):      ping_app_main_scanner_try();
                                       break;
    }
}

static void exit_ping_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       if( ping_result_cont != NULL )
                                        {
                                            lv_obj_del( ping_result_cont ); 
                                            ping_result_cont = NULL;           
                                        }        
                                        mainbar_jump_back();
                                        break;
    }
}

