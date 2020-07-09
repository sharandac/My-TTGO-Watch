#ifndef _SCREENSHOT_H
    #define _SCREENSHOT_H

    #include "config.h"

    void screenshot_setup( void );
    void screenshot_take( void );

    struct _PNG_HEADER {
        uint8_t     png[8] = { 0x89,0x50,0x4e,0x47,0x0d,0x0a,0x1a,0x0a };
        uint32_t    len = 13;
        uint8_t     IHDR[4] = { 'I', 'H', 'D', 'R' };
        uint32_t    width = LV_HOR_RES_MAX;
        uint32_t    height = LV_VER_RES_MAX;
        uint8_t     bitdepth = 8;
        uint8_t     colortype = 2;
        uint8_t     compression = 0;
        uint8_t     filter = 0;
        uint8_t     interlace_method = 0;
        uint32_t    crc = 0;
    };

    struct _PNG_RGB_PIXEL {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    struct _PNG_IDAT_CHUNCK {
        uint32_t    len;
        uint8_t     IDAT[ 4 ] = { 'I', 'D', 'A', 'T' };
        uint8_t     data[ LV_HOR_RES_MAX * LV_VER_RES_MAX * sizeof( _PNG_RGB_PIXEL ) ];
        uint32_t    crc;
    };

#endif // _SCREENSHOT_H