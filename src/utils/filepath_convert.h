#ifndef _FILEPATH_CONVERT_H
    #define _FILEPATH_CONVERT_H

    /**
     * @brief depend on build destination get local config path
     * 
     * @param   dst_str     pointer to destination string thats includes local path
     * @param   max_len     size of dst_str
     * @param   local_path  local device path to convert
     * 
     * @return  pointer to the destination path
     */
    char *filepath_convert( char * dst_str, int max_len, const char* local_path );

#endif // _FILEPATH_CONVERT_H