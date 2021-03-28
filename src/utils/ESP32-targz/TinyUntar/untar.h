#ifndef __UNTAR_H
#define __UNTAR_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define IS_BASE256_ENCODED(buffer) (((unsigned char)buffer[0] & 0x80) > 0)
#define GET_NUM_BLOCKS(filesize) (int)ceil((double)filesize / (double)TAR_BLOCK_SIZE)

inline int get_last_block_portion_size(int filesize);

#ifdef _MSC_VER
	#define strtoull _strtoui64
	#define snprintf _snprintf
#endif

#define TAR_T_NORMAL1 0
#define TAR_T_NORMAL2 '0'
#define TAR_T_HARD '1'
#define TAR_T_SYMBOLIC '2'
#define TAR_T_CHARSPECIAL '3'
#define TAR_T_BLOCKSPECIAL '4'
#define TAR_T_DIRECTORY '5'
#define TAR_T_FIFO '6'
#define TAR_T_CONTIGUOUS '7'
#define TAR_T_GLOBALEXTENDED 'g'
#define TAR_T_EXTENDED 'x'

#define TAR_EXPANDING_DONE        2
#define TAR_CONTINUE              1
#define TAR_OK                    0
#define TAR_ERROR                -1

#define TAR_ERR_DATACB_FAIL      -2
#define TAR_ERR_HEADERCB_FAIL    -3
#define TAR_ERR_FOOTERCB_FAIL    -4
#define TAR_ERR_READBLOCK_FAIL   -5
#define TAR_ERR_HEADERTRANS_FAIL -6
#define TAR_ERR_HEADERPARSE_FAIL -7
#define TAR_ERROR_HEAP           -8

#define TAR_BLOCK_SIZE 512

#define TAR_HT_PRE11988 1
#define TAR_HT_P10031 2

enum entry_type_e { T_NORMAL, T_HARDLINK, T_SYMBOLIC, T_CHARSPECIAL,
					T_BLOCKSPECIAL, T_DIRECTORY, T_FIFO, T_CONTIGUOUS,
					T_GLOBALEXTENDED, T_EXTENDED, T_OTHER };

// Describes a header for TARs conforming to pre-POSIX.1-1988 .
struct header_s
{
	char filename[100];
	char filemode[8];
	char uid[8];
	char gid[8];
	char filesize[12];
	char mtime[12];
	char checksum[8];
	char type;
	char link_target[100];

	char ustar_indicator[6];
	char ustar_version[2];
	char user_name[32];
	char group_name[32];
	char device_major[8];
	char device_minor[8];
};

typedef struct header_s header_t;

struct header_translated_s
{
	char filename[101];
	unsigned long long filemode;
	unsigned long long uid;
	unsigned long long gid;
	unsigned long long filesize;
	unsigned long long mtime;
	unsigned long long checksum;
	enum entry_type_e type;
	char link_target[101];

	char ustar_indicator[6];
	char ustar_version[3];
	char user_name[32];
	char group_name[32];
	unsigned long long device_major;
	unsigned long long device_minor;
};

typedef struct header_translated_s header_translated_t;

typedef int (*entry_header_callback_t)(header_translated_t *header,
										int entry_index,
										void *context_data);

typedef int(*entry_read_callback_t)(unsigned char* buff,
                                     size_t buffsize );

typedef int (*entry_write_callback_t)(header_translated_t *header,
										int entry_index,
										void *context_data,
										unsigned char *block,
										int length);

typedef int (*entry_end_callback_t)(header_translated_t *header,
									int entry_index,
									void *context_data);



struct entry_callbacks_s
{
	entry_header_callback_t header_cb;
    entry_read_callback_t read_cb;
	entry_write_callback_t write_cb;
	entry_end_callback_t end_cb;
};

typedef struct entry_callbacks_s entry_callbacks_t;


__attribute__((unused))static void (*tar_error_logger)(const char* subject, ...);
__attribute__((unused))static void (*tar_debug_logger)(const char* subject, ...);

int tar_setup(  entry_callbacks_t *callbacks, void *context_data );
void tar_abort( const char* msgstr, int iserror);
//int read_tar_data_block();
int read_tar( entry_callbacks_t *callbacks, void *context_data);
int read_tar_step();
void dump_header(header_translated_t *header);
unsigned long long decode_base256(unsigned const char *buffer);
char *trim(char *raw, int length);
int parse_header(unsigned const char buffer[512], header_t *header);
int translate_header(header_t *raw_header, header_translated_t *parsed);
enum entry_type_e get_type_from_char(char raw_type);

#endif

