#include "config.h"
#include "touch.h"

lv_indev_t *touch_indev = NULL;

static bool touch_read(lv_indev_drv_t * drv, lv_indev_data_t*data);
static bool touch_getXY( int16_t &x, int16_t &y );

void touch_setup( TTGOClass *ttgo ) {
    touch_indev = lv_indev_get_next( NULL );

    touch_indev->driver.read_cb = touch_read;
}

static bool touch_getXY( int16_t &x, int16_t &y ) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    TP_Point p;

    if ( !ttgo->touch->touched() ) {
        return false;
    }    
    p = ttgo->touch->getPoint();

    uint8_t rotation = ttgo->tft->getRotation();
    switch ( rotation ) {
    case 0:
        x = TFT_WIDTH - p.x;
        y = TFT_HEIGHT - p.y;
        break;
    case 1:
        x = TFT_WIDTH - p.y;
        y = p.x;
        break;
    case 3:
        x = p.y;
        y = TFT_HEIGHT - p.x;
        break;
    case 2:
    default:
        x = p.x;
        y = p.y;
    }
    return true;
}

static bool touch_read(lv_indev_drv_t * drv, lv_indev_data_t*data) {
    data->state = touch_getXY(data->point.x, data->point.y) ?  LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
    return false;
}