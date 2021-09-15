/**
 * @file lv_png.h
 *
 */

#ifndef LV_PNG_H
#define LV_PNG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register the PNG decoder functions in LittlevGL
 */
void lv_png_init(void);
void lv_rgb_as_png( const char* filename, const unsigned char* image, unsigned int w, unsigned int h );
void lv_rgba_as_png( const char* filename, const unsigned char* image, unsigned int w, unsigned int h );
void lv_8grey_as_png( const char* filename, const unsigned char* image, unsigned int w, unsigned int h );
void lv_4grey_as_png( const char* filename, const unsigned char* image, unsigned int w, unsigned int h );

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_PNG_H*/
