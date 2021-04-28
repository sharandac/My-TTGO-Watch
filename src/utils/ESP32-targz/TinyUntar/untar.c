#include "untar.h"

char *empty_string = "";
entry_callbacks_t *read_tar_callbacks = NULL;
unsigned char *read_buffer = NULL;
header_t header;
header_translated_t header_translated;
void *read_context_data = NULL;
int num_blocks;
int num_blocks_iterator;
int current_data_size;
int entry_index;
int empty_count;
int received_bytes;
int indatablock;
int tar_error = TAR_OK;

extern void (*tar_error_logger)(const char* subject, ...);
extern void (*tar_debug_logger)(const char* subject, ...);

void log_error(const char *message) {
  if(tar_error_logger) tar_error_logger("[TAR ERROR]: %s\n", message);
  //else printf("[TAR ERROR]: %s\n", message);
}

void log_debug(const char *message) {
  if(tar_debug_logger) tar_debug_logger("[TAR DEBUG]: %s\n", message);
  //else printf("[TAR DEBUG]: %s\n", message);
}

int parse_header(const unsigned char buffer[TAR_BLOCK_SIZE], header_t *header) {
  //log_debug("Copying tar header");
  memcpy(header, buffer, sizeof(header_t));
  return TAR_OK;
}

unsigned long long decode_base256( __attribute__((unused)) const unsigned char *buffer) {
  return TAR_OK;
}

__attribute__((unused)) static void dump_hex(const char *ptr, int length) {
  int i = 0;
  printf("DUMP: ");
  while(i < length) {
    printf("%c", (ptr[i] >= 0x20 ? ptr[i] : '.'));
    i++;
  }
  printf("\n");
  i = 0;
  while(i < length) {
    printf("%X ", ptr[i]);

    i++;
  }
  printf("\n\n");
}

char *trim(char *raw, int length) {
  int i = 0;
  int j = length - 1;
  int is_empty = 0;
  // Determine left padding.
  while((raw[i] == 0 || raw[i] == ' ')) {
    i++;
    if(i >= length) {
      is_empty = 1;
      break;
    }
  }
  if(is_empty == 1)
    return empty_string;
  // Determine right padding.
  while((raw[j] == 0 || raw[j] == ' ')) {
    j--;
    if(j <= i)
      break;
  }
  // Place the terminator.
  raw[j + 1] = 0;
  // Return an offset pointer.
  return &raw[i];
}

int translate_header(header_t *raw_header, header_translated_t *parsed) {
  char buffer[101];
  char *buffer_ptr;
  const int R_OCTAL = 8;
  //
  memcpy(buffer, raw_header->filename, 100);
  buffer_ptr = trim(buffer, 100);
  strcpy(parsed->filename, buffer_ptr);
  parsed->filename[strlen(buffer_ptr)] = 0;
  //
  memcpy(buffer, raw_header->filemode, 8);
  buffer_ptr = trim(buffer, 8);

  if(IS_BASE256_ENCODED(buffer) != 0)
      parsed->filemode = decode_base256((const unsigned char *)buffer_ptr);
  else
      parsed->filemode = strtoull(buffer_ptr, NULL, R_OCTAL);
  //
  memcpy(buffer, raw_header->uid, 8);
  buffer_ptr = trim(buffer, 8);

  if(IS_BASE256_ENCODED(buffer) != 0)
      parsed->uid = decode_base256((const unsigned char *)buffer_ptr);
  else
      parsed->uid = strtoull(buffer_ptr, NULL, R_OCTAL);
  //
  memcpy(buffer, raw_header->gid, 8);
  buffer_ptr = trim(buffer, 8);

  if(IS_BASE256_ENCODED(buffer) != 0)
      parsed->gid = decode_base256((const unsigned char *)buffer_ptr);
  else
      parsed->gid = strtoull(buffer_ptr, NULL, R_OCTAL);
  //
  memcpy(buffer, raw_header->filesize, 12);
  buffer_ptr = trim(buffer, 12);

  if(IS_BASE256_ENCODED(buffer) != 0)
      parsed->filesize = decode_base256((const unsigned char *)buffer_ptr);
  else
      parsed->filesize = strtoull(buffer_ptr, NULL, R_OCTAL);
  //
  memcpy(buffer, raw_header->mtime, 12);
  buffer_ptr = trim(buffer, 12);

  if(IS_BASE256_ENCODED(buffer) != 0)
      parsed->mtime = decode_base256((const unsigned char *)buffer_ptr);
  else
      parsed->mtime = strtoull(buffer_ptr, NULL, R_OCTAL);
  //
  memcpy(buffer, raw_header->checksum, 8);
  buffer_ptr = trim(buffer, 8);

  if(IS_BASE256_ENCODED(buffer) != 0)
      parsed->checksum = decode_base256((const unsigned char *)buffer_ptr);
  else
      parsed->checksum = strtoull(buffer_ptr, NULL, R_OCTAL);
  //
  parsed->type = get_type_from_char(raw_header->type);

  memcpy(buffer, raw_header->link_target, 100);
  buffer_ptr = trim(buffer, 100);
  strcpy(parsed->link_target, buffer_ptr);
  parsed->link_target[strlen(buffer_ptr)] = 0;
  //
  memcpy(buffer, raw_header->ustar_indicator, 6);
  buffer_ptr = trim(buffer, 6);
  strcpy(parsed->ustar_indicator, buffer_ptr);
  parsed->ustar_indicator[strlen(buffer_ptr)] = 0;
  //
  memcpy(buffer, raw_header->ustar_version, 2);
  buffer_ptr = trim(buffer, 2);
  strcpy(parsed->ustar_version, buffer_ptr);
  parsed->ustar_version[strlen(buffer_ptr)] = 0;

  if(strcmp(parsed->ustar_indicator, "ustar") == 0) {
    //
    memcpy(buffer, raw_header->user_name, 32);
    buffer_ptr = trim(buffer, 32);
    strcpy(parsed->user_name, buffer_ptr);
    parsed->user_name[strlen(buffer_ptr)] = 0;
    //
    memcpy(buffer, raw_header->group_name, 32);
    buffer_ptr = trim(buffer, 32);
    strcpy(parsed->group_name, buffer_ptr);
    parsed->group_name[strlen(buffer_ptr)] = 0;
    //
    memcpy(buffer, raw_header->device_major, 8);
    buffer_ptr = trim(buffer, 8);

    if(IS_BASE256_ENCODED(buffer) != 0)
        parsed->device_major = decode_base256((const unsigned char *)buffer_ptr);
    else
        parsed->device_major = strtoull(buffer_ptr, NULL, R_OCTAL);
    //
    memcpy(buffer, raw_header->device_minor, 8);
    buffer_ptr = trim(buffer, 8);

    if(IS_BASE256_ENCODED(buffer) != 0) {
      parsed->device_minor = decode_base256((const unsigned char *)buffer_ptr);
    } else {
      parsed->device_minor = strtoull(buffer_ptr, NULL, R_OCTAL);
    }
  } else {
    strcpy(parsed->user_name, "");
    strcpy(parsed->group_name, "");

    parsed->device_major = 0;
    parsed->device_minor = 0;
  }
  return TAR_OK;
}

int read_block(unsigned char *buffer) {
  char message[200];
  int num_read;

  //log_debug("Will read block");

  if( read_tar_callbacks->read_cb == NULL ) {
    log_error("read_cb() has NOT been defined" );
    tar_error = TAR_ERR_READBLOCK_FAIL;
    return TAR_ERROR;
  }

  num_read = read_tar_callbacks->read_cb(buffer, TAR_BLOCK_SIZE);

  if(num_read < TAR_BLOCK_SIZE) {
    snprintf(message,
      200,
      "Read has stopped short at (%d) count "
      "rather than (%d). Quitting under error.",
      num_read, TAR_BLOCK_SIZE
    );
    tar_error = TAR_ERR_READBLOCK_FAIL;
    log_error(message);
    return TAR_ERROR;
  }
  return TAR_OK;
}


int expand_tar_data_block() {

  if(num_blocks_iterator >= num_blocks - 1)
    current_data_size = get_last_block_portion_size(header_translated.filesize);
  else
    current_data_size = TAR_BLOCK_SIZE;

  read_buffer[current_data_size] = 0;

  if(read_tar_callbacks->write_cb(&header_translated, entry_index, read_context_data, read_buffer, current_data_size) != 0) {
    //log_error("Data callback failed.");
    return TAR_ERR_DATACB_FAIL;
  }
  num_blocks_iterator++;
  received_bytes += current_data_size;

  return TAR_OK;

}

void tar_abort( const char* msgstr, int iserror ) {
  if( iserror == 1 ) {
    log_error( msgstr );
  } else {
    if( msgstr[0] != 0 ) {
      log_debug( msgstr );
    }
  }
  if( read_buffer != NULL ) {
    free( read_buffer );
    read_buffer = NULL;
  }
  read_tar_callbacks = NULL;
}


int tar_setup(  entry_callbacks_t *callbacks, void *context_data ) {
  //log_debug("entering tar setup");
  tar_error = TAR_OK;
  read_tar_callbacks = callbacks;
  read_context_data = context_data;
  read_buffer = (unsigned char*)malloc(TAR_BLOCK_SIZE + 1);
  if( read_buffer == NULL ) {
    return TAR_ERROR_HEAP;
  }
  entry_index = 0;
  empty_count = 0;
  indatablock = -1;
  read_buffer[TAR_BLOCK_SIZE] = 0;
  return TAR_OK;
}


int tar_datablock_step() {

  static int block_read = 0;

  if(num_blocks_iterator < num_blocks) {
    if( block_read == 0 ) {
      if(read_block( read_buffer ) != 0) {
        tar_abort("Could not read block. File too short.", 1);
        tar_error = TAR_ERR_READBLOCK_FAIL;
        return tar_error;
      }
      block_read = 1;
    } else {
      int res = expand_tar_data_block();
      if( res != 0 ) {
        tar_abort("Data callback failed", 1);
        return res;
      }
      block_read = 0;
    }
    return TAR_CONTINUE;
  } else {
    indatablock = -1;
    if(read_tar_callbacks->end_cb(&header_translated, entry_index, read_context_data) != 0) {
      tar_abort("End callback failed.", 1);
      tar_error = TAR_ERR_FOOTERCB_FAIL;
      return tar_error;
    }
    entry_index++;
    return TAR_ERROR;
  }
}


int tar_step() {

  static int readstep = 0;

  if( tar_error != TAR_OK ) {
    tar_abort("tar expanding interrupted!", 1);
    return tar_error;
  }

  if( indatablock == 0 ) {
    return tar_datablock_step();
  }

  if(empty_count >= 2) {
    tar_abort("tar expanding done!", 0);
    return TAR_EXPANDING_DONE;
  }

  if( readstep == 0 ) {
    if(read_block( read_buffer ) != 0) {
      tar_abort("tar expanding done!", 0);
      return TAR_ERROR;
    }
    readstep = 1;
    return TAR_OK;
  } else {
    readstep = 0;
  }

  // If we haven't yet determined what format to support, read the
  // header of the next entry, now. This should be done only at the
  // top of the archive.
  if( parse_header(read_buffer, &header) != 0) {
      tar_abort("Could not understand the header of the first entry in the TAR.", 1);
      tar_error = TAR_ERR_HEADERPARSE_FAIL;
      return tar_error;
  } else if(strlen(header.filename) == 0) {
      empty_count++;
      //entry_index++;
      return TAR_OK;
  } else {
    if(translate_header(&header, &header_translated) != 0) {
      tar_abort("Could not translate header.", 1);
      tar_error = TAR_ERR_HEADERTRANS_FAIL;
      return tar_error;
    }

    if(read_tar_callbacks->header_cb(&header_translated, entry_index, read_context_data) != 0) {
      tar_abort("An error occured during Header callback.", 1);
      tar_error = TAR_ERR_HEADERCB_FAIL;
      return tar_error;
    }
    num_blocks_iterator = 0;
    received_bytes = 0;
    num_blocks = GET_NUM_BLOCKS(header_translated.filesize);
    indatablock = 0;

    int res = tar_datablock_step();
    if( num_blocks > 0 && res < 0 ) {
      char message[200];
      snprintf(message, 200, "tar_datablock_step return code (%d)", res );
      log_error(message);
      return res;
    }
    return TAR_OK;
  }

}


int read_tar_step() {
  if( read_tar_callbacks == NULL ) {
    //tar_abort("No callbacks defined!", 1);
    return TAR_ERROR;
  }
  int res = tar_step();

  if( res < 0 ) {
    if( res != TAR_ERROR ) {
      char message[200];
      snprintf(message, 200, "read_tar return code (%d)", res );
      tar_abort(message, 1);
      return res;
    } else {
      //tar_abort("Unpacking success!", 0);
      return TAR_OK;
    }
  } else {
    return res == TAR_EXPANDING_DONE ? TAR_EXPANDING_DONE : TAR_OK;
  }
}


int read_tar( entry_callbacks_t *callbacks, void *context_data) {
  if( read_tar_callbacks != NULL ) {
    read_tar_callbacks = NULL;
  }
  read_tar_callbacks = callbacks;
  read_context_data = context_data;
  read_buffer = (unsigned char*)malloc(TAR_BLOCK_SIZE + 1);

  entry_index = 0;
  empty_count = 0;
  indatablock = -1;

  read_buffer[TAR_BLOCK_SIZE] = 0;
  // The end of the file is represented by two empty entries (which we
  // expediently identify by filename length).

  while(empty_count < 2) {
    if(read_block( read_buffer ) != 0)
        break;

    // If we haven't yet determined what format to support, read the
    // header of the next entry, now. This should be done only at the
    // top of the archive.

    if(parse_header(read_buffer, &header) != 0) {
      tar_abort("Could not understand the header of the first entry in the TAR.", 1);
      tar_error = TAR_ERR_HEADERPARSE_FAIL;
      return tar_error;
    } else if(strlen(header.filename) == 0) {
      empty_count++;
    } else {
      if(translate_header(&header, &header_translated) != 0) {
        tar_abort("Could not translate header.", 1);
        tar_error = TAR_ERR_HEADERTRANS_FAIL;
        return tar_error;
      }
      if(callbacks->header_cb(&header_translated, entry_index, context_data) != 0) {
        tar_abort("An error occured during Header callback.", 1);
        tar_error = TAR_ERR_HEADERCB_FAIL;
        return tar_error;
      }
      int i = 0;
      received_bytes = 0;
      num_blocks = GET_NUM_BLOCKS(header_translated.filesize);
      while(i < num_blocks) {
        if(read_block( read_buffer ) != 0) {
          tar_abort("Could not read block. File too short.", 1);
          tar_error = TAR_ERR_READBLOCK_FAIL;
          return tar_error;
        }

        if(i >= num_blocks - 1)
          current_data_size = get_last_block_portion_size(header_translated.filesize);
        else
          current_data_size = TAR_BLOCK_SIZE;

        read_buffer[current_data_size] = 0;

        if(callbacks->write_cb(&header_translated, entry_index, context_data, read_buffer, current_data_size) != 0) {
          tar_abort("Data callback failed.", 1);
          tar_error = TAR_ERR_DATACB_FAIL;
          return tar_error;
        }
        i++;
        received_bytes += current_data_size;
      }
      if(callbacks->end_cb(&header_translated, entry_index, context_data) != 0) {
        tar_abort("End callback failed.", 1);
        tar_error = TAR_ERR_FOOTERCB_FAIL;
        return tar_error;
      }
    }
    entry_index++;
  }

  tar_abort("tar expanding done!", 0);
  return TAR_OK;
}


void dump_header(header_translated_t *header) {
  if( !tar_debug_logger ) return;
  if( header->type == T_DIRECTORY ) return;
  tar_debug_logger("===========================================\n");
  tar_debug_logger("      filename: %s\n", header->filename);
  tar_debug_logger("      filemode: 0%o (%llu)\n", (unsigned int)header->filemode, header->filemode);
  tar_debug_logger("           uid: 0%o (%llu)\n", (unsigned int)header->uid, header->uid);
  tar_debug_logger("           gid: 0%o (%llu)\n", (unsigned int)header->gid, header->gid);
  tar_debug_logger("      filesize: 0%o (%llu)\n", (unsigned int)header->filesize, header->filesize);
  tar_debug_logger("         mtime: 0%o (%llu)\n", (unsigned int)header->mtime, header->mtime);
  tar_debug_logger("      checksum: 0%o (%llu)\n", (unsigned int)header->checksum, header->checksum);
  tar_debug_logger("          type: %d\n", header->type);
  tar_debug_logger("   link_target: %s\n", header->link_target);
  //tar_debug_logger("\n");

  tar_debug_logger("     ustar ind: %s\n", header->ustar_indicator);
  tar_debug_logger("     ustar ver: %s\n", header->ustar_version);
  tar_debug_logger("     user name: %s\n", header->user_name);
  tar_debug_logger("    group name: %s\n", header->group_name);
  tar_debug_logger("device (major): %llu\n", header->device_major);
  tar_debug_logger("device (minor): %llu\n", header->device_minor);
  //tar_debug_logger("\n");

  tar_debug_logger("  data blocks = %d\n", GET_NUM_BLOCKS(header->filesize));
  tar_debug_logger("  last block portion = %d\n", get_last_block_portion_size(header->filesize));
  tar_debug_logger("===========================================\n");
  //tar_debug_logger("\n");
}

enum entry_type_e get_type_from_char(char raw_type) {
  switch(raw_type) {
    case TAR_T_NORMAL1:
    case TAR_T_NORMAL2:
        return T_NORMAL;

    case TAR_T_HARD:
        return T_HARDLINK;

    case TAR_T_SYMBOLIC:
        return T_SYMBOLIC;

    case TAR_T_CHARSPECIAL:
        return T_CHARSPECIAL;

    case TAR_T_BLOCKSPECIAL:
        return T_CHARSPECIAL;

    case TAR_T_DIRECTORY:
        return T_DIRECTORY;

    case TAR_T_FIFO:
        return T_FIFO;

    case TAR_T_CONTIGUOUS:
        return T_CONTIGUOUS;

    case TAR_T_GLOBALEXTENDED:
        return T_GLOBALEXTENDED;

    case TAR_T_EXTENDED:
        return T_EXTENDED;
  }

  return T_OTHER;
}

inline int get_last_block_portion_size(int filesize) {
  const int partial = filesize % TAR_BLOCK_SIZE;
  return (partial > 0 ? partial : TAR_BLOCK_SIZE);
}
