#ifndef __HTTP_OTA_H
    #define __HTTP_OTA_H

    #include "callback.h"

    #define HTTP_OTA_START          _BV(0)      /** @brief http ota start event mask, callback arg is (char*) */
    #define HTTP_OTA_FINISH         _BV(1)      /** @brief http ota finish event mask, callback arg is (char*) */
    #define HTTP_OTA_ERROR          _BV(2)      /** @brief http ota error event mask, callback arg is (char*) */
    #define HTTP_OTA_PROGRESS       _BV(3)      /** @brief http ota progress event mask, callback arg is (int16_t*) */
	
    /**
     * @brief   start an http ota update
     * 
     * @param   url     pointer to an url
     * @param   md5     pointer to an md5 hash
     * 
     * @return  true if success or false if failed
     */
    bool http_ota_start( const char* url, const char* md5 );
    /**
     * @brief register an callback function for an http_ota event
     * 
     * @param   event           event mask
     * @param   callback_func   pointer to an callback function
     * @param   id              pointer to an id string
     * 
     * @return  true if registration succes or false if failed
     */
    bool http_ota_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );

#endif /* __HTTP_OTA_H */