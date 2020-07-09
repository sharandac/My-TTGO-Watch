#include "config.h"

#include "keyboard.h"

static lv_obj_t *kb = NULL;
static void kb_event_cb(lv_obj_t * ta, lv_event_t event);

void keyboard_setup( void ) {
    if ( kb != NULL )
        return;
        
    kb = lv_keyboard_create( lv_scr_act() , NULL);
    lv_obj_set_size(kb,  LV_HOR_RES, LV_VER_RES / 2);
    lv_keyboard_set_cursor_manage(kb, true);
    lv_obj_set_event_cb( kb, kb_event_cb );
    lv_obj_set_hidden( kb, true );

}

static void kb_event_cb(lv_obj_t * ta, lv_event_t event) {
    lv_keyboard_def_event_cb( ta, event );
    if ( event == LV_EVENT_CANCEL ) {
        lv_obj_set_hidden( kb, true );
    }
    if ( event == LV_EVENT_APPLY ) {
        lv_obj_set_hidden( kb, true );
    }
}

void keyboard_set_textarea( lv_obj_t *textarea ){
    if ( kb == NULL )
        return;
    keyboard_show();
    lv_keyboard_set_textarea( kb, textarea );
}

void keyboard_hide( void ) {
    if ( kb == NULL )
        return;
    lv_obj_set_hidden( kb, true );
}

void keyboard_show( void ) {
    if ( kb == NULL )
        return;
    lv_obj_set_hidden( kb, false );
}
