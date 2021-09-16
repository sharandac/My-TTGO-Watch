#ifndef _LOGGING_H
    #define _LOGGING_H

    #ifdef NATIVE_64BIT
        #include <stdio.h>
        #include <stdlib.h>

        #define LOG( format, ... )      printf( format "\r\n", ##__VA_ARGS__)

        #define log_i( format, ...)      printf( "[I]%s (line:%d) " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
        #define log_e( format, ...)      printf( "[E]%s (line:%d) " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)                
        #define log_d( format, ...)      printf( "[D]%s (line:%d) " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)                
        #define LOG_W( format, ...)      printf( "[W]%s (line:%d) " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)                
        #define LOG_V( format, ...)      printf( "[V]%s (line:%d) " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)                

        #define log_i( format, ...)      printf( "[I]%s (line:%d) " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)
        #define log_e( format, ...)      printf( "[E]%s (line:%d) " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)                
        #define log_d( format, ...)      printf( "[D]%s (line:%d) " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)                
        #define log_w( format, ...)      printf( "[W]%s (line:%d) " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)                
        #define log_v( format, ...)      printf( "[V]%s (line:%d) " format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)                
    #endif
    
#endif // _LOGGING_H