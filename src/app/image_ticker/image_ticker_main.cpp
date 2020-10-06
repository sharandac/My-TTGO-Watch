/****************************************************************************
 *   Aug 3 12:17:11 2020
 *   Copyright  2020  Dirk Brosswick
 *   Email: dirk.brosswick@googlemail.com
 ****************************************************************************/
 
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "config.h"
#include <TTGO.h>

#include "image_ticker.h"
#include "image_ticker_main.h"
#include "image_ticker_fetch.h"

#include "gui/mainbar/app_tile/app_tile.h"
#include "gui/mainbar/main_tile/main_tile.h"
#include "gui/mainbar/mainbar.h"
#include "gui/statusbar.h"


#include "lodepng.h"
#include "hardware/wifictl.h"


lv_obj_t *image_ticker_main_tile = NULL;
lv_style_t image_ticker_main_style;

lv_task_t * _image_ticker_task;

LV_IMG_DECLARE(exit_32px);
LV_IMG_DECLARE(setup_32px);
LV_IMG_DECLARE(refresh_32px);
LV_FONT_DECLARE(Ubuntu_72px);

static void exit_image_ticker_main_event_cb( lv_obj_t * obj, lv_event_t event );
static void enter_image_ticker_setup_event_cb( lv_obj_t * obj, lv_event_t event );
void image_ticker_task( lv_task_t * task );

void lv_png_init(void);
static lv_res_t decoder_info(struct _lv_img_decoder * decoder, const void * src, lv_img_header_t * header);
static lv_res_t decoder_open(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);
static void decoder_close(lv_img_decoder_t * dec, lv_img_decoder_dsc_t * dsc);
static void convert_color_depth(uint8_t * img, uint32_t px_cnt);

void image_ticker_main_setup( uint32_t tile_num ) {

    image_ticker_main_tile = mainbar_get_tile_obj( tile_num );
    lv_style_copy( &image_ticker_main_style, mainbar_get_style() );



    /*Call once after lv_init()*/
    lv_png_init();

    /*Create an image obejct and use the converter png file*/
    log_i("lv_img_create..");
    lv_obj_t * img_obj = lv_img_create(image_ticker_main_tile, NULL);
    //lv_style_copy( &image_ticker_main_style, mainbar_get_style() );
    lv_img_set_src(img_obj, "/imgtick.png");
    lv_obj_align(img_obj, image_ticker_main_tile, LV_ALIGN_IN_TOP_MID, 0, 0 );
    log_i("lv_img_created");

    lv_obj_t * exit_btn = lv_imgbtn_create( image_ticker_main_tile, NULL);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_PRESSED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_RELEASED, &exit_32px);
    lv_imgbtn_set_src(exit_btn, LV_BTN_STATE_CHECKED_PRESSED, &exit_32px);
    lv_obj_add_style(exit_btn, LV_IMGBTN_PART_MAIN, &image_ticker_main_style );
    lv_obj_align(exit_btn, image_ticker_main_tile, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10 );
    lv_obj_set_event_cb( exit_btn, exit_image_ticker_main_event_cb );

    lv_obj_t * setup_btn = lv_imgbtn_create( image_ticker_main_tile, NULL);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_PRESSED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_RELEASED, &setup_32px);
    lv_imgbtn_set_src(setup_btn, LV_BTN_STATE_CHECKED_PRESSED, &setup_32px);
    lv_obj_add_style(setup_btn, LV_IMGBTN_PART_MAIN, &image_ticker_main_style );
    lv_obj_align(setup_btn, image_ticker_main_tile, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10 );
    lv_obj_set_event_cb( setup_btn, enter_image_ticker_setup_event_cb );

    wifictl_register_cb( WIFICTL_CONNECT_IP | WIFICTL_OFF_REQUEST | WIFICTL_OFF | WIFICTL_DISCONNECT , image_ticker_wifictl_event_cb, "imageticker" );

    // create an task that runs every secound
    //_image_ticker_task = lv_task_create( image_ticker_task, 10000, LV_TASK_PRIO_MID, NULL );
}

static void enter_image_ticker_setup_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       statusbar_hide( true );
                                        mainbar_jump_to_tilenumber( image_ticker_get_app_setup_tile_num(), LV_ANIM_ON );
                                        break;
    }
}

static void exit_image_ticker_main_event_cb( lv_obj_t * obj, lv_event_t event ) {
    switch( event ) {
        case( LV_EVENT_CLICKED ):       mainbar_jump_to_maintile( LV_ANIM_OFF );
                                        break;
    }
}

void image_ticker_task( lv_task_t * task ) {
     //image_ticker_fetch_jpg();
}

bool image_ticker_wifictl_event_cb( EventBits_t event, void *arg ) {
    image_ticker_config_t *image_ticker_config = image_ticker_get_config();
    switch( event ) {
        case WIFICTL_CONNECT_IP:    if (image_ticker_config->autosync ) {
                                        image_ticker_fetch_jpg();
                                        }
                                    break;
        case WIFICTL_OFF_REQUEST:
                                    break;
        case WIFICTL_OFF:
                                    break;
        case WIFICTL_DISCONNECT: 
                                    break;
    }
    return( true );
}


 // Register the PNG decoder functions in LittlevGL
void lv_png_init(void)
{
    lv_img_decoder_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_info_cb(dec, decoder_info);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_close_cb(dec, decoder_close);
}


/**
 * Get info about a PNG image
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RES_OK: no error; LV_RES_INV: can't get the info
 */
static lv_res_t decoder_info(struct _lv_img_decoder * decoder, const void * src, lv_img_header_t * header)
{
    (void) decoder; /*Unused*/
     lv_img_src_t src_type = lv_img_src_get_type(src);          /*Get the source type*/

     /*If it's a PNG file...*/
     if(src_type == LV_IMG_SRC_FILE) {
         const char * fn = (char *)src;
         if(!strcmp(&fn[strlen(fn) - 3], "png")) {              /*Check the extension*/

             /* Read the width and height from the file. They have a constant location:
              * [16..23]: width
              * [24..27]: height
              */
            uint32_t size[2];
#ifdef LODEPNG_SPIFFS
            log_i("Opening image file %s", fn);
            fs::File file = SPIFFS.open(fn, "r");
            if(!file) return LV_RES_INV;
            file.seek(16);
            file.readBytes((char *)&size, 8);
            file.close();
#else
             FILE* file;
             file = fopen(fn, "rb" );
             if(!file) return LV_RES_INV;
             fseek(file, 16, SEEK_SET);
             size_t rn = fread(size, 1 , 8, file);
             fclose(file);
             if(rn != 8) return LV_RES_INV;
#endif
             /*Save the data in the header*/
             header->always_zero = 0;
             header->cf = LV_IMG_CF_RAW_ALPHA;
             /*The width and height are stored in Big endian format so convert them to little endian*/
             header->w = (lv_coord_t) ((size[0] & 0xff000000) >> 24) +  ((size[0] & 0x00ff0000) >> 8);
             header->h = (lv_coord_t) ((size[1] & 0xff000000) >> 24) +  ((size[1] & 0x00ff0000) >> 8);

             return LV_RES_OK;
         }
     }
     /*If it's a PNG file in a  C array...*/
     else if(src_type == LV_IMG_SRC_VARIABLE) {
         const lv_img_dsc_t * img_dsc = (lv_img_dsc_t *)src;
         header->always_zero = 0;
         header->cf = img_dsc->header.cf;       /*Save the color format*/
         header->w = img_dsc->header.w;         /*Save the color width*/
         header->h = img_dsc->header.h;         /*Save the color height*/
         return LV_RES_OK;
     }

     return LV_RES_INV;         /*If didn't succeeded earlier then it's an error*/
}

/**
 * Open a PNG image and return the decided image
 * @param src can be file name or pointer to a C array
 * @param style style of the image object (unused now but certain formats might use it)
 * @return pointer to the decoded image or  `LV_IMG_DECODER_OPEN_FAIL` if failed
 */
static lv_res_t decoder_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{

    (void) decoder; /*Unused*/
    uint32_t error;                 /*For the return values of PNG decoder functions*/

    uint8_t * img_data = NULL;

    /*If it's a PNG file...*/
    if(dsc->src_type == LV_IMG_SRC_FILE) {
        const char * fn = (char *)dsc->src;

        if(!strcmp(&fn[strlen(fn) - 3], "png")) {              /*Check the extension*/

            /*Load the PNG file into buffer. It's still compressed (not decoded)*/
            unsigned char * png_data;      /*Pointer to the loaded data. Same as the original file just loaded into the RAM*/
            size_t png_data_size;          /*Size of `png_data` in bytes*/

            error = lodepng_load_file(&png_data, &png_data_size, fn);   /*Load the file*/
            if(error) {
                printf("error %u: %s\n", error, lodepng_error_text(error));
                return LV_RES_INV;
            }

            /*Decode the PNG image*/
            uint32_t png_width;             /*Will be the width of the decoded image*/
            uint32_t png_height;            /*Will be the width of the decoded image*/

            /*Decode the loaded image in ARGB8888 */
            error = lodepng_decode32(&img_data, &png_width, &png_height, png_data, png_data_size);
            free(png_data); /*Free the loaded file*/
            if(error) {
                printf("error %u: %s\n", error, lodepng_error_text(error));
                return LV_RES_INV;
            }

            /*Convert the image to the system's color depth*/
            convert_color_depth(img_data,  png_width * png_height);
            dsc->img_data = img_data;
            return LV_RES_OK;     /*The image is fully decoded. Return with its pointer*/
        }
    }
    /*If it's a PNG file in a  C array...*/
    else if(dsc->src_type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = (lv_img_dsc_t *)dsc->src;
        uint32_t png_width;             /*No used, just required by he decoder*/
        uint32_t png_height;            /*No used, just required by he decoder*/

        /*Decode the image in ARGB8888 */
        error = lodepng_decode32(&img_data, &png_width, &png_height, img_dsc->data, img_dsc->data_size);

        if(error) {
            return LV_RES_INV;
        }

        /*Convert the image to the system's color depth*/
        convert_color_depth(img_data,  png_width * png_height);

        dsc->img_data = img_data;
        return LV_RES_OK;     /*Return with its pointer*/
    }

    return LV_RES_INV;    /*If not returned earlier then it failed*/
}

/**
 * Free the allocated resources
 */
static void decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    (void) decoder; /*Unused*/
    if(dsc->img_data) free((uint8_t *)dsc->img_data);
}

/**
 * If the display is not in 32 bit format (ARGB888) then covert the image to the current color depth
 * @param img the ARGB888 image
 * @param px_cnt number of pixels in `img`
 */
static void convert_color_depth(uint8_t * img, uint32_t px_cnt)
{
#if LV_COLOR_DEPTH == 32
    lv_color32_t * img_argb = (lv_color32_t*)img;
    lv_color_t c;
    lv_color_t * img_c = (lv_color_t *) img;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        c = LV_COLOR_MAKE(img_argb[i].ch.red, img_argb[i].ch.green, img_argb[i].ch.blue);
        img_c[i].ch.red = c.ch.blue;
        img_c[i].ch.blue = c.ch.red;
    }
#elif LV_COLOR_DEPTH == 16
    lv_color32_t * img_argb = (lv_color32_t*)img;
    lv_color_t c;
    uint32_t i;
    for(i = 0; i < px_cnt; i++) {
        c = LV_COLOR_MAKE(img_argb[i].ch.blue, img_argb[i].ch.green, img_argb[i].ch.red);
        img[i*3 + 2] = img_argb[i].ch.alpha;
        img[i*3 + 1] = c.full >> 8;
        img[i*3 + 0] = c.full & 0xFF;
    }
#elif LV_COLOR_DEPTH == 8
    lv_color32_t * img_argb = (lv_color32_t*)img;
       lv_color_t c;
       uint32_t i;
       for(i = 0; i < px_cnt; i++) {
           c = LV_COLOR_MAKE(img_argb[i].red, img_argb[i].green, img_argb[i].blue);
           img[i*3 + 1] = img_argb[i].alpha;
           img[i*3 + 0] = c.full
       }
#endif
}
