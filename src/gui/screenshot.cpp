#include "config.h"
#include "screenshot.h"

uint16_t *png;

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p );

void screenshot_setup( void ) {
    png = (uint16_t*)ps_malloc( LV_HOR_RES_MAX * LV_VER_RES_MAX * sizeof( lv_color_t ) );
}

void screenshot_take( void ) {
    lv_disp_drv_t driver;
    lv_disp_t *system_disp;;

    system_disp = lv_disp_get_default();
    driver.flush_cb = system_disp->driver.flush_cb;
    system_disp->driver.flush_cb = screenshot_disp_flush;
    lv_obj_invalidate( lv_scr_act() );
    lv_refr_now( system_disp );
    system_disp->driver.flush_cb = driver.flush_cb;
}

void screenshot_save( void ) {
    SPIFFS.remove("/screen.565");
    fs::File file = SPIFFS.open( "/screen.565", FILE_WRITE );
    
    file.write( (uint8_t *)png, LV_HOR_RES_MAX * LV_VER_RES_MAX * 2 );
    file.close();
}

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p ) {

    uint32_t x, y;
    uint16_t *data = (uint16_t *)color_p;

    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            *(png + (y * LV_HOR_RES_MAX + x )) = *data;
            data++;
        }
    } 
    lv_disp_flush_ready(disp_drv);
}
