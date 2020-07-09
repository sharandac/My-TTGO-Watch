#include "config.h"
#include <stdio.h>
#include <TTGO.h>

#include "gui.h"
#include "statusbar.h"
#include "keyboard.h"
#include "mainbar/mainbar.h"

#include "hardware/powermgm.h"
#include "hardware/display.h"

LV_IMG_DECLARE(bg2)

/**
 * Create a demo application
 */
void gui_setup(void)
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    //Create wallpaper
    lv_obj_t *img_bin = lv_img_create( lv_scr_act() , NULL);  /*Create an image object*/
    lv_img_set_src(img_bin, &bg2 );
    lv_obj_set_width( img_bin, hres );
    lv_obj_set_height( img_bin, vres );
    lv_obj_align(img_bin, NULL, LV_ALIGN_CENTER, 0, 0);
    
    mainbar_setup();
    statusbar_setup();
    keyboard_setup();
    lv_disp_trig_activity(NULL);

    return;
}

/*
 *
 */
void gui_loop( TTGOClass *ttgo ){

    // do task handler if still an useraction or go to standby after timeout
    if ( !powermgm_get_event( POWERMGM_STANDBY ) ) {
        if (lv_disp_get_inactive_time(NULL) < display_get_timeout() * 1000 ) {
            lv_task_handler();
            if ( lv_disp_get_inactive_time(NULL) > ( ( display_get_timeout() * 1000 ) - display_get_brightness() * 8 ) ) {
                ttgo->bl->adjust( ( ( display_get_timeout() * 1000 ) - lv_disp_get_inactive_time(NULL) ) / 8 );
            }
            else {
                ttgo->bl->adjust( display_get_brightness() );
            }
        }
        else {
            powermgm_set_event( POWERMGM_PMU_BUTTON );
        }
    }
}