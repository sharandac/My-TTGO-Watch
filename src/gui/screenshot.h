#ifndef _SCREENSHOT_H
    #define _SCREENSHOT_H

    #include "config.h"

    #define     SCREENSHOT_EVENT   _BV(0)

    void screenshot_setup( void );
    void screenshot_take( void );
    void screenshot_save( void );

    struct PNG_IMAGE {
        uint8_t     png[ 8 ];
        uint32_t    IHDR_len;
        uint8_t     IHDR[ 4 ];
        uint32_t    width;
        uint32_t    height;
        uint8_t     bitdepth;
        uint8_t     colortype;
        uint8_t     compression;
        uint8_t     filter;
        uint8_t     interlace_method;
        uint32_t    IHDR_crc;
        uint32_t    IDAT_len = LV_HOR_RES_MAX * LV_VER_RES_MAX * 3;
        uint8_t     IDAT[ 4 ];
        uint8_t     data[ LV_HOR_RES_MAX * LV_VER_RES_MAX * 3 ];
        uint32_t    IDAT_crc;
        uint32_t    IEND_len;
        uint8_t     IEND[ 4 ];
        uint32_t    IEND_crc;
    } __attribute__((packed));

#endif // _SCREENSHOT_H