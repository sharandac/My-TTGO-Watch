#include <config.h>
#include "gui/charsets.h"

#if defined(USE_EXTENDED_CHARSET) && USE_EXTENDED_CHARSET == CHARSET_CYRILLIC

// 0x20-0xFF, 0x2012-0x2022, 0x401-0x045F, 0x0024, 0x00A3, 0x20AC, 0x00A9, 0x00AB, 0x00BB, 0x00AE, 0x00B0-0x00B4, 0x00D7, 0x2022, 0x2032-0x203A
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
    0x2d, 0x21, 0x3e, 0x3, 0x17, 0x0,

    /* U+00A0 " " */

    /* U+00A1 "¡" */
    0x34, 0x51, 0x3, 0xf, 0xfe, 0x2, 0x10,

    /* U+00A2 "¢" */
    0x4, 0x87, 0xd3, 0x2d, 0xe6, 0x81, 0x90, 0xc8,
    0x4c, 0x14, 0xfa, 0x26, 0x80, 0xd0,

    /* U+00A3 "£" */
    0x3, 0xf0, 0x54, 0xf0, 0x64, 0x3f, 0xce, 0x78,
    0xe, 0x78, 0x3f, 0xc8, 0x7e, 0xf8,

    /* U+00A4 "¤" */
    0x10, 0x90, 0x5f, 0x40, 0x6d, 0x40, 0x51, 0x4,
    0xa2, 0x9, 0xb1, 0x7, 0xd4, 0x0,

    /* U+00A5 "¥" */
    0xa0, 0x5b, 0x20, 0x58, 0xb2, 0x80, 0x4d, 0xb,
    0x3, 0x83, 0xc7, 0x83, 0xfb, 0xc7, 0x81, 0xe3,
    0xc0,

    /* U+00A6 "¦" */
    0xc3, 0xfb, 0xe, 0xc3, 0xf8,

    /* U+00A7 "§" */
    0x1f, 0x81, 0x7c, 0x12, 0x19, 0xb0, 0x2b, 0xb0,
    0x22, 0xf4, 0x28, 0x9d, 0x3, 0xc8, 0x64, 0xf,
    0xd0, 0xfc, 0x80,

    /* U+00A8 "¨" */
    0x11, 0x1c, 0x80,

    /* U+00A9 "©" */
    0xa, 0xf4, 0x1b, 0xfc, 0xa, 0x3e, 0x14, 0x2b,
    0xe6, 0x82, 0x21, 0xff, 0xc3, 0x44, 0x3c, 0xab,
    0xcd, 0xa, 0x1e, 0x14, 0xf, 0xf0,

    /* U+00AA "ª" */
    0x2e, 0x9, 0x43, 0xc0, 0x68, 0x0,

    /* U+00AB "«" */
    0x2, 0x20, 0xb2, 0x3, 0x44, 0x40, 0xe2, 0x34,
    0x7, 0x2c, 0xc, 0x90,

    /* U+00AC "¬" */
    0x7f, 0x87, 0xf0, 0x7f, 0xf2, 0x50,

    /* U+00AD "­" */
    0xbc, 0xde, 0x40,

    /* U+00AE "®" */
    0xa, 0xf4, 0x1b, 0xfc, 0xa, 0x7a, 0x58, 0x5b,
    0xd5, 0x8, 0x7f, 0xdd, 0x2, 0x41, 0xa8, 0x25,
    0x8b, 0xa1, 0x4c, 0x1a, 0x81, 0xfe, 0x0,

    /* U+00AF "¯" */
    0x7e,

    /* U+00B0 "°" */
    0x28, 0x5e, 0x83, 0x56,

    /* U+00B1 "±" */
    0xb, 0xf, 0xf3, 0xe3, 0xc3, 0xe3, 0xc1, 0xff,
    0xc6, 0x82, 0x7d, 0x78,

    /* U+00B2 "²" */
    0x6d, 0x52, 0xb, 0x5a, 0x80,

    /* U+00B3 "³" */
    0x2d, 0x31, 0x1a, 0x34, 0xc0,

    /* U+00B4 "´" */
    0xc, 0xc5, 0xc,

    /* U+00B5 "µ" */
    0x30, 0xb0, 0xff, 0xe7, 0x41, 0xef, 0x5, 0x7c,
    0x1e,

    /* U+00B6 "¶" */
    0x5, 0xf2, 0x69, 0x4d, 0xf, 0xfe, 0xa, 0x1f,
    0x70, 0x79, 0x41, 0xff, 0xd3,

    /* U+00B7 "·" */
    0x20, 0x20,

    /* U+00B8 "¸" */
    0x2, 0x14, 0xd, 0x0,

    /* U+00B9 "¹" */
    0x2c, 0x83, 0xe0,

    /* U+00BA "º" */
    0x2f, 0x9, 0xe8, 0x39, 0x3d, 0x0,

    /* U+00BB "»" */
    0x11, 0xa, 0x34, 0xd, 0x40, 0xd0, 0x25, 0x11,
    0x75, 0x8c, 0x60,

    /* U+00BC "¼" */
    0xf, 0xfe, 0x5, 0x84, 0xe1, 0xb0, 0xd4, 0xc,
    0x84, 0x88, 0x7e, 0xb0, 0xfc, 0xa0, 0x50, 0x2c,
    0xa0, 0x87, 0x94, 0xe, 0xe, 0xa0, 0xad, 0x9,
    0x40, 0x7c, 0x80,

    /* U+00BD "½" */
    0xf, 0xf9, 0xc2, 0xa0, 0x7e, 0x60, 0x90, 0xa8,
    0x1f, 0x94, 0x1f, 0x50, 0xb4, 0x18, 0xa2, 0xf0,
    0x54, 0xa, 0x81, 0x28, 0x14, 0xa, 0x81, 0x3e,

    /* U+00BE "¾" */
    0x7c, 0x1a, 0x9, 0xe4, 0x8, 0x75, 0x10, 0x50,
    0x35, 0x44, 0x86, 0x7b, 0x18, 0xa0, 0xf9, 0xa0,
    0x87, 0xca, 0x38, 0x3a, 0x82, 0xb4, 0x32, 0x87,
    0xc8,

    /* U+00BF "¿" */
    0x6, 0x84, 0xa0, 0xa0, 0xd2, 0x12, 0x81, 0xc0,
    0x94, 0x12, 0x14, 0x79,

    /* U+00C0 "À" */
    0xf, 0xfa, 0xf, 0xc8, 0x7a, 0x43, 0xdc, 0x1f,
    0x21, 0xd2, 0xa0, 0xcd, 0xa1, 0x20, 0x44, 0x1a,
    0x7, 0x2, 0xbc, 0xa1, 0x3f, 0xe, 0x86, 0x70,

    /* U+00C1 "Á" */
    0xf, 0xfe, 0x4, 0x1e, 0x43, 0xe6, 0xf, 0x70,
    0x7c, 0x87, 0x4a, 0x83, 0x36, 0x84, 0x81, 0x10,
    0x68, 0x1c, 0xa, 0xf2, 0x84, 0xfc, 0x3a, 0x19,
    0xc0,

    /* U+00C2 "Â" */
    0xf, 0xfa, 0x43, 0x93, 0x43, 0x34, 0x43, 0xb8,
    0x3e, 0x43, 0xa5, 0x41, 0x9b, 0x42, 0x40, 0x88,
    0x34, 0xe, 0x5, 0x79, 0x42, 0x7e, 0x1d, 0xc,
    0xe0,

    /* U+00C3 "Ã" */
    0xf, 0xf3, 0x90, 0x6f, 0xc1, 0xa3, 0x43, 0xb8,
    0x3e, 0x43, 0xa5, 0x41, 0x9b, 0x42, 0x40, 0x88,
    0x34, 0xe, 0x5, 0x79, 0x42, 0x7e, 0x1d, 0xc,
    0xe0,

    /* U+00C4 "Ä" */
    0x2, 0x4, 0x36, 0xac, 0x34, 0xa8, 0x3b, 0x83,
    0xe4, 0x3a, 0x54, 0x19, 0xb4, 0x24, 0x8, 0x83,
    0x40, 0xe0, 0x57, 0x94, 0x27, 0xe1, 0xd0, 0xce,

    /* U+00C5 "Å" */
    0xa, 0x81, 0xec, 0x3e, 0x90, 0xff, 0xe0, 0xca,
    0x83, 0x36, 0x84, 0x81, 0x10, 0x68, 0x1c, 0xa,
    0xf2, 0x84, 0xfc, 0x3a, 0x19, 0xc0,

    /* U+00C6 "Æ" */
    0xe, 0xfe, 0x43, 0x69, 0xf2, 0x12, 0x61, 0xfa,
    0x81, 0xfa, 0x10, 0x7c, 0x13, 0x5, 0xf0, 0x22,
    0xe0, 0xf3, 0x7a, 0xe, 0x94, 0x1b, 0xf0,

    /* U+00C7 "Ç" */
    0x3, 0xf4, 0x6, 0x9f, 0x2, 0x88, 0x10, 0x90,
    0xe4, 0x3e, 0x43, 0xf9, 0xf, 0x51, 0x2, 0x6,
    0xfc, 0x13, 0x1c, 0x1a, 0xc,

    /* U+00C8 "È" */
    0xf, 0x98, 0x32, 0xc1, 0xb4, 0x1f, 0xc0, 0xbe,
    0x2, 0x1f, 0xf7, 0xd0, 0x2f, 0x40, 0x43, 0x90,
    0xeb, 0xe4,

    /* U+00C9 "É" */
    0xf, 0xec, 0x35, 0x3, 0x48, 0x3f, 0x81, 0x7c,
    0x4, 0x3f, 0xef, 0xa0, 0x5e, 0x80, 0x87, 0x21,
    0xd7, 0xc8,

    /* U+00CA "Ê" */
    0xf, 0xd8, 0x69, 0xc2, 0xa6, 0xf, 0xe0, 0x5f,
    0x1, 0xf, 0xfb, 0xe8, 0x17, 0xa0, 0x21, 0xc8,
    0x75, 0xf2,

    /* U+00CB "Ë" */
    0x11, 0xa, 0x24, 0x19, 0xa7, 0xf0, 0x2f, 0x80,
    0x87, 0xfd, 0xf4, 0xb, 0xd0, 0x10, 0xe4, 0x3a,
    0xf9,

    /* U+00CC "Ì" */
    0xa, 0x4a, 0x3, 0x6, 0x1f, 0xfc, 0xd0,

    /* U+00CD "Í" */
    0xf, 0x60, 0xe0, 0x68, 0x30, 0xff, 0xeb, 0x0,

    /* U+00CE "Î" */
    0xf, 0xa0, 0xa3, 0x5, 0x30, 0xb0, 0xff, 0xef,
    0x0,

    /* U+00CF "Ï" */
    0x11, 0x4, 0x49, 0x9a, 0xc, 0x3f, 0xfb, 0xc0,

    /* U+00D0 "Ð" */
    0x2f, 0xa0, 0xcd, 0xe3, 0xf, 0xa6, 0xf, 0xb4,
    0x8b, 0xe, 0x8b, 0xf, 0xfa, 0x43, 0x98, 0x80,
    0xdc, 0x60,

    /* U+00D1 "Ñ" */
    0xf, 0xed, 0x41, 0x3f, 0x4, 0x94, 0x6, 0x84,
    0xc4, 0x1e, 0x70, 0xe9, 0x83, 0xa4, 0x3d, 0x61,
    0xca, 0x83, 0xa0, 0xf9, 0x0,

    /* U+00D2 "Ò" */
    0x9, 0x41, 0xf3, 0x7, 0xec, 0x3c, 0xfd, 0x4,
    0xd3, 0x98, 0x14, 0x47, 0x50, 0x10, 0xce, 0x87,
    0xe4, 0x3f, 0xc8, 0x67, 0x28, 0x8e, 0xa1, 0xa7,
    0x30,

    /* U+00D3 "Ó" */
    0xe, 0x83, 0xe8, 0x3f, 0x70, 0x73, 0xdc, 0x13,
    0x4e, 0x60, 0x51, 0x1d, 0x40, 0x43, 0x3a, 0x1f,
    0x90, 0xff, 0x21, 0x9c, 0xa2, 0x3a, 0x86, 0x9c,
    0xc0,

    /* U+00D4 "Ô" */
    0xd, 0x7, 0xd1, 0x87, 0xba, 0x7, 0x37, 0x40,
    0x9a, 0x73, 0x2, 0x88, 0xea, 0x2, 0x19, 0xd0,
    0xfc, 0x87, 0xf9, 0xc, 0xe5, 0x11, 0xd4, 0x34,
    0xe6, 0x0,

    /* U+00D5 "Õ" */
    0xf, 0xfe, 0x5, 0x4, 0x3b, 0xe4, 0x39, 0x58,
    0x73, 0xf4, 0x13, 0x4e, 0x60, 0x51, 0x1d, 0x40,
    0x43, 0x3a, 0x1f, 0x90, 0xff, 0x21, 0x9c, 0xa2,
    0x3a, 0x86, 0x9c, 0xc0,

    /* U+00D6 "Ö" */
    0xb, 0x24, 0x3a, 0x34, 0x39, 0x10, 0xe7, 0xe8,
    0x26, 0x9c, 0xc0, 0xa2, 0x3a, 0x80, 0x86, 0x74,
    0x3f, 0x21, 0xfe, 0x43, 0x39, 0x44, 0x75, 0xd,
    0x39, 0x80,

    /* U+00D7 "×" */
    0x10, 0xee, 0x34, 0x9d, 0x40, 0x4c, 0x11, 0xc9,
    0xc6, 0x0,

    /* U+00D8 "Ø" */
    0xf, 0xf9, 0xfa, 0xc0, 0xd3, 0x90, 0xa8, 0x87,
    0xc8, 0x2d, 0x90, 0xb8, 0x24, 0x12, 0x83, 0xa2,
    0x3, 0x93, 0x8e, 0xa2, 0xae, 0x60, 0x6f, 0xd0,
    0x0,

    /* U+00D9 "Ù" */
    0xf, 0xfb, 0xf, 0x98, 0x3d, 0x40, 0xd8, 0x6d,
    0xf, 0xfe, 0xf2, 0x19, 0x19, 0x20, 0xa6, 0xcc,

    /* U+00DA "Ú" */
    0xf, 0xfe, 0x4, 0x87, 0x22, 0x1c, 0xc1, 0xb0,
    0xda, 0x1f, 0xfd, 0xe4, 0x32, 0x32, 0x41, 0x4d,
    0x98,

    /* U+00DB "Û" */
    0xf, 0xf9, 0x83, 0xd0, 0x87, 0x72, 0x16, 0x1b,
    0x43, 0xff, 0xbc, 0x86, 0x46, 0x48, 0x29, 0xb3,
    0x0,

    /* U+00DC "Ü" */
    0x5, 0x1c, 0x35, 0x18, 0x3e, 0x42, 0xc3, 0x68,
    0x7f, 0xf7, 0x90, 0xc8, 0xc9, 0x5, 0x36, 0x60,

    /* U+00DD "Ý" */
    0xf, 0xfe, 0x6, 0x1e, 0xe0, 0xf6, 0x85, 0x40,
    0x9c, 0x90, 0xa8, 0x6, 0x11, 0x4, 0xab, 0xd,
    0x44, 0x39, 0x20, 0xff, 0xe7, 0x80,

    /* U+00DE "Þ" */
    0xc3, 0xdf, 0x20, 0xba, 0x22, 0x34, 0xf, 0xf4,
    0x9f, 0x31, 0x7a, 0x2, 0x18,

    /* U+00DF "ß" */
    0x7, 0xc8, 0x37, 0xa0, 0x50, 0x87, 0xb8, 0x34,
    0xa0, 0xe4, 0x3a, 0x9a, 0x19, 0xa0, 0x7f, 0xae,
    0x80,

    /* U+00E0 "à" */
    0xf, 0xd8, 0x75, 0x3, 0x30, 0x57, 0xa0, 0x5c,
    0x86, 0x44, 0xbc, 0x5, 0x79, 0xf, 0x95, 0xe4,

    /* U+00E1 "á" */
    0xf, 0xe9, 0x9, 0x10, 0x98, 0x2b, 0xd0, 0x2e,
    0x43, 0x22, 0x5e, 0x2, 0xbc, 0x87, 0xca, 0xf2,
    0x0,

    /* U+00E2 "â" */
    0xf, 0xcc, 0x1a, 0x20, 0xba, 0x2, 0xf4, 0xb,
    0x90, 0xc8, 0x97, 0x80, 0xaf, 0x21, 0xf2, 0xbc,
    0x80,

    /* U+00E3 "ã" */
    0xf, 0x9f, 0x40, 0x7d, 0x2, 0xf4, 0xb, 0x90,
    0xc8, 0x97, 0x80, 0xaf, 0x21, 0xf2, 0xbc, 0x80,

    /* U+00E4 "ä" */
    0x28, 0xe0, 0xa3, 0x7, 0x20, 0xbd, 0x2, 0xe4,
    0x32, 0x25, 0xe0, 0x2b, 0xc8, 0x7c, 0xaf, 0x20,

    /* U+00E5 "å" */
    0x5, 0x3, 0xfd, 0x40, 0xaf, 0x40, 0xb9, 0xc,
    0x89, 0x78, 0xa, 0xf2, 0x1f, 0x2b, 0xc8,

    /* U+00E6 "æ" */
    0x2f, 0x2b, 0xc8, 0x2e, 0xaf, 0x41, 0x91, 0x3,
    0x17, 0x87, 0xc8, 0xaf, 0x2b, 0xe8, 0x39, 0x41,
    0x2b, 0xc5, 0x3c, 0x0,

    /* U+00E7 "ç" */
    0x7, 0xc0, 0xdf, 0x5, 0x4, 0x87, 0x21, 0xf4,
    0x1b, 0x3c, 0x14, 0x61, 0x41, 0x0,

    /* U+00E8 "è" */
    0xf, 0xed, 0xe, 0xe0, 0xf6, 0x1d, 0xe8, 0x2e,
    0xc8, 0x24, 0x91, 0x3e, 0xd, 0xf8, 0x48, 0x3b,
    0x3c, 0x80,

    /* U+00E9 "é" */
    0xf, 0xf3, 0x7, 0x50, 0x3b, 0xe, 0xf4, 0x17,
    0x64, 0x12, 0x48, 0x9f, 0x6, 0xfc, 0x24, 0x1d,
    0x9e, 0x40,

    /* U+00EA "ê" */
    0xf, 0xf6, 0x1d, 0xd0, 0x36, 0xc1, 0xbd, 0x5,
    0xd9, 0x4, 0x92, 0x27, 0xc1, 0xbf, 0x9, 0x7,
    0x67, 0x90,

    /* U+00EB "ë" */
    0x1c, 0xc2, 0x62, 0xc, 0x88, 0x6f, 0x41, 0x76,
    0x41, 0x24, 0x89, 0xf0, 0x6f, 0xc2, 0x41, 0xd9,
    0xe4,

    /* U+00EC "ì" */
    0xa, 0x4a, 0x3, 0x6, 0x1f, 0xfc, 0x70,

    /* U+00ED "í" */
    0xc, 0xc5, 0xc, 0x18, 0x7f, 0xf1, 0xc0,

    /* U+00EE "î" */
    0xf, 0xb0, 0xbb, 0x6, 0xe1, 0x61, 0xff, 0xd4,

    /* U+00EF "ï" */
    0x33, 0x4, 0x40, 0x44, 0x2c, 0x3f, 0xfa, 0x80,

    /* U+00F0 "ð" */
    0x9, 0xf, 0xba, 0x5, 0x2, 0xb, 0xc8, 0x57,
    0x5, 0x9e, 0x14, 0x42, 0x1f, 0xc8, 0x72, 0x8,
    0x82, 0xdd, 0xc0,

    /* U+00F1 "ñ" */
    0xf, 0xdf, 0x5, 0xf0, 0x3f, 0x5, 0xe6, 0xd,
    0x21, 0xff, 0xcc,

    /* U+00F2 "ò" */
    0xf, 0xe9, 0xe, 0xa0, 0x7b, 0xe, 0xbc, 0x16,
    0x76, 0x14, 0x22, 0x83, 0x22, 0x19, 0xa, 0x11,
    0x33, 0xb0,

    /* U+00F3 "ó" */
    0xf, 0xf3, 0x7, 0x50, 0x3b, 0xe, 0xbc, 0x16,
    0x76, 0x14, 0x22, 0x83, 0x22, 0x19, 0xa, 0x11,
    0x33, 0xb0,

    /* U+00F4 "ô" */
    0xf, 0xf6, 0x1d, 0xd8, 0x6d, 0xc3, 0x5e, 0xb,
    0x3b, 0xa, 0x11, 0x41, 0x91, 0xc, 0x85, 0x8,
    0x99, 0xd8,

    /* U+00F5 "õ" */
    0xf, 0xef, 0x83, 0x7c, 0x1a, 0xf0, 0x59, 0xd8,
    0x50, 0x8a, 0xc, 0x88, 0x64, 0x28, 0x44, 0xce,
    0xc0,

    /* U+00F6 "ö" */
    0x6, 0x61, 0xa2, 0xc, 0x88, 0x6b, 0xc1, 0x67,
    0x61, 0x42, 0x28, 0x32, 0x21, 0x90, 0xa1, 0x13,
    0x3b, 0x0,

    /* U+00F7 "÷" */
    0xb, 0xf, 0x41, 0xe4, 0x27, 0xf8, 0x7f, 0x83,
    0x61, 0xe8, 0x20,

    /* U+00F8 "ø" */
    0xf, 0xeb, 0xe0, 0x66, 0x86, 0x82, 0x50, 0x39,
    0x12, 0x82, 0x12, 0xc2, 0x17, 0x60, 0xfc, 0x0,

    /* U+00F9 "ù" */
    0xf, 0xda, 0x1b, 0x83, 0xb0, 0xb0, 0xb0, 0xff,
    0xe7, 0x42, 0xd, 0xe4,

    /* U+00FA "ú" */
    0xf, 0xea, 0x6, 0x60, 0xd8, 0x58, 0x58, 0x7f,
    0xf3, 0xa1, 0x6, 0xf2,

    /* U+00FB "û" */
    0xf, 0xce, 0x1a, 0xa0, 0xb6, 0x6, 0x16, 0x1f,
    0xfc, 0xe8, 0x41, 0xbc, 0x80,

    /* U+00FC "ü" */
    0x1c, 0xc0, 0xc4, 0x12, 0x20, 0xc2, 0xc3, 0xff,
    0x9d, 0x8, 0x37, 0x90,

    /* U+00FD "ý" */
    0xf, 0xed, 0x9, 0x60, 0x98, 0x14, 0x5, 0x34,
    0xe, 0x86, 0x45, 0x82, 0xee, 0x2, 0x28, 0x29,
    0xd, 0x60, 0xf4, 0x80,

    /* U+00FE "þ" */
    0xf, 0xd8, 0x7f, 0xf2, 0x7d, 0x6, 0xb3, 0x9,
    0x21, 0xe, 0x43, 0xc8, 0x48, 0x88, 0x2e, 0xc2,
    0xf8, 0x3f, 0x80,

    /* U+00FF "ÿ" */
    0x28, 0xe0, 0xa3, 0x7, 0x25, 0x1, 0x4d, 0x3,
    0xa1, 0x91, 0x60, 0xbb, 0x80, 0x8a, 0xa, 0x43,
    0x58, 0x3d, 0x20,

    /* U+0401 "Ё" */
    0x11, 0xa, 0x24, 0x19, 0xa7, 0xf0, 0x2f, 0x80,
    0x87, 0xfd, 0xf4, 0xb, 0xd0, 0x10, 0xe4, 0x3a,
    0xf9,

    /* U+0402 "Ђ" */
    0xff, 0x40, 0xf4, 0x7a, 0xf, 0xfe, 0x1f, 0xc8,
    0x77, 0xa8, 0x1f, 0x22, 0x1f, 0x38, 0x7c, 0xe1,
    0xd6, 0xa0,

    /* U+0403 "Ѓ" */
    0xf, 0x98, 0x2a, 0x5, 0x83, 0xf8, 0xbe, 0x10,
    0xff, 0xea, 0x80,

    /* U+0404 "Є" */
    0x3, 0xf4, 0x6, 0x9f, 0x2, 0x88, 0x10, 0x90,
    0xe4, 0xfa, 0x2, 0x7d, 0x6, 0x43, 0xd4, 0x40,
    0x81, 0xa7, 0xc0,

    /* U+0405 "Ѕ" */
    0x1f, 0x90, 0x53, 0x91, 0x11, 0x2, 0x41, 0xd9,
    0xc1, 0xad, 0x83, 0xa4, 0x8, 0x30, 0x37, 0x8c,

    /* U+0406 "І" */
    0xc3, 0xff, 0x8a,

    /* U+0407 "Ї" */
    0x11, 0x4, 0x49, 0x9a, 0xc, 0x3f, 0xfb, 0xc0,

    /* U+0408 "Ј" */
    0xd, 0x87, 0xff, 0x65, 0x44, 0x82, 0xd8,

    /* U+0409 "Љ" */
    0x3, 0xf9, 0xf, 0xef, 0x21, 0xff, 0xce, 0xbd,
    0x7, 0x21, 0x3c, 0xc1, 0x61, 0xb1, 0x90, 0x98,
    0x3f, 0xd0, 0x76, 0x32, 0x46, 0x19, 0xe6, 0x0,

    /* U+040A "Њ" */
    0xc3, 0x68, 0x7f, 0xf3, 0xff, 0x17, 0xa0, 0x5f,
    0x17, 0x30, 0x87, 0x99, 0xf, 0xfe, 0x3b, 0x40,
    0xf5, 0xd1,

    /* U+040B "Ћ" */
    0xff, 0x40, 0xf4, 0x7a, 0xf, 0xfe, 0x1f, 0xc1,
    0xef, 0x38, 0x7d, 0x7, 0xff, 0x44,

    /* U+040C "Ќ" */
    0xf, 0xf6, 0x1d, 0x21, 0xd4, 0xb, 0xa, 0x81,
    0xa6, 0x9, 0x58, 0x54, 0xc3, 0x28, 0x3a, 0x70,
    0xce, 0x41, 0xdb, 0x7, 0x68,

    /* U+040D "Ѝ" */
    0xf, 0xec, 0x3d, 0x61, 0xce, 0x16, 0x1a, 0x81,
    0x94, 0x1d, 0x21, 0xbb, 0xa, 0x50, 0x4a, 0x83,
    0x50, 0x3c, 0x87, 0x41, 0xc0,

    /* U+040E "Ў" */
    0x4, 0x48, 0x5e, 0x10, 0x9e, 0x5, 0x84, 0xf0,
    0x6a, 0xc, 0x12, 0x48, 0x86, 0x6a, 0xa, 0x44,
    0x39, 0xc, 0x90, 0x1e, 0xc2,

    /* U+040F "Џ" */
    0xc3, 0x68, 0x7f, 0xf7, 0xd0, 0xf5, 0xf0, 0x3d,
    0x1e, 0x43, 0xe0,

    /* U+0410 "А" */
    0xb, 0x83, 0xe4, 0x3a, 0x54, 0x19, 0xb4, 0x24,
    0x8, 0x83, 0x40, 0xe0, 0x57, 0x94, 0x27, 0xe1,
    0xd0, 0xce,

    /* U+0411 "Б" */
    0xfe, 0x5, 0xf0, 0x10, 0xef, 0x90, 0x5d, 0x42,
    0x32, 0x1f, 0x23, 0x25, 0xd4,

    /* U+0412 "В" */
    0xfd, 0x5, 0xe9, 0x41, 0x95, 0x87, 0x38, 0x3e,
    0x10, 0x5e, 0x70, 0x20, 0x82, 0x41, 0x5, 0x78,
    0xc0,

    /* U+0413 "Г" */
    0xfe, 0x2f, 0x84, 0x3f, 0xfa, 0xa0,

    /* U+0414 "Д" */
    0x3, 0xf9, 0xe, 0xf0, 0x7f, 0xf0, 0xd0, 0xfb,
    0xf, 0xce, 0x1f, 0x90, 0xf4, 0x87, 0xb5, 0x7c,
    0x49, 0x7f, 0x21, 0xfe,

    /* U+0415 "Е" */
    0xfe, 0x5, 0xf0, 0x10, 0xff, 0xbe, 0x81, 0x7a,
    0x2, 0x1c, 0x87, 0x5f, 0x20,

    /* U+0416 "Ж" */
    0x30, 0xb0, 0x38, 0x2c, 0x3a, 0x80, 0x54, 0x17,
    0x21, 0x4c, 0x4a, 0xe, 0x50, 0xe1, 0xca, 0x8a,
    0x21, 0xa8, 0x8d, 0x2, 0xd4, 0x12, 0xa1, 0x20,
    0xe9, 0x40,

    /* U+0417 "З" */
    0x3e, 0x82, 0xbc, 0x60, 0x42, 0x42, 0xf3, 0x5,
    0xe7, 0xe, 0x83, 0xf9, 0x40, 0x82, 0xbc, 0x60,

    /* U+0418 "И" */
    0xc3, 0x50, 0x32, 0x83, 0xa4, 0x37, 0x61, 0x4a,
    0x9, 0x50, 0x6a, 0x7, 0x90, 0xe8, 0x38,

    /* U+0419 "Й" */
    0x4, 0x41, 0xd8, 0x75, 0xc0, 0xc3, 0x50, 0x32,
    0x83, 0xa4, 0x37, 0x61, 0x4a, 0x9, 0x50, 0x6a,
    0x7, 0x90, 0xe8, 0x38,

    /* U+041A "К" */
    0xc2, 0xa0, 0x69, 0x82, 0x56, 0x15, 0x30, 0xca,
    0xe, 0x9c, 0x33, 0x90, 0x76, 0xc1, 0xda, 0x0,

    /* U+041B "Л" */
    0x3, 0xf9, 0xd, 0xe4, 0x3f, 0xf9, 0x8, 0x7b,
    0xf, 0x98, 0x3a, 0xf, 0x46, 0x1c,

    /* U+041C "М" */
    0x28, 0x1d, 0x84, 0x86, 0x82, 0x48, 0x24, 0x34,
    0x82, 0x82, 0x5, 0x42, 0xc1, 0xa5, 0x83, 0xf5,
    0x1, 0x86, 0x90, 0xfc, 0xc1, 0x80,

    /* U+041D "Н" */
    0xc3, 0x48, 0x7f, 0xf2, 0xbf, 0x20, 0xbe, 0x9,
    0x9, 0xf, 0xfe, 0x18,

    /* U+041E "О" */
    0x3, 0xf4, 0x13, 0x4e, 0x60, 0x51, 0x1d, 0x40,
    0x43, 0x3a, 0x1f, 0x90, 0xff, 0x21, 0x9c, 0xa2,
    0x3a, 0x86, 0x9c, 0xc0,

    /* U+041F "П" */
    0xff, 0x25, 0xf0, 0x48, 0x7f, 0xf8, 0xc0,

    /* U+0420 "Р" */
    0xfd, 0x2, 0xf3, 0x8, 0x24, 0x34, 0x9f, 0x31,
    0x7a, 0x2, 0x1f, 0xfc, 0x40,

    /* U+0421 "С" */
    0x3, 0xf4, 0x6, 0x9f, 0x2, 0x88, 0x10, 0x90,
    0xe4, 0x3e, 0x43, 0xf9, 0xf, 0x51, 0x2, 0x6,
    0x9f, 0x0,

    /* U+0422 "Т" */
    0xff, 0x5e, 0x8f, 0x40, 0x43, 0xff, 0xc4,

    /* U+0423 "У" */
    0xb0, 0x9e, 0xd, 0x41, 0x82, 0x49, 0x10, 0xcd,
    0x41, 0x48, 0x87, 0x21, 0x92, 0x3, 0xd8, 0x40,

    /* U+0424 "Ф" */
    0xc, 0xc1, 0xe6, 0x8e, 0x84, 0xdc, 0x54, 0xa,
    0x20, 0x75, 0x84, 0x39, 0xc3, 0xfc, 0x87, 0xce,
    0x38, 0x68, 0x4a, 0x71, 0x98, 0x4f, 0x1c, 0x0,

    /* U+0425 "Х" */
    0x70, 0x9c, 0x54, 0xb, 0x4, 0xa9, 0xd, 0x46,
    0xc, 0x88, 0x74, 0xb0, 0x4a, 0xa8, 0x2a, 0x9,
    0x8, 0x83, 0x40,

    /* U+0426 "Ц" */
    0xc3, 0x68, 0x7f, 0xf9, 0x90, 0xfa, 0xf8, 0x9f,
    0xf2, 0x1f, 0xc0,

    /* U+0427 "Ч" */
    0x70, 0xa8, 0x1f, 0xfc, 0x24, 0x3f, 0x21, 0xd4,
    0xf2, 0x7, 0xe4, 0x3f, 0xf8, 0x60,

    /* U+0428 "Ш" */
    0xc2, 0x90, 0x38, 0x7f, 0xfc, 0x50, 0xb0, 0xeb,
    0xca, 0xf4, 0x0,

    /* U+0429 "Щ" */
    0xc2, 0x90, 0x38, 0x7f, 0xfe, 0x10, 0xb0, 0xf5,
    0xe5, 0x7a, 0x3f, 0xfd, 0x7, 0xff, 0x0,

    /* U+042A "Ъ" */
    0xfa, 0xe, 0xf2, 0x1f, 0xfc, 0x67, 0xd0, 0x73,
    0xc6, 0x1e, 0x60, 0xff, 0xe2, 0xc1, 0xcf, 0x8c,

    /* U+042B "Ы" */
    0xc3, 0xd8, 0x7f, 0xf2, 0x3e, 0x43, 0xae, 0x88,
    0x64, 0x56, 0x1f, 0xfc, 0x14, 0x56, 0x1a, 0xe8,
    0x84,

    /* U+042C "Ь" */
    0xc3, 0xff, 0x8d, 0xf2, 0xb, 0xa8, 0x46, 0x43,
    0xe4, 0x64, 0xba, 0x80,

    /* U+042D "Э" */
    0x7e, 0x80, 0xdc, 0xe0, 0x47, 0x10, 0xe7, 0x7,
    0xc8, 0x5f, 0x21, 0xe7, 0x11, 0xc5, 0x5c, 0xe0,

    /* U+042E "Ю" */
    0xc2, 0x6f, 0x41, 0xcc, 0x73, 0x86, 0xd8, 0x71,
    0x3c, 0xe1, 0x9c, 0xb0, 0xfe, 0x43, 0xff, 0x81,
    0x61, 0x9c, 0x25, 0x43, 0x88, 0x6c, 0xe7, 0x0,

    /* U+042F "Я" */
    0x5, 0xf4, 0x67, 0x90, 0xa0, 0xf2, 0x1d, 0x4f,
    0x20, 0x4b, 0x40, 0x88, 0x6a, 0x6, 0x44, 0x30,

    /* U+0430 "а" */
    0x2f, 0x40, 0xb9, 0xc, 0x89, 0x78, 0xa, 0xf2,
    0x1f, 0x2b, 0xc8,

    /* U+0431 "б" */
    0xe, 0x42, 0x7d, 0x1, 0xbe, 0x5, 0x3, 0xdf,
    0x21, 0x7a, 0x81, 0x22, 0x21, 0xc8, 0x79, 0x3,
    0x8, 0x91, 0xd8,

    /* U+0432 "в" */
    0x3f, 0x6, 0xf3, 0x7, 0xfb, 0xce, 0x17, 0xa8,
    0x1f, 0xef, 0x50,

    /* U+0433 "г" */
    0x3f, 0x3, 0xe0, 0xff, 0xe6, 0x80,

    /* U+0434 "д" */
    0x7, 0xe0, 0xde, 0xf, 0xfe, 0x2a, 0x83, 0xac,
    0x36, 0x5e, 0x32, 0xfc, 0x28, 0x38,

    /* U+0435 "е" */
    0x7, 0xa0, 0xbb, 0x20, 0x92, 0x44, 0xf8, 0x37,
    0xe1, 0x20, 0xec, 0xf2, 0x0,

    /* U+0436 "ж" */
    0x30, 0x60, 0xd2, 0xc2, 0x94, 0x2a, 0x22, 0xb,
    0x47, 0xd, 0x67, 0x20, 0xe0, 0xa8, 0x4a, 0x9,
    0x50,

    /* U+0437 "з" */
    0x3e, 0x41, 0xea, 0x7, 0xe7, 0xa0, 0x1f, 0x41,
    0xf3, 0xf2, 0x0,

    /* U+0438 "и" */
    0x30, 0xb0, 0xec, 0x39, 0x41, 0xd4, 0xd, 0x40,
    0xe5, 0x7, 0x61, 0x80,

    /* U+0439 "й" */
    0x6, 0x41, 0x9c, 0x3a, 0xe0, 0xb0, 0xb0, 0xec,
    0x39, 0x41, 0xd4, 0xd, 0x40, 0xe5, 0x7, 0x61,
    0x80,

    /* U+043A "к" */
    0x30, 0x50, 0x34, 0x41, 0x4c, 0x19, 0x41, 0xdb,
    0x7, 0x6a, 0xe, 0xa0,

    /* U+043B "л" */
    0x7, 0xe0, 0xde, 0xf, 0xfe, 0x2a, 0x1e, 0xa0,
    0x6d, 0x41, 0xb8, 0x2c,

    /* U+043C "м" */
    0x38, 0x2e, 0x2, 0x4, 0x32, 0x8d, 0xa, 0x83,
    0xa1, 0x3c, 0x86, 0xc4, 0x39, 0xc2,

    /* U+043D "н" */
    0x30, 0xb0, 0xff, 0xe2, 0xfc, 0x1b, 0xe0, 0xff,
    0xe2, 0x0,

    /* U+043E "о" */
    0x5, 0xe0, 0xb3, 0xb0, 0xa1, 0x14, 0x19, 0x10,
    0xc8, 0x50, 0x89, 0x9d, 0x80,

    /* U+043F "п" */
    0x3f, 0x81, 0xf0, 0x7f, 0xf4, 0xc0,

    /* U+0440 "р" */
    0x3e, 0x83, 0x78, 0xc3, 0xa1, 0xe, 0x43, 0xc8,
    0x48, 0x88, 0x2e, 0xc2, 0xf8, 0x3f, 0x80,

    /* U+0441 "с" */
    0x7, 0xc0, 0xdf, 0x5, 0x4, 0x87, 0xfc, 0x90,
    0x6c, 0xf0,

    /* U+0442 "т" */
    0xfe, 0x7c, 0x5a, 0x1f, 0xfd, 0x20,

    /* U+0443 "у" */
    0xa0, 0x29, 0xa0, 0x74, 0x32, 0x2c, 0x17, 0x70,
    0x11, 0x41, 0x48, 0x6b, 0x7, 0xa4, 0x0,

    /* U+0444 "ф" */
    0xf, 0xfe, 0xe, 0x1f, 0xfc, 0xcb, 0x38, 0x2c,
    0xcc, 0xc4, 0x82, 0x84, 0x3e, 0x70, 0xf9, 0xd2,
    0xa, 0x13, 0x33, 0x30, 0xac, 0xe0, 0xff, 0x80,

    /* U+0445 "х" */
    0x70, 0x6a, 0x14, 0x27, 0x4c, 0x3f, 0x23, 0x2,
    0xe4, 0x90, 0x30,

    /* U+0446 "ц" */
    0x30, 0xb0, 0xff, 0xed, 0x7c, 0x67, 0xe8, 0x3e,
    0x40,

    /* U+0447 "ч" */
    0x60, 0x48, 0x7f, 0xf0, 0x50, 0xca, 0xf2, 0xb,
    0xc8, 0x7c,

    /* U+0448 "ш" */
    0x30, 0x50, 0x18, 0x7f, 0xf8, 0xfc, 0xaf, 0x0,

    /* U+0449 "щ" */
    0x30, 0x50, 0x18, 0x7f, 0xfa, 0x7c, 0xaf, 0x19,
    0xff, 0x41, 0xfe, 0x40,

    /* U+044A "ъ" */
    0xf8, 0x37, 0x83, 0xf7, 0xa0, 0xde, 0x20, 0xed,
    0xe, 0xd0, 0xbc, 0x40,

    /* U+044B "ы" */
    0x30, 0xec, 0x3f, 0xf8, 0x1e, 0x83, 0xde, 0x20,
    0xfb, 0x43, 0xed, 0xe, 0xf1, 0x4,

    /* U+044C "ь" */
    0x30, 0xff, 0xe0, 0x7c, 0x17, 0x9c, 0x34, 0x1d,
    0x5, 0xe7,

    /* U+044D "э" */
    0x7d, 0x1, 0xf1, 0x86, 0x82, 0x7c, 0x23, 0xe1,
    0xa, 0x3, 0xe3, 0x0,

    /* U+044E "ю" */
    0x30, 0x3e, 0x83, 0xd4, 0xc8, 0x32, 0xc4, 0x85,
    0xc1, 0xfb, 0x83, 0xf9, 0x62, 0x43, 0xa9, 0x90,

    /* U+044F "я" */
    0x1f, 0x92, 0xf3, 0x86, 0xc0, 0xfa, 0x9, 0xe0,
    0x30, 0x4a, 0xc,

    /* U+0450 "ѐ" */
    0xf, 0xed, 0xe, 0xe0, 0xf6, 0x1d, 0xe8, 0x2e,
    0xc8, 0x24, 0x91, 0x3e, 0xd, 0xf8, 0x48, 0x3b,
    0x3c, 0x80,

    /* U+0451 "ё" */
    0x1c, 0xc2, 0x62, 0xc, 0x88, 0x6f, 0x41, 0x76,
    0x41, 0x24, 0x89, 0xf0, 0x6f, 0xc2, 0x41, 0xd9,
    0xe4,

    /* U+0452 "ђ" */
    0xf, 0xb0, 0xd9, 0xe0, 0x67, 0x83, 0x7c, 0x17,
    0x98, 0x34, 0x87, 0xff, 0x37, 0x2, 0x19, 0xa0,

    /* U+0453 "ѓ" */
    0xf, 0xd2, 0x4, 0x40, 0xc0, 0xfc, 0xf, 0x83,
    0xff, 0x9a,

    /* U+0454 "є" */
    0x7, 0xc0, 0xdf, 0x5, 0x4, 0x9e, 0x42, 0xf2,
    0x24, 0x1b, 0x7c, 0x80,

    /* U+0455 "ѕ" */
    0x2f, 0x2d, 0xe4, 0x39, 0xb8, 0xe, 0x61, 0x63,
    0xea, 0x0,

    /* U+0456 "і" */
    0x10, 0x40, 0xc1, 0x87, 0xff, 0x1c,

    /* U+0457 "ї" */
    0x11, 0x4, 0x40, 0xcc, 0x2c, 0x3f, 0xfa, 0x80,

    /* U+0458 "ј" */
    0x6, 0x14, 0x12, 0x16, 0x1f, 0xfc, 0xe4, 0x15,
    0x0,

    /* U+0459 "љ" */
    0x7, 0xe0, 0xfb, 0xc1, 0xff, 0x7a, 0xf, 0xac,
    0xc4, 0x32, 0x40, 0xa0, 0x74, 0x4a, 0x3, 0xe7,

    /* U+045A "њ" */
    0x30, 0xb0, 0xff, 0xe4, 0x5c, 0x17, 0xc5, 0x90,
    0x3e, 0xb, 0x43, 0xf6, 0x87, 0xbc, 0x40,

    /* U+045B "ћ" */
    0xf, 0xb0, 0xd9, 0xe0, 0x67, 0x83, 0x7c, 0x17,
    0x98, 0x34, 0x87, 0xff, 0x30,

    /* U+045C "ќ" */
    0xf, 0xf5, 0x3, 0x98, 0x3b, 0xd, 0x82, 0x81,
    0xa2, 0xa, 0x60, 0xca, 0xe, 0xd8, 0x3b, 0x50,
    0x75, 0x0,

    /* U+045D "ѝ" */
    0xf, 0xea, 0x7, 0x48, 0x7b, 0xd, 0x85, 0x87,
    0x61, 0xca, 0xe, 0xa0, 0x6a, 0x7, 0x28, 0x3b,
    0xc,

    /* U+045E "ў" */
    0x14, 0x40, 0x7e, 0xa, 0xd2, 0x80, 0xa6, 0x81,
    0xd0, 0xc8, 0xb0, 0x5d, 0xc0, 0x45, 0x5, 0x21,
    0xac, 0x1e, 0x90, 0x0,

    /* U+045F "џ" */
    0x30, 0xb0, 0xff, 0xea, 0x7c, 0xf, 0x1e, 0xf,
    0x0,

    /* U+2013 "–" */
    0x3f, 0xc7, 0xf8,

    /* U+2014 "—" */
    0x3f, 0xff, 0x80, 0x7f, 0xff, 0x0,

    /* U+2015 "―" */
    0x3f, 0xff, 0x80, 0x7f, 0xff, 0x0,

    /* U+2018 "‘" */
    0xd, 0x22, 0xc0,

    /* U+2019 "’" */
    0x30, 0xca, 0x18,

    /* U+201A "‚" */
    0x30, 0xca, 0x18,

    /* U+201C "“" */
    0xf, 0x4c, 0x8b, 0xc1, 0x80,

    /* U+201D "”" */
    0x33, 0x43, 0x2f, 0xc, 0xc0,

    /* U+201E "„" */
    0x33, 0x43, 0x2f, 0xc, 0xc0,

    /* U+2020 "†" */
    0x4, 0x87, 0xff, 0xa, 0xdb, 0x6d, 0xb4, 0x3f,
    0xfa, 0x9a, 0x19, 0x8,

    /* U+2021 "‡" */
    0x4, 0x87, 0xff, 0xa, 0xdb, 0x6d, 0xb4, 0x3c,
    0xed, 0x8e, 0xd8, 0x7f, 0xf0, 0xf4, 0x32, 0x10,

    /* U+2022 "•" */
    0x2, 0xe, 0x50, 0x34, 0x10,

    /* U+2039 "‹" */
    0x2, 0xb, 0x3, 0x8, 0x48, 0x83, 0x82, 0xc0,

    /* U+203A "›" */
    0x10, 0xa0, 0x98, 0x1a, 0xa, 0xb, 0xc, 0x0,

    /* U+20AC "€" */
    0xa, 0xf0, 0x59, 0xe0, 0xd0, 0x4e, 0x79, 0x19,
    0xf2, 0x33, 0xe0, 0x39, 0xe0, 0xe8, 0x3b, 0x3c,
    0x80
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
    {.bitmap_index = 1155, .adv_w = 108, .box_w = 7, .box_h = 3, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 1161, .adv_w = 44, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1161, .adv_w = 53, .box_w = 3, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1168, .adv_w = 108, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1182, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1196, .adv_w = 108, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 1210, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1227, .adv_w = 54, .box_w = 2, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1232, .adv_w = 94, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1251, .adv_w = 72, .box_w = 5, .box_h = 2, .ofs_x = 0, .ofs_y = 8},
    {.bitmap_index = 1254, .adv_w = 156, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1276, .adv_w = 75, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1282, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1294, .adv_w = 108, .box_w = 7, .box_h = 5, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1300, .adv_w = 57, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1303, .adv_w = 156, .box_w = 9, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1326, .adv_w = 72, .box_w = 4, .box_h = 1, .ofs_x = 0, .ofs_y = 8},
    {.bitmap_index = 1327, .adv_w = 64, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 1331, .adv_w = 108, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1343, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1348, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1353, .adv_w = 72, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 1356, .adv_w = 111, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1365, .adv_w = 124, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1378, .adv_w = 47, .box_w = 3, .box_h = 2, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 1380, .adv_w = 72, .box_w = 4, .box_h = 3, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1384, .adv_w = 69, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1387, .adv_w = 86, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 1393, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1404, .adv_w = 168, .box_w = 11, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1431, .adv_w = 168, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1455, .adv_w = 168, .box_w = 11, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1480, .adv_w = 78, .box_w = 5, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1492, .adv_w = 127, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1516, .adv_w = 127, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1541, .adv_w = 127, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1566, .adv_w = 127, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1591, .adv_w = 127, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1615, .adv_w = 127, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1637, .adv_w = 180, .box_w = 11, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1660, .adv_w = 119, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1681, .adv_w = 110, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1699, .adv_w = 110, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1717, .adv_w = 110, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1735, .adv_w = 110, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1752, .adv_w = 52, .box_w = 3, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1759, .adv_w = 52, .box_w = 4, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1767, .adv_w = 52, .box_w = 5, .box_h = 13, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1776, .adv_w = 52, .box_w = 5, .box_h = 12, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 1784, .adv_w = 139, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1802, .adv_w = 140, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1823, .adv_w = 149, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1848, .adv_w = 149, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1873, .adv_w = 149, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1899, .adv_w = 149, .box_w = 9, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1927, .adv_w = 149, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1953, .adv_w = 108, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 1963, .adv_w = 149, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 1988, .adv_w = 132, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2004, .adv_w = 132, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2021, .adv_w = 132, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2038, .adv_w = 132, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2054, .adv_w = 115, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2076, .adv_w = 117, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2089, .adv_w = 120, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2106, .adv_w = 100, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2122, .adv_w = 100, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2139, .adv_w = 100, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2156, .adv_w = 100, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2172, .adv_w = 100, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2188, .adv_w = 100, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2203, .adv_w = 165, .box_w = 10, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2223, .adv_w = 89, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 2237, .adv_w = 107, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2255, .adv_w = 107, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2273, .adv_w = 107, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2291, .adv_w = 107, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2308, .adv_w = 49, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2315, .adv_w = 49, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2322, .adv_w = 49, .box_w = 5, .box_h = 11, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 2330, .adv_w = 49, .box_w = 5, .box_h = 10, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 2338, .adv_w = 113, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2357, .adv_w = 110, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2368, .adv_w = 113, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2386, .adv_w = 113, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2404, .adv_w = 113, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2422, .adv_w = 113, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2439, .adv_w = 113, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2457, .adv_w = 108, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2468, .adv_w = 113, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 2484, .adv_w = 110, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2496, .adv_w = 110, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2508, .adv_w = 110, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2521, .adv_w = 110, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2533, .adv_w = 95, .box_w = 6, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 2553, .adv_w = 113, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 2572, .adv_w = 95, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 2591, .adv_w = 110, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2608, .adv_w = 144, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2626, .adv_w = 98, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2637, .adv_w = 119, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2656, .adv_w = 102, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2672, .adv_w = 52, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2675, .adv_w = 52, .box_w = 5, .box_h = 12, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 2683, .adv_w = 96, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2690, .adv_w = 200, .box_w = 13, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2714, .adv_w = 197, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2732, .adv_w = 144, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2746, .adv_w = 121, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2767, .adv_w = 140, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2788, .adv_w = 113, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2809, .adv_w = 134, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 2820, .adv_w = 127, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2838, .adv_w = 119, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2851, .adv_w = 123, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2868, .adv_w = 98, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2874, .adv_w = 139, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 2894, .adv_w = 110, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2907, .adv_w = 174, .box_w = 11, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2933, .adv_w = 110, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 2949, .adv_w = 140, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2964, .adv_w = 140, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2984, .adv_w = 121, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3000, .adv_w = 136, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3014, .adv_w = 167, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3036, .adv_w = 135, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3048, .adv_w = 149, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3068, .adv_w = 134, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3075, .adv_w = 117, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3088, .adv_w = 119, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3106, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3113, .adv_w = 113, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3129, .adv_w = 166, .box_w = 10, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3153, .adv_w = 121, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3172, .adv_w = 138, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 3183, .adv_w = 121, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3197, .adv_w = 189, .box_w = 10, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3208, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 3223, .adv_w = 139, .box_w = 9, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3239, .adv_w = 164, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3256, .adv_w = 118, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3268, .adv_w = 119, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3284, .adv_w = 198, .box_w = 11, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 3308, .adv_w = 122, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3324, .adv_w = 100, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3335, .adv_w = 114, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3354, .adv_w = 105, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3365, .adv_w = 80, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3371, .adv_w = 116, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3385, .adv_w = 107, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3398, .adv_w = 146, .box_w = 9, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3415, .adv_w = 94, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3426, .adv_w = 115, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3438, .adv_w = 115, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3455, .adv_w = 103, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3467, .adv_w = 115, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 3479, .adv_w = 140, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3493, .adv_w = 113, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3503, .adv_w = 113, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3516, .adv_w = 111, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3522, .adv_w = 113, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3537, .adv_w = 89, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3547, .adv_w = 89, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3553, .adv_w = 95, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3568, .adv_w = 147, .box_w = 9, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3592, .adv_w = 98, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3603, .adv_w = 113, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3612, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3622, .adv_w = 158, .box_w = 9, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3630, .adv_w = 160, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3642, .adv_w = 115, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3654, .adv_w = 146, .box_w = 9, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3668, .adv_w = 101, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3678, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3690, .adv_w = 156, .box_w = 10, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3706, .adv_w = 105, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3717, .adv_w = 107, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3735, .adv_w = 107, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3752, .adv_w = 110, .box_w = 6, .box_h = 13, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3768, .adv_w = 80, .box_w = 5, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3778, .adv_w = 96, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3790, .adv_w = 86, .box_w = 5, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3800, .adv_w = 49, .box_w = 3, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3806, .adv_w = 49, .box_w = 5, .box_h = 10, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 3814, .adv_w = 49, .box_w = 4, .box_h = 12, .ofs_x = -1, .ofs_y = -2},
    {.bitmap_index = 3823, .adv_w = 166, .box_w = 10, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3839, .adv_w = 163, .box_w = 10, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3854, .adv_w = 110, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3867, .adv_w = 103, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3885, .adv_w = 115, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 3902, .adv_w = 95, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3922, .adv_w = 110, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3931, .adv_w = 95, .box_w = 7, .box_h = 2, .ofs_x = -1, .ofs_y = 3},
    {.bitmap_index = 3934, .adv_w = 191, .box_w = 13, .box_h = 2, .ofs_x = -1, .ofs_y = 3},
    {.bitmap_index = 3940, .adv_w = 191, .box_w = 13, .box_h = 2, .ofs_x = -1, .ofs_y = 3},
    {.bitmap_index = 3946, .adv_w = 45, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 3949, .adv_w = 45, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 3952, .adv_w = 45, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3955, .adv_w = 80, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 3960, .adv_w = 80, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 3965, .adv_w = 80, .box_w = 5, .box_h = 4, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 3970, .adv_w = 91, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 3982, .adv_w = 91, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 3998, .adv_w = 69, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 4003, .adv_w = 56, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 4011, .adv_w = 56, .box_w = 4, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 4019, .adv_w = 108, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_3[] = {
    0x0, 0x1, 0x2, 0x5, 0x6, 0x7, 0x9, 0xa,
    0xb, 0xd, 0xe, 0xf, 0x26, 0x27, 0x99
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 160, .range_length = 96, .glyph_id_start = 96,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 1025, .range_length = 95, .glyph_id_start = 192,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 8211, .range_length = 154, .glyph_id_start = 287,
        .unicode_list = unicode_list_3, .glyph_id_ofs_list = NULL, .list_length = 15, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
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
    48, 49, 0, 50, 51, 0, 52, 0,
    0, 53, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 54, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 55, 0, 0, 0, 56,
    10, 10, 10, 10, 10, 10, 14, 12,
    14, 14, 14, 14, 0, 0, 0, 0,
    13, 0, 21, 21, 21, 21, 21, 0,
    21, 26, 26, 26, 26, 30, 57, 58,
    34, 34, 34, 34, 34, 34, 0, 36,
    0, 0, 0, 0, 0, 0, 0, 0,
    59, 41, 42, 42, 42, 42, 42, 0,
    42, 0, 0, 0, 0, 0, 60, 0,
    61, 62, 63, 64, 65, 0, 0, 0,
    0, 0, 66, 67, 0, 68, 0, 69,
    62, 70, 63, 71, 61, 0, 72, 0,
    0, 67, 0, 73, 0, 74, 0, 75,
    64, 76, 68, 77, 78, 79, 0, 0,
    79, 0, 0, 0, 80, 74, 0, 81,
    82, 83, 84, 85, 86, 87, 88, 0,
    0, 89, 0, 0, 0, 90, 0, 91,
    92, 93, 94, 95, 96, 97, 0, 0,
    97, 98, 0, 98, 99, 90, 0, 86,
    86, 100, 84, 101, 102, 0, 0, 0,
    98, 98, 103, 89, 0, 94, 0, 6,
    6, 0, 104, 0, 105, 104, 0, 105,
    0, 0, 0, 54, 55, 0
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
    17, 0, 18, 0, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 0, 28, 0,
    0, 0, 29, 0, 30, 31, 32, 33,
    34, 0, 0, 35, 0, 0, 36, 36,
    37, 36, 38, 36, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 0, 48, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 49, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 50, 0, 0, 0, 0,
    12, 12, 12, 12, 12, 12, 51, 13,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 17, 17, 17, 17, 17, 0,
    17, 21, 21, 21, 21, 25, 0, 0,
    29, 29, 29, 29, 29, 29, 29, 30,
    32, 32, 32, 32, 52, 0, 53, 54,
    55, 36, 37, 37, 37, 37, 37, 0,
    37, 41, 41, 41, 41, 45, 0, 45,
    0, 56, 0, 57, 58, 0, 0, 59,
    60, 0, 56, 0, 0, 61, 0, 62,
    0, 0, 0, 63, 0, 64, 65, 0,
    0, 0, 60, 66, 0, 57, 0, 0,
    57, 56, 61, 67, 68, 0, 69, 0,
    0, 56, 0, 0, 70, 0, 71, 72,
    73, 74, 75, 76, 77, 78, 79, 80,
    80, 80, 81, 82, 75, 83, 75, 84,
    77, 85, 86, 87, 88, 75, 89, 75,
    75, 90, 75, 75, 91, 92, 93, 77,
    77, 94, 75, 95, 96, 97, 98, 99,
    81, 75, 100, 80, 80, 86, 75, 7,
    7, 0, 101, 102, 103, 101, 102, 103,
    0, 0, 0, 49, 50, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -15, -5, -5, -24, 0,
    -5, -5, 0, 0, 0, 2, 2, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -26, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 15, 0, 0, 0, 0, 0,
    0, 0, 0, -6, -6, 0, 0, -6,
    -6, -3, 2, -2, 3, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, -2, -2, -2, 0, -2, -2,
    -2, -5, 0, -7, -3, -5, -8, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -12, -2, -2, -23, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -19,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 5,
    -6, -6, 5, 0, -6, -6, 3, -13,
    -4, -15, -8, 2, -15, 4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, 0, 0, -10, 0,
    -2, 0, -6, -10, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -10, -5, -5,
    -14, 0, -5, -5, 0, 6, 0, 6,
    4, 4, 7, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -15, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, -2, 0,
    0, -2, -2, 0, -16, -2, -4, -2,
    0, -8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, -2, -2, -5, 0,
    -2, -2, -3, -8, -2, -7, -4, -7,
    -10, -7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -15, -4, 0, -12, 5, 0, 3, 0,
    0, -3, 6, -4, -4, 6, 0, -4,
    -4, 3, -12, -2, -12, -3, 5, -15,
    4, 0, -5, 0, -2, -2, -2, 0,
    -3, 0, 0, -2, -2, 2, 0, -3,
    -4, -4, 5, -3, 3, -4, -2, -4,
    0, 7, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -14, -13, 5, 0, -2,
    -2, -5, -2, -2, 0, -4, -2, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, -2, -4, -5, 0,
    0, -7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -4, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 0, 4, -6, 0, 0, 3, -3,
    4, -4, -4, 4, 0, -4, -4, 2,
    3, 0, 3, 0, 2, 3, 2, -2,
    0, 0, -3, -3, -3, 0, -3, 0,
    0, -3, -3, 0, 0, -4, -3, -3,
    3, -3, 0, -5, 0, -9, 0, 4,
    0, 0, 0, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 3, -3, -5, -2, -6,
    0, -6, 0, -8, 0, -7, -2, -4,
    0, 0, -5, 0, 0, 0, -2, -5,
    0, -3, -3, -5, -8, -3, -2, -8,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, -8, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, -2, -7, 0, -3, -4, 0, -2,
    2, 0, 0, 0, 0, -3, 3, -5,
    -5, 3, 0, -5, -5, 0, 0, -4,
    0, 0, 0, 0, 0, -3, -8, 0,
    -3, -3, -3, 0, -4, 0, 0, -3,
    -3, 0, -3, -4, -5, -5, 2, -5,
    0, -4, -4, -4, 0, 3, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, -2, 0, -5, 0, 0, -12,
    0, -12, -3, 2, -3, -9, -2, -2,
    -15, 0, -2, -2, 0, 3, 0, 3,
    0, 0, 4, 0, -3, -8, -10, -3,
    -3, -3, 0, -3, 0, -5, -3, -3,
    0, 0, -5, 0, 0, -7, 0, -5,
    0, 0, 0, -4, -15, 0, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -20, 0, 0, 0, -2, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, -3, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, -2, 0, -7,
    0, 0, 0, -2, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, -4,
    0, 0, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    0, -3, 0, -6, 3, -9, 3, 0,
    2, -4, 7, -7, -7, 5, 0, -7,
    -7, 0, 3, 0, 4, 2, 5, 4,
    4, 0, -5, 0, -4, -4, -4, 0,
    -4, 0, 0, -4, -4, 3, 0, -2,
    -3, -9, 5, -2, 3, -4, 0, -12,
    0, 7, 0, 0, 4, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 3, 0, -24,
    -2, 0, -27, 4, -13, 2, 0, 0,
    0, 5, -9, -9, 5, 0, -9, -9,
    3, -22, -3, -19, -9, 4, -22, 3,
    0, -6, 0, -2, -2, -2, 0, -2,
    0, 0, -2, -2, 0, 0, 0, -8,
    -7, 4, -4, 2, -5, -3, -7, 2,
    6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -23, -23, 3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -1, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, -4, -2, -5,
    0, -6, 0, -8, 0, -7, -2, -4,
    0, 0, -5, 0, 0, 0, -2, -5,
    0, -3, -3, -5, -8, -3, -2, -8,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, -8, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, -2, -6, -2, 0, -2, -5, 0,
    -17, 0, -12, 0, 0, 0, -10, 0,
    0, -18, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, -3, -8, -4,
    -4, -4, -6, 0, -4, 0, 0, -4,
    -2, 0, 0, 0, 2, 2, 0, 2,
    0, 0, -5, -3, 0, -16, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, -22, -3, -4, -2, 0, 0, -6,
    0, 0, 0, -7, -2, -4, 0, 0,
    -5, 0, 0, 0, -2, -5, 0, -3,
    -3, -5, -8, -3, -2, 0, -3, 0,
    0, 0, 0, 0, 3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, -2,
    -4, 0, 0, -2, 0, 0, 3, 0,
    2, 0, -2, -3, 5, -2, -2, 4,
    0, -2, -2, 0, 0, 0, -2, 0,
    4, -4, 3, 0, -6, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 4, 0, 2, -3,
    -3, -4, 0, 6, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, -5, 2, 0, -14, -9, -17,
    -16, 4, -10, -12, -5, -5, -17, 0,
    -5, -5, 0, 5, 0, 5, 3, 3,
    6, 0, 0, -6, -9, -11, -11, -11,
    0, -11, 0, -8, -11, -11, -9, 0,
    -7, -4, -16, -3, -4, -6, -5, 0,
    -16, -15, -14, 0, 4, 6, -15, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -13, 0,
    0, -2, 0, 0, -2, 0, -8, -2,
    0, 0, -2, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, -7, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, -2, -4, 0,
    0, -7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, -14, 0, 0, 0, 0,
    0, -12, -3, -3, -18, -1, -3, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, -9, -9, -9, 0, -9,
    0, -6, -9, -9, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, -3,
    0, 0, -7, 0, -9, -2, 3, -4,
    -3, -3, -3, -12, 0, -3, -3, 0,
    4, 0, 4, 2, 2, 5, 0, 0,
    -7, -6, -4, -4, -4, 0, -5, 0,
    -4, -4, -4, -3, 0, -4, 0, 0,
    0, 0, 0, -3, 0, -4, 0, -9,
    0, 0, 3, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 5, -7, 0, 0, -5, 0,
    -5, 2, -5, 2, 0, 0, -5, 5,
    -5, -5, 4, 0, -5, -5, 0, 2,
    0, 3, 0, 3, 4, 2, 0, -7,
    0, -3, -3, -3, 0, -3, 0, 0,
    -3, -3, 0, 0, -2, -2, -7, 4,
    -1, 0, -5, 0, -9, 0, 5, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 2, 0, 0, -8, 3, 0,
    -12, -9, -16, -7, 5, -11, -15, -8,
    -8, -22, -2, -8, -8, 0, 6, 0,
    6, 4, 4, 7, 2, 0, -5, -8,
    -9, -9, -9, 0, -9, 0, -8, -9,
    -9, -4, 0, -7, 0, 0, -3, 0,
    -5, -4, 0, -15, -6, -15, 0, 0,
    8, -15, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 2,
    3, -15, 0, 0, -4, 0, 0, 2,
    -12, 0, 0, 0, -3, 3, -7, -7,
    3, 0, -7, -7, 0, 0, -3, 2,
    0, 0, 2, 0, -3, -8, 0, -3,
    -3, -3, 0, -4, 0, 0, -3, -3,
    0, -2, -4, -4, -5, 2, -4, 0,
    -7, -3, -10, 0, 3, 0, 0, 2,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, -8, -8, -5,
    0, -8, -8, -5, -6, -7, -6, -7,
    -7, -5, -8, 0, 15, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, -2, 0, 0,
    -2, -2, 0, 0, 0, 0, -2, -2,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, -5, -4, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, -2, 0, -3, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 0, -7,
    0, -5, 0, 0, 0, -4, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    -2, -3, -2, -4, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, -6, 0, 0, 0, 0,
    0, 0, 2, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, -2, 0, 0, -3, 0,
    0, -2, 0, 0, 0, 0, 2, 2,
    3, 2, 0, -4, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 7,
    2, -10, -7, -8, 0, 5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 7,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 4, 3, 3,
    4, 0, 0, 7, -4, 2, 0, 6,
    0, 8, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 4, -10, 0, -7, 0, -5, -4,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -6, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, -2, 0, -3,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    2, 0, 0, -4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, -5, -5,
    -5, 0, -6, 0, 0, -6, -5, 0,
    0, 0, 0, 0, 4, 0, 0, -4,
    0, -7, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, -5, -4, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, -3, 0, -3, 0, 0, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -4, -5, 0, 0,
    -6, 0, -5, -4, 0, 0, -4, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, -2, -3, -3, -4, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, -5, 0, 0, -6,
    0, -5, -4, 0, 0, -4, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    -2, -3, -3, -3, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, -11, -6, -9, 0, -8, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 4, 4, 3,
    4, 0, 0, 0, -4, 0, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, -10, 0, 0, 0, 0, 0,
    2, -7, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -5, 0,
    -2, -3, -2, 0, -2, 0, 0, -3,
    -2, 0, 0, 0, 0, 0, 3, 0,
    0, -4, 0, -5, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, -7,
    0, -6, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, -3,
    -3, -3, 0, -2, 0, 0, -3, -3,
    0, 0, 0, 3, 3, 3, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 2,
    -7, 0, 0, 0, 0, 2, -6, 0,
    -5, 0, -7, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, -2, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 3, 3, 3, 3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, -4, -4, -4,
    0, -4, 0, 0, -4, -4, 0, 0,
    0, 0, 0, 3, 0, 0, -4, 0,
    -6, 0, 0, 0, 0, 0, -5, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, 0, -4, -4, -4, -2,
    -4, 0, 0, -4, -3, 0, -2, -2,
    0, 0, 0, 0, 0, -5, -4, -6,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -5, -6, -2, 0, -6, -6,
    -4, 0, -4, 0, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 15, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, -4, 0, 0, 0, -3,
    -5, -2, -5, -4, -6, -5, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -16,
    -2, -6, -4, 0, -9, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -15, 0,
    -2, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    -8, 0, 0, 0, -4, -17, 0, -8,
    -4, -10, -15, -8, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -9, -7, -7, -12,
    0, -7, -7, -6, -21, -8, -9, -7,
    -13, -13, -13, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -14, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, -6, 0, -7, 0, -12, 0, -8,
    0, -7, 0, -5, 0, 0, -11, 0,
    0, 0, 0, -8, 0, -4, 0, -8,
    -8, -9, -3, -8, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, 0, -9, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, -3, -11, 0,
    -8, 0, -5, -6, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, -5, 0, -5, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, -6, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -8, 0,
    -5, -4, 0, 0, -4, 0, -5, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, -2,
    -3, -3, -3, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -7, -7, 0, 0, -3, -4, 0,
    -2, 2, 0, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, -8,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, -4, -4, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 3, 0,
    0, 3, 0, 0, 0, 0, -5, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    -3, 0, 0, 0, 0, 0, -3, 0,
    -3, -5, -3, 2, -3, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, -2, -7, 0, -3, -6,
    -2, 3, -3, -2, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, -8, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, -4, 0, 0, 0, -3, -4,
    0, -4, 0, 0, 0, 0, 0, 0,
    -3, -2, 0, 0, 0, 0, 0, -7,
    -3, 0, -3, 0, -7, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 2, 0, -14,
    -9, -17, -16, 4, -10, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, -16, -15, 0, 0, 0, 0,
    0, 5, -5, 0, -21, -12, 4, -12,
    -12, 0, 0, 0, -5, 3, 0, 3,
    0, -19, 0, -17, -18, -22, -22, -14,
    -16, -18, -22, -20, -22, -18, -12, -12,
    -15, -13, -15, -12, -15, -17, -21, 5,
    -22, -20, 0, 8, 0, 5, 0, 0,
    -13, 0, 0, -4, 0, 0, 4, -6,
    0, 0, 3, -3, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, -9, 0, 0, 0, 0, 0, 0,
    3, -4, 0, 4, 3, 0, 4, 3,
    0, 0, 0, -4, 2, 0, 0, 0,
    0, -3, 0, 0, 3, -3, 3, 0,
    0, 4, 0, -3, 0, -6, -3, -3,
    3, -9, -6, 0, 0, 0, 4, 0,
    0, 0, 0, 0, 4, 0, 2, 3,
    0, -2, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -9, -3, 0, 0,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    -14, -4, -4, -7, 0, 0, 0, 0,
    -6, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, -10, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -7, 0,
    0, 0, 0, 0, 0, 0, -10, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -6, 0, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, -4, 0, 0, 0,
    -7, 0, 0, 0, -3, 0, 0, -4,
    -4, -4, -3, 0, 0, 0, 0, 0,
    -3, 0, -6, 3, -9, 3, 0, 2,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -4, 0, -12, 0,
    0, 0, 0, 0, 0, 3, -7, 0,
    5, 5, 0, 7, 5, 4, 3, 0,
    -7, 5, 0, 4, 3, 0, -5, 0,
    0, 5, -4, 5, 0, 0, 5, 0,
    -4, 0, -8, -9, -5, 5, -12, -4,
    4, 0, 4, 4, -4, 3, 0, 0,
    0, 4, 0, 0, 3, 0, 3, -6,
    3, 0, -14, -2, -13, -4, 5, -7,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, -8, -2, 0,
    0, 0, 0, 0, 5, -3, 0, -16,
    -12, 5, -12, -12, 3, 0, -1, -3,
    4, 0, 4, 0, -5, -3, -3, -4,
    -12, -7, 0, 0, -4, -12, -6, -7,
    -4, 0, 0, -8, 0, 0, 0, 0,
    -3, -6, 6, -7, -4, 0, 8, 0,
    6, 4, 5, -14, 0, -15, -4, 0,
    -12, 5, 0, 3, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, -2, -4, 0, 0, 0,
    0, 0, 0, -12, -4, 3, 5, 5,
    0, 6, 5, 4, 3, 0, -4, 5,
    -10, 4, 3, 0, -3, 0, 0, 5,
    -2, 5, 0, 0, 5, 0, -2, 0,
    -7, -3, -2, 5, -5, -5, 3, 0,
    4, 0, 0, 3, 0, 0, 0, 0,
    -14, -13, 5, 0, -2, -2, -5, -2,
    -2, 0, -4, -2, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, -4, 0, 0, -2, -4, -3,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, -2, 0, 5, -3, 3,
    0, 8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 5, -3, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 5, 4, 0, 5,
    4, 3, 3, 0, -5, 3, -5, 3,
    0, 0, 0, 0, 0, 4, 0, 4,
    0, 0, 4, 0, 0, 0, 0, 6,
    0, 4, -4, 0, 0, 0, 3, 0,
    0, 0, 0, 0, 12, 0, 0, 0,
    0, 0, -2, -2, -5, 0, 0, 0,
    -3, -2, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, -7, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    -3, 0, 0, -2, -3, -3, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, -3, -3, -4, -3, 0, 0, 0,
    0, -2, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -3,
    -4, -2, -5, 0, -6, 3, -8, 0,
    -7, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -8, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -6, 0,
    0, 0, 0, 0, 0, 0, -5, 0,
    0, -4, -4, -7, -4, -4, -9, -6,
    0, 0, -5, 0, -6, 0, -2, 0,
    0, 0, -5, 0, -2, -2, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, 0, 0, -2,
    -2, -3, 0, -3, -2, -6, -2, 0,
    -2, -5, 0, -17, 0, -12, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, -8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -18, -14, 0, -10, -13, -5, -3, 0,
    0, -3, 0, 0, 0, -4, 0, 0,
    0, -9, 0, 0, 0, 0, -9, -2,
    0, 0, 0, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 2, -22, 0, 0, -5,
    2, 0, -14, -9, -17, -16, 4, -10,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, -16, -15, 0,
    0, 0, 0, 0, 5, -5, 0, 0,
    0, 0, -12, 0, 0, 0, 0, -5,
    3, 0, 0, 0, -9, 0, -14, 0,
    -13, -11, -13, -16, -14, -13, -15, -11,
    -8, 0, -4, -11, -3, 0, 0, 0,
    0, 0, 5, 0, 0, 0, 8, 0,
    5, 0, 0, -13, -3, -4, -2, -5,
    0, -6, 0, -8, 0, -7, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, -8,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, -11, -11,
    -7, -4, -11, -12, -9, 0, 0, -5,
    0, -8, 0, -3, 0, -3, -2, -8,
    0, -2, -2, -2, -8, -3, 0, -3,
    0, 0, 0, 0, 0, 0, 0, -3,
    0, -3, 0, 0, -3, -3, -3, -3,
    -3, -2, -6, 0, 0, -5, 0, -5,
    2, -5, 2, 0, 0, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, -9, 0, 0, 0, 0,
    0, 0, 2, -5, 0, 0, 0, 0,
    5, 0, 0, 0, 0, -5, 3, 0,
    0, 0, 0, -5, 0, 0, 4, -3,
    4, 0, 0, 4, 0, -3, 0, -6,
    -1, -3, 4, -8, -6, 0, 0, 0,
    3, -4, 0, 0, 0, 0, 3, 0,
    0, 2, 0, -2, 0, 5, -3, 3,
    0, 8, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 5, -3, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 5, 4, 0, 5,
    4, 3, 3, 0, -5, 3, -5, 3,
    0, 0, 0, 0, 0, 4, 0, 4,
    0, 0, 4, 0, 0, 0, 0, 6,
    0, 4, -4, 0, 0, 0, 3, 0,
    0, 0, 0, 0, 13, 0, 0, 0,
    0, -3, -4, -2, -5, 0, -6, 3,
    -8, 0, -7, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -2, -8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, -4, -4, -7, -4, -4,
    -9, -6, 0, 0, -5, 0, -5, 0,
    0, 0, 0, 0, -4, 0, 0, -2,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -2, -3, 0, -3, -2, -6,
    0, -5, 0, -5, -4, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, -2,
    -4, -2, 0, -4, -4, 3, -4, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -4, -3, 0, 0,
    0, 0, 0, 0, 0, 0, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -7,
    0, -4, -3, 0, 0, 0, 0, -4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -5, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -3, 0, -15, -11, -9, 0, -10, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -3, -2, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -8, 0, 0, 0, 0, -7, 0, 0,
    0, 4, 4, 0, 3, 0, 4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, 5,
    0, 0, 0, 7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 4,
    0, 4, 0, 0, 4, 0, 0, 0,
    0, 5, -2, 4, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, -5, -3,
    -2, 3, -2, -2, -6, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, -6, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, -2, -3, 0, 0, 0,
    -5, 2, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, -7, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 4, -5, 4,
    0, 0, 4, 0, -6, 0, 0, 0,
    -6, 4, 0, 0, 0, 0, 3, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, 0, -5, -3, 0, 0,
    0, 0, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -6, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, -5, 2,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -4, 0,
    -7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 4, -5, 4, 0, 0,
    4, 0, -6, 0, 0, 0, -6, 4,
    0, 0, 0, 0, 3, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -6, 0, -5, -4, -4, 3, -4, -3,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -4, -3, 0, 0,
    0, 0, 0, 0, -3, 0, -3, 0,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, -5, 0, 0, -6,
    -2, -5, -4, -4, 3, -4, -3, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -4, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, -3, 0, 0, 0,
    0, 0, 0, -3, 0, -3, 0, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, -4, -5, 0, 0, 0, 0,
    0, 0, 2, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, -2, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    4, 0, 4, 0, 0, 4, 0, -2,
    0, 3, 2, -2, 3, 0, 3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, -10, -6, -9, 0, -9, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, -2, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -7,
    0, 0, 0, 0, -7, 0, 0, 0,
    4, 4, 0, 3, 0, 4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    -4, 0, -6, 0, -6, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -5, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -5, -2,
    0, 0, 0, -5, 0, -2, 0, 4,
    3, -2, 2, 0, 4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, -6, -4, -6, 0, -5, -4, -5,
    3, -4, -4, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, -6, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    -4, 0, -2, 0, 0, 0, 0, -3,
    0, -3, 0, 0, -3, 0, 0, -2,
    0, 0, 0, 0, 0, 0, -4, -5,
    0, 0, 0, -3, 0, 0, 0, -3,
    0, 0, -2, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -5, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    -2, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 4, -4, 4, 0,
    0, 4, 0, -4, 0, 0, 0, -4,
    3, 0, 0, 0, 0, 0, 0, -3,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 5, 0, 0, 0, 7,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 4, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 4,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 4, 0, 4, 0, 0,
    4, 0, 0, 0, 0, 5, -2, 4,
    -2, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 16, 0, 0, 0, 0, 0,
    -16, 0, -5, -10, 0, 0, 0, 0,
    -3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -9, -4, 0, 0, 0,
    -7, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    0, -5, -4, 0, 3, -4, 0, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4, 0, 0, 0, 0,
    0, 0, 0, -3, 0, -3, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, -5, 0, 0, -11, -4,
    0, -5, 0, 0, 0, 0, -6, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -2, -4, 0,
    0, 0, -3, 0, 0, -3, -2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -2, -7,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -4, -4, -6, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, 0, -2, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -4, 0, -5, 0,
    0, 0, 0, 0, -4, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -2, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -11, -3, -4, -5, 0,
    0, 0, 0, -6, -2, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -2, -8, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -6, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -14, -4, -4, -22,
    0, -4, -4, 0, 0, 0, 4, 3,
    0, 2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -25, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 5, -5, -5, 5, 0,
    -5, -6, 3, -13, -4, -14, -7, 2,
    -15, 4, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 105,
    .right_class_cnt     = 103,
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
    .cmap_num = 4,
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
    .line_height = 16,          /*The maximum line height required by the font*/
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

#endif // USE_EXTENDED_CHARSET

