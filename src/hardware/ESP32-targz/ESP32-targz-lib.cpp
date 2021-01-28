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
#include "ESP32-targz-lib.h"
#include "hardware/alloc.h"

struct TarGzStream
{
  Stream *gz;
  Stream *tar;
  Stream *output;
  size_t gz_size;
  size_t tar_size;
  size_t output_size;
};

TarGzStream tarGzStream;

static fs::File untarredFile;
static fs::FS *tarFS = nullptr;

TAR::entry_callbacks_t tarCallbacks;

void          (*tgzLogger)( const char* format, ...) = nullptr;
static size_t (*fstotalBytes)() = nullptr;
static size_t (*fsfreeBytes)()  = nullptr;
static void   (*fsSetupSizeTools)( fsTotalBytesCb cbt, fsFreeBytesCb cbf ) = nullptr;
static void   (*tarProgressCallback)( uint8_t progress ) = nullptr;
static void   (*tarMessageCallback)( const char* format, ...) = nullptr;
static void   (*gzMessageCallback)( const char* format, ...) = nullptr;
static void   (*tarStatusProgressCallback)( const char* name, size_t size, size_t total_unpacked ) = nullptr;
static void   (*gzProgressCallback)( uint8_t progress ) = nullptr;
static bool   (*gzWriteCallback)( unsigned char* buff, size_t buffsize ) = nullptr;

static const char* tarDestFolder = nullptr;
static unsigned char __attribute__((aligned(4))) *output_buffer = nullptr;

static tarGzErrorCode _error = ESP32_TARGZ_OK;

static bool firstblock = true;
static bool lastblock = false;
static size_t tarCurrentFileSize = 0;
static size_t tarCurrentFileSizeProgress = 0;
static int32_t untarredBytesCount = 0;
static size_t totalFiles = 0;
static size_t totalFolders = 0;

#if defined ESP32
  static bool unTarDoHealthChecks = true; // set to false for faster writes
#endif
#if defined ESP8266
  static bool unTarDoHealthChecks = false; // ESP8266 is unstable with health checks
#endif

static bool targz_halt_on_error = false;

static unsigned char *uzlib_gzip_dict = nullptr;
static int64_t uzlib_bytesleft = 0;
static uint32_t output_position = 0;  //position in output_buffer
//static unsigned char __attribute__((aligned(4))) uzlib_read_cb_buff[GZIP_BUFF_SIZE];
static uint16_t blockmod = GZIP_BUFF_SIZE / TAR_BLOCK_SIZE;
static struct GZ::TINF_DATA uzLibDecompressor;
static uint16_t gzTarBlockPos = 0;
static size_t tarReadGzStreamBytes = 0;
size_t min_output_buffer_size = 512;

static bool halt_on_error()
{
  return targz_halt_on_error;
}

static void targz_system_halt()
{
  log_e("System halted after error code #%d", _error); while(1) { yield(); }
}

static void setError( tarGzErrorCode code )
{
  _error = code;
  if( _error != ESP32_TARGZ_OK && halt_on_error() ) targz_system_halt();
}



BaseUnpacker::BaseUnpacker()
{

  fstotalBytes = nullptr;
  fsfreeBytes  = nullptr;
  fsSetupSizeTools = nullptr;
  tarFS = nullptr;
  tarDestFolder = nullptr;
  output_buffer = nullptr;
  _error = ESP32_TARGZ_OK;

  //firstblock = true;
  //lastblock = false;
  tarCurrentFileSize = 0;
  tarCurrentFileSizeProgress = 0;
  untarredBytesCount = 0;
  totalFiles = 0;
  totalFolders = 0;

  uzlib_gzip_dict = nullptr;
  uzlib_bytesleft = 0;
  output_position = 0;  //position in output_buffer
  blockmod = GZIP_BUFF_SIZE / TAR_BLOCK_SIZE;
  gzTarBlockPos = 0;
  tarReadGzStreamBytes = 0;


}


void BaseUnpacker::setGeneralError( tarGzErrorCode code )
{
  setError( code );
}

void BaseUnpacker::haltOnError( bool halt )
{
  targz_halt_on_error = halt;
}


int8_t BaseUnpacker::tarGzGetError()
{
  return (int8_t)_error;
}


void BaseUnpacker::tarGzClearError()
{
  _error = ESP32_TARGZ_OK;
}


bool BaseUnpacker::tarGzHasError()
{
  return _error != ESP32_TARGZ_OK;
}


void BaseUnpacker::defaultTarStatusProgressCallback( const char* name, size_t size, size_t total_unpacked )
{
  Serial.printf("[TAR] %-64s %8d bytes - %8d Total bytes\n", name, size, total_unpacked );
}


// unpack sourceFS://fileName.tar contents to destFS::/destFolder/
void BaseUnpacker::defaultProgressCallback( uint8_t progress )
{
  static int8_t uzLibLastProgress = -1;
  if( uzLibLastProgress != progress ) {
    if( uzLibLastProgress == -1 ) {
      Serial.print("Progress: 0% ▓");
    }
    uzLibLastProgress = progress;
    switch( progress ) {
      //case   0: Serial.print("0% ▓");  break;
      case  25: Serial.print(" 25% ");break;
      case  50: Serial.print(" 50% ");break;
      case  75: Serial.print(" 75% ");break;
      case 100: Serial.print("▓ 100%\n"); uzLibLastProgress = -1; break;
      default: if( progress > 0 && progress < 100) Serial.print( "▓" ); break;
    }
  }
}


// progress callback for TAR, leave empty for less console output
void BaseUnpacker::tarNullProgressCallback( CC_UNUSED uint8_t progress )
{
  // print( message );
}


// progress callback for GZ, leave empty for less console output
void BaseUnpacker::targzNullProgressCallback( CC_UNUSED uint8_t progress )
{
  // printf("Progress: %d", progress );
}


// error/warning/info NULL logger, for less console output
void BaseUnpacker::targzNullLoggerCallback( CC_UNUSED const char* format, ...)
{
  //va_list args;
  //va_start(args, format);
  //vprintf(format, args);
  //va_end(args);
}


// error/warning/info FULL logger, for more console output
void BaseUnpacker::targzPrintLoggerCallback(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}


// set totalBytes() function callback
void BaseUnpacker::setFsTotalBytesCb( fsTotalBytesCb cb )
{
  log_d("Assigning setFsTotalBytesCb callback : 0x%8x", (uint)cb );
  fstotalBytes = cb;
}


// set freelBytes() function callback
void BaseUnpacker::setFsFreeBytesCb( fsFreeBytesCb cb )
{
  log_d("Assigning setFsFreeBytesCb callback : 0x%8x", (uint)cb );
  fsfreeBytes = cb;
}


// set logger callback
void BaseUnpacker::setLoggerCallback( genericLoggerCallback cb )
{
  log_d("Assigning debug logger callback : 0x%8x", (uint)cb );
  tgzLogger = cb;
}


// private (enables)
void BaseUnpacker::initFSCallbacks()
{
  if( fsSetupSizeTools && fstotalBytes && fsfreeBytes ) {
    log_d("Setting up fs size tools");
    fsSetupSizeTools( fstotalBytes, fsfreeBytes );
  } else {
    log_d("Skipping fs size tools setup");
  }
}


// public (assigns)
void BaseUnpacker::setupFSCallbacks( fsTotalBytesCb cbt, fsFreeBytesCb cbf )
{
  setFsTotalBytesCb( cbt );
  setFsFreeBytesCb( cbf );
  if( fsSetupSizeTools != NULL ) {
    log_d("deleting fsSetupSizeTools");
    fsSetupSizeTools = NULL;
  }
  log_d("Assigning lambda to fsSetupSizeTools");
  fsSetupSizeTools = []( fsTotalBytesCb cbt, fsFreeBytesCb cbf ) {
    log_d("Calling fsSetupSizeTools lambda");
    setFsTotalBytesCb( cbt );
    setFsFreeBytesCb( cbf );
  };
}


// generate hex view in the console, one call per line
void BaseUnpacker::hexDumpData( const char* buff, size_t buffsize, uint32_t output_size )
{
  static size_t totalBytes = 0;
  String bytesStr = "";
  String binaryStr = "";
  char byteToStr[32];

  for( size_t i=0; i<buffsize; i++ ) {
    sprintf( byteToStr, "%02X", buff[i] );
    bytesStr  += String( byteToStr ) + String(" ");
    if( isprint( buff[i] ) ) {
      binaryStr += String( buff[i] );
    } else {
      binaryStr += ".";
    }
  }
  sprintf( byteToStr, "[0x%04X - 0x%04X] ",  totalBytes, totalBytes+buffsize);
  totalBytes += buffsize;
  if( buffsize < output_size ) {
    for( size_t i=0; i<output_size-buffsize; i++ ) {
      bytesStr  += "-- ";
      binaryStr += ".";
    }
  }
  Serial.println( byteToStr + bytesStr + " " + binaryStr );
}


// show the contents of a given file as a hex dump
void BaseUnpacker::hexDumpFile( fs::FS &fs, const char* filename, uint32_t output_size )
{
  File binFile = fs.open( filename, FILE_READ );
  //log_w("File size : %d", binFile.size() );
  // only dump small files
  if( binFile.size() > 0 ) {
    //size_t output_size = 32;
    Serial.printf("Showing file %s (%d bytes) md5: %s\n", filename, binFile.size(), MD5Sum::fromFile( binFile ) );
    binFile.seek(0);
    char* buff = new char[output_size];
    uint8_t bytes_read = binFile.readBytes( buff, output_size );
    //String bytesStr  = "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
    //String binaryStr = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    //String addrStr = "[0x0000 - 0x0000] ";
    //char byteToStr[32];
    //size_t totalBytes = 0;
    while( bytes_read > 0 ) {

      hexDumpData( buff, bytes_read, output_size );
      /*
      bytesStr = "";
      binaryStr = "";
      for( int i=0; i<bytes_read; i++ ) {
        sprintf( byteToStr, "%02X", buff[i] );
        bytesStr  += String( byteToStr ) + String(" ");
        if( isprint( buff[i] ) ) {
          binaryStr += String( buff[i] );
        } else {
          binaryStr += ".";
        }
      }
      sprintf( byteToStr, "[0x%04X - 0x%04X] ",  totalBytes, totalBytes+bytes_read);
      totalBytes += bytes_read;
      if( bytes_read < output_size ) {
        for( size_t i=0; i<output_size-bytes_read; i++ ) {
          bytesStr  += "-- ";
          binaryStr += ".";
        }
      }
      Serial.println( byteToStr + bytesStr + " " + binaryStr );
      */
      bytes_read = binFile.readBytes( buff, output_size );
    }
  } else {
    Serial.printf("Ignoring file %s (%d bytes)", filename, binFile.size() );
  }
  binFile.close();
}


// get a directory listing of a given filesystem
#if defined ESP32

  void BaseUnpacker::tarGzListDir( fs::FS &fs, const char * dirName, uint8_t levels, bool hexDump )
  {
    File root = fs.open( dirName, FILE_READ );
    if( !root ) {
      log_e("[ERROR] in tarGzListDir: Can't open %s dir", dirName );
      if( halt_on_error() ) targz_system_halt();
      return;
    }
    if( !root.isDirectory() ) {
      log_e("[ERROR] in tarGzListDir: %s is not a directory", dirName );
      if( halt_on_error() ) targz_system_halt();
      return;
    }
    File file = root.openNextFile();
    while( file ) {
      if( file.isDirectory() ) {
        Serial.printf( "[DIR]  %s\n", file.name() );
        if( levels && levels > 0  ) {
          tarGzListDir( fs, file.name(), levels -1, hexDump );
        }
      } else {
        Serial.printf( "[FILE] %-32s %8d bytes - md5:%s\n", file.name(), file.size(), MD5Sum::fromFile( file ) );
        if( hexDump ) {
          hexDumpFile( fs, file.name() );
        }
      }
      file = root.openNextFile();
    }
  }

#elif defined( ESP8266 )


  void BaseUnpacker::printDirectory(fs::FS &fs, File dir, int numTabs, uint8_t levels, bool hexDump)
  {
    while (true) {

      File entry =  dir.openNextFile();
      if (! entry) {
        // no more files
        break;
      }
      for (uint8_t i = 0; i < numTabs; i++) {
        Serial.print("  ");
      }

      if (entry.isDirectory()) {
        if( levels > 0 ) {
          Serial.printf( "[DIR] %s\n", entry.name() );
          printDirectory(fs, entry, numTabs + 1, levels-1, hexDump );
        }
      } else {
        if( entry.size() > 0 ) {
          Serial.printf( "[FILE] %-32s %8d bytes - md5:%s\n", entry.name(), entry.size(), MD5Sum::fromFile( entry ) );
        } else {
          Serial.printf( "[????] %-32s \n", entry.name() );
        }

        if( hexDump ) {
          hexDumpFile( fs, entry.name() );
        }

      }
      entry.close();
    }
  }

  void BaseUnpacker::tarGzListDir(fs::FS &fs, const char * dirname, uint8_t levels, bool hexDump)
  {
    //void( hexDump ); // not used (yet?) with ESP82
    Serial.printf("Listing directory %s with level %d\n", dirname, levels);

    File root = fs.open(dirname, "r");
    if( !root.isDirectory() ){
      log_e( "%s is not a directory", dirname );
      return;
    }
    printDirectory(fs, root, 0, levels, hexDump );
    return;
  }





#endif











TarUnpacker::TarUnpacker()
{

}


void TarUnpacker::setTarStatusProgressCallback( tarStatusProgressCb cb )
{
  tarStatusProgressCallback = cb;
}


// set progress callback for TAR
void TarUnpacker::setTarProgressCallback( genericProgressCallback cb )
{
  log_d("Assigning tar progress callback : 0x%8x", (uint)cb );
  tarProgressCallback = cb;
}


// set tar unpacker message callback
void TarUnpacker::setTarMessageCallback( genericLoggerCallback cb )
{
  log_d("Assigning tar message callback : 0x%8x", (uint)cb );
  tarMessageCallback = cb;
}


// safer but slower
void TarUnpacker::setTarVerify( bool verify )
{
  log_d("Setting tar verify : %s", verify ? "true" : "false" );
  unTarDoHealthChecks = verify;
}




int TarUnpacker::unTarHeaderCallBack( TAR::header_translated_t *proper,  CC_UNUSED int entry_index,  CC_UNUSED void *context_data )
{
  dump_header(proper);
  static size_t totalsize = 0;
  if(proper->type == TAR::T_NORMAL) {

    if( fstotalBytes &&  fsfreeBytes ) {
      size_t freeBytes  = fsfreeBytes();
      if( freeBytes < proper->filesize ) {
        // Abort before the partition is smashed!
        log_e("[TAR ERROR] Not enough space left on device (%llu bytes required / %d bytes available)!", proper->filesize, freeBytes );
        return ESP32_TARGZ_FS_FULL_ERROR;
      }
    } else {
      #if defined WARN_LIMITED_FS
        log_w("[TAR WARNING] Can't check target medium for free space (required:%llu, free:\?\?), will try to expand anyway", proper->filesize );
      #endif
    }

    char file_path[256] = {0};
    memset( file_path, 0, 256 );
    // avoid double slashing root path
    if( strcmp( tarDestFolder, FOLDER_SEPARATOR ) != 0 ) {
      strcat(file_path, tarDestFolder);
    }
    // only append slash if destination folder does not end with a slash
    if( file_path[strlen(file_path)-1] != FOLDER_SEPARATOR[0] ) {
      strcat(file_path, FOLDER_SEPARATOR);
    }
    strcat(file_path, proper->filename);

    if( tarFS->exists( file_path ) ) {
      // file will be truncated
      /*
      untarredFile = tarFS->open( file_path, FILE_READ );
      bool isdir = untarredFile.isDirectory();
      untarredFile.close();
      if( isdir ) {
        log_d("[TAR DEBUG] Keeping %s folder", file_path);
      } else {
        log_d("[TAR DEBUG] Deleting %s as it is in the way", file_path);
        tarFS->remove( file_path );
      }
      */
    } else {
      // create directory (recursively if necessary)
      mkdirp( tarFS, file_path );
    }
    //TODO: limit this check to SPIFFS/LittleFS only
    if( strlen( file_path ) > 32 ) {
      // WARNING: SPIFFS LIMIT
      #if defined WARN_LIMITED_FS
        log_w("[TAR WARNING] file path is longer than 32 chars (SPIFFS limit) and may fail: %s", file_path);
        setError( ESP32_TARGZ_TAR_ERR_FILENAME_TOOLONG ); // don't break untar for that
      #endif
    } else {
      log_d("[TAR] Creating %s", file_path);
    }

    untarredFile = tarFS->open(file_path, FILE_WRITE);
    if(!untarredFile) {
      log_e("[ERROR] in unTarHeaderCallBack: Could not open [%s] for write.", file_path);
      setError( ESP32_TARGZ_FS_ERROR );
      return ESP32_TARGZ_FS_ERROR;
    }
    tarGzStream.output = &untarredFile;
    tarCurrentFileSize = proper->filesize; // for progress
    tarCurrentFileSizeProgress = 0; // for progress

    totalsize += proper->filesize;
    if( tarStatusProgressCallback ) {
      tarStatusProgressCallback( proper->filename, proper->filesize, totalsize );
    }
    if( totalsize == proper->filesize )
      tarProgressCallback( 0 );

  } else {

    switch( proper->type ) {
      case TAR::T_HARDLINK:       log_d("Ignoring hard link to %s.", proper->filename); break;
      case TAR::T_SYMBOLIC:       log_d("Ignoring sym link to %s.", proper->filename); break;
      case TAR::T_CHARSPECIAL:    log_d("Ignoring special char."); break;
      case TAR::T_BLOCKSPECIAL:   log_d("Ignoring special block."); break;
      case TAR::T_DIRECTORY:      log_d("Entering %s directory.", proper->filename);
        //tarMessageCallback( "Entering %s directory\n", proper->filename );
        if( tarStatusProgressCallback ) {
          tarStatusProgressCallback( proper->filename, 0, totalsize );
        }
        totalFolders++;
      break;
      case TAR::T_FIFO:           log_d("Ignoring FIFO request."); break;
      case TAR::T_CONTIGUOUS:     log_d("Ignoring contiguous data to %s.", proper->filename); break;
      case TAR::T_GLOBALEXTENDED: log_d("Ignoring global extended data."); break;
      case TAR::T_EXTENDED:       log_d("Ignoring extended data."); break;
      case TAR::T_OTHER: default: log_d("Ignoring unrelevant data.");       break;
    }

  }



  return ESP32_TARGZ_OK;
}





int TarUnpacker::unTarEndCallBack( TAR::header_translated_t *proper, CC_UNUSED int entry_index, CC_UNUSED void *context_data)
{
  int ret = ESP32_TARGZ_OK;
  char tmp_path[256] = {0};
  if(untarredFile) {

    if( unTarDoHealthChecks ) {
      memset( tmp_path, 0, 256 );
      snprintf( tmp_path, 256, "%s", untarredFile.name() );
      size_t pos = untarredFile.position();
      untarredFile.close();
      // health check 1: file existence
      if( !tarFS->exists( tmp_path ) ) {
        log_e("[TAR ERROR] File %s was not created although it was properly decoded, path is too long ?", tmp_path );
        return ESP32_TARGZ_FS_WRITE_ERROR;
      }
      // health check 2: compare stream buffer position with speculated file size
      if( pos != proper->filesize ) {
        log_e("[TAR ERROR] File size and data size do not match (%d vs %d)!", (int)pos, (int)proper->filesize);
        return ESP32_TARGZ_FS_WRITE_ERROR;
      }
      // health check 3: reopen file to check size on filesystem
      untarredFile = tarFS->open(tmp_path, FILE_READ);
      size_t tmpsize = untarredFile.size();
      if( !untarredFile ) {
        log_e("[TAR ERROR] Failed to re-open %s for size reading", tmp_path);
        return ESP32_TARGZ_FS_READSIZE_ERROR;
      }
      // health check 4: see if everyone (buffer, stream, filesystem) agree
      if( tmpsize == 0 || proper->filesize != tmpsize || pos != tmpsize ) {
        log_e("[TAR ERROR] Byte sizes differ between written file %s (%d), tar headers (%d) and/or stream buffer (%d) !!", tmp_path, (int)tmpsize, (int)proper->filesize, (int)pos );
        untarredFile.close();
        return ESP32_TARGZ_FS_ERROR;
      }
      log_d("Expanded %s (%d bytes)", tmp_path, (int)tmpsize );
    }

    untarredFile.close();

    static size_t totalsize = 0;
    if( proper->type != TAR::T_DIRECTORY ) {
      totalsize += proper->filesize;
    }

    tarProgressCallback( 100 );
    log_d("Total expanded bytes: %d, heap free: %d", (int)totalsize, ESP.getFreeHeap() );

    tarMessageCallback( "%s", proper->filename );

  } else {
    log_v("[TAR INFO] unTarEndCallBack: nofile for `%s`", proper->filename );
  }
  totalFiles++;

  return ret;
}


// unpack sourceFS://fileName.tar contents to destFS::/destFolder/
bool TarUnpacker::tarExpander( fs::FS &sourceFS, const char* fileName, fs::FS &destFS, const char* destFolder )
{

  tarGzClearError();
  initFSCallbacks();
  tarFS = &destFS;
  tarDestFolder = destFolder;

  if (!tgzLogger ) {
    setLoggerCallback( targzPrintLoggerCallback );
  }
  if( !tarProgressCallback ) {
    setTarProgressCallback( tarNullProgressCallback );
  }
  if( !tarMessageCallback ) {
    setTarMessageCallback( targzNullLoggerCallback );
  }
  if( !sourceFS.exists( fileName ) ) {
    log_e("Error: file %s does not exist or is not reachable", fileName);
    setError( ESP32_TARGZ_FS_ERROR );
    return false;
  }
  if( !destFS.exists( tarDestFolder ) ) {
    destFS.mkdir( tarDestFolder );
  }

  tgzLogger("[TAR] Expanding %s to folder %s\n", fileName, destFolder );

  untarredBytesCount = 0;

  tarCallbacks = {
    unTarHeaderCallBack,
    unTarStreamReadCallback,
    unTarStreamWriteCallback,
    unTarEndCallBack
  };
  fs::File tarFile = sourceFS.open( fileName, FILE_READ );
  tarGzStream.tar_size = tarFile.size();
  tarGzStream.tar = &tarFile;
  //tinyUntarReadCallback = &unTarStreamReadCallback;
  TAR::tar_error_logger      = tgzLogger;
  TAR::tar_debug_logger      = tgzLogger; // comment this out if too verbose

  totalFiles = 0;
  totalFolders = 0;

  //tarProgressCallback( 0 );

  int res = TAR::read_tar( &tarCallbacks, NULL );
  if( res != TAR_OK ) {
    log_e("[ERROR] operation aborted while expanding tar file %s (return code #%d", fileName, res-30);
    setError( (tarGzErrorCode)(res-30) );
    return false;
  }

  //tarProgressCallback( 100 );

  return true;
}


// tinyUntarReadCallback
int TarUnpacker::unTarStreamReadCallback( unsigned char* buff, size_t buffsize )
{
  return tarGzStream.tar->readBytes( buff, buffsize );
}


int TarUnpacker::unTarStreamWriteCallback(CC_UNUSED TAR::header_translated_t *proper, CC_UNUSED int entry_index, CC_UNUSED void *context_data, unsigned char *block, int length)
{
  if( tarGzStream.output ) {
    int wlen = tarGzStream.output->write( block, length );
    if( wlen != length ) {
      //tgzLogger("\n");
      log_e("[TAR ERROR] Written length differs from buffer length (unpacked bytes:%d, expected: %d, returned: %d)!\n", untarredBytesCount, length, wlen );
      return ESP32_TARGZ_FS_ERROR;
    }
    untarredBytesCount+=wlen;
    // TODO: file unpack progress
    log_v("[TAR INFO] unTarStreamWriteCallback wrote %d bytes to %s", length, proper->filename );
    tarCurrentFileSizeProgress += wlen;
    if( tarCurrentFileSize > 0 ) {
      // this is a per-file progress, not an overall progress !
      int32_t progress = (100*tarCurrentFileSizeProgress) / tarCurrentFileSize;
      if( progress != 100 && progress != 0 ) {
        tarProgressCallback( progress );
      }
    }
  }
  return ESP32_TARGZ_OK;
}









GzUnpacker::GzUnpacker()
{

}


// set progress callback for GZ
void GzUnpacker::setGzProgressCallback( genericProgressCallback cb )
{
  log_d("Assigning GZ progress callback : 0x%8x", (uint)cb );
  gzProgressCallback = cb;
}



// set logger callback
void GzUnpacker::setGzMessageCallback( genericLoggerCallback cb )
{
  log_d("Assigning debug logger callback : 0x%8x", (uint)cb );
  gzMessageCallback = cb;
}




void GzUnpacker::gzExpanderCleanup()
{
  if( uzlib_gzip_dict != nullptr ) {
    delete( uzlib_gzip_dict );
    uzlib_gzip_dict = NULL;
  }
}


// gzWriteCallback
bool GzUnpacker::gzUpdateWriteCallback( unsigned char* buff, size_t buffsize )
{
  if( Update.write( buff, buffsize ) ) {
    log_v("Wrote %d bytes", buffsize );
    return true;
  } else {
    log_e("Failed to write %d bytes", buffsize );
  }
  return false;
}


// gzWriteCallback
bool GzUnpacker::gzStreamWriteCallback( unsigned char* buff, size_t buffsize )
{
  if( ! tarGzStream.output->write( buff, buffsize ) ) {
    log_w("\n[GZ WARNING] failed to write %d bytes, will try a second time\n", buffsize );
    if( ! tarGzStream.output->write( buff, buffsize ) ) {
      log_e("\n[GZ ERROR] failed to write %d bytes\n", buffsize );
      setError( ESP32_TARGZ_STREAM_ERROR );
      return false;
    }

  } else {
    log_v("Wrote %d bytes", buffsize );
  }
  return true;
}


// gz filesystem helper
uint8_t GzUnpacker::gzReadByte( fs::File &gzFile, const int32_t addr, fs::SeekMode mode )
{
  gzFile.seek( addr, mode );
  return gzFile.read();
}


// 1) check if a file has valid gzip headers
// 2) calculate space needed for decompression
// 2) check if enough space is available on device
bool GzUnpacker::gzReadHeader( fs::File &gzFile )
{
  tarGzStream.output_size = 0;
  tarGzStream.gz_size = gzFile.size();
  bool ret = false;
  if ((gzReadByte(gzFile, 0) == 0x1f) && (gzReadByte(gzFile, 1) == 0x8b)) {
    // GZIP signature matched.  Find real size as encoded at the end
    tarGzStream.output_size =  gzReadByte(gzFile, tarGzStream.gz_size - 4);
    tarGzStream.output_size += gzReadByte(gzFile, tarGzStream.gz_size - 3)<<8;
    tarGzStream.output_size += gzReadByte(gzFile, tarGzStream.gz_size - 2)<<16;
    tarGzStream.output_size += gzReadByte(gzFile, tarGzStream.gz_size - 1)<<24;
    log_i("[GZ INFO] valid gzip file detected! gz size: %d bytes, expanded size:%d bytes", tarGzStream.gz_size, tarGzStream.output_size);
    // Check for free space left on device before writing
    if( fstotalBytes &&  fsfreeBytes ) {
      size_t freeBytes  = fsfreeBytes();
      if( freeBytes < tarGzStream.output_size ) {
        // not enough space on device
        log_e("[GZ ERROR] Target medium will be out of space (required:%d, free:%d), aborting!", tarGzStream.output_size, freeBytes);
        return false;
      } else {
        log_i("[GZ INFO] Available space:%d bytes", freeBytes);
      }
    } else {
      #if defined WARN_LIMITED_FS
        log_w("[GZ WARNING] Can't check target medium for free space (required:%d, free:\?\?), will try to expand anyway\n", tarGzStream.output_size );
      #endif
    }
    ret = true;
  }
  gzFile.seek(0);
  return ret;
}


// read a byte from the decompressed destination file, at 'offset' from the current position.
// offset will be the negative offset back into the written output stream.
// note: this does not ever write to the output stream; it simply reads from it.
unsigned int GzUnpacker::gzReadDestByte(int offset, unsigned char *out)
{
  unsigned char data;
  //delta between our position in output_buffer, and the desired offset in the output stream
  int delta = output_position + offset;
  if (delta >= 0) {
    //we haven't written output_buffer to persistent storage yet; we need to read from output_buffer
    data = output_buffer[delta];
  } else {
    fs::File *f = (fs::File*)tarGzStream.output;
    //we need to read from persistent storage
    //save where we are in the file
    long last_pos = f->position();
    f->seek( last_pos+delta, fs::SeekSet );
    data = f->read();//gzReadByte(*f, last_pos+delta, fs::SeekSet);
    f->seek( last_pos, fs::SeekSet );
  }
  *out = data;

  return 0;
}


// consume and return a byte from the source stream into the argument 'out'.
// returns 0 on success, or -1 on error.
unsigned int GzUnpacker::gzReadSourceByte(CC_UNUSED struct GZ::TINF_DATA *data, unsigned char *out)
{
  //if( !BaseUnpacker::tarGzStream.gz->available() ) return -1;
  if (tarGzStream.gz->readBytes( out, 1 ) != 1) {
    log_d("readSourceByte read error, available is %d.  attempting one-time retry", tarGzStream.gz->available());
    if (tarGzStream.gz->readBytes( out, 1 ) != 1) {
      log_e("readSourceByte read error, available is %d.  failed at retry", tarGzStream.gz->available());
      return -1;
    }
  } else {
    //log_v("read 1 byte: 0x%02x", out[0] );
  }
  return 0;
}


// gz decompression main routine, handles all logical cases
// isupdate      => zerofill to fit SPI_FLASH_SEC_SIZE
// stream_to_tar => sent bytes to tar instead of filesystem
// use_dict      => change memory usage stragegy
// show_progress => enable/disable bytes count (not always applicable)
int GzUnpacker::gzUncompress( bool isupdate, bool stream_to_tar, bool use_dict, bool show_progress )
{

  log_d("gzUncompress( isupdate = %s, stream_to_tar = %s, use_dict = %s, show_progress = %s)",
    isupdate      ? "true" : "false",
    stream_to_tar ? "true" : "false",
    use_dict      ? "true" : "false",
    show_progress ? "true" : "false"
  );

  if( !tarGzStream.gz->available() ) {
    log_e("[ERROR] in gzUncompress: gz resource doesn't exist!");
    return ESP32_TARGZ_STREAM_ERROR;
  }

  #if defined ESP32
    size_t output_buffer_size = SPI_FLASH_SEC_SIZE; // SPI_FLASH_SEC_SIZE = 4Kb
  #endif
  #if defined ESP8266
    size_t output_buffer_size = min_output_buffer_size; // must be a multiple of 512 (tar block size)
  #endif

  int uzlib_dict_size = 0;
  int res = 0;

  GZ::uzlib_init();

  if ( use_dict == true ) {
    uzlib_gzip_dict = new unsigned char[GZIP_DICT_SIZE];
    if( uzlib_gzip_dict == NULL ) {
      log_e("[ERROR] can't alloc %d bytes for gzip dict (%d bytes free)", GZIP_DICT_SIZE, ESP.getFreeHeap() );
      gzExpanderCleanup();
      return ESP32_TARGZ_UZLIB_MALLOC_FAIL; // TODO : Number this error
    }
    uzlib_dict_size = GZIP_DICT_SIZE;
    uzLibDecompressor.readDestByte   = NULL;
    log_d("[INFO] gzUncompress tradeoff: faster, used %d bytes of ram (heap after alloc: %d)", GZIP_DICT_SIZE+output_buffer_size, ESP.getFreeHeap());
    //log_w("[%d] alloc() done", ESP.getFreeHeap() );
  } else {
    if( stream_to_tar ) {
      log_e("[ERROR] gz->tar->filesystem streaming requires a gzip dictionnnary");
      return ESP32_TARGZ_NEEDS_DICT;
    } else {
      uzLibDecompressor.readDestByte   = gzReadDestByte;
      log_d("[INFO] gz output is file");
    }
    //output_buffer_size = SPI_FLASH_SEC_SIZE;
    log_d("[INFO] gzUncompress tradeoff: slower will use %d bytes of ram (heap before alloc: %d)", output_buffer_size, ESP.getFreeHeap());
    uzlib_gzip_dict = NULL;
    uzlib_dict_size = 0;
  }

  uzLibDecompressor.source         = nullptr;
  uzLibDecompressor.readSourceByte = gzReadSourceByte;
  uzLibDecompressor.destSize       = 1;
  uzLibDecompressor.log            = targzPrintLoggerCallback;

  res = GZ::uzlib_gzip_parse_header(&uzLibDecompressor);
  if (res != TINF_OK) {
    log_e("[ERROR] in gzUncompress: uzlib_gzip_parse_header failed (response code %d!", res);
    //if( halt_on_error() ) targz_system_halt();
    gzExpanderCleanup();
    return ESP32_TARGZ_UZLIB_PARSE_HEADER_FAILED;
  }

  GZ::uzlib_uncompress_init(&uzLibDecompressor, uzlib_gzip_dict, uzlib_dict_size);

  output_buffer = (unsigned char *)CALLOC( output_buffer_size+1, sizeof(unsigned char) );
  if( output_buffer == NULL ) {
    log_e("[ERROR] can't alloc %d bytes for output buffer", output_buffer_size );
    gzExpanderCleanup();
    return ESP32_TARGZ_UZLIB_MALLOC_FAIL; // TODO : Number this error
  }

  /* decompress a single byte at a time */
  output_position = 0;
  unsigned int outlen = 0;

  if( show_progress ) {
    gzProgressCallback( 0 );
  }

  if( stream_to_tar ) {
    // tar will pull bytes from gz for when needed
    //tinyUntarReadCallback = &tarReadGzStream;
    blockmod = output_buffer_size / TAR_BLOCK_SIZE;
    log_d("[INFO] output_buffer_size=%d blockmod=%d", output_buffer_size, blockmod );
    untarredBytesCount = 0;
    int ret = TAR::tar_setup(&tarCallbacks, NULL);
    firstblock = false;
    if( ret != TAR_OK ) {
      setError( (tarGzErrorCode)(ret-30) );
      return (tarGzErrorCode)(ret-30);
    }
    while( TAR::read_tar_step() == TAR_OK );
    outlen = untarredBytesCount;

  } else {
    // gz will fill a buffer and trigger a write callback
    do {
      // link to gz internals
      uzLibDecompressor.dest = &output_buffer[output_position];
      res = GZ::uzlib_uncompress_chksum(&uzLibDecompressor);
      if (res != TINF_OK) {
        // uncompress done or aborted, no need to go further
        break;
      }
      output_position++;
      // when destination buffer is filled, write/stream it
      if (output_position == output_buffer_size) {
        log_v("[INFO] Buffer full, now writing %d bytes (total=%d)", output_buffer_size, outlen);
        gzWriteCallback( output_buffer, output_buffer_size );
        outlen += output_buffer_size;
        output_position = 0;
      }

      if( show_progress ) {
        uzlib_bytesleft = tarGzStream.output_size - outlen;
        int32_t progress = 100*(tarGzStream.output_size-uzlib_bytesleft) / tarGzStream.output_size;
        gzProgressCallback( progress );
      }

    } while ( res == TINF_OK );

    if (res != TINF_DONE) {
      log_w("[GZ WARNING] uzlib_uncompress_chksum return code=%d, premature end at position %d while %d bytes left", res, output_position, (int)uzlib_bytesleft);
    }

    if( output_position > 0 ) {
      gzWriteCallback( output_buffer, output_position );
      outlen += output_position;
      output_position = 0;
    }

    if( isupdate && outlen > 0 ) {
      size_t updatable_size = ( outlen + SPI_FLASH_SEC_SIZE-1 ) & ~( SPI_FLASH_SEC_SIZE-1 );
      size_t zerofill_size  = updatable_size - outlen;
      if( zerofill_size <= SPI_FLASH_SEC_SIZE ) {
        memset( output_buffer, 0, zerofill_size );
        // zero-fill to fit update.h required binary size
        gzWriteCallback( output_buffer, zerofill_size );
        outlen += zerofill_size;
        output_position = 0;
      }
    }

  }

  if( show_progress ) {
    gzProgressCallback( 100 );
  }

  log_d("decompressed %d bytes", outlen + output_position);

  free( output_buffer );
  gzExpanderCleanup();

  return outlen > 0 ? ESP32_TARGZ_OK : ESP32_TARGZ_STREAM_ERROR;
}


// uncompress gz sourceFile to destFile
bool GzUnpacker::gzExpander( fs::FS sourceFS, const char* sourceFile, fs::FS destFS, const char* destFile )
{
  tarGzClearError();
  initFSCallbacks();
  if (!tgzLogger ) {
    setLoggerCallback( targzPrintLoggerCallback );
  }
  bool gz_use_dict = true;
  if( ESP.getFreeHeap() < GZIP_DICT_SIZE+GZIP_BUFF_SIZE ) {
    size_t free_min_heap_blocks = ESP.getFreeHeap() / 512; // leave 1k heap, eat all the rest !
    if( free_min_heap_blocks <1 ) {
      setError( ESP32_TARGZ_HEAP_TOO_LOW );
      return false;
    }
    min_output_buffer_size = free_min_heap_blocks * 512;
    if( min_output_buffer_size > GZIP_BUFF_SIZE ) min_output_buffer_size = GZIP_BUFF_SIZE;
    log_w("Disabling GZIP Dictionary (heap wanted:%d, available: %d, buffer: %d bytes), writes will be slow", ESP.getFreeHeap(), GZIP_DICT_SIZE+GZIP_BUFF_SIZE, min_output_buffer_size );
    gz_use_dict = false;
    //
  } else {
    log_d("Current heap budget (available:%d, needed:%d)", ESP.getFreeHeap(), GZIP_DICT_SIZE+GZIP_BUFF_SIZE );
  }

  if( destFile == nullptr ) {
    // when no destination name is provided, it will be speculated
    // assumption: the source filename ends with ".gz"
    String sourceFileCopy = String(sourceFile);
    size_t slen = sourceFileCopy.length();
    if( sourceFileCopy.endsWith(".gz") ) {
      slen -= 3;
      sourceFileCopy = sourceFileCopy.substring( 0, slen );
      destFile = (const char*)CALLOC( slen+1, sizeof(char) );
      if( destFile == NULL ) {
        setError( ESP32_TARGZ_HEAP_TOO_LOW );
        return false;
      }
      snprintf( (char*)destFile, slen+1, "%s", sourceFileCopy.c_str() );
      log_w("Speculated filename: %s", destFile );
    } else {
      setError( ESP32_TARGZ_TAR_ERR_FILENAME_NOT_GZ );
      return false;
    }
  }

  tgzLogger("[GZ] Expanding %s to %s\n", sourceFile, destFile );

  fs::File gz = sourceFS.open( sourceFile, FILE_READ );
  if( !gzProgressCallback ) {
    setGzProgressCallback( defaultProgressCallback );
  }
  if( !gzReadHeader( gz ) ) {
    log_e("[GZ ERROR] in gzExpander: invalid gzip file or not enough space left on device ?");
    gz.close();
    setError( ESP32_TARGZ_UZLIB_INVALID_FILE );
    return false;
  }

  if( destFS.exists( destFile ) ) {
    log_d("[GZ INFO] Deleting %s as it is in the way", destFile);
    destFS.remove( destFile );
  }
  fs::File outfile = destFS.open( destFile, "w+" );
  tarGzStream.gz = &gz;
  tarGzStream.output = &outfile;
  gzWriteCallback = &gzStreamWriteCallback; // for regular unzipping

  int ret = gzUncompress( false, false, gz_use_dict );

  outfile.close();
  gz.close();

  if( ret!=0 ) {
    log_e("gzUncompress returned error code %d", ret);
    setError( (tarGzErrorCode)ret );
    return false;
  }
  log_d("uzLib expander finished!");

  /*
  outfile = destFS.open( destFile, FILE_READ );
  log_d("Expanded %s to %s (%d bytes)", sourceFile, destFile, outfile.size() );
  outfile.close();
  */
  if( gzMessageCallback ) {
    gzMessageCallback("%s", destFile );
  }

  if( fstotalBytes &&  fsfreeBytes ) {
    log_d("[GZ Info] FreeBytes after expansion=%d", fsfreeBytes() );
  }

  return true;
}


// uncompress gz file to flash (expected to be a valid gzipped firmware)
bool GzUnpacker::gzUpdater( fs::FS &fs, const char* gz_filename, int partition, bool restart_on_update )
{
  tarGzClearError();
  initFSCallbacks();
  if (!tgzLogger ) {
    setLoggerCallback( targzPrintLoggerCallback );
  }
  // ESP8266 does not need such check as the unpacker is in the bootloader

  if( !fs.exists( gz_filename )  ) {
    log_e("[ERROR] in gzUpdater: %s does not exist", gz_filename);
    setError( ESP32_TARGZ_UZLIB_INVALID_FILE );
    return false;
  }
  log_d("uzLib SPIFFS Updater start!");
  fs::File gz = fs.open( gz_filename, FILE_READ );
  #if defined ESP8266
    int update_size = gz.size();
  #endif
  #if defined ESP32
    int update_size = UPDATE_SIZE_UNKNOWN;
  #endif
  return gzStreamUpdater( (Stream*)&gz, update_size, partition, restart_on_update );
}




// uncompress gz stream (file or HTTP) to flash (expected to be a valid Arduino compiled binary sketch)
bool GzUnpacker::gzStreamUpdater( Stream *stream, size_t update_size, int partition, bool restart_on_update )
{
  if( !gzProgressCallback ) {
    setGzProgressCallback( defaultProgressCallback );
  }
  if( !tgzLogger ) {
    setLoggerCallback( targzPrintLoggerCallback );
  }

  size_t size = stream->available();
  if( ! size ) {
    log_e("Bad stream, aborting");
    setError( ESP32_TARGZ_STREAM_ERROR );
    return false;
  }

  #ifdef ESP8266
    // ESP8266 has built-in uzlib so no need to uncompress, just update with gzData
    bool use_buffered_writes = false; // use buffered writes when the gz size is unknown, otherwise use stream writes
    size_t stream_size = 0;

    if( int( update_size ) < 1 ) {
      use_buffered_writes = true;
      stream_size = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      log_d("Stream size is unknown, aligning update to the partition available size: %d ", stream_size );
    } else {
      Update.runAsync( true );
      stream_size = update_size;
      log_d("Stream size is %d", stream_size );
    }

    if ( !Update.begin( stream_size, partition )) { // U_FLASH or U_PART
      log_e("Can't begin update");
      setError( (tarGzErrorCode)(Update.getError()-20) ); // "-20" offset is Update error id to esp32-targz error id
      return false;
    }

    if( !use_buffered_writes ) {
      // stream method
      static bool finished = false;
      // async progress
      Update.onProgress([]( size_t done, size_t total ) {
        size_t progress = (100*done)/total;
        if(! finished ) gzProgressCallback( progress );
        if( progress == 100 ) finished = true;
      });
      // walk stream
      while( stream->available() ) {
        if(! Update.writeStream( *stream ) ) {
          log_e("Update couldn't read stream");
          setError( (tarGzErrorCode)(Update.getError()-20) ); // "-20" offset is Update error id to esp32-targz error id
          return false;
        }
        if( finished ) break;
        yield();
      }
    } else {
      // buffered writes method, uses 4KB ram
      // TODO: make this adjustable
      size_t buffsize = 4096;
      uint8_t *buffer = new uint8_t[4096];
      if( !buffer) {
        log_e("Could not allocate %d bytes for stream read", buffsize );
        setError( ESP32_TARGZ_HEAP_TOO_LOW );
        return false;
      }
      uint8_t progress = 0;
      gzProgressCallback( progress );

      while( stream->available() ) {
        size_t len = stream->readBytes( buffer, buffsize );
        if( len < 1 ) break; // end of stream
        if (Update.write(buffer, len) != len) {
          log_e("Updater could not write %d bytes", len );
          setError( (tarGzErrorCode)(Update.getError()-20) ); // "-20" offset is Update error id to esp32-targz error id
          return false;
        } else {
          progress = (Update.progress()*100)/Update.size();
          gzProgressCallback( progress );
        }
        yield();
      }
      if( progress != 100 ) {
        gzProgressCallback( 100 );
      }
      delete buffer;
    }

    if ( !Update.end( true ) ) {
      Update.printError(Serial);
      log_e( "Update Error Occurred. Error #: %u", Update.getError() );
      setError( (tarGzErrorCode)(Update.getError()-20) ); // "-20" offset is Update error id to esp32-targz error id
      return false;
    }

    if ( !Update.isFinished() ) {
      log_e("Update incomplete");
      setError( ESP32_TARGZ_UPDATE_INCOMPLETE );
      return false;
    }

    log_d("Update finished !");
    if( restart_on_update ) ESP.restart();
  #endif // ifdef ESP8266


  #if defined ESP32
    // unfortunately ESP32 doesn't handle gzipped firmware from the bootloader
    bool show_progress = false;
    bool use_dict = true;

    if( ESP.getFreeHeap() < GZIP_DICT_SIZE+GZIP_BUFF_SIZE ) {
      // log_w("Disabling gzip dictionnary (havailable:%d, needed:%d)", ESP.getFreeHeap(), GZIP_DICT_SIZE+GZIP_BUFF_SIZE );
      log_w("Insufficient heap to decompress (available:%d, needed:%d), aborting", ESP.getFreeHeap(), GZIP_DICT_SIZE+GZIP_BUFF_SIZE );
      setError( ESP32_TARGZ_HEAP_TOO_LOW );
      return false;
    }

    tarGzStream.gz = stream;
    gzWriteCallback = &gzUpdateWriteCallback; // for unzipping direct to flash

    if( int( update_size ) < 1 || update_size == UPDATE_SIZE_UNKNOWN ) {
      tgzLogger("[GZUpdater] Starting update with unknown binary size\n");
      if( !Update.begin( UPDATE_SIZE_UNKNOWN, partition ) ) {
        setError( (tarGzErrorCode)(Update.getError()-20) ); // "-20" offset is Update error id to esp32-targz error id
        return false;
      }
      Update.onProgress([]( size_t done, size_t total ) {
        if ( done != 0 ) {
          gzProgressCallback( ( 100*done ) / total );
        }
      });
      show_progress = false;
    }
    else {
      tgzLogger("[GZUpdater] Starting update\n");
      if( !Update.begin( ( ( update_size + SPI_FLASH_SEC_SIZE-1 ) & ~( SPI_FLASH_SEC_SIZE-1 ) ), partition ) ) {
        setError( (tarGzErrorCode)(Update.getError()-20) ); // "-20" offset is Update error id to esp32-targz error id
        return false;
      }
      Update.onProgress([]( size_t done, size_t total ) {
        if ( done != 0 ) {
          gzProgressCallback( ( 100*done ) / total );
        }
      });
      show_progress = false;
    }
    // process with unzipping
    int ret = gzUncompress( true, false, use_dict, show_progress );
    // unzipping ended
    if( ret!=0 ) {
      log_e("gzHTTPUpdater returned error code %d", ret);
      setError( (tarGzErrorCode)ret );
      return false;
    }

    if ( Update.end( true ) ) {
      log_d( "OTA done!" );
      if ( Update.isFinished() ) {
        // yay
        log_d("Update finished !");
        if( restart_on_update ) ESP.restart();
      } else {
        log_e( "Update not finished? Something went wrong!" );
        setError( ESP32_TARGZ_UPDATE_INCOMPLETE );
        return false;
      }
    } else {
      log_e( "Update Error Occurred. Error #: %u", Update.getError() );
      setError( (tarGzErrorCode)(Update.getError()-20) ); // "-20" offset is Update error id to esp32-targz error id
      return false;
    }
    log_d("uzLib filesystem Updater finished!\n");
    setError( (tarGzErrorCode)ret );
  #endif // ifdef ESP32

  return true;
}












TarGzUnpacker::TarGzUnpacker() : TarUnpacker(), GzUnpacker()
{

}


// gzWriteCallback
bool TarGzUnpacker::gzProcessTarBuffer( CC_UNUSED unsigned char* buff, CC_UNUSED size_t buffsize )
{
  if( lastblock ) {
    return true;
  }

  if( firstblock ) {
    if( TAR::tar_setup(&tarCallbacks, NULL) == TAR_OK ) {
      firstblock = false;
    } else {
      return false;
    }
  }
  gzTarBlockPos = 0;
  while( gzTarBlockPos < blockmod ) {
    int response = TAR::read_tar_step(); // warn: this may fire more than 1 read_cb()
    if( response == TAR_EXPANDING_DONE ) {
      log_d("[TAR] Expanding done !");
      lastblock = true;
      return true;
    }
    if( gzTarBlockPos > blockmod ) {
      log_e("[ERROR] read_tar_step() fired more too many read_cb()");
      setError( ESP32_TARGZ_TAR_ERR_GZREAD_FAIL );
      return false;
    }
    if( response < 0 ) {
      log_e("[WARN] gzProcessTarBuffer failed reading %d bytes (buffsize=%d) in gzip block #%d/%d, got response %d", TAR_BLOCK_SIZE, buffsize, gzTarBlockPos%blockmod, blockmod, response);
      setError( ESP32_TARGZ_TAR_ERR_GZREAD_FAIL );
      return false;
    }
  }
  log_v("gz buffer processed by tar (%d steps)", gzTarBlockPos);
  return true;
}


// tinyUntarReadCallback
int TarGzUnpacker::tarReadGzStream( unsigned char* buff, size_t buffsize )
{
  if( buffsize%TAR_BLOCK_SIZE !=0 ) {
    log_e("[ERROR] tarReadGzStream Can't unmerge tar blocks (%d bytes) from gz block (%d bytes)\n", buffsize, GZIP_BUFF_SIZE);
    setError( ESP32_TARGZ_TAR_ERR_GZDEFL_FAIL );
    return 0;
  }
  size_t i;
  for( i=0; i<buffsize; i++) {
    uzLibDecompressor.dest = &buff[i];
    int res = GZ::uzlib_uncompress_chksum(&uzLibDecompressor);
    if (res != TINF_OK) {
      // uncompress done or aborted, no need to go further
      break;
    }
  }
  tarReadGzStreamBytes += i;

  uzlib_bytesleft = tarGzStream.output_size - tarReadGzStreamBytes;
  int32_t progress = 100*(tarGzStream.output_size-uzlib_bytesleft) / tarGzStream.output_size;
  gzProgressCallback( progress );
  return i;
}


// tinyUntarReadCallback
int TarGzUnpacker::gzFeedTarBuffer( unsigned char* buff, size_t buffsize )
{
  static size_t bytes_fed = 0;
  if( buffsize%TAR_BLOCK_SIZE !=0 ) {
    log_e("[ERROR] gzFeedTarBuffer Can't unmerge tar blocks (%d bytes) from gz block (%d bytes)\n", buffsize, GZIP_BUFF_SIZE);
    setError( ESP32_TARGZ_TAR_ERR_GZDEFL_FAIL );
    return 0;
  }
  uint32_t blockpos = gzTarBlockPos%blockmod;
  memcpy( buff, output_buffer/*uzlib_buffer*/+(TAR_BLOCK_SIZE*blockpos), TAR_BLOCK_SIZE );
  bytes_fed += TAR_BLOCK_SIZE;
  log_v("[TGZ INFO][tarbuf<-gzbuf] block #%d (%d mod %d) at output_buffer[%d] (%d bytes, total %d)", blockpos, gzTarBlockPos, blockmod, (TAR_BLOCK_SIZE*blockpos), buffsize, bytes_fed );
  gzTarBlockPos++;
  return TAR_BLOCK_SIZE;
}



/*
bool TarGzUnpacker::tarGzExpanderNoTempFile( Stream* stream, fs::FS destFS, const char* destFolder )
{
}
*/

// uncompress gz sourceFile directly to untar, no intermediate file
bool TarGzUnpacker::tarGzExpanderNoTempFile( fs::FS sourceFS, const char* sourceFile, fs::FS destFS, const char* destFolder )
{
  tarGzClearError();
  initFSCallbacks();
  if (!tgzLogger ) {
    setLoggerCallback( targzPrintLoggerCallback );
  }
  if( !gzProgressCallback ) {
    setGzProgressCallback( defaultProgressCallback );
  }
  if( !tarProgressCallback ) {
    setTarProgressCallback( tarNullProgressCallback );
  }
  if( !tarMessageCallback ) {
    setTarMessageCallback( targzNullLoggerCallback );
  }
  if( gzProgressCallback && gzProgressCallback == tarProgressCallback ) {
    log_v("Disabling colliding gzProgressCallback");
    setGzProgressCallback( targzNullProgressCallback );
  }

  if( ESP.getFreeHeap() < GZIP_DICT_SIZE+GZIP_BUFF_SIZE ) {
    log_e("Insufficient heap to decompress (available:%d, needed:%d), aborting", ESP.getFreeHeap(), GZIP_DICT_SIZE+GZIP_BUFF_SIZE );
    setError( ESP32_TARGZ_HEAP_TOO_LOW );
    return false;
  } else {
    log_d("Current heap budget (available:%d, needed:%d)", ESP.getFreeHeap(), GZIP_DICT_SIZE+GZIP_BUFF_SIZE );
  }
  if( !sourceFS.exists( sourceFile ) ) {
    log_e("gzip file %s does not exist", sourceFile);
    setError( ESP32_TARGZ_UZLIB_INVALID_FILE );
    return false;
  }
  fs::File gz = sourceFS.open( sourceFile, FILE_READ );

  tgzLogger("[TGZ] Will direct-expand %s to %s\n", sourceFile, destFolder );

  if( !gzReadHeader( gz ) ) {
    log_e("[GZ ERROR] in tarGzExpanderNoTempFile: invalid gzip file or not enough space left on device ?");
    gz.close();
    setError( ESP32_TARGZ_UZLIB_INVALID_FILE );
    return false;
  }

  tarGzStream.gz = &gz;
  tarFS = &destFS;
  tarDestFolder = destFolder;

  if( !destFS.exists( tarDestFolder ) ) {
    destFS.mkdir( tarDestFolder );
  }

  untarredBytesCount = 0;
  gzTarBlockPos = 0;

  tarCallbacks = {
    unTarHeaderCallBack,
    tarReadGzStream,
    unTarStreamWriteCallback,
    unTarEndCallBack
  };

  TAR::tar_error_logger = tgzLogger;
  TAR::tar_debug_logger = tgzLogger; // comment this out if too verbose

  gzWriteCallback       = &gzProcessTarBuffer;
  tarReadGzStreamBytes = 0;

  totalFiles = 0;
  totalFolders = 0;

  firstblock = true; // trigger TAR setup from gzUncompress callback
  lastblock  = false;

  int ret = gzUncompress( false, true );

  gz.close();

  if( ret!=0 ) {
    log_e("gzUncompress returned error code %d", ret);
    setError( (tarGzErrorCode)ret );
    return false;
  }
  log_d("uzLib expander finished!");

  if( fstotalBytes &&  fsfreeBytes ) {
    log_d("[GZ Info] FreeBytes after expansion=%d", fsfreeBytes() );
  }

  return true;
}


// unzip sourceFS://sourceFile.tar.gz contents into destFS://destFolder
bool TarGzUnpacker::tarGzExpander( fs::FS sourceFS, const char* sourceFile, fs::FS destFS, const char* destFolder, const char* tempFile )
{
  tarGzClearError();
  initFSCallbacks();

  if( tempFile != nullptr ) {

    tgzLogger("[TGZ] Will expand using an intermediate file: %s\n", tempFile );

    mkdirp( &destFS, tempFile );

    if( gzExpander(sourceFS, sourceFile, destFS, tempFile) ) {
      log_d("[INFO] heap before tar-expanding: %d)", ESP.getFreeHeap());
      if( tarExpander(destFS, tempFile, destFS, destFolder) ) {
        // yay
      }
    }
    if( destFS.exists( tempFile ) ) destFS.remove( tempFile );

    return !tarGzHasError();
  } else {

    tgzLogger("[TGZ] Will expand without intermediate file\n" );

    if( gzProgressCallback && gzProgressCallback == tarProgressCallback ) {
      log_v("Disabling gzprogress callback for this instance");
      setGzProgressCallback( targzNullProgressCallback );
    }

    return tarGzExpanderNoTempFile( sourceFS, sourceFile, destFS, destFolder );
  }
}




// uncompress tar+gz stream (file or HTTP) to filesystem without intermediate tar file
bool TarGzUnpacker::tarGzStreamExpander( Stream *stream, fs::FS &destFS, const char* destFolder )
{

  if( !stream->available() ) {
    log_e("Bad stream, aborting");
    setError( ESP32_TARGZ_STREAM_ERROR );
    return false;
  }

  if( !gzProgressCallback ) {
    setGzProgressCallback( defaultProgressCallback );
  }

  tarGzStream.gz = stream;

  tarFS = &destFS;
  tarDestFolder = destFolder;
  if( !tarProgressCallback ) {
    setTarProgressCallback( tarNullProgressCallback );
  }
  if( !tarMessageCallback ) {
    setTarMessageCallback( targzNullLoggerCallback );
  }
  if( !destFS.exists( tarDestFolder ) ) {
    destFS.mkdir( tarDestFolder );
  }

  untarredBytesCount = 0;
  gzTarBlockPos = 0;

  tarCallbacks = {
    unTarHeaderCallBack,
    gzFeedTarBuffer,
    unTarStreamWriteCallback,
    unTarEndCallBack
  };

  TAR::tar_error_logger      = tgzLogger; // targzPrintLoggerCallback or tgzLogger
  TAR::tar_debug_logger      = tgzLogger; // comment this out if too verbose

  gzWriteCallback = &gzProcessTarBuffer;

  totalFiles = 0;
  totalFolders = 0;

  firstblock = true; // trigger TAR setup from gzUncompress callback
  lastblock  = false;

  bool use_dict = true;

#if defined ESP8266

  // calculate minimal ram for gzip
  int available_heap = (ESP.getFreeHeap()-(GZIP_DICT_SIZE+min_output_buffer_size+2048)); // leave 1k heap for the stack
  if( available_heap < 0 ) {
    use_dict = false;
    available_heap = (ESP.getFreeHeap()-(min_output_buffer_size+2048)); // leave 1k heap for the stack
    if( available_heap < 0 ) {
      setError( ESP32_TARGZ_HEAP_TOO_LOW );
      return false;
    }
  }

  // calculate minimal ram for tar
  size_t free_min_heap_blocks = available_heap / 1024;
  if( free_min_heap_blocks <1 ) {
    setError( ESP32_TARGZ_HEAP_TOO_LOW );
    return false;
  }
  // adjust gz->tar buffer size accordingly
  min_output_buffer_size = free_min_heap_blocks * 1024;
  if( min_output_buffer_size > GZIP_BUFF_SIZE ) min_output_buffer_size = GZIP_BUFF_SIZE; // cap to 4096

  min_output_buffer_size = 4096;

  blockmod = min_output_buffer_size / TAR_BLOCK_SIZE;
  tgzLogger("tarGzStreamExpander will unpack stream to %s folder using %d buffered bytes and %s dictionary\n", destFolder, min_output_buffer_size, use_dict ? "36Kb for the" : "NO" );



#endif



  int ret = gzUncompress( false, false, use_dict, false );

  if( ret!=0 ) {
    log_e("tarGzStreamExpander returned error code %d", ret);
    setError( (tarGzErrorCode)ret );
    return false;
  }

  return true;
}




