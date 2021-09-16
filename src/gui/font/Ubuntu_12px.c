#include <config.h>
#include "gui/charsets.h"

#if !defined(USE_EXTENDED_CHARSET) // Use dafault charset: latin chars only

/*******************************************************************************
 * Size: 12 px
 * Bpp: 2
 * Opts: 
 ******************************************************************************/
#include "lvgl.h"

#ifndef UBUNTU_12PX
#define UBUNTU_12PX 1
#endif

#if UBUNTU_12PX

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */

    /* U+0021 "!" */
    0x34, 0x39, 0xf, 0xf6, 0x8, 0xa,

    /* U+0022 "\"" */
    0x33, 0xf, 0xfe, 0x4, 0x40,

    /* U+0023 "#" */
    0x3, 0xc, 0x1e, 0xf0, 0xf3, 0xb2, 0xb3, 0xc6,
    0x87, 0xff, 0x11, 0xcf, 0x1a, 0xa7, 0x56, 0x9e,
    0x6, 0x0,

    /* U+0024 "$" */
    0xb, 0xd, 0x9a, 0x77, 0x90, 0x21, 0xd8, 0x6c,
    0xe0, 0xbc, 0xc1, 0xa4, 0x41, 0x82, 0xf3, 0x9e,
    0x30, 0xd0, 0x0,

    /* U+0025 "%" */
    0x2e, 0x3, 0x81, 0x31, 0x28, 0x1f, 0x2c, 0x4,
    0xc6, 0x81, 0xae, 0x5b, 0x83, 0xaa, 0xf4, 0x12,
    0x83, 0xf5, 0x3, 0xe5, 0x2, 0xe8,

    /* U+0026 "&" */
    0x5, 0xe0, 0xc9, 0xb0, 0x7d, 0x84, 0xbc, 0x12,
    0x58, 0x6b, 0xb3, 0x14, 0x36, 0x5, 0x1, 0xd,
    0x7e, 0x60,

    /* U+0027 "'" */
    0x30, 0xfd, 0x0,

    /* U+0028 "(" */
    0xf, 0x30, 0x28, 0x28, 0x1e, 0x2, 0x83, 0xff,
    0x86, 0x82, 0xc0, 0xa0, 0x98, 0x14,

    /* U+0029 ")" */
    0xa, 0x4a, 0x4, 0x96, 0x4, 0x43, 0x90, 0x93,
    0xc2, 0xa8, 0x48,

    /* U+002A "*" */
    0xf, 0xd2, 0x6, 0x4, 0x23, 0x60, 0xd1, 0x7,
    0x44,

    /* U+002B "+" */
    0x9, 0xf, 0x41, 0xfe, 0x7c, 0x78, 0x7c, 0x78,
    0x3f, 0xf8, 0x60,

    /* U+002C "," */
    0x30, 0xca, 0x18,

    /* U+002D "-" */
    0xbc, 0xde, 0x40,

    /* U+002E "." */
    0x20, 0x20,

    /* U+002F "/" */
    0xe, 0x43, 0x58, 0x66, 0xf, 0xe9, 0xc, 0xe1,
    0xc8, 0x4a, 0xd, 0x61, 0xc8, 0x4a, 0xd, 0x61,
    0xc8, 0x40,

    /* U+0030 "0" */
    0x7, 0xa0, 0xb7, 0x20, 0xa3, 0x44, 0x24, 0x3f,
    0xf8, 0x88, 0x48, 0x68, 0xd0, 0x6e, 0x40,

    /* U+0031 "1" */
    0x6, 0xac, 0x29, 0x14, 0x1f, 0xfc, 0xa0,

    /* U+0032 "2" */
    0x2f, 0x41, 0x79, 0x44, 0x7, 0xc, 0xe1, 0x4a,
    0x4, 0xc0, 0x56, 0x15, 0x3, 0xaf, 0x80,

    /* U+0033 "3" */
    0x2f, 0x40, 0xf9, 0x42, 0x1e, 0xf2, 0x81, 0xe5,
    0x6, 0xa0, 0x7c, 0x82, 0x85, 0xe5, 0x0,

    /* U+0034 "4" */
    0xd, 0xa1, 0xb0, 0xea, 0x7, 0x28, 0x35, 0x3,
    0x94, 0x1a, 0x3c, 0x4d, 0xf1, 0x21, 0xf0,

    /* U+0035 "5" */
    0x1f, 0xa0, 0x3e, 0x8f, 0x7, 0x68, 0x5f, 0x91,
    0x53, 0x86, 0xd1, 0x4, 0x97, 0x98,

    /* U+0036 "6" */
    0xa, 0xe0, 0xb3, 0x81, 0x50, 0x66, 0xf0, 0x6f,
    0x58, 0x72, 0x1f, 0xf4, 0x41, 0x66, 0xe0,

    /* U+0037 "7" */
    0x7f, 0x87, 0xe5, 0x7, 0x30, 0x69, 0xe, 0x60,
    0xc8, 0x7b, 0x43, 0x9c, 0x3c, 0x84,

    /* U+0038 "8" */
    0x7, 0xa0, 0xbb, 0x20, 0x93, 0x42, 0x8e, 0x4,
    0xea, 0x5, 0xe7, 0x10, 0xa0, 0x22, 0x21, 0x53,
    0xb0,

    /* U+0039 "9" */
    0x1f, 0x41, 0x53, 0x21, 0x12, 0x44, 0x24, 0x2b,
    0xd0, 0x4f, 0xa0, 0xf5, 0x2, 0x71, 0x7, 0x38,
    0x0,

    /* U+003A ":" */
    0x30, 0x20, 0x83, 0xf4, 0x4,

    /* U+003B ";" */
    0x30, 0x20, 0x83, 0xf6, 0x19, 0xa, 0x0,

    /* U+003C "<" */
    0xe, 0x82, 0x7e, 0x7, 0x4d, 0x9, 0x41, 0xbe,
    0x90, 0x9f, 0x40,

    /* U+003D "=" */
    0x7f, 0x87, 0xf8, 0x3f, 0x3f, 0xc3, 0xfc, 0x0,

    /* U+003E ">" */
    0x20, 0xfb, 0x83, 0x5f, 0x41, 0xe4, 0xf, 0x4c,
    0x1d, 0xa0,

    /* U+003F "?" */
    0x7d, 0xf, 0x94, 0x12, 0x12, 0xc0, 0xb0, 0x28,
    0x26, 0x9, 0x41, 0x78, 0x0,

    /* U+0040 "@" */
    0x9, 0xfa, 0xe, 0xff, 0x90, 0x4d, 0x3f, 0x40,
    0x65, 0x79, 0x14, 0x17, 0x6, 0xf0, 0x7f, 0x61,
    0xb8, 0x33, 0x84, 0xaf, 0x1a, 0x86, 0x2f, 0xd0,
    0x26, 0xf, 0xef, 0xe8, 0x20,

    /* U+0041 "A" */
    0xb, 0x83, 0xe4, 0x3a, 0x54, 0x19, 0xb4, 0x24,
    0x8, 0x83, 0x40, 0xe0, 0x57, 0x94, 0x27, 0xe1,
    0xd0, 0xce,

    /* U+0042 "B" */
    0xfd, 0x5, 0xe9, 0x41, 0x95, 0x87, 0x38, 0x3e,
    0x10, 0x5e, 0x70, 0x20, 0x82, 0x41, 0x5, 0x78,
    0xc0,

    /* U+0043 "C" */
    0x3, 0xf4, 0x6, 0x9f, 0x2, 0x88, 0x10, 0x90,
    0xe4, 0x3e, 0x43, 0xf9, 0xf, 0x51, 0x2, 0x6,
    0x9f, 0x0,

    /* U+0044 "D" */
    0xfd, 0x5, 0x73, 0x81, 0x1c, 0x43, 0x98, 0x3b,
    0xf, 0x61, 0xe6, 0x11, 0xc4, 0xb9, 0xc0,

    /* U+0045 "E" */
    0xfe, 0x5, 0xf0, 0x10, 0xff, 0xbe, 0x81, 0x7a,
    0x2, 0x1c, 0x87, 0x5f, 0x20,

    /* U+0046 "F" */
    0xfe, 0x5, 0xf0, 0x10, 0xff, 0xbe, 0x81, 0x7a,
    0x2, 0x1f, 0xfc, 0x40,

    /* U+0047 "G" */
    0x3, 0xf4, 0x6, 0x9f, 0x2, 0x88, 0x10, 0x90,
    0xe4, 0x39, 0x10, 0xe9, 0x2, 0x1e, 0xa2, 0x1c,
    0xd3, 0xc2,

    /* U+0048 "H" */
    0xc3, 0x48, 0x7f, 0xf2, 0xbf, 0x20, 0xbe, 0x9,
    0x9, 0xf, 0xfe, 0x18,

    /* U+0049 "I" */
    0xc3, 0xff, 0x8a,

    /* U+004A "J" */
    0xd, 0x87, 0xff, 0x65, 0x44, 0x82, 0xd8,

    /* U+004B "K" */
    0xc2, 0xa0, 0x69, 0x82, 0x56, 0x15, 0x30, 0xca,
    0xe, 0x9c, 0x33, 0x90, 0x76, 0xc1, 0xda, 0x0,

    /* U+004C "L" */
    0xc3, 0xff, 0xbc, 0x87, 0x5f, 0x0,

    /* U+004D "M" */
    0x28, 0x1d, 0x84, 0x86, 0x82, 0x48, 0x24, 0x34,
    0x82, 0x82, 0x5, 0x42, 0xc1, 0xa5, 0x83, 0xf5,
    0x1, 0x86, 0x90, 0xfc, 0xc1, 0x80,

    /* U+004E "N" */
    0xd0, 0x98, 0x83, 0xce, 0x1d, 0x30, 0x74, 0x87,
    0xac, 0x39, 0x50, 0x74, 0x1f, 0x20,

    /* U+004F "O" */
    0x3, 0xf4, 0x13, 0x4e, 0x60, 0x51, 0x1d, 0x40,
    0x43, 0x3a, 0x1f, 0x90, 0xff, 0x21, 0x9c, 0xa2,
    0x3a, 0x86, 0x9c, 0xc0,

    /* U+0050 "P" */
    0xfd, 0x2, 0xf3, 0x8, 0x24, 0x34, 0x9f, 0x31,
    0x7a, 0x2, 0x1f, 0xfc, 0x40,

    /* U+0051 "Q" */
    0x3, 0xf4, 0x13, 0x4e, 0x60, 0x51, 0x1d, 0x40,
    0x43, 0x3a, 0x1f, 0x90, 0xfb, 0x2, 0x19, 0x5,
    0x11, 0xd4, 0x34, 0xe6, 0x9, 0xe2, 0xf, 0x2b,
    0xc1, 0xeb, 0xc0,

    /* U+0052 "R" */
    0xfc, 0x85, 0x7a, 0x40, 0x82, 0x81, 0xd2, 0xf,
    0x88, 0x16, 0x41, 0x27, 0x7, 0x98, 0x3a, 0x40,

    /* U+0053 "S" */
    0x1f, 0x90, 0x53, 0x91, 0x11, 0x2, 0x41, 0xd9,
    0xc1, 0xad, 0x83, 0xa4, 0x8, 0x30, 0x37, 0x8c,

    /* U+0054 "T" */
    0xff, 0x5e, 0x8f, 0x40, 0x43, 0xff, 0xc4,

    /* U+0055 "U" */
    0x30, 0xda, 0x1f, 0xfd, 0xe4, 0x32, 0x32, 0x41,
    0x4d, 0x98,

    /* U+0056 "V" */
    0xa0, 0x6a, 0x68, 0x67, 0x43, 0x22, 0x30, 0x34,
    0x1a, 0x6, 0x2, 0x4, 0x39, 0xb8, 0x34, 0xb0,
    0x79, 0x8,

    /* U+0057 "W" */
    0xa0, 0x7c, 0xda, 0x16, 0x16, 0x21, 0x24, 0x7,
    0xd, 0xa8, 0x24, 0x8, 0xc1, 0xe7, 0x50, 0xb8,
    0x1b, 0xe3, 0xbc, 0x4, 0x50, 0x8a, 0xc, 0x84,
    0x84,

    /* U+0058 "X" */
    0x70, 0x9c, 0x54, 0xb, 0x4, 0xa9, 0xd, 0x46,
    0xc, 0x88, 0x74, 0xb0, 0x4a, 0xa8, 0x2a, 0x9,
    0x8, 0x83, 0x40,

    /* U+0059 "Y" */
    0xa0, 0x4e, 0x48, 0x54, 0x3, 0x8, 0x82, 0x55,
    0x86, 0xa2, 0x1c, 0x90, 0x7f, 0xf3, 0xc0,

    /* U+005A "Z" */
    0x7f, 0x95, 0xf2, 0x21, 0x9c, 0x37, 0x6, 0x84,
    0x33, 0x86, 0x90, 0xca, 0x83, 0x67, 0xe4,

    /* U+005B "[" */
    0xf8, 0xf0, 0x7f, 0xf4, 0xfc,

    /* U+005C "\\" */
    0x10, 0xe8, 0x39, 0x41, 0xb8, 0x32, 0x1e, 0x50,
    0x6e, 0xc, 0x87, 0x94, 0x1b, 0x83, 0x21, 0xe5,
    0x6, 0xf0,

    /* U+005D "]" */
    0xfd, 0x7, 0xff, 0x4f, 0x80,

    /* U+005E "^" */
    0x3, 0x87, 0x4c, 0x12, 0xa4, 0x2a, 0x14, 0x2,
    0x4, 0x0,

    /* U+005F "_" */
    0x3f, 0xc7, 0xf8,

    /* U+0060 "`" */
    0xd, 0x82, 0x83, 0x0,

    /* U+0061 "a" */
    0x2f, 0x40, 0xb9, 0xc, 0x89, 0x78, 0xa, 0xf2,
    0x1f, 0x2b, 0xc8,

    /* U+0062 "b" */
    0xf, 0xd8, 0x7f, 0xf2, 0x7e, 0xd, 0x76, 0x12,
    0x22, 0x1c, 0x87, 0x90, 0xe8, 0x41, 0xe7, 0x0,

    /* U+0063 "c" */
    0x7, 0xc0, 0xdf, 0x5, 0x4, 0x87, 0xfc, 0x90,
    0x6c, 0xf0,

    /* U+0064 "d" */
    0xf, 0xfe, 0x6, 0x1f, 0xfc, 0x5f, 0x82, 0xde,
    0x2, 0x42, 0x1f, 0xc8, 0x7e, 0x83, 0xb3, 0xc0,

    /* U+0065 "e" */
    0x7, 0xa0, 0xbb, 0x20, 0x92, 0x44, 0xf8, 0x37,
    0xe1, 0x20, 0xec, 0xf2, 0x0,

    /* U+0066 "f" */
    0x7, 0xa3, 0xb8, 0x8, 0x6f, 0x20, 0xf2, 0x1f,
    0xfc, 0xd0,

    /* U+0067 "g" */
    0x7, 0xd1, 0xbc, 0x82, 0x11, 0xf, 0xf9, 0x11,
    0x7, 0x70, 0x5e, 0x51, 0xf1, 0x0,

    /* U+0068 "h" */
    0xf, 0xb0, 0xff, 0xe3, 0x7c, 0x15, 0xb0, 0x12,
    0x43, 0xff, 0x98,

    /* U+0069 "i" */
    0x30, 0x40, 0x41, 0x87, 0xff, 0x1c,

    /* U+006A "j" */
    0x6, 0x14, 0x12, 0x16, 0x1f, 0xfc, 0xe4, 0x15,
    0x0,

    /* U+006B "k" */
    0x10, 0xf4, 0x1f, 0xfc, 0xbd, 0xd, 0x28, 0x28,
    0x83, 0x21, 0xed, 0x41, 0xdc, 0x1e, 0x70,

    /* U+006C "l" */
    0xd, 0x87, 0xff, 0x39, 0xc0,

    /* U+006D "m" */
    0x3e, 0xa7, 0xa0, 0xbc, 0xaf, 0x21, 0xd2, 0x5,
    0x7, 0x21, 0xff, 0xd6,

    /* U+006E "n" */
    0x3f, 0x5, 0xe6, 0xd, 0x21, 0xff, 0xcc,

    /* U+006F "o" */
    0x5, 0xe0, 0xb3, 0xb0, 0xa1, 0x14, 0x19, 0x10,
    0xc8, 0x50, 0x89, 0x9d, 0x80,

    /* U+0070 "p" */
    0x3e, 0x83, 0x78, 0xc3, 0xa1, 0xe, 0x43, 0xc8,
    0x48, 0x88, 0x2e, 0xc2, 0xf8, 0x3f, 0x80,

    /* U+0071 "q" */
    0x5, 0xf0, 0x33, 0xc1, 0xa0, 0xc8, 0x7f, 0xf0,
    0x52, 0x10, 0xb7, 0x83, 0x7c, 0x1f, 0xc0,

    /* U+0072 "r" */
    0x2f, 0x23, 0xe4, 0x3f, 0xf9, 0xa0,

    /* U+0073 "s" */
    0x2f, 0x2d, 0xe4, 0x39, 0xb8, 0xe, 0x61, 0x63,
    0xea, 0x0,

    /* U+0074 "t" */
    0xf, 0x61, 0xfe, 0xf2, 0xf, 0x21, 0xff, 0xc5,
    0x42, 0xa6, 0x80,

    /* U+0075 "u" */
    0x30, 0xb0, 0xff, 0xe7, 0x42, 0xd, 0xe4,

    /* U+0076 "v" */
    0xa0, 0x1b, 0x41, 0xe4, 0x25, 0x1, 0x41, 0x5e,
    0x80, 0xb8, 0x25, 0x0,

    /* U+0077 "w" */
    0xa0, 0x34, 0x19, 0x87, 0x94, 0x28, 0x4c, 0xf0,
    0x6e, 0x45, 0x1, 0x97, 0x6, 0xd4, 0x72, 0x12,
    0x62, 0x40,

    /* U+0078 "x" */
    0x70, 0x6a, 0x14, 0x27, 0x4c, 0x3f, 0x23, 0x2,
    0xe4, 0x90, 0x30,

    /* U+0079 "y" */
    0xa0, 0x29, 0xa0, 0x74, 0x32, 0x2c, 0x17, 0x70,
    0x11, 0x41, 0x48, 0x6b, 0x7, 0xa4, 0x0,

    /* U+007A "z" */
    0x7f, 0xf, 0x94, 0x12, 0xa0, 0xa8, 0x14, 0xa0,
    0x98, 0x28, 0xf8,

    /* U+007B "{" */
    0x5, 0x8a, 0xc0, 0x87, 0xfb, 0xc4, 0xa2, 0x50,
    0x3c, 0x1f, 0xf2, 0x5, 0x60,

    /* U+007C "|" */
    0x48, 0x3f, 0xf9, 0x60,

    /* U+007D "}" */
    0xe0, 0x6a, 0x2, 0x1f, 0xf7, 0x80, 0xa8, 0x54,
    0x78, 0x3f, 0xc8, 0x35, 0x0,

    /* U+007E "~" */
    0x2d, 0x21, 0x3e, 0x3, 0x17, 0x0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 44, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 0, .adv_w = 53, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 6, .adv_w = 80, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 11, .adv_w = 128, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 29, .adv_w = 108, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 48, .adv_w = 165, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 70, .adv_w = 128, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 88, .adv_w = 46, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 91, .adv_w = 62, .box_w = 4, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 105, .adv_w = 62, .box_w = 3, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 116, .adv_w = 92, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 125, .adv_w = 108, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 136, .adv_w = 47, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 139, .adv_w = 57, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 142, .adv_w = 47, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 144, .adv_w = 74, .box_w = 6, .box_h = 13, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 162, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 177, .adv_w = 108, .box_w = 4, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 184, .adv_w = 108, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 108, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 214, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 108, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 243, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 258, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 272, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 306, .adv_w = 47, .box_w = 3, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 311, .adv_w = 47, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 318, .adv_w = 108, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 329, .adv_w = 108, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 337, .adv_w = 108, .box_w = 7, .box_h = 6, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 347, .adv_w = 78, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 360, .adv_w = 182, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 389, .adv_w = 127, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 407, .adv_w = 123, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 424, .adv_w = 119, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 442, .adv_w = 137, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 457, .adv_w = 110, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 470, .adv_w = 103, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 482, .adv_w = 129, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 500, .adv_w = 135, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 512, .adv_w = 52, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 515, .adv_w = 96, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 522, .adv_w = 121, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 538, .adv_w = 100, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 544, .adv_w = 167, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 566, .adv_w = 140, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 580, .adv_w = 149, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 600, .adv_w = 117, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 613, .adv_w = 149, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 640, .adv_w = 121, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 656, .adv_w = 102, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 672, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 679, .adv_w = 132, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 689, .adv_w = 126, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 707, .adv_w = 178, .box_w = 11, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 732, .adv_w = 121, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 751, .adv_w = 115, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 766, .adv_w = 110, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 781, .adv_w = 63, .box_w = 3, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 786, .adv_w = 74, .box_w = 6, .box_h = 13, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 804, .adv_w = 63, .box_w = 3, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 809, .adv_w = 108, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 819, .adv_w = 94, .box_w = 7, .box_h = 2, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 822, .adv_w = 72, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 826, .adv_w = 100, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 837, .adv_w = 113, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 853, .adv_w = 89, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 863, .adv_w = 113, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 879, .adv_w = 107, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 892, .adv_w = 74, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 902, .adv_w = 111, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 916, .adv_w = 110, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 927, .adv_w = 49, .box_w = 3, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 933, .adv_w = 49, .box_w = 4, .box_h = 12, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 942, .adv_w = 100, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 957, .adv_w = 52, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 962, .adv_w = 165, .box_w = 10, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 974, .adv_w = 110, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 981, .adv_w = 113, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 994, .adv_w = 113, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1009, .adv_w = 113, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1024, .adv_w = 74, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1030, .adv_w = 86, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1040, .adv_w = 77, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1051, .adv_w = 110, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1058, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1070, .adv_w = 149, .box_w = 10, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1088, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1099, .adv_w = 95, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1114, .adv_w = 90, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1125, .adv_w = 64, .box_w = 4, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1138, .adv_w = 54, .box_w = 2, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1142, .adv_w = 64, .box_w = 4, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1155, .adv_w = 108, .box_w = 7, .box_h = 3, .ofs_x = 0, .ofs_y = 2}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 1, 0, 0, 0, 0,
    1, 2, 3, 4, 0, 5, 6, 5,
    7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 8, 8, 0, 0, 0,
    0, 9, 10, 11, 12, 13, 14, 15,
    16, 0, 0, 17, 18, 19, 20, 0,
    21, 22, 23, 24, 0, 25, 26, 27,
    28, 29, 30, 31, 32, 0, 33, 0,
    0, 0, 34, 35, 36, 0, 0, 37,
    0, 38, 0, 39, 40, 0, 41, 41,
    42, 43, 44, 45, 0, 46, 0, 47,
    48, 49, 0, 50, 51, 0, 52, 0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 1, 2, 0, 0, 0, 0,
    2, 3, 4, 5, 0, 6, 7, 6,
    8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 9, 9, 0, 0, 0,
    10, 11, 12, 0, 13, 0, 0, 0,
    14, 0, 0, 15, 0, 0, 16, 0,
    17, 0, 17, 0, 18, 19, 20, 21,
    22, 23, 24, 25, 26, 0, 27, 0,
    0, 0, 28, 0, 29, 30, 31, 32,
    33, 0, 0, 34, 0, 0, 35, 35,
    36, 35, 37, 35, 38, 39, 40, 41,
    42, 43, 44, 45, 46, 0, 47, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -15, -5, -5, -24, 0,
    -5, 0, 0, 0, 2, 2, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 15, 0, 0, 0, 0, 0,
    0, 0, 0, -6, -6, 0, 0, -6,
    -3, 2, -2, 3, 0, 0, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, -2, -2, -2, 0, -2, -2,
    -5, 0, -7, -3, -5, -8, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -12, -2, -2, -23, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 5,
    -6, -6, 5, 0, -6, 3, -13, -4,
    -15, -8, 2, -15, 4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, 0, -10, 0, -2,
    0, -6, -10, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -10, -5, -5,
    -14, 0, -5, 0, 6, 0, 6, 4,
    4, 7, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, -2, 0,
    0, -2, 0, -16, -2, -4, -2, 0,
    -8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, -2, -2, -5, 0,
    -2, -3, -8, -2, -7, -4, -7, -10,
    -7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -15, -4, 0, -12, 5, 0, 3, 0,
    0, -3, 6, -4, -4, 6, 0, -4,
    3, -12, -2, -12, -3, 5, -15, 4,
    0, -5, 0, -2, -2, -2, 0, -3,
    0, 0, -2, -2, 2, 0, -3, -4,
    -4, 5, -3, 3, -4, -2, 0, -2,
    -2, -5, -2, -2, 0, -4, -2, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, -2, -4, -5, 0, 0,
    -7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, -4, 0, 0, -4,
    0, 0, 4, -6, 0, 0, 3, -3,
    4, -4, -4, 4, 0, -4, 2, 3,
    0, 3, 0, 2, 3, 2, -2, 0,
    0, -3, -3, -3, 0, -3, 0, 0,
    -3, -3, 0, 0, -4, -3, -3, 3,
    -3, 0, -5, 0, -3, -5, -2, -6,
    0, -6, 0, -8, 0, -7, -2, -4,
    0, 0, -5, 0, 0, -2, -5, 0,
    -3, -3, -5, -8, -3, -2, -8, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, -3, -4, 0, -2,
    2, 0, 0, 0, 0, -3, 3, -5,
    -5, 3, 0, -5, 0, 0, -4, 0,
    0, 0, 0, 0, -3, -8, 0, -3,
    -3, -3, 0, -4, 0, 0, -3, -3,
    0, -3, -4, -5, -5, 2, -5, 0,
    -4, -4, -2, 0, -5, 0, 0, -12,
    0, -12, -3, 2, -3, -9, -2, -2,
    -15, 0, -2, 0, 3, 0, 3, 0,
    0, 4, 0, -3, -8, -10, -3, -3,
    -3, 0, -3, 0, -5, -3, -3, 0,
    0, -5, 0, 0, -7, 0, -5, 0,
    0, 0, 0, 0, -2, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, 0, -3, 0, 0, -2,
    0, 0, 0, -2, 0, -2, 0, -7,
    0, 0, 0, -2, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, -7, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, -4, 0,
    0, -3, 0, -6, 3, -9, 3, 0,
    2, -4, 7, -7, -7, 5, 0, -7,
    0, 3, 0, 4, 2, 5, 4, 4,
    0, -5, 0, -4, -4, -4, 0, -4,
    0, 0, -4, -4, 3, 0, -2, -3,
    -9, 5, -2, 3, -4, 0, 0, -24,
    -2, 0, -27, 4, -13, 2, 0, 0,
    0, 5, -9, -9, 5, 0, -9, 3,
    -22, -3, -19, -9, 4, -22, 3, 0,
    -6, 0, -2, -2, -2, 0, -2, 0,
    0, -2, -2, 0, 0, 0, -8, -7,
    4, -4, 2, -5, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -1, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, -4, -2, -5,
    0, -6, 0, -8, 0, -7, -2, -4,
    0, 0, -5, 0, 0, -2, -5, 0,
    -3, -3, -5, -8, -3, -2, -8, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, -2, 0, -2, -5, 0,
    -17, 0, -12, 0, 0, 0, -10, 0,
    0, -18, 0, 0, 0, 0, 0, 0,
    0, -3, 0, 0, -3, -8, -4, -4,
    -4, -6, 0, -4, 0, 0, -4, -2,
    0, 0, 0, 2, 2, 0, 2, 0,
    0, -5, -3, -4, -2, 0, 0, -6,
    0, 0, 0, -7, -2, -4, 0, 0,
    -5, 0, 0, -2, -5, 0, -3, -3,
    -5, -8, -3, -2, 0, -3, 0, 0,
    0, 0, 0, 3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 3, 0,
    2, 0, -2, -3, 5, -2, -2, 4,
    0, -2, 0, 0, 0, -2, 0, 4,
    -4, 3, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 4, 0, 2, -3, -3,
    0, 0, -5, 2, 0, -14, -9, -17,
    -16, 4, -10, -12, -5, -5, -17, 0,
    -5, 0, 5, 0, 5, 3, 3, 6,
    0, 0, -6, -9, -11, -11, -11, 0,
    -11, 0, -8, -11, -11, -9, 0, -7,
    -4, -16, -3, -4, -6, -5, 0, 0,
    0, -2, 0, 0, -2, 0, -8, -2,
    0, 0, -2, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, -7, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, -2, -4, 0, 0,
    0, 0, 0, -14, 0, 0, 0, 0,
    0, -12, -3, -3, -18, -1, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, -9, -9, -9, 0, -9, 0,
    -6, -9, -9, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, -3,
    0, 0, -7, 0, -9, -2, 3, -4,
    -3, -3, -3, -12, 0, -3, 0, 4,
    0, 4, 2, 2, 5, 0, 0, -7,
    -6, -4, -4, -4, 0, -5, 0, -4,
    -4, -4, -3, 0, -4, 0, 0, 0,
    0, 0, -3, 0, 0, 0, -5, 0,
    -5, 2, -5, 2, 0, 0, -5, 5,
    -5, -5, 4, 0, -5, 0, 2, 0,
    3, 0, 3, 4, 2, 0, -7, 0,
    -3, -3, -3, 0, -3, 0, 0, -3,
    -3, 0, 0, -2, -2, -7, 4, -1,
    0, -5, 0, 0, 0, -8, 3, 0,
    -12, -9, -16, -7, 5, -11, -15, -8,
    -8, -22, -2, -8, 0, 6, 0, 6,
    4, 4, 7, 2, 0, -5, -8, -9,
    -9, -9, 0, -9, 0, -8, -9, -9,
    -4, 0, -7, 0, 0, -3, 0, -5,
    -4, 0, 0, 0, -4, 0, 0, 2,
    -12, 0, 0, 0, -3, 3, -7, -7,
    3, 0, -7, 0, 0, -3, 2, 0,
    0, 2, 0, -3, -8, 0, -3, -3,
    -3, 0, -4, 0, 0, -3, -3, 0,
    -2, -4, -4, -5, 2, -4, 0, -7,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, -8, -8, -5,
    0, -8, -5, -6, -7, -6, -7, -7,
    -5, -8, 0, 15, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, -2, 0, 0,
    -2, 0, 0, 0, 0, -2, -2, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, -5, -4, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    -2, 0, -3, 0, 0, -4, 0, -7,
    0, -5, 0, 0, 0, -4, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, -2,
    -3, -2, -4, 0, -4, 0, 0, 0,
    0, 0, 2, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, 0, -2, 0, 0, -3, 0, 0,
    -2, 0, 0, 0, 0, 2, 2, 3,
    2, 0, -4, 0, 0, 0, 0, 7,
    2, -10, -7, -8, 0, 5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 4, 3, 3, 4,
    0, 0, 7, 0, -7, 0, -5, -4,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, -2, 0, -3, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    2, 0, 0, -4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, -5, -5, -5,
    0, -6, 0, 0, -6, -5, 0, 0,
    0, 0, 0, 4, 0, 0, -4, 0,
    0, -6, 0, -5, -4, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, -3, 0, -3, 0, 0, -4, 0,
    -6, 0, -5, -4, 0, 0, -4, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    -2, -3, -3, -4, 0, -4, 0, -6,
    0, -5, -4, 0, 0, -4, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, -2,
    -3, -3, -3, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 6, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, -11, -6, -9, 0, -8, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -5, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 4, 4, 3, 4,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, -7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, -2,
    -3, -2, 0, -2, 0, 0, -3, -2,
    0, 0, 0, 0, 0, 3, 0, 0,
    -4, 0, 0, 0, 0, 0, 2, -7,
    0, -6, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, -3, -3,
    -3, 0, -2, 0, 0, -3, -3, 0,
    0, 0, 3, 3, 3, 3, 0, 0,
    0, 0, 0, 0, 0, 2, -6, 0,
    -5, 0, -7, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, -2, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 3, 3, 3, 3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, 0, -4, -4, -4, 0,
    -4, 0, 0, -4, -4, 0, 0, 0,
    0, 0, 3, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -7, 0, -4, -4, -4, -2, -4,
    0, 0, -4, -3, 0, -2, -2, 0,
    0, 0, 0, 0, -5, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -5, -6, -2, 0, -6, -4,
    0, -4, 0, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 15, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, -4, 0, 0, -3, -5,
    -2, -5, -4, -6, -5, -7, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 52,
    .right_class_cnt     = 47,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 2,
    .kern_classes = 1,
    .bitmap_format = 1,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t Ubuntu_12px = {
#else
lv_font_t Ubuntu_12px = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 14,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if UBUNTU_12PX*/

#endif // !USE_EXTENDED_CHARSET
