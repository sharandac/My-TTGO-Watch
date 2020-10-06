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

#include "prayer_app.h"
#include "prayer_app_main.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"

lv_obj_t *prayer_app_main_tile = NULL;
lv_style_t prayer_app_main_style;
lv_style_t prayer_page_main_style;
lv_style_t prayer_text_main_style;

lv_task_t * _prayer_app_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_32px);

static void exit_prayer_app_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_prayer_app_setup_event_cb( lv_obj_t * obj, lv_event_t event );
static void bestemmione_event_cb( lv_obj_t * obj, lv_event_t event );
static void refresh_bestemmione_event_cb( lv_obj_t * obj, lv_event_t event );
void prayer_app_task( lv_task_t * task );


lv_obj_t *bestemmione;

void prayer_app_main_setup( uint32_t tile_num ) {

    prayer_app_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &prayer_app_main_style, mainbar_get_style() );

    lv_obj_t * exit_btn = lv_imgbtn_create( prayer_app_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &prayer_app_main_style );
    lv_obj_align(exit_btn, prayer_app_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_prayer_app_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( prayer_app_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &prayer_app_main_style );
    lv_obj_align(setup_btn, prayer_app_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_prayer_app_setup_event_cb );


        /*Create a page*/
    lv_obj_t * page = lv_page_create(prayer_app_main_tile, NULL);
    lv_style_copy( &prayer_page_main_style, &prayer_app_main_style );
    //lv_style_set_bg_color( &prayer_page_main_style, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa( &prayer_page_main_style, LV_OBJ_PART_MAIN, LV_OPA_50);
    lv_obj_set_size(page, 240, 192);
    lv_obj_align(page, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);



    /*Create a label on the page*/
    lv_style_copy( &prayer_text_main_style, &prayer_page_main_style );
    lv_style_set_text_font( &prayer_text_main_style, LV_STATE_DEFAULT, &Ubuntu_32px);
    lv_style_set_text_color(&prayer_text_main_style, LV_STATE_DEFAULT, LV_COLOR_BLUE);
    bestemmione = lv_label_create(page, NULL);
    lv_obj_add_style( bestemmione, LV_OBJ_PART_MAIN, &prayer_text_main_style );
    lv_label_set_long_mode(bestemmione, LV_LABEL_LONG_BREAK);            /*Automatically break long lines*/
    lv_obj_set_width(bestemmione, lv_page_get_width_fit(page));          /*Set the label width to max value to not show hor. scroll bars*/
    lv_obj_set_event_cb( bestemmione, bestemmione_event_cb );

    lv_obj_t * reload_btn = lv_imgbtn_create( prayer_app_main_tile, NULL);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_PRESSED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_RELEASED, &refresh_32px);
    lv_imgbtn_set_src(reload_btn, LV_BTN_STATE_CHECKED_PRESSED, &refresh_32px);
    lv_obj_add_style(reload_btn, LV_IMGBTN_PART_MAIN, &prayer_app_main_style );
    lv_obj_align(reload_btn, prayer_app_main_tile, LV_ALIGN_IN_BOTTOM_MID, 0 , -10 );
    lv_obj_set_event_cb( reload_btn, refresh_bestemmione_event_cb );

    porcodio();

    // create an task that runs every secound
    _prayer_app_task = lv_task_create( prayer_app_task, 1000, LV_TASK_PRIO_MID, NULL );
}

static void enter_prayer_app_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( prayer_app_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_prayer_app_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

static void bestemmione_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_LONG_PRESSED ):       porcodio();
                                        break;
    }
}

static void refresh_bestemmione_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       porcodio();
                                        break;
    }
}

void prayer_app_task( lv_task_t * task ) {
    // put your code her
}

void porcodio() {
    String da[]={
    	"dalla madonna maiala",
    	"dalla portanna la madonna",
    	"dal dio bubù",
    	"dallo scanato gesù boia",
    	"dalla canaja de dio",
    	"dai fedeli inferociti",
    	"dai testimoni di geova",
    	"dai terrapiattisti",
    	"dai musulmani"
    };
    #define nda 9
    
    String aggettivi_madonna[]={    
        "maiala",
        "puttana",
        "puttina",
        "rincoglionita",
        "stitica",
        "putrefatta",
        "con le croste al culo",
        "col covid",
        "pompinara",
        "lesbica brutta"
    };
    #define aggm 10
    
    String aggettivi[]={    
        "porco",
        "cane",
        "maiale",
        "merdoso",
        "putrido",
        "succhiatore di cazzi",
        "col covid",
        "pedofilo",
        "torturatore di animali",
        "spacciatore di droga",
        "ladro di pollame",
        "licantropo",
        "onanista",
        "parassita",
        "ciclista senza sellino",
        "evasore fiscale",
        "impestato",
        "idiota",
        "bestemmiatore per primo"
    };
    #define agg 19
    
    String sorti_madonna[]={
    	"squoiata",
    	"inculata a sangue",
    	"presa a bastonate",
    	"frustata",
    	"presa a calci",
    	"sodomizzata",
    	"costretta a prostituirsi",
    	"che insulta suo figlio incitata da"
    };
    #define som 8

    String sorti[]={
	"frustato in piazza",
	"sodomizzato",
	"affogato nella merda",
	"avvelenato",
    "picchiato a sangue",
    "coperto di insulti",
    "talmente stupido da essere deriso"
    };
    #define so 7

    String soggetti[]={
        "il dio porco", 
        "porco di quel dio", 
        "dio", 
        "gesù", 
        "gesù merda", 
        "dio bubù", 
        "san giuseppe", 
        "il papa", 
        "la merda del clero",
        "padre pio"
    };
    #define sog 10
        
    int l,s1,s2,d1,d2;

    String tmp;
    srand(time(NULL));
    
    if ((rand() % 2)==0) {
        tmp=soggetti[rand() % sog] + " ";
        tmp = tmp + aggettivi[rand() % agg];
        l=(rand() % 3);
        if (l!=0) {
            s1=(rand() % so);
            d1=(rand() % nda);
            tmp = tmp + " " + sorti[s1];
            tmp = tmp + " " + da[d1];
            if (l==2) {
                tmp = tmp + " e";
                do { s2=(rand() % so); } while (s2==s1);
                do { d2=(rand() % nda); } while (d2==d1);
                tmp = tmp + " " + sorti[d2];
                tmp = tmp + " " + da[d2];              
            }
        }


    } else {
        tmp="La madonna ";
        tmp = tmp + aggettivi_madonna[rand() % aggm];
        l=(rand() % 3);
        if (l!=0) {
            s1=(rand() % som);
            d1=(rand() % nda);
            tmp = tmp + " " + sorti_madonna[s1];
            tmp = tmp + " " + da[d1];
            if (l==2) {
                tmp = tmp + " e";
                do { s2=(rand() % som); } while (s2==s1);
                do { d2=(rand() % nda); } while (d2==d1);
                tmp = tmp + " " + sorti_madonna[d2];
                tmp = tmp + " " + da[d2];              
            }
        }

    }

    char bufferone[1000]="";
    tmp.toCharArray(bufferone, 1000);

    lv_label_set_text(bestemmione, bufferone);
}