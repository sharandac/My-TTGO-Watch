/****************************************************************************
 *   Aug 3 22:21:17 2020
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
#include "driver/i2s.h"

#include "sound.h"
#include "gui/sound/snd_start.h"
#include "gui/sound/snd_signal.h"

TaskHandle_t _sound_Task;
void sound_Task( void * pvParameters );

void sound_setup( void ) {

    esp_err_t err;

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 8000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 64   //Interrupt level 1
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = TWATCH_DAC_IIS_BCK, //this is BCK pin
        .ws_io_num = TWATCH_DAC_IIS_WS, // this is LRCK pin
        .data_out_num = TWATCH_DAC_IIS_DOUT, // this is DATA output pin
        .data_in_num = -1   //Not used
    };

    err = i2s_driver_install( I2S_PORT, &i2s_config, 0, NULL);
    if (err != ESP_OK) { 
        log_e("Failed installing driver: %d\r\n", err );
        while (true);
    }

    err = i2s_set_pin( I2S_PORT, &pin_config);
    if (err != ESP_OK) { 
        log_e("Failed settings pin config: %d\r\n", err );
        while (true);
    }

    xTaskCreate(    sound_Task,      /* Function to implement the task */
                    "sound Task",    /* Name of the task */
                    2000,              /* Stack size in words */
                    NULL,               /* Task input parameter */
                    1,                  /* Priority of the task */
                    &_sound_Task );  /* Task handle. */

}

void sound_Task( void * pvParameters ) {
    esp_err_t err;
    size_t written = 0;
    
    err = i2s_write( I2S_PORT, start_raw, sizeof(start_raw), &written, portMAX_DELAY );
    if (err != ESP_OK) { 
        log_e("Failed write bytes: %d\r\n", err );
    }
    i2s_stop( I2S_PORT );
    vTaskDelete( NULL );
}