#include "config.h"
#include <TTGO.h>
#include "screenshot.h"
#include <rom/crc.h>
#include <endian.h>

lv_disp_drv_t screenshot_disp_drv;
static lv_disp_buf_t screenshot_disp_buf;
static lv_color_t screenshot_buf1[ LV_HOR_RES_MAX * 1 ];

lv_disp_t *system_disp;
lv_disp_t *screenshot_disp;

uint16_t *png;

volatile bool screenshot = false;

static void screenshot_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p );

void screenshot_setup( void ) {

    png = (uint16_t*)ps_malloc( LV_HOR_RES_MAX * LV_VER_RES_MAX * 2 );


    system_disp = lv_disp_get_default();
    lv_disp_drv_init( &screenshot_disp_drv );
    lv_disp_buf_init( &screenshot_disp_buf, screenshot_buf1, NULL, LV_HOR_RES_MAX * 1 );
    screenshot_disp_drv.hor_res = TFT_WIDTH;
    screenshot_disp_drv.ver_res = TFT_HEIGHT;
    screenshot_disp_drv.flush_cb = screenshot_disp_flush;
    screenshot_disp_drv.buffer = &screenshot_disp_buf;
    screenshot_disp = lv_disp_drv_register( &screenshot_disp_drv ); 

    lv_disp_set_default( system_disp );
}

void screenshot_take( void ) {
    Serial.printf("take screenshot\r\n");

    screenshot_disp->driver.flush_cb = system_disp->driver.flush_cb;
    system_disp->driver.flush_cb = screenshot_disp_flush;
    lv_obj_invalidate( lv_scr_act() );
    lv_refr_now( system_disp );
    system_disp->driver.flush_cb = screenshot_disp->driver.flush_cb;
}

void screenshot_save( void ) {
    Serial.printf("save screenshot\r\n");

    SPIFFS.remove("/screen.565");
    fs::File file = SPIFFS.open( "/screen.565", FILE_WRITE );
    
    file.write( (uint8_t *)png, LV_HOR_RES_MAX * LV_VER_RES_MAX * 2 );
    file.close();

    fs::File root = SPIFFS.open("/");
    file = root.openNextFile();
    while(file){
        Serial.printf("FILE: %s (%dbytes)\r\n", file.name(), file.size() );
        file = root.openNextFile();
    }
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
