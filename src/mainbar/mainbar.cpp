#include <stdio.h>
#include "config.h"

#include "mainbar.h"
#include "main_tile/main_tile.h"
#include "setup_tile/setup_tile.h"
#include "note_tile/note_tile.h"
#include "app_tile/app_tile.h"

#include "wlan_settings_tile/wlan_settings_tile.h"

static lv_style_t mainbarstyle;
static lv_obj_t *mainbar = NULL;

static lv_point_t valid_pos[ TILE_NUM ];

lv_tile_entry_t tile_entry[ TILE_NUM ] {
    { NULL, MAIN_TILE, main_tile_setup, { 0 , 0 } },
    { NULL, NOTE_TILE, note_tile_setup, { 1 , 0 } },
    { NULL, APP_TILE, app_tile_setup, { 0 , 1 } },
    { NULL, SETUP_TILE, setup_tile_setup, { 1 , 1 } },
    { NULL, WLAN_SETTINGS_TILE, wlan_settings_tile_setup, { 1,3 } },
    { NULL, WLAN_PASSWORD_TILE, wlan_password_tile_setup, { 2,3 } }
};

void mainbar_setup( void ) {

    lv_style_init( &mainbarstyle );
    lv_style_set_radius(&mainbarstyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_color(&mainbarstyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
    lv_style_set_bg_opa(&mainbarstyle, LV_OBJ_PART_MAIN, LV_OPA_0);
    lv_style_set_border_width(&mainbarstyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color(&mainbarstyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_image_recolor(&mainbarstyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    
    mainbar = lv_tileview_create( lv_scr_act(), NULL);
    lv_tileview_set_valid_positions(mainbar, valid_pos, TILE_NUM );
    lv_tileview_set_edge_flash(mainbar, false);
    lv_obj_add_style( mainbar, LV_OBJ_PART_MAIN, &mainbarstyle );
    lv_page_set_scrlbar_mode(mainbar, LV_SCRLBAR_MODE_OFF);

    for( int tile = 0 ; tile < TILE_NUM ; tile++ ) {
        tile_entry[ tile ].tile = lv_obj_create( mainbar, NULL);
        lv_obj_set_size( tile_entry[ tile ].tile, LV_HOR_RES, LV_VER_RES);
        lv_obj_reset_style_list( tile_entry[ tile ].tile, LV_OBJ_PART_MAIN );
        lv_obj_add_style( tile_entry[ tile ].tile, LV_OBJ_PART_MAIN, &mainbarstyle );
        lv_obj_set_pos( tile_entry[ tile ].tile, tile_entry[ tile ].pos.x * LV_HOR_RES , tile_entry[ tile ].pos.y * LV_VER_RES );
        lv_tileview_add_element( mainbar, tile_entry[ tile ].tile);
        if ( tile_entry[ tile ].tilecallback != NULL )
            tile_entry[ tile ].tilecallback( tile_entry[ tile ].tile, &mainbarstyle, LV_HOR_RES , LV_VER_RES );
        valid_pos[ tile ].x = tile_entry[ tile ].pos.x;
        valid_pos[ tile ].y = tile_entry[ tile ].pos.y;
    }
    mainbar_jump_to_maintile();
}

void mainbar_jump_to_tile( lv_coord_t x, lv_coord_t y ) {
    lv_tileview_set_tile_act(mainbar, x, y,LV_ANIM_ON );
}

void mainbar_jump_to_tilenumber( lv_tile_number tile_number ) {
    for ( int tile = 0 ; tile < TILE_NUM; tile++ ) {
        if ( tile_entry[ tile ].tile_number == tile_number ) {
            lv_tileview_set_tile_act(mainbar, tile_entry[ tile ].pos.x, tile_entry[ tile ].pos.y, LV_ANIM_ON );
            break;
        }
    }
}

void mainbar_jump_to_maintile( void ) {
    for ( int tile = 0 ; tile < TILE_NUM; tile++ ) {
        if ( tile_entry[ tile ].tile_number == MAIN_TILE ) {
            lv_tileview_set_tile_act(mainbar, tile_entry[ tile ].pos.x, tile_entry[ tile ].pos.y, LV_ANIM_ON );
            break;
        }
    }
}