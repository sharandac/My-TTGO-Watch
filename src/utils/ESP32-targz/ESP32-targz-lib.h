/*\

  MIT License

  Copyright (c) 2020-now tobozo

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ESP32-tgz is a wrapper to uzlib.h and untar.h third party libraries.
  Those libraries have been adapted and/or modified to fit this project's needs
  and are bundled with their initial license files.

  - uzlib: https://github.com/pfalcon/uzlib
  - untar: https://github.com/dsoprea/TinyUntar


  Tradeoffs :
    - speed: fast decompression needs 32Kb memory
    - memory: reducing memory use by dropping the gz_dictionary is VERY slow and prevents tar->gz->filesystem direct streaming
    - space: limited filesystems (<512KB spiffs) need tar->gz->filesystem direct streaming

\*/

#ifndef _ESP_TGZ_H
#define _ESP_TGZ_H

#include <FS.h>


#if defined( ESP32 )
  #include <Update.h>
#elif defined( ESP8266 )
  //#ifdef USE_LittleFS
  //  #define SPIFFS LittleFS
  //  #include <LittleFS.h>
  //#endif
  #include <Updater.h>
#else
  #error Unsupported architecture
#endif

#define GZIP_DICT_SIZE 32768
#define GZIP_BUFF_SIZE 4096

#define FOLDER_SEPARATOR "/"

#ifndef FILE_READ
  #define FILE_READ "r"
#endif
#ifndef FILE_WRITE
  #define FILE_WRITE "w+"
#endif
#ifndef SPI_FLASH_SEC_SIZE
  #define SPI_FLASH_SEC_SIZE 4096
#endif

#define CC_UNUSED __attribute__((unused))


namespace GZ
{
  #include "uzlib/uzlib.h"     // https://github.com/pfalcon/uzlib
}

namespace TAR
{
  extern "C"
  {
    #include "TinyUntar/untar.h" // https://github.com/dsoprea/TinyUntar
  }
}

// Callbacks for getting free/total space left on *destination* device.
// Optional but recommended to prevent SPIFFS/LittleFS/FFat partitions
// to explode during stream writes.
typedef size_t (*fsTotalBytesCb)();
typedef size_t (*fsFreeBytesCb)();

// setup filesystem helpers (totalBytes, freeBytes)
// must be done from outside the library since FS is an abstraction of an abstraction :(
typedef void (*fsSetupCallbacks)( fsTotalBytesCb cbt, fsFreeBytesCb cbf );

// tar doesn't have a real progress, so provide a status instead
typedef void (*tarStatusProgressCb)( const char* name, size_t size, size_t total_unpacked );

// Callbacks for progress and misc output messages, default is verbose
typedef void (*genericProgressCallback)( uint8_t progress );
typedef void (*genericLoggerCallback)( const char* format, ... );

// This is only to centralize error codes and spare the
// hassle of looking up in three different library folders

typedef enum tarGzErrorCode /* int8_t */
{

  ESP32_TARGZ_OK                         =   0,   // yay
  // general library errors
  ESP32_TARGZ_FS_ERROR                   =  -1,   // Filesystem error
  ESP32_TARGZ_STREAM_ERROR               =  -6,   // same a Filesystem error
  ESP32_TARGZ_UPDATE_INCOMPLETE          =  -7,   // Update not finished? Something went wrong
  ESP32_TARGZ_TAR_ERR_GZDEFL_FAIL        =  -38,  // Library error during deflating
  ESP32_TARGZ_TAR_ERR_GZREAD_FAIL        =  -39,  // Library error during gzip read
  ESP32_TARGZ_TAR_ERR_FILENAME_TOOLONG   =  -40,  // Library error during file creation
  ESP32_TARGZ_FS_FULL_ERROR              =  -100, // no space left on device
  ESP32_TARGZ_FS_WRITE_ERROR             =  -101, // no space left on device
  ESP32_TARGZ_FS_READSIZE_ERROR          =  -102, // no space left on device
  ESP32_TARGZ_HEAP_TOO_LOW               =  -103, // not enough heap
  ESP32_TARGZ_NEEDS_DICT                 =  -104, // gzip dictionnary needs to be enabled
  ESP32_TARGZ_UZLIB_PARSE_HEADER_FAILED  =  -105, // Gz Error when parsing header
  ESP32_TARGZ_UZLIB_MALLOC_FAIL          =  -106, // Gz Error when allocating memory
  ESP32_TARGZ_INTEGRITY_FAIL             =  -107, // General error, file integrity check fail

  // UZLIB: keeping error values from uzlib.h as is (no offset)
  ESP32_TARGZ_UZLIB_INVALID_FILE         =  -2,   // Not a valid gzip file
  ESP32_TARGZ_UZLIB_DATA_ERROR           =  -3,   // Gz Error TINF_DATA_ERROR
  ESP32_TARGZ_UZLIB_CHKSUM_ERROR         =  -4,   // Gz Error TINF_CHKSUM_ERROR
  ESP32_TARGZ_UZLIB_DICT_ERROR           =  -5,   // Gz Error TINF_DICT_ERROR
  ESP32_TARGZ_TAR_ERR_FILENAME_NOT_GZ    =  -41,  // Gz error, can't guess filename

  // UPDATE: adding -20 offset to actual error values from Update.h
  ESP32_TARGZ_UPDATE_ERROR_ABORT         =  -8,   // Updater Error UPDATE_ERROR_ABORT        -20   // (12-20) = -8
  ESP32_TARGZ_UPDATE_ERROR_BAD_ARGUMENT  =  -9,   // Updater Error UPDATE_ERROR_BAD_ARGUMENT -20   // (11-20) = -9
  ESP32_TARGZ_UPDATE_ERROR_NO_PARTITION  =  -10,  // Updater Error UPDATE_ERROR_NO_PARTITION -20   // (10-20) = -10
  ESP32_TARGZ_UPDATE_ERROR_ACTIVATE      =  -11,  // Updater Error UPDATE_ERROR_ACTIVATE     -20   // (9-20)  = -11
  ESP32_TARGZ_UPDATE_ERROR_MAGIC_BYTE    =  -12,  // Updater Error UPDATE_ERROR_MAGIC_BYTE   -20   // (8-20)  = -12
  ESP32_TARGZ_UPDATE_ERROR_MD5           =  -13,  // Updater Error UPDATE_ERROR_MD5          -20   // (7-20)  = -13
  ESP32_TARGZ_UPDATE_ERROR_STREAM        =  -14,  // Updater Error UPDATE_ERROR_STREAM       -20   // (6-20)  = -14
  ESP32_TARGZ_UPDATE_ERROR_SIZE          =  -15,  // Updater Error UPDATE_ERROR_SIZE         -20   // (5-20)  = -15
  ESP32_TARGZ_UPDATE_ERROR_SPACE         =  -16,  // Updater Error UPDATE_ERROR_SPACE        -20   // (4-20)  = -16
  ESP32_TARGZ_UPDATE_ERROR_READ          =  -17,  // Updater Error UPDATE_ERROR_READ         -20   // (3-20)  = -17
  ESP32_TARGZ_UPDATE_ERROR_ERASE         =  -18,  // Updater Error UPDATE_ERROR_ERASE        -20   // (2-20)  = -18
  ESP32_TARGZ_UPDATE_ERROR_WRITE         =  -19,  // Updater Error UPDATE_ERROR_WRITE        -20   // (1-20)  = -19

  // TAR: adding -30 offset to actual error values from untar.h
  ESP32_TARGZ_TAR_ERR_DATACB_FAIL        =  -32,  // Tar Error TAR_ERR_DATACB_FAIL       -30   // (-2-30) = -32
  ESP32_TARGZ_TAR_ERR_HEADERCB_FAIL      =  -33,  // Tar Error TAR_ERR_HEADERCB_FAIL     -30   // (-3-30) = -33
  ESP32_TARGZ_TAR_ERR_FOOTERCB_FAIL      =  -34,  // Tar Error TAR_ERR_FOOTERCB_FAIL     -30   // (-4-30) = -34
  ESP32_TARGZ_TAR_ERR_READBLOCK_FAIL     =  -35,  // Tar Error TAR_ERR_READBLOCK_FAIL    -30   // (-5-30) = -35
  ESP32_TARGZ_TAR_ERR_HEADERTRANS_FAIL   =  -36,  // Tar Error TAR_ERR_HEADERTRANS_FAIL  -30   // (-6-30) = -36
  ESP32_TARGZ_TAR_ERR_HEADERPARSE_FAIL   =  -37,  // Tar Error TAR_ERR_HEADERPARSE_FAIL  -30   // (-7-30) = -37
  ESP32_TARGZ_TAR_ERR_HEAP_TOO_LOW       =  -38,  // Tar Error TAR_ERROR_HEAP            -30   // (-8-30) = -38

} ErrorCodes ;




struct BaseUnpacker
{
  BaseUnpacker();
  bool tarGzHasError();
  int8_t tarGzGetError();
  void tarGzClearError();
  void haltOnError( bool halt );
  void initFSCallbacks();
  void tarGzListDir( fs::FS &fs, const char * dirName, uint8_t levels, bool hexDump = false);
  #ifdef ESP8266
  void printDirectory(fs::FS &fs, File dir, int numTabs, uint8_t levels, bool hexDump);
  #endif
  void hexDumpData( const char* buff, size_t buffsize, uint32_t output_size = 32 );
  void hexDumpFile( fs::FS &fs, const char* filename, uint32_t output_size = 32 );
  void setLoggerCallback( genericLoggerCallback cb );
  void setupFSCallbacks( fsTotalBytesCb cbt, fsFreeBytesCb cbf ); // setup abstract filesystem helpers (totalBytes, freeBytes)
  static void tarNullProgressCallback( uint8_t progress );
  static void targzNullLoggerCallback( const char* format, ... );
  static void targzNullProgressCallback( uint8_t progress );  // null progress callback, use with setProgressCallback or setTarProgressCallback to silent output
  static void targzPrintLoggerCallback(const char* format, ...);  // error/warning/info logger, use with setLoggerCallback() to enable output
  static void defaultProgressCallback( uint8_t progress );  // print progress callback, use with setProgressCallback or setTarProgressCallback to enable progress output
  static void defaultTarStatusProgressCallback( const char* name, size_t size, size_t total_unpacked ); // print tar status since a progress can't be provided
  static void setFsTotalBytesCb( fsTotalBytesCb cb ); // filesystem helpers totalBytes
  static void setFsFreeBytesCb( fsFreeBytesCb cb ); // filesystem helpers freeBytes
  static void setGeneralError( tarGzErrorCode code );
};


struct TarUnpacker : virtual public BaseUnpacker
{
  TarUnpacker();
  bool tarExpander( fs::FS &sourceFS, const char* fileName, fs::FS &destFS, const char* destFolder );
  //TODO: tarStreamExpander( Stream* sourceStream, fs::FS &destFS, const char* destFolder );
  void setTarStatusProgressCallback( tarStatusProgressCb cb );
  void setTarProgressCallback( genericProgressCallback cb ); // for tar
  void setTarMessageCallback( genericLoggerCallback cb ); // for tar
  void setTarVerify( bool verify ); // enables health checks but does slower writes
  static int unTarStreamReadCallback( unsigned char* buff, size_t buffsize );
  static int unTarStreamWriteCallback( TAR::header_translated_t *proper, int entry_index, void *context_data, unsigned char *block, int length);
  static int unTarHeaderCallBack(TAR::header_translated_t *proper,  int entry_index,  void *context_data);
  static int unTarEndCallBack( TAR::header_translated_t *proper, int entry_index, void *context_data);
};


struct GzUnpacker : virtual public BaseUnpacker
{
  GzUnpacker();
  bool    gzExpander( fs::FS sourceFS, const char* sourceFile, fs::FS destFS, const char* destFile = nullptr );
  //TODO: gzStreamExpander( Stream* sourceStream, fs::FS destFS, const char* destFile );
  bool    gzUpdater( fs::FS &sourceFS, const char* gz_filename, int partition = U_FLASH, bool restart_on_update = true ); // flashes the ESP with the content of a *gzipped* file
  bool    gzStreamUpdater( Stream *stream, size_t update_size = 0, int partition = U_FLASH, bool restart_on_update = true ); // flashes the ESP from a gzip stream (file or http), no progress callbacks
  void    setGzProgressCallback( genericProgressCallback cb );
  void    setGzMessageCallback( genericLoggerCallback cb );
  uint8_t *gzGetBufferUint8();
  void    gzExpanderCleanup();
  int     gzUncompress( bool isupdate = false, bool stream_to_tar = false, bool use_dict = true, bool show_progress = true );
  static bool         gzUpdateWriteCallback( unsigned char* buff, size_t buffsize );
  static bool         gzStreamWriteCallback( unsigned char* buff, size_t buffsize );
  static bool         gzReadHeader(fs::File &gzFile);
  static uint8_t      gzReadByte(fs::File &gzFile, const int32_t addr, fs::SeekMode mode=fs::SeekSet);
  static unsigned int gzReadDestByte(int offset, unsigned char *out);
  static unsigned int gzReadSourceByte(struct GZ::TINF_DATA *data, unsigned char *out);
};


struct TarGzUnpacker :
   public TarUnpacker,
   public GzUnpacker
{

  TarGzUnpacker();
  // unzip sourceFS://sourceFile.tar.gz contents into destFS://destFolder
  bool tarGzExpander( fs::FS sourceFS, const char* sourceFile, fs::FS destFS, const char* destFolder="/tmp", const char* tempFile = "/tmp/data.tar" );
  // same as tarGzExpander but without intermediate file
  bool tarGzExpanderNoTempFile( fs::FS sourceFS, const char* sourceFile, fs::FS destFS, const char* destFolder="/tmp" );
  //#if defined ESP32
  // unpack stream://fileName.tar.gz contents to destFS::/destFolder/
  bool tarGzStreamExpander( Stream *stream, fs::FS &destFs, const char* destFolder = "/" );
  //#endif
  static bool gzProcessTarBuffer( unsigned char* buff, size_t buffsize );
  static int tarReadGzStream( unsigned char* buff, size_t buffsize );
  static int gzFeedTarBuffer( unsigned char* buff, size_t buffsize );
};


#ifdef ESP8266
  // some ESP32 => ESP8266 syntax shim

  #define U_PART U_FS
  #define ARDUHAL_LOG_FORMAT(letter, format)  "[" #letter "][%s:%u] %s(): " format "\r\n", __FILE__, __LINE__, __FUNCTION__

  #if defined DEBUG_ESP_PORT
    // always show errors/warnings when debug port is there
    #define log_n(format, ...) DEBUG_ESP_PORT.printf(ARDUHAL_LOG_FORMAT(N, format), ##__VA_ARGS__);
    #define log_e(format, ...) DEBUG_ESP_PORT.printf(ARDUHAL_LOG_FORMAT(E, format), ##__VA_ARGS__);
    #define log_w(format, ...) DEBUG_ESP_PORT.printf(ARDUHAL_LOG_FORMAT(W, format), ##__VA_ARGS__);

    #if defined DEBUG_ESP_CORE
      // be verbose
      #define log_i(format, ...) DEBUG_ESP_PORT.printf(ARDUHAL_LOG_FORMAT(I, format), ##__VA_ARGS__);
      #define log_d(format, ...) DEBUG_ESP_PORT.printf(ARDUHAL_LOG_FORMAT(D, format), ##__VA_ARGS__);
      #define log_v(format, ...) DEBUG_ESP_PORT.printf(ARDUHAL_LOG_FORMAT(V, format), ##__VA_ARGS__);
    #else
      // don't be verbose, only errors+warnings
      #define log_i BaseUnpacker::targzNullLoggerCallback
      #define log_d BaseUnpacker::targzNullLoggerCallback
      #define log_v BaseUnpacker::targzNullLoggerCallback
    #endif

  #else
    #define log_n BaseUnpacker::targzNullLoggerCallback
    #define log_e BaseUnpacker::targzNullLoggerCallback
    #define log_w BaseUnpacker::targzNullLoggerCallback
    #define log_i BaseUnpacker::targzNullLoggerCallback
    #define log_d BaseUnpacker::targzNullLoggerCallback
    #define log_v BaseUnpacker::targzNullLoggerCallback
  #endif

#else
  #define U_PART U_SPIFFS
#endif

// md5sum (essentially for debug)
#include "helpers/md5_sum.h"
// helpers: mkdir, mkpath, dirname
#include "helpers/path_tools.h"


#endif // #ifdef _ESP_TGZ_H
