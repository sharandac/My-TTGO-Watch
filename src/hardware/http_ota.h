#ifndef __HTTP_OTA_H
    #define __HTTP_OTA_H

    #include "callback.h"

    #define HTTP_OTA_START          _BV(0)
    #define HTTP_OTA_FINISH         _BV(1)
    #define HTTP_OTA_ERROR          _BV(2)
    #define HTTP_OTA_PROGRESS       _BV(3)
	
    bool http_ota_start( const char* url, const char* md5 );
    bool http_ota_register_cb( EventBits_t event, CALLBACK_FUNC callback_func, const char *id );

#endif /* __HTTP_OTA_H */