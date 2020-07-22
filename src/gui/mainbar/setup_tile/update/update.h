#ifndef _UPDATE_H
    #define _UPDATE_H

    #include <TTGO.h>

    #define FIRMWARE_LOCATION   "https://github.com/sharandac/My-TTGO-Watch/blob/master/ttgo-t-watch2020_v1.ino.bin"

    void update_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );
    void update_update_firmware( void );

#endif // _UPDATE_H