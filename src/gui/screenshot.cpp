#include "config.h"
#include "screenshot.h"

lv_disp_drv_t screenshot_disp_drv;
static lv_disp_buf_t screenshot_disp_buf;
static lv_color_t screenshot_buf1[ LV_HOR_RES_MAX * 1 ];

lv_disp_t *system_disp;
lv_disp_t *screenshot_disp;

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p );

void screenshot_setup( void ) {

    system_disp = lv_disp_get_default();

    lv_disp_buf_init(&screenshot_disp_buf, screenshot_buf1, NULL, LV_HOR_RES_MAX * 1 );

    screenshot_disp_drv.hor_res = TFT_WIDTH;
    screenshot_disp_drv.ver_res = TFT_HEIGHT;
    screenshot_disp_drv.flush_cb = screenshot_disp_flush;
    screenshot_disp_drv.buffer = &screenshot_disp_buf;
    screenshot_disp = lv_disp_drv_register( &screenshot_disp_drv ); 

    lv_disp_set_default( system_disp );
}

void screenshot_take( void ) {
/*
    unsigned char* image = (unsigned char*)ps_malloc(240 * 240 * 3);
    Serial.printf("Total heap: %d\r\n", ESP.getHeapSize());
    Serial.printf("Free heap: %d\r\n", ESP.getFreeHeap());
    Serial.printf("Total PSRAM: %d\r\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\r\n", ESP.getFreePsram());
*/
    lv_obj_invalidate(lv_scr_act());
    lv_disp_set_default( screenshot_disp );
    lv_refr_now( screenshot_disp ); 
    lv_obj_invalidate(lv_scr_act());
    lv_disp_set_default( system_disp );
    lv_refr_now( system_disp ); 

//    free(image);
}

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p ) {

    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
//        Serial.printf("refresh Y=%d\r\n",y);
//        for(x = area->x1; x <= area->x2; x++) {
//        }
    }
    lv_disp_flush_ready(disp_drv);
}
