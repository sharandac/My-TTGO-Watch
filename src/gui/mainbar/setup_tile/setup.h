#ifndef _SETUP_H
    #define _SETUP_H

    #include <TTGO.h>

    /*
     * @brief setup the setup tile
     * 
     * @param   tile    pointer to the tile obj
     * @param   style   pointer to the style obj
     * @param   hres    horizonal resolution
     * @param   vres    vertical resolution
     */
    void setup_tile_setup( lv_obj_t *tile, lv_style_t *style, lv_coord_t hres, lv_coord_t vres );

#endif // _SETUP_H