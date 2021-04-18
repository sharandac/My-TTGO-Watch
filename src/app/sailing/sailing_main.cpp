/****************************************************************************
 *   Apr 17 00:28:11 2021
 *   Copyright  2021  Federico Liuzzi
 *   Email: f.liuzzi02@gmail.com
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
#include <WiFi.h>
#include <AsyncUDP.h>

#include "sailing.h"
#include "sailing_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"
#include "gui/app.h"

#include "hardware/wifictl.h"

AsyncUDP udp;

struct pack {
  float heading;
  float gspeed;
  float vmg;
  float distance;
};
pack attuale;

lv_obj_t *sailing_main_tile = NULL;
lv_style_t sailing_main_style;
lv_style_t heading_main_style;

lv_obj_t * heading_label = NULL;
lv_obj_t * gspeed_label = NULL;
lv_obj_t * vmg_label = NULL;
lv_obj_t * distance_label = NULL;

lv_task_t * _sailing_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_32px);
LV_FONT_DECLARE(Ubuntu_48px);
LV_FONT_DECLARE(lv_font_montserrat_28);

void rmc(char dati[]);
void rmb(char dati[]);
void apb(char dati[]);

bool sailing_wifictl_event_cb( EventBits_t event, void *arg );

static void exit_sailing_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_sailing_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void sailing_task( lv_task_t * task );

void sailing_main_setup( uint32_t tile_num ) {

    //Init struct
    attuale.heading = 0.0;
    attuale.gspeed = 0.0;
    attuale.vmg = 0.0;
    attuale.distance = 0.0;

    sailing_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &sailing_main_style, mainbar_get_style() );
    lv_style_set_bg_color( &sailing_main_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &sailing_main_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &sailing_main_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font( &sailing_main_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_obj_add_style( sailing_main_tile, LV_OBJ_PART_MAIN, &sailing_main_style );

    // heading style
    lv_style_copy( &heading_main_style, mainbar_get_style() );
    lv_style_set_bg_color( &heading_main_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK );
    lv_style_set_bg_opa( &heading_main_style, LV_OBJ_PART_MAIN, LV_OPA_100);
    lv_style_set_border_width( &heading_main_style, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_font( &heading_main_style, LV_STATE_DEFAULT, &Ubuntu_48px);

    lv_obj_t * exit_btn = lv_imgbtn_create( sailing_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &sailing_main_style );
    lv_obj_align(exit_btn, sailing_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_sailing_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( sailing_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &sailing_main_style );
    lv_obj_align(setup_btn, sailing_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_sailing_setup_event_cb );

    lv_obj_t * heading_info_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( heading_info_label, LV_OBJ_PART_MAIN, &heading_main_style );
    lv_label_set_text( heading_info_label, "H =" );
    lv_obj_align( heading_info_label, sailing_main_tile, LV_ALIGN_IN_TOP_LEFT, 0, 20 );
    heading_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( heading_label, LV_OBJ_PART_MAIN, &heading_main_style );
    lv_label_set_text( heading_label, "0°" );
    lv_obj_align( heading_label, sailing_main_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 20 );

    lv_obj_t * gspeed_info_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( gspeed_info_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( gspeed_info_label, "Gs =" );
    lv_obj_align( gspeed_info_label, sailing_main_tile, LV_ALIGN_IN_LEFT_MID, 0, -30 );
    gspeed_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( gspeed_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( gspeed_label, "0kt" );
    lv_obj_align( gspeed_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, -30 );

    lv_obj_t * vmg_info_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( vmg_info_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( vmg_info_label, "Vmg =" );
    lv_obj_align( vmg_info_label, sailing_main_tile, LV_ALIGN_IN_LEFT_MID, 0, 10 );
    vmg_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( vmg_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( vmg_label, "0kt" );
    lv_obj_align( vmg_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, 10 );

    lv_obj_t * distance_info_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( distance_info_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( distance_info_label, "Ds =" );
    lv_obj_align( distance_info_label, sailing_main_tile, LV_ALIGN_IN_LEFT_MID, 0, 50 );
    distance_label = lv_label_create( sailing_main_tile, NULL );
    lv_obj_add_style( distance_label, LV_OBJ_PART_MAIN, &sailing_main_style );
    lv_label_set_text( distance_label, "0nm" );
    lv_obj_align( distance_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, 50 );

    // create an task that runs every secound
    _sailing_task = lv_task_create( sailing_task, 1000, LV_TASK_PRIO_MID, NULL );

    //udp listening
    wifictl_register_cb( WIFICTL_OFF | WIFICTL_CONNECT, sailing_wifictl_event_cb, "sailing data" );
    
}

bool sailing_wifictl_event_cb( EventBits_t event, void *arg ) {
    switch( event ) {
        case WIFICTL_CONNECT:       
                                if(udp.listen(1234)) {
                                        Serial.print("[I] UDP Listening on IP: ");
                                        Serial.println(WiFi.localIP());
                                        udp.onPacket([](AsyncUDPPacket packet) {
                                            char buf[packet.length()];
                                            
                                            for (int i=0;i<packet.length();i++){
                                                buf[i]= (char)*(packet.data()+i);
                                            }

                                            // Serial.println(String(buf));

                                            if(String(buf).startsWith("$ECRMB")) rmb(buf);
                                            if(String(buf).startsWith("$ECRMC")) rmc(buf);
                                            if(String(buf).startsWith("$ECAPB")) apb(buf);
                                        });
                                    }
                                    break;
    }
    return(true);
}

static void enter_sailing_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( sailing_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_sailing_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

static void sailing_main_update_label()
{
    char heading[10];
    char gspeed[10];
    char vmg[10];
    char distance[10];
    snprintf(heading, sizeof( heading ), "%.1f°", attuale.heading);
    snprintf(gspeed, sizeof( gspeed ), "%.1fkt", attuale.gspeed);
    snprintf(vmg, sizeof( vmg ), "%.1fkt", attuale.vmg);
    snprintf(distance, sizeof( distance ), "%.1fnm", attuale.distance);

    lv_label_set_text( heading_label, heading);
    lv_obj_align( heading_label, sailing_main_tile, LV_ALIGN_IN_TOP_RIGHT, 0, 20 );

    lv_label_set_text( gspeed_label, gspeed );
    lv_obj_align( gspeed_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, -30 );

    lv_label_set_text( vmg_label, vmg );
    lv_obj_align( vmg_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, 10 );

    lv_label_set_text( distance_label, distance );
    lv_obj_align( distance_label, sailing_main_tile, LV_ALIGN_IN_RIGHT_MID, 0, 50 );
}

void sailing_task( lv_task_t * task ) {
    // put your code her
    sailing_main_update_label();
}

//Makes RMC phrase understandable
void rmc(char dati[]){
  
  String info[12];
  int i,j = 0;
  
  for(i = 0; i<12; i++){
    info[i] = "";
    do{
      info[i].concat(dati[j]);
      j++;
    }while(dati[j] != ',');
    j++;
  }

  //passing RMC info to global struct
  attuale.gspeed = info[7].toFloat();
  // Serial.println(attuale.gspeed);
}

//Makes RMB phrase understandable
void rmb(char dati[]){
  
  String info[14];
  int i,j = 0;
  
  for(i = 0; i<14; i++){
    info[i] = "";
    do{
      info[i].concat(dati[j]);
      j++;
    }while(dati[j] != ',');
    j++;
  }

  //passing RMB info to global struct
  attuale.distance = info[10].toFloat();
  attuale.vmg = info[12].toFloat();
  //Serial.println("Recieved RMB");
}

//Makes APB phrase understandable
void apb(char dati[]){
  
  String info[14];
  int i,j = 0;
  
  for(i = 0; i<14; i++){
    info[i] = "";
    do{
      info[i].concat(dati[j]);
      j++;
    }while(dati[j] != ',');
    j++;
  }

  //passing APB info to global struct
  attuale.heading = info[13].toFloat();
}
