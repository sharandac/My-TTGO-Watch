#ifndef _DECOMPRESS_H
    #define _DECOMPRESS_H

    typedef void (*ProgressCallback)( uint8_t progress );

    bool decompress_file_into_spiffs( const char*filename, const char *dest, ProgressCallback cb );
    bool decompress_stream_into_flash( Stream *stream, const char* md5, int32_t firmwaresize, ProgressCallback cb );

#endif /* _DECOMPRESS_H */