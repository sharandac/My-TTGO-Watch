/*
** This source file is part of MY-BASIC
**
** For the latest info, see https://github.com/paladin-t/my_basic/
**
** Copyright (C) 2011 - 2018 Wang Renxin
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of
** this software and associated documentation files (the "Software"), to deal in
** the Software without restriction, including without limitation the rights to
** use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
** the Software, and to permit persons to whom the Software is furnished to do so,
** subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
** FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
** COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
** IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifdef _MSC_VER
#  ifndef _CRT_SECURE_NO_WARNINGS
#   define _CRT_SECURE_NO_WARNINGS
# endif /* _CRT_SECURE_NO_WARNINGS */
#endif /* _MSC_VER */

#include "my_basic.h"
#if defined ARDUINO && !defined MB_CP_ARDUINO
# define MB_CP_ARDUINO
#endif /* ARDUINO && !MB_CP_ARDUINO */
#ifdef MB_CP_ARDUINO
# ifndef MB_DISABLE_LOAD_FILE
#   define MB_DISABLE_LOAD_FILE  // Corrado: questa poi è da abilitare
# endif /* MB_DISABLE_LOAD_FILE */
# ifndef MB_MANUAL_REAL_FORMATTING
#   define MB_MANUAL_REAL_FORMATTING
# endif /* MB_MANUAL_REAL_FORMATTING */
#endif /* MB_CP_ARDUINO */
#ifdef MB_CP_VC
# include <conio.h>
# include <locale.h>
# include <malloc.h>
# include <Windows.h>
#else /* MB_CP_VC */
# include <stdint.h>
#endif /* MB_CP_VC */
#ifndef MB_CP_ARDUINO
# include <memory.h>
#endif /* MB_CP_ARDUINO */
#include <assert.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef MB_CP_VC
# pragma warning(push)
# pragma warning(disable : 4127)
# pragma warning(disable : 4305)
# pragma warning(disable : 4309)
# pragma warning(disable : 4805)
# pragma warning(disable : 4996)
#endif /* MB_CP_VC */

#ifdef MB_CP_CLANG
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wunused-function"
# pragma clang diagnostic ignored "-Wunused-variable"
#endif /* MB_CP_CLANG */

#ifdef MB_CP_BORLANDC
# pragma warn -8004
# pragma warn -8008
# pragma warn -8012
#endif /* MB_CP_BORLANDC */

#ifdef MB_COMPACT_MODE
# pragma pack(1)
#endif /* MB_COMPACT_MODE */

/*
** {========================================================
** Data type declarations
*/

/** Macros */

/* Version information */
#define MB_VER_MAJOR 1
#define MB_VER_MINOR 2
#define MB_VER_REVISION 0
#define MB_VER_SUFFIX
#define MB_VERSION ((MB_VER_MAJOR * 0x01000000) + (MB_VER_MINOR * 0x00010000) + (MB_VER_REVISION))
#define MB_MAKE_STRINGIZE(A) #A
#define MB_STRINGIZE(A) MB_MAKE_STRINGIZE(A)
#if MB_VER_REVISION == 0
# define MB_VERSION_STRING MB_STRINGIZE(MB_VER_MAJOR.MB_VER_MINOR MB_VER_SUFFIX)
#else /* MB_VER_REVISION == 0 */
# define MB_VERSION_STRING MB_STRINGIZE(MB_VER_MAJOR.MB_VER_MINOR.MB_VER_REVISION MB_VER_SUFFIX)
#endif /* MB_VER_REVISION == 0 */

/* Define as 1 to create hash table nodes lazily, 0 obligingly */
#ifndef _LAZY_HASH_TABLE
# define _LAZY_HASH_TABLE 1
#endif /* _LAZY_HASH_TABLE */

/* Define as 1 to treat warning as error, 0 just leave it */
#ifndef _WARNING_AS_ERROR
# define _WARNING_AS_ERROR 0
#endif /* _WARNING_AS_ERROR */

/* Define as 1 to automatically raise error during popping argument, 0 just return an error result */
#ifndef _SIMPLE_ARG_ERROR
# define _SIMPLE_ARG_ERROR 0
#endif /* _SIMPLE_ARG_ERROR */

/* Define as 1 to use a comma to PRINT a new line, 0 to use a semicolon */
#ifndef _COMMA_AS_NEWLINE
# define _COMMA_AS_NEWLINE 0
#endif /* _COMMA_AS_NEWLINE */

/* Define as 1 to enable multiline statement */
#ifndef _MULTILINE_STATEMENT
# define _MULTILINE_STATEMENT 1
#endif /* _MULTILINE_STATEMENT */

/* Hash table size */
#ifndef _HT_ARRAY_SIZE_DEFAULT
# define _HT_ARRAY_SIZE_TINY 1
# define _HT_ARRAY_SIZE_XSMALL 31
# define _HT_ARRAY_SIZE_SMALL 193
# define _HT_ARRAY_SIZE_MID 1543
# define _HT_ARRAY_SIZE_BIG 12289
# define _HT_ARRAY_SIZE_DEFAULT _HT_ARRAY_SIZE_XSMALL
# define _HT_ARRAY_SIZE_GC _HT_ARRAY_SIZE_SMALL
# define _HT_ARRAY_SIZE_GLOBALSCOPES _HT_ARRAY_SIZE_SMALL
#endif /* _HT_ARRAY_SIZE_DEFAULT */



/* Max length of a single symbol */
#ifndef _SINGLE_SYMBOL_MAX_LENGTH
# define _SINGLE_SYMBOL_MAX_LENGTH 128
#endif /* _SINGLE_SYMBOL_MAX_LENGTH */

/* Buffer length of some string operations */
#ifndef _INPUT_MAX_LENGTH
# define _INPUT_MAX_LENGTH 256
#endif /* _INPUT_MAX_LENGTH */
#ifndef _TEMP_FORMAT_MAX_LENGTH
# define _TEMP_FORMAT_MAX_LENGTH 32
#endif /* _TEMP_FORMAT_MAX_LENGTH */
#ifndef _LAMBDA_NAME_MAX_LENGTH
# define _LAMBDA_NAME_MAX_LENGTH 32
#endif /* _LAMBDA_NAME_MAX_LENGTH */

/* Localization specifier */
#ifndef _LOCALIZATION_USEING
# define _LOCALIZATION_USEING 1
#endif /* _LOCALIZATION_USEING */
#ifndef _LOCALIZATION_STR
# define _LOCALIZATION_STR ""
#endif /* _LOCALIZATION_STR */

/* Helper */
#ifdef MB_COMPACT_MODE
# define _PACK1 : 1
# define _PACK2 : 2
# define _PACK8 : 8
#else /* MB_COMPACT_MODE */
# define _PACK1
# define _PACK2
# define _PACK8
#endif /* MB_COMPACT_MODE */

#ifndef _UNALIGNED_ARG
# if defined MB_CP_VC && defined MB_OS_WIN64
#   ifdef MB_COMPACT_MODE
#     define _UNALIGNED_ARG __unaligned
#   else /* MB_COMPACT_MODE */
#     define _UNALIGNED_ARG
#   endif /* MB_COMPACT_MODE */
# else
#   define _UNALIGNED_ARG
# endif
#endif /* _UNALIGNED_ARG */

#ifndef sgn
# define sgn(__v) ((__v) ? ((__v) > 0 ? 1 : -1) : 0)
#endif /* sgn */

#ifndef islower
# define islower(__c) ((__c) >= 'a' && (__c) <= 'z')
#endif /* islower */
#ifndef toupper
# define toupper(__c) (islower(__c) ? ((__c) - 'a' + 'A') : (__c))
#endif /* toupper */

#ifndef countof
# define countof(__a) (sizeof(__a) / sizeof(*(__a)))
#endif /* countof */

#ifndef _mb_check_exit
# define _mb_check_exit(__expr, __exit) do { if((__expr) != MB_FUNC_OK) goto __exit; } while(0)
#endif /* _mb_check_exit */
#ifndef _mb_check_mark_exit
# define _mb_check_mark_exit(__expr, __result, __exit) do { __result = (__expr); if(__result != MB_FUNC_OK) goto __exit; } while(0)
#endif /* _mb_check_mark_exit */

/** Collections */

/* Collection functors */
#define _OP_RESULT_NORMAL 0
#define _OP_RESULT_DEL_NODE -1

typedef int (* _common_compare_t)(void*, void*);
typedef int (* _common_operation_t)(void*, void*);

/* List */
typedef _common_compare_t _ls_compare_t;
typedef _common_operation_t _ls_operation_t;

typedef struct _ls_node_t {
	void* data;
	struct _ls_node_t* prev;
	struct _ls_node_t* next;
	void* extra;
} _ls_node_t;

/* Dictionary */
typedef unsigned (* _ht_hash_t)(void*, void*);
typedef _common_compare_t _ht_compare_t;
typedef _common_operation_t _ht_operation_t;

typedef struct _ht_node_t {
	_ls_operation_t free_extra;
	_ht_compare_t compare;
	_ht_hash_t hash;
	unsigned array_size;
	unsigned count;
	_ls_node_t** array;
} _ht_node_t;

/** Normal enum/struct/union/const, etc. */

#ifdef MB_ENABLE_FULL_ERROR
/* Error description text */
MBCONST static const char* const _ERR_DESC[] = {
	"No error",
	/** Common */
	"Function already exists",
	"Function not exists",
	"Not supported",
	/** Parsing */
	"Open file failed",
	"Symbol too long",
	"Invalid character",
	"Invalid module",
	/** Running */
	"Empty program",
	"Program too long",
	"Syntax error",
	"Out of memory",
	"Overflow",
	"Unexpected type",
	"Invalid string",
	"Integer expected",
	"Number expected",
	"String expected",
	"Variable expected",
	"Index out of bound",
	"Cannot find with given index",
	"Too many dimensions",
	"Rank out of bound",
	"Invalid identifier usage",
	"Duplicate identifier",
	"Incomplete structure",
	"Label not exists",
	"No return point",
	"Colon expected",
	"Comma expected",
	"Comma or semicolon expected",
	"Open bracket expected",
	"Close bracket expected",
	"Nested too much",
	"Operation failed",
	"Operator expected",
	"Assign operator expected",
	"THEN statement expected",
	"ELSE statement expected",
	"ENDIF statement expected",
	"TO statement expected",
	"NEXT statement expected",
	"UNTIL statement expected",
	"Loop variable expected",
	"Jump label expected",
	"Calculation error",
	"Invalid expression",
	"Divide by zero",
	"Wrong function reached",
	"Cannot suspend in a routine",
	"Cannot mix instructional and structured sub routines",
	"Invalid routine",
	"Routine expected",
	"Duplicate routine",
	"Invalid class",
	"Class expected",
	"Duplicate class",
	"HASH and COMPARE must be provided together",
	"Invalid lambda",
	"Empty collection",
	"List expected",
	"Invalid iterator",
	"Iterable expected",
	"Collection expected",
	"Collection or iterator expected",
	"Referenced type expected",
	/** Extended abort */
	"Extended abort"
};

mb_static_assert(countof(_ERR_DESC) == SE_COUNT);
#endif /* MB_ENABLE_FULL_ERROR */

/* Data type */
typedef enum _data_e {
	_DT_INVALID = -1,
	_DT_NIL = 0,
	_DT_UNKNOWN,
	_DT_INT,
	_DT_REAL,
	_DT_STRING,
	_DT_TYPE,
	_DT_USERTYPE,
#ifdef MB_ENABLE_USERTYPE_REF
	_DT_USERTYPE_REF,
#endif /* MB_ENABLE_USERTYPE_REF */
	_DT_FUNC,
	_DT_VAR,
	_DT_ARRAY,
#ifdef MB_ENABLE_COLLECTION_LIB
	_DT_LIST,
	_DT_LIST_IT,
	_DT_DICT,
	_DT_DICT_IT,
#endif /* MB_ENABLE_COLLECTION_LIB */
	_DT_LABEL, /* Label type, used for GOTO, GOSUB statement */
#ifdef MB_ENABLE_CLASS
	_DT_CLASS, /* Object instance */
#endif /* MB_ENABLE_CLASS */
	_DT_ROUTINE, /* User defined sub routine in script */
#ifdef MB_ENABLE_LAMBDA
	_DT_OUTER_SCOPE,
#endif /* MB_ENABLE_LAMBDA */
	_DT_SEP, /* Separator */
#ifdef MB_ENABLE_SOURCE_TRACE
	_DT_PREV_IMPORT,
	_DT_POST_IMPORT,
#endif /* MB_ENABLE_SOURCE_TRACE */
	_DT_EOS /* End of statement */
} _data_e;

#ifdef MB_ENABLE_COLLECTION_LIB
# define _HAS_REF_OBJ_LOCK
#endif /* MB_ENABLE_COLLECTION_LIB */

#ifdef _HAS_REF_OBJ_LOCK
typedef short mb_lock_t;
#endif /* _HAS_REF_OBJ_LOCK */

struct _ref_t;

typedef void (* _unref_func_t)(struct _ref_t*, void*);

#define _NONE_REF 1

#ifndef _ref_count_t
typedef unsigned _ref_count_t;
#endif /* _ref_count_t */

/* The reference structure should be always at the head of an object */
typedef struct _ref_t {
	_ref_count_t* count;
	_ref_count_t* weak_count;
	_unref_func_t on_unref;
	_data_e type _PACK8;
	struct mb_interpreter_t* s;
} _ref_t;

typedef struct _gc_t {
	_ht_node_t* table;
	_ht_node_t* recursive_table;
	_ht_node_t* collected_table;
	_ht_node_t* valid_table;
	unsigned char collecting;
	bool_t disabled _PACK1;
} _gc_t;

#ifdef MB_ENABLE_USERTYPE_REF
typedef struct _calculation_operator_info_t {
	mb_meta_operator_t is;
	mb_meta_operator_t add;
	mb_meta_operator_t sub;
	mb_meta_operator_t mul;
	mb_meta_operator_t div;
	mb_meta_operator_t neg;
} _calculation_operator_info_t;

typedef struct _usertype_ref_t {
	_ref_t ref;
	void* usertype;
	mb_dtor_func_t dtor;
	mb_clone_func_t clone;
	mb_hash_func_t hash;
	mb_cmp_func_t cmp;
	mb_fmt_func_t fmt;
#ifdef MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF
	mb_alive_value_checker_t alive_checker;
#endif /* MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF */
	_calculation_operator_info_t* calc_operators;
	mb_meta_func_t coll_func;
	mb_meta_func_t generic_func;
} _usertype_ref_t;
#endif /* MB_ENABLE_USERTYPE_REF */

typedef struct _func_t {
	char* name;
	mb_func_t pointer;
} _func_t;

#define _PATHING_NONE 0
#define _PATHING_NORMAL 1
#define _PATHING_UNKNOWN_FOR_NOT_FOUND 2
#define _PATHING_UPVALUE 3

#define _PN(__b) ((!!(__b)) ? (_PATHING_NORMAL) : (_PATHING_NONE))
#define _PU(__b) ((!!(__b)) ? (_PATHING_UNKNOWN_FOR_NOT_FOUND) : (_PATHING_NONE))

typedef struct _var_t {
	char* name;
	struct _object_t* data;
#ifdef MB_ENABLE_CLASS
	unsigned char pathing _PACK2;
	bool_t is_me _PACK1;
#endif /* MB_ENABLE_CLASS */
} _var_t;

typedef struct _array_t {
#ifdef MB_ENABLE_ARRAY_REF
	_ref_t ref;
#endif /* MB_ENABLE_ARRAY_REF */
	char* name;
	_data_e type;
#ifndef MB_SIMPLE_ARRAY
	_data_e* types;
#endif /* MB_SIMPLE_ARRAY */
	void* raw;
	unsigned count;
	unsigned char dimension_count;
	unsigned dimensions[MB_MAX_DIMENSION_COUNT];
} _array_t;

#ifdef MB_ENABLE_COLLECTION_LIB
typedef struct _array_helper_t {
	struct mb_interpreter_t* s;
	_array_t* array;
	int index;
} _array_helper_t;
#endif /* MB_ENABLE_COLLECTION_LIB */

#ifdef MB_ENABLE_COLLECTION_LIB
typedef struct _list_t {
	_ref_t ref;
	mb_lock_t lock;
	_ls_node_t* list;
	_ls_node_t* cached_node;
	int cached_index;
	int_t count;
	int_t* range_begin;
} _list_t;

typedef struct _list_it_t {
	_ref_t weak_ref;
	_list_t* list;
	bool_t locking _PACK1;
	union {
		_ls_node_t* node;
		int_t ranging;
	} curr;
} _list_it_t;

typedef struct _dict_t {
	_ref_t ref;
	mb_lock_t lock;
	_ht_node_t* dict;
} _dict_t;

#define _INVALID_DICT_IT ((_ls_node_t*)(intptr_t)~0)

typedef struct _dict_it_t {
	_ref_t weak_ref;
	_dict_t* dict;
	bool_t locking _PACK1;
	unsigned curr_bucket;
	_ls_node_t* curr_node;
} _dict_it_t;

typedef struct _keys_helper_t {
	mb_value_t* keys;
	int size;
	int index;
} _keys_helper_t;
#endif /* MB_ENABLE_COLLECTION_LIB */

typedef struct _label_t {
	char* name;
	_ls_node_t* node;
} _label_t;

#ifdef MB_ENABLE_CLASS
#define _META_LIST_MAX_DEPTH UINT_MAX

#define _CLASS_ME "ME"

#define _CLASS_HASH_FUNC "HASH"
#define _CLASS_COMPARE_FUNC "COMPARE"
#define _CLASS_TO_STRING_FUNC "TO_STRING"

#define _CLASS_OVERRIDE_FMT "_%s"

typedef struct _class_t {
	_ref_t ref;
	char* name;
	struct _class_t* created_from;
	_ls_node_t* meta_list;
	struct _running_context_t* scope;
	struct _routine_t* hash;
	struct _routine_t* compare;
	void* userdata;
} _class_t;
#endif /* MB_ENABLE_CLASS */

#ifdef MB_ENABLE_LAMBDA
typedef struct _running_context_ref_t {
	_ref_t ref;
	struct _running_context_ref_t* prev;
	struct _running_context_t* scope;
} _running_context_ref_t;

typedef struct _upvalue_scope_tuple_t {
	struct mb_interpreter_t* s;
#ifdef MB_ENABLE_CLASS
	_class_t* instance;
#endif /* MB_ENABLE_CLASS */
	struct _running_context_t* scope;
	_running_context_ref_t* outer_scope;
	struct _lambda_t* lambda;
	_ht_node_t* filled;
} _upvalue_scope_tuple_t;

typedef struct _lambda_t {
	_ref_t ref;
	struct _running_context_t* scope;
	_ls_node_t* parameters;
	_running_context_ref_t* outer_scope;
	struct _running_context_t* overlapped;
	_ht_node_t* upvalues;
	_ls_node_t* entry;
	_ls_node_t* end;
} _lambda_t;
#endif /* MB_ENABLE_LAMBDA */

typedef struct _routine_t {
	union {
		struct {
			struct _running_context_t* scope;
			_ls_node_t* parameters;
			_ls_node_t* entry;
		} basic;
#ifdef MB_ENABLE_LAMBDA
		_lambda_t lambda;
#endif /* MB_ENABLE_LAMBDA */
		struct {
			mb_routine_func_t entry;
		} native;
	} func;
	char* name;
#ifdef MB_ENABLE_SOURCE_TRACE
	char* source_file;
#endif /* MB_ENABLE_SOURCE_TRACE */
#ifdef MB_ENABLE_CLASS
	_class_t* instance;
#endif /* MB_ENABLE_CLASS */
	bool_t is_cloned _PACK1;
	mb_routine_type_e type;
} _routine_t;

#ifdef MB_ENABLE_SOURCE_TRACE
typedef struct _import_info_t {
	char* source_file;
} _import_info_t;
#endif /* MB_ENABLE_SOURCE_TRACE */

typedef union _raw_u { mb_data_e e; char c; int_t i; real_t r; void* p; mb_val_bytes_t b; } _raw_u;

typedef unsigned char _raw_t[sizeof(_raw_u)];

typedef struct _object_t {
	_data_e type;
	union {
		int_t integer;
		real_t float_point;
		char* string;
		mb_data_e type;
		void* usertype;
#ifdef MB_ENABLE_USERTYPE_REF
		_usertype_ref_t* usertype_ref;
#endif /* MB_ENABLE_USERTYPE_REF */
		_func_t* func;
		_var_t* variable;
		_array_t* array;
#ifdef MB_ENABLE_COLLECTION_LIB
		_list_t* list;
		_list_it_t* list_it;
		_dict_t* dict;
		_dict_it_t* dict_it;
#endif /* MB_ENABLE_COLLECTION_LIB */
		_label_t* label;
#ifdef MB_ENABLE_CLASS
		_class_t* instance;
#endif /* MB_ENABLE_CLASS */
		_routine_t* routine;
		char separator;
#ifdef MB_ENABLE_SOURCE_TRACE
		_import_info_t* import_info;
#endif /* MB_ENABLE_SOURCE_TRACE */
		mb_val_bytes_t bytes;
		void* pointer;
		_raw_t raw;
	} data;
	bool_t is_ref _PACK1;
#ifdef MB_PREFER_SPEED
	bool_t is_const _PACK1;
#endif /* MB_PREFER_SPEED */
#ifdef MB_ENABLE_SOURCE_TRACE
	int source_pos;
	unsigned short source_row;
	unsigned short source_col;
#else /* MB_ENABLE_SOURCE_TRACE */
	char source_pos _PACK1;
#endif /* MB_ENABLE_SOURCE_TRACE */
} _object_t;

#ifdef MB_ENABLE_MODULE
typedef struct _module_func_t {
	char* module;
	mb_func_t func;
} _module_func_t;
#endif /* MB_ENABLE_MODULE */

typedef struct _dynamic_buffer_t {
	char bytes[_TEMP_FORMAT_MAX_LENGTH];
	union {
		char* charp;
#if defined MB_CP_VC && defined MB_ENABLE_UNICODE
		wchar_t* wcharp;
#endif /* MB_CP_VC && MB_ENABLE_UNICODE */
	} pointer;
	size_t size;
} _dynamic_buffer_t;

#define _MB_MEM_TAG_SIZE (sizeof(mb_mem_tag_t))

MBAPI size_t MB_SIZEOF_4BYTES = 4;
MBAPI size_t MB_SIZEOF_8BYTES = 8;
MBAPI size_t MB_SIZEOF_32BYTES = 32;
MBAPI size_t MB_SIZEOF_64BYTES = 64;
MBAPI size_t MB_SIZEOF_128BYTES = 128;
MBAPI size_t MB_SIZEOF_256BYTES = 256;
MBAPI size_t MB_SIZEOF_512BYTES = 512;
#ifdef MB_ENABLE_ALLOC_STAT
MBAPI size_t MB_SIZEOF_INT = _MB_MEM_TAG_SIZE + sizeof(int);
MBAPI size_t MB_SIZEOF_PTR = _MB_MEM_TAG_SIZE + sizeof(intptr_t);
MBAPI size_t MB_SIZEOF_LSN = _MB_MEM_TAG_SIZE + sizeof(_ls_node_t);
MBAPI size_t MB_SIZEOF_HTN = _MB_MEM_TAG_SIZE + sizeof(_ht_node_t);
MBAPI size_t MB_SIZEOF_HTA = _MB_MEM_TAG_SIZE + sizeof(_ht_node_t*) * _HT_ARRAY_SIZE_DEFAULT;
MBAPI size_t MB_SIZEOF_OBJ = _MB_MEM_TAG_SIZE + sizeof(_object_t);
#ifdef MB_ENABLE_USERTYPE_REF
MBAPI size_t MB_SIZEOF_UTR = _MB_MEM_TAG_SIZE + sizeof(_usertype_ref_t);
#endif /* MB_ENABLE_USERTYPE_REF */
MBAPI size_t MB_SIZEOF_FUN = _MB_MEM_TAG_SIZE + sizeof(_func_t);
MBAPI size_t MB_SIZEOF_VAR = _MB_MEM_TAG_SIZE + sizeof(_var_t);
MBAPI size_t MB_SIZEOF_ARR = _MB_MEM_TAG_SIZE + sizeof(_array_t);
#ifdef MB_ENABLE_COLLECTION_LIB
MBAPI size_t MB_SIZEOF_LST = _MB_MEM_TAG_SIZE + sizeof(_list_t);
MBAPI size_t MB_SIZEOF_DCT = _MB_MEM_TAG_SIZE + sizeof(_dict_t);
#endif /* MB_ENABLE_COLLECTION_LIB */
MBAPI size_t MB_SIZEOF_LBL = _MB_MEM_TAG_SIZE + sizeof(_label_t);
#ifdef MB_ENABLE_CLASS
MBAPI size_t MB_SIZEOF_CLS = _MB_MEM_TAG_SIZE + sizeof(_class_t);
#endif /* MB_ENABLE_CLASS */
MBAPI size_t MB_SIZEOF_RTN = _MB_MEM_TAG_SIZE + sizeof(_routine_t);
#else /* MB_ENABLE_ALLOC_STAT */
MBAPI size_t MB_SIZEOF_INT = sizeof(int);
MBAPI size_t MB_SIZEOF_PTR = sizeof(intptr_t);
MBAPI size_t MB_SIZEOF_LSN = sizeof(_ls_node_t);
MBAPI size_t MB_SIZEOF_HTN = sizeof(_ht_node_t);
MBAPI size_t MB_SIZEOF_HTA = sizeof(_ht_node_t*) * _HT_ARRAY_SIZE_DEFAULT;
MBAPI size_t MB_SIZEOF_OBJ = sizeof(_object_t);
#ifdef MB_ENABLE_USERTYPE_REF
MBAPI size_t MB_SIZEOF_UTR = sizeof(_usertype_ref_t);
#endif /* MB_ENABLE_USERTYPE_REF */
MBAPI size_t MB_SIZEOF_FUN = sizeof(_func_t);
MBAPI size_t MB_SIZEOF_VAR = sizeof(_var_t);
MBAPI size_t MB_SIZEOF_ARR = sizeof(_array_t);
#ifdef MB_ENABLE_COLLECTION_LIB
MBAPI size_t MB_SIZEOF_LST = sizeof(_list_t);
MBAPI size_t MB_SIZEOF_DCT = sizeof(_dict_t);
#endif /* MB_ENABLE_COLLECTION_LIB */
MBAPI size_t MB_SIZEOF_LBL = sizeof(_label_t);
#ifdef MB_ENABLE_CLASS
MBAPI size_t MB_SIZEOF_CLS = sizeof(_class_t);
#endif /* MB_ENABLE_CLASS */
MBAPI size_t MB_SIZEOF_RTN = sizeof(_routine_t);
#endif /* MB_ENABLE_ALLOC_STAT */

#ifndef _CONST_PART1
# ifdef MB_PREFER_SPEED
#   define _CONST_PART1 false, false,
# else /* MB_PREFER_SPEED */
#   define _CONST_PART1 false,
# endif /* MB_PREFER_SPEED */
#endif /* _CONST_PART1 */
#ifndef _CONST_PART2
# ifdef MB_ENABLE_SOURCE_TRACE
#   define _CONST_PART2 0, 0, 0
# else /* MB_ENABLE_SOURCE_TRACE */
#   define _CONST_PART2 0
# endif /* MB_ENABLE_SOURCE_TRACE */
#endif /* _CONST_PART2 */
#ifndef _CONST_TAIL
# define _CONST_TAIL _CONST_PART1 _CONST_PART2
#endif /* _CONST_TAIL */

MBCONST static const _object_t _OBJ_INT_UNIT = { _DT_INT, (int_t)1, _CONST_TAIL };
MBCONST static const _object_t _OBJ_INT_ZERO = { _DT_INT, (int_t)0, _CONST_TAIL };
#define _MAKE_NIL(__o) do { memset((__o), 0, sizeof(_object_t)); (__o)->type = _DT_NIL; } while(0)

static _object_t* _OBJ_BOOL_TRUE = 0;
static _object_t* _OBJ_BOOL_FALSE = 0;

#ifdef MB_ENABLE_CLASS
MBCONST static const _object_t _OBJ_UNKNOWN = { _DT_UNKNOWN, (int_t)0, _CONST_TAIL };
MBCONST static const _ls_node_t _LS_NODE_UNKNOWN = { (void*)&_OBJ_UNKNOWN, 0, 0, 0 };
#endif /* MB_ENABLE_CLASS */

#define _VAR_ARGS_STR "..."

#define _IS_VAR_ARGS(__v) ((__v) == &_VAR_ARGS)

#ifdef MB_ENABLE_CLASS
MBCONST static const _var_t _VAR_ARGS = { _VAR_ARGS_STR, 0, 0, 0 };
#else /* MB_ENABLE_CLASS */
MBCONST static const _var_t _VAR_ARGS = { _VAR_ARGS_STR, 0 };
#endif /* MB_ENABLE_CLASS */

/* Parsing context */
#define _CLASS_STATE_NONE 0
#define _CLASS_STATE_PROC 1

MBCONST static const char _MULTI_LINE_COMMENT_PREFIX[] = "'[";
MBCONST static const char _MULTI_LINE_COMMENT_POSTFIX[] = "']";

typedef enum _parsing_state_e {
	_PS_NORMAL,
	_PS_STRING,
	_PS_COMMENT,
	_PS_MULTI_LINE_COMMENT
} _parsing_state_e;

typedef enum _symbol_state_e {
	_SS_IDENTIFIER,
	_SS_OPERATOR
} _symbol_state_e;

#define _ROUTINE_STATE_NONE 0
#define _ROUTINE_STATE_DEF 1
#define _ROUTINE_STATE_PARAMS 2

typedef struct _parsing_context_t {
	_ls_node_t* imported;
	char current_char;
	char current_symbol[_SINGLE_SYMBOL_MAX_LENGTH + 1];
	int current_symbol_nonius;
	int current_symbol_contains_accessor;
	_object_t* last_symbol;
	int multi_line_comment_count;
	_parsing_state_e parsing_state;
	_symbol_state_e symbol_state;
#ifdef MB_ENABLE_CLASS
	unsigned short class_state;
#endif /* MB_ENABLE_CLASS */
	unsigned short routine_state;
	unsigned short routine_params_state;
	int parsing_pos;
	unsigned short parsing_row;
	unsigned short parsing_col;
} _parsing_context_t;

/* Running context */
#define _SCOPE_META_ROOT (1 << 0)
#define _SCOPE_META_REF (1 << 1)

#define _INFINITY_CALC_DEPTH -1

typedef struct _running_context_t {
	struct _running_context_t* prev;
	unsigned meta;
	_ht_node_t* var_dict;
	struct _running_context_t* ref;
	_var_t* next_loop_var;
	mb_value_t intermediate_value;
	int calc_depth;
#ifdef MB_ENABLE_LAMBDA
	_ls_node_t* refered_lambdas;
#endif /* MB_ENABLE_LAMBDA */
} _running_context_t;

/* Expression processing utilities */
typedef struct _tuple3_t {
	void* e1;
	void* e2;
	void* e3;
} _tuple3_t;

/* Interpreter tag */
#define _JMP_NIL 0x00
#define _JMP_INS 0x01
#define _JMP_STR 0x02

#define _NO_EAT_COMMA 2

typedef struct mb_interpreter_t {
	/** Fundamental */
#ifdef MB_ENABLE_FORK
	struct mb_interpreter_t* forked_from;
	_running_context_t* forked_context;
	_ls_node_t* all_forked;
#endif /* MB_ENABLE_FORK */
	bool_t valid _PACK1;
	void* userdata;
	_ls_node_t* ast;

	//When we enter a function durung parsing, we save the AST
	//We have so far, and we restore it on exit.

	//This is because we save function ASTs in a separate place.
	_ls_node_t* saved_ast;



	/** Memory management */
	_gc_t gc;
	_ls_node_t* edge_destroy_objects;
	_ls_node_t* lazy_destroy_objects;
	/** Scripting interface and module */
	_ht_node_t* local_func_dict;
	_ht_node_t* global_func_dict;
#ifdef MB_ENABLE_MODULE
	_ht_node_t* module_func_dict;
	char* with_module;
	_ls_node_t* using_modules;
#endif /* MB_ENABLE_MODULE */
	/** Parsing and interpreting */
	char* source_file;
	_parsing_context_t* parsing_context;
	_running_context_t* running_context;
	int run_count;
	bool_t has_run _PACK1;
	_ls_node_t* var_args;
#ifdef MB_ENABLE_USERTYPE_REF
	_object_t* usertype_ref_ahead;
#endif /* MB_ENABLE_USERTYPE_REF */
	unsigned char jump_set;
#ifdef MB_ENABLE_CLASS
	_class_t* last_instance;
	bool_t calling _PACK1;
#endif /* MB_ENABLE_CLASS */
	_routine_t* last_routine;
	_ls_node_t* sub_stack;
	_ls_node_t* suspent_point;
	int schedule_suspend_tag;
	int_t no_eat_comma_mark;
	_ls_node_t* skip_to_eoi;
	_ls_node_t* in_neg_expr;
#ifdef MB_ENABLE_STACK_TRACE
	_ls_node_t* stack_frames;
#endif /* MB_ENABLE_STACK_TRACE */
#if _MULTILINE_STATEMENT
	_ls_node_t* multiline_enabled;
#endif /* _MULTILINE_STATEMENT */
	/** Error handling */
	bool_t handled_error _PACK1;
	mb_error_e last_error;
	char* last_error_file;
	int last_error_pos;
	unsigned short last_error_row;
	unsigned short last_error_col;
	/** Handlers */
	mb_alive_checker_t alive_check_handler;
	mb_debug_stepped_handler_t debug_stepped_handler;
	mb_error_handler_t error_handler;
	mb_print_func_t printer;
	mb_input_func_t inputer;
	mb_import_handler_t import_handler;
  
	void (*yieldfunc)(struct mb_interpreter_t *);
	int yieldcounter;
	struct mb_interpreter_t * parent;

} mb_interpreter_t;

/* Operations */
MBCONST static const char _PRECEDE_TABLE[20][20] = { /* Operator priority table */
	/* +    -    *    /   MOD   ^    (    )    =    >    <    >=   <=   ==   <>  AND   OR  NOT  NEG   IS */
	{ '>', '>', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* + */
	{ '>', '>', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* - */
	{ '>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* * */
	{ '>', '>', '>', '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* / */
	{ '>', '>', '<', '<', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* MOD */
	{ '>', '>', '>', '>', '>', '>', '<', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* ^ */
	{ '<', '<', '<', '<', '<', '<', '<', '=', ' ', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<' }, /* ( */
	{ '>', '>', '>', '>', '>', '>', ' ', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>' }, /* ) */
	{ '<', '<', '<', '<', '<', '<', '<', ' ', '=', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<' }, /* = */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* > */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* < */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* >= */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* <= */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* == */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>' }, /* <> */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '>' }, /* AND */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '>' }, /* OR */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '>', '>', '>' }, /* NOT */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<' }, /* NEG */
	{ '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '>', '>' }  /* IS */
};

static _object_t* _exp_assign = 0;

#define _copy_bytes(__l, __r) do { memcpy((__l), (__r), sizeof(mb_val_bytes_t)); } while(0)

#define _set_real_with_hex(__r, __i) \
	do { \
		if(sizeof(__r) == sizeof(unsigned char)) { \
			unsigned char __b = __i; \
			memcpy(&(__r), &__b, sizeof(__r)); \
		} else if(sizeof(__r) == sizeof(unsigned short)) { \
			unsigned short __b = __i; \
			memcpy(&(__r), &__b, sizeof(__r)); \
		} else if(sizeof(__r) == sizeof(unsigned)) { \
			unsigned __b = __i; \
			memcpy(&(__r), &__b, sizeof(__r)); \
		} else if(sizeof(__r) == sizeof(unsigned long)) { \
			unsigned long __b = __i; \
			memcpy(&(__r), &__b, sizeof(__r)); \
		} else if(sizeof(__r) == sizeof(unsigned long long)) { \
			unsigned long long __b = __i; \
			memcpy(&(__r), &__b, sizeof(__r)); \
		} else { \
			mb_assert(0 && "Invalid real number precision."); \
		} \
	} while(0)

#if MB_CONVERT_TO_INT_LEVEL == MB_CONVERT_TO_INT_LEVEL_NONE
# define _convert_to_int_if_posible(__o) do { ((void)(__o)); } while(0)
#else /* MB_CONVERT_TO_INT_LEVEL == MB_CONVERT_TO_INT_LEVEL_NONE */
# define _convert_to_int_if_posible(__o) \
		do { \
			if((__o)->type == _DT_REAL && (real_t)(int_t)(__o)->data.float_point == (__o)->data.float_point) { \
				(__o)->type = _DT_INT; \
				(__o)->data.integer = (int_t)(__o)->data.float_point; \
			} \
		} while(0)
#endif /* MB_CONVERT_TO_INT_LEVEL == MB_CONVERT_TO_INT_LEVEL_NONE */

#define _instruct_head(__tuple) \
	_object_t opndv1; \
	_object_t opndv2; \
	_tuple3_t* tpptr = (_tuple3_t*)(*__tuple); \
	_object_t* opnd1 = (_object_t*)(tpptr->e1); \
	_object_t* opnd2 = (_object_t*)(tpptr->e2); \
	_object_t* val = (_object_t*)(tpptr->e3);
#define _instruct_common(__tuple) \
	_instruct_head(__tuple) \
	opndv1.type = (opnd1->type == _DT_INT || (opnd1->type == _DT_VAR && opnd1->data.variable->data->type == _DT_INT)) ? \
		_DT_INT : _DT_REAL; \
	opndv1.data = opnd1->type == _DT_VAR ? opnd1->data.variable->data->data : opnd1->data; \
	opndv2.type = (opnd2->type == _DT_INT || (opnd2->type == _DT_VAR && opnd2->data.variable->data->type == _DT_INT)) ? \
		_DT_INT : _DT_REAL; \
	opndv2.data = opnd2->type == _DT_VAR ? opnd2->data.variable->data->data : opnd2->data;
#define _instruct_fun_num_num(__optr, __tuple) \
	do { \
		_instruct_common(__tuple) \
		if(opndv1.type == _DT_INT && opndv2.type == _DT_INT) { \
			val->type = _DT_REAL; \
			val->data.float_point = (real_t)__optr((real_t)opndv1.data.integer, (real_t)opndv2.data.integer); \
		} else { \
			val->type = _DT_REAL; \
			val->data.float_point = (real_t)__optr( \
				opndv1.type == _DT_INT ? opndv1.data.integer : opndv1.data.float_point, \
				opndv2.type == _DT_INT ? opndv2.data.integer : opndv2.data.float_point); \
		} \
		_convert_to_int_if_posible(val); \
	} while(0)
#define _instruct_bool_op_bool(__optr, __tuple) \
	do { \
		_instruct_common(__tuple) \
		if(opndv1.type == _DT_NIL) { opndv1.type = _DT_INT; opndv1.data.integer = 0; } \
		else if(opndv1.type != _DT_INT && opndv1.type != _DT_REAL) { opndv1.type = _DT_INT; opndv1.data.integer = 1; } \
		if(opndv2.type == _DT_NIL) { opndv2.type = _DT_INT; opndv2.data.integer = 0; } \
		else if(opndv2.type != _DT_INT && opndv2.type != _DT_REAL) { opndv2.type = _DT_INT; opndv2.data.integer = 1; } \
		if(opndv1.type == _DT_INT && opndv2.type == _DT_INT) { \
			if((real_t)(opndv1.data.integer __optr opndv2.data.integer) == (real_t)opndv1.data.integer __optr (real_t)opndv2.data.integer) { \
				val->type = _DT_INT; \
				val->data.integer = opndv1.data.integer __optr opndv2.data.integer; \
			} else { \
				val->type = _DT_REAL; \
				val->data.float_point = (real_t)((real_t)opndv1.data.integer __optr (real_t)opndv2.data.integer); \
			} \
		} else { \
			val->type = _DT_REAL; \
			val->data.float_point = (real_t) \
				((opndv1.type == _DT_INT ? opndv1.data.integer : opndv1.data.float_point) __optr \
				(opndv2.type == _DT_INT ? opndv2.data.integer : opndv2.data.float_point)); \
		} \
		_convert_to_int_if_posible(val); \
	} while(0)
#define _instruct_int_op_int(__optr, __tuple) \
	do { \
		_instruct_common(__tuple) \
		if(opndv1.type == _DT_INT && opndv2.type == _DT_INT) { \
			val->type = _DT_INT; \
			val->data.integer = opndv1.data.integer __optr opndv2.data.integer; \
		} else { \
			val->type = _DT_INT; \
			val->data.integer = \
				((opndv1.type == _DT_INT ? opndv1.data.integer : (int_t)(opndv1.data.float_point)) __optr \
				(opndv2.type == _DT_INT ? opndv2.data.integer : (int_t)(opndv2.data.float_point))); \
		} \
	} while(0)
#define _instruct_num_op_num(__optr, __tuple) \
	do { \
		_instruct_common(__tuple) \
		if(opndv1.type == _DT_INT && opndv2.type == _DT_INT) { \
			if((real_t)(opndv1.data.integer __optr opndv2.data.integer) == (real_t)opndv1.data.integer __optr (real_t)opndv2.data.integer) { \
				val->type = _DT_INT; \
				val->data.integer = opndv1.data.integer __optr opndv2.data.integer; \
			} else { \
				val->type = _DT_REAL; \
				val->data.float_point = (real_t)((real_t)opndv1.data.integer __optr (real_t)opndv2.data.integer); \
			} \
		} else { \
			val->type = _DT_REAL; \
			val->data.float_point = (real_t) \
				((opndv1.type == _DT_INT ? opndv1.data.integer : opndv1.data.float_point) __optr \
				(opndv2.type == _DT_INT ? opndv2.data.integer : opndv2.data.float_point)); \
		} \
		_convert_to_int_if_posible(val); \
	} while(0)
#define _instruct_obj_op_obj(__optr, __tuple) \
	do { \
		_instruct_head(__tuple); \
		opndv1.type = opnd1->type == _DT_VAR ? opnd1->data.variable->data->type : opnd1->type; \
		opndv1.data = opnd1->type == _DT_VAR ? opnd1->data.variable->data->data : opnd1->data; \
		opndv2.type = opnd2->type == _DT_VAR ? opnd2->data.variable->data->type : opnd2->type; \
		opndv2.data = opnd2->type == _DT_VAR ? opnd2->data.variable->data->data : opnd2->data; \
		val->type = _DT_INT; \
		if(opndv1.type == opndv2.type) { \
			val->data.integer = (int_t)(mb_memcmp(&opndv1.data, &opndv2.data, sizeof(_raw_t)) __optr 0); \
		} else { \
			val->data.integer = (int_t)(opndv1.type __optr opndv2.type); \
		} \
	} while(0)
#define _instruct_connect_strings(__tuple) \
	do { \
		char* _str1 = 0; \
		char* _str2 = 0; \
		_tuple3_t* tpptr = (_tuple3_t*)(*__tuple); \
		_object_t* opnd1 = (_object_t*)(tpptr->e1); \
		_object_t* opnd2 = (_object_t*)(tpptr->e2); \
		_object_t* val = (_object_t*)(tpptr->e3); \
		val->type = _DT_STRING; \
		if(val->data.string) { \
			safe_free(val->data.string); \
		} \
		_str1 = _extract_string(opnd1); \
		_str2 = _extract_string(opnd2); \
		val->data.string = (char*)mb_malloc(strlen(_str1) + strlen(_str2) + 1); \
		memset(val->data.string, 0, strlen(_str1) + strlen(_str2) + 1); \
		strcat(val->data.string, _str1); \
		strcat(val->data.string, _str2); \
	} while(0)
#define _instruct_compare_strings(__optr, __tuple) \
	do { \
		char* _str1 = 0; \
		char* _str2 = 0; \
		_tuple3_t* tpptr = (_tuple3_t*)(*__tuple); \
		_object_t* opnd1 = (_object_t*)(tpptr->e1); \
		_object_t* opnd2 = (_object_t*)(tpptr->e2); \
		_object_t* val = (_object_t*)(tpptr->e3); \
		val->type = _DT_INT; \
		_str1 = _extract_string(opnd1); \
		_str2 = _extract_string(opnd2); \
		val->data.integer = strcmp(_str1, _str2) __optr 0; \
	} while(0)
#ifdef MB_ENABLE_USERTYPE_REF
# if !defined _instruct_obj_meta_obj
#   define _instruct_obj_meta_obj(__s, __tuple, __optr, __result, __exit) \
			do { \
				_tuple3_t* tpptr = (_tuple3_t*)(*__tuple); \
				_object_t* opnd1 = (_object_t*)(tpptr->e1); \
				_object_t* opnd2 = (_object_t*)(tpptr->e2); \
				_object_t* retval = (_object_t*)(tpptr->e3); \
				if(opnd1->type == _DT_VAR) opnd1 = opnd1->data.variable->data; \
				if(opnd2->type == _DT_VAR) opnd2 = opnd2->data.variable->data; \
				{ \
					mb_value_t vfst, vscd; \
					mb_value_t ret; \
					mb_make_nil(vfst); \
					mb_make_nil(vscd); \
					mb_make_nil(ret); \
					if(opnd1->type == _DT_USERTYPE_REF && opnd1->data.usertype_ref->calc_operators && opnd1->data.usertype_ref->calc_operators->__optr) { \
						_internal_object_to_public_value(opnd1, &vfst); \
						_internal_object_to_public_value(opnd2, &vscd); \
						__result = opnd1->data.usertype_ref->calc_operators->__optr((__s), (__tuple), &vfst, &vscd, &ret); \
						_public_value_to_internal_object(&ret, retval); \
						goto __exit; \
					} else if(opnd2->type == _DT_USERTYPE_REF && opnd2->data.usertype_ref->calc_operators && opnd2->data.usertype_ref->calc_operators->__optr) { \
						_internal_object_to_public_value(opnd1, &vfst); \
						_internal_object_to_public_value(opnd2, &vscd); \
						__result = opnd2->data.usertype_ref->calc_operators->__optr((__s), (__tuple), &vfst, &vscd, &ret); \
						_public_value_to_internal_object(&ret, retval); \
						goto __exit; \
					} \
				} \
			} while(0)
# endif /* _instruct_obj_meta_obj */
#endif /* MB_ENABLE_USERTYPE_REF */
#ifndef _instruct_obj_meta_obj
# define _instruct_obj_meta_obj(__s, __tuple, __optr, __result, __exit) do { ((void)(__s)); ((void)(__tuple)); ((void)(__result)); } while(0)
#endif /* _instruct_obj_meta_obj */
#define _proc_div_by_zero(__s, __tuple, __exit, __result, __kind) \
	do { \
		_instruct_common(__tuple) \
		if((opndv2.type == _DT_INT && opndv2.data.integer == 0) || (opndv2.type == _DT_REAL && opndv2.data.float_point == 0.0f)) { \
			if((opndv1.type == _DT_INT && opndv1.data.integer == 0) || (opndv1.type == _DT_REAL && opndv1.data.float_point == 0.0f)) { \
				val->type = _DT_REAL; \
				_set_real_with_hex(val->data.float_point, MB_FNAN); \
			} else { \
				val->type = _DT_REAL; \
				_set_real_with_hex(val->data.float_point, MB_FINF); \
			} \
			_handle_error_on_obj((__s), __kind, (__s)->source_file, ((__tuple) && *(__tuple)) ? ((_object_t*)(((_tuple3_t*)(*(__tuple)))->e1)) : 0, MB_FUNC_WARNING, __exit, __result); \
		} \
	} while(0)

#define _set_tuple3_result(__l, __r) \
	do { \
		_object_t* val = (_object_t*)(((_tuple3_t*)(*(__l)))->e3); \
		val->type = _DT_INT; \
		val->data.integer = __r; \
	} while(0)

#define _math_calculate_fun_real(__s, __l, __a, __f, __exit, __result) \
	do { \
		switch((__a).type) { \
		case MB_DT_INT: \
			(__a).value.float_point = (real_t)__f((real_t)(__a).value.integer); \
			(__a).type = MB_DT_REAL; \
			break; \
		case MB_DT_REAL: \
			(__a).value.float_point = (real_t)__f((__a).value.float_point); \
			break; \
		default: \
			_handle_error_on_obj(__s, SE_RN_NUMBER_EXPECTED, (__s)->source_file, ((__l) && *(__l)) ? ((_object_t*)(((_tuple3_t*)(*(__l)))->e1)) : 0, MB_FUNC_WARNING, __exit, __result); \
			break; \
		} \
		mb_convert_to_int_if_posible(__a); \
	} while(0)

#define _using_jump_set_of_instructional(__s, __obj, __exit, __result) \
	do { \
		if((__s)->jump_set & (~_JMP_INS)) { \
			_handle_error_on_obj(__s, SE_RN_CANNOT_MIX_INSTRUCTIONAL_AND_STRUCTURED, (__s)->source_file, DON(__obj), MB_FUNC_ERR, __exit, __result); \
		} else { \
			(__s)->jump_set |= _JMP_INS; \
		} \
	} while(0)
#define _using_jump_set_of_structured(__s, __obj, __exit, __result) \
	do { \
		if((__s)->jump_set & (~_JMP_STR)) { \
			_handle_error_on_obj(__s, SE_RN_CANNOT_MIX_INSTRUCTIONAL_AND_STRUCTURED, (__s)->source_file, DON(__obj), MB_FUNC_ERR, __exit, __result); \
		} else { \
			(__s)->jump_set |= _JMP_STR; \
		} \
	} while(0)

/* ========================================================} */

/*
** {========================================================
** Private function declarations
*/

/** List operations */

static int _ls_cmp_data(void* node, void* info);
static int _ls_cmp_extra(void* node, void* info);
static int _ls_cmp_extra_object(void* node, void* info);
static int _ls_cmp_extra_string(void* node, void* info);
#ifdef MB_ENABLE_MODULE
static int _ls_cmp_module_func(void* node, void* info);
#endif /* MB_ENABLE_MODULE */

static _ls_node_t* _ls_create_node(void* data);
static _ls_node_t* _ls_create(void);
static _ls_node_t* _ls_find(_ls_node_t* list, void* data, _ls_compare_t cmp, int* idx);
static _ls_node_t* _ls_back(_ls_node_t* node);
static _ls_node_t* _ls_pushback(_ls_node_t* list, void* data);
static void* _ls_popback(_ls_node_t* list);
static _ls_node_t* _ls_front(_ls_node_t* node);
static void* _ls_popfront(_ls_node_t* list);
static _ls_node_t* _ls_insert_at(_ls_node_t* list, int index, void* data);
static unsigned _ls_remove(_ls_node_t* list, _ls_node_t* node, _ls_operation_t op);
static unsigned _ls_try_remove(_ls_node_t* list, void* info, _ls_compare_t cmp, _ls_operation_t op);
static unsigned _ls_foreach(_ls_node_t* list, _ls_operation_t op);
static _ls_node_t* _ls_sort(_ls_node_t* _UNALIGNED_ARG * list, _ls_compare_t cmp);
static unsigned _ls_count(_ls_node_t* list);
static bool_t _ls_empty(_ls_node_t* list);
static void _ls_clear(_ls_node_t* list);
static void _ls_destroy(_ls_node_t* list);
static int _ls_free_extra(void* data, void* extra);
#define _LS_FOREACH(L, O, P, E) \
	do { \
		_ls_node_t* __lst = L; \
		int __opresult = _OP_RESULT_NORMAL; \
		_ls_node_t* __tmp = 0; \
		mb_assert(L); \
		__lst = __lst->next; \
		while(__lst) { \
			if(P != 0) { \
				P(__lst->data, __lst->extra, E); \
			} \
			if(O != 0) { \
				__opresult = O(__lst->data, __lst->extra); \
			} \
			__tmp = __lst; \
			__lst = __lst->next; \
			if(_OP_RESULT_DEL_NODE == __opresult) { \
				__tmp->prev->next = __lst; \
				if(__lst) { \
					__lst->prev = __tmp->prev; \
				} \
				safe_free(__tmp); \
				(L)->data = (char*)(L)->data - 1; \
			} \
		} \
	} while(0)

/** Dictionary operations */

static unsigned _ht_hash_object(void* ht, void* d);
static unsigned _ht_hash_string(void* ht, void* d);
static unsigned _ht_hash_intptr(void* ht, void* d);
static unsigned _ht_hash_ref(void* ht, void* d);

static int _ht_cmp_object(void* d1, void* d2);
static int _ht_cmp_string(void* d1, void* d2);
static int _ht_cmp_intptr(void* d1, void* d2);
static int _ht_cmp_ref(void* d1, void* d2);

static _ht_node_t* _ht_create(unsigned size, _ht_compare_t cmp, _ht_hash_t hs, _ls_operation_t freeextra);
static _ls_node_t* _ht_find(_ht_node_t* ht, void* key);
static unsigned _ht_set_or_insert(_ht_node_t* ht, void* key, void* value);
static unsigned _ht_remove(_ht_node_t* ht, void* key, _ls_compare_t cmp);
static unsigned _ht_foreach(_ht_node_t* ht, _ht_operation_t op);
static unsigned _ht_count(_ht_node_t* ht);
static void _ht_clear(_ht_node_t* ht);
static void _ht_destroy(_ht_node_t* ht);
static int _ht_remove_existing(void* data, void* extra, _ht_node_t* ht);
#define _HT_FOREACH(H, O, P, E) \
	do { \
		_ls_node_t* __bucket = 0; \
		unsigned __ul = 0; \
		if((H)->array) { \
			for(__ul = 0; __ul < (H)->array_size; ++__ul) { \
				__bucket = (H)->array[__ul]; \
				if(__bucket) { \
					_LS_FOREACH(__bucket, O, P, E); \
				} \
			} \
		} \
	} while(0)

/** Memory manipulations */

static void _init_dynamic_buffer(_dynamic_buffer_t* buf);
static void _dispose_dynamic_buffer(_dynamic_buffer_t* buf);
static size_t _countof_dynamic_buffer(_dynamic_buffer_t* buf, size_t es);
static void _resize_dynamic_buffer(_dynamic_buffer_t* buf, size_t es, size_t c);

#define _INIT_BUF(b) do { _init_dynamic_buffer(&(b)); } while(0)
#define _DISPOSE_BUF(b) do { _dispose_dynamic_buffer(&(b)); } while(0)
#define _CHARS_OF_BUF(b) (_countof_dynamic_buffer((&b), sizeof(char)))
#define _RESIZE_CHAR_BUF(b, c) do { _resize_dynamic_buffer(&(b), sizeof(char), (c)); } while(0)
#define _HEAP_CHAR_BUF(b) (((b).pointer.charp != (b).bytes) ? ((b).pointer.charp) : (mb_memdup((b).pointer.charp, (unsigned)(b).size)))
#define _CHAR_BUF_PTR(b) ((b).pointer.charp)
#if defined MB_CP_VC && defined MB_ENABLE_UNICODE
#define _WCHARS_OF_BUF(b) (_countof_dynamic_buffer((&b), sizeof(wchar_t)))
#define _RESIZE_WCHAR_BUF(b, c) do { _resize_dynamic_buffer(&(b), sizeof(wchar_t), (c)); } while(0)
#define _WCHAR_BUF_PTR(b) ((b).pointer.wcharp)
#endif /* MB_CP_VC && MB_ENABLE_UNICODE */

#define _MB_CHECK_MEM_TAG_SIZE(y, s) ((y)(mb_mem_tag_t)(s) == (s))
#define _MB_WRITE_MEM_TAG_SIZE(t, s) (*((mb_mem_tag_t*)((char*)(t) - _MB_MEM_TAG_SIZE)) = (mb_mem_tag_t)(s))
#define _MB_READ_MEM_TAG_SIZE(t) (*((mb_mem_tag_t*)((char*)(t) - _MB_MEM_TAG_SIZE)))

#ifdef MB_ENABLE_ALLOC_STAT
static volatile size_t _mb_allocated = 0;
#else /* MB_ENABLE_ALLOC_STAT */
static const size_t _mb_allocated = (size_t)(~0);
#endif /* MB_ENABLE_ALLOC_STAT */

static mb_memory_allocate_func_t _mb_allocate_func = 0;
static mb_memory_free_func_t _mb_free_func = 0;

static void* mb_malloc(size_t s);
static void mb_free(void* p);

static int mb_memcmp(void* l, void* r, size_t s);
static size_t mb_memtest(void* p, size_t s);

static char* mb_strdup(const char* p, size_t s);
static char* mb_strupr(char* s);

#define safe_free(__p) do { if(__p) { mb_free(__p); __p = 0; } else { mb_assert(0 && "Memory already released."); } } while(0)

static bool_t mb_is_little_endian(void);

/** Unicode handling */

#if defined MB_CP_VC && defined MB_ENABLE_UNICODE
static int mb_bytes_to_wchar(const char* sz, wchar_t** out, size_t size);
static int mb_bytes_to_wchar_ansi(const char* sz, wchar_t** out, size_t size);
static int mb_wchar_to_bytes(const wchar_t* sz, char** out, size_t size);
#endif /* MB_CP_VC && MB_ENABLE_UNICODE */

static int mb_uu_getbom(const char** ch);
#ifdef MB_ENABLE_UNICODE
static int mb_uu_ischar(const char* ch);
static int mb_uu_strlen(const char* ch);
static int mb_uu_substr(const char* ch, int begin, int count, char** o);
#endif /* MB_ENABLE_UNICODE */

/** Expression processing */

#ifndef _ONCALC /* Uprootable stub */
# define _ONCALC(__s, __tuple, __optr, __result, __exit) do { ((void)(__s)); ((void)(__tuple)); ((void)(__result)); } while(0)
#endif /* _ONCALC */
#ifndef _ONNEG /* Uprootable stub */
# define _ONNEG(__s, __l, __arg, __result, __exit) do { ((void)(__s)); ((void)(__l)); ((void)(__arg)); ((void)(__result)); } while(0)
#endif /* _ONNEG */

#ifndef _ONCOND /* Uprootable stub */
# define _ONCOND(__s, __o, __v) do { ((void)(__s)); ((void)(__o)); ((void)(__v)); } while(0)
#endif /* _ONCOND */

static bool_t _is_operator(mb_func_t op);
static bool_t _is_flow(mb_func_t op);
static bool_t _is_unary(mb_func_t op);
static bool_t _is_binary(mb_func_t op);
static char _get_priority(mb_func_t op1, mb_func_t op2);
static int _get_priority_index(mb_func_t op);
static _object_t* _operate_operand(mb_interpreter_t* s, _object_t* optr, _object_t* opnd1, _object_t* opnd2, int* status);
static bool_t _is_expression_terminal(mb_interpreter_t* s, _object_t* obj);
static bool_t _is_unexpected_calc_type(mb_interpreter_t* s, _object_t* obj);
static bool_t _is_referenced_calc_type(mb_interpreter_t* s, _object_t* obj);
static int _calc_expression(mb_interpreter_t* s, _ls_node_t** l, _object_t** val);

#ifndef _PREVCALL /* Uprootable stub */
# define _PREVCALL(__s, __l, __r) do { ((void)(__s)); ((void)(__l)); ((void)(__r)); } while(0)
#endif /* _PREVCALL */
#ifndef _POSTCALL /* Uprootable stub */
# define _POSTCALL(__s, __l, __r) do { ((void)(__s)); ((void)(__l)); ((void)(__r)); } while(0)
#endif /* _POSTCALL */

static _ls_node_t* _push_var_args(mb_interpreter_t* s);
static void _pop_var_args(mb_interpreter_t* s, _ls_node_t* last_var_args);
static int _pop_arg(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, unsigned* ia, _routine_t* r, mb_pop_routine_arg_func_t pop_arg, _ls_node_t* args, mb_value_t* arg);
static int _proc_args(mb_interpreter_t* s, _ls_node_t** l, _running_context_t* running, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg, bool_t proc_ref, _ls_node_t* args);
static int _eval_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg);
static int _eval_script_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg);
#ifdef MB_ENABLE_LAMBDA
static int _eval_lambda_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg);
#endif /* MB_ENABLE_LAMBDA */
static int _eval_native_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg);
static int _has_routine_lex_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r);
static int _pop_routine_lex_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r, mb_value_t* val);
static int _has_routine_fun_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r);
static int _pop_routine_fun_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r, mb_value_t* val);
static bool_t _is_print_terminal(mb_interpreter_t* s, _object_t* obj);
static mb_meta_status_e _try_overridden(mb_interpreter_t* s, void** l, mb_value_t* d, const char* f, mb_meta_func_e t);

/** Handlers */

#define _handle_error_now(__s, __err, __f, __result) \
	do { \
		_set_current_error((__s), (__err), (__f)); \
		if((__s)->error_handler) { \
			if((__s)->handled_error) \
				break; \
			(__s)->handled_error = true; \
			((__s)->error_handler)((__s), (__s)->last_error, (char*)mb_get_error_desc((__s)->last_error), \
				(__s)->last_error_file, \
				(__s)->parsing_context && !(__s)->run_count ? (__s)->parsing_context->parsing_pos : (__s)->last_error_pos, \
				(__s)->parsing_context && !(__s)->run_count ? (__s)->parsing_context->parsing_row : (__s)->last_error_row, \
				(__s)->parsing_context && !(__s)->run_count ? (__s)->parsing_context->parsing_col : (__s)->last_error_col, \
				(__result)); \
		} \
	} while(0)
#if _WARNING_AS_ERROR
# define _handle_error_at_pos(__s, __err, __f, __pos, __row, __col, __ret, __exit, __result) \
		do { \
			if(_set_current_error((__s), (__err), (__f))) { \
				_set_error_pos((__s), (__pos), (__row), (__col)); \
			} \
			__result = (__ret); \
			goto __exit; \
		} while(0)
#else /* _WARNING_AS_ERROR */
# define _handle_error_at_pos(__s, __err, __f, __pos, __row, __col, __ret, __exit, __result) \
		do { \
			if(_set_current_error((__s), (__err), (__f))) { \
				_set_error_pos((__s), (__pos), (__row), (__col)); \
				if((__ret) != MB_FUNC_WARNING) { \
					__result = (__ret); \
				} \
			} \
			goto __exit; \
		} while(0)
#endif /* _WARNING_AS_ERROR */
#ifdef MB_ENABLE_SOURCE_TRACE
# define _handle_error_on_obj(__s, __err, __f, __obj, __ret, __exit, __result) \
		do { \
			if(__obj) { \
				_handle_error_at_pos((__s), (__err), (__f), (__obj)->source_pos, (__obj)->source_row, (__obj)->source_col, (__ret), __exit, (__result)); \
			} else { \
				_handle_error_at_pos((__s), (__err), (__f), 0, 0, 0, (__ret), __exit, (__result)); \
			} \
		} while(0)
#else /* MB_ENABLE_SOURCE_TRACE */
# define _handle_error_on_obj(__s, __err, __f, __obj, __ret, __exit, __result) \
		do { ((void)(__obj)); _handle_error_at_pos((__s), (__err), (__f), 0, 0, 0, (__ret), __exit, (__result)); } while(0)
#endif /* MB_ENABLE_SOURCE_TRACE */

#define _OUTTER_SCOPE(__s) ((__s)->prev ? (__s)->prev : (__s))

static bool_t _set_current_error(mb_interpreter_t* s, mb_error_e err, char* f);

static mb_print_func_t _get_printer(mb_interpreter_t* s);
static mb_input_func_t _get_inputer(mb_interpreter_t* s);

static void _print_string(mb_interpreter_t* s, _object_t* obj);

/** Parsing helpers */

static char* _load_file(mb_interpreter_t* s, const char* f, const char* prefix, bool_t importing);
static void _end_of_file(_parsing_context_t* context);

#define _ZERO_CHAR '\0'
#define _NEWLINE_CHAR '\n'
#define _RETURN_CHAR '\r'
#define _STRING_POSTFIX_CHAR '$'
#define _DUMMY_ASSIGN_CHAR "#"
#define _INVALID_CLASS_CHAR 0x18
#define _INVALID_ROUTINE_CHAR 0x1B

#define _REMARK_STR "REM"

static bool_t _is_blank_char(char c);
static bool_t _is_eof_char(char c);
static bool_t _is_newline_char(char c);
static bool_t _is_separator_char(char c);
static bool_t _is_bracket_char(char c);
static bool_t _is_quotation_char(char c);
static bool_t _is_comment_char(char c);
static bool_t _is_accessor_char(char c);
static bool_t _is_numeric_char(char c);
static bool_t _is_identifier_char(char c);
static bool_t _is_operator_char(char c);
static bool_t _is_exponential_char(char c);
static bool_t _is_using_at_char(char c);
static bool_t _is_exponent_prefix(char* s, int begin, int end);

static int _append_char_to_symbol(mb_interpreter_t* s, char c);
#ifdef MB_ENABLE_UNICODE_ID
static int _append_uu_char_to_symbol(mb_interpreter_t* s, const char* str, int n);
#endif /* MB_ENABLE_UNICODE_ID */
static int _cut_symbol(mb_interpreter_t* s, int pos, unsigned short row, unsigned short col);
static int _append_symbol(mb_interpreter_t* s, char* sym, bool_t* delsym, int pos, unsigned short row, unsigned short col);
static int _create_symbol(mb_interpreter_t* s, _ls_node_t* l, char* sym, _object_t** obj, _ls_node_t*** asgn, bool_t* delsym);
static _data_e _get_symbol_type(mb_interpreter_t* s, char* sym, _raw_t* value);
static int _parse_char(mb_interpreter_t* s, const char* str, int n, int pos, unsigned short row, unsigned short col);
static void _set_error_pos(mb_interpreter_t* s, int pos, unsigned short row, unsigned short col);
static char* _prev_import(mb_interpreter_t* s, char* lf, int* pos, unsigned short* row, unsigned short* col);
static char* _post_import(mb_interpreter_t* s, char* lf, int* pos, unsigned short* row, unsigned short* col);

/** Object processors */

#define DON(__o) ((__o) ? ((_object_t*)((__o)->data)) : 0)
#define DON2(__a) (((__a) && *(__a)) ? (_object_t*)((*((_ls_node_t**)(__a)))->data) : 0)
#define TON(__t) (((__t) && *(__t)) ? (_object_t*)(((_tuple3_t*)(*(__t)))->e1) : 0)

#define _IS_EOS(__o) (__o && ((_object_t*)(__o))->type == _DT_EOS)
#define _IS_SEP(__o, __c) (((_object_t*)(__o))->type == _DT_SEP && ((_object_t*)(__o))->data.separator == __c)
#define _IS_FUNC(__o, __f) (((_object_t*)(__o))->type == _DT_FUNC && ((_object_t*)(__o))->data.func->pointer == __f)
#define _IS_UNARY_FUNC(__o) (((_object_t*)(__o))->type == _DT_FUNC && _is_unary(((_object_t*)(__o))->data.func->pointer))
#define _IS_VAR(__o) ((__o) && ((_object_t*)(__o))->type == _DT_VAR)
#ifdef MB_ENABLE_COLLECTION_LIB
# define _IS_LIST(__o) ((__o) && ((_object_t*)(__o))->type == _DT_LIST)
# define _IS_DICT(__o) ((__o) && ((_object_t*)(__o))->type == _DT_DICT)
# define _IS_COLL(__o) (_IS_LIST(__o) || _IS_DICT(__o))
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
# define _IS_CLASS(__o) ((__o) && ((_object_t*)(__o))->type == _DT_CLASS)
# define _GET_CLASS(__o) ((!__o) ? 0 : (_IS_CLASS(__o) ? (__o) : (_IS_VAR(__o) && _IS_CLASS((__o)->data.variable->data) ? (__o)->data.variable->data : 0)))
# define _IS_ME(__v) (!!(__v)->is_me)
#else /* MB_ENABLE_CLASS */
# define _IS_ME(__v) false
#endif /* MB_ENABLE_CLASS */
#define _IS_ROUTINE(__o) ((__o) && ((_object_t*)(__o))->type == _DT_ROUTINE)
#define _GET_ROUTINE(__o) ((!__o) ? 0 : (_IS_ROUTINE(__o) ? (__o) : (_IS_VAR(__o) && _IS_ROUTINE((__o)->data.variable->data) ? (__o)->data.variable->data : 0)))

#ifdef MB_ENABLE_USERTYPE_REF
# define _REF_USERTYPE_REF(__o) \
		case _DT_USERTYPE_REF: \
			_ref(&(__o)->data.usertype_ref->ref, (__o)->data.usertype_ref); \
			break;
# define _UNREF_USERTYPE_REF(__o) \
		case _DT_USERTYPE_REF: \
			_unref(&(__o)->data.usertype_ref->ref, (__o)->data.usertype_ref); \
			break;
# define _ADDGC_USERTYPE_REF(__o, __g) \
		case _DT_USERTYPE_REF: \
			_gc_add(&(__o)->data.usertype_ref->ref, (__o)->data.usertype_ref, (__g)); \
			break;
#else /* MB_ENABLE_USERTYPE_REF */
# define _REF_USERTYPE_REF(__o) { (void)(__o); }
# define _UNREF_USERTYPE_REF(__o) { (void)(__o); }
# define _ADDGC_USERTYPE_REF(__o, __g) { (void)(__o); (void)(__g); }
#endif /* MB_ENABLE_USERTYPE_REF */
#ifdef MB_ENABLE_ARRAY_REF
# define _REF_ARRAY(__o) \
		case _DT_ARRAY: \
			if(!(__o)->is_ref) \
				_ref(&(__o)->data.array->ref, (__o)->data.array); \
			break;
# define _UNREF_ARRAY(__o) \
		case _DT_ARRAY: \
			if(!(__o)->is_ref) \
				_unref(&(__o)->data.array->ref, (__o)->data.array); \
			break;
# define _ADDGC_ARRAY(__o, __g) \
		case _DT_ARRAY: \
			if(!(__o)->is_ref) \
				_gc_add(&(__o)->data.array->ref, (__o)->data.array, (__g)); \
			break;
#else /* MB_ENABLE_ARRAY_REF */
# define _REF_ARRAY(__o) case _DT_ARRAY: { (void)(__o); } break;
# define _UNREF_ARRAY(__o) case _DT_ARRAY: { (void)(__o); } break;
# define _ADDGC_ARRAY(__o, __g) case _DT_ARRAY: { (void)(__o); (void)(__g); } break;
# define _DESTROY_ARRAY(__o) \
		case _DT_ARRAY: \
			if(!(__o)->is_ref) \
				_destroy_array((__o)->data.array); \
			break;
#endif /* MB_ENABLE_ARRAY_REF */
#ifdef MB_ENABLE_COLLECTION_LIB
# define _REF_COLL(__o) \
		case _DT_LIST: \
			_ref(&(__o)->data.list->ref, (__o)->data.list); \
			break; \
		case _DT_DICT: \
			_ref(&(__o)->data.dict->ref, (__o)->data.dict); \
			break;
# define _UNREF_COLL(__o) \
		case _DT_LIST: \
			_unref(&(__o)->data.list->ref, (__o)->data.list); \
			break; \
		case _DT_DICT: \
			_unref(&(__o)->data.dict->ref, (__o)->data.dict); \
			break;
# define _ADDGC_COLL(__o, __g) \
		case _DT_LIST: \
			_gc_add(&(__o)->data.list->ref, (__o)->data.list, (__g)); \
			break; \
		case _DT_DICT: \
			_gc_add(&(__o)->data.dict->ref, (__o)->data.dict, (__g)); \
			break;
# define _UNREF_COLL_IT(__o) \
		case _DT_LIST_IT: \
			_destroy_list_it((__o)->data.list_it); \
			break; \
		case _DT_DICT_IT: \
			_destroy_dict_it((__o)->data.dict_it); \
			break;
# define _ADDGC_COLL_IT(__o, __g) \
		case _DT_LIST_IT: \
			_destroy_list_it((__o)->data.list_it); \
			break; \
		case _DT_DICT_IT: \
			_destroy_dict_it((__o)->data.dict_it); \
			break;
#else /* MB_ENABLE_COLLECTION_LIB */
# define _REF_COLL(__o) { (void)(__o); }
# define _UNREF_COLL(__o) { (void)(__o); }
# define _ADDGC_COLL(__o, __g) { (void)(__o); (void)(__g); }
# define _UNREF_COLL_IT(__o) { (void)(__o); }
# define _ADDGC_COLL_IT(__o, __g) { (void)(__o); (void)(__g); }
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
# define _REF_CLASS(__o) \
		case _DT_CLASS: \
			_ref(&(__o)->data.instance->ref, (__o)->data.instance); \
			break;
# define _UNREF_CLASS(__o) \
		case _DT_CLASS: \
			if(!(__o)->is_ref) \
				_unref(&(__o)->data.instance->ref, (__o)->data.instance); \
			break;
# define _ADDGC_CLASS(__o, __g) \
		case _DT_CLASS: \
			if(!(__o)->is_ref) \
				_gc_add(&(__o)->data.instance->ref, (__o)->data.instance, (__g)); \
			break;
#else /* MB_ENABLE_CLASS */
# define _REF_CLASS(__o) { (void)(__o); }
# define _UNREF_CLASS(__o) { (void)(__o); }
# define _ADDGC_CLASS(__o, __g) { (void)(__o); (void)(__g); }
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_LAMBDA
# define _REF_ROUTINE(__o) \
		case _DT_ROUTINE: \
			if(!(__o)->is_ref && (__o)->data.routine->type == MB_RT_LAMBDA) \
				_ref(&(__o)->data.routine->func.lambda.ref, (__o)->data.routine); \
			break;
# define _UNREF_ROUTINE(__o) \
		case _DT_ROUTINE: \
			if(!(__o)->is_ref && (__o)->data.routine->type == MB_RT_LAMBDA) \
				_unref(&(__o)->data.routine->func.lambda.ref, (__o)->data.routine); \
			else if(!(__o)->is_ref && (__o)->data.routine->type != MB_RT_LAMBDA) \
				_destroy_routine(0, (__o)->data.routine); \
			break;
# define _ADDGC_ROUTINE(__o, __g, __r) \
		case _DT_ROUTINE: \
			if(!(__o)->is_ref && (__o)->data.routine->type == MB_RT_LAMBDA) \
				_gc_add(&(__o)->data.routine->func.lambda.ref, (__o)->data.routine, (__g)); \
			else if((__r) && !(__o)->is_ref && (__o)->data.routine->type != MB_RT_LAMBDA) \
				_dispose_object(__o); \
			break;
# define _COLL_ROUTINE(__o) \
		do { \
			if((__o)->type == _DT_ROUTINE && (__o)->data.routine->type != MB_RT_LAMBDA) \
				(__o)->is_ref = true; \
		} while(0)
#else /* MB_ENABLE_LAMBDA */
# define _REF_ROUTINE(__o) case _DT_ROUTINE: { (void)(__o); } break;
# define _UNREF_ROUTINE(__o) case _DT_ROUTINE: { (void)(__o); } break;
# define _ADDGC_ROUTINE(__o, __g, __r) \
		case _DT_ROUTINE: \
			((void)(__g)); \
			((void)(__r)); \
			_dispose_object(__o); \
			break;
# define _COLL_ROUTINE(__o) \
		do { \
			if((__o)->type == _DT_ROUTINE) \
				(__o)->is_ref = true; \
		} while(0)
#endif /* MB_ENABLE_LAMBDA */
#define _ADDGC_STRING(__o) \
	case _DT_STRING: \
		_dispose_object(__o); \
		break;
#define _REF(__o) \
	switch((__o)->type) { \
	_REF_USERTYPE_REF(__o) \
	_REF_ARRAY(__o) \
	_REF_COLL(__o) \
	_REF_CLASS(__o) \
	_REF_ROUTINE(__o) \
	default: break; \
	}
#define _UNREF(__o) \
	switch((__o)->type) { \
	_UNREF_USERTYPE_REF(__o) \
	_UNREF_ARRAY(__o) \
	_UNREF_COLL(__o) \
	_UNREF_CLASS(__o) \
	_UNREF_ROUTINE(__o) \
	default: break; \
	}
#define _ADDGC(__o, __g, __r) \
	if(!(__o)->data.pointer || !_ht_find((__g)->collected_table, (__o)->data.pointer)) { \
		switch((__o)->type) { \
		_ADDGC_USERTYPE_REF(__o, __g) \
		_ADDGC_ARRAY(__o, __g) \
		_ADDGC_COLL(__o, __g) \
		_ADDGC_COLL_IT(__o, __g) \
		_ADDGC_CLASS(__o, __g) \
		_ADDGC_ROUTINE(__o, __g, __r) \
		_ADDGC_STRING(__o) \
		default: break; \
		} \
	}
#ifndef _GCNOW /* Uprootable stub */
# define _GCNOW(__s) (!!(__s))
#endif /* _GCNOW */
#ifndef _PREPAREGC /* Uprootable stub */
# define _PREPAREGC(__s, __g) do { ((void)(__s)); ((void)(__g)); } while(0)
#endif /* _PREPAREGC */
#ifndef _PREVGC /* Uprootable stub */
# define _PREVGC(__s, __g) do { ((void)(__s)); ((void)(__g)); } while(0)
#endif /* _PREVGC */
#ifndef _POSTGC /* Uprootable stub */
# define _POSTGC(__s, __g) do { ((void)(__s)); ((void)(__g)); } while(0)
#endif /* _POSTGC */

static int_t _get_size_of(_data_e type);
static bool_t _try_get_value(_object_t* obj, mb_value_u* val, _data_e expected);

static bool_t _is_nil(void* obj);
static bool_t _is_number(void* obj);
static bool_t _is_string(void* obj);
static char* _extract_string(_object_t* obj);
#ifdef MB_MANUAL_REAL_FORMATTING
static void _real_to_str(real_t r, char* str, size_t size, size_t afterpoint);
#endif /* MB_MANUAL_REAL_FORMATTING */

#ifdef _HAS_REF_OBJ_LOCK
static bool_t _lock_ref_object(mb_lock_t* lk, _ref_t* ref, void* obj);
static bool_t _unlock_ref_object(mb_lock_t* lk, _ref_t* ref, void* obj);
static bool_t _write_on_ref_object(mb_lock_t* lk, _ref_t* ref, void* obj);
#endif /* _HAS_REF_OBJ_LOCK */

static bool_t _is_ref(_object_t* obj);
static _ref_count_t _ref(_ref_t* ref, void* data);
static bool_t _unref(_ref_t* ref, void* data);
static _ref_count_t _weak_ref(_ref_t* ref, void* data, _ref_t* weak);
static bool_t _weak_unref(_ref_t* weak);
static void _create_ref(_ref_t* ref, _unref_func_t dtor, _data_e t, mb_interpreter_t* s);
static void _destroy_ref(_ref_t* ref);

static void _gc_add(_ref_t* ref, void* data, _gc_t* gc);
static unsigned _gc_remove(_ref_t* ref, void* data, _gc_t* gc);
static int _gc_add_reachable(void* data, void* extra, void* h);
static int _gc_add_reachable_both(void* data, void* extra, void* h);
#ifdef MB_ENABLE_FORK
static int _gc_get_reachable_in_forked(void* data, void* extra, _ht_node_t* valid);
#endif /* MB_ENABLE_FORK */
static void _gc_get_reachable(mb_interpreter_t* s, _ht_node_t* ht, _running_context_t* end);
static void _gc_alive_marker(mb_interpreter_t* s, void* h, mb_value_t val);
static int _gc_destroy_garbage_in_list(void* data, void* extra, _gc_t* gc);
static int _gc_destroy_garbage_in_dict(void* data, void* extra, _gc_t* gc);
#ifdef MB_ENABLE_CLASS
static int _gc_destroy_garbage_in_class(void* data, void* extra, _gc_t* gc);
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_LAMBDA
static int _gc_destroy_garbage_in_lambda(void* data, void* extra, _gc_t* gc);
static void _gc_destroy_garbage_in_outer_scope(_running_context_ref_t* p, _gc_t* gc);
#endif /* MB_ENABLE_LAMBDA */
static int _gc_destroy_garbage(void* data, void* extra, _gc_t* gc);
#ifdef MB_ENABLE_CLASS
static int _gc_destroy_garbage_class(void* data, void* extra, _gc_t* gc);
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_CLASS
static int _gc_destroy_garbage_outer_scope(void* data, void* extra, _gc_t* gc);
#endif /* MB_ENABLE_CLASS */
static void _gc_swap_tables(mb_interpreter_t* s);
static void _gc_try_trigger(mb_interpreter_t* s);
static void _gc_collect_garbage(mb_interpreter_t* s, int depth);

#ifdef MB_ENABLE_USERTYPE_REF
static _usertype_ref_t* _create_usertype_ref(mb_interpreter_t* s, void* val, mb_dtor_func_t un, mb_clone_func_t cl, mb_hash_func_t hs, mb_cmp_func_t cp, mb_fmt_func_t ft);
static void _destroy_usertype_ref(_usertype_ref_t* c);
static void _unref_usertype_ref(_ref_t* ref, void* data);
static void _clone_usertype_ref(_usertype_ref_t* src, _object_t* tgt);
static bool_t _try_call_func_on_usertype_ref(mb_interpreter_t* s, _ls_node_t** ast, _object_t* obj, _ls_node_t* pathed, int* ret);
#endif /* MB_ENABLE_USERTYPE_REF */

static _array_t* _create_array(mb_interpreter_t* s, const char* n, _data_e t);
static void _destroy_array(_array_t* arr);
static void _init_array(_array_t* arr);
static _array_t* _clone_array(mb_interpreter_t* s, _array_t* arr);
static int _get_array_pos(mb_interpreter_t* s, _array_t* arr, int* d, int c);
static int _get_array_index(mb_interpreter_t* s, _ls_node_t** l, _object_t* c, unsigned* index, bool_t* literally);
static bool_t _get_array_elem(mb_interpreter_t* s, _array_t* arr, unsigned index, mb_value_u* val, _data_e* type);
static int _set_array_elem(mb_interpreter_t* s, _ls_node_t* ast, _array_t* arr, unsigned index, mb_value_u* val, _data_e* type);
static void _clear_array(_array_t* arr);
static bool_t _is_array(void* obj);
#ifdef MB_ENABLE_ARRAY_REF
static void _unref_array(_ref_t* ref, void* data);
#endif /* MB_ENABLE_ARRAY_REF */

#ifdef MB_ENABLE_COLLECTION_LIB
static _list_t* _create_list(mb_interpreter_t* s);
static void _destroy_list(_list_t* c);
static _dict_t* _create_dict(mb_interpreter_t* s);
static void _destroy_dict(_dict_t* c);
static _list_it_t* _create_list_it(_list_t* coll, bool_t lock);
static bool_t _destroy_list_it(_list_it_t* it);
static _list_it_t* _move_list_it_next(_list_it_t* it);
static _dict_it_t* _create_dict_it(_dict_t* coll, bool_t lock);
static bool_t _destroy_dict_it(_dict_it_t* it);
static _dict_it_t* _move_dict_it_next(_dict_it_t* it);
static void _unref_list(_ref_t* ref, void* data);
static void _unref_dict(_ref_t* ref, void* data);
static bool_t _push_list(_list_t* coll, mb_value_t* val, _object_t* oarg);
static bool_t _pop_list(_list_t* coll, mb_value_t* val, mb_interpreter_t* s);
static bool_t _insert_list(_list_t* coll, int_t idx, mb_value_t* val, _object_t** oval);
static bool_t _set_list(_list_t* coll, int_t idx, mb_value_t* val, _object_t** oval);
static bool_t _remove_at_list(_list_t* coll, int_t idx);
static _ls_node_t* _node_at_list(_list_t* coll, int index);
static bool_t _at_list(_list_t* coll, int_t idx, mb_value_t* oval);
static bool_t _find_list(_list_t* coll, mb_value_t* val, int* idx);
static void _clear_list(_list_t* coll);
static void _sort_list(_list_t* coll);
static void _invalidate_list_cache(_list_t* coll);
static void _fill_ranged(_list_t* coll);
static bool_t _set_dict(_dict_t* coll, mb_value_t* key, mb_value_t* val, _object_t* okey, _object_t* oval);
static bool_t _remove_dict(_dict_t* coll, mb_value_t* key);
static bool_t _find_dict(_dict_t* coll, mb_value_t* val, mb_value_t* oval);
static void _clear_dict(_dict_t* coll);
static bool_t _invalid_list_it(_list_it_t* it);
static bool_t _invalid_dict_it(_dict_it_t* it);
static bool_t _assign_with_it(_object_t* tgt, _object_t* src);
static bool_t _try_purge_it(mb_interpreter_t* s, mb_value_t* val, _object_t* obj);
static int _clone_to_list(void* data, void* extra, _list_t* coll);
static int _clone_to_dict(void* data, void* extra, _dict_t* coll);
static int _copy_list_to_array(void* data, void* extra, _array_helper_t* h);
static int _copy_keys_to_value_array(void* data, void* extra, _keys_helper_t* h);
#endif /* MB_ENABLE_COLLECTION_LIB */

#ifdef MB_ENABLE_CLASS
typedef int (* _class_scope_walker_t)(void*, void*, void*);
typedef bool_t (* _class_meta_walker_t)(_class_t*, void*, void*);
static void _init_class(mb_interpreter_t* s, _class_t* instance, char* n);
static void _begin_class(mb_interpreter_t* s);
static bool_t _end_class(mb_interpreter_t* s);
static void _unref_class(_ref_t* ref, void* data);
static void _destroy_class(_class_t* c);
static bool_t _traverse_class(_class_t* c, _class_scope_walker_t scope_walker, _class_meta_walker_t meta_walker, unsigned meta_depth, bool_t meta_walk_on_self, void* extra_data, void* ret);
static bool_t _link_meta_class(mb_interpreter_t* s, _class_t* derived, _class_t* base);
static void _unlink_meta_class(mb_interpreter_t* s, _class_t* derived);
static int _unlink_meta_instance(void* data, void* extra, _class_t* derived);
static int _clone_clsss_field(void* data, void* extra, void* n);
static bool_t _clone_class_meta_link(_class_t* meta, void* n, void* ret);
static int _search_class_meta_function(mb_interpreter_t* s, _class_t* instance, const char* n, _routine_t* _UNALIGNED_ARG * f);
static int _search_class_hash_and_compare_functions(mb_interpreter_t* s, _class_t* instance);
static bool_t _is_a_class(_class_t* instance, void* m, void* ret);
static bool_t _add_class_meta_reachable(_class_t* meta, void* ht, void* ret);
#ifdef MB_ENABLE_COLLECTION_LIB
static int _reflect_class_field(void* data, void* extra, void* d);
#endif /* MB_ENABLE_COLLECTION_LIB */
static int _format_class_to_string(mb_interpreter_t* s, void** l, _class_t* instance, _object_t* out, bool_t* got_tostr);
static _class_t* _reflect_string_to_class(mb_interpreter_t* s, const char* n, mb_value_t* arg);
static bool_t _is_valid_class_accessor_following_routine(mb_interpreter_t* s, _var_t* var, _ls_node_t* ast, _ls_node_t** out);
#endif /* MB_ENABLE_CLASS */
static void _init_routine(mb_interpreter_t* s, _routine_t* routine, char* n, mb_routine_func_t f);
static int _begin_routine(mb_interpreter_t* s);
static bool_t _end_routine(mb_interpreter_t* s);
static void _begin_routine_definition(mb_interpreter_t* s);
static void _begin_routine_parameter_list(mb_interpreter_t* s);
static void _end_routine_parameter_list(mb_interpreter_t* s);
static _object_t* _duplicate_parameter(void* data, void* extra, _running_context_t* running);
static _routine_t* _clone_routine(_routine_t* sub, void* c, bool_t toupval);
#ifdef MB_ENABLE_LAMBDA
static _running_context_t* _init_lambda(mb_interpreter_t* s, _routine_t* routine);
static void _unref_routine(_ref_t* ref, void* data);
static void _destroy_routine(mb_interpreter_t* s, _routine_t* r);
static void _mark_upvalue(mb_interpreter_t* s, _lambda_t* lambda, _object_t* obj, const char* n);
static void _try_mark_upvalue(mb_interpreter_t* s, _routine_t* r, _object_t* obj);
static _running_context_ref_t* _create_outer_scope(mb_interpreter_t* s);
static void _unref_outer_scope(_ref_t* ref, void* data);
static void _destroy_outer_scope(_running_context_ref_t* p);
static int _do_nothing_on_ht_for_lambda(void* data, void* extra);
static int _fill_with_upvalue(void* data, void* extra, _upvalue_scope_tuple_t* tuple);
static int _remove_filled_upvalue(void* data, void* extra, _ht_node_t* ht);
static int _fill_outer_scope(void* data, void* extra, _upvalue_scope_tuple_t* tuple);
static int _remove_this_lambda_from_upvalue(void* data, void* extra, _routine_t* routine);
static _running_context_t* _link_lambda_scope_chain(mb_interpreter_t* s, _lambda_t* lambda, bool_t weak);
static _running_context_t* _unlink_lambda_scope_chain(mb_interpreter_t* s, _lambda_t* lambda, bool_t weak);
static bool_t _is_valid_lambda_body_node(mb_interpreter_t* s, _lambda_t* lambda, _object_t* obj);
#endif /* MB_ENABLE_LAMBDA */
#ifdef MB_ENABLE_CLASS
static _running_context_t* _reference_scope_by_class(mb_interpreter_t* s, _running_context_t* p, _class_t* c);
static _running_context_t* _push_scope_by_class(mb_interpreter_t* s, _running_context_t* p);
static _ls_node_t* _search_identifier_in_class(mb_interpreter_t* s, _class_t* instance, const char* n, _ht_node_t** ht, _running_context_t** sp);
#endif /* MB_ENABLE_CLASS */
static _running_context_t* _reference_scope_by_routine(mb_interpreter_t* s, _running_context_t* p, _routine_t* r);
static _running_context_t* _push_weak_scope_by_routine(mb_interpreter_t* s, _running_context_t* p, _routine_t* r);
static _running_context_t* _push_scope_by_routine(mb_interpreter_t* s, _running_context_t* p);
static void _destroy_scope(mb_interpreter_t* s, _running_context_t* p);
static _running_context_t* _pop_weak_scope(mb_interpreter_t* s, _running_context_t* p);
static _running_context_t* _pop_scope(mb_interpreter_t* s, bool_t tidy);
static void _out_of_scope(mb_interpreter_t* s, _running_context_t* running, void* instance, _routine_t* routine, bool_t lose);
static _running_context_t* _find_scope(mb_interpreter_t* s, _running_context_t* p);
static _running_context_t* _get_root_scope(_running_context_t* scope);
#ifdef MB_ENABLE_LAMBDA
static _running_context_ref_t* _get_root_ref_scope(_running_context_ref_t* scope);
#endif /* MB_ENABLE_LAMBDA */
static _running_context_t* _get_scope_to_add_routine(mb_interpreter_t* s);
static _ls_node_t* _search_identifier_in_scope_chain(mb_interpreter_t* s, _running_context_t* scope, const char* n, int fp, _ht_node_t** ht, _running_context_t** sp);
static _array_t* _search_array_in_scope_chain(mb_interpreter_t* s, _array_t* i, _object_t** o);
static _var_t* _search_var_in_scope_chain(mb_interpreter_t* s, _var_t* i, _object_t** o);
static _ls_node_t* _search_identifier_accessor(mb_interpreter_t* s, _running_context_t* scope, const char* n, _ht_node_t** ht, _running_context_t** sp, bool_t unknown_for_not_found);

static _var_t* _create_var(_object_t** oobj, const char* n, size_t ns, bool_t dup_name);
static _object_t* _create_object(void);
static int _clone_object(mb_interpreter_t* s, _object_t* obj, _object_t* tgt, bool_t toupval, bool_t deep);
static int _dispose_object(_object_t* obj);
static int _destroy_object(void* data, void* extra);
static int _destroy_object_with_extra(void* data, void* extra);
static int _destroy_object_not_compile_time(void* data, void* extra);
static int _destroy_object_capsule_only(void* data, void* extra);
static int _do_nothing_on_object(void* data, void* extra);
static int _lose_object(void* data, void* extra, _running_context_t* running);
static int _remove_source_object(void* data, void* extra);
static int _destroy_memory(void* data, void* extra);
static int _compare_numbers(const _object_t* first, const _object_t* second);
static bool_t _is_internal_object(_object_t* obj);
static _data_e _public_type_to_internal_type(mb_data_e t);
static mb_data_e _internal_type_to_public_type(_data_e t);
static int _public_value_to_internal_object(mb_value_t* pbl, _object_t* itn);
static int _internal_object_to_public_value(_object_t* itn, mb_value_t* pbl);
static int _create_internal_object_from_public_value(mb_value_t* pbl, _object_t** itn);
static int _compare_public_value_and_internal_object(mb_value_t* pbl, _object_t* itn);
static void _try_clear_intermediate_value(void* data, void* extra, mb_interpreter_t* s);
static void _remove_if_exists(void* data, void* extra, _ls_node_t* ls);
static void _destroy_var_arg(void* data, void* extra, _gc_t* gc);
static void _destroy_edge_objects(mb_interpreter_t* s);
static void _mark_edge_destroy_string(mb_interpreter_t* s, char* ch);
static void _destroy_lazy_objects(mb_interpreter_t* s);
static void _mark_lazy_destroy_string(mb_interpreter_t* s, char* ch);
static void _assign_public_value(mb_interpreter_t* s, mb_value_t* tgt, mb_value_t* src, bool_t pit);
static void _swap_public_value(mb_value_t* tgt, mb_value_t* src);
static int _clear_scope_chain(mb_interpreter_t* s);
static int _dispose_scope_chain(mb_interpreter_t* s);
static void _tidy_scope_chain(mb_interpreter_t* s);
static void _collect_intermediate_value_in_scope(_running_context_t* running, void* data);
#ifdef MB_ENABLE_FORK
static void _collect_intermediate_value_in_scope_chain(void* data, void* extra, void* d);
#endif /* MB_ENABLE_FORK */
static void _collect_intermediate_value(_ref_t* ref, void* data);
static void _mark_dangling_intermediate_value(mb_interpreter_t* s, _running_context_t* running);
static _object_t* _eval_var_in_print(mb_interpreter_t* s, _object_t** val_ptr, _ls_node_t** ast, _object_t* obj);

/** Interpretation */

static int _stepped(mb_interpreter_t* s, _ls_node_t* ast);
static int _execute_statement(mb_interpreter_t* s, _ls_node_t** l, bool_t force_next);
static int _common_end_looping(mb_interpreter_t* s, _ls_node_t** l);
static int _common_keep_looping(mb_interpreter_t* s, _ls_node_t** l, _var_t* var_loop);
static int _execute_normal_for_loop(mb_interpreter_t* s, _ls_node_t** l, _var_t* var_loop);
#ifdef MB_ENABLE_COLLECTION_LIB
static int _execute_ranged_for_loop(mb_interpreter_t* s, _ls_node_t** l, _var_t* var_loop);
#endif /* MB_ENABLE_COLLECTION_LIB */
static int _skip_to(mb_interpreter_t* s, _ls_node_t** l, mb_func_t f, _data_e t);
static bool_t _skip_single_line_struct(_ls_node_t** ast, mb_func_t func);
static int _skip_if_chunk(mb_interpreter_t* s, _ls_node_t** l);
static int _skip_struct(mb_interpreter_t* s, _ls_node_t** l, mb_func_t open_func, mb_func_t post_open_func, mb_func_t close_func);
static bool_t _multiline_statement(mb_interpreter_t* s);

static _running_context_t* _create_running_context(bool_t create_var_dict);
static _parsing_context_t* _reset_parsing_context(_parsing_context_t* context);
static void _destroy_parsing_context(_parsing_context_t* _UNALIGNED_ARG * context);

/** Interface processors */

#ifdef MB_ENABLE_MODULE
static _module_func_t* _create_module_func(mb_interpreter_t* s, mb_func_t f);
static int _ls_destroy_module_func(void* data, void* extra);
static int _ht_destroy_module_func_list(void* data, void* extra);
#endif /* MB_ENABLE_MODULE */
static char* _generate_func_name(mb_interpreter_t* s, char* n, bool_t with_mod);
static int _register_func(mb_interpreter_t* s, char* n, mb_func_t f, bool_t local);
static int _remove_func(mb_interpreter_t* s, char* n, bool_t local);
static _ls_node_t* _find_func(mb_interpreter_t* s, char* n, bool_t* mod);

static int _open_constant(mb_interpreter_t* s);
static int _close_constant(mb_interpreter_t* s);
static int _open_core_lib(mb_interpreter_t* s);
static int _close_core_lib(mb_interpreter_t* s);
static int _open_std_lib(mb_interpreter_t* s);
static int _close_std_lib(mb_interpreter_t* s);
#ifdef MB_ENABLE_COLLECTION_LIB
static int _open_coll_lib(mb_interpreter_t* s);
static int _close_coll_lib(mb_interpreter_t* s);
#endif /* MB_ENABLE_COLLECTION_LIB */

/* ========================================================} */

/*
** {========================================================
** Lib declarations
*/

/** Macro */

#ifdef MB_CP_VC
# if MB_CP_VC < 1300
#   define MB_FUNC 0
# else /* MB_CP_VC < 1300 */
#   define MB_FUNC __FUNCTION__
# endif /* MB_CP_VC < 1300 */
#elif defined MB_CP_BORLANDC
# define MB_FUNC __FUNC__
#elif defined MB_CP_PELLESC
# define MB_FUNC __func__
#else /* MB_CP_VC */
# define MB_FUNC __FUNCTION__
#endif /* MB_CP_VC */

#ifdef MB_CP_VC
# if MB_CP_VC < 1300
#   define _do_nothing(__s, __l, __exit, __result) \
			do { \
				_ls_node_t* ast = 0; static int i = 0; ++i; \
				printf("Unaccessable function called %d times.\n", i); \
				ast = (_ls_node_t*)(*(__l)); \
				_handle_error_on_obj((__s), SE_RN_WRONG_FUNCTION_REACHED, (__s)->source_file, DON(ast), MB_FUNC_ERR, __exit, (__result)); \
			} while(0)
# endif /* MB_CP_VC < 1300 */
#endif /* MB_CP_VC */
#ifndef _do_nothing
# define _do_nothing(__s, __l, __exit, __result) \
		do { \
			_ls_node_t* ast = (_ls_node_t*)(*(__l)); \
			_handle_error_on_obj((__s), SE_RN_WRONG_FUNCTION_REACHED, (char*)MB_FUNC, DON(ast), MB_FUNC_ERR, __exit, (__result)); \
		} while(0);
#endif /* _do_nothing */

/** Core lib */

static int _core_dummy_assign(mb_interpreter_t* s, void** l);
static int _core_add(mb_interpreter_t* s, void** l);
static int _core_min(mb_interpreter_t* s, void** l);
static int _core_mul(mb_interpreter_t* s, void** l);
static int _core_div(mb_interpreter_t* s, void** l);
static int _core_mod(mb_interpreter_t* s, void** l);
static int _core_pow(mb_interpreter_t* s, void** l);
static int _core_open_bracket(mb_interpreter_t* s, void** l);
static int _core_close_bracket(mb_interpreter_t* s, void** l);
static int _core_neg(mb_interpreter_t* s, void** l);
static int _core_equal(mb_interpreter_t* s, void** l);
static int _core_less(mb_interpreter_t* s, void** l);
static int _core_greater(mb_interpreter_t* s, void** l);
static int _core_less_equal(mb_interpreter_t* s, void** l);
static int _core_greater_equal(mb_interpreter_t* s, void** l);
static int _core_not_equal(mb_interpreter_t* s, void** l);
static int _core_and(mb_interpreter_t* s, void** l);
static int _core_or(mb_interpreter_t* s, void** l);
static int _core_not(mb_interpreter_t* s, void** l);
static int _core_is(mb_interpreter_t* s, void** l);
static int _core_let(mb_interpreter_t* s, void** l);
static int _core_dim(mb_interpreter_t* s, void** l);
static int _core_if(mb_interpreter_t* s, void** l);
static int _core_then(mb_interpreter_t* s, void** l);
static int _core_elseif(mb_interpreter_t* s, void** l);
static int _core_else(mb_interpreter_t* s, void** l);
static int _core_endif(mb_interpreter_t* s, void** l);
static int _core_for(mb_interpreter_t* s, void** l);
#ifdef MB_ENABLE_COLLECTION_LIB
static int _core_in(mb_interpreter_t* s, void** l);
#endif /* MB_ENABLE_COLLECTION_LIB */
static int _core_to(mb_interpreter_t* s, void** l);
static int _core_step(mb_interpreter_t* s, void** l);
static int _core_next(mb_interpreter_t* s, void** l);
static int _core_while(mb_interpreter_t* s, void** l);
static int _core_wend(mb_interpreter_t* s, void** l);
static int _core_do(mb_interpreter_t* s, void** l);
static int _core_until(mb_interpreter_t* s, void** l);
static int _core_exit(mb_interpreter_t* s, void** l);
static int _core_goto(mb_interpreter_t* s, void** l);
static int _core_gosub(mb_interpreter_t* s, void** l);
static int _core_return(mb_interpreter_t* s, void** l);
static int _core_call(mb_interpreter_t* s, void** l);
static int _core_def(mb_interpreter_t* s, void** l);
static int _core_enddef(mb_interpreter_t* s, void** l);
static int _core_args(mb_interpreter_t* s, void** l);
#ifdef MB_ENABLE_CLASS
static int _core_class(mb_interpreter_t* s, void** l);
static int _core_endclass(mb_interpreter_t* s, void** l);
static int _core_new(mb_interpreter_t* s, void** l);
static int _core_var(mb_interpreter_t* s, void** l);
static int _core_reflect(mb_interpreter_t* s, void** l);
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_LAMBDA
static int _core_lambda(mb_interpreter_t* s, void** l);
#endif /* MB_ENABLE_LAMBDA */
#ifdef MB_ENABLE_ALLOC_STAT
static int _core_mem(mb_interpreter_t* s, void** l);
#endif /* MB_ENABLE_ALLOC_STAT */
static int _core_type(mb_interpreter_t* s, void** l);
static int _core_import(mb_interpreter_t* s, void** l);
static int _core_end(mb_interpreter_t* s, void** l);

/** Standard lib */

static int _std_abs(mb_interpreter_t* s, void** l);
static int _std_sgn(mb_interpreter_t* s, void** l);
static int _std_sqr(mb_interpreter_t* s, void** l);
static int _std_floor(mb_interpreter_t* s, void** l);
static int _std_ceil(mb_interpreter_t* s, void** l);
static int _std_fix(mb_interpreter_t* s, void** l);
static int _std_round(mb_interpreter_t* s, void** l);
static int _std_srnd(mb_interpreter_t* s, void** l);
static int _std_rnd(mb_interpreter_t* s, void** l);
static int _std_sin(mb_interpreter_t* s, void** l);
static int _std_cos(mb_interpreter_t* s, void** l);
static int _std_tan(mb_interpreter_t* s, void** l);
static int _std_asin(mb_interpreter_t* s, void** l);
static int _std_acos(mb_interpreter_t* s, void** l);
static int _std_atan(mb_interpreter_t* s, void** l);
static int _std_exp(mb_interpreter_t* s, void** l);
static int _std_log(mb_interpreter_t* s, void** l);
static int _std_asc(mb_interpreter_t* s, void** l);
static int _std_chr(mb_interpreter_t* s, void** l);
static int _std_left(mb_interpreter_t* s, void** l);
static int _std_mid(mb_interpreter_t* s, void** l);
static int _std_right(mb_interpreter_t* s, void** l);
static int _std_str(mb_interpreter_t* s, void** l);
static int _std_val(mb_interpreter_t* s, void** l);
static int _std_len(mb_interpreter_t* s, void** l);
static int _std_get(mb_interpreter_t* s, void** l);
static int _std_set(mb_interpreter_t* s, void** l);
static int _std_print(mb_interpreter_t* s, void** l);
static int _std_input(mb_interpreter_t* s, void** l);

/** Collection lib */

#ifdef MB_ENABLE_COLLECTION_LIB
static int _coll_list(mb_interpreter_t* s, void** l);
static int _coll_dict(mb_interpreter_t* s, void** l);
static int _coll_push(mb_interpreter_t* s, void** l);
static int _coll_pop(mb_interpreter_t* s, void** l);
static int _coll_back(mb_interpreter_t* s, void** l);
static int _coll_insert(mb_interpreter_t* s, void** l);
static int _coll_sort(mb_interpreter_t* s, void** l);
static int _coll_exists(mb_interpreter_t* s, void** l);
static int _coll_index_of(mb_interpreter_t* s, void** l);
static int _coll_remove(mb_interpreter_t* s, void** l);
static int _coll_clear(mb_interpreter_t* s, void** l);
static int _coll_clone(mb_interpreter_t* s, void** l);
static int _coll_to_array(mb_interpreter_t* s, void** l);
static int _coll_iterator(mb_interpreter_t* s, void** l);
static int _coll_move_next(mb_interpreter_t* s, void** l);
#endif /* MB_ENABLE_COLLECTION_LIB */

/** Lib information */


static yield_interval = 100;

#define _CORE_ID_TYPE "TYPE"

MBCONST static const _func_t _core_libs[] = {
	{ _DUMMY_ASSIGN_CHAR, _core_dummy_assign },
	{ "+", _core_add },
	{ "-", _core_min },
	{ "*", _core_mul },
	{ "/", _core_div },
	{ "MOD", _core_mod },
	{ "^", _core_pow },
	{ "(", _core_open_bracket },
	{ ")", _core_close_bracket },
	{ 0, _core_neg },

	{ "=", _core_equal },
	{ "<", _core_less },
	{ ">", _core_greater },
	{ "<=", _core_less_equal },
	{ ">=", _core_greater_equal },
	{ "<>", _core_not_equal },

	{ "AND", _core_and },
	{ "OR", _core_or },
	{ "NOT", _core_not },

	{ "IS", _core_is },

	{ "LET", _core_let },
	{ "DIM", _core_dim },

	{ "IF", _core_if },
	{ "THEN", _core_then },
	{ "ELSEIF", _core_elseif },
	{ "ELSE", _core_else },
	{ "ENDIF", _core_endif },

	{ "FOR", _core_for },
#ifdef MB_ENABLE_COLLECTION_LIB
	{ "IN", _core_in },
#endif /* MB_ENABLE_COLLECTION_LIB */
	{ "TO", _core_to },
	{ "STEP", _core_step },
	{ "NEXT", _core_next },
	{ "WHILE", _core_while },
	{ "WEND", _core_wend },
	{ "DO", _core_do },
	{ "UNTIL", _core_until },

	{ "EXIT", _core_exit },
	{ "GOTO", _core_goto },
	{ "GOSUB", _core_gosub },
	{ "RETURN", _core_return },

	{ "CALL", _core_call },
	{ "DEF", _core_def },
	{ "ENDDEF", _core_enddef },
	{ _VAR_ARGS_STR, _core_args },

#ifdef MB_ENABLE_CLASS
	{ "CLASS", _core_class },
	{ "ENDCLASS", _core_endclass },
	{ "NEW", _core_new },
	{ "VAR", _core_var },
	{ "REFLECT", _core_reflect },
#endif /* MB_ENABLE_CLASS */

#ifdef MB_ENABLE_LAMBDA
	{ "LAMBDA", _core_lambda },
#ifdef MB_LAMBDA_ALIAS
	{ MB_LAMBDA_ALIAS, _core_lambda },
#endif /* MB_LAMBDA_ALIAS */
#endif /* MB_ENABLE_LAMBDA */

#ifdef MB_ENABLE_ALLOC_STAT
	{ "MEM", _core_mem },
#endif /* MB_ENABLE_ALLOC_STAT */

	{ _CORE_ID_TYPE, _core_type },
	{ "IMPORT", _core_import },
	{ "END", _core_end }
};

#define _STD_ID_VAL "VAL"
#define _STD_ID_LEN "LEN"
#define _STD_ID_GET "GET"
#define _STD_ID_SET "SET"

MBCONST static const _func_t _std_libs[] = {
	{ "ABS", _std_abs },
	{ "SGN", _std_sgn },
	{ "SQR", _std_sqr },
	{ "FLOOR", _std_floor },
	{ "CEIL", _std_ceil },
	{ "FIX", _std_fix },
	{ "ROUND", _std_round },
	{ "SRND", _std_srnd },
	{ "RND", _std_rnd },
	{ "SIN", _std_sin },
	{ "COS", _std_cos },
	{ "TAN", _std_tan },
	{ "ASIN", _std_asin },
	{ "ACOS", _std_acos },
	{ "ATAN", _std_atan },
	{ "EXP", _std_exp },
	{ "LOG", _std_log },

	{ "ASC", _std_asc },
	{ "CHR", _std_chr },
	{ "LEFT", _std_left },
	{ "MID", _std_mid },
	{ "RIGHT", _std_right },
	{ "STR", _std_str },
	{ _STD_ID_VAL, _std_val },

	{ _STD_ID_LEN, _std_len },
	{ _STD_ID_GET, _std_get },
	{ _STD_ID_SET, _std_set },

	{ "PRINT", _std_print },
	{ "INPUT", _std_input }
};

#ifdef MB_ENABLE_COLLECTION_LIB
# define _COLL_ID_LIST "LIST"
# define _COLL_ID_DICT "DICT"
# define _COLL_ID_PUSH "PUSH"
# define _COLL_ID_POP "POP"
# define _COLL_ID_BACK "BACK"
# define _COLL_ID_INSERT "INSERT"
# define _COLL_ID_SORT "SORT"
# define _COLL_ID_EXISTS "EXISTS"
# define _COLL_ID_INDEX_OF "INDEX_OF"
# define _COLL_ID_REMOVE "REMOVE"
# define _COLL_ID_CLEAR "CLEAR"
# define _COLL_ID_CLONE "CLONE"
# define _COLL_ID_TO_ARRAY "TO_ARRAY"
# define _COLL_ID_ITERATOR "ITERATOR"
# define _COLL_ID_MOVE_NEXT "MOVE_NEXT"

MBCONST static const _func_t _coll_libs[] = {
	{ _COLL_ID_LIST, _coll_list },
	{ _COLL_ID_DICT, _coll_dict },
	{ _COLL_ID_PUSH, _coll_push },
	{ _COLL_ID_POP, _coll_pop },
	{ _COLL_ID_BACK, _coll_back },
	{ _COLL_ID_INSERT, _coll_insert },
	{ _COLL_ID_SORT, _coll_sort },
	{ _COLL_ID_EXISTS, _coll_exists },
	{ _COLL_ID_INDEX_OF, _coll_index_of },
	{ _COLL_ID_REMOVE, _coll_remove },
	{ _COLL_ID_CLEAR, _coll_clear },
	{ _COLL_ID_CLONE, _coll_clone },
	{ _COLL_ID_TO_ARRAY, _coll_to_array },
	{ _COLL_ID_ITERATOR, _coll_iterator },
	{ _COLL_ID_MOVE_NEXT, _coll_move_next }
};
#endif /* MB_ENABLE_COLLECTION_LIB */

/* ========================================================} */

/*
** {========================================================
** Private function definitions
*/

/** List operations */

static int _ls_cmp_data(void* node, void* info) {
	_ls_node_t* n = (_ls_node_t*)node;

	return (n->data == info) ? 0 : 1;
}

static int _ls_cmp_extra(void* node, void* info) {
	_ls_node_t* n = (_ls_node_t*)node;

	return (n->extra == info) ? 0 : 1;
}

static int _ls_cmp_extra_object(void* node, void* info) {
	_ls_node_t* n = (_ls_node_t*)node;

	return _ht_cmp_object(n->extra, info);
}

static int _ls_cmp_extra_string(void* node, void* info) {
	_ls_node_t* n = (_ls_node_t*)node;
	char* s1 = (char*)n->extra;
	char* s2 = (char*)info;

	return strcmp(s1, s2);
}

#ifdef MB_ENABLE_MODULE
static int _ls_cmp_module_func(void* node, void* info) {
	_module_func_t* m = (_module_func_t*)node;
	mb_interpreter_t* s = (mb_interpreter_t*)info;

	return strcmp(m->module, s->with_module);
}
#endif /* MB_ENABLE_MODULE */

static _ls_node_t* _ls_create_node(void* data) {
	_ls_node_t* result = 0;

	result = (_ls_node_t*)mb_malloc(sizeof(_ls_node_t));
	mb_assert(result);
	memset(result, 0, sizeof(_ls_node_t));
	result->data = data;

	return result;
}

static _ls_node_t* _ls_create(void) {
	_ls_node_t* result = 0;

	result = _ls_create_node(0);

	return result;
}

static _ls_node_t* _ls_find(_ls_node_t* list, void* data, _ls_compare_t cmp, int* idx) {
	_ls_node_t* result = 0;

	mb_assert(list && data && cmp);

	if(idx) *idx = 0;

	list = list->next;
	while(list) {
		if(!cmp(list->data, data)) {
			result = list;

			break;
		}
		list = list->next;
		if(idx) ++*idx;
	}

	return result;
}

static _ls_node_t* _ls_back(_ls_node_t* node) {
	_ls_node_t* result = node;

	result = result->prev;

	return result;
}

static _ls_node_t* _ls_pushback(_ls_node_t* list, void* data) {
	_ls_node_t* result = 0;
	_ls_node_t* tmp = 0;

	mb_assert(list);

	result = _ls_create_node(data);

	tmp = _ls_back(list);
	if(!tmp)
		tmp = list;
	tmp->next = result;
	result->prev = tmp;
	list->prev = result;
	list->data = (char*)list->data + 1;

	return result;
}

static void* _ls_popback(_ls_node_t* list) {
	void* result = 0;
	_ls_node_t* tmp = 0;

	mb_assert(list);

	tmp = _ls_back(list);
	if(tmp) {
		result = tmp->data;
		if(list != tmp->prev)
			list->prev = tmp->prev;
		else
			list->prev = 0;
		tmp->prev->next = 0;
		safe_free(tmp);
		list->data = (char*)list->data - 1;
	}

	return result;
}

static _ls_node_t* _ls_front(_ls_node_t* node) {
	_ls_node_t* result = node;

	result = result->next;

	return result;
}

static void* _ls_popfront(_ls_node_t* list) {
	void* result = 0;
	_ls_node_t* tmp = 0;

	mb_assert(list);

	tmp = _ls_front(list);
	if(tmp) {
		result = tmp->data;
		list->next = tmp->next;
		if(tmp->next)
			tmp->next->prev = list;
		if(!list->next)
			list->prev = 0;
		tmp->prev = tmp->next = 0;
		safe_free(tmp);
		list->data = (char*)list->data - 1;
	}

	return result;
}

static _ls_node_t* _ls_insert_at(_ls_node_t* list, int index, void* data) {
	_ls_node_t* result = 0;
	_ls_node_t* tmp = 0;

	mb_assert(list);

	tmp = list->next;
	while(tmp && index) {
		tmp = tmp->next;
		--index;
	}
	if(!tmp) {
		if(index == 0)
			result = _ls_pushback(list, data);
	} else {
		result = _ls_create_node(data);
		tmp->prev->next = result;
		result->prev = tmp->prev;
		result->next = tmp;
		tmp->prev = result;
	}
	list->data = (char*)list->data + 1;

	return result;
}

static unsigned _ls_remove(_ls_node_t* list, _ls_node_t* node, _ls_operation_t op) {
	unsigned result = 0;

	mb_assert(list && node);

	if(node->prev)
		node->prev->next = node->next;
	if(node->next)
		node->next->prev = node->prev;
	if(list->prev == node)
		list->prev = node->prev;
	if(list->prev == list)
		list->prev = 0;
	if(op)
		op(node->data, node->extra);
	safe_free(node);
	++result;
	list->data = (char*)list->data - 1;

	return result;
}

static unsigned _ls_try_remove(_ls_node_t* list, void* info, _ls_compare_t cmp, _ls_operation_t op) {
	unsigned result = 0;
	_ls_node_t* tmp = 0;

	mb_assert(list && cmp);

	tmp = list->next;
	while(tmp) {
		if(cmp(tmp, info) == 0) {
			if(tmp->prev)
				tmp->prev->next = tmp->next;
			if(tmp->next)
				tmp->next->prev = tmp->prev;
			if(list->prev == tmp)
				list->prev = tmp->prev;
			if(list->prev == list)
				list->prev = 0;
			if(op)
				op(tmp->data, tmp->extra);
			safe_free(tmp);
			++result;
			list->data = (char*)list->data - 1;

			break;
		}
		tmp = tmp->next;
	}

	return result;
}

static unsigned _ls_foreach(_ls_node_t* list, _ls_operation_t op) {
	unsigned idx = 0;
	int opresult = _OP_RESULT_NORMAL;
	_ls_node_t* node = 0;
	_ls_node_t* tmp = 0;

	mb_assert(list && op);

	node = list->next;
	while(node) {
		opresult = op(node->data, node->extra);
		++idx;
		tmp = node;
		node = node->next;

		if(_OP_RESULT_DEL_NODE == opresult) {
			tmp->prev->next = node;
			if(node)
				node->prev = tmp->prev;
			safe_free(tmp);
			list->data = (char*)list->data - 1;
		}
	}

	return idx;
}

static _ls_node_t* _ls_sort(_ls_node_t* _UNALIGNED_ARG * list, _ls_compare_t cmp) {
	/* Copyright 2001 Simon Tatham, http://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.c */
	bool_t is_circular = false, is_double = true;
	_ls_node_t* p, * q, * e, * tail, * oldhead;
	int insize, nmerges, psize, qsize, i;
	_ls_node_t* lst = 0;

	mb_assert(list && *list && cmp);

	lst = *list;
	if(lst) lst = lst->next;

	if(!lst)
		return 0;

	insize = 1;

	while(1) {
		p = lst;
		oldhead = lst;
		lst = 0;
		tail = 0;

		nmerges = 0;

		while(p) {
			nmerges++;
			q = p;
			psize = 0;
			for(i = 0; i < insize; i++) {
				psize++;
				if(is_circular)
					q = (q->next == oldhead ? 0 : q->next);
				else
					q = q->next;
				if(!q)
					break;
			}

			qsize = insize;

			while(psize > 0 || (qsize > 0 && q)) {
				if(psize == 0) {
					e = q; q = q->next; qsize--;
					if(is_circular && q == oldhead) q = 0;
				} else if(qsize == 0 || !q) {
					e = p; p = p->next; psize--;
					if(is_circular && p == oldhead) p = 0;
				} else if(cmp(p->data, q->data) <= 0) {
					e = p; p = p->next; psize--;
					if(is_circular && p == oldhead) p = 0;
				} else {
					e = q; q = q->next; qsize--;
					if(is_circular && q == oldhead) q = 0;
				}

				if(tail)
					tail->next = e;
				else
					lst = e;
				if(is_double)
					e->prev = tail;
				tail = e;
			}

			p = q;
		}
		if(is_circular) {
			tail->next = lst;
			if(is_double)
				lst->prev = tail;
		} else {
			tail->next = 0;
		}

		if(nmerges <= 1) {
			(*list)->next = lst;
			(*list)->prev = tail;

			lst->prev = *list;

			return *list;
		}

		insize *= 2;
	}
}

static unsigned _ls_count(_ls_node_t* list) {
	union { void* p; unsigned u; } tmp;

	mb_assert(list);

	tmp.p = list->data;

	return tmp.u;
}

static bool_t _ls_empty(_ls_node_t* list) {
	bool_t result = false;

	mb_assert(list);

	result = 0 == list->next;

	return result;
}

static void _ls_clear(_ls_node_t* list) {
	_ls_node_t* tmp = 0;

	mb_assert(list);

	list->data = 0;

	tmp = list;
	list = list->next;
	tmp->next = 0;
	tmp->prev = 0;

	while(list) {
		tmp = list;
		list = list->next;
		safe_free(tmp);
	}
}

static void _ls_destroy(_ls_node_t* list) {
	_ls_clear(list);
	safe_free(list);
}

static int _ls_free_extra(void* data, void* extra) {
	int result = _OP_RESULT_DEL_NODE;
	mb_unrefvar(data);

	mb_assert(extra);

	safe_free(extra);

	return result;
}

/** Dictionary operations */

static unsigned _ht_hash_object(void* ht, void* d) {
	unsigned result = 0;
	_ht_node_t* self = (_ht_node_t*)ht;
	_object_t* o = (_object_t*)d;
	size_t i = 0;
	unsigned h = 0;
#ifdef MB_ENABLE_CLASS
	_object_t val;
#endif /* MB_ENABLE_CLASS */

	mb_assert(ht);

	h = o->type;
	switch(o->type) {
	case _DT_STRING:
		h = 5 * h + _ht_hash_string(ht, o->data.string);
		if(self->array_size == 1)
			result = 0;
		else
			result = h % self->array_size;

		break;
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		if(o->data.instance->hash) {
			mb_interpreter_t* s = o->data.instance->ref.s;
			_ls_node_t ast;
			_ls_node_t* tmp = &ast;
			mb_value_t va[1];
			mb_make_nil(va[0]);
			memset(&ast, 0, sizeof(_ls_node_t));
			if(_eval_routine(s, &tmp, va, 1, o->data.instance->hash, _has_routine_fun_arg, _pop_routine_fun_arg) == MB_FUNC_OK) {
				_MAKE_NIL(&val);
				_public_value_to_internal_object(&s->running_context->intermediate_value, &val);
				if(val.type != _DT_INT) {
					int ignored = MB_FUNC_OK;
					_handle_error_on_obj(s, SE_RN_INTEGER_EXPECTED, s->source_file, o, MB_FUNC_ERR, _exit, ignored);
				}

				o = &val;
			}
		}

		goto _default;
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_USERTYPE_REF
	case _DT_USERTYPE_REF:
		if(o->data.usertype_ref->hash) {
			if(self->array_size == 1) {
				result = 0;
			} else {
				h = 5 * h + o->data.usertype_ref->hash(o->data.usertype_ref->ref.s, o->data.usertype_ref->usertype);
				result = h % self->array_size;
			}

			break;
		}

		goto _default;
#endif /* MB_ENABLE_USERTYPE_REF */
	default:
#if defined MB_ENABLE_CLASS || defined MB_ENABLE_USERTYPE_REF
_default:
#endif /* MB_ENABLE_CLASS || MB_ENABLE_USERTYPE_REF */
		if(self->array_size == 1) {
			result = 0;
		} else {
			for(i = 0; i < sizeof(_raw_t); ++i)
				h = 5 * h + o->data.raw[i];
			result = h % self->array_size;
		}

		break;
	}

	goto _exit; /* Avoid an unreferenced label warning */

_exit:
	return result;
}

static unsigned _ht_hash_string(void* ht, void* d) {
	unsigned result = 0;
	_ht_node_t* self = (_ht_node_t*)ht;
	char* s = (char*)d;
	unsigned h = 0;

	mb_assert(ht);

	if(self->array_size == 1) {
		result = 0;
	} else {
		for( ; *s; ++s)
			h = 5 * h + *s;
		result = h % self->array_size;
	}

	return result;
}

static unsigned _ht_hash_intptr(void* ht, void* d) {
	unsigned result = 0;
	_ht_node_t* self = (_ht_node_t*)ht;

	mb_assert(ht);

	if(self->array_size == 1) {
		result = 0;
	} else {
		uintptr_t u = (uintptr_t)d;
		result = (unsigned)(u % self->array_size);
	}

	return result;
}

static unsigned _ht_hash_ref(void* ht, void* d) {
	unsigned result = 0;
	_ht_node_t* self = (_ht_node_t*)ht;
	_ref_t* ref = (_ref_t*)d;

	mb_assert(ht);

	if(self->array_size == 1) {
		result = 0;
	} else {
		uintptr_t u = (uintptr_t)ref;
		result = (unsigned)(u % self->array_size);
	}

	return result;
}

static int _ht_cmp_object(void* d1, void* d2) {
	_object_t* o1 = (_object_t*)d1;
	_object_t* o2 = (_object_t*)d2;
	int i = 0;
#ifdef MB_ENABLE_CLASS
	_routine_t* cmp = 0;
	_object_t val;
	bool_t fst = true;
#endif /* MB_ENABLE_CLASS */

	if(o1->type < o2->type)
		return -1;
	else if(o1->type > o2->type)
		return 1;

	switch(o1->type) {
	case _DT_STRING:
		return _ht_cmp_string(o1->data.string, o2->data.string);
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		if(o1->data.instance->compare) {
			cmp = o1->data.instance->compare;
			fst = true;
		} else if(o2->data.instance->compare) {
			cmp = o2->data.instance->compare;
			fst = false;
		}
		if(cmp) {
			mb_interpreter_t* s = o1->data.instance->ref.s;
			_ls_node_t ast;
			_ls_node_t* tmp = &ast;
			mb_value_t va[1];
			mb_make_nil(va[0]);
			_internal_object_to_public_value(fst ? o2 : o1, &va[0]);
			memset(&ast, 0, sizeof(_ls_node_t));
			if(_eval_routine(s, &tmp, va, 1, cmp, _has_routine_fun_arg, _pop_routine_fun_arg) == MB_FUNC_OK) {
				_MAKE_NIL(&val);
				_public_value_to_internal_object(&s->running_context->intermediate_value, &val);
				if(val.type != _DT_INT) {
					int ignored = MB_FUNC_OK;
					_handle_error_on_obj(s, SE_RN_INTEGER_EXPECTED, s->source_file, o1, MB_FUNC_ERR, _exit, ignored);
				}

				return (int)(fst ? val.data.integer : -val.data.integer);
			}
		}

		goto _default;
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_USERTYPE_REF
	case _DT_USERTYPE_REF:
		if(o1->data.usertype_ref->cmp)
			return o1->data.usertype_ref->cmp(o1->data.usertype_ref->ref.s, o1->data.usertype_ref->usertype, o2->data.usertype_ref->usertype);
		else if(o2->data.usertype_ref->cmp)
			return o2->data.usertype_ref->cmp(o1->data.usertype_ref->ref.s, o1->data.usertype_ref->usertype, o2->data.usertype_ref->usertype);

		goto _default;
#endif /* MB_ENABLE_USERTYPE_REF */
	default:
#if defined MB_ENABLE_CLASS || defined MB_ENABLE_USERTYPE_REF
_default:
#endif /* MB_ENABLE_CLASS || MB_ENABLE_USERTYPE_REF */
		if(mb_is_little_endian()) {
			for(i = (int)sizeof(_raw_t) - 1; i >= 0; --i) {
				if(o1->data.raw[i] < o2->data.raw[i])
					return -1;
				else if(o1->data.raw[i] > o2->data.raw[i])
					return 1;
			}
		} else {
			for(i = 0; i < (int)sizeof(_raw_t); ++i) {
				if(o1->data.raw[i] < o2->data.raw[i])
					return -1;
				else if(o1->data.raw[i] > o2->data.raw[i])
					return 1;
			}
		}

		break;
	}

	goto _exit; /* Avoid an unreferenced label warning */

_exit:
	return 0;
}

static int _ht_cmp_string(void* d1, void* d2) {
	char* s1 = (char*)d1;
	char* s2 = (char*)d2;

	return strcmp(s1, s2);
}

static int _ht_cmp_intptr(void* d1, void* d2) {
	intptr_t i1 = (intptr_t)d1;
	intptr_t i2 = (intptr_t)d2;

	if(i1 < i2)
		return -1;
	else if(i1 > i2)
		return 1;

	return 0;
}

static int _ht_cmp_ref(void* d1, void* d2) {
	_ref_t* r1 = (_ref_t*)d1;
	_ref_t* r2 = (_ref_t*)d2;
	intptr_t i = (intptr_t)r1 - (intptr_t)r2;

	if(i < 0)
		return -1;
	else if(i > 0)
		return 1;

	return 0;
}

static _ht_node_t* _ht_create(unsigned size, _ht_compare_t cmp, _ht_hash_t hs, _ls_operation_t freeextra) {
	const unsigned array_size = size ? size : _HT_ARRAY_SIZE_DEFAULT;
	_ht_node_t* result = 0;
	unsigned ul = 0;

	if(!cmp)
		cmp = _ht_cmp_intptr;
	if(!hs)
		hs = _ht_hash_intptr;

	result = (_ht_node_t*)mb_malloc(sizeof(_ht_node_t));
	result->free_extra = freeextra;
	result->compare = cmp;
	result->hash = hs;
	result->array_size = array_size;
	result->count = 0;
#if _LAZY_HASH_TABLE
	mb_unrefvar(ul);

	result->array = 0;
#else /* _LAZY_HASH_TABLE */
	result->array = (_ls_node_t**)mb_malloc(sizeof(_ls_node_t*) * result->array_size);
	for(ul = 0; ul < result->array_size; ++ul)
		result->array[ul] = _ls_create();
#endif /* _LAZY_HASH_TABLE */

	return result;
}

static _ls_node_t* _ht_find(_ht_node_t* ht, void* key) {
	_ls_node_t* result = 0;
	_ls_node_t* bucket = 0;
	unsigned hash_code = 0;

	mb_assert(ht && key);

	hash_code = ht->hash(ht, key);
	if(ht->array && ht->array[hash_code]) {
		bucket = ht->array[hash_code];
		bucket = bucket->next;
	}
	while(bucket) {
		if(ht->compare(bucket->extra, key) == 0) {
			result = bucket;

			break;
		}
		bucket = bucket->next;
	}

	return result;
}

static unsigned _ht_set_or_insert(_ht_node_t* ht, void* key, void* value) {
	unsigned result = 0;
	_ls_node_t* bucket = 0;
	unsigned hash_code = 0;
	unsigned ul = 0;

	mb_assert(ht && key);

	bucket = _ht_find(ht, key);
	if(bucket) { /* Update */
		bucket->data = value;
		++result;
	} else { /* Insert */
		hash_code = ht->hash(ht, key);
		if(!ht->array) {
			ht->array = (_ls_node_t**)mb_malloc(sizeof(_ls_node_t*) * ht->array_size);
			for(ul = 0; ul < ht->array_size; ++ul)
				ht->array[ul] = 0;
		}
		if(!ht->array[hash_code])
			ht->array[hash_code] = _ls_create();
		bucket = ht->array[hash_code];
		bucket = _ls_pushback(bucket, value);
		mb_assert(bucket);
		bucket->extra = key;
		++ht->count;
		++result;
	}

	return result;
}

static unsigned _ht_remove(_ht_node_t* ht, void* key, _ls_compare_t cmp) {
	unsigned result = 0;
	unsigned hash_code = 0;
	_ls_node_t* bucket = 0;

	mb_assert(ht && key);

	if(!cmp)
		cmp = _ls_cmp_extra;

	bucket = _ht_find(ht, key);
	hash_code = ht->hash(ht, key);
	if(ht->array && ht->array[hash_code]) {
		bucket = ht->array[hash_code];
		result = _ls_try_remove(bucket, key, cmp, ht->free_extra);
		ht->count -= result;
	}

	return result;
}

static unsigned _ht_foreach(_ht_node_t* ht, _ht_operation_t op) {
	unsigned result = 0;
	_ls_node_t* bucket = 0;
	unsigned ul = 0;

	if(ht->array) {
		for(ul = 0; ul < ht->array_size; ++ul) {
			bucket = ht->array[ul];
			if(bucket)
				result += _ls_foreach(bucket, op);
		}
	}

	return result;
}

static unsigned _ht_count(_ht_node_t* ht) {
	mb_assert(ht);

	return ht->count;
}

static void _ht_clear(_ht_node_t* ht) {
	unsigned ul = 0;

	mb_assert(ht);

	if(ht->array) {
		for(ul = 0; ul < ht->array_size; ++ul) {
			if(ht->array[ul])
				_ls_clear(ht->array[ul]);
		}
		ht->count = 0;
	}
}

static void _ht_destroy(_ht_node_t* ht) {
	unsigned ul = 0;

	mb_assert(ht);

	if(ht->array) {
		if(ht->free_extra)
			_ht_foreach(ht, ht->free_extra);

		for(ul = 0; ul < ht->array_size; ++ul) {
			if(ht->array[ul])
				_ls_destroy(ht->array[ul]);
		}

		safe_free(ht->array);
	}
	safe_free(ht);
}

static int _ht_remove_existing(void* data, void* extra, _ht_node_t* ht) {
	int result = _OP_RESULT_NORMAL;
	mb_unrefvar(data);

	if(_ht_find(ht, extra))
		_ht_remove(ht, extra, 0);

	return result;
}

/** Memory manipulations */

/* Initialize a chunk of resizable dynamic buffer */
static void _init_dynamic_buffer(_dynamic_buffer_t* buf) {
	mb_assert(buf);

	memset(buf->bytes, 0, sizeof(buf->bytes));
	buf->pointer.charp = buf->bytes;
	buf->size = sizeof(buf->bytes);
}

/* Dispose a chunk of resizable dynamic buffer */
static void _dispose_dynamic_buffer(_dynamic_buffer_t* buf) {
	mb_assert(buf);

	if(buf->pointer.charp != buf->bytes) {
		safe_free(buf->pointer.charp);
	}
	buf->pointer.charp = 0;
	buf->size = 0;
}

/* Get the element count of a chunk of resizable dynamic buffer */
static size_t _countof_dynamic_buffer(_dynamic_buffer_t* buf, size_t es) {
	mb_assert(buf);

	return buf->size / es;
}

/* Resize a chunk of resizable dynamic buffer */
static void _resize_dynamic_buffer(_dynamic_buffer_t* buf, size_t es, size_t c) {
	size_t as = es * c;

	mb_assert(buf);

	if(as > buf->size) {
		if(buf->pointer.charp != buf->bytes) {
			safe_free(buf->pointer.charp);
		}
		buf->pointer.charp = (char*)mb_malloc(as);
		buf->size = as;
	}
	}


void * reusable_buffer = 0;
int reusable_buffer_size = 0;

/* Allocate a chunk of memory with a specific size */
static void* mb_malloc(size_t s) {
	char* ret = 0;
	size_t rs = s;

#ifdef MB_ENABLE_ALLOC_STAT

	if (s<=reusable_buffer_size)
	{
		reusable_buffer_size = 0;
		return reusable_buffer;
	}
	if(!_MB_CHECK_MEM_TAG_SIZE(size_t, s))
		return 0;
	rs += _MB_MEM_TAG_SIZE;
#endif /* MB_ENABLE_ALLOC_STAT */
	if(_mb_allocate_func)
		ret = _mb_allocate_func((unsigned)rs);
	else
		ret = (char*)malloc(rs);
	mb_assert(ret);
#ifdef MB_ENABLE_ALLOC_STAT
	_mb_allocated += s;
	ret += _MB_MEM_TAG_SIZE;
	_MB_WRITE_MEM_TAG_SIZE(ret, s);
#endif /* MB_ENABLE_ALLOC_STAT */

	return (void*)ret;
}

/* Free a chunk of memory */
static void mb_free(void* p) {
	mb_assert(p);

#ifdef MB_ENABLE_ALLOC_STAT
	do {
		size_t os = _MB_READ_MEM_TAG_SIZE(p);
		
		//Heuristic, it's smallish objects that are going
		//to be most often deallocated and replaced with one of similar size.
		if(os<256)
		{
			//If there's already a reusable_buffer smaller than
			//This one, free it.
			//Don't just leave it and free this one, because
			//Then a tiny reusable_buffer might never get used/
			//and would just sit there.

			//If the reusable_buffer is bigger than us,

			if(reusable_buffer_size ==0)
			{
				reusable_buffer = p;
				reusable_buffer_size = os;
				return;			
			}
			//This buffer is bigger than the old one,
			//free the old one because
			//This one is more likely to be used in the future.
			if(os> reusable_buffer_size)
			{
				_mb_allocated -= reusable_buffer_size;
		        void * tofree = (char*)reusable_buffer - _MB_MEM_TAG_SIZE;
				if(_mb_free_func)
					_mb_free_func((char*)tofree);
				else
					free(_mb_free_func);
				reusable_buffer = p;
				reusable_buffer_size = os;
				return;
			}
		}
		_mb_allocated -= os;
		p = (char*)p - _MB_MEM_TAG_SIZE;
	} while(0);
#endif /* MB_ENABLE_ALLOC_STAT */
	if(_mb_free_func)
		_mb_free_func((char*)p);
	else
		free(p);
}

/* Compare two chunks of memory */
static int mb_memcmp(void* l, void* r, size_t s) {
	unsigned char* lc = (unsigned char*)l;
	unsigned char* rc = (unsigned char*)r;
	int i = 0;

	if(mb_is_little_endian()) {
		for(i = (int)s - 1; i >= 0; i--) {
			if(lc[i] < rc[i])
				return -1;
			else if(lc[i] > rc[i])
				return 1;
		}
	} else {
		for(i = 0; i < (int)s; i++) {
			if(lc[i] < rc[i])
				return -1;
			else if(lc[i] > rc[i])
				return 1;
		}
	}

	return 0;
}

/* Detect whether a chunk of memory contains any non-zero byte */
static size_t mb_memtest(void* p, size_t s) {
	size_t result = 0;
	size_t i = 0;

	for(i = 0; i < s; i++)
		result += ((unsigned char*)p)[i];

	return result;
}

/* Duplicate a string */
static char* mb_strdup(const char* p, size_t s) {
#ifdef MB_ENABLE_ALLOC_STAT
	if(!s) {
		s = _MB_READ_MEM_TAG_SIZE(p);
	}

	return mb_memdup(p, (unsigned)s);
#else /* MB_ENABLE_ALLOC_STAT */
	if(s)
		return mb_memdup(p, (unsigned)s);

	return mb_memdup(p, (unsigned)(strlen(p) + 1));
#endif /* MB_ENABLE_ALLOC_STAT */
}

/* Change a string to upper case */
static char* mb_strupr(char* s) {
	char* t = s;

	while(*s) {
		*s = toupper(*s);
		++s;
	}

	return t;
}

/* Determine whether it's running on a little endian platform */
static bool_t mb_is_little_endian(void) {
	int i = 1;

	return ((char*)&i)[0] == 1;
}

/** Unicode handling */

#if defined MB_CP_VC && defined MB_ENABLE_UNICODE
/* Map a UTF8 character string to a UTF16 (wide character) string */
static int mb_bytes_to_wchar(const char* sz, wchar_t** out, size_t size) {
	int result = MultiByteToWideChar(CP_UTF8, 0, sz, -1, 0, 0);
	if((int)size >= result)
		MultiByteToWideChar(CP_UTF8, 0, sz, -1, *out, result);

	return result;
}

/* Map an ANSI character string to a UTF16 (wide character) string */
static int mb_bytes_to_wchar_ansi(const char* sz, wchar_t** out, size_t size) {
	int result = MultiByteToWideChar(CP_ACP, 0, sz, -1, 0, 0);
	if((int)size >= result)
		MultiByteToWideChar(CP_ACP, 0, sz, -1, *out, result);

	return result;
}

/* Map a UTF16 (wide character) string to a UTF8 character string */
static int mb_wchar_to_bytes(const wchar_t* sz, char** out, size_t size) {
	int result = WideCharToMultiByte(CP_UTF8, 0, sz, -1, 0, 0, 0, 0);
	if((int)size >= result)
		WideCharToMultiByte(CP_UTF8, 0, sz, -1, *out, result, 0, 0);

	return result;
}
#endif /* MB_CP_VC && MB_ENABLE_UNICODE */

/* Determine whether a string begins with a BOM, and ignore it */
static int mb_uu_getbom(const char** ch) {
#ifdef __cplusplus
	signed char** ptr = (signed char**)ch;
#else /* __cplusplus */
	char** ptr = (char**)ch;
#endif /* __cplusplus */

	if(!ptr && !(*ptr))
		return 0;

	if((*ptr)[0] == -17 && (*ptr)[1] == -69 && (*ptr)[2] == -65) {
		*ptr += 3;

		return 3;
	} else if((*ptr)[0] == -2 && (*ptr)[1] == -1) {
		*ptr += 2;

		return 2;
	}

	return 0;
}

#ifdef MB_ENABLE_UNICODE
/* Determine whether a buffer starts with a UTF8 encoded character, and return taken byte count */
static int mb_uu_ischar(const char* ch) {
	/* Copyright 2008, 2009 Bjoern Hoehrmann, http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ */
#define _TAKE(__ch, __c, __r) do { __c = *__ch++; __r++; } while(0)
#define _COPY(__ch, __c, __r, __cp) do { _TAKE(__ch, __c, __r); __cp = (__cp << 6) | ((unsigned char)__c & 0x3Fu); } while(0)
#define _TRANS(__m, __cp, __g) do { __cp &= ((__g[(unsigned char)c] & __m) != 0); } while(0)
#define _TAIL(__ch, __c, __r, __cp, __g) do { _COPY(__ch, __c, __r, __cp); _TRANS(0x70, __cp, __g); } while(0)

	MBCONST static const unsigned char range[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
		0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
		8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		10, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 11, 6, 6, 6, 5, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	};

	int result = 0;
	unsigned codepoint = 0;
	unsigned char type = 0;
	char c = 0;

	if(!ch)
		return 0;

	_TAKE(ch, c, result);
	if(!(c & 0x80)) {
		codepoint = (unsigned char)c;

		return 1;
	}

	type = range[(unsigned char)c];
	codepoint = (0xFF >> type) & (unsigned char)c;

	switch(type) {
	case 2: _TAIL(ch, c, result, codepoint, range); return result;
	case 3: _TAIL(ch, c, result, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	case 4: _COPY(ch, c, result, codepoint); _TRANS(0x50, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	case 5: _COPY(ch, c, result, codepoint); _TRANS(0x10, codepoint, range); _TAIL(ch, c, result, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	case 6: _TAIL(ch, c, result, codepoint, range); _TAIL(ch, c, result, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	case 10: _COPY(ch, c, result, codepoint); _TRANS(0x20, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	case 11: _COPY(ch, c, result, codepoint); _TRANS(0x60, codepoint, range); _TAIL(ch, c, result, codepoint, range); _TAIL(ch, c, result, codepoint, range); return result;
	default: return 0;
	}

#undef _TAKE
#undef _COPY
#undef _TRANS
#undef _TAIL
}

/* Tell how many UTF8 character are there in a string */
static int mb_uu_strlen(const char* ch) {
	int result = 0;

	if(!ch)
		return 0;

	while(*ch) {
		int t = mb_uu_ischar(ch);
		if(t <= 0)
			return t;
		ch += t;
		result++;
	}

	return result;
}

/* Retrieve a sub string of a UTF8 string */
static int mb_uu_substr(const char* ch, int begin, int count, char** o) {
	int cnt = 0;
	const char* b = 0;
	const char* e = 0;
	int l = 0;

	if(!ch || begin < 0 || count <= 0 || !o)
		return -1;

	while(*ch) {
		int t = mb_uu_ischar(ch);
		if(t <= 0)
			return t;
		if(cnt == begin) {
			b = ch;

			break;
		}
		ch += t;
		cnt++;
	}

	while(*ch) {
		int t = mb_uu_ischar(ch);
		if(t <= 0)
			return t;
		if(cnt == begin + count) {
			e = ch;

			break;
		}
		ch += t;
		e = ch;
		cnt++;
	}

	if(!(*ch) && (cnt != begin + count))
		return -1;

	l = (int)(e - b);
	*o = (char*)mb_malloc(l + 1);
	memcpy(*o, b, l);
	(*o)[l] = _ZERO_CHAR;

	return l;
}
#endif /* MB_ENABLE_UNICODE */

/** Expression processing */

/* Determine whether a function is an operator */
static bool_t _is_operator(mb_func_t op) {
	return (
		(op == _core_dummy_assign) ||
		(op == _core_add) ||
		(op == _core_min) ||
		(op == _core_mul) ||
		(op == _core_div) ||
		(op == _core_mod) ||
		(op == _core_pow) ||
		(op == _core_open_bracket) ||
		(op == _core_close_bracket) ||
		(op == _core_equal) ||
		(op == _core_less) ||
		(op == _core_greater) ||
		(op == _core_less_equal) ||
		(op == _core_greater_equal) ||
		(op == _core_not_equal) ||
		(op == _core_and) ||
		(op == _core_or) ||
		(op == _core_is)
	);
}

/* Determine whether a function is for flow control */
static bool_t _is_flow(mb_func_t op) {
	return (
		(op == _core_if) ||
		(op == _core_then) ||
		(op == _core_elseif) ||
		(op == _core_else) ||
		(op == _core_endif) ||
		(op == _core_for) ||
		(op == _core_to) ||
		(op == _core_step) ||
		(op == _core_next) ||
		(op == _core_while) ||
		(op == _core_wend) ||
		(op == _core_do) ||
		(op == _core_until) ||
		(op == _core_exit) ||
		(op == _core_goto) ||
		(op == _core_gosub) ||
		(op == _core_return) ||
		(op == _core_end)
	);
}

/* Determine whether a function is unary */
static bool_t _is_unary(mb_func_t op) {
	return (op == _core_neg) || (op == _core_not);
}

/* Determine whether a function is binary */
static bool_t _is_binary(mb_func_t op) {
	return (
		(op == _core_add) ||
		(op == _core_min) ||
		(op == _core_mul) ||
		(op == _core_div) ||
		(op == _core_mod) ||
		(op == _core_pow) ||
		(op == _core_equal) ||
		(op == _core_less) ||
		(op == _core_greater) ||
		(op == _core_less_equal) ||
		(op == _core_greater_equal) ||
		(op == _core_not_equal) ||
		(op == _core_and) ||
		(op == _core_or) ||
		(op == _core_is)
	);
}

/* Get the priority of two operators */
static char _get_priority(mb_func_t op1, mb_func_t op2) {
	char result = _ZERO_CHAR;
	int idx1 = 0;
	int idx2 = 0;

	mb_assert(op1 && op2);

	idx1 = _get_priority_index(op1);
	idx2 = _get_priority_index(op2);
	mb_assert(idx1 < countof(_PRECEDE_TABLE) && idx2 < countof(_PRECEDE_TABLE[0]));
	result = _PRECEDE_TABLE[idx1][idx2];

	return result;
}

/* Get the index of an operator in the priority table */
static int _get_priority_index(mb_func_t op) {
	int i = 0;

	MBCONST mb_func_t funcs[] = {
		_core_add,
		_core_min,
		_core_mul,
		_core_div,
		_core_mod,
		_core_pow,
		_core_open_bracket,
		_core_close_bracket,
		_core_dummy_assign,
		_core_greater,
		_core_less,
		_core_greater_equal,
		_core_less_equal,
		_core_equal,
		_core_not_equal,
		_core_and,
		_core_or,
		_core_not,
		_core_neg,
		_core_is
	};

	mb_assert(op);

	for(i = 0; i < countof(funcs); i++) {
		if(op == funcs[i])
			return i;
	}

	mb_assert(0 && "Unknown operator.");

	return -1;
}

/* Operate two operands */
static _object_t* _operate_operand(mb_interpreter_t* s, _object_t* optr, _object_t* opnd1, _object_t* opnd2, int* status) {
	_object_t* result = 0;
	_tuple3_t tp;
	_tuple3_t* tpptr = 0;
	int _status = 0;

	mb_assert(s && optr);
	mb_assert(optr->type == _DT_FUNC);

	if(!opnd1)
		return result;

	result = _create_object();

	memset(&tp, 0, sizeof(_tuple3_t));
	tp.e1 = opnd1;
	tp.e2 = opnd2;
	tp.e3 = result;
	tpptr = &tp;

	_status = (optr->data.func->pointer)(s, (void**)&tpptr);
	if(status)
		*status = _status;
	if(_status != MB_FUNC_OK) {
		if(_status != MB_FUNC_WARNING) {
			safe_free(result);
		}
		if(_set_current_error(s, SE_RN_OPERATION_FAILED, 0)) {
#ifdef MB_ENABLE_SOURCE_TRACE
			_set_error_pos(s, optr->source_pos, optr->source_row, optr->source_col);
#else /* MB_ENABLE_SOURCE_TRACE */
			_set_error_pos(s, 0, 0, 0);
#endif /* MB_ENABLE_SOURCE_TRACE */
		}
	}

	return result;
}

/* Determine whether an object is an expression termination */
static bool_t _is_expression_terminal(mb_interpreter_t* s, _object_t* obj) {
	bool_t result = false;

	mb_assert(s && obj);

	result = (
		(obj->type == _DT_EOS) ||
		(obj->type == _DT_SEP) ||
		(obj->type == _DT_FUNC &&
			(obj->data.func->pointer == _core_then ||
			obj->data.func->pointer == _core_elseif ||
			obj->data.func->pointer == _core_else ||
			obj->data.func->pointer == _core_endif ||
			obj->data.func->pointer == _core_to ||
			obj->data.func->pointer == _core_step))
	);

	return result;
}

/* Determine whether an object is an unexpected calculation result */
static bool_t _is_unexpected_calc_type(mb_interpreter_t* s, _object_t* obj) {
	mb_assert(s);

	return !obj || (
		(obj->type == _DT_FUNC) ||
		(obj->type == _DT_LABEL) ||
#ifdef MB_ENABLE_LAMBDA
		(obj->type == _DT_OUTER_SCOPE) ||
#endif /* MB_ENABLE_LAMBDA */
		(obj->type == _DT_SEP) ||
#ifdef MB_ENABLE_SOURCE_TRACE
		(obj->type == _DT_PREV_IMPORT) ||
		(obj->type == _DT_POST_IMPORT) ||
#endif /* MB_ENABLE_SOURCE_TRACE */
		(obj->type == _DT_EOS)
	);
}

/* Determine whether an object is a referenced calculation result */
static bool_t _is_referenced_calc_type(mb_interpreter_t* s, _object_t* obj) {
	mb_assert(s && obj);

	return (
#ifdef MB_ENABLE_USERTYPE_REF
		(obj->type == _DT_USERTYPE_REF) ||
#endif /* MB_ENABLE_USERTYPE_REF */
#ifdef MB_ENABLE_COLLECTION_LIB
		(obj->type == _DT_LIST) || (obj->type == _DT_DICT) || (obj->type == _DT_LIST_IT) || (obj->type == _DT_DICT_IT) ||
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		(obj->type == _DT_CLASS) ||
#endif /* MB_ENABLE_CLASS */
		(obj->type == _DT_ARRAY) ||
		(obj->type == _DT_ROUTINE)
	);
}

/* Calculate an expression */
static int _calc_expression(mb_interpreter_t* s, _ls_node_t** l, _object_t** val) {
	int result = 0;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;
	_ls_node_t* garbage = 0;
	_ls_node_t* optr = 0;
	_ls_node_t* opnd = 0;
	_object_t* c = 0;
	_object_t* x = 0;
	_object_t* a = 0;
	_object_t* b = 0;
	_object_t* r = 0;
	_object_t* theta = 0;
	char pri = _ZERO_CHAR;
	int* inep = 0;
	int f = 0;

	_object_t* guard_val = 0;
	int bracket_count = 0;
	bool_t hack = false;
	_ls_node_t* errn = 0;
	bool_t gce = true;

	mb_assert(s && l);

	gce = mb_get_gc_enabled(s);
	mb_set_gc_enabled(s, false);

	running = s->running_context;
	ast = *l;

	c = (_object_t*)ast->data;
#ifdef MB_PREFER_SPEED
	if(c->is_const) {
		ast = ast->next;

		goto _fast;
	}
#endif /* MB_PREFER_SPEED */

	optr = _ls_create();
	opnd = _ls_create();

#define _LAZY_INIT_GLIST do { if(!garbage) garbage = _ls_create(); } while(0)

	inep = (int*)mb_malloc(sizeof(int));
	*inep = 0;
	_ls_pushback(s->in_neg_expr, inep);

	do {
		if(c->type == _DT_STRING) {
			if(ast->next) {
				_object_t* _fsn = (_object_t*)ast->next->data;
				if(_IS_FUNC(_fsn, _core_add) || _IS_FUNC(_fsn, _core_equal) || _IS_FUNC(_fsn, _core_not_equal) || _IS_FUNC(_fsn, _core_and) || _IS_FUNC(_fsn, _core_or) || _IS_FUNC(_fsn, _core_is))
					break;
			}

			(*val)->type = _DT_STRING;
			(*val)->data.string = c->data.string;
			(*val)->is_ref = true;
			ast = ast->next;

			goto _exit;
		}
	} while(0);
	guard_val = c;
	ast = ast->next;
	_ls_pushback(optr, _exp_assign);
	while(
		!(c->type == _DT_FUNC && strcmp(c->data.func->name, _DUMMY_ASSIGN_CHAR) == 0) ||
		!(((_object_t*)(_ls_back(optr)->data))->type == _DT_FUNC && strcmp(((_object_t*)(_ls_back(optr)->data))->data.func->name, _DUMMY_ASSIGN_CHAR) == 0)) {
		if(!hack) {
			if(_IS_FUNC(c, _core_open_bracket)) {
				++bracket_count;
			} else if(_IS_FUNC(c, _core_close_bracket)) {
				--bracket_count;
				if(bracket_count < 0) {
					c = _exp_assign;
					ast = ast->prev;

					continue;
				}
			}
		}
		hack = false;
		if(!(c->type == _DT_FUNC && _is_operator(c->data.func->pointer))) {
			if(_is_expression_terminal(s, c)) {
				c = _exp_assign;
				if(ast)
					ast = ast->prev;
				if(bracket_count) {
					_object_t _cb;
					_func_t _cbf;
					_MAKE_NIL(&_cb);
					_cb.type = _DT_FUNC;
					_cb.data.func = &_cbf;
					_cb.data.func->name = ")";
					_cb.data.func->pointer = _core_close_bracket;
					while(bracket_count) {
						_ls_pushback(optr, &_cb);
						bracket_count--;
						f = 0;
					}
					errn = ast;
				}
			} else {
				if(c->type == _DT_ARRAY) {
#ifdef MB_ENABLE_CLASS
					if(s->last_instance) {
						_ls_node_t* cs = _search_identifier_in_scope_chain(s, 0, c->data.array->name, _PATHING_NORMAL, 0, 0);
						if(cs)
							c = (_object_t*)cs->data;
					}
#endif /* MB_ENABLE_CLASS */
_array:
					if(ast && !_IS_FUNC(((_object_t*)ast->data), _core_open_bracket)) {
						_ls_pushback(opnd, c);
						f++;
					} else {
						unsigned arr_idx = 0;
						mb_value_u arr_val;
						_data_e arr_type;
						_object_t* arr_elem = 0;
						ast = ast->prev;
						result = _get_array_index(s, &ast, c, &arr_idx, 0);
						if(result != MB_FUNC_OK) {
							_handle_error_on_obj(s, SE_RN_CALCULATION_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
						}
						ast = ast->next;
						_get_array_elem(s, c->data.array, arr_idx, &arr_val, &arr_type);
						arr_elem = _create_object();
						_LAZY_INIT_GLIST;
						_ls_pushback(garbage, arr_elem);
						arr_elem->type = arr_type;
						arr_elem->is_ref = true;
						_copy_bytes(arr_elem->data.bytes, arr_val.bytes);
						if(f) {
							_handle_error_on_obj(s, SE_RN_OPERATOR_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
						}
						_ls_pushback(opnd, arr_elem);
						f++;
					}
				} else if(c->type == _DT_FUNC) {
					ast = ast->prev;
					if(_IS_UNARY_FUNC(c)) {
#ifdef MB_ENABLE_STACK_TRACE
						_ls_pushback(s->stack_frames, c->data.func->name);
#endif /* MB_ENABLE_STACK_TRACE */
						result = (c->data.func->pointer)(s, (void**)&ast);
#ifdef MB_ENABLE_STACK_TRACE
						_ls_popback(s->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */
					} else {
						int calc_depth = running->calc_depth;
						running->calc_depth = _INFINITY_CALC_DEPTH;
#ifdef MB_ENABLE_STACK_TRACE
						_ls_pushback(s->stack_frames, c->data.func->name);
#endif /* MB_ENABLE_STACK_TRACE */
						result = (c->data.func->pointer)(s, (void**)&ast);
#ifdef MB_ENABLE_STACK_TRACE
						_ls_popback(s->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */
						running->calc_depth = calc_depth;
					}
					if(result != MB_FUNC_OK) {
						_handle_error_on_obj(s, SE_RN_CALCULATION_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
					}
					c = _create_object();
					_LAZY_INIT_GLIST;
					_ls_pushback(garbage, c);
					result = _public_value_to_internal_object(&running->intermediate_value, c);
					switch(c->type) {
					case _DT_ROUTINE:
						if(c->data.routine->type != MB_RT_SCRIPT)
							break;
						mb_make_nil(running->intermediate_value);
						/* Fall through */
					case _DT_STRING:
						c->is_ref = true;
					default: /* Do nothing */
						break;
					}
					if(result != MB_FUNC_OK)
						goto _error;
					if(f) {
						_handle_error_on_obj(s, SE_RN_OPERATOR_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
					}
					if(_is_array(c)) {
						goto _array;
					} else {
						if(ast && _IS_FUNC(ast->data, _core_open_bracket)) {
							_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
						}
					}
					_ls_pushback(opnd, c);
					f++;
				} else if(c->type == _DT_ROUTINE) {
_routine:
					do {
#ifdef MB_ENABLE_CLASS
						bool_t calling = false;
						_object_t* obj = 0;
						_ls_node_t* fn = 0;
#endif /* MB_ENABLE_CLASS */
						ast = ast->prev;
#ifdef MB_ENABLE_CLASS
						calling = s->calling;
						s->calling = false;
#endif /* MB_ENABLE_CLASS */
						result = _eval_routine(s, &ast, 0, 0, c->data.routine, _has_routine_lex_arg, _pop_routine_lex_arg);
#ifdef MB_ENABLE_CLASS
						s->calling = calling;
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_CLASS
						obj = ast ? (_object_t*)ast->data : 0;
						if(_IS_VAR(obj) && _is_valid_class_accessor_following_routine(s, obj->data.variable, ast, &fn)) {
							if(fn) {
								if(ast) ast = ast->next;
								obj = (_object_t*)fn->data;
								if(_IS_VAR(obj)) {
									c = obj;

									goto _var;
								}
							}
						}
#endif /* MB_ENABLE_CLASS */
					} while(0);
					if(ast)
						ast = ast->prev;
					if(result == MB_FUNC_END)
						goto _error;
					if(result != MB_FUNC_OK) {
						_handle_error_on_obj(s, SE_RN_CALCULATION_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
					}
					c = _create_object();
					_LAZY_INIT_GLIST;
					_ls_pushback(garbage, c);
					result = _public_value_to_internal_object(&running->intermediate_value, c);
					if(result != MB_FUNC_OK)
						goto _error;
					if(f) {
						_handle_error_on_obj(s, SE_RN_OPERATOR_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
					}
					_ls_pushback(opnd, c);
					f++;
				} else if(c->type == _DT_VAR && c->data.variable->data->type == _DT_ARRAY) {
					unsigned arr_idx = 0;
					mb_value_u arr_val;
					_data_e arr_type;
					_object_t* arr_elem = 0;

					if(ast && !_IS_FUNC(((_object_t*)ast->data), _core_open_bracket)) {
						c = c->data.variable->data;
						_ls_pushback(opnd, c);
						f++;
					} else {
						ast = ast->prev;
						result = _get_array_index(s, &ast, 0, &arr_idx, 0);
						if(result != MB_FUNC_OK) {
							_handle_error_on_obj(s, SE_RN_CALCULATION_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
						}
						ast = ast->next;
						_get_array_elem(s, c->data.variable->data->data.array, arr_idx, &arr_val, &arr_type);
						arr_elem = _create_object();
						_LAZY_INIT_GLIST;
						_ls_pushback(garbage, arr_elem);
						arr_elem->type = arr_type;
						arr_elem->is_ref = true;
						if(arr_type == _DT_INT) {
							arr_elem->data.integer = arr_val.integer;
						} else if(arr_type == _DT_REAL) {
							arr_elem->data.float_point = arr_val.float_point;
						} else if(arr_type == _DT_STRING) {
							arr_elem->data.string = arr_val.string;
						} else if(arr_type == _DT_USERTYPE) {
							arr_elem->data.usertype = arr_val.usertype;
						} else {
#ifdef MB_SIMPLE_ARRAY
							mb_assert(0 && "Unsupported.");
#else /* MB_SIMPLE_ARRAY */
							_copy_bytes(arr_elem->data.bytes, arr_val.bytes);
#endif /* MB_SIMPLE_ARRAY */
						}
						if(f) {
							_handle_error_on_obj(s, SE_RN_OPERATOR_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
						}
						_ls_pushback(opnd, arr_elem);
						f++;
					}
				} else {
					if(c->type == _DT_VAR) {
						do {
							_ls_node_t* cs = _search_identifier_in_scope_chain(s, 0, c->data.variable->name,
#ifdef MB_ENABLE_CLASS
								_PU(c->data.variable->pathing),
#else /* MB_ENABLE_CLASS */
								0,
#endif /* MB_ENABLE_CLASS */
								0,
								0
							);
							if(cs) {
#ifdef MB_ENABLE_USERTYPE_REF
								_ls_node_t* fn = ast;
								if(fn) fn = fn->prev;
								if(_try_call_func_on_usertype_ref(s, &fn, c, cs, 0)) {
									ast = fn;
									c = _create_object();
									_LAZY_INIT_GLIST;
									_ls_pushback(garbage, c);
									_public_value_to_internal_object(&running->intermediate_value, c);
									_REF(c)
								} else {
#else /* MB_ENABLE_USERTYPE_REF */
								{
#endif /* MB_ENABLE_USERTYPE_REF */
									c = (_object_t*)cs->data;
									if(c && c->type == _DT_VAR && c->data.variable->data->type == _DT_ROUTINE) {
										c = c->data.variable->data;
									}
									if(ast && ast && _IS_FUNC(ast->data, _core_open_bracket)) {
										if(c && c->type == _DT_ROUTINE)
											goto _routine;
									}
								}
							}
						} while(0);
#ifdef MB_ENABLE_CLASS
_var:
#endif /* MB_ENABLE_CLASS */
						if(ast) {
							_object_t* _err_or_bracket = (_object_t*)ast->data;
							do {
#ifdef MB_ENABLE_COLLECTION_LIB
								if(_IS_VAR(c) && _IS_COLL(c->data.variable->data)) {
									if(_IS_FUNC(_err_or_bracket, _core_open_bracket)) {
										int_t idx = 0;
										mb_value_t key;
										mb_value_t ret;
										_object_t* ocoll = c->data.variable->data;

										mb_make_nil(ret);

										*l = ast->prev;

										_mb_check_exit(mb_attempt_open_bracket(s, (void**)l), _error);

										switch(ocoll->type) {
										case _DT_LIST:
											_mb_check_exit(mb_pop_int(s, (void**)l, &idx), _error);
											if(!_at_list(ocoll->data.list, idx, &ret)) {
												_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, TON(l), MB_FUNC_ERR, _error, result);
											}

											break;
										case _DT_DICT:
											mb_make_nil(key);
											_mb_check_exit(mb_pop_value(s, (void**)l, &key), _error);
											if(!_find_dict(ocoll->data.dict, &key, &ret)) {
												_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, TON(l), MB_FUNC_ERR, _error, result);
											}

											break;
										default: /* Do nothing */
											break;
										}

										_mb_check_exit(mb_attempt_close_bracket(s, (void**)l), _error);

										c = _create_object();
										_LAZY_INIT_GLIST;
										_ls_pushback(garbage, c);
										_public_value_to_internal_object(&ret, c);

										ast = *l;
									}

									break;
								}
#endif /* MB_ENABLE_COLLECTION_LIB */
								if(_IS_FUNC(_err_or_bracket, _core_open_bracket)) {
									_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
								}
							} while(0);
						}
					}
					if(f) {
						_handle_error_on_obj(s, SE_RN_OPERATOR_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
					}
					_ls_pushback(opnd, c);
					f++;
				}
				if(running->calc_depth != _INFINITY_CALC_DEPTH)
					running->calc_depth--;
				if(ast && (running->calc_depth == _INFINITY_CALC_DEPTH || running->calc_depth)) {
					c = (_object_t*)ast->data;
					if(c->type == _DT_FUNC && !_is_operator(c->data.func->pointer) && !_is_flow(c->data.func->pointer)) {
						_ls_foreach(opnd, _remove_source_object);

						_handle_error_on_obj(s, SE_RN_COLON_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
					}
					ast = ast->next;
				} else {
					c = _exp_assign;
				}
			}
		} else {
			pri = _get_priority(((_object_t*)(_ls_back(optr)->data))->data.func->pointer, c->data.func->pointer);
			switch(pri) {
			case '<':
				_ls_pushback(optr, c);
				c = (_object_t*)ast->data;
				ast = ast->next;
				f = 0;

				break;
			case '=':
				x = (_object_t*)_ls_popback(optr);
				c = (_object_t*)ast->data;
				ast = ast->next;

				break;
			case '>':
				theta = (_object_t*)_ls_popback(optr);
				b = (_object_t*)_ls_popback(opnd);
				a = (_object_t*)_ls_popback(opnd);
				r = _operate_operand(s, theta, a, b, &result);
				if(!r) {
					_ls_clear(optr);
					_handle_error_on_obj(s, SE_RN_OPERATION_FAILED, s->source_file, errn ? DON(errn) : DON(ast), MB_FUNC_ERR, _error, result);
				}
				_ls_pushback(opnd, r);
				_LAZY_INIT_GLIST;
				_ls_pushback(garbage, r);
				if(_IS_FUNC(c, _core_close_bracket))
					hack = true;

				break;
			case ' ':
				_handle_error_on_obj(s, SE_RN_OPERATION_FAILED, s->source_file, errn ? DON(errn) : DON(ast), MB_FUNC_ERR, _error, result);

				break;
			}
		}
	}

	if(errn) {
		_handle_error_on_obj(s, SE_RN_CLOSE_BRACKET_EXPECTED, s->source_file, DON(errn), MB_FUNC_ERR, _error, result);
	}

	c = (_object_t*)(_ls_popback(opnd));
	if(_is_unexpected_calc_type(s, c)) {
		_handle_error_on_obj(s, SE_RN_UNEXPECTED_TYPE, s->source_file, DON(ast), MB_FUNC_ERR, _error, result);
	}
#ifdef MB_PREFER_SPEED
	if(ast && ast->prev == *l) {
		_object_t* obj = (_object_t*)(*l)->data;
		switch(obj->type) {
		case _DT_NIL: /* Fall through */
		case _DT_INT: /* Fall through */
		case _DT_REAL:
			obj->is_const = true;

			break;
		default: /* Do nothing */
			break;
		}
	}
_fast:
#endif /* MB_PREFER_SPEED */
	if(c->type == _DT_VAR) {
		(*val)->type = c->data.variable->data->type;
		(*val)->data = c->data.variable->data->data;
		if(_is_string(c))
			(*val)->is_ref = true;
	} else {
		(*val)->type = c->type;
		if(_is_string(c)) {
			char* _str = _extract_string(c);
			(*val)->data.string = mb_strdup(_str, strlen(_str) + 1);
			(*val)->is_ref = false;
		} else {
			(*val)->data = c->data;
		}
	}
	if(guard_val != c && garbage && _ls_try_remove(garbage, c, _ls_cmp_data, 0)) {
		_try_clear_intermediate_value(c, 0, s);

		if(_is_referenced_calc_type(s, c))
			_destroy_object_capsule_only(c, 0);
		else
			_destroy_object(c, 0);
	}

	while(0) {
_error:
		if(garbage) {
			_LS_FOREACH(garbage, _do_nothing_on_object, _remove_if_exists, opnd);
		}
	}

_exit:
	if(garbage) {
		_LS_FOREACH(garbage, _destroy_object, _try_clear_intermediate_value, s);
		_ls_destroy(garbage);
	}
	if(optr) {
		_ls_foreach(optr, _destroy_object_not_compile_time);
		_ls_destroy(optr);
	}
	if(opnd) {
		_ls_foreach(opnd, _destroy_object_not_compile_time);
		_ls_destroy(opnd);
	}
	if(inep) {
		mb_free(_ls_popback(s->in_neg_expr));
	}
	*l = ast;
	mb_set_gc_enabled(s, gce);

	return result;
#undef _LAZY_INIT_GLIST
}

/* Push current variable argument list */
static _ls_node_t* _push_var_args(mb_interpreter_t* s) {
	_ls_node_t* result = s->var_args;

	s->var_args = 0;

	return result;
}

/* Pop current variable argument list */
static void _pop_var_args(mb_interpreter_t* s, _ls_node_t* last_var_args) {
	_ls_node_t* var_args = s->var_args;

	s->var_args = last_var_args;
	if(var_args) {
		_LS_FOREACH(var_args, _do_nothing_on_object, _destroy_var_arg, &s->gc);
		_ls_destroy(var_args);
	}
}

/* Pop an argument from the caller or a variable argument list */
static int _pop_arg(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, unsigned* ia, _routine_t* r, mb_pop_routine_arg_func_t pop_arg, _ls_node_t* args, mb_value_t* arg) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = *l;

	mb_make_nil(*arg);
#if _MULTILINE_STATEMENT
	if(_multiline_statement(s)) {
		_object_t* obj = 0;
		obj = (_object_t*)ast->data;
		while(obj && obj->type == _DT_EOS) {
			ast = ast->next;
			obj = ast ? (_object_t*)ast->data : 0;
		}
	}
#endif /* _MULTILINE_STATEMENT */
	if(ast && ast->data && _IS_FUNC(ast->data, _core_args)) {
		if(args) {
			_object_t* obj = (_object_t*)_ls_popfront(args);
			if(obj) {
				_internal_object_to_public_value(obj, arg);
				_destroy_object_capsule_only(obj, 0);
			}
		} else {
			arg->type = MB_DT_UNKNOWN;
		}
	} else {
		result = pop_arg(s, (void**)l, va, ca, ia, r, arg);
	}

	return result;
}

/* Process arguments of a routine */
static int _proc_args(mb_interpreter_t* s, _ls_node_t** l, _running_context_t* running, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg, bool_t proc_ref, _ls_node_t* args) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* parameters = 0;
	mb_value_t arg;
	_ls_node_t* pars = 0;
	_var_t* var = 0;
	_ls_node_t* rnode = 0;
	unsigned ia = 0;
	_ls_node_t* var_args = 0;

	parameters = r->func.basic.parameters;
#ifdef MB_ENABLE_LAMBDA
	if(r->type == MB_RT_LAMBDA)
		parameters = r->func.lambda.parameters;
#endif /* MB_ENABLE_LAMBDA */

	if(parameters) {
		mb_make_nil(arg);
		pars = parameters;
		pars = pars->next;
		while(pars && (!has_arg || (has_arg && has_arg(s, (void**)l, va, ca, &ia, r)))) {
			var = (_var_t*)pars->data;
			pars = pars->next;
			if(_IS_VAR_ARGS(var))
				break;

			if(pop_arg) {
				mb_check(_pop_arg(s, l, va, ca, &ia, r, pop_arg, args, &arg));
#ifdef MB_ENABLE_COLLECTION_LIB
				if(_try_purge_it(s, &arg, 0)) {
					_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
				}
#endif /* MB_ENABLE_COLLECTION_LIB */
			}

			if(running->meta == _SCOPE_META_REF) {
				_object_t* obj = (_object_t*)(_ht_find(running->var_dict, var->name)->data);
				var = obj->data.variable;

				if(proc_ref)
					var->data->is_ref = false;
			} else {
				rnode = _search_identifier_in_scope_chain(s, running, var->name, _PATHING_NONE, 0, 0);
				if(rnode)
					var = ((_object_t*)rnode->data)->data.variable;

				if(proc_ref)
					var->data->is_ref = true;
			}

			if(!pop_arg && var->data->type == _DT_STRING && !var->data->is_ref)
				_mark_lazy_destroy_string(s, var->data->data.string);
			result = _public_value_to_internal_object(&arg, var->data);

			if(result != MB_FUNC_OK)
				break;

			if(args && _ls_empty(args))
				break;
		}

		if(_IS_VAR_ARGS(var)) {
			if(has_arg && !var_args && _IS_VAR_ARGS(var))
				var_args = s->var_args = _ls_create();

			while(has_arg && has_arg(s, (void**)l, va, ca, &ia, r)) {
				if(pop_arg) {
					mb_check(_pop_arg(s, l, va, ca, &ia, r, pop_arg, args, &arg));
				}

				if(var_args) {
					_object_t* obj = _create_object();
					result = _public_value_to_internal_object(&arg, obj);
					if(obj->type == _DT_ROUTINE && obj->data.routine->type == MB_RT_SCRIPT)
						obj->is_ref = true;
					_ls_pushback(var_args, obj);
				}

				if(args && _ls_empty(args))
					break;
			}
		}

		ast = *l;
		if(ast) {
			_object_t* obj = (_object_t*)ast->data;
			if(obj && _IS_FUNC(obj, _core_args)) {
				if(ast) ast = ast->next;
				*l = ast;
			}
		}
	}

#ifdef MB_ENABLE_COLLECTION_LIB
_exit :
#endif /* MB_ENABLE_COLLECTION_LIB */
	return result;
}

/* Evaluate a routine */
static int _eval_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg) {
	int result = MB_FUNC_OK;
#ifdef MB_ENABLE_SOURCE_TRACE
	char* src = 0;
#endif /* MB_ENABLE_SOURCE_TRACE */
#if defined MB_ENABLE_STACK_TRACE && defined MB_ENABLE_LAMBDA
	char ln[_LAMBDA_NAME_MAX_LENGTH];
#endif /* MB_ENABLE_STACK_TRACE && MB_ENABLE_LAMBDA */

	_PREVCALL(s, l, r);

#ifdef MB_ENABLE_STACK_TRACE
	_ls_pushback(s->stack_frames, r->name);
#endif /* MB_ENABLE_STACK_TRACE */

#ifdef MB_ENABLE_SOURCE_TRACE
	src = s->source_file;
	s->source_file = r->source_file;
#endif /* MB_ENABLE_SOURCE_TRACE */

	if(r->type == MB_RT_SCRIPT && r->func.basic.entry) {
		result = _eval_script_routine(s, l, va, ca, r, has_arg, pop_arg);
#ifdef MB_ENABLE_LAMBDA
	} else if(r->type == MB_RT_LAMBDA && r->func.lambda.entry) {
#ifdef MB_ENABLE_STACK_TRACE
		_ls_node_t* top = _ls_back(s->stack_frames);
		if(top) {
			sprintf(ln, "LAMBDA_0x%p", &r->func.lambda.ref);
			top->data = ln;
		}
#endif /* MB_ENABLE_STACK_TRACE */
		result = _eval_lambda_routine(s, l, va, ca, r, has_arg, pop_arg);
#endif /* MB_ENABLE_LAMBDA */
	} else if(r->type == MB_RT_NATIVE && r->func.native.entry) {
		result = _eval_native_routine(s, l, va, ca, r, has_arg, pop_arg);
	} else {
		_handle_error_on_obj(s, SE_RN_INVALID_ROUTINE, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

_exit:
#ifdef MB_ENABLE_SOURCE_TRACE
	s->source_file = src;
#endif /* MB_ENABLE_SOURCE_TRACE */

#ifdef MB_ENABLE_STACK_TRACE
	_ls_popback(s->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */

	_POSTCALL(s, l, r);

	return result;
}

/* Evaluate a script routine */
static int _eval_script_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;
	_routine_t* lastr = 0;
	mb_value_t inte;
	_ls_node_t* lastv = 0;
	bool_t succ = false;
#ifdef MB_ENABLE_CLASS
	bool_t pushed_inst = false;
	_class_t* last_inst = 0;
	bool_t same_inst = s->last_routine ? s->last_routine->instance == r->instance : false;
#else /* MB_ENABLE_CLASS */
	bool_t same_inst = true;
#endif /* MB_ENABLE_CLASS */

	mb_assert(s && l && r);

	if(!va && s->last_routine && !s->last_routine->func.basic.parameters && same_inst && (s->last_routine->name == r->name || !strcmp(s->last_routine->name, r->name))) {
		ast = *l;
		_skip_to(s, &ast, 0, _DT_EOS);
		if(ast && ((_object_t*)ast->data)->type == _DT_EOS)
			ast = ast->next;
		if(ast && _IS_FUNC((_object_t*)ast->data, _core_enddef)) { /* Tail recursion optimization */
			*l = r->func.basic.entry;
			if(*l)
				*l = (*l)->next;

			goto _tail;
		}
	}

	lastr = s->last_routine;
	s->last_routine = r;

	lastv = _push_var_args(s);

	if(!va) {
		mb_check(mb_attempt_open_bracket(s, (void**)l));
	}

	running = _push_weak_scope_by_routine(s, r->func.basic.scope, r);
	result = _proc_args(s, l, running, va, ca, r, has_arg, pop_arg, true, lastv);
	if(result != MB_FUNC_OK) {
		if(running->meta == _SCOPE_META_REF)
			_destroy_scope(s, running);
		else
			_pop_weak_scope(s, running);

		goto _error;
	}
	running = _pop_weak_scope(s, running);

	if(!va) {
		_mb_check_mark_exit(mb_attempt_close_bracket(s, (void**)l), result, _error);
	}

	ast = *l;
	_ls_pushback(s->sub_stack, ast);

#ifdef MB_ENABLE_CLASS
	if(r->instance && s->last_instance != r->instance) {
		pushed_inst = true;
		last_inst = s->last_instance;
		s->last_instance = r->instance;
		if(r->instance)
			_push_scope_by_class(s, r->instance->scope);
	}
#endif /* MB_ENABLE_CLASS */

	running = _push_scope_by_routine(s, running);

	*l = r->func.basic.entry;
	if(!(*l)) {
		_handle_error_on_obj(s, SE_RN_INVALID_ROUTINE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	do {
		result = _execute_statement(s, l, true);
		ast = *l;
		if(result == MB_SUB_RETURN) {
			result = MB_FUNC_OK;

			break;
		}
		if(result == MB_FUNC_SUSPEND) {
			_handle_error_now(s, SE_RN_CANNOT_SUSPEND_HERE, s->last_error_file, result);

			goto _exit;
		}
		if(result != MB_FUNC_OK) {
			if(result >= MB_EXTENDED_ABORT)
				s->last_error = SE_EA_EXTENDED_ABORT;
			_handle_error_now(s, s->last_error, s->last_error_file, result);

			goto _exit;
		}
	} while(ast);

#ifdef MB_ENABLE_CLASS
	_out_of_scope(s, running, r->instance, r, true);
#else /* MB_ENABLE_CLASS */
	_out_of_scope(s, running, 0, r, true);
#endif /* MB_ENABLE_CLASS */

	result = _proc_args(s, l, running, 0, 0, r, 0, 0, false, 0);
	if(result != MB_FUNC_OK)
		goto _exit;

	succ = true;

	mb_make_nil(inte);
	_swap_public_value(&inte, &running->intermediate_value);

	_pop_scope(s, true);

_exit:
	if(!succ)
		_pop_scope(s, true);

#ifdef MB_ENABLE_CLASS
	if(pushed_inst) {
		if(r->instance)
			_pop_scope(s, false);
		s->last_instance = last_inst;
	}
#endif /* MB_ENABLE_CLASS */

	if(succ)
		_assign_public_value(s, &s->running_context->intermediate_value, &inte, false);

_error:
	s->last_routine = lastr;

	_pop_var_args(s, lastv);

_tail:
	return result;
}

#ifdef MB_ENABLE_LAMBDA
/* Evaluate a lambda routine */
static int _eval_lambda_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;
	_routine_t* lastr = 0;
	mb_value_t inte;
	_ls_node_t* lastv = 0;

	mb_assert(s && l && r);

	lastr = s->last_routine;
	s->last_routine = r;

	lastv = _push_var_args(s);

	if(!va) {
		mb_check(mb_attempt_open_bracket(s, (void**)l));
	}

	running = _link_lambda_scope_chain(s, &r->func.lambda, true);
	if(!running) {
		_handle_error_on_obj(s, SE_RN_INVALID_ROUTINE, s->source_file, DON2(l), MB_FUNC_ERR, _error, result);
	}
	result = _proc_args(s, l, running, va, ca, r, has_arg, pop_arg, true, lastv);
	ast = *l;
	if(result != MB_FUNC_OK) {
		_unlink_lambda_scope_chain(s, &r->func.lambda, true);

		goto _error;
	}
	running = _unlink_lambda_scope_chain(s, &r->func.lambda, true);

	if(!va) {
		_mb_check_mark_exit(mb_attempt_close_bracket(s, (void**)l), result, _error);
	}

	ast = *l;
	_ls_pushback(s->sub_stack, ast);

	running = _link_lambda_scope_chain(s, &r->func.lambda, false);

	*l = r->func.lambda.entry;
	if(!(*l)) {
		_handle_error_on_obj(s, SE_RN_INVALID_ROUTINE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	do {
		result = _execute_statement(s, l, true);
		ast = *l;
		if(result == MB_SUB_RETURN) {
			result = MB_FUNC_OK;

			break;
		}
		if(result == MB_FUNC_SUSPEND) {
			_handle_error_now(s, SE_RN_CANNOT_SUSPEND_HERE, s->last_error_file, result);

			goto _exit;
		}
		if(result != MB_FUNC_OK) {
			if(result >= MB_EXTENDED_ABORT)
				s->last_error = SE_EA_EXTENDED_ABORT;
			_handle_error_now(s, s->last_error, s->last_error_file, result);

			goto _exit;
		}
	} while(ast);

	_out_of_scope(s, running, 0, r, true);

	result = _proc_args(s, l, running, 0, 0, r, 0, 0, false, 0);
	if(result != MB_FUNC_OK)
		goto _exit;

	mb_make_nil(inte);
	_swap_public_value(&inte, &running->intermediate_value);

	running = _unlink_lambda_scope_chain(s, &r->func.lambda, false);

	_assign_public_value(s, &s->running_context->intermediate_value, &inte, false);

_exit:
	if(result != MB_FUNC_OK)
		_unlink_lambda_scope_chain(s, &r->func.lambda, false);

_error:
	s->last_routine = lastr;

	_pop_var_args(s, lastv);

	*l = ast;

	return result;
}
#endif /* MB_ENABLE_LAMBDA */

/* Evaluate a native routine */
static int _eval_native_routine(mb_interpreter_t* s, _ls_node_t** l, mb_value_t* va, unsigned ca, _routine_t* r, mb_has_routine_arg_func_t has_arg, mb_pop_routine_arg_func_t pop_arg) {
	int result = MB_FUNC_OK;
	_routine_t* lastr = 0;
	mb_routine_func_t entry = 0;
	_ls_node_t* lastv = 0;

	mb_assert(s && l && r);

	lastr = s->last_routine;
	s->last_routine = r;

	lastv = _push_var_args(s);

	entry = r->func.native.entry;
	if(!entry) {
		_handle_error_on_obj(s, SE_RN_INVALID_ROUTINE, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

	result = entry(s, (void**)l, va, ca, r, has_arg, pop_arg);

_exit:
	s->last_routine = lastr;

	_pop_var_args(s, lastv);

	return result;
}

/* Detect if there is any more lexical argument */
static int _has_routine_lex_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r) {
	mb_unrefvar(va);
	mb_unrefvar(ca);
	mb_unrefvar(ia);
	mb_unrefvar(r);

	return mb_has_arg(s, l);
}

/* Pop a lexical argument */
static int _pop_routine_lex_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r, mb_value_t* val) {
	mb_unrefvar(va);
	mb_unrefvar(ca);
	mb_unrefvar(ia);
	mb_unrefvar(r);

	return mb_pop_value(s, l, val);
}

/* Detect if there is any more argument in the argument list */
static int _has_routine_fun_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r) {
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(va);
	mb_unrefvar(r);

	return *ia < ca;
}

/* Pop an argument from the argument list */
static int _pop_routine_fun_arg(mb_interpreter_t* s, void** l, mb_value_t* va, unsigned ca, unsigned* ia, void* r, mb_value_t* val) {
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(ca);
	mb_unrefvar(r);

	memcpy(val, &(va[*ia]), sizeof(mb_value_t));
	(*ia)++;

	return MB_FUNC_OK;
}

/* Determine whether an object is a PRINT termination */
static bool_t _is_print_terminal(mb_interpreter_t* s, _object_t* obj) {
	bool_t result = false;

	mb_assert(s && obj);

	result = (
		_IS_EOS(obj) ||
		_IS_SEP(obj, ':') ||
		_IS_FUNC(obj, _core_elseif) ||
		_IS_FUNC(obj, _core_else) ||
		_IS_FUNC(obj, _core_endif)
	);

	return result;
}

/* Try to call overridden function */
static mb_meta_status_e _try_overridden(mb_interpreter_t* s, void** l, mb_value_t* d, const char* f, mb_meta_func_e t) {
	mb_assert(s && l && d && f);

#ifdef MB_ENABLE_USERTYPE_REF
	if(d->type == MB_DT_USERTYPE_REF) {
		_object_t obj;
		_MAKE_NIL(&obj);
		_public_value_to_internal_object(d, &obj);
		if(t == MB_MF_COLL && obj.data.usertype_ref->coll_func)
			return obj.data.usertype_ref->coll_func(s, l, d, f);
		else if(t == MB_MF_FUNC && obj.data.usertype_ref->generic_func)
			return obj.data.usertype_ref->generic_func(s, l, d, f);
	}
#endif /* MB_ENABLE_USERTYPE_REF */
#ifdef MB_ENABLE_CLASS
	if(d->type == MB_DT_CLASS) {
		char buf[_TEMP_FORMAT_MAX_LENGTH];
		_ls_node_t* ofn = 0;
		_object_t obj;
		_MAKE_NIL(&obj);
		_public_value_to_internal_object(d, &obj);
		sprintf(buf, _CLASS_OVERRIDE_FMT, f);
		ofn = _search_identifier_in_class(s, obj.data.instance, buf, 0, 0);
		if(ofn) {
			_object_t* ofo = (_object_t*)ofn->data;
			_ls_node_t* ast = (_ls_node_t*)*l;
			mb_value_t va[1];
			mb_make_nil(va[0]);
			if(_eval_routine(s, &ast, va, 0, ofo->data.routine, _has_routine_lex_arg, _pop_routine_lex_arg) == MB_FUNC_OK) {
				if(ast)
					*l = ast->prev;

				return (mb_meta_status_e)(MB_MS_DONE | MB_MS_RETURNED);
			}
		}
	}
#endif /* MB_ENABLE_CLASS */
#if !defined MB_ENABLE_USERTYPE_REF || !defined MB_ENABLE_CLASS
	mb_unrefvar(t);
#endif /* !MB_ENABLE_USERTYPE_REF && !MB_ENABLE_CLASS */

	return MB_MS_NONE;
}

/** Handlers */

/* Set current error information */
static bool_t _set_current_error(mb_interpreter_t* s, mb_error_e err, char* f) {
	mb_assert(s && err >= 0);

	if(s->last_error == SE_NO_ERR) {
		s->last_error = err;
		s->last_error_file = f;

		return true;
	}

	return false;
}

/* Get a print functor of an interpreter */
static mb_print_func_t _get_printer(mb_interpreter_t* s) {
	mb_assert(s);

	if(s->printer)
		return s->printer;

	return printf;
}

/* Get an input functor of an interpreter */
static mb_input_func_t _get_inputer(mb_interpreter_t* s) {
	mb_assert(s);

	if(s->inputer)
		return s->inputer;

	return mb_gets;
}

/* Print a string */
static void _print_string(mb_interpreter_t* s, _object_t* obj) {
#if defined MB_CP_VC && defined MB_ENABLE_UNICODE
#if _LOCALIZATION_USEING
	char* loc = 0;
#endif /* _LOCALIZATION_USEING */
	char* str = 0;
	_dynamic_buffer_t buf;
	size_t lbuf = 0;

	mb_assert(s && obj);

#if _LOCALIZATION_USEING
	loc = setlocale(LC_ALL, _LOCALIZATION_STR);
#endif /* _LOCALIZATION_USEING */
	str = obj->data.string ? obj->data.string : MB_NULL_STRING;
	_INIT_BUF(buf);
	while((lbuf = (size_t)mb_bytes_to_wchar(str, &_WCHAR_BUF_PTR(buf), _WCHARS_OF_BUF(buf))) > _WCHARS_OF_BUF(buf)) {
		_RESIZE_WCHAR_BUF(buf, lbuf);
	}
	_get_printer(s)("%ls", _WCHAR_BUF_PTR(buf));
	_DISPOSE_BUF(buf);
#if _LOCALIZATION_USEING
	setlocale(LC_ALL, loc);
#endif /* _LOCALIZATION_USEING */
#else /* MB_CP_VC && MB_ENABLE_UNICODE */
	mb_assert(s && obj);

	_get_printer(s)("%s", obj->data.string ? obj->data.string : MB_NULL_STRING);
#endif /* MB_CP_VC && MB_ENABLE_UNICODE */
}

/** Parsing helpers */

/* Read all content of a file into a buffer */
static char* _load_file(mb_interpreter_t* s, const char* f, const char* prefix, bool_t importing) {
#ifndef MB_DISABLE_LOAD_FILE
	FILE* fp = 0;
	char* buf = 0;
	long curpos = 0;
	long l = 0;
	long i = 0;
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = (_parsing_context_t*)s->parsing_context;

	if(_ls_find(context->imported, (void*)f, (_ls_compare_t)_ht_cmp_string, 0)) {
		buf = (char*)f;
	} else {
		fp = fopen(f, "rb");
		if(fp) {
			if(importing) {
				buf = mb_strdup(f, strlen(f) + 1);
				_ls_pushback(context->imported, buf);
				buf = 0;
			}

			curpos = ftell(fp);
			fseek(fp, 0L, SEEK_END);
			l = ftell(fp);
			fseek(fp, curpos, SEEK_SET);
			if(prefix) {
				i = (long)strlen(prefix);
				l += i;
			}
			buf = (char*)mb_malloc((size_t)(l + 1));
			mb_assert(buf);
			if(prefix)
				memcpy(buf, prefix, i);
			fread(buf + i, 1, l, fp);
			do {
				char* off = buf + i;
				int b = mb_uu_getbom((const char**)&off);
				if(b) {
					memmove(buf + i, buf + i + b, l - b - i);
					buf[l - b] = _ZERO_CHAR;
				}
			} while(0);
			fclose(fp);
			buf[l] = _ZERO_CHAR;
		}
	}

	return buf;
#else /* MB_DISABLE_LOAD_FILE */
	return 0;
#endif /* MB_DISABLE_LOAD_FILE */
}

/* Finish loading a file */
static void _end_of_file(_parsing_context_t* context) {
	if(context)
		context->parsing_state = _PS_NORMAL;
}

/* Determine whether a character is blank */
static bool_t _is_blank_char(char c) {
	return (c == ' ') || (c == '\t');
}

/* Determine whether a character is end of file */
static bool_t _is_eof_char(char c) {
#ifdef __cplusplus
	union { signed char s; char c; } u;
	u.c = c;

	return u.s == EOF;
#else /* __cplusplus */
	return (c == EOF);
#endif /* __cplusplus */
}

/* Determine whether a character is newline */
static bool_t _is_newline_char(char c) {
	return (c == _RETURN_CHAR) || (c == _NEWLINE_CHAR) || _is_eof_char(c);
}

/* Determine whether a character is separator */
static bool_t _is_separator_char(char c) {
	return (c == ',') || (c == ';') || (c == ':');
}

/* Determine whether a character is bracket */
static bool_t _is_bracket_char(char c) {
	return (c == '(') || (c == ')');
}

/* Determine whether a character is quotation mark */
static bool_t _is_quotation_char(char c) {
	return (c == '"');
}

/* Determine whether a character is comment mark */
static bool_t _is_comment_char(char c) {
	return (c == '\'');
}

/* Determine whether a character is accessor char */
static bool_t _is_accessor_char(char c) {
	return (c == '.');
}

/* Determine whether a character is numeric char */
static bool_t _is_numeric_char(char c) {
	return (c >= '0' && c <= '9') || _is_accessor_char(c);
}

/* Determine whether a character is identifier char */
static bool_t _is_identifier_char(char c) {
#if defined MB_ENABLE_LAMBDA && defined MB_LAMBDA_ALIAS
	char* p = MB_LAMBDA_ALIAS;
	while(*p) {
		if(c == *p) return true;
		++p;
	}
#endif /* MB_ENABLE_LAMBDA && MB_LAMBDA_ALIAS */

	return (
		(c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
		(c == '_') ||
		_is_numeric_char(c) ||
		(c == _STRING_POSTFIX_CHAR)
	);
}

/* Determine whether a character is operator char */
static bool_t _is_operator_char(char c) {
	return (
		(c == '+') || (c == '-') || (c == '*') || (c == '/') ||
		(c == '^') ||
		(c == '(') || (c == ')') ||
		(c == '=') ||
		(c == '>') || (c == '<')
	);
}

/* Determine whether a character is exponential char */
static bool_t _is_exponential_char(char c) {
	return (c == 'e') || (c == 'E');
}

/* Determine whether a character is module using char */
static bool_t _is_using_at_char(char c) {
	return (c == '@');
}

/* Determine whether current symbol is exponent prefix */
static bool_t _is_exponent_prefix(char* s, int begin, int end) {
	int i = 0;

	mb_assert(s);

	if(end < 0)
		return false;

	for(i = begin; i <= end; i++) {
		if(!_is_numeric_char(s[i]))
			return false;
	}

	return true;
}

/* Parse a character and append it to current parsing symbol */
static int _append_char_to_symbol(mb_interpreter_t* s, char c) {
	int result = MB_FUNC_OK;
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;

	if(_is_accessor_char(c))
		context->current_symbol_contains_accessor++;

	if(context->current_symbol_nonius + 1 >= _SINGLE_SYMBOL_MAX_LENGTH) {
		_set_current_error(s, SE_PS_SYMBOL_TOO_LONG, 0);

		result = MB_FUNC_ERR;
	} else {
		context->current_symbol[context->current_symbol_nonius] = c;
		++context->current_symbol_nonius;
	}

	return result;
}

#ifdef MB_ENABLE_UNICODE_ID
/* Parse a UTF8 character and append it to current parsing symbol */
static int _append_uu_char_to_symbol(mb_interpreter_t* s, const char* str, int n) {
	int result = MB_FUNC_OK;
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;

	if(context->current_symbol_nonius + n >= _SINGLE_SYMBOL_MAX_LENGTH) {
		_set_current_error(s, SE_PS_SYMBOL_TOO_LONG, 0);

		result = MB_FUNC_ERR;
	} else {
		memcpy(&context->current_symbol[context->current_symbol_nonius], str, n);
		context->current_symbol_nonius += n;
	}

	return result;
}
#endif /* MB_ENABLE_UNICODE_ID */

/* Cut current symbol when current one parsing is finished */
static int _cut_symbol(mb_interpreter_t* s, int pos, unsigned short row, unsigned short col) {
	int result = MB_FUNC_OK;
	_parsing_context_t* context = 0;
	char* sym = 0;
	int status = 0;
	bool_t delsym = false;

	mb_assert(s);

	context = s->parsing_context;
	if(context->current_symbol_nonius && context->current_symbol[0] != _ZERO_CHAR) {
		sym = (char*)mb_malloc(context->current_symbol_nonius + 1);
		memcpy(sym, context->current_symbol, context->current_symbol_nonius + 1);

		status = _append_symbol(s, sym, &delsym, pos, row, col);
		if(status || delsym) {
			safe_free(sym);
		}
		result = status;
	}
	memset(context->current_symbol, 0, sizeof(context->current_symbol));
	context->current_symbol_nonius = 0;
	context->current_symbol_contains_accessor = 0;

	return result;
}

/* Append cut symbol to the AST list */
static int _append_symbol(mb_interpreter_t* s, char* sym, bool_t* delsym, int pos, unsigned short row, unsigned short col) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_ls_node_t** assign = 0;
	_ls_node_t* node = 0;
	_parsing_context_t* context = 0;

	mb_assert(s && sym);

	ast = s->ast;
	result = _create_symbol(s, ast, sym, &obj, &assign, delsym);
	if(obj) {
#ifdef MB_ENABLE_SOURCE_TRACE
		obj->source_pos = pos;
		obj->source_row = row;
		obj->source_col = col;
#else /* MB_ENABLE_SOURCE_TRACE */
		mb_unrefvar(row);
		mb_unrefvar(col);

		obj->source_pos = (char)!!pos;
#endif /* MB_ENABLE_SOURCE_TRACE */

		node = _ls_pushback(ast, obj);
		if(assign)
			*assign = node;

		context = s->parsing_context;
		context->last_symbol = obj;
	}

	return result;
}

/* Create a syntax symbol */
static int _create_symbol(mb_interpreter_t* s, _ls_node_t* l, char* sym, _object_t** obj, _ls_node_t*** asgn, bool_t* delsym) {
	int result = MB_FUNC_OK;
	_data_e type;
	union {
		_func_t* func; _array_t* array;
#ifdef MB_ENABLE_CLASS
		_class_t* instance;
#endif /* MB_ENABLE_CLASS */
		_routine_t* routine; _var_t* var; _label_t* label; real_t float_point; int_t integer; _raw_t any;
	} tmp;
	_raw_t value;
	unsigned ul = 0;
	_parsing_context_t* context = 0;
	_running_context_t* running = 0;
	_ls_node_t* glbsyminscope = 0;
	bool_t is_field = false;
	mb_unrefvar(l);

	mb_assert(s && sym && obj);

	memset(value, 0, sizeof(_raw_t));

	context = s->parsing_context;
	running = s->running_context;

	*obj = _create_object();
#ifdef MB_ENABLE_SOURCE_TRACE
	(*obj)->source_pos = -1;
	(*obj)->source_row = (*obj)->source_col = 0xFFFF;
#else /* MB_ENABLE_SOURCE_TRACE */
	(*obj)->source_pos = -1;
#endif /* MB_ENABLE_SOURCE_TRACE */

	type = _get_symbol_type(s, sym, &value);
	if(s->last_error != SE_NO_ERR) {
		result = MB_FUNC_ERR;

		goto _exit;
	}
	(*obj)->type = type;
	switch(type) {
	case _DT_NIL:
		memcpy(tmp.any, value, sizeof(_raw_t));
		if(tmp.integer) { /* Nil type */
			(*obj)->type = _DT_NIL;
		} else { /* End of line character */
			safe_free(*obj);
		}
		safe_free(sym);

		break;
	case _DT_INT:
		memcpy(tmp.any, value, sizeof(_raw_t));
		(*obj)->data.integer = tmp.integer;
		safe_free(sym);

		break;
	case _DT_REAL:
		memcpy(tmp.any, value, sizeof(_raw_t));
		(*obj)->data.float_point = tmp.float_point;
		safe_free(sym);

		break;
	case _DT_STRING: {
			size_t _sl = strlen(sym);
			(*obj)->data.string = (char*)mb_malloc(_sl - 2 + 1);
			memcpy((*obj)->data.string, sym + sizeof(char), _sl - 2);
			(*obj)->data.string[_sl - 2] = _ZERO_CHAR;
			*delsym = true;
		}

		break;
	case _DT_FUNC:
		tmp.func = (_func_t*)mb_malloc(sizeof(_func_t));
		memset(tmp.func, 0, sizeof(_func_t));
		tmp.func->name = sym;
		memcpy(&tmp.func->pointer, value, sizeof(tmp.func->pointer));
		(*obj)->data.func = tmp.func;

		break;
	case _DT_ARRAY:
		glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, _PATHING_NONE, 0, 0);
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_ARRAY) {
			(*obj)->data.array = ((_object_t*)glbsyminscope->data)->data.array;
			(*obj)->is_ref = true;
			*delsym = true;
		} else {
			tmp.array = _create_array(s, sym, _DT_UNKNOWN);
			memcpy(&tmp.array->type, value, sizeof(tmp.array->type));
			(*obj)->data.array = tmp.array;

			ul = _ht_set_or_insert(running->var_dict, sym, *obj);
			mb_assert(ul);

			*obj = _create_object();
			(*obj)->type = type;
			(*obj)->data.array = tmp.array;
			(*obj)->is_ref = true;
		}

		break;
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		if(!_is_identifier_char(*sym))
			*sym = _INVALID_CLASS_CHAR;
		glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, _PATHING_NONE, 0, 0);
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_CLASS) {
			(*obj)->data.instance = ((_object_t*)glbsyminscope->data)->data.instance;
			(*obj)->is_ref = true;
			*delsym = true;
			if(running != (*obj)->data.instance->scope &&
				(context->class_state != _CLASS_STATE_NONE) &&
				_IS_FUNC(context->last_symbol, _core_class)) {
				_push_scope_by_class(s, (*obj)->data.instance->scope);
			}
		} else {
			tmp.instance = (_class_t*)mb_malloc(sizeof(_class_t));
			_init_class(s, tmp.instance, sym);
			_push_scope_by_class(s, tmp.instance->scope);
			s->last_instance = tmp.instance;

			(*obj)->data.instance = tmp.instance;

			ul = _ht_set_or_insert(running->var_dict, sym, *obj);
			mb_assert(ul);

			*obj = _create_object();
			(*obj)->type = type;
			(*obj)->data.instance = tmp.instance;
			(*obj)->is_ref = true;
		}

		break;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		if(!_is_identifier_char(*sym))
			*sym = _INVALID_ROUTINE_CHAR;
		glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, _PATHING_NONE, 0, 0);
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_ROUTINE) {
			(*obj)->data.routine = ((_object_t*)glbsyminscope->data)->data.routine;
			(*obj)->is_ref = true;
			*delsym = true;
			if(running != (*obj)->data.routine->func.basic.scope &&
				context->routine_state &&
				_IS_FUNC(context->last_symbol, _core_def)) {
				_push_scope_by_routine(s, (*obj)->data.routine->func.basic.scope);
			}
		} else {
			_running_context_t* tba = 0;
			tmp.routine = (_routine_t*)mb_malloc(sizeof(_routine_t));
			_init_routine(s, tmp.routine, sym, 0);
			_push_scope_by_routine(s, tmp.routine->func.basic.scope);
			(*obj)->data.routine = tmp.routine;

			tba = _get_scope_to_add_routine(s);
			ul = _ht_set_or_insert(tba->var_dict, sym, *obj);
			mb_assert(ul);
			if(tba != _OUTTER_SCOPE(running) && tba != running)
				_pop_scope(s, false);

			*obj = _create_object();
			(*obj)->type = type;
			(*obj)->data.routine = tmp.routine;
			(*obj)->is_ref = true;

#ifdef MB_ENABLE_CLASS
			tmp.routine->instance = s->last_instance;
#endif /* MB_ENABLE_CLASS */
		}

		break;
	case _DT_VAR:
		if(context->routine_params_state == _ROUTINE_STATE_PARAMS)
			glbsyminscope = _ht_find(running->var_dict, sym);
		else
			glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, _PATHING_NONE, 0, 0);
#ifdef MB_ENABLE_CLASS
		is_field = context->last_symbol && _IS_FUNC(context->last_symbol, _core_var);
#endif /* MB_ENABLE_CLASS */
		if(!is_field && glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_VAR) {
			(*obj)->data.variable = ((_object_t*)glbsyminscope->data)->data.variable;
			(*obj)->is_ref = true;
			*delsym = true;
		} else {
#ifdef MB_ENABLE_CLASS
			if(strcmp(sym, _CLASS_ME) == 0) {
				_handle_error_now(s, SE_RN_INVALID_ID_USAGE, s->source_file, MB_FUNC_ERR);
				(*obj)->is_ref = true;
				*delsym = true;

				goto _exit;
			}
#endif /* MB_ENABLE_CLASS */
			tmp.var = (_var_t*)mb_malloc(sizeof(_var_t));
			memset(tmp.var, 0, sizeof(_var_t));
			tmp.var->name = sym;
			tmp.var->data = _create_object();
			tmp.var->data->type = (sym[strlen(sym) - 1] == _STRING_POSTFIX_CHAR) ? _DT_STRING : _DT_INT;
			tmp.var->data->data.integer = 0;
#ifdef MB_ENABLE_CLASS
			if(context->class_state != _CLASS_STATE_NONE)
				tmp.var->pathing = _PATHING_NORMAL;
			else if(!is_field)
				tmp.var->pathing = context->current_symbol_contains_accessor ? _PATHING_NORMAL : _PATHING_NONE;
#endif /* MB_ENABLE_CLASS */
			(*obj)->data.variable = tmp.var;

			ul = _ht_set_or_insert(running->var_dict, sym, *obj);
			mb_assert(ul);

			*obj = _create_object();
			(*obj)->type = type;
			(*obj)->data.variable = tmp.var;
			(*obj)->is_ref = true;
		}

		break;
	case _DT_LABEL:
		if(context->current_char == ':') {
			if(mb_memtest(value, sizeof(_raw_t))) {
				memcpy(&((*obj)->data.label), value, sizeof((*obj)->data.label));
				(*obj)->is_ref = true;
				*delsym = true;
			} else {
				tmp.label = (_label_t*)mb_malloc(sizeof(_label_t));
				memset(tmp.label, 0, sizeof(_label_t));
				tmp.label->name = sym;
				*asgn = &(tmp.label->node);
				(*obj)->data.label = tmp.label;

				ul = _ht_set_or_insert(running->var_dict, sym, *obj);
				mb_assert(ul);

				*obj = _create_object();
				(*obj)->type = type;
				(*obj)->data.label = tmp.label;
				(*obj)->is_ref = true;
			}
		} else {
			(*obj)->data.label = (_label_t*)mb_malloc(sizeof(_label_t));
			memset((*obj)->data.label, 0, sizeof(_label_t));
			(*obj)->data.label->name = sym;
		}

		break;
	case _DT_SEP:
		(*obj)->data.separator = sym[0];
		safe_free(sym);

		break;
	case _DT_EOS:
		safe_free(sym);

		break;
	default: /* Do nothing */
		break;
	}

_exit:
	return result;
}

/* Get the type of a syntax symbol */
static _data_e _get_symbol_type(mb_interpreter_t* s, char* sym, _raw_t* value) {
	_data_e result = _DT_NIL;
	union { real_t float_point; int_t integer; _object_t* obj; _raw_t any; } tmp;
	char* conv_suc = 0;
	_parsing_context_t* context = 0;
	_running_context_t* running = 0;
	_ls_node_t* glbsyminscope = 0;
	size_t _sl = 0;
	_data_e en = _DT_UNKNOWN;
	intptr_t ptr = 0;
	bool_t mod = false;

	mb_assert(s && sym);
	_sl = strlen(sym);
	mb_assert(_sl > 0);

	context = s->parsing_context;
	running = s->running_context;

	/* int_t */
	tmp.integer = (int_t)mb_strtol(sym, &conv_suc, 0);
	if(*conv_suc == _ZERO_CHAR) {
		memcpy(*value, tmp.any, sizeof(_raw_t));

		result = _DT_INT;

		goto _exit;
	}
	/* real_t */
	tmp.float_point = (real_t)mb_strtod(sym, &conv_suc);
	if(*conv_suc == _ZERO_CHAR) {
		memcpy(*value, tmp.any, sizeof(_raw_t));

		result = _DT_REAL;

		goto _exit;
	}
	/* String */
	if(_is_quotation_char(sym[0]) && _is_quotation_char(sym[_sl - 1]) && _sl >= 2) {
		result = _DT_STRING;

		if(context->last_symbol && _IS_FUNC(context->last_symbol, _core_import)) {
			/* IMPORT statement */
			int n = context->current_symbol_nonius;
			char current_symbol[_SINGLE_SYMBOL_MAX_LENGTH + 1];
			char* buf = 0;
			memcpy(current_symbol, context->current_symbol, sizeof(current_symbol));
			memset(context->current_symbol, 0, sizeof(current_symbol));
			context->current_symbol_nonius = 0;
			context->last_symbol = 0;
			sym[_sl - 1] = _ZERO_CHAR;
			context->parsing_state = _PS_NORMAL;
			/* Using a module */
			if(_is_using_at_char(*(sym + 1))) {
#ifdef MB_ENABLE_MODULE
				char* ns = mb_strdup(sym + 2, strlen(sym + 2) + 1);
				mb_strupr(ns);
				if(_ls_find(s->using_modules, ns, (_ls_compare_t)_ht_cmp_string, 0)) {
					safe_free(ns);
				} else {
					_ls_pushback(s->using_modules, ns);
				}

				goto _end_import;
#else /* MB_ENABLE_MODULE */
				_handle_error_now(s, SE_CM_NOT_SUPPORTED, s->source_file, MB_FUNC_ERR);

				goto _end_import;
#endif /* MB_ENABLE_MODULE */
			}
			/* Import another file */
			buf = _load_file(s, sym + 1, ":", true);
			if(buf) {
				if(buf != sym + 1) {
					char* lf = (char*)(_ls_back(context->imported)->data);
					int pos; unsigned short row, col;
					lf = _prev_import(s, lf, &pos, &row, &col);
					mb_load_string(s, buf, true);
					safe_free(buf);
					_post_import(s, lf, &pos, &row, &col);
				}
			} else {
				if(!_ls_find(context->imported, (void*)(sym + 1), (_ls_compare_t)_ht_cmp_string, 0)) {
					if(s->import_handler) {
						_object_t* sep = 0;
						char* lf = 0;
						int pos; unsigned short row, col;
						sep = _create_object();
						sep->type = _DT_SEP;
						sep->data.separator = ':';
						_ls_pushback(s->ast, sep);
						_ls_pushback(context->imported, mb_strdup(sym + 1, strlen(sym + 1) + 1));
						lf = (char*)(_ls_back(context->imported)->data);
						lf = _prev_import(s, lf, &pos, &row, &col);
						if(s->import_handler(s, sym + 1) != MB_FUNC_OK) {
							_ls_node_t* last = _ls_back(context->imported);
							if(s->last_error == SE_NO_ERR) {
								context->parsing_pos = pos;
								context->parsing_row = row;
								context->parsing_col = col;
								_handle_error_now(s, SE_PS_OPEN_FILE_FAILED, s->source_file, MB_FUNC_ERR);
							}
							_destroy_memory(last->data, last->extra);
							_ls_popback(context->imported);
						}
						_post_import(s, lf, &pos, &row, &col);
					} else {
						_handle_error_now(s, SE_PS_OPEN_FILE_FAILED, s->source_file, MB_FUNC_ERR);
					}
				}
			}

_end_import:
			context->parsing_state = _PS_STRING;
			sym[_sl - 1] = '"';
			context->current_symbol_nonius = n;
			memcpy(context->current_symbol, current_symbol, sizeof(current_symbol));
			result = _DT_NIL;
		}

		goto _exit;
	}
	/* Nil */
	if(!strcmp(sym, MB_NIL)) {
		tmp.integer = ~0;
		memcpy(*value, tmp.any, sizeof(_raw_t));

		result = _DT_NIL;

		goto _exit;
	}
	/* REM */
	if(!strcmp(sym, _REMARK_STR)) {
		context->parsing_state = _PS_COMMENT;

		result = _DT_EOS;

		goto _exit;
	}
	/* _array_t */
	glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, _PATHING_NONE, 0, 0);
	if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_ARRAY) {
		tmp.obj = (_object_t*)glbsyminscope->data;
		memcpy(*value, &(tmp.obj->data.array->type), sizeof(tmp.obj->data.array->type));

		result = _DT_ARRAY;

		goto _exit;
	}
	if(context->last_symbol && _IS_FUNC(context->last_symbol, _core_dim)) {
#ifdef MB_SIMPLE_ARRAY
		en = (sym[_sl - 1] == _STRING_POSTFIX_CHAR ? _DT_STRING : _DT_REAL);
#else /* MB_SIMPLE_ARRAY */
		en = _DT_REAL;
#endif /* MB_SIMPLE_ARRAY */
		memcpy(*value, &en, sizeof(en));

		result = _DT_ARRAY;

		goto _exit;
	}
	/* _class_t */
#ifdef MB_ENABLE_CLASS
	if(context->last_symbol) {
		glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, _PATHING_NONE, 0, 0);
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_ROUTINE)
			goto _routine;
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_CLASS) {
			if(_IS_FUNC(context->last_symbol, _core_class)) {
				_handle_error_now(s, SE_RN_DUPLICATE_CLASS, s->source_file, MB_FUNC_ERR);
			}
			result = _DT_CLASS;

			goto _exit;
		}
		if(_IS_FUNC(context->last_symbol, _core_class)) {
			if(s->last_instance) {
				_handle_error_now(s, SE_RN_DUPLICATE_CLASS, s->source_file, MB_FUNC_ERR);

				goto _exit;
			}
			_begin_class(s);
#ifdef MB_ENABLE_UNICODE_ID
			if(!_is_identifier_char(sym[0]) && !mb_uu_ischar(sym)) {
#else /* MB_ENABLE_UNICODE_ID */
			if(!_is_identifier_char(sym[0])) {
#endif /* MB_ENABLE_UNICODE_ID */
				result = _DT_NIL;

				goto _exit;
			}
			if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_VAR) {
				_handle_error_now(s, SE_RN_INVALID_CLASS, s->source_file, MB_FUNC_ERR);

				goto _exit;
			}

			if(context->routine_state > 1) {
				_handle_error_now(s, SE_RN_INVALID_CLASS, s->source_file, MB_FUNC_ERR);

				goto _exit;
			}

			result = _DT_CLASS;

			goto _exit;
		} else if(_IS_FUNC(context->last_symbol, _core_endclass)) {
			if(_end_class(s))
				_pop_scope(s, false);
		}
	}
_routine:
#endif /* MB_ENABLE_CLASS */
	/* _routine_t */
	if(context->last_symbol && !_is_bracket_char(sym[0])) {
		glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, _PATHING_NONE, 0, 0);
		if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_ROUTINE) {
			if(_IS_FUNC(context->last_symbol, _core_def)) {
				if(_begin_routine(s) != MB_FUNC_OK)
					goto _exit;
			}
			result = _DT_ROUTINE;

			goto _exit;
		}
		if(_IS_FUNC(context->last_symbol, _core_def) || _IS_FUNC(context->last_symbol, _core_call)) {
			if(_IS_FUNC(context->last_symbol, _core_def)) {
				if(_begin_routine(s) != MB_FUNC_OK)
					goto _exit;
			}
#ifdef MB_ENABLE_UNICODE_ID
			if(!_is_identifier_char(sym[0]) && !mb_uu_ischar(sym)) {
#else /* MB_ENABLE_UNICODE_ID */
			if(!_is_identifier_char(sym[0])) {
#endif /* MB_ENABLE_UNICODE_ID */
				result = _DT_NIL;

				goto _exit;
			}
			if(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_VAR) {
				_handle_error_now(s, SE_RN_INVALID_ROUTINE, s->source_file, MB_FUNC_ERR);

				goto _exit;
			}

			if(_IS_FUNC(context->last_symbol, _core_def)) {
				if(context->routine_state > 1) {
					_handle_error_now(s, SE_RN_INVALID_ROUTINE, s->source_file, MB_FUNC_ERR);

					goto _exit;
				}
			}

			result = _DT_ROUTINE;

			goto _exit;
		} else if(_IS_FUNC(context->last_symbol, _core_enddef)) {
			if(_end_routine(s))
				_pop_scope(s, false);
		}
	}
	/* _func_t */
	if(!context->last_symbol ||
		(context->last_symbol && ((context->last_symbol->type == _DT_FUNC && context->last_symbol->data.func->pointer != _core_close_bracket) ||
		context->last_symbol->type == _DT_SEP || context->last_symbol->type == _DT_EOS))) {
		if(strcmp("-", sym) == 0) {
			ptr = (intptr_t)_core_neg;
			memcpy(*value, &ptr, sizeof(intptr_t));

			result = _DT_FUNC;

			goto _exit;
		}
	}
	glbsyminscope = _find_func(s, sym, &mod);
	if(glbsyminscope) {
		if(context->last_symbol && context->last_symbol->type == _DT_ROUTINE) {
			if(_sl == 1 && sym[0] == '(') {
				if(context->routine_params_state == _ROUTINE_STATE_DEF)
					_begin_routine_parameter_list(s);
			}
		} else if(context->routine_params_state == _ROUTINE_STATE_PARAMS) {
			if(_sl == 1 && sym[0] == ')')
				_end_routine_parameter_list(s);
		}

#ifdef MB_ENABLE_MODULE
		if(mod) {
			_module_func_t* mp = (_module_func_t*)glbsyminscope->data;
			ptr = (intptr_t)mp->func;
			memcpy(*value, &ptr, sizeof(intptr_t));
		} else {
			ptr = (intptr_t)glbsyminscope->data;
			memcpy(*value, &ptr, sizeof(intptr_t));
			if(ptr == (intptr_t)_core_def)
				_begin_routine_definition(s);
		}
#else /* MB_ENABLE_MODULE */
		ptr = (intptr_t)glbsyminscope->data;
		memcpy(*value, &ptr, sizeof(intptr_t));
		if(ptr == (intptr_t)_core_def)
			_begin_routine_definition(s);
#endif /* MB_ENABLE_MODULE */

		result = _DT_FUNC;

		goto _exit;
	}
	/* MB_EOS */
	if(_sl == 1 && sym[0] == MB_EOS) {
		if(_IS_EOS(context->last_symbol))
			result = _DT_NIL;
		else
			result = _DT_EOS;

		goto _exit;
	}
	/* Separator */
	if(_sl == 1 && _is_separator_char(sym[0])) {
		result = _DT_SEP;

		goto _exit;
	}
	/* _var_t */
	glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, _PATHING_NONE, 0, 0);
	if(glbsyminscope) {
		if(((_object_t*)glbsyminscope->data)->type != _DT_LABEL) {
			memcpy(*value, &glbsyminscope->data, sizeof(glbsyminscope->data));

			result = _DT_VAR;

			goto _exit;
		}
	}
	/* _label_t */
	if(context->current_char == ':') {
		if(!context->last_symbol || _IS_EOS(context->last_symbol)) {
			glbsyminscope = _search_identifier_in_scope_chain(s, 0, sym, _PATHING_NONE, 0, 0);
			if(glbsyminscope)
				memcpy(*value, &glbsyminscope->data, sizeof(glbsyminscope->data));

			result = _DT_LABEL;

			goto _exit;
		}
	}
	if(context->last_symbol && (_IS_FUNC(context->last_symbol, _core_goto) || _IS_FUNC(context->last_symbol, _core_gosub))) {
		result = _DT_LABEL;

		goto _exit;
	}
	/* Otherwise */
	result = _DT_VAR;

_exit:
	return result;
}

/* Parse a character */
static int _parse_char(mb_interpreter_t* s, const char* str, int n, int pos, unsigned short row, unsigned short col) {
	int result = MB_FUNC_OK;
	_parsing_context_t* context = 0;
	char last_char = _ZERO_CHAR;
	char c = _ZERO_CHAR;
#ifdef MB_ENABLE_UNICODE_ID
	unsigned uc = 0;
#else /* MB_ENABLE_UNICODE_ID */
	mb_unrefvar(n);
#endif /* MB_ENABLE_UNICODE_ID */

	mb_assert(s && s->parsing_context);

	context = s->parsing_context;

	if(str) {
#ifdef MB_ENABLE_UNICODE_ID
		if(n == 1)
			c = *str;
		else
			memcpy(&uc, str, n);
#else /* MB_ENABLE_UNICODE_ID */
		c = *str;
#endif /* MB_ENABLE_UNICODE_ID */
	} else {
		c = MB_EOS;
	}

	last_char = context->current_char;
	context->current_char = c;

	switch(context->parsing_state) {
	case _PS_NORMAL:
#ifdef MB_ENABLE_UNICODE_ID
		if(uc) {
			if(context->symbol_state == _SS_IDENTIFIER) {
				_mb_check_exit(result = _append_uu_char_to_symbol(s, str, n), _exit);
			} else if(context->symbol_state == _SS_OPERATOR) {
				context->symbol_state = _SS_IDENTIFIER;
				_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
				_mb_check_exit(result = _append_uu_char_to_symbol(s, str, n), _exit);
			}

			break;
		}
#endif /* MB_ENABLE_UNICODE_ID */

		c = toupper(c);
		if(_is_blank_char(c)) { /* \t space */
			_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
		} else if(_is_newline_char(c)) { /* \r \n EOF */
			_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
			_mb_check_exit(result = _append_char_to_symbol(s, MB_EOS), _exit);
			_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
		} else if(_is_separator_char(c) || _is_bracket_char(c)) { /* , ; : ( ) */
			_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
			_mb_check_exit(result = _append_char_to_symbol(s, c), _exit);
			_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
		} else if(_is_quotation_char(c)) { /* " */
			_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
			_mb_check_exit(result = _append_char_to_symbol(s, c), _exit);
			context->parsing_state = _PS_STRING;
		} else if(_is_comment_char(c)) { /* ' */
			_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
			_mb_check_exit(result = _append_char_to_symbol(s, MB_EOS), _exit);
			_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
			context->parsing_state = _PS_COMMENT;
			context->multi_line_comment_count = 1;
		} else {
			if(context->symbol_state == _SS_IDENTIFIER) {
				if(_is_identifier_char(c)) {
					_mb_check_exit(result = _append_char_to_symbol(s, c), _exit);
				} else if(_is_operator_char(c)) {
					if(_is_exponent_prefix(context->current_symbol, 0, context->current_symbol_nonius - 2) && _is_exponential_char(last_char) && c == '-') {
						_mb_check_exit(result = _append_char_to_symbol(s, c), _exit);
					} else {
						context->symbol_state = _SS_OPERATOR;
						_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
						_mb_check_exit(result = _append_char_to_symbol(s, c), _exit);
					}
				} else {
					_handle_error_at_pos(s, SE_PS_INVALID_CHAR, s->source_file, pos, row, col, MB_FUNC_ERR, _exit, result);
				}
			} else if(context->symbol_state == _SS_OPERATOR) {
				if(_is_identifier_char(c)) {
					context->symbol_state = _SS_IDENTIFIER;
					_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
					_mb_check_exit(result = _append_char_to_symbol(s, c), _exit);
				} else if(_is_operator_char(c)) {
					if(c == '-')
						_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
					_mb_check_exit(result = _append_char_to_symbol(s, c), _exit);
				} else {
					_handle_error_at_pos(s, SE_PS_INVALID_CHAR, s->source_file, pos, row, col, MB_FUNC_ERR, _exit, result);
				}
			} else {
				mb_assert(0 && "Impossible.");
			}
		}

		break;
	case _PS_STRING:
		if(_is_quotation_char(c)) { /* " */
			_mb_check_exit(result = _append_char_to_symbol(s, c), _exit);
			_mb_check_exit(result = _cut_symbol(s, pos, row, col), _exit);
			context->parsing_state = _PS_NORMAL;
		} else {
			_mb_check_exit(result = _append_char_to_symbol(s, c), _exit);
		}

		break;
	case _PS_COMMENT:
		if(_is_eof_char(c)) { /* EOF */
			context->parsing_state = _PS_NORMAL;

			break;
		}
		if(context->multi_line_comment_count != 0 && c == _MULTI_LINE_COMMENT_PREFIX[context->multi_line_comment_count++]) {
			if(context->multi_line_comment_count >= countof(_MULTI_LINE_COMMENT_PREFIX) - 1) {
				context->parsing_state = _PS_MULTI_LINE_COMMENT;
				context->multi_line_comment_count = 0;

				break;
			}

			break;
		} else {
			context->multi_line_comment_count = 0;
		}
		if(_is_newline_char(c)) /* \r \n EOF */
			context->parsing_state = _PS_NORMAL;

		break;
	case _PS_MULTI_LINE_COMMENT:
		if(_is_eof_char(c)) { /* EOF */
			context->parsing_state = _PS_NORMAL;

			break;
		}
		if(_is_comment_char(c) && context->multi_line_comment_count == 0) {
			context->multi_line_comment_count = 1;
		} else if(context->multi_line_comment_count != 0 && c == _MULTI_LINE_COMMENT_POSTFIX[context->multi_line_comment_count++]) {
			if(context->multi_line_comment_count >= countof(_MULTI_LINE_COMMENT_POSTFIX) - 1) {
				context->parsing_state = _PS_NORMAL;
				context->multi_line_comment_count = 0;
			}
		} else {
			context->multi_line_comment_count = 0;
		}

		break;
	default:
		mb_assert(0 && "Unknown parsing state.");

		break;
	}

_exit:
	return result;
}

/* Set the position of an error */
static void _set_error_pos(mb_interpreter_t* s, int pos, unsigned short row, unsigned short col) {
	mb_assert(s);

	s->last_error_pos = pos;
	s->last_error_row = row;
	s->last_error_col = col;
}

/* Do something before importing another file */
static char* _prev_import(mb_interpreter_t* s, char* lf, int* pos, unsigned short* row, unsigned short* col) {
#ifdef MB_ENABLE_SOURCE_TRACE
	char* result = 0;
	_parsing_context_t* context = 0;
	_import_info_t* info = 0;
	_object_t* obj = 0;

	mb_assert(s);

	context = s->parsing_context;
	if(context) {
		if(pos) *pos = context->parsing_pos;
		if(row) *row = context->parsing_row;
		if(col) *col = context->parsing_col;
		context->parsing_pos = 0;
		context->parsing_row = 1;
		context->parsing_col = 0;
	}

	result = s->source_file;
	s->source_file = lf;

	obj = _create_object();
	obj->type = _DT_EOS;
	obj->is_ref = false;
	_ls_pushback(s->ast, obj);

	info = (_import_info_t*)mb_malloc(sizeof(_import_info_t));
	info->source_file = lf ? mb_strdup(lf, strlen(lf) + 1) : 0;
	obj = _create_object();
	obj->type = _DT_PREV_IMPORT;
	obj->is_ref = false;
	obj->data.import_info = info;
	_ls_pushback(s->ast, obj);

	return result;
#else /* MB_ENABLE_SOURCE_TRACE */
	mb_unrefvar(s);
	mb_unrefvar(lf);
	mb_unrefvar(pos);
	mb_unrefvar(row);
	mb_unrefvar(col);

	return 0;
#endif /* MB_ENABLE_SOURCE_TRACE */
}

/* Do something after importing another file */
static char* _post_import(mb_interpreter_t* s, char* lf, int* pos, unsigned short* row, unsigned short* col) {
#ifdef MB_ENABLE_SOURCE_TRACE
	char* result = 0;
	_parsing_context_t* context = 0;
	_import_info_t* info = 0;
	_object_t* obj = 0;

	mb_assert(s);

	context = s->parsing_context;
	if(context) {
		if(pos) context->parsing_pos = *pos;
		if(row) context->parsing_row = *row;
		if(col) context->parsing_col = *col;
	}

	s->source_file = lf;
	result = s->source_file;

	info = (_import_info_t*)mb_malloc(sizeof(_import_info_t));
	info->source_file = lf ? mb_strdup(lf, strlen(lf) + 1) : 0;
	obj = _create_object();
	obj->type = _DT_POST_IMPORT;
	obj->is_ref = false;
	obj->data.import_info = info;
	_ls_pushback(s->ast, obj);

	return result;
#else /* MB_ENABLE_SOURCE_TRACE */
	mb_unrefvar(s);
	mb_unrefvar(lf);
	mb_unrefvar(pos);
	mb_unrefvar(row);
	mb_unrefvar(col);

	return 0;
#endif /* MB_ENABLE_SOURCE_TRACE */
}

/** Object processors */

/* Get the size of a data type */
static int_t _get_size_of(_data_e type) {
	int_t result = 0;

#ifdef MB_SIMPLE_ARRAY
	if(type == _DT_INT) {
		result = sizeof(int_t);
	} else if(type == _DT_REAL) {
		result = sizeof(real_t);
	} else if(type == _DT_STRING) {
		result = sizeof(char*);
	} else {
		mb_assert(0 && "Unsupported.");
	}
#else /* MB_SIMPLE_ARRAY */
	mb_unrefvar(type);

	result = sizeof(_raw_u);
#endif /* MB_SIMPLE_ARRAY */

	return result;
}

/* Try to get a value (typed as int_t, real_t or char*) */
static bool_t _try_get_value(_object_t* obj, mb_value_u* val, _data_e expected) {
	bool_t result = false;

	mb_assert(obj && val);

	if(obj->type == _DT_INT && (expected == _DT_UNKNOWN || expected == _DT_INT)) {
		val->integer = obj->data.integer;
		result = true;
	} else if(obj->type == _DT_REAL && (expected == _DT_UNKNOWN || expected == _DT_REAL)) {
		val->float_point = obj->data.float_point;
		result = true;
	} else if(obj->type == _DT_VAR) {
		result = _try_get_value(obj->data.variable->data, val, expected);
	}

	return result;
}

/* Determine if an object is a nil */
static bool_t _is_nil(void* obj) {
	bool_t result = false;
	_object_t* o = 0;

	mb_assert(obj);

	o = (_object_t*)obj;
	if(o->type == _DT_NIL)
		result = true;
	else if(o->type == _DT_VAR)
		result = o->data.variable->data->type == _DT_NIL;

	return result;
}

/* Determine if an object is a number */
static bool_t _is_number(void* obj) {
	bool_t result = false;
	_object_t* o = 0;

	mb_assert(obj);

	o = (_object_t*)obj;
	if(o->type == _DT_INT || o->type == _DT_REAL)
		result = true;
	else if(o->type == _DT_VAR)
		result = o->data.variable->data->type == _DT_INT || o->data.variable->data->type == _DT_REAL;

	return result;
}

/* Determine if an object is a string value or a string variable */
static bool_t _is_string(void* obj) {
	bool_t result = false;
	_object_t* o = 0;

	mb_assert(obj);

	o = (_object_t*)obj;
	if(o->type == _DT_STRING)
		result = true;
	else if(o->type == _DT_VAR)
		result = o->data.variable->data->type == _DT_STRING;

	return result;
}

/* Extract a string from an object */
static char* _extract_string(_object_t* obj) {
	char* result = 0;

	mb_assert(obj);

	if(obj->type == _DT_STRING)
		result = obj->data.string;
	else if(obj->type == _DT_VAR && obj->data.variable->data->type == _DT_STRING)
		result = obj->data.variable->data->data.string;

	if(!result)
		result = MB_NULL_STRING;

	return result;
}

#ifdef MB_MANUAL_REAL_FORMATTING
/* Convert a real number to string */
static void _real_to_str(real_t r, char* str, size_t size, size_t afterpoint) {
	size_t pos = 0;
	size_t len = 0;
	char curr[4];
	int val = (int)r;
	char dot = 0;

	itoa(val, str, 10);
	if(r < 0) {
		r *= -1;
		val *= -1;
	}
	pos = len = strlen(str);
	while(pos < size - 1) {
		r = r - (real_t)val;
		if(r == 0.0)
			break;
		if(!dot) {
			dot = 1;
			str[pos++] = '.';
		}
		r *= 10;
		val = (int)r;
		itoa(val, curr, 10);
		str[pos++] = *curr;
		if(--afterpoint == 0)
			break;
	}
	str[pos] = _ZERO_CHAR;
}
#endif /* MB_MANUAL_REAL_FORMATTING */

#ifdef _HAS_REF_OBJ_LOCK
/* Lock a referenced object */
static bool_t _lock_ref_object(mb_lock_t* lk, _ref_t* ref, void* obj) {
	mb_assert(lk);

	_ref(ref, obj);
	if(*lk >= 0)
		++(*lk);
	else
		--(*lk);

	return true;
}

/* Unlock a referenced object */
static bool_t _unlock_ref_object(mb_lock_t* lk, _ref_t* ref, void* obj) {
	bool_t result = true;

	mb_assert(lk);

	if(*lk > 0)
		--(*lk);
	else if(*lk < 0)
		++(*lk);
	else
		result = false;
	_unref(ref, obj);

	return result;
}

/* Write operation on a referenced object */
static bool_t _write_on_ref_object(mb_lock_t* lk, _ref_t* ref, void* obj) {
	bool_t result = true;
	mb_unrefvar(ref);
	mb_unrefvar(obj);

	mb_assert(lk);

	if(*lk > 0)
		*lk = -(*lk);
	else
		result = false;

	return result;
}
#endif /* _HAS_REF_OBJ_LOCK */

/* Tell whether an object is referenced */
static bool_t _is_ref(_object_t* obj) {
	switch(obj->type) {
#ifdef MB_ENABLE_USERTYPE_REF
	case _DT_USERTYPE_REF:
		return true;
#endif /* MB_ENABLE_USERTYPE_REF */
#ifdef MB_ENABLE_ARRAY_REF
	case _DT_ARRAY:
		return true;
#endif /* MB_ENABLE_ARRAY_REF */
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		return true;
	case _DT_DICT:
		return true;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		return true;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
#ifdef MB_ENABLE_LAMBDA
		return obj->data.routine->type == MB_RT_LAMBDA;
#else /* MB_ENABLE_LAMBDA */
		return false;
#endif /* MB_ENABLE_LAMBDA */
	default:
		return false;
	}
}

/* Increase the reference of a stub by 1 */
static _ref_count_t _ref(_ref_t* ref, void* data) {
	_ref_count_t before = *ref->count;

	mb_assert((intptr_t)ref == (intptr_t)data);

	++(*ref->count);
	if(before > *ref->count) {
		mb_assert(0 && "Too many referencing, count overflow, please redefine _ref_count_t.");

		_handle_error_now(ref->s, SE_RN_OVERFLOW, ref->s->last_error_file, MB_FUNC_ERR);
	}

	return *ref->count;
}

/* Decrease the reference of a stub by 1 */
static bool_t _unref(_ref_t* ref, void* data) {
	bool_t result = true;
	_gc_t* gc = 0;
	bool_t cld = false;

	mb_assert((intptr_t)ref == (intptr_t)data);

	cld = *ref->count == _NONE_REF + 1;
	do {
		gc = &ref->s->gc;
		result = --(*ref->count) == _NONE_REF;
		mb_assert(*ref->count >= _NONE_REF);
		_gc_add(ref, data, &ref->s->gc);
		if(ref->count && *ref->count == _NONE_REF)
			_collect_intermediate_value(ref, data);
		ref->on_unref(ref, data);
		if(result)
			_gc_remove(ref, data, gc);
	} while(0);
	if(cld) {
		_ht_set_or_insert(gc->collected_table, ref, data);
		_ht_set_or_insert(gc->collected_table, data, ref);
	}

	return result;
}

/* Increase the weak reference of a stub by 1 */
static _ref_count_t _weak_ref(_ref_t* ref, void* data, _ref_t* weak) {
	_ref_count_t before = *ref->weak_count;
	mb_unrefvar(data);

	++(*ref->weak_count);
	if(before > *ref->weak_count) {
		mb_assert(0 && "Too many referencing, weak count overflow, please redefine _ref_count_t.");

		_handle_error_now(ref->s, SE_RN_OVERFLOW, ref->s->last_error_file, MB_FUNC_ERR);
	}
	memcpy(weak, ref, sizeof(_ref_t));

	return *ref->weak_count;
}

/* Decrease the weak reference of a stub by 1 */
static bool_t _weak_unref(_ref_t* weak) {
	bool_t result = true;

	--(*weak->weak_count);
	mb_assert(*weak->weak_count >= _NONE_REF);
	if(weak->count && *weak->count == _NONE_REF)
		result = false;
	if(weak->count && *weak->count == _NONE_REF && weak->weak_count && *weak->weak_count == _NONE_REF) {
		safe_free(weak->weak_count);
		safe_free(weak->count);
	}

	return result;
}

/* Create a reference stub, initialize the reference count with zero */
static void _create_ref(_ref_t* ref, _unref_func_t dtor, _data_e t, mb_interpreter_t* s) {
#ifdef MB_ENABLE_FORK
	mb_interpreter_t* src = 0;
#endif /* MB_ENABLE_FORK */

	if(ref->count)
		return;

#ifdef MB_ENABLE_FORK
	while(mb_get_forked_from(s, &src) == MB_FUNC_OK)
		s = src;
#endif /* MB_ENABLE_FORK */

	ref->count = (_ref_count_t*)mb_malloc(sizeof(_ref_count_t));
	*ref->count = _NONE_REF;
	ref->weak_count = (_ref_count_t*)mb_malloc(sizeof(_ref_count_t));
	*ref->weak_count = _NONE_REF;
	ref->on_unref = dtor;
	ref->type = t;
	ref->s = s;
}

/* Destroy a reference stub */
static void _destroy_ref(_ref_t* ref) {
	if(!ref->count || !ref->weak_count)
		return;

	if(*ref->weak_count == _NONE_REF) {
		safe_free(ref->weak_count);
		safe_free(ref->count);
	}
	ref->on_unref = 0;
}

/* Add a referenced object to GC table for later garbage detection */
static void _gc_add(_ref_t* ref, void* data, _gc_t* gc) {
	_ht_node_t* table = 0;

	mb_assert(ref && data);

	if(!ref->count)
		return;

	if(gc && _ht_find(gc->collected_table, ref))
		_ht_remove(gc->collected_table, ref, 0);

	if(!gc->table)
		return;

	if(gc->collecting)
		table = gc->recursive_table;
	else
		table = gc->table;

	if(gc && gc->valid_table && _ht_find(gc->valid_table, ref))
		_ht_remove(table, ref, 0);
	else if(ref->count && *ref->count > _NONE_REF)
		_ht_set_or_insert(table, ref, data);
	else
		_ht_remove(table, ref, 0);
}

/* Remove a referenced object from GC */
static unsigned _gc_remove(_ref_t* ref, void* data, _gc_t* gc) {
	_ht_node_t* table = 0;

	mb_assert(ref && data && gc);

	if(gc->collecting)
		table = gc->recursive_table;
	else
		table = gc->table;

	if(table)
		return _ht_remove(table, ref, 0);

	return 0;
}

/* Get reachable objects */
static int _gc_add_reachable(void* data, void* extra, void* h) {
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	_var_t* var = 0;
	_ht_node_t* ht = 0;

	mb_assert(data && h);

	ht = (_ht_node_t*)h;
	obj = (_object_t*)data;
	if(_is_internal_object(obj))
		goto _exit;
	switch(obj->type) {
	case _DT_VAR:
		var = (_var_t*)obj->data.variable;
		_gc_add_reachable(var->data, extra, ht);

		break;
#ifdef MB_ENABLE_USERTYPE_REF
	case _DT_USERTYPE_REF:
		if(!_ht_find(ht, &obj->data.usertype_ref->ref))
			_ht_set_or_insert(ht, &obj->data.usertype_ref->ref, obj->data.usertype_ref);
#ifdef MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF
		if(obj->data.usertype_ref->alive_checker) {
			mb_value_t val;
			mb_make_nil(val);
			_internal_object_to_public_value(obj, &val);
			obj->data.usertype_ref->alive_checker(obj->data.usertype_ref->ref.s, h, val, _gc_alive_marker);
		}
#endif /* MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF */

		break;
#endif /* MB_ENABLE_USERTYPE_REF */
#ifdef MB_ENABLE_ARRAY_REF
	case _DT_ARRAY:
		if(!_ht_find(ht, &obj->data.array->ref))
			_ht_set_or_insert(ht, &obj->data.array->ref, obj->data.array);

		break;
#endif /* MB_ENABLE_ARRAY_REF */
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		if(!_ht_find(ht, &obj->data.list->ref)) {
			_ht_set_or_insert(ht, &obj->data.list->ref, obj->data.list);
			_LS_FOREACH(obj->data.list->list, _do_nothing_on_object, _gc_add_reachable, ht);
		}

		break;
	case _DT_DICT:
		if(!_ht_find(ht, &obj->data.dict->ref)) {
			_ht_set_or_insert(ht, &obj->data.dict->ref, obj->data.dict);
			_HT_FOREACH(obj->data.dict->dict, _do_nothing_on_object, _gc_add_reachable_both, ht);
		}

		break;
	case _DT_LIST_IT:
		if(!_ht_find(ht, &obj->data.list_it->list->ref)) {
			_ht_set_or_insert(ht, &obj->data.list_it->list->ref, obj->data.list_it->list);
			_LS_FOREACH(obj->data.list_it->list->list, _do_nothing_on_object, _gc_add_reachable, ht);
		}

		break;
	case _DT_DICT_IT:
		if(!_ht_find(ht, &obj->data.dict_it->dict->ref)) {
			_ht_set_or_insert(ht, &obj->data.dict_it->dict->ref, obj->data.dict_it->dict);
			_HT_FOREACH(obj->data.dict_it->dict->dict, _do_nothing_on_object, _gc_add_reachable_both, ht);
		}

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		if(!_ht_find(ht, &obj->data.instance->ref)) {
			_ht_set_or_insert(ht, &obj->data.instance->ref, obj->data.instance);
			_traverse_class(obj->data.instance, _gc_add_reachable, _add_class_meta_reachable, _META_LIST_MAX_DEPTH, false, ht, 0);
		}

		break;
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_LAMBDA
	case _DT_ROUTINE:
		if(obj->data.routine->type == MB_RT_LAMBDA) {
			if(!_ht_find(ht, &obj->data.routine->func.lambda.ref)) {
				_running_context_ref_t* outs = obj->data.routine->func.lambda.outer_scope;
				_ht_set_or_insert(ht, &obj->data.routine->func.lambda.ref, obj->data.routine);
				_HT_FOREACH(obj->data.routine->func.lambda.scope->var_dict, _do_nothing_on_object, _gc_add_reachable, ht);
				while(outs) {
					_ht_set_or_insert(ht, &outs->ref, outs);
					_HT_FOREACH(outs->scope->var_dict, _do_nothing_on_object, _gc_add_reachable, ht);
					outs = outs->prev;
				}
			}
		}

		break;
#endif /* MB_ENABLE_LAMBDA */
	default: /* Do nothing */
		break;
	}

_exit:
	return result;
}

/* Get reachable objects from both key and value */
static int _gc_add_reachable_both(void* data, void* extra, void* h) {
	int result = _OP_RESULT_NORMAL;

	mb_assert(data && extra && h);

	_gc_add_reachable(extra, 0, h);
	_gc_add_reachable(data, extra, h);

	return result;
}

#ifdef MB_ENABLE_FORK
/* Get reachable objects in a forked environment */
static int _gc_get_reachable_in_forked(void* data, void* extra, _ht_node_t* valid) {
	int result = _OP_RESULT_NORMAL;
	mb_interpreter_t* s = 0;
	_running_context_t* root = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	s = (mb_interpreter_t*)data;
	root = _get_root_scope(s->running_context);
	_gc_get_reachable(s, valid, root);

	return result;
}
#endif /* MB_ENABLE_FORK */

/* Get all reachable referenced objects */
static void _gc_get_reachable(mb_interpreter_t* s, _ht_node_t* ht, _running_context_t* end) {
	_running_context_t* running = 0;
	_ht_node_t* scope = 0;

	mb_assert(s && ht);

	running = s->running_context;
	while(running && running != end) {
		scope = running->var_dict;
		if(scope) {
			_HT_FOREACH(scope, _do_nothing_on_object, _gc_add_reachable, ht);
		}
		running = running->prev;
	}
}

/* Alive marker functor of a value */
static void _gc_alive_marker(mb_interpreter_t* s, void* h, mb_value_t val) {
	_ht_node_t* ht = 0;
	_object_t obj;
	mb_unrefvar(s);

	ht = (_ht_node_t*)h;
	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&val, &obj);
	_gc_add_reachable(&obj, 0, h);
}

/* Destroy only the capsule (wrapper) of an object, leave the data behind, and add it to GC if possible */
static int _gc_destroy_garbage_in_list(void* data, void* extra, _gc_t* gc) {
	int result = _OP_RESULT_DEL_NODE;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	_ADDGC(obj, gc, false)
	safe_free(obj);

	return result;
}

/* Destroy only the capsule (wrapper) of an object, leave the data behind, deal with extra as well, and add it to GC if possible */
static int _gc_destroy_garbage_in_dict(void* data, void* extra, _gc_t* gc) {
	int result = _OP_RESULT_DEL_NODE;
	_object_t* obj = 0;

	mb_assert(data);

	obj = (_object_t*)data;
	_ADDGC(obj, gc, false)
	safe_free(obj);

	obj = (_object_t*)extra;
	_ADDGC(obj, gc, false)
	safe_free(obj);

	return result;
}

#ifdef MB_ENABLE_CLASS
/* Destroy only the capsule (wrapper) of an object, leave the data behind, deal with extra as well, and add it to GC if possible */
static int _gc_destroy_garbage_in_class(void* data, void* extra, _gc_t* gc) {
	int result = _OP_RESULT_DEL_NODE;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	if(obj->type == _DT_VAR) {
		_gc_destroy_garbage_in_class(obj->data.variable->data, 0, gc);
		safe_free(obj->data.variable->name);
		safe_free(obj->data.variable);
	} else {
		_ADDGC(obj, gc, true)
	}
	safe_free(obj);

	return result;
}
#endif /* MB_ENABLE_CLASS */

#ifdef MB_ENABLE_LAMBDA
/* Destroy only the capsule (wrapper) of an object, leave the data behind, deal with extra as well, and add it to GC if possible */
static int _gc_destroy_garbage_in_lambda(void* data, void* extra, _gc_t* gc) {
	int result = _OP_RESULT_DEL_NODE;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	if(obj->type == _DT_VAR) {
#ifdef MB_ENABLE_CLASS
		if(_is_string(obj) && obj->data.variable->pathing) {
			safe_free(obj->data.variable->data);
		} else {
			_gc_destroy_garbage_in_lambda(obj->data.variable->data, 0, gc);
		}
#else /* MB_ENABLE_CLASS */
		_gc_destroy_garbage_in_lambda(obj->data.variable->data, 0, gc);
#endif /* MB_ENABLE_CLASS */
		safe_free(obj->data.variable->name);
		safe_free(obj->data.variable);
	} else {
		_ADDGC(obj, gc, false)
	}
	safe_free(obj);

	return result;
}

/* Collect garbage of outer scopes */
static void _gc_destroy_garbage_in_outer_scope(_running_context_ref_t* p, _gc_t* gc) {
	while(p) {
		_running_context_ref_t* scope = p;
		p = p->prev;
		_HT_FOREACH(scope->scope->var_dict, _do_nothing_on_object, _gc_destroy_garbage_in_lambda, gc);
		_ht_clear(scope->scope->var_dict);
	}
}
#endif /* MB_ENABLE_LAMBDA */

/* Collect a garbage */
static int _gc_destroy_garbage(void* data, void* extra, _gc_t* gc) {
	int result = _OP_RESULT_NORMAL;
	_ref_t* ref = 0;
	bool_t proc = true;
#ifdef MB_ENABLE_COLLECTION_LIB
	_list_t* lst = 0;
	_dict_t* dct = 0;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_LAMBDA
	_routine_t* routine = 0;
	_running_context_ref_t* outs = 0;
#endif /* MB_ENABLE_LAMBDA */

	mb_assert(data && extra);

	ref = (_ref_t*)extra;
	if(_ht_find(gc->collected_table, ref)) {
		proc = true;

		goto _exit;
	}
	switch(ref->type) {
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		lst = (_list_t*)data;
		_LS_FOREACH(lst->list, _do_nothing_on_object, _gc_destroy_garbage_in_list, gc);
		_ls_clear(lst->list);
		lst->count = 0;

		break;
	case _DT_DICT:
		dct = (_dict_t*)data;
		_HT_FOREACH(dct->dict, _do_nothing_on_object, _gc_destroy_garbage_in_dict, gc);
		_ht_clear(dct->dict);

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_LAMBDA
	case _DT_ROUTINE:
		routine = (_routine_t*)data;
		if(routine->type == MB_RT_LAMBDA) {
			_HT_FOREACH(routine->func.lambda.scope->var_dict, _do_nothing_on_object, _gc_destroy_garbage_in_lambda, gc);
			_ht_clear(routine->func.lambda.scope->var_dict);
			outs = routine->func.lambda.outer_scope;
			if(outs) {
				if(!_ht_find(gc->collected_table, &outs->ref))
					_gc_add(&outs->ref, outs, gc);
				routine->func.lambda.outer_scope = 0;
			}
			while(outs) {
				if(_ht_find(gc->collected_table, &outs->ref))
					break;
				_HT_FOREACH(outs->scope->var_dict, _do_nothing_on_object, _remove_this_lambda_from_upvalue, routine);
				outs = outs->prev;
			}
		}

		break;
#endif /* MB_ENABLE_LAMBDA */
#ifdef MB_ENABLE_USERTYPE_REF
	case _DT_USERTYPE_REF: /* Fall through */
#endif /* MB_ENABLE_USERTYPE_REF */
	case _DT_ARRAY: /* Do nothing */
		break;
	default:
		proc = false;

		break;
	}
	if(proc && ref->count)
		_unref(ref, data);

_exit:
	if(proc)
		result = _OP_RESULT_DEL_NODE;

	return result;
}

#ifdef MB_ENABLE_CLASS
/* Collect a class instance */
static int _gc_destroy_garbage_class(void* data, void* extra, _gc_t* gc) {
	int result = _OP_RESULT_NORMAL;
	_ref_t* ref = 0;
	bool_t proc = true;
	_class_t* instance = 0;

	mb_assert(data && extra);

	ref = (_ref_t*)extra;
	if(_ht_find(gc->collected_table, ref)) {
		proc = true;

		goto _exit;
	}
	switch(ref->type) {
	case _DT_CLASS:
		instance = (_class_t*)data;
		_HT_FOREACH(instance->scope->var_dict, _do_nothing_on_object, _gc_destroy_garbage_in_class, gc);
		_ht_clear(instance->scope->var_dict);
		_ls_clear(instance->meta_list);
#ifdef MB_ENABLE_LAMBDA
		if(instance->scope->refered_lambdas) {
			_ls_destroy(instance->scope->refered_lambdas);
			instance->scope->refered_lambdas = 0;
		}
#endif /* MB_ENABLE_LAMBDA */

		break;
	default:
		proc = false;

		break;
	}
	if(proc && ref->count)
		_unref(ref, data);

_exit:
	if(proc)
		result = _OP_RESULT_DEL_NODE;

	return result;
}
#endif /* MB_ENABLE_CLASS */

#ifdef MB_ENABLE_LAMBDA
/* Collect an outer scope */
static int _gc_destroy_garbage_outer_scope(void* data, void* extra, _gc_t* gc) {
	int result = _OP_RESULT_NORMAL;
	_ref_t* ref = 0;
	bool_t proc = true;
	_running_context_ref_t* outs = 0;

	mb_assert(data && extra);

	ref = (_ref_t*)extra;
	if(_ht_find(gc->collected_table, ref)) {
		proc = true;

		goto _exit;
	}
	switch(ref->type) {
	case _DT_OUTER_SCOPE:
		outs = (_running_context_ref_t*)data;
		if(!_ht_find(gc->collected_table, &outs->ref))
			_gc_destroy_garbage_in_outer_scope(outs, gc);

		break;
	default:
		proc = false;

		break;
	}
	if(proc && ref->count)
		_unref(ref, data);

_exit:
	if(proc)
		result = _OP_RESULT_DEL_NODE;

	return result;
}
#endif /* MB_ENABLE_LAMBDA */

/* Swap active garbage table and recursive table */
static void _gc_swap_tables(mb_interpreter_t* s) {
	_ht_node_t* tmp = 0;

	mb_assert(s);

	tmp = s->gc.table;
	s->gc.table = s->gc.recursive_table;
	s->gc.recursive_table = tmp;
}

/* Try trigger garbage collection */
static void _gc_try_trigger(mb_interpreter_t* s) {
	if(!_GCNOW(s))
		return;

	if(_ht_count(s->gc.table) >= MB_GC_GARBAGE_THRESHOLD)
		_gc_collect_garbage(s, 1);
}

/* Collect all garbage */
static void _gc_collect_garbage(mb_interpreter_t* s, int depth) {
#ifdef MB_ENABLE_FORK
	mb_interpreter_t* src = 0;
#endif /* MB_ENABLE_FORK */
	_ht_node_t* valid = 0;
	_gc_t* gc = 0;

	mb_assert(s);

#ifdef MB_ENABLE_FORK
	while(mb_get_forked_from(s, &src) == MB_FUNC_OK)
		s = src;
#endif /* MB_ENABLE_FORK */

	gc = &s->gc;

	/* Check whether it's paused */
	if(gc->disabled)
		return;

	/* Prepare for GC */
	if(gc->collecting)
		return;
	gc->collecting++;
	_PREPAREGC(s, gc);

	/* Get reachable information */
	valid = _ht_create(0, _ht_cmp_ref, _ht_hash_ref, _do_nothing_on_object);
	if(depth != -1)
		gc->valid_table = valid;
	_gc_get_reachable(s, valid, 0);
#ifdef MB_ENABLE_FORK
	if(s->all_forked) {
		_LS_FOREACH(s->all_forked, _do_nothing_on_object, _gc_get_reachable_in_forked, valid);
	}
#endif /* MB_ENABLE_FORK */
	if(s->alive_check_handler)
		s->alive_check_handler(s, valid, _gc_alive_marker);

	/* Get unreachable information */
	_HT_FOREACH(valid, _do_nothing_on_object, _ht_remove_existing, gc->table);
	_HT_FOREACH(valid, _do_nothing_on_object, _ht_remove_existing, gc->recursive_table);

	/* Collect garbage */
	_PREVGC(s, gc);
	do {
#ifdef MB_ENABLE_CLASS
		_HT_FOREACH(gc->table, _do_nothing_on_object, _gc_destroy_garbage_class, &s->gc);
#endif /* MB_ENABLE_CLASS */
		_HT_FOREACH(gc->table, _do_nothing_on_object, _gc_destroy_garbage, &s->gc);
#ifdef MB_ENABLE_LAMBDA
		_HT_FOREACH(gc->table, _do_nothing_on_object, _gc_destroy_garbage_outer_scope, &s->gc);
#endif /* MB_ENABLE_LAMBDA */
		_ht_clear(gc->table);
		if(gc->collecting > 1)
			gc->collecting--;

		if(!depth || !_ht_count(gc->recursive_table))
			break;

		_gc_swap_tables(s);
		gc->collecting++;
	} while(1);
	_POSTGC(s, gc);

	/* Tidy */
	_ht_clear(gc->collected_table);
	gc->valid_table = 0;
	_ht_clear(valid);
	_ht_destroy(valid);
	gc->collecting--;
	mb_assert(!gc->collecting);
}

#ifdef MB_ENABLE_USERTYPE_REF
/* Create a referenced usertype */
static _usertype_ref_t* _create_usertype_ref(mb_interpreter_t* s, void* val, mb_dtor_func_t un, mb_clone_func_t cl, mb_hash_func_t hs, mb_cmp_func_t cp, mb_fmt_func_t ft) {
	_usertype_ref_t* result = 0;

	mb_assert(s);

	result = (_usertype_ref_t*)mb_malloc(sizeof(_usertype_ref_t));
	memset(result, 0, sizeof(_usertype_ref_t));
	result->usertype = val;
	result->dtor = un;
	result->clone = cl;
	result->hash = hs;
	result->cmp = cp;
	result->fmt = ft;
	_create_ref(&result->ref, _unref_usertype_ref, _DT_USERTYPE_REF, s);

	return result;
}

/* Destroy a referenced usertype */
static void _destroy_usertype_ref(_usertype_ref_t* c) {
	mb_assert(c);

	if(c->dtor)
		c->dtor(c->ref.s, c->usertype);
	if(c->calc_operators) {
		safe_free(c->calc_operators);
	}
	_destroy_ref(&c->ref);
	safe_free(c);
}

/* Unreference a referenced usertype */
static void _unref_usertype_ref(_ref_t* ref, void* data) {
	mb_assert(ref);

	if(*ref->count == _NONE_REF)
		_destroy_usertype_ref((_usertype_ref_t*)data);
}

/* Clone a referenced usertype to a target object */
static void _clone_usertype_ref(_usertype_ref_t* src, _object_t* tgt) {
	void* cpy = 0;

	assert(src && tgt);

	_MAKE_NIL(tgt);

	if(!src->clone)
		return;
	cpy = src->clone(src->ref.s, src->usertype);
	if(!cpy)
		return;
	tgt->type = _DT_USERTYPE_REF;
	tgt->data.usertype_ref = _create_usertype_ref(
		src->ref.s, cpy,
		src->dtor, src->clone, src->hash, src->cmp, src->fmt
	);
#ifdef MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF
	tgt->data.usertype_ref->alive_checker = src->alive_checker;
#endif /* MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF */
	if(src->calc_operators) {
		tgt->data.usertype_ref->calc_operators = (_calculation_operator_info_t*)mb_malloc(sizeof(_calculation_operator_info_t));
		memcpy(tgt->data.usertype_ref->calc_operators, src->calc_operators, sizeof(_calculation_operator_info_t));
	}
	tgt->data.usertype_ref->coll_func = src->coll_func;
	tgt->data.usertype_ref->generic_func = src->generic_func;
	_ref(&tgt->data.usertype_ref->ref, tgt->data.usertype_ref);
}

/* Try to call a registered function on a referenced usertype */
static bool_t _try_call_func_on_usertype_ref(mb_interpreter_t* s, _ls_node_t** ast, _object_t* obj, _ls_node_t* pathed, int* ret) {
	_object_t* tmp = (_object_t*)pathed->data;
	if(tmp && tmp->type == _DT_VAR && tmp->data.variable->data->type == _DT_USERTYPE_REF) {
		bool_t mod = false;
		_ls_node_t* fn = 0;
		mb_func_t func = 0;
		char* r = strrchr(obj->data.variable->name, '.');
		if(!r) return false;
		++r;
		fn = _find_func(s, r, &mod);
		if(fn && fn->data) {
#ifdef MB_ENABLE_MODULE
			if(mod) {
				_module_func_t* mp = (_module_func_t*)fn->data;
				func = (mb_func_t)(intptr_t)mp->func;
			} else {
				func = (mb_func_t)(intptr_t)fn->data;
			}
#else /* MB_ENABLE_MODULE */
			func = (mb_func_t)(intptr_t)fn->data;
#endif /* MB_ENABLE_MODULE */
			s->usertype_ref_ahead = (_object_t*)tmp->data.variable->data;
#ifdef MB_ENABLE_STACK_TRACE
			_ls_pushback(s->stack_frames, r);
#endif /* MB_ENABLE_STACK_TRACE */
			if(ret)
				*ret = (func)(s, (void**)ast);
			else
				(func)(s, (void**)ast);
#ifdef MB_ENABLE_STACK_TRACE
			_ls_popback(s->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */

			return true;
		}
	}

	return false;
}
#endif /* MB_ENABLE_USERTYPE_REF */

/* Create an array */
static _array_t* _create_array(mb_interpreter_t* s, const char* n, _data_e t) {
	_array_t* result = 0;

	mb_assert(s);

	result = (_array_t*)mb_malloc(sizeof(_array_t));
	memset(result, 0, sizeof(_array_t));
	result->type = t;
	result->name = (char*)n;
#ifdef MB_ENABLE_ARRAY_REF
	_create_ref(&result->ref, _unref_array, _DT_ARRAY, s);
	_ref(&result->ref, result);
#else /* MB_ENABLE_ARRAY_REF */
	mb_unrefvar(s);
#endif /* MB_ENABLE_ARRAY_REF */

	return result;
}

/* Destroy an array */
static void _destroy_array(_array_t* arr) {
	mb_assert(arr);

	_clear_array(arr);
	if(arr->name) {
		safe_free(arr->name);
	}
#ifndef MB_SIMPLE_ARRAY
	if(arr->types) {
		safe_free(arr->types);
	}
#endif /* MB_SIMPLE_ARRAY */
#ifdef MB_ENABLE_ARRAY_REF
	_destroy_ref(&arr->ref);
#endif /* MB_ENABLE_ARRAY_REF */
	safe_free(arr);
}

/* Initialize an array */
static void _init_array(_array_t* arr) {
	int elemsize = 0;

	mb_assert(arr);

#ifdef MB_SIMPLE_ARRAY
	elemsize = (int)_get_size_of(arr->type);
#else /* MB_SIMPLE_ARRAY */
	elemsize = (int)_get_size_of(_DT_UNKNOWN);
#endif /* MB_SIMPLE_ARRAY */
	mb_assert(arr->count > 0);
	mb_assert(!arr->raw);
	arr->raw = (void*)mb_malloc(elemsize * arr->count);
	if(arr->raw)
		memset(arr->raw, 0, elemsize * arr->count);
#ifndef MB_SIMPLE_ARRAY
	arr->types = (_data_e*)mb_malloc(sizeof(_data_e) * arr->count);
	if(arr->types) {
		unsigned ul = 0;
		for(ul = 0; ul < arr->count; ++ul)
			arr->types[ul] = _DT_INT;
	}
#endif /* MB_SIMPLE_ARRAY */
}

/* Clone an array */
static _array_t* _clone_array(mb_interpreter_t* s, _array_t* arr) {
	_array_t* result = 0;
	unsigned index = 0;
	mb_value_u val;
	_data_e type = _DT_NIL;

	mb_assert(s && arr);

	result = _create_array(s, mb_strdup(arr->name, 0), arr->type);
	result->count = arr->count;
	result->dimension_count = arr->dimension_count;
	memcpy(result->dimensions, arr->dimensions, sizeof(result->dimensions));
	_init_array(result);
	for(index = 0; index < arr->count; index++) {
		_get_array_elem(s, arr, index, &val, &type);
		_set_array_elem(s, 0, result, index, &val, &type);
	}

	return result;
}

/* Calculate the true index of an array */
static int _get_array_pos(mb_interpreter_t* s, _array_t* arr, int* d, int c) {
	int result = 0;
	int i = 0;
	unsigned n = 0;
	int f = 1;

	mb_assert(s && arr && d);

	if(c < 0 || c > arr->dimension_count) {
		result = -1;

		goto _exit;
	}
	for(i = 0; i < c; i++) {
		n = d[i];
		if(n >= arr->dimensions[i]) {
			result = -1;

			goto _exit;
		}
		result += n * f;
		f *= arr->dimensions[i];
	}

_exit:
	return result;
}

/* Calculate the true index of an array, used when walking through an AST */
static int _get_array_index(mb_interpreter_t* s, _ls_node_t** l, _object_t* c, unsigned* index, bool_t* literally) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* arr = 0;
	_object_t* len = 0;
	_object_t subscript;
	_object_t* subscript_ptr = 0;
	mb_value_u val;
	int dcount = 0;
	int f = 1;
	unsigned idx = 0;

	mb_assert(s && l && index);

	subscript_ptr = &subscript;
	_MAKE_NIL(subscript_ptr);

	if(literally) *literally = false;

	/* Array name */
	ast = *l;
	if(!c && ast && _is_array(ast->data))
		c = (_object_t*)ast->data;
	if(!_is_array(c)) {
		_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	if(((_object_t*)c)->type == _DT_ARRAY)
		arr = (_object_t*)c;
	else
		arr = ((_object_t*)c)->data.variable->data;
	/* = */
	if(literally && ast->next && _IS_FUNC((_object_t*)ast->next->data, _core_equal)) {
		*literally = true;

		goto _exit;
	}
	/* ( */
	if(!ast->next || ((_object_t*)ast->next->data)->type != _DT_FUNC || ((_object_t*)ast->next->data)->data.func->pointer != _core_open_bracket) {
		_handle_error_on_obj(
			s, SE_RN_OPEN_BRACKET_EXPECTED, s->source_file,
			(ast && ast->next) ? ((_object_t*)ast->next->data) : 0,
			MB_FUNC_ERR, _exit, result
		);
	}
	ast = ast->next;
	/* Array subscript */
	if(!ast->next) {
		_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	while(((_object_t*)ast->data)->type != _DT_FUNC || ((_object_t*)ast->data)->data.func->pointer != _core_close_bracket) {
		/* Calculate an integer value */
		result = _calc_expression(s, &ast, &subscript_ptr);
		if(result != MB_FUNC_OK)
			goto _exit;
		len = subscript_ptr;
		if(!_try_get_value(len, &val, _DT_INT)) {
			_handle_error_on_obj(s, SE_RN_UNEXPECTED_TYPE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if(val.integer < 0) {
			_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if(dcount + 1 > arr->data.array->dimension_count) {
			_handle_error_on_obj(s, SE_RN_TOO_MANY_DIMENSIONS, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if((unsigned)val.integer >= arr->data.array->dimensions[dcount]) {
			_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		idx += (unsigned)val.integer * f;
		/* Comma? */
		if(_IS_SEP(ast->data, ','))
			ast = ast->next;

		f *= arr->data.array->dimensions[dcount];
		++dcount;
	}
	*index = idx;
	if(!arr->data.array->raw) {
		_handle_error_on_obj(s, SE_RN_RANK_OUT_OF_BOUND, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

_exit:
	*l = ast;

	return result;
}

/* Get the value of an element in an array */
static bool_t _get_array_elem(mb_interpreter_t* s, _array_t* arr, unsigned index, mb_value_u* val, _data_e* type) {
	bool_t result = true;
	int_t elemsize = 0;
	unsigned pos = 0;
	void* rawptr = 0;

	mb_assert(s && arr && val && type);
	mb_assert(index < arr->count);

	elemsize = _get_size_of(arr->type);
	pos = (unsigned)(elemsize * index);
	rawptr = (void*)((intptr_t)arr->raw + pos);
	if(arr->type == _DT_REAL) {
#ifdef MB_SIMPLE_ARRAY
		val->float_point = *((real_t*)rawptr);
		*type = _DT_REAL;
#else /* MB_SIMPLE_ARRAY */
		_copy_bytes(val->bytes, *((mb_val_bytes_t*)rawptr));
		*type = arr->types[index];
#endif /* MB_SIMPLE_ARRAY */
	} else if(arr->type == _DT_STRING) {
		val->string = *((char**)rawptr);
		*type = _DT_STRING;
	} else {
		mb_assert(0 && "Unsupported.");
	}

	return result;
}

/* Set the value of an element in an array */
static int _set_array_elem(mb_interpreter_t* s, _ls_node_t* ast, _array_t* arr, unsigned index, mb_value_u* val, _data_e* type) {
	int result = MB_FUNC_OK;
	int_t elemsize = 0;
	unsigned pos = 0;
	void* rawptr = 0;
	mb_unrefvar(ast);

	mb_assert(s && arr && val);
	mb_assert(index < arr->count);

	elemsize = _get_size_of(arr->type);
	pos = (unsigned)(elemsize * index);
	rawptr = (void*)((intptr_t)arr->raw + pos);
#ifdef MB_SIMPLE_ARRAY
	switch(*type) {
	case _DT_INT:
		*((real_t*)rawptr) = (real_t)val->integer;

		break;
	case _DT_REAL:
		*((real_t*)rawptr) = val->float_point;

		break;
	case _DT_STRING: {
			size_t _sl = 0;
			if(arr->type != _DT_STRING) {
				_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
			}
			_sl = strlen(val->string);
			*((char**)rawptr) = (char*)mb_malloc(_sl + 1);
			memcpy(*((char**)rawptr), val->string, _sl + 1);
		}

		break;
	default:
		result = MB_FUNC_ERR;

		break;
	}
#else /* MB_SIMPLE_ARRAY */
	switch(*type) {
	case _DT_STRING: {
			size_t _sl = 0;
			_sl = strlen(val->string);
			*((char**)rawptr) = (char*)mb_malloc(_sl + 1);
			memcpy(*((char**)rawptr), val->string, _sl + 1);
			arr->types[index] = _DT_STRING;
		}

		break;
	default:
		_copy_bytes(*((mb_val_bytes_t*)rawptr), val->bytes);
		arr->types[index] = *type;

		break;
	}
#endif /* MB_SIMPLE_ARRAY */

	goto _exit; /* Avoid an unreferenced label warning */

_exit:
	return result;
}

/* Clear an array */
static void _clear_array(_array_t* arr) {
	char* str = 0;
	int_t elemsize = 0;
	unsigned pos = 0;
	void* rawptr = 0;
	unsigned ul = 0;

	mb_assert(arr);

	if(arr->raw) {
#ifndef MB_SIMPLE_ARRAY
		if(arr->type == _DT_REAL) {
			for(ul = 0; ul < arr->count; ++ul) {
				elemsize = _get_size_of(arr->type);
				pos = (unsigned)(elemsize * ul);
				rawptr = (void*)((intptr_t)arr->raw + pos);
				if(arr->types[ul] == _DT_STRING) {
					str = *((char**)rawptr);
					if(str) {
						safe_free(str);
					}
				} else {
					_object_t obj;
					obj.type = arr->types[ul];
					_copy_bytes(obj.data.bytes, rawptr);
					_dispose_object(&obj);
				}
			}
		}
#endif /* MB_SIMPLE_ARRAY */
		if(arr->type == _DT_STRING) {
			for(ul = 0; ul < arr->count; ++ul) {
				elemsize = _get_size_of(arr->type);
				pos = (unsigned)(elemsize * ul);
				rawptr = (void*)((intptr_t)arr->raw + pos);
				str = *((char**)rawptr);
				if(str) {
					safe_free(str);
				}
			}
		}
		safe_free(arr->raw);
	}
}

/* Determine if an object is an array value or an array variable */
static bool_t _is_array(void* obj) {
	bool_t result = false;
	_object_t* o = 0;

	o = (_object_t*)obj;
	if(o && o->type == _DT_ARRAY)
		result = true;
	else if(o && o->type == _DT_VAR)
		result = o->data.variable->data->type == _DT_ARRAY;

	return result;
}

#ifdef MB_ENABLE_ARRAY_REF
/* Unreference an array */
static void _unref_array(_ref_t* ref, void* data) {
	mb_assert(ref);

	if(*ref->count == _NONE_REF)
		_destroy_array((_array_t*)data);
}
#endif /* MB_ENABLE_ARRAY_REF */

#ifdef MB_ENABLE_COLLECTION_LIB
/* Create a list */
static _list_t* _create_list(mb_interpreter_t* s) {
	_list_t* result = 0;

	mb_assert(s);

	result = (_list_t*)mb_malloc(sizeof(_list_t));
	memset(result, 0, sizeof(_list_t));
	result->list = _ls_create();
	_create_ref(&result->ref, _unref_list, _DT_LIST, s);

	return result;
}

/* Destroy a list */
static void _destroy_list(_list_t* c) {
	mb_assert(c);

	if(c->range_begin) { safe_free(c->range_begin); }
	_ls_foreach(c->list, _destroy_object);
	_ls_destroy(c->list);
	_destroy_ref(&c->ref);
	safe_free(c);
}

/* Create a dictionary */
static _dict_t* _create_dict(mb_interpreter_t* s) {
	_dict_t* result = 0;

	mb_assert(s);

	result = (_dict_t*)mb_malloc(sizeof(_dict_t));
	memset(result, 0, sizeof(_dict_t));
	result->dict = _ht_create(0, _ht_cmp_object, _ht_hash_object, _destroy_object_with_extra);
	_create_ref(&result->ref, _unref_dict, _DT_DICT, s);

	return result;
}

/* Destroy a dictionary */
static void _destroy_dict(_dict_t* c) {
	mb_assert(c);

	_ht_foreach(c->dict, _destroy_object_with_extra);
	_ht_destroy(c->dict);
	_destroy_ref(&c->ref);
	safe_free(c);
}

/* Create an iterator of a list */
static _list_it_t* _create_list_it(_list_t* coll, bool_t lock) {
	_list_it_t* result = 0;

	mb_assert(coll);

	result = (_list_it_t*)mb_malloc(sizeof(_list_it_t));
	memset(result, 0, sizeof(_list_it_t));
	result->list = coll;
	result->locking = lock;
	if(coll->range_begin)
		result->curr.ranging = *coll->range_begin - sgn(coll->count);
	else
		result->curr.node = coll->list;
	if(lock)
		_lock_ref_object(&coll->lock, &coll->ref, coll);
	_weak_ref(&coll->ref, coll, &result->weak_ref);

	return result;
}

/* Destroy an iterator of a list */
static bool_t _destroy_list_it(_list_it_t* it) {
	bool_t result = true;

	mb_assert(it);

	if(_weak_unref(&it->weak_ref))
		_unlock_ref_object(&it->list->lock, &it->list->ref, it->list);
	safe_free(it);

	return result;
}

/* Move an iterator of a list to next step */
static _list_it_t* _move_list_it_next(_list_it_t* it) {
	_list_it_t* result = 0;

	if(!it || !it->list || !it->list->list)
		goto _exit;

	if(it->list->lock < 0) { /* The iterator goes invalid if collection has been changed after obtaining iterator */
		result = it;

		goto _exit;
	}

	if(!it->curr.node && !it->list->range_begin)
		goto _exit;

	if(it->list->range_begin) {
		if(it->list->lock)
			it->curr.ranging += sgn(it->list->count);

		if(it->list->count > 0 && it->curr.ranging < *it->list->range_begin + it->list->count)
			result = it;
		else if(it->list->count < 0 && it->curr.ranging > *it->list->range_begin + it->list->count)
			result = it;
	} else {
		if(it->list->lock)
			it->curr.node = it->curr.node->next;

		if(it->curr.node)
			result = it;
	}

_exit:
	return result;
}

/* Create an iterator of a dictionary */
static _dict_it_t* _create_dict_it(_dict_t* coll, bool_t lock) {
	_dict_it_t* result = 0;

	mb_assert(coll);

	result = (_dict_it_t*)mb_malloc(sizeof(_dict_it_t));
	memset(result, 0, sizeof(_dict_it_t));
	result->dict = coll;
	result->locking = lock;
	result->curr_bucket = 0;
	result->curr_node = _INVALID_DICT_IT;
	if(lock)
		_lock_ref_object(&coll->lock, &coll->ref, coll);
	_weak_ref(&coll->ref, coll, &result->weak_ref);

	return result;
}

/* Destroy an iterator of a dictionary */
static bool_t _destroy_dict_it(_dict_it_t* it) {
	bool_t result = true;

	mb_assert(it);

	if(_weak_unref(&it->weak_ref))
		_unlock_ref_object(&it->dict->lock, &it->dict->ref, it->dict);
	safe_free(it);

	return result;
}

/* Move an iterator of a dictionary to next step */
static _dict_it_t* _move_dict_it_next(_dict_it_t* it) {
	_dict_it_t* result = 0;

	if(!it || !it->dict || !it->dict->dict || !it->curr_node)
		goto _exit;

	if(!it->dict->lock)
		goto _exit;

	if(it->curr_node && it->curr_node != _INVALID_DICT_IT) {
		it->curr_node = it->curr_node->next;
		if(!it->curr_node)
			++it->curr_bucket;
	}
	if(!it->curr_node || it->curr_node == _INVALID_DICT_IT) {
		if(!it->dict->dict->array)
			goto _exit;
		for( ; it->curr_bucket < it->dict->dict->array_size; ++it->curr_bucket) {
			it->curr_node = it->dict->dict->array[it->curr_bucket];
			if(it->curr_node && it->curr_node->next) {
				it->curr_node = it->curr_node->next;

				break;
			}
		}
	}

	if(it->curr_node && it->curr_node->extra)
		result = it;

_exit:
	return result;
}

/* Unreference a list */
static void _unref_list(_ref_t* ref, void* data) {
	mb_assert(ref);

	if(*ref->count == _NONE_REF)
		_destroy_list((_list_t*)data);
}

/* Unreference a dictionary */
static void _unref_dict(_ref_t* ref, void* data) {
	mb_assert(ref);

	if(*ref->count == _NONE_REF)
		_destroy_dict((_dict_t*)data);
}

/* Push a value to a list */
static bool_t _push_list(_list_t* coll, mb_value_t* val, _object_t* oarg) {
	mb_assert(coll && (val || oarg));

	if(_try_purge_it(coll->ref.s, val, oarg))
		return false;

	_fill_ranged(coll);

	if(val && !oarg)
		_create_internal_object_from_public_value(val, &oarg);
	_COLL_ROUTINE(oarg);
	_ls_pushback(coll->list, oarg);
	coll->count++;

	_write_on_ref_object(&coll->lock, &coll->ref, coll);
	_invalidate_list_cache(coll);

	return true;
}

/* Pop a value from a list */
static bool_t _pop_list(_list_t* coll, mb_value_t* val, mb_interpreter_t* s) {
	_object_t* oval = 0;

	mb_assert(coll && val && s);

	_fill_ranged(coll);

	oval = (_object_t*)_ls_popback(coll->list);
	if(oval) {
		_internal_object_to_public_value(oval, val);
		_destroy_object_capsule_only(oval, 0);
		coll->count--;

		if(val->type == MB_DT_STRING)
			_mark_lazy_destroy_string(s, val->value.string);

		_write_on_ref_object(&coll->lock, &coll->ref, coll);
		_invalidate_list_cache(coll);

		return true;
	} else {
		mb_make_nil(*val);

		return false;
	}
}

/* Insert a value into a list */
static bool_t _insert_list(_list_t* coll, int_t idx, mb_value_t* val, _object_t** oval) {
	_object_t* oarg = 0;

	mb_assert(coll && val);

	if(_try_purge_it(coll->ref.s, val, oval ? *oval : 0))
		return false;

	_fill_ranged(coll);

	_create_internal_object_from_public_value(val, &oarg);
	_COLL_ROUTINE(oarg);
	if(oval)
		*oval = oarg;

	if(_ls_insert_at(coll->list, (int)idx, oarg)) {
		coll->count++;
		_write_on_ref_object(&coll->lock, &coll->ref, coll);
		_invalidate_list_cache(coll);

		return true;
	}

	return false;
}

/* Set an element in a list with a specific index with a given value */
static bool_t _set_list(_list_t* coll, int_t idx, mb_value_t* val, _object_t** oval) {
	_ls_node_t* result = 0;
	_object_t* oarg = 0;

	mb_assert(coll && (val || (oval && *oval)));

	if(_try_purge_it(coll->ref.s, val, oval ? *oval : 0))
		return false;

	_fill_ranged(coll);

	result = _node_at_list(coll, (int)idx);
	if(result) {
		if(result->data)
			_destroy_object(result->data, 0);
		if(val) {
			_create_internal_object_from_public_value(val, &oarg);
			_COLL_ROUTINE(oarg);
			if(oval)
				*oval = oarg;
		} else {
			oarg = *oval;
		}
		result->data = oarg;

		_write_on_ref_object(&coll->lock, &coll->ref, coll);
		_invalidate_list_cache(coll);
	}

	return !!(result && result->data);
}

/* Remove an element in a list with a specific index */
static bool_t _remove_at_list(_list_t* coll, int_t idx) {
	bool_t result = false;
	_ls_node_t* node = 0;

	mb_assert(coll);

	_fill_ranged(coll);

	node = _node_at_list(coll, (int)idx);
	if(node) {
		if(node->data) {
			_ls_remove(coll->list, node, _destroy_object);
			coll->count--;

			_write_on_ref_object(&coll->lock, &coll->ref, coll);
			_invalidate_list_cache(coll);

			result = true;
		}
	}

	return result;
}

/* Get a node in a list with a specific index */
static _ls_node_t* _node_at_list(_list_t* coll, int index) {
	_ls_node_t* result = 0;
	_ls_node_t* tmp = 0;
	int n = 0;

	mb_assert(coll);

	_fill_ranged(coll);

	if(index >= 0 && index < (int)coll->count) {
		/* Position: HEAD ... LEFT ... PIVOT ... RIGHT ... TAIL
			PIVOT is a cached node and,
			LEN(HEAD to LEFT) == LEN(LEFT to PIVOT) && LEN(PIVOT to RIGHT) == LEN(RIGHT to TAIL)
		 */
		int head = 0,
			left = coll->cached_index / 2,
			right = coll->cached_index + (coll->count - coll->cached_index) / 2,
			tail = coll->count - 1;
		if(coll->cached_node) {
			if(index >= head && index < left) { /* [HEAD, LEFT) */
				n = index;
				tmp = coll->list->next;
				while(tmp && n) {
					tmp = tmp->next;
					--n;
				}
				if(tmp) {
					result = tmp;
					coll->cached_node = tmp;
					coll->cached_index = index;
				}
			} else if(index >= left && index <= right) { /* [LEFT, RIGHT] */
				while(index != coll->cached_index) {
					if(index > coll->cached_index) {
						coll->cached_node = coll->cached_node->next;
						coll->cached_index++;
					} else if(index < coll->cached_index) {
						coll->cached_node = coll->cached_node->prev;
						coll->cached_index--;
					}
				}
				result = coll->cached_node;
			} else if(index > right && index <= tail) { /* (RIGHT, TAIL] */
				n = tail - index;
				tmp = coll->list->prev;
				while(tmp && n) {
					tmp = tmp->prev;
					--n;
				}
				if(tmp) {
					result = tmp;
					coll->cached_node = tmp;
					coll->cached_index = index;
				}
			} else {
				mb_assert(0 && "Impossible.");
			}
		} else {
			n = index;
			tmp = coll->list->next;
			while(tmp && n) {
				tmp = tmp->next;
				--n;
			}
			if(tmp) {
				result = tmp;
				coll->cached_node = tmp;
				coll->cached_index = index;
			}
		}
	}

	return result;
}

/* Get the value in a list with a specific index */
static bool_t _at_list(_list_t* coll, int_t idx, mb_value_t* oval) {
	_ls_node_t* result = 0;

	mb_assert(coll && oval);

	_fill_ranged(coll);

	result = _node_at_list(coll, (int)idx);
	if(oval && result && result->data)
		_internal_object_to_public_value((_object_t*)result->data, oval);

	return !!(result && result->data);
}

/* Find a value in a list */
static bool_t _find_list(_list_t* coll, mb_value_t* val, int* idx) {
	bool_t result = false;
	_object_t* oarg = 0;

	mb_assert(coll && val);

	_fill_ranged(coll);

	_create_internal_object_from_public_value(val, &oarg);
	result = !!_ls_find(coll->list, oarg, (_ls_compare_t)_ht_cmp_object, idx);
	_destroy_object(oarg, 0);

	return result;
}

/* Clear a list */
static void _clear_list(_list_t* coll) {
	mb_assert(coll);

	if(coll->range_begin) { safe_free(coll->range_begin); }

	_ls_foreach(coll->list, _destroy_object);
	_ls_clear(coll->list);
	coll->count = 0;

	_write_on_ref_object(&coll->lock, &coll->ref, coll);
	_invalidate_list_cache(coll);
}

/* Sort a list */
static void _sort_list(_list_t* coll) {
	mb_assert(coll);

	_ls_sort(&coll->list, (_ls_compare_t)_ht_cmp_object);

	_write_on_ref_object(&coll->lock, &coll->ref, coll);
	_invalidate_list_cache(coll);
}

/* Invalidate cached list index */
static void _invalidate_list_cache(_list_t* coll) {
	mb_assert(coll);

	coll->cached_node = 0;
	coll->cached_index = 0;
}

/* Fill a ranged list with numbers */
static void _fill_ranged(_list_t* coll) {
	_object_t* obj = 0;

	mb_assert(coll);

	if(coll->range_begin) {
		mb_value_t arg;
		int_t begin = *coll->range_begin;
		int_t end = *coll->range_begin + coll->count;
		int_t step = sgn(coll->count);

		do {
			mb_make_int(arg, begin);
			_create_internal_object_from_public_value(&arg, &obj);
			_ls_pushback(coll->list, obj);

			begin += step;
		} while(begin != end);
		safe_free(coll->range_begin);

		_write_on_ref_object(&coll->lock, &coll->ref, coll);
		_invalidate_list_cache(coll);
	}
}

/* Set an element to a dictionary with a key-value pair */
static bool_t _set_dict(_dict_t* coll, mb_value_t* key, mb_value_t* val, _object_t* okey, _object_t* oval) {
	_ls_node_t* exists = 0;

	mb_assert(coll && (key || okey) && (val || oval));

	if(_try_purge_it(coll->ref.s, key, okey))
		return false;
	if(_try_purge_it(coll->ref.s, val, oval))
		return false;

	if(key && !okey)
		_create_internal_object_from_public_value(key, &okey);
	if(val && !oval)
		_create_internal_object_from_public_value(val, &oval);
	exists = _ht_find(coll->dict, okey);
	if(exists)
		_ht_remove(coll->dict, okey, _ls_cmp_extra_object);
	_COLL_ROUTINE(okey);
	_COLL_ROUTINE(oval);
	_ht_set_or_insert(coll->dict, okey, oval);

	_write_on_ref_object(&coll->lock, &coll->ref, coll);

	return true;
}

/* Remove an element to a dictionary with a specific key */
static bool_t _remove_dict(_dict_t* coll, mb_value_t* key) {
	_ls_node_t* result = 0;
	_object_t* okey = 0;

	mb_assert(coll && key);

	_create_internal_object_from_public_value(key, &okey);
	result = _ht_find(coll->dict, okey);
	if(result && result->data) {
		_ht_remove(coll->dict, okey, _ls_cmp_extra_object);
		_destroy_object(okey, 0);

		_write_on_ref_object(&coll->lock, &coll->ref, coll);

		return true;
	}

	return false;
}

/* Find a key in a dictionary */
static bool_t _find_dict(_dict_t* coll, mb_value_t* val, mb_value_t* oval) {
	_ls_node_t* result = 0;
	_object_t* oarg = 0;

	mb_assert(coll && val);

	if(val->type == MB_DT_LIST_IT || val->type == MB_DT_DICT_IT)
		return false;
	_create_internal_object_from_public_value(val, &oarg);
	result = _ht_find(coll->dict, oarg);
	_destroy_object(oarg, 0);
	if(oval) {
		if(result && result->data) {
			_internal_object_to_public_value((_object_t*)result->data, oval);
		} else {
			oval->type = MB_DT_UNKNOWN;
			oval->value.integer = 0;

			return true;
		}
	}

	return !!(result && result->data);
}

/* Clear a dictionary */
static void _clear_dict(_dict_t* coll) {
	mb_assert(coll);

	_ht_foreach(coll->dict, _destroy_object_with_extra);
	_ht_clear(coll->dict);

	_write_on_ref_object(&coll->lock, &coll->ref, coll);
}

/* Determin whether a list iterator is invalid */
static bool_t _invalid_list_it(_list_it_t* it) {
	if(!it)
		return false;

	return it && it->list && it->list->lock <= 0;
}

/* Determin whether a dictionary iterator is invalid */
static bool_t _invalid_dict_it(_dict_it_t* it) {
	if(!it)
		return false;

	return it && it->dict && it->dict->lock <= 0;
}

/* Assign an iterator to another object */
static bool_t _assign_with_it(_object_t* tgt, _object_t* src) {
	mb_assert(tgt && src);

	if(src->type != _DT_LIST_IT && src->type != _DT_DICT_IT)
		return false;

	switch(src->type) {
	case _DT_LIST_IT:
		if(src->data.list_it->locking) {
			tgt->data.list_it = _create_list_it(src->data.list_it->list, true);
		} else {
			tgt->data = src->data;
			tgt->data.list_it->locking = true;
			_lock_ref_object(&tgt->data.list_it->list->lock, &tgt->data.list_it->list->ref, tgt->data.list_it->list);
		}

		break;
	case _DT_DICT_IT:
		if(src->data.dict_it->locking) {
			tgt->data.dict_it = _create_dict_it(src->data.dict_it->dict, true);
		} else {
			tgt->data = src->data;
			tgt->data.dict_it->locking = true;
			_lock_ref_object(&tgt->data.dict_it->dict->lock, &tgt->data.dict_it->dict->ref, tgt->data.dict_it->dict);
		}

		break;
	default:
		mb_assert(0 && "Impossible.");

		break;
	}

	return true;
}

/* Try to purege an iterator */
static bool_t _try_purge_it(mb_interpreter_t* s, mb_value_t* val, _object_t* obj) {
	bool_t result = false;
	_object_t tmp;

	mb_assert(s && (val || obj));

	_MAKE_NIL(&tmp);
#ifdef MB_ENABLE_COLLECTION_LIB
	if(val) {
		switch(val->type) {
		case MB_DT_LIST_IT: /* Fall through */
		case MB_DT_DICT_IT:
			_public_value_to_internal_object(val, &tmp);
			obj = &tmp;

			break;
		default: /* Do nothing */
			break;
		}
	}
	if(obj) {
		if(obj->type == _DT_LIST_IT) {
			result = true;
			if(obj->data.list_it->locking)
				return result;

			_destroy_list_it(obj->data.list_it); /* Process dangling value */
		} else if(obj->type == _DT_DICT_IT) {
			result = true;
			if(obj->data.dict_it->locking)
				return result;

			_destroy_dict_it(obj->data.dict_it); /* Process dangling value */
		} else {
			return result;
		}
	}
#endif /* MB_ENABLE_COLLECTION_LIB */

	return result;
}

/* Clone an object to a list */
static int _clone_to_list(void* data, void* extra, _list_t* coll) {
	_object_t* obj = 0;
	_object_t* tgt = 0;
	mb_unrefvar(extra);

	mb_assert(data && coll);

	_fill_ranged(coll);

	tgt = _create_object();
	obj = (_object_t*)data;
	_clone_object(coll->ref.s, obj, tgt, false, false);
	_push_list(coll, 0, tgt);
	_REF(tgt)

	return 1;
}

/* Clone a key-value pair to a dictionary */
static int _clone_to_dict(void* data, void* extra, _dict_t* coll) {
	_object_t* kobj = 0;
	_object_t* ktgt = 0;
	_object_t* vobj = 0;
	_object_t* vtgt = 0;

	mb_assert(data && extra && coll);

	ktgt = _create_object();
	kobj = (_object_t*)extra;
	_clone_object(coll->ref.s, kobj, ktgt, false, false);

	vtgt = _create_object();
	vobj = (_object_t*)data;
	_clone_object(coll->ref.s, vobj, vtgt, false, false);

	_set_dict(coll, 0, 0, ktgt, vtgt);
	_REF(ktgt)
	_REF(vtgt)

	return 1;
}

/* Copy an object from a list to an array */
static int _copy_list_to_array(void* data, void* extra, _array_helper_t* h) {
	_object_t* obj = 0;
	mb_value_t val;
	_data_e type = _DT_NIL;
	mb_unrefvar(extra);

	mb_assert(data && h);

	obj = (_object_t*)data;
	mb_make_nil(val);
	_internal_object_to_public_value(obj, &val);
	type = obj->type;
	_set_array_elem(h->s, 0, h->array, h->index++, &val.value, &type);

	return 1;
}

/* Copy all keys of a dictionary to a value array */
static int _copy_keys_to_value_array(void* data, void* extra, _keys_helper_t* h) {
	_object_t* obj = 0;
	mb_value_t val;
	mb_unrefvar(data);

	mb_assert(extra && h);

	if(h->index >= h->size)
		return 1;

	obj = (_object_t*)extra;
	mb_make_nil(val);
	_internal_object_to_public_value(obj, &val);
	h->keys[h->index++] = val;

	return 1;
}
#endif /* MB_ENABLE_COLLECTION_LIB */

#ifdef MB_ENABLE_CLASS
/* Initialize a class */
static void _init_class(mb_interpreter_t* s, _class_t* instance, char* n) {
	_running_context_t* running = 0;
	_object_t* meobj = 0;
	_var_t* me = 0;

	mb_assert(s && instance && n);

	running = s->running_context;

	memset(instance, 0, sizeof(_class_t));
	_create_ref(&instance->ref, _unref_class, _DT_CLASS, s);
	_ref(&instance->ref, instance);
	instance->name = n;
	instance->meta_list = _ls_create();
	instance->scope = _create_running_context(true);
	instance->created_from = instance;

	me = _create_var(&meobj, _CLASS_ME, strlen(_CLASS_ME) + 1, true);
	me->data->type = _DT_CLASS;
	me->data->data.instance = instance;
	me->pathing = _PATHING_NORMAL;
	me->is_me = true;
	_ht_set_or_insert(instance->scope->var_dict, me->name, meobj);
}

/* Begin parsing a class */
static void _begin_class(mb_interpreter_t* s) {
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	context->class_state = _CLASS_STATE_PROC;
}

/* End parsing a class */
static bool_t _end_class(mb_interpreter_t* s) {
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	if(context->routine_state) {
		_handle_error_now(s, SE_RN_INVALID_ROUTINE, s->source_file, MB_FUNC_ERR);
	}
	if(context->class_state == _CLASS_STATE_NONE) {
		_handle_error_now(s, SE_RN_INVALID_CLASS, s->source_file, MB_FUNC_ERR);

		return false;
	}
	context->class_state = _CLASS_STATE_NONE;
	s->last_instance = 0;

	return true;
}

/* Unreference a class instance */
static void _unref_class(_ref_t* ref, void* data) {
	mb_assert(ref);

	if(ref->s->valid)
		_out_of_scope(ref->s, ((_class_t*)data)->scope, (_class_t*)data, 0, false);

	if(*ref->count == _NONE_REF)
		_destroy_class((_class_t*)data);
}

/* Destroy a class instance */
static void _destroy_class(_class_t* c) {
	mb_assert(c);

	if(c->meta_list) {
		_unlink_meta_class(c->ref.s, c);
		_ls_destroy(c->meta_list);
	}
	if(c->scope->var_dict) {
		_ht_foreach(c->scope->var_dict, _destroy_object);
		_ht_destroy(c->scope->var_dict);
	}
#ifdef MB_ENABLE_LAMBDA
	if(c->scope->refered_lambdas) {
		_ls_destroy(c->scope->refered_lambdas);
		c->scope->refered_lambdas = 0;
	}
#endif /* MB_ENABLE_LAMBDA */
	safe_free(c->scope);
	_destroy_ref(&c->ref);
	safe_free(c->name);
	safe_free(c);
}

/* Traverse all fields of a class instance, and its meta class instances recursively as well */
static bool_t _traverse_class(_class_t* c, _class_scope_walker_t scope_walker, _class_meta_walker_t meta_walker, unsigned meta_depth, bool_t meta_walk_on_self, void* extra_data, void* ret) {
	bool_t result = true;
	_ls_node_t* node = 0;
	_class_t* meta = 0;

	mb_assert(c);

	if(scope_walker) {
		_HT_FOREACH(c->scope->var_dict, _do_nothing_on_object, scope_walker, extra_data);
	}
	if(meta_walk_on_self) {
		if(meta_walker) {
			result = meta_walker(c, extra_data, ret);
			if(!result)
				goto _exit;
		}
	}
	node = c->meta_list ? c->meta_list->next : 0;
	while(node) {
		meta = (_class_t*)node->data;
		if(meta_walker && meta_depth) {
			result = meta_walker(meta, extra_data, ret);
			if(!result)
				break;
		}
		result = _traverse_class(
			meta,
			scope_walker,
			meta_walker, meta_depth ? meta_depth - 1 : 0,
			meta_walk_on_self,
			extra_data, ret
		);
		if(!result)
			break;
		node = node->next;
	}

_exit:
	return result;
}

/* Link a class instance to the meta list of another class instance */
static bool_t _link_meta_class(mb_interpreter_t* s, _class_t* derived, _class_t* base) {
	mb_assert(s && derived && base);

	if(_ls_find(derived->meta_list, base, (_ls_compare_t)_ht_cmp_intptr, 0)) {
		_handle_error_now(s, SE_RN_INVALID_CLASS, s->source_file, MB_FUNC_ERR);

		return false;
	}

	_ls_pushback(derived->meta_list, base);
	_ref(&base->ref, base);

	return true;
}

/* Unlink all meta class instances of a class instance */
static void _unlink_meta_class(mb_interpreter_t* s, _class_t* derived) {
	mb_assert(s && derived);

	_LS_FOREACH(derived->meta_list, _do_nothing_on_object, _unlink_meta_instance, derived);
	_ls_clear(derived->meta_list);
}

/* Unlink a meta class instance */
static int _unlink_meta_instance(void* data, void* extra, _class_t* derived) {
	_class_t* base = 0;
	mb_unrefvar(extra);

	mb_assert(data && derived);

	base = (_class_t*)data;
	_unref(&base->ref, base);

	return 0;
}

/* Clone fields of a class instance to another */
static int _clone_clsss_field(void* data, void* extra, void* n) {
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	_array_t* arr = 0;
	_var_t* var = 0;
	_routine_t* sub = 0;
	_class_t* instance = (_class_t*)n;
	_object_t* ret = 0;
	mb_unrefvar(extra);

	mb_assert(data && n);

	obj = (_object_t*)data;
	if(_is_internal_object(obj))
		goto _exit;
	switch(obj->type) {
	case _DT_VAR:
		var = obj->data.variable;
		if(!_IS_ME(var)) {
			if(_ht_find(instance->scope->var_dict, var->name))
				break;

			ret = _duplicate_parameter(var, 0, instance->scope);
			_clone_object(instance->ref.s, obj, ret->data.variable->data, false, var->data->type != _DT_CLASS);
#ifdef MB_ENABLE_SOURCE_TRACE
			ret->source_pos = -1;
			ret->source_row = ret->source_col = 0xFFFF;
#else /* MB_ENABLE_SOURCE_TRACE */
			ret->source_pos = -1;
#endif /* MB_ENABLE_SOURCE_TRACE */
		}

		break;
	case _DT_ARRAY:
		arr = obj->data.array;
		if(!_ht_find(instance->scope->var_dict, arr->name)) {
			ret = _create_object();
			ret->type = _DT_ARRAY;
			ret->is_ref = false;
			_clone_object(instance->ref.s, obj, ret, false, false);

			_ht_set_or_insert(instance->scope->var_dict, ret->data.array->name, ret);
		}

		break;
	case _DT_ROUTINE:
		sub = obj->data.routine;
		if(!_ht_find(instance->scope->var_dict, sub->name)) {
			_routine_t* routine = _clone_routine(sub, instance, false);
			ret = _create_object();
			ret->type = _DT_ROUTINE;
			ret->data.routine = routine;
			ret->is_ref = false;

			_ht_set_or_insert(instance->scope->var_dict, obj->data.routine->name, ret);
		}

		break;
	default: /* Do nothing */
		break;
	}

_exit:
	return result;
}

/* Link meta class to a new instance */
static bool_t _clone_class_meta_link(_class_t* meta, void* n, void* ret) {
	_class_t* instance = (_class_t*)n;
	mb_unrefvar(ret);

	mb_assert(meta && n);

	_link_meta_class(instance->ref.s, instance, meta);

	return true;
}

/* Search for a meta function with a specific name and assign to a member field */
static int _search_class_meta_function(mb_interpreter_t* s, _class_t* instance, const char* n, _routine_t* _UNALIGNED_ARG * f) {
	_ls_node_t* node = 0;

	mb_assert(s);

	node = _search_identifier_in_class(s, instance, n, 0, 0);
	if(f) *f = 0;
	if(node) {
		_object_t* obj = (_object_t*)node->data;
		if(obj && _IS_ROUTINE(obj)) {
			if(f) *f = obj->data.routine;

			return 1;
		}
	}

	return 0;
}

/* Search for the HASH and COMPARE meta function for a class */
static int _search_class_hash_and_compare_functions(mb_interpreter_t* s, _class_t* instance) {
	mb_assert(s && instance);

	_search_class_meta_function(s, instance, _CLASS_HASH_FUNC, &instance->hash);
	_search_class_meta_function(s, instance, _CLASS_COMPARE_FUNC, &instance->compare);

	if(!instance->hash && !instance->compare) {
		return MB_FUNC_OK;
	} else if(instance->hash && instance->compare) {
		return MB_FUNC_OK;
	} else {
		instance->hash = 0;
		instance->compare = 0;

		return MB_FUNC_WARNING;
	}
}

/* Detect whether a class instance is inherited from another */
static bool_t _is_a_class(_class_t* instance, void* m, void* ret) {
	_class_t* meta = (_class_t*)m;
	bool_t* r = (bool_t*)ret;
	bool_t is_a = false;

	mb_assert(instance && meta && ret);

	do {
		if(instance == meta) {
			is_a = true;

			break;
		}
		if(instance == instance->created_from)
			break;
		instance = instance->created_from;
	} while(1);

	*r = is_a;

	return !(*r);
}

/* Add a meta class instance to a GC reachable table */
static bool_t _add_class_meta_reachable(_class_t* meta, void* ht, void* ret) {
	_ht_node_t* htable = (_ht_node_t*)ht;
	mb_unrefvar(ret);

	mb_assert(meta && ht);

	if(!_ht_find(htable, &meta->ref))
		_ht_set_or_insert(htable, &meta->ref, meta);

	return true;
}

#ifdef MB_ENABLE_COLLECTION_LIB
/* Reflect each field of a class instance to a dictionary */
static int _reflect_class_field(void* data, void* extra, void* d) {
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	_var_t* var = 0;
	_routine_t* sub = 0;
	_dict_t* coll = (_dict_t*)d;
	_object_t tmp;
	mb_unrefvar(extra);

	mb_assert(data && d);

	_MAKE_NIL(&tmp);
	tmp.type = _DT_STRING;
	obj = (_object_t*)data;
	if(_is_internal_object(obj))
		goto _exit;
	switch(obj->type) {
	case _DT_VAR:
		var = (_var_t*)obj->data.variable;
		tmp.data.string = var->name;
		if(!_ht_find(coll->dict, &tmp)) {
			mb_value_t key, val;
			mb_make_string(key, var->name);
			_internal_object_to_public_value(obj, &val);
			_set_dict(coll, &key, &val, 0, 0);
		}

		break;
	case _DT_ROUTINE:
		sub = (_routine_t*)obj->data.routine;
		tmp.data.string = sub->name;
		if(!_ht_find(coll->dict, &tmp)) {
			mb_value_t key, val;
			mb_make_string(key, sub->name);
			mb_make_type(val, _internal_type_to_public_type(obj->type));
			_set_dict(coll, &key, &val, 0, 0);
		}

		break;
	default: /* Do nothing */
		break;
	}

_exit:
	return result;
}
#endif /* MB_ENABLE_COLLECTION_LIB */

/* Call the TOSTRING function of a class instance to format it */
static int _format_class_to_string(mb_interpreter_t* s, void** l, _class_t* instance, _object_t* out, bool_t* got_tostr) {
	int result = MB_FUNC_OK;
	_ls_node_t* tsn = 0;

	mb_assert(s && l && instance && out);

	tsn = _search_identifier_in_class(s, instance, _CLASS_TO_STRING_FUNC, 0, 0);
	if(got_tostr) *got_tostr = false;
	if(tsn) {
		_object_t* tso = (_object_t*)tsn->data;
		_ls_node_t* tmp = (_ls_node_t*)*l;
		mb_value_t va[1];
		mb_make_nil(va[0]);
		if(_eval_routine(s, &tmp, va, 1, tso->data.routine, _has_routine_fun_arg, _pop_routine_fun_arg) == MB_FUNC_OK) {
			_MAKE_NIL(out);
			_public_value_to_internal_object(&s->running_context->intermediate_value, out);
			if(out->type == _DT_STRING) {
				out->data.string = mb_strdup(out->data.string, strlen(out->data.string) + 1);
				out->is_ref = false;
				mb_make_nil(s->running_context->intermediate_value);
			} else {
				_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}
			if(got_tostr)
				*got_tostr = true;
		}
	}

_exit:
	return result;
}

/* Reflect a class instance from a string */
static _class_t* _reflect_string_to_class(mb_interpreter_t* s, const char* n, mb_value_t* arg) {
	_ls_node_t* cs = 0;
	_object_t* c = 0;

	cs = _search_identifier_in_scope_chain(s, 0, n, _PATHING_NONE, 0, 0);
	if(!cs)
		return 0;
	c = (_object_t*)cs->data;
	if(!c)
		return 0;
	c = _GET_CLASS(c);
	if(!c)
		return 0;
	if(arg)
		_internal_object_to_public_value(c, arg);

	return c->data.instance;
}

/* Detect whether it's accessing a member of a class instance following a sub routine */
static bool_t _is_valid_class_accessor_following_routine(mb_interpreter_t* s, _var_t* var, _ls_node_t* ast, _ls_node_t** out) {
	bool_t result = false;
	_running_context_t* running = 0;

	mb_assert(s && var && ast);

	running = s->running_context;

	if(out) *out = 0;

	if(_is_accessor_char(*var->name) && (ast && ast->prev && _IS_FUNC(ast->prev->data, _core_close_bracket)) && running->intermediate_value.type == MB_DT_CLASS) {
		_class_t* instance = (_class_t*)running->intermediate_value.value.instance;
		_ls_node_t* fn = _search_identifier_in_class(s, instance, var->name + 1, 0, 0);
		result = true;
		if(fn && out)
			*out = fn;
	}

	return result;
}
#endif /* MB_ENABLE_CLASS */

/* Initialize a routine */
static void _init_routine(mb_interpreter_t* s, _routine_t* routine, char* n, mb_routine_func_t f) {
	_running_context_t* running = 0;

	mb_assert(s && routine);

	running = s->running_context;

	memset(routine, 0, sizeof(_routine_t));
	routine->name = n;

	if(n && f)
		routine->type = MB_RT_NATIVE;
	else if(n && !f)
		routine->type = MB_RT_SCRIPT;
#ifdef MB_ENABLE_LAMBDA
	else if(!n && !f)
		routine->type = MB_RT_LAMBDA;
#endif /* MB_ENABLE_LAMBDA */

	switch(routine->type) {
	case MB_RT_SCRIPT:
		routine->func.basic.scope = _create_running_context(true);

		break;
#ifdef MB_ENABLE_LAMBDA
	case MB_RT_LAMBDA:
		_create_ref(&routine->func.lambda.ref, _unref_routine, _DT_ROUTINE, s);
		_ref(&routine->func.lambda.ref, routine);

		break;
#endif /* MB_ENABLE_LAMBDA */
	case MB_RT_NATIVE:
		routine->func.native.entry = f;

		break;
	default: /* Do nothing */
		break;
	}

#ifdef MB_ENABLE_SOURCE_TRACE
	if(s->source_file)
		routine->source_file = mb_strdup(s->source_file, 0);
#endif /* MB_ENABLE_SOURCE_TRACE */
}

/* Begin parsing a routine */
static int _begin_routine(mb_interpreter_t* s) {
	int result = MB_FUNC_OK;
	_parsing_context_t* context = 0;
	unsigned short before = 0;

	mb_assert(s);

	context = s->parsing_context;
	before = context->routine_state++;
	if(before > context->routine_state) {
		context->routine_state--;
		result = MB_FUNC_ERR;
		_handle_error_now(s, SE_RN_INVALID_ROUTINE, s->last_error_file, result);
	}

	return result;
}

/* End parsing a routine */
static bool_t _end_routine(mb_interpreter_t* s) {
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	if(!context->routine_state) {
		_handle_error_now(s, SE_RN_INVALID_ROUTINE, s->source_file, MB_FUNC_ERR);

		return false;
	}
	context->routine_state--;

	return true;
}

/* Begin parsing the definition of a routine */
static void _begin_routine_definition(mb_interpreter_t* s) {
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	context->routine_params_state = _ROUTINE_STATE_DEF;
}

/* Begin parsing the parameter list of a routine */
static void _begin_routine_parameter_list(mb_interpreter_t* s) {
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	context->routine_params_state = _ROUTINE_STATE_PARAMS;
}

/* End parsing the parameter list of a routine */
static void _end_routine_parameter_list(mb_interpreter_t* s) {
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	context->routine_params_state = _ROUTINE_STATE_NONE;
}

/* Duplicate a parameter from a parameter list to variable dictionary */
static _object_t* _duplicate_parameter(void* data, void* extra, _running_context_t* running) {
	_var_t* ref = 0;
	_var_t* var = 0;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(running);

	if(data == 0)
		return 0;

	ref = (_var_t*)data;

	var = _create_var(&obj, ref->name, 0, true);

	_ht_set_or_insert(running->var_dict, var->name, obj);

	return obj;
}

/* Clone a routine */
static _routine_t* _clone_routine(_routine_t* sub, void* c, bool_t toupval) {
	_routine_t* result = 0;
#ifdef MB_ENABLE_CLASS
	_class_t* instance = (_class_t*)c;
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(c);
#endif /* MB_ENABLE_CLASS */

	mb_assert(sub);

#ifdef MB_ENABLE_LAMBDA
	if(toupval || sub->type == MB_RT_LAMBDA)
		result = sub;
#else /* MB_ENABLE_LAMBDA */
	mb_unrefvar(toupval);
#endif /* MB_ENABLE_LAMBDA */

	if(!result) {
		result = (_routine_t*)mb_malloc(sizeof(_routine_t));
		memset(result, 0, sizeof(_routine_t));
		result->name = sub->name;
#ifdef MB_ENABLE_SOURCE_TRACE
		result->source_file = sub->source_file;
#endif /* MB_ENABLE_SOURCE_TRACE */
#ifdef MB_ENABLE_CLASS
		result->instance = instance;
#endif /* MB_ENABLE_CLASS */
		result->is_cloned = true;
		result->type = sub->type;
		result->func = sub->func;
	}

	return result;
}

#ifdef MB_ENABLE_LAMBDA
/* Initialize a lambda */
static _running_context_t* _init_lambda(mb_interpreter_t* s, _routine_t* routine) {
	_running_context_t* result = 0;
	_lambda_t* lambda = 0;

	mb_assert(s && routine);

	_init_routine(s, routine, 0, 0);
	mb_assert(routine->type == MB_RT_LAMBDA);
	lambda = &routine->func.lambda;
	lambda->scope = _create_running_context(true);
	result = _push_scope_by_routine(s, lambda->scope);

	return result;
}

/* Unreference a lambda routine */
static void _unref_routine(_ref_t* ref, void* data) {
	mb_assert(ref);

	if(*ref->count == _NONE_REF)
		_destroy_routine(ref->s, (_routine_t*)data);
}

/* Destroy a lambda routine */
static void _destroy_routine(mb_interpreter_t* s, _routine_t* r) {
	_gc_t* gc = 0;

	mb_assert(r);

	if(s) gc = &s->gc;
	if(!r->is_cloned) {
		if(r->name) {
			safe_free(r->name);
		}
#ifdef MB_ENABLE_SOURCE_TRACE
		if(r->source_file) {
			safe_free(r->source_file);
		}
#endif /* MB_ENABLE_SOURCE_TRACE */
		switch(r->type) {
		case MB_RT_SCRIPT:
			if(r->func.basic.scope) {
				_destroy_scope(s, r->func.basic.scope);
				r->func.basic.scope = 0;
			}
			if(r->func.basic.parameters)
				_ls_destroy(r->func.basic.parameters);

			break;
		case MB_RT_LAMBDA:
			_destroy_ref(&r->func.lambda.ref);
			if(r->func.lambda.scope->var_dict) {
				_ht_foreach(r->func.lambda.scope->var_dict, _destroy_object);
				_ht_destroy(r->func.lambda.scope->var_dict);
			}
			safe_free(r->func.lambda.scope);
			if(r->func.lambda.parameters)
				_ls_destroy(r->func.lambda.parameters);
			if(r->func.lambda.outer_scope && !_ht_find(gc->collected_table, &r->func.lambda.outer_scope->ref))
				_unref(&r->func.lambda.outer_scope->ref, r->func.lambda.outer_scope);
			if(r->func.lambda.upvalues)
				_ht_destroy(r->func.lambda.upvalues);

			break;
		case MB_RT_NATIVE: /* Do nothing */
			break;
		default: /* Do nothing */
			break;
		}
	}
	safe_free(r);
}

/* Mark an upvalue of a lambda */
static void _mark_upvalue(mb_interpreter_t* s, _lambda_t* lambda, _object_t* obj, const char* n) {
	_running_context_t* running = 0;
	_running_context_t* found_in_scope = 0;
	_ls_node_t* scp = 0;

	mb_assert(s && lambda && obj);

	running = s->running_context;
	scp = _search_identifier_in_scope_chain(s, running, n, _PATHING_NORMAL, 0, &found_in_scope);
	if(scp && found_in_scope) {
		_object_t* rot = (_object_t*)scp->data;
		rot = _GET_ROUTINE(rot);
		if(rot && lambda->scope && lambda->scope->prev != found_in_scope)
			return;
		if(!found_in_scope->refered_lambdas)
			found_in_scope->refered_lambdas = _ls_create();
		if(!_ls_find(found_in_scope->refered_lambdas, lambda, (_ls_compare_t)_ht_cmp_intptr, 0))
			_ls_pushback(found_in_scope->refered_lambdas, lambda);
	}

	if(!lambda->upvalues)
		lambda->upvalues = _ht_create(0, _ht_cmp_string, _ht_hash_string, 0);
	_ht_set_or_insert(lambda->upvalues, obj->data.variable->name, obj);
}

/* Try to mark upvalues of a lambda */
static void _try_mark_upvalue(mb_interpreter_t* s, _routine_t* r, _object_t* obj) {
	_lambda_t* lambda = 0;
	_ls_node_t* node = 0;

	mb_assert(s && r && obj);
	mb_assert(r->type == MB_RT_LAMBDA);

	lambda = &r->func.lambda;

	switch(obj->type) {
	case _DT_VAR:
		node = _ht_find(lambda->scope->var_dict, obj->data.variable->name);
		if(!node || !node->data) {
			/* Mark upvalues referencing outer scope chain */
			_mark_upvalue(s, lambda, obj, obj->data.variable->name);
		}

		break;
	default: /* Do nothing */
		break;
	}
}

/* Create an outer scope, which is a referenced type */
static _running_context_ref_t* _create_outer_scope(mb_interpreter_t* s) {
	_running_context_ref_t* result = 0;

	mb_assert(s);

	result = (_running_context_ref_t*)mb_malloc(sizeof(_running_context_ref_t));
	memset(result, 0, sizeof(_running_context_ref_t));
	_create_ref(&result->ref, _unref_outer_scope, _DT_OUTER_SCOPE, s);
	result->scope = _create_running_context(true);

	return result;
}

/* Unreference an outer scope */
static void _unref_outer_scope(_ref_t* ref, void* data) {
	mb_assert(ref);

	if(*ref->count == _NONE_REF)
		_destroy_outer_scope((_running_context_ref_t*)data);
}

/* Destroy an outer scope */
static void _destroy_outer_scope(_running_context_ref_t* p) {
	mb_assert(p);

	if(p) {
		_running_context_ref_t* scope = p;
		p = p->prev;
		_destroy_scope(scope->ref.s, scope->scope);
		_destroy_ref(&scope->ref);
		safe_free(scope);
	}
	while(p) {
		_running_context_ref_t* scope = p;
		p = p->prev;
		_unref(&scope->ref, scope);
	}
}

/* Do nothing, this is a helper function for lambda */
static int _do_nothing_on_ht_for_lambda(void* data, void* extra) {
	int result = _OP_RESULT_NORMAL;
	mb_unrefvar(data);
	mb_unrefvar(extra);

	return result;
}

/* Fill an outer scope with the original value */
static int _fill_with_upvalue(void* data, void* extra, _upvalue_scope_tuple_t* tuple) {
	_object_t* obj = (_object_t*)data;
	const char* n = (const char*)extra;
	unsigned ul = 0;
	_ls_node_t* ast = 0;
	_ls_node_t* nput = 0;

	nput = _ht_find(tuple->outer_scope->scope->var_dict, (void*)n);
	if(!nput) {
		_ls_node_t* nori = 0;
#ifdef MB_ENABLE_CLASS
		if(tuple->instance)
			nori = _search_identifier_in_scope_chain(tuple->s, tuple->scope, n, _PATHING_NORMAL, 0, 0);
		else
			nori = _ht_find(tuple->scope->var_dict, (void*)n);
#else /* MB_ENABLE_CLASS */
		nori = _ht_find(tuple->scope->var_dict, (void*)n);
#endif /* MB_ENABLE_CLASS */
		if(nori) {
			_object_t* ovar = 0;
			_var_t* var = _create_var(&ovar, n, 0, true);
			obj = (_object_t*)nori->data;
			_clone_object(tuple->s, obj, var->data, true, true);
			_REF(var->data)
			if(_IS_ROUTINE(obj) && obj->data.routine->type != MB_RT_LAMBDA) {
				ovar->is_ref = true;
				var->data->is_ref = true;
			}
#ifdef MB_ENABLE_CLASS
			if(obj->type == _DT_VAR)
				var->pathing = obj->data.variable->pathing;
			else
				var->pathing = _PATHING_NONE;
#endif /* MB_ENABLE_CLASS */
			ul = _ht_set_or_insert(tuple->outer_scope->scope->var_dict, ovar->data.variable->name, ovar);
			mb_assert(ul);
			_ht_set_or_insert(tuple->filled, extra, data);

			ast = tuple->lambda->entry;
			while(ast && ast != tuple->lambda->end->next) {
				_object_t* aobj = (_object_t*)ast->data;
				if(aobj) {
					switch(aobj->type) {
					case _DT_VAR:
						if(!strcmp(aobj->data.variable->name, ovar->data.variable->name)) {
#ifdef MB_ENABLE_CLASS
							aobj->data.variable->pathing = _PATHING_UPVALUE;
#endif /* MB_ENABLE_CLASS */
						}

						break;
					default: /* Do nothing */
						break;
					}
				}
				ast = ast->next;
			}
		}
	}

	return 0;
}

/* Remove filled upvalues */
static int _remove_filled_upvalue(void* data, void* extra, _ht_node_t* ht) {
	_ht_remove_existing(data, extra, ht);

	return _OP_RESULT_NORMAL;
}

/* Fill an outer scope with the original one */
static int _fill_outer_scope(void* data, void* extra, _upvalue_scope_tuple_t* tuple) {
	_lambda_t* lambda = (_lambda_t*)data;
	mb_unrefvar(extra);

	if(lambda->upvalues) {
		tuple->filled = _ht_create(0, _ht_cmp_intptr, _ht_hash_intptr, 0); {
			tuple->lambda = lambda;
			_HT_FOREACH(lambda->upvalues, _do_nothing_on_ht_for_lambda, _fill_with_upvalue, tuple);
			tuple->lambda = 0;
		}
		_HT_FOREACH(tuple->filled, _do_nothing_on_ht_for_lambda, _remove_filled_upvalue, lambda->upvalues);
		if(!_ht_count(lambda->upvalues)) {
			_ht_destroy(lambda->upvalues);
			lambda->upvalues = 0;
		}
		_ht_destroy(tuple->filled);
	}

	if(lambda->outer_scope) {
		_running_context_ref_t* root_ref = _get_root_ref_scope(lambda->outer_scope);
		root_ref->prev = tuple->outer_scope;
		root_ref->scope->prev = tuple->outer_scope->scope;
	} else {
		lambda->outer_scope = tuple->outer_scope;
	}

	_ref(&tuple->outer_scope->ref, tuple->outer_scope);

	return 0;
}

/* Remove a lambda from outer scope, which collected the lambda itself as an upvalue */
static int _remove_this_lambda_from_upvalue(void* data, void* extra, _routine_t* routine) {
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	assert(routine->type == MB_RT_LAMBDA);

	obj = (_object_t*)data;
	if(obj->type == _DT_VAR)
		obj = obj->data.variable->data;
	if(obj->type == _DT_ROUTINE) {
		if(obj->data.routine == routine) {
			assert(obj->data.routine->type == MB_RT_LAMBDA);
			_MAKE_NIL(obj);
		}
	}

	return result;
}

/* Link the local scope of a lambda and all upvalue scopes in chain to a given scope */
static _running_context_t* _link_lambda_scope_chain(mb_interpreter_t* s, _lambda_t* lambda, bool_t weak) {
	_running_context_ref_t* root_ref = 0;
	_running_context_t* root = 0;

	if(lambda->outer_scope) {
		lambda->scope->prev = lambda->outer_scope->scope;
		if(_find_scope(s, lambda->scope->prev)) {
			lambda->overlapped = s->running_context;
			if(!weak)
				s->running_context = lambda->scope;

			return lambda->scope;
		}
		root_ref = _get_root_ref_scope(lambda->outer_scope);
		root_ref->scope->prev = 0;
	}
	root = _get_root_scope(lambda->scope);

	if(weak) {
		_running_context_t* ret = _push_weak_scope_by_routine(s, root, 0);
		if(ret != root) {
			_destroy_scope(s, ret);

			return 0;
		}
	} else {
		root->prev = s->running_context;
		s->running_context = lambda->scope;
	}

	return lambda->scope;
}

/* Unlink the local scope of a lambda and all upvalue scopes in chain from a given scope */
static _running_context_t* _unlink_lambda_scope_chain(mb_interpreter_t* s, _lambda_t* lambda, bool_t weak) {
	_running_context_ref_t* root_ref = 0;
	_running_context_t* root = 0;

	if(lambda->outer_scope) {
		if(lambda->overlapped) {
			if(!weak)
				s->running_context = lambda->overlapped;
			lambda->overlapped = 0;
			lambda->scope->prev = 0;

			return lambda->scope;
		}
		root_ref = _get_root_ref_scope(lambda->outer_scope);
		root = root_ref->scope;
	} else {
		root = lambda->scope;
	}

	if(weak)
		_pop_weak_scope(s, root);
	else
		s->running_context = root->prev;

	root->prev = 0;
	lambda->scope->prev = 0;

	return lambda->scope;
}

/* Check whether an object is a valid lambda body node */
static bool_t _is_valid_lambda_body_node(mb_interpreter_t* s, _lambda_t* lambda, _object_t* obj) {
	mb_unrefvar(s);
	mb_unrefvar(lambda);

	return (
		!_IS_FUNC(obj, _core_def) &&
		!_IS_FUNC(obj, _core_enddef) &&
#ifdef MB_ENABLE_CLASS
		!_IS_FUNC(obj, _core_class) &&
		!_IS_FUNC(obj, _core_endclass) &&
#endif /* MB_ENABLE_CLASS */
		true
	);
}
#endif /* MB_ENABLE_LAMBDA */

#ifdef MB_ENABLE_CLASS
/* Create a scope reference to an existing one by a class */
static _running_context_t* _reference_scope_by_class(mb_interpreter_t* s, _running_context_t* p, _class_t* c) {
	_running_context_t* result = 0;
	mb_unrefvar(c);

	mb_assert(s && p);

	if(p->meta == _SCOPE_META_REF)
		p = p->ref;

	result = _create_running_context(false);
	result->meta = _SCOPE_META_REF;
	result->ref = p;

	return result;
}

/* Push a scope by a class */
static _running_context_t* _push_scope_by_class(mb_interpreter_t* s, _running_context_t* p) {
	mb_assert(s);

	if(_find_scope(s, p))
		p = _reference_scope_by_class(s, p, 0);
	p->prev = s->running_context;
	s->running_context = p;

	return s->running_context;
}

/* Try to search an identifire from a class */
static _ls_node_t* _search_identifier_in_class(mb_interpreter_t* s, _class_t* instance, const char* n, _ht_node_t** ht, _running_context_t** sp) {
	_ls_node_t* result = 0;
	_ls_node_t* node = 0;
	_class_t* meta = 0;

	mb_assert(s && instance && n);

	result = _ht_find(instance->scope->var_dict, (void*)n);
	if(result) {
		if(ht) *ht = instance->scope->var_dict;
		if(sp) *sp = instance->scope;
	}

	if(!result) {
		node = instance->meta_list ? instance->meta_list->next : 0;
		while(node) {
			meta = (_class_t*)node->data;
			result = _search_identifier_in_class(s, meta, n, ht, sp);
			if(result)
				break;
			node = node->next;
		}
	}

	return result;
}
#endif /* MB_ENABLE_CLASS */

/* Create a scope reference to an existing one by a routine */
static _running_context_t* _reference_scope_by_routine(mb_interpreter_t* s, _running_context_t* p, _routine_t* r) {
	_running_context_t* result = 0;

	mb_assert(s && p);

	if(p->meta == _SCOPE_META_REF)
		p = p->ref;

	result = _create_running_context(false);
	result->meta = _SCOPE_META_REF;
	result->ref = p;
	if(r && r->func.basic.parameters) {
		result->var_dict = _ht_create(0, _ht_cmp_string, _ht_hash_string, 0);
		_LS_FOREACH(r->func.basic.parameters, _do_nothing_on_object, _duplicate_parameter, result);
	}

	return result;
}

/* Push a weak scope by a routine */
static _running_context_t* _push_weak_scope_by_routine(mb_interpreter_t* s, _running_context_t* p, _routine_t* r) {
	mb_assert(s);

	if(_find_scope(s, p))
		p = _reference_scope_by_routine(s, p, r);
	if(p)
		p->prev = s->running_context;

	return p;
}

/* Push a scope by a routine */
static _running_context_t* _push_scope_by_routine(mb_interpreter_t* s, _running_context_t* p) {
	mb_assert(s);

	if(_find_scope(s, p))
		p = _reference_scope_by_routine(s, p, 0);
	if(p) {
		p->prev = s->running_context;
		s->running_context = p;
	}

	return s->running_context;
}

/* Destroy a scope */
static void _destroy_scope(mb_interpreter_t* s, _running_context_t* p) {
	mb_unrefvar(s);

	if(p->var_dict) {
		_ht_foreach(p->var_dict, _destroy_object);
		_ht_destroy(p->var_dict);
#ifdef MB_ENABLE_LAMBDA
		if(p->refered_lambdas) {
			_ls_destroy(p->refered_lambdas);
			p->refered_lambdas = 0;
		}
#endif /* MB_ENABLE_LAMBDA */
	}
	safe_free(p);
}

/* Pop a weak scope */
static _running_context_t* _pop_weak_scope(mb_interpreter_t* s, _running_context_t* p) {
	mb_assert(s);

	if(p)
		p->prev = 0;

	return p;
}

/* Pop a scope */
static _running_context_t* _pop_scope(mb_interpreter_t* s, bool_t tidy) {
	_running_context_t* running = 0;

	mb_assert(s);

	running = s->running_context;
	s->running_context = running->prev;
	running->prev = 0;
	if(running->meta == _SCOPE_META_REF)
		_destroy_scope(s, running);
	else if(tidy)
		_out_of_scope(s, running, 0, 0, true);

	return s->running_context;
}

/* Out of current scope */
static void _out_of_scope(mb_interpreter_t* s, _running_context_t* running, void* instance, _routine_t* routine, bool_t lose) {
#ifdef MB_ENABLE_LAMBDA
	_upvalue_scope_tuple_t tuple;
#endif /* MB_ENABLE_LAMBDA */
	mb_unrefvar(routine);

	mb_assert(s);

#ifdef MB_ENABLE_LAMBDA
	if(running->refered_lambdas) {
		tuple.s = s;
#ifdef MB_ENABLE_CLASS
		tuple.instance = (_class_t*)instance;
#else /* MB_ENABLE_CLASS */
		mb_unrefvar(instance);
#endif /* MB_ENABLE_CLASS */
		tuple.scope = running;
		tuple.outer_scope = _create_outer_scope(s);
		tuple.lambda = 0;

		_LS_FOREACH(running->refered_lambdas, _do_nothing_on_ht_for_lambda, _fill_outer_scope, &tuple);

		_ls_destroy(running->refered_lambdas);
		running->refered_lambdas = 0;
	}
#else /* MB_ENABLE_LAMBDA */
	mb_unrefvar(instance);
#endif /* MB_ENABLE_LAMBDA */

	if(lose) {
		if(running->var_dict)
			_HT_FOREACH(running->var_dict, _do_nothing_on_object, _lose_object, running);
	}
}

/* Find a scope in a scope chain */
static _running_context_t* _find_scope(mb_interpreter_t* s, _running_context_t* p) {
	_running_context_t* running = 0;

	mb_assert(s);

	running = s->running_context;
	while(running) {
		if(running == p)
			return running;

		if(running->ref == p)
			return running->ref;

		running = running->prev;
	}

	return running;
}

/* Get the root scope in a scope chain */
static _running_context_t* _get_root_scope(_running_context_t* scope) {
	_running_context_t* result = 0;

	while(scope) {
		result = scope;
		scope = scope->prev;
	}

	return result;
}

#ifdef MB_ENABLE_LAMBDA
/* Get the root referenced scope in a referenced scope chain */
static _running_context_ref_t* _get_root_ref_scope(_running_context_ref_t* scope) {
	_running_context_ref_t* result = 0;

	while(scope) {
		result = scope;
		scope = scope->prev;
	}

	return result;
}
#endif /* MB_ENABLE_LAMBDA */

/* Get a proper scope to define a routine */
static _running_context_t* _get_scope_to_add_routine(mb_interpreter_t* s) {
	_parsing_context_t* context = 0;
	_running_context_t* running = 0;
	unsigned short class_state = _CLASS_STATE_NONE;

	mb_assert(s);

	context = s->parsing_context;
	running = s->running_context;
#ifdef MB_ENABLE_CLASS
	class_state = context->class_state;
#endif /* MB_ENABLE_CLASS */
	if(class_state != _CLASS_STATE_NONE) {
		if(running)
			running = running->prev;
	} else {
		while(running) {
			if(running->meta == _SCOPE_META_ROOT)
				break;

			running = running->prev;
		}
	}

	return running;
}

/* Try to search an identifier in a scope chain */
static _ls_node_t* _search_identifier_in_scope_chain(mb_interpreter_t* s, _running_context_t* scope, const char* n, int fp, _ht_node_t** ht, _running_context_t** sp) {
	_ls_node_t* result = 0;
	_running_context_t* running = 0;
	_ht_node_t* fn = 0;
	_running_context_t* fs = 0;

	mb_assert(s && n);

#ifdef MB_ENABLE_CLASS
	if(fp) {
		result = _search_identifier_accessor(s, scope, n, &fn, &fs, fp == _PATHING_UNKNOWN_FOR_NOT_FOUND);
		if(result)
			goto _exit;
	}

	if(s->last_routine && s->last_routine->instance) {
		_class_t* lastinst = s->last_routine->instance;
		s->last_routine->instance = 0;
		result = _search_identifier_in_class(s, lastinst, n, &fn, &fs);
		s->last_routine->instance = lastinst;
		if(result)
			goto _exit;
	}
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(fp);
#endif /* MB_ENABLE_CLASS */

	if(scope)
		running = scope;
	else
		running = s->running_context;
	while(running && !result) {
		if(running->var_dict) {
			result = _ht_find(running->var_dict, (void*)n);
			fn = running->var_dict;
			fs = running;
			if(!result && running->meta == _SCOPE_META_REF) {
				result = _ht_find(running->ref->var_dict, (void*)n);
				fn = running->ref->var_dict;
				fs = running->ref;
			}
			if(result)
				break;
		}

		running = running->prev;
	}

#ifdef MB_ENABLE_CLASS
_exit:
#endif /* MB_ENABLE_CLASS */
	if(ht) *ht = fn;
	if(sp) *sp = fs;

	return result;
}

/* Try to search an array in a scope chain */
static _array_t* _search_array_in_scope_chain(mb_interpreter_t* s, _array_t* i, _object_t** o) {
	_object_t* obj = 0;
	_ls_node_t* scp = 0;
	_array_t* result = 0;

	mb_assert(s && i);

	result = i;
	if(result->name)
		scp = _search_identifier_in_scope_chain(s, 0, result->name, _PATHING_NONE, 0, 0);
	if(scp) {
		obj = (_object_t*)scp->data;
		if(obj && obj->type == _DT_ARRAY) {
			result = obj->data.array;
			if(o) *o = obj;
		}
	}

	return result;
}

/* Try to search a variable in a scope chain */
static _var_t* _search_var_in_scope_chain(mb_interpreter_t* s, _var_t* i, _object_t** o) {
	_object_t* obj = 0;
	_ls_node_t* scp = 0;
	_var_t* result = 0;

	mb_assert(s && i);

	if(o) *o = 0;
	result = i;
	scp = _search_identifier_in_scope_chain(s, 0, result->name, _PATHING_NORMAL, 0, 0);
	if(scp) {
		obj = (_object_t*)scp->data;
		if(obj && obj->type == _DT_VAR) {
			if(o) *o = obj;
			result = obj->data.variable;
		}
	}

	return result;
}

/* Try to search an identifier accessor in a scope */
static _ls_node_t* _search_identifier_accessor(mb_interpreter_t* s, _running_context_t* scope, const char* n, _ht_node_t** ht, _running_context_t** sp, bool_t unknown_for_not_found) {
	_ls_node_t* result = 0;
	_object_t* obj = 0;
	char acc[_SINGLE_SYMBOL_MAX_LENGTH];
	int i = 0;
	int j = 0;
	int nc = 0;
#ifdef MB_ENABLE_CLASS
	_class_t* instance = 0;
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(unknown_for_not_found);
#endif /* MB_ENABLE_CLASS */

	mb_assert(s && n);

	while((i == 0) || (i > 0 && n[i - 1])) {
		acc[j] = n[i];
		if(_is_accessor_char(acc[j]) || acc[j] == _ZERO_CHAR) {
			acc[j] = _ZERO_CHAR;
			++nc;
			do {
#ifdef MB_ENABLE_CLASS
				if(instance) {
					result = _search_identifier_in_class(s, instance, acc, ht, sp);
					if(!result && unknown_for_not_found) {
						result = (_ls_node_t*)&_LS_NODE_UNKNOWN;

						return result;
					}

					break;
				} else if(nc > 1) {
					return 0;
				}
#endif /* MB_ENABLE_CLASS */

				result = _search_identifier_in_scope_chain(s, scope, acc, _PATHING_NONE, ht, sp);
			} while(0);

			if(!result)
				return 0;
			obj = (_object_t*)result->data;
			if(!obj)
				return 0;
			switch(obj->type) {
			case _DT_VAR:
#ifdef MB_ENABLE_USERTYPE_REF
				if(obj->data.variable->data->type == _DT_USERTYPE_REF)
					return result;
#endif /* MB_ENABLE_USERTYPE_REF */
#ifdef MB_ENABLE_CLASS
				if(obj->data.variable->data->type == _DT_CLASS)
					instance = obj->data.variable->data->data.instance;
#endif /* MB_ENABLE_CLASS */

				break;
#ifdef MB_ENABLE_CLASS
			case _DT_CLASS:
				instance = obj->data.instance;

				break;
#endif /* MB_ENABLE_CLASS */
			case _DT_ARRAY: /* Fall through */
			case _DT_ROUTINE: /* Do nothing */
				break;
			default:
				mb_assert(0 && "Unsupported.");

				return 0;
			}

			j = 0;
			i++;

			continue;
		}
		j++;
		i++;
	}

	return result;
}

/* Create a variable object */
static _var_t* _create_var(_object_t** oobj, const char* n, size_t ns, bool_t dup_name) {
	_object_t* obj = 0;
	_var_t* var = 0;

	var = (_var_t*)mb_malloc(sizeof(_var_t));
	memset(var, 0, sizeof(_var_t));
	if(dup_name)
		var->name = mb_strdup(n, ns);
	else
		var->name = (char*)n;
	var->data = _create_object();

	if(!oobj || !(*oobj))
		obj = _create_object();
	else
		obj = *oobj;
	_MAKE_NIL(obj);
	obj->type = _DT_VAR;
	obj->data.variable = var;
	obj->is_ref = false;

	if(oobj) *oobj = obj;

	return var;
}

/* Create an _object_t struct */
static _object_t* _create_object(void) {
	_object_t* result = 0;

	result = (_object_t*)mb_malloc(sizeof(_object_t));
	_MAKE_NIL(result);

	return result;
}

/* Clone the data of an object */
static int _clone_object(mb_interpreter_t* s, _object_t* obj, _object_t* tgt, bool_t toupval, bool_t deep) {
	int result = 0;

	mb_assert(obj && tgt);

	_MAKE_NIL(tgt);
	if(_is_internal_object(obj))
		goto _exit;
	tgt->type = obj->type;
	switch(obj->type) {
	case _DT_VAR:
		_clone_object(s, obj->data.variable->data, tgt, toupval, deep);

		break;
	case _DT_STRING:
		tgt->data.string = mb_strdup(obj->data.string, 0);

		break;
#ifdef MB_ENABLE_USERTYPE_REF
	case _DT_USERTYPE_REF:
		_clone_usertype_ref(obj->data.usertype_ref, tgt);

		break;
#endif /* MB_ENABLE_USERTYPE_REF */
	case _DT_FUNC:
		tgt->data.func->name = mb_strdup(obj->data.func->name, strlen(obj->data.func->name) + 1);
		tgt->data.func->pointer = obj->data.func->pointer;

		break;
	case _DT_ARRAY:
		if(deep) {
			tgt->data.array = _clone_array(s, obj->data.array);
		} else {
			tgt->data.array = obj->data.array;
#ifdef MB_ENABLE_ARRAY_REF
			_ref(&obj->data.array->ref, obj->data.array);
#endif /* MB_ENABLE_ARRAY_REF */
		}

		break;
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		if(deep) {
			tgt->data.list = _create_list(obj->data.list->ref.s);
			_ref(&tgt->data.list->ref, tgt->data.list);
			_LS_FOREACH(obj->data.list->list, _do_nothing_on_object, _clone_to_list, tgt->data.list);
		} else {
			tgt->data.list = obj->data.list;
			_ref(&obj->data.list->ref, obj->data.list);
		}

		break;
	case _DT_LIST_IT:
		tgt->data.list_it = _create_list_it(obj->data.list_it->list, true);

		break;
	case _DT_DICT:
		if(deep) {
			tgt->data.dict = _create_dict(obj->data.dict->ref.s);
			_ref(&tgt->data.dict->ref, tgt->data.dict);
			_HT_FOREACH(obj->data.dict->dict, _do_nothing_on_object, _clone_to_dict, tgt->data.dict);
		} else {
			tgt->data.dict = obj->data.dict;
			_ref(&obj->data.dict->ref, obj->data.dict);
		}

		break;
	case _DT_DICT_IT:
		tgt->data.dict_it = _create_dict_it(obj->data.dict_it->dict, true);

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
	case _DT_LABEL:
		tgt->data.label->name = mb_strdup(obj->data.label->name, 0);
		tgt->data.label->node = obj->data.label->node;

		break;
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		if(deep) {
			tgt->data.instance = (_class_t*)mb_malloc(sizeof(_class_t));
			_init_class(s, tgt->data.instance, mb_strdup(obj->data.instance->name, 0));
			tgt->data.instance->created_from = obj->data.instance->created_from;
			_push_scope_by_class(s, tgt->data.instance->scope);
			_traverse_class(obj->data.instance, _clone_clsss_field, _clone_class_meta_link, _META_LIST_MAX_DEPTH, false, tgt->data.instance, 0);
			if(_search_class_hash_and_compare_functions(s, tgt->data.instance) != MB_FUNC_OK) {
				mb_assert(0 && "Impossible.");
			}
			tgt->data.instance->userdata = obj->data.instance->userdata;
			_pop_scope(s, false);
		} else {
			tgt->data.instance = obj->data.instance;
			_ref(&obj->data.instance->ref, obj->data.instance);
		}

		break;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		tgt->data.routine = _clone_routine(
			obj->data.routine,
#ifdef MB_ENABLE_CLASS
			obj->data.routine->instance,
#else /* MB_ENABLE_CLASS */
			0,
#endif /* MB_ENABLE_CLASS */
			toupval
		);

		break;
	case _DT_NIL: /* Fall through */
	case _DT_UNKNOWN: /* Fall through */
	case _DT_INT: /* Fall through */
	case _DT_REAL: /* Fall through */
	case _DT_TYPE: /* Fall through */
	case _DT_SEP: /* Fall through */
	case _DT_EOS: /* Fall through */
	case _DT_USERTYPE:
		tgt->data = obj->data;

		break;
	default:
		mb_assert(0 && "Invalid type.");

		break;
	}
	tgt->is_ref = false;
#ifdef MB_ENABLE_SOURCE_TRACE
	tgt->source_pos = 0;
	tgt->source_row = 0;
	tgt->source_col = 0;
#else /* MB_ENABLE_SOURCE_TRACE */
	tgt->source_pos = 0;
#endif /* MB_ENABLE_SOURCE_TRACE */
	++result;

_exit:
	return result;
}

/* Dispose the data of an object */
static int _dispose_object(_object_t* obj) {
	int result = 0;
	_var_t* var = 0;

	mb_assert(obj);

	if(_is_internal_object(obj))
		goto _exit;
	switch(obj->type) {
	case _DT_VAR:
		if(!obj->is_ref) {
			var = (_var_t*)obj->data.variable;
			safe_free(var->name);
			mb_assert(var->data->type != _DT_VAR);
			if(_IS_ME(var))
				_destroy_object_capsule_only(var->data, 0);
			else
				_destroy_object(var->data, 0);
			safe_free(var);
		}

		break;
	case _DT_STRING:
		if(!obj->is_ref) {
			if(obj->data.string) {
				safe_free(obj->data.string);
			}
		}

		break;
	case _DT_FUNC:
		safe_free(obj->data.func->name);
		safe_free(obj->data.func);

		break;
	_UNREF_USERTYPE_REF(obj)
#ifdef MB_ENABLE_ARRAY_REF
	_UNREF_ARRAY(obj)
#else /* MB_ENABLE_ARRAY_REF */
	_DESTROY_ARRAY(obj)
#endif /* MB_ENABLE_ARRAY_REF */
	_UNREF_COLL(obj)
	_UNREF_COLL_IT(obj)
	_UNREF_CLASS(obj)
	_UNREF_ROUTINE(obj)
	case _DT_LABEL:
		if(!obj->is_ref) {
			safe_free(obj->data.label->name);
			safe_free(obj->data.label);
		}

		break;
#ifdef MB_ENABLE_SOURCE_TRACE
	case _DT_PREV_IMPORT: /* Fall through */
	case _DT_POST_IMPORT:
		if(!obj->is_ref) {
			if(obj->data.import_info) {
				if(obj->data.import_info->source_file) {
					safe_free(obj->data.import_info->source_file);
				}
				safe_free(obj->data.import_info);
			}
		}

		break;
#endif /* MB_ENABLE_SOURCE_TRACE */
	case _DT_NIL: /* Fall through */
	case _DT_UNKNOWN: /* Fall through */
	case _DT_INT: /* Fall through */
	case _DT_REAL: /* Fall through */
	case _DT_TYPE: /* Fall through */
	case _DT_SEP: /* Fall through */
	case _DT_EOS: /* Fall through */
	case _DT_USERTYPE: /* Do nothing */
		break;
	default:
		mb_assert(0 && "Invalid type.");

		break;
	}
	obj->is_ref = false;
	obj->type = _DT_NIL;
	memset(&obj->data, 0, sizeof(obj->data));
#ifdef MB_ENABLE_SOURCE_TRACE
	obj->source_pos = 0;
	obj->source_row = 0;
	obj->source_col = 0;
#else /* MB_ENABLE_SOURCE_TRACE */
	obj->source_pos = 0;
#endif /* MB_ENABLE_SOURCE_TRACE */
	++result;

_exit:
	return result;
}

/* Destroy an object and its data */
static int _destroy_object(void* data, void* extra) {
	int result = _OP_RESULT_DEL_NODE;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	if(!_dispose_object(obj))
		goto _exit;
	safe_free(obj);

_exit:
	return result;
}



/* Destroy an object, including its data and extra data */
static int _destroy_object_with_extra(void* data, void* extra) {
	int result = _OP_RESULT_DEL_NODE;
	_object_t* obj = 0;

	mb_assert(data);

	obj = (_object_t*)data;
	if(!_dispose_object(obj))
		goto _exit;
	safe_free(obj);
	obj = (_object_t*)extra;
	if(!_dispose_object(obj))
		goto _exit;
	safe_free(obj);

_exit:
	return result;
}

/* Destroy an object which is not come from compile time */
static int _destroy_object_not_compile_time(void* data, void* extra) {
	int result = _OP_RESULT_DEL_NODE;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	if(!obj->source_pos) {
		if(!_dispose_object(obj))
			goto _exit;
		safe_free(obj);
	}

_exit:
	return result;
}

/* Destroy only the capsule (wrapper) of an object, leave the data behind */
static int _destroy_object_capsule_only(void* data, void* extra) {
	int result = _OP_RESULT_DEL_NODE;
	_object_t* obj = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	obj = (_object_t*)data;
	safe_free(obj);

	return result;
}

/* Do nothing with an object, this is a helper function */
static int _do_nothing_on_object(void* data, void* extra) {
	int result = _OP_RESULT_NORMAL;
	mb_unrefvar(data);
	mb_unrefvar(extra);

	return result;
}

/* Lose an object in a scope */
static int _lose_object(void* data, void* extra, _running_context_t* running) {
	int result = _OP_RESULT_NORMAL;
	_object_t* obj = 0;
	bool_t make_nil = true;

	mb_assert(data && extra);

	obj = (_object_t*)data;
#ifdef MB_ENABLE_LAMBDA
	if(obj->type == _DT_ROUTINE && obj->data.routine->type == MB_RT_LAMBDA)
		obj->is_ref = false;
#endif /* MB_ENABLE_LAMBDA */
#ifdef MB_ENABLE_COLLECTION_LIB
	if(obj->type == _DT_LIST_IT) {
		if((!obj->is_ref || !obj->data.list_it->locking) && running->intermediate_value.value.list_it != obj->data.list_it) {
			_destroy_list_it(obj->data.list_it); /* Process dangling value */
		}

		goto _exit;
	} else if(obj->type == _DT_DICT_IT) {
		if((!obj->is_ref || !obj->data.dict_it->locking) && running->intermediate_value.value.dict_it != obj->data.dict_it) {
			_destroy_dict_it(obj->data.dict_it); /* Process dangling value */
		}

		goto _exit;
	}
#endif /* MB_ENABLE_COLLECTION_LIB */
	switch(obj->type) {
	case _DT_VAR:
		_lose_object(obj->data.variable->data, extra, running);
		make_nil = false;

		break;
	_UNREF_USERTYPE_REF(obj)
	_UNREF_ARRAY(obj)
	_UNREF_COLL(obj)
	_UNREF_CLASS(obj)
	_UNREF_ROUTINE(obj)
	default:
		make_nil = false;

		break;
	}

#ifdef MB_ENABLE_COLLECTION_LIB
_exit:
#endif /* MB_ENABLE_COLLECTION_LIB */
	if(make_nil) {
		_MAKE_NIL(obj);
	}

	return result;
}

/* Remove an object referenced to source code */
static int _remove_source_object(void* data, void* extra) {
	int result = _OP_RESULT_DEL_NODE;
	mb_unrefvar(extra);

	mb_assert(data);

	return result;
}

/* Destroy a chunk of memory */
static int _destroy_memory(void* data, void* extra) {
	int result = _OP_RESULT_NORMAL;
	mb_unrefvar(extra);

	mb_assert(data);

	safe_free(data);

	return result;
}

/* Compare two numbers from two objects */
static int _compare_numbers(const _object_t* first, const _object_t* second) {
	int result = 0;

	mb_assert(first && second);
	mb_assert((first->type == _DT_INT || first->type == _DT_REAL) && (second->type == _DT_INT || second->type == _DT_REAL));

	if(first->type == _DT_INT && second->type == _DT_INT) {
		if(first->data.integer > second->data.integer)
			result = 1;
		else if(first->data.integer < second->data.integer)
			result = -1;
	} else if(first->type == _DT_REAL && second->type == _DT_REAL) {
		if(first->data.float_point > second->data.float_point)
			result = 1;
		else if(first->data.float_point < second->data.float_point)
			result = -1;
	} else {
		if((first->type == _DT_INT ? (real_t)first->data.integer : first->data.float_point) > (second->type == _DT_INT ? (real_t)second->data.integer : second->data.float_point))
			result = 1;
		else if((first->type == _DT_INT ? (real_t)first->data.integer : first->data.float_point) < (second->type == _DT_INT ? (real_t)second->data.integer : second->data.float_point))
			result = -1;
	}

	return result;
}

/* Determine whether an object is internal */
static bool_t _is_internal_object(_object_t* obj) {
	bool_t result = false;

	mb_assert(obj);

	result = (
		(_exp_assign == obj) ||
		(_OBJ_BOOL_TRUE == obj) || (_OBJ_BOOL_FALSE == obj)
	);

	return result;
}

/* Convert a public mb_data_e type to an internal _data_e */
static _data_e _public_type_to_internal_type(mb_data_e t) {
	switch(t) {
	case MB_DT_NIL:
		return _DT_NIL;
	case MB_DT_INT:
		return _DT_INT;
	case MB_DT_REAL:
		return _DT_REAL;
	case MB_DT_STRING:
		return _DT_STRING;
	case MB_DT_TYPE:
		return _DT_TYPE;
	case MB_DT_USERTYPE:
		return _DT_USERTYPE;
#ifdef MB_ENABLE_USERTYPE_REF
	case MB_DT_USERTYPE_REF:
		return _DT_USERTYPE_REF;
#endif /* MB_ENABLE_USERTYPE_REF */
	case MB_DT_ARRAY:
		return _DT_ARRAY;
#ifdef MB_ENABLE_COLLECTION_LIB
	case MB_DT_LIST:
		return _DT_LIST;
	case MB_DT_LIST_IT:
		return _DT_LIST_IT;
	case MB_DT_DICT:
		return _DT_DICT;
	case MB_DT_DICT_IT:
		return _DT_DICT_IT;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case MB_DT_CLASS:
		return _DT_CLASS;
#endif /* MB_ENABLE_CLASS */
	case MB_DT_ROUTINE:
		return _DT_ROUTINE;
	default:
		return _DT_UNKNOWN;
	}
}

/* Convert an internal mb_data_e type to a public _data_e */
static mb_data_e _internal_type_to_public_type(_data_e t) {
	switch(t) {
	case _DT_NIL:
		return MB_DT_NIL;
	case _DT_INT:
		return MB_DT_INT;
	case _DT_REAL:
		return MB_DT_REAL;
	case _DT_STRING:
		return MB_DT_STRING;
	case _DT_TYPE:
		return MB_DT_TYPE;
	case _DT_USERTYPE:
		return MB_DT_USERTYPE;
#ifdef MB_ENABLE_USERTYPE_REF
	case _DT_USERTYPE_REF:
		return MB_DT_USERTYPE_REF;
#endif /* MB_ENABLE_USERTYPE_REF */
	case _DT_ARRAY:
		return MB_DT_ARRAY;
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		return MB_DT_LIST;
	case _DT_LIST_IT:
		return MB_DT_LIST_IT;
	case _DT_DICT:
		return MB_DT_DICT;
	case _DT_DICT_IT:
		return MB_DT_DICT_IT;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		return MB_DT_CLASS;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		return MB_DT_ROUTINE;
	default:
		return MB_DT_UNKNOWN;
	}
}

/* Assign a public mb_value_t to an internal _object_t */
static int _public_value_to_internal_object(mb_value_t* pbl, _object_t* itn) {
	int result = MB_FUNC_OK;

	mb_assert(pbl && itn);

	_UNREF(itn)

	switch(pbl->type) {
	case MB_DT_NIL:
		itn->type = _DT_NIL;
		itn->data.integer = false;

		break;
	case MB_DT_UNKNOWN:
		itn->type = _DT_UNKNOWN;
		itn->data.integer = false;

		break;
	case MB_DT_INT:
		itn->type = _DT_INT;
		itn->data.integer = pbl->value.integer;

		break;
	case MB_DT_REAL:
		itn->type = _DT_REAL;
		itn->data.float_point = pbl->value.float_point;

		break;
	case MB_DT_STRING:
		itn->type = _DT_STRING;
		itn->data.string = pbl->value.string;
		itn->is_ref = true;

		break;
	case MB_DT_TYPE:
		itn->type = _DT_TYPE;
		itn->data.type = pbl->value.type;

		break;
	case MB_DT_USERTYPE:
		itn->type = _DT_USERTYPE;
		memcpy(itn->data.bytes, pbl->value.bytes, sizeof(mb_val_bytes_t));

		break;
#ifdef MB_ENABLE_USERTYPE_REF
	case MB_DT_USERTYPE_REF:
		itn->type = _DT_USERTYPE_REF;
		itn->data.usertype_ref = (_usertype_ref_t*)pbl->value.usertype_ref;

		break;
#endif /* MB_ENABLE_USERTYPE_REF */
	case MB_DT_ARRAY:
		itn->type = _DT_ARRAY;
		itn->data.array = (_array_t*)pbl->value.array;
		itn->is_ref = false;

		break;
#ifdef MB_ENABLE_COLLECTION_LIB
	case MB_DT_LIST:
		itn->type = _DT_LIST;
		itn->data.list = (_list_t*)pbl->value.list;

		break;
	case MB_DT_LIST_IT:
		itn->type = _DT_LIST_IT;
		itn->data.list_it = (_list_it_t*)pbl->value.list_it;

		break;
	case MB_DT_DICT:
		itn->type = _DT_DICT;
		itn->data.dict = (_dict_t*)pbl->value.dict;

		break;
	case MB_DT_DICT_IT:
		itn->type = _DT_DICT_IT;
		itn->data.dict_it = (_dict_it_t*)pbl->value.dict_it;

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case MB_DT_CLASS:
		itn->type = _DT_CLASS;
		itn->data.instance = (_class_t*)pbl->value.instance;

		break;
#endif /* MB_ENABLE_CLASS */
	case MB_DT_ROUTINE:
		itn->type = _DT_ROUTINE;
		itn->data.routine = (_routine_t*)pbl->value.routine;

		break;
	default:
		result = MB_FUNC_ERR;

		break;
	}

	return result;
}

/* Assign an internal _object_t to a public mb_value_t */
static int _internal_object_to_public_value(_object_t* itn, mb_value_t* pbl) {
	int result = MB_FUNC_OK;

	mb_assert(pbl && itn);

	switch(itn->type) {
	case _DT_VAR:
		result = _internal_object_to_public_value(itn->data.variable->data, pbl);

		break;
	case _DT_NIL:
		mb_make_nil(*pbl);

		break;
	case _DT_UNKNOWN:
		pbl->type = MB_DT_UNKNOWN;
		pbl->value.integer = false;

		break;
	case _DT_INT:
		pbl->type = MB_DT_INT;
		pbl->value.integer = itn->data.integer;

		break;
	case _DT_REAL:
		pbl->type = MB_DT_REAL;
		pbl->value.float_point = itn->data.float_point;

		break;
	case _DT_STRING:
		pbl->type = MB_DT_STRING;
		pbl->value.string = itn->data.string;

		break;
	case _DT_TYPE:
		pbl->type = MB_DT_TYPE;
		pbl->value.type = itn->data.type;

		break;
	case _DT_USERTYPE:
		pbl->type = MB_DT_USERTYPE;
		memcpy(pbl->value.bytes, itn->data.bytes, sizeof(mb_val_bytes_t));

		break;
#ifdef MB_ENABLE_USERTYPE_REF
	case _DT_USERTYPE_REF:
		pbl->type = MB_DT_USERTYPE_REF;
		pbl->value.usertype_ref = itn->data.usertype_ref;

		break;
#endif /* MB_ENABLE_USERTYPE_REF */
	case _DT_ARRAY:
		pbl->type = MB_DT_ARRAY;
		pbl->value.array = itn->data.array;

		break;
#ifdef MB_ENABLE_COLLECTION_LIB
	case _DT_LIST:
		pbl->type = MB_DT_LIST;
		pbl->value.list = itn->data.list;

		break;
	case _DT_LIST_IT:
		pbl->type = MB_DT_LIST_IT;
		pbl->value.list_it = itn->data.list;

		break;
	case _DT_DICT:
		pbl->type = MB_DT_DICT;
		pbl->value.dict = itn->data.dict;

		break;
	case _DT_DICT_IT:
		pbl->type = MB_DT_DICT_IT;
		pbl->value.dict_it = itn->data.dict_it;

		break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		pbl->type = MB_DT_CLASS;
		pbl->value.instance = itn->data.instance;

		break;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		pbl->type = MB_DT_ROUTINE;
		pbl->value.routine = itn->data.routine;

		break;
	default:
		result = MB_FUNC_ERR;

		break;
	}

	return result;
}

/* Create an internal object from a public value */
static int _create_internal_object_from_public_value(mb_value_t* pbl, _object_t** itn) {
	int result = MB_FUNC_OK;

	mb_assert(pbl && itn);

	*itn = _create_object();
	_public_value_to_internal_object(pbl, *itn);
	if((*itn)->type == _DT_STRING) {
		(*itn)->is_ref = false;
		(*itn)->data.string = mb_strdup((*itn)->data.string, strlen((*itn)->data.string) + 1);
	}

	return result;
}

/* Compare a public value and an internal object */
static int _compare_public_value_and_internal_object(mb_value_t* pbl, _object_t* itn) {
	int result = 0;
	mb_value_t tmp;

	mb_make_nil(tmp);
	_internal_object_to_public_value(itn, &tmp);
	if(pbl->type != tmp.type) {
		result = pbl->type - tmp.type;
	} else {
		switch(pbl->type) {
		case MB_DT_NIL:
			result = 0;

			break;
		case MB_DT_INT:
			result = mb_memcmp(&pbl->value.integer, &tmp.value.integer, sizeof(int_t));

			break;
		case MB_DT_REAL:
			result = mb_memcmp(&pbl->value.float_point, &tmp.value.float_point, sizeof(real_t));

			break;
		case MB_DT_STRING:
			result = mb_memcmp(&pbl->value.string, &tmp.value.string, sizeof(char*));

			break;
		case MB_DT_TYPE:
			result = mb_memcmp(&pbl->value.type, &tmp.value.type, sizeof(mb_data_e));

			break;
		case MB_DT_USERTYPE:
			result = mb_memcmp(&pbl->value.bytes, &tmp.value.bytes, sizeof(mb_val_bytes_t));

			break;
#ifdef MB_ENABLE_USERTYPE_REF
		case MB_DT_USERTYPE_REF:
			result = mb_memcmp(&pbl->value.usertype_ref, &tmp.value.usertype_ref, sizeof(void*));

			break;
#endif /* MB_ENABLE_USERTYPE_REF */
		case MB_DT_ARRAY:
			result = mb_memcmp(&pbl->value.array, &tmp.value.array, sizeof(void*));

			break;
#ifdef MB_ENABLE_COLLECTION_LIB
		case MB_DT_LIST:
			result = mb_memcmp(&pbl->value.list, &tmp.value.list, sizeof(void*));

			break;
		case MB_DT_LIST_IT:
			result = mb_memcmp(&pbl->value.list_it, &tmp.value.list_it, sizeof(void*));

			break;
		case MB_DT_DICT:
			result = mb_memcmp(&pbl->value.dict, &tmp.value.dict, sizeof(void*));

			break;
		case MB_DT_DICT_IT:
			result = mb_memcmp(&pbl->value.dict_it, &tmp.value.dict_it, sizeof(void*));

			break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		case MB_DT_CLASS:
			result = mb_memcmp(&pbl->value.instance, &tmp.value.instance, sizeof(void*));

			break;
#endif /* MB_ENABLE_CLASS */
		case MB_DT_ROUTINE:
			result = mb_memcmp(&pbl->value.routine, &tmp.value.routine, sizeof(void*));

			break;
		default:
			result = mb_memcmp(pbl->value.bytes, tmp.value.bytes, sizeof(mb_val_bytes_t));

			break;
		}
	}

	return result;
}

/* Try clear the intermediate value */
static void _try_clear_intermediate_value(void* data, void* extra, mb_interpreter_t* s) {
	_object_t* obj = 0;
	_running_context_t* running = 0;
	mb_unrefvar(extra);

	mb_assert(s);

	if(data == 0)
		return;

	obj = (_object_t*)data;
	running = s->running_context;

	if(!_compare_public_value_and_internal_object(&running->intermediate_value, obj)) {
		mb_make_nil(running->intermediate_value);
	}
}

/* Remove from another list if exists */
static void _remove_if_exists(void* data, void* extra, _ls_node_t* ls) {
	_object_t* obj = 0;
	mb_unrefvar(extra);

	obj = (_object_t*)data;
	_ls_try_remove(ls, obj, _ls_cmp_data, 0);
}

/* Destroy an object in variable argument list */
static void _destroy_var_arg(void* data, void* extra, _gc_t* gc) {
	_object_t* obj = 0;
	mb_unrefvar(extra);
	mb_unrefvar(gc);

	mb_assert(data);

	obj = (_object_t*)data;
	_UNREF(obj)
	safe_free(obj);
}

/* Destroy edge destroying objects */
static void _destroy_edge_objects(mb_interpreter_t* s) {
	if(!s) return;

	_ls_foreach(s->edge_destroy_objects, _destroy_object);
	_ls_clear(s->edge_destroy_objects);
}

/* Mark a string as an edge destroying object */
static void _mark_edge_destroy_string(mb_interpreter_t* s, char* ch) {
	_object_t* temp_obj = 0;

	mb_assert(s && ch);

	temp_obj = _create_object();
	temp_obj->type = _DT_STRING;
	temp_obj->is_ref = false;
	temp_obj->data.string = ch;
	_ls_pushback(s->edge_destroy_objects, temp_obj);
}

/* Destroy lazy destroying objects */
static void _destroy_lazy_objects(mb_interpreter_t* s) {
	mb_assert(s);

	_LS_FOREACH(s->lazy_destroy_objects, _destroy_object, _try_clear_intermediate_value, s);
	_ls_clear(s->lazy_destroy_objects);
}

/* Mark a string as a lazy destroying object */
static void _mark_lazy_destroy_string(mb_interpreter_t* s, char* ch) {
	_object_t* temp_obj = 0;

	mb_assert(s && ch);

	temp_obj = _create_object();
	temp_obj->type = _DT_STRING;
	temp_obj->is_ref = false;
	temp_obj->data.string = ch;
	_ls_pushback(s->lazy_destroy_objects, temp_obj);
}

/* Assign a value with another */
static void _assign_public_value(mb_interpreter_t* s, mb_value_t* tgt, mb_value_t* src, bool_t pit) {
	_object_t obj;
	mb_value_t nil;

	mb_assert(tgt);

#ifdef MB_ENABLE_COLLECTION_LIB
	if(pit && _try_purge_it(s, tgt, 0))
		return;
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(s);
	mb_unrefvar(pit);
#endif /* MB_ENABLE_COLLECTION_LIB */

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(tgt, &obj);
	_UNREF(&obj)

	mb_make_nil(nil);
	if(!src)
		src = &nil;
	memcpy(tgt, src, sizeof(mb_value_t));
	*src = nil;
}

/* Swap two public values */
static void _swap_public_value(mb_value_t* tgt, mb_value_t* src) {
	mb_value_t tmp;

	mb_assert(tgt && src);

	tmp = *tgt;
	*tgt = *src;
	*src = tmp;
}

/* Clear the scope chain */
static int _clear_scope_chain(mb_interpreter_t* s) {
	int result = 0;
	_running_context_t* running = 0;
	_running_context_t* prev = 0;
	
	char stopFlag = 0;
	mb_assert(s);

	running = s->running_context;
	while(running) {
		prev = running->prev;

		_ht_foreach(running->var_dict, _destroy_object);
		_ht_clear(running->var_dict);

		//If we just cleared an interpreters root context, stop before we get to the parent context
		if (stopFlag)
		{
			break;
		}
#ifdef MB_ENABLE_LAMBDA
		if(running->refered_lambdas)
			_ls_clear(running->refered_lambdas);
#endif /* MB_ENABLE_LAMBDA */

		result++;
		running = prev;
		//We've reached a meta_root, we can stop now before we get to the parent.
		if(running->meta = _SCOPE_META_ROOT)
		{
			stopFlag = 1;
		}
	}

	return result;
}

/* Dispose the scope chain */
static int _dispose_scope_chain(mb_interpreter_t* s) {
	int result = 0;
	_running_context_t* running = 0;
	_running_context_t* prev = 0;

	mb_assert(s);

	running = s->running_context;
	while(running) {
		prev = running->prev;

		_ht_foreach(running->var_dict, _destroy_object);
		_ht_clear(running->var_dict);
		_ht_destroy(running->var_dict);
#ifdef MB_ENABLE_LAMBDA
		if(running->refered_lambdas) {
			_ls_clear(running->refered_lambdas);
			_ls_destroy(running->refered_lambdas);
			running->refered_lambdas = 0;
		}
#endif /* MB_ENABLE_LAMBDA */
		running->var_dict = 0;
		mb_dispose_value(s, running->intermediate_value);
		safe_free(running);

		result++;
		running = prev;
	}
	s->running_context = 0;

	return result;
}

/* Tidy the scope chain */
static void _tidy_scope_chain(mb_interpreter_t* s) {
	_parsing_context_t* context = 0;

	mb_assert(s);

	context = s->parsing_context;
	if(!context) {
		while(s->running_context->prev)
			_pop_scope(s, false);

		return;
	}

	while(context->routine_state && s->running_context->meta != _SCOPE_META_ROOT) {
		if(_end_routine(s)) {
			if(!s->running_context->prev)
				break;
			_pop_scope(s, false);
		}
	}
#ifdef MB_ENABLE_CLASS
	while(context->class_state != _CLASS_STATE_NONE) {
		if(_end_class(s)) {
			if(!s->running_context->prev)
				break;
			_pop_scope(s, false);
		}
	}
#endif /* MB_ENABLE_CLASS */
}

/* Collect the intermediate value in a scope */
static void _collect_intermediate_value_in_scope(_running_context_t* running, void* data) {
	_object_t tmp;

	mb_assert(running && data);

	if(!running)
		return;

	_MAKE_NIL(&tmp);
	_public_value_to_internal_object(&running->intermediate_value, &tmp);
	if(tmp.data.pointer == data) {
		switch(tmp.type) {
#ifdef MB_ENABLE_USERTYPE_REF
		case _DT_USERTYPE_REF: /* Fall through */
#endif /* MB_ENABLE_USERTYPE_REF */
#ifdef MB_ENABLE_COLLECTION_LIB
		case _DT_LIST: /* Fall through */
		case _DT_DICT: /* Fall through */
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		case _DT_CLASS: /* Fall through */
#endif /* MB_ENABLE_CLASS */
		case _DT_ARRAY:
			mb_make_nil(running->intermediate_value);

			break;
		default: /* Do nothing */
			break;
		}
	}
}

#ifdef MB_ENABLE_FORK
/* Collect all intermediate values in scope chain */
static void _collect_intermediate_value_in_scope_chain(void* data, void* extra, void* d) {
	mb_interpreter_t* s = 0;
	_running_context_t* running = 0;
	mb_unrefvar(extra);

	s = (mb_interpreter_t*)data;
	running = s->running_context;
	while(running) {
		_collect_intermediate_value_in_scope(running, d);
		running = running->prev;
	}
}
#endif /* MB_ENABLE_FORK */

/* Collect the intermediate value */
static void _collect_intermediate_value(_ref_t* ref, void* data) {
	mb_interpreter_t* s = 0;

	mb_assert(ref && data);

	s = ref->s;
	if(!s) return;
	if(s->running_context)
		_collect_intermediate_value_in_scope(s->running_context, data);
#ifdef MB_ENABLE_FORK
	if(s->all_forked) {
		while(mb_get_forked_from(s, &s) == MB_FUNC_OK) {
			/* Do nothing */
		}
		_LS_FOREACH(s->all_forked, _do_nothing_on_object, _collect_intermediate_value_in_scope_chain, data);
	}
#endif /* MB_ENABLE_FORK */
}

/* Mark the intermediate value to be collected if it's dangling */
static void _mark_dangling_intermediate_value(mb_interpreter_t* s, _running_context_t* running) {
	mb_assert(s && running);

	switch(running->intermediate_value.type) {
#ifdef MB_ENABLE_USERTYPE_REF
	case MB_DT_USERTYPE_REF: /* Fall through */
#endif /* MB_ENABLE_USERTYPE_REF */
#ifdef MB_ENABLE_COLLECTION_LIB
	case MB_DT_LIST: /* Fall through */
	case MB_DT_DICT: /* Fall through */
	case MB_DT_LIST_IT: /* Fall through */
	case MB_DT_DICT_IT: /* Fall through */
#endif /* MB_ENABLE_COLLECTION_LIB */
	case MB_DT_ROUTINE: {
			_object_t tmp;
			_MAKE_NIL(&tmp);
			_public_value_to_internal_object(&running->intermediate_value, &tmp);
#ifdef MB_ENABLE_COLLECTION_LIB
			if(tmp.type == _DT_LIST_IT && tmp.data.list_it->locking)
				break;
			else if(tmp.type == _DT_DICT_IT && tmp.data.dict_it->locking)
				break;
#endif /* MB_ENABLE_COLLECTION_LIB */
			_ADDGC(&tmp, &s->gc, false) /* Process dangling value */
		}

		break;
	default: /* Do nothing */
		break;
	}
}

/* Evaluate a variable, this is a helper function for the PRINT statement */
static _object_t* _eval_var_in_print(mb_interpreter_t* s, _object_t** val_ptr, _ls_node_t** ast, _object_t* obj) {
	_object_t tmp;

	mb_assert(s);

	switch(obj->type) {
	case _DT_ROUTINE:
		_execute_statement(s, ast, true);
		_MAKE_NIL(&tmp);
		_public_value_to_internal_object(&s->running_context->intermediate_value, &tmp);
		if(tmp.type == _DT_STRING) {
			tmp.data.string = mb_strdup(tmp.data.string, strlen(tmp.data.string) + 1);
			tmp.is_ref = false;
			mb_make_nil(s->running_context->intermediate_value);
		}
		**val_ptr = tmp;
		if(obj->data.routine->type != MB_RT_NATIVE) {
			if(*ast)
				*ast = (*ast)->prev;
		}

		break;
	case _DT_VAR:
		*val_ptr = obj->data.variable->data;
		if(*ast) *ast = (*ast)->next;

		break;
	default:
		*val_ptr = obj;
		if(*ast) *ast = (*ast)->next;

		break;
	}

	return *val_ptr;
}

/** Interpretation */

/* Callback a stepped debug handler, this function is called each step */
static int _stepped(mb_interpreter_t* s, _ls_node_t* ast) {
	int result = MB_FUNC_OK;
	_object_t* obj = 0;

	mb_assert(s);

	if(s->debug_stepped_handler) {
		if(ast && ast->data) {
			obj = (_object_t*)ast->data;
#ifdef MB_ENABLE_SOURCE_TRACE
			result = s->debug_stepped_handler(s, (void**)&ast, s->source_file, obj->source_pos, obj->source_row, obj->source_col);
#else /* MB_ENABLE_SOURCE_TRACE */
			result = s->debug_stepped_handler(s, (void**)&ast, s->source_file, obj->source_pos, 0, 0);
#endif /* MB_ENABLE_SOURCE_TRACE */
		} else {
			result = s->debug_stepped_handler(s, (void**)&ast, s->source_file, -1, 0, 0);
		}
	}

	return result;
}

/* Execute the ast, this is the core execution function */
static int _execute_statement(mb_interpreter_t* s, _ls_node_t** l, bool_t force_next) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_running_context_t* running = 0;
	_ls_node_t* sub_stack = 0;
	bool_t skip_to_eoi = true;
	bool_t end_of_ast = false;

	//mb_assert(s && l);

	running = s->running_context;
	sub_stack = s->sub_stack;

	ast = *l;

	obj = (_object_t*)ast->data;

_retry:
	switch(obj->type) {
	case _DT_FUNC:
		if(_is_binary(obj->data.func->pointer)) {
			_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if(_is_flow(obj->data.func->pointer)) {
			result = (obj->data.func->pointer)(s, (void**)&ast);
		} else {
#ifdef MB_ENABLE_STACK_TRACE
			_ls_pushback(s->stack_frames, obj->data.func->name);
#endif /* MB_ENABLE_STACK_TRACE */
			result = (obj->data.func->pointer)(s, (void**)&ast);
#ifdef MB_ENABLE_STACK_TRACE
			_ls_popback(s->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */
			_mark_dangling_intermediate_value(s, running);
		}
		if(result == MB_FUNC_IGNORE) {
			result = MB_FUNC_OK;
			obj = (_object_t*)ast->data;

			goto _retry;
		}

		break;
	case _DT_VAR:
#ifdef MB_ENABLE_CLASS
		if(obj->data.variable->data->type == _DT_ROUTINE) {
			if(ast && ast->next && _IS_FUNC(ast->next->data, _core_open_bracket)) {
				obj = obj->data.variable->data;

				goto _retry;
			} else {
				result = _core_let(s, (void**)&ast);
			}
		} else if(s->last_instance || obj->data.variable->pathing) {
			/* Need to path */
			_ls_node_t* pathed = _search_identifier_in_scope_chain(s, 0, obj->data.variable->name, _PU(obj->data.variable->pathing), 0, 0);
			if(pathed && pathed->data) {
				if(obj != (_object_t*)pathed->data) {
					/* Found another node */
#ifdef MB_ENABLE_USERTYPE_REF
					if(_try_call_func_on_usertype_ref(s, &ast, obj, pathed, &result))
						break;
#endif /* MB_ENABLE_USERTYPE_REF */

					obj = (_object_t*)pathed->data;

					goto _retry;
				} else {
					/* Final node */
					result = _core_let(s, (void**)&ast);
				}
			} else {
				/* Normal node */
				result = _core_let(s, (void**)&ast);
			}
		} else {
			/* Do not need to path */
			result = _core_let(s, (void**)&ast);
		}
#else /* MB_ENABLE_CLASS */
		result = _core_let(s, (void**)&ast);
#endif /* MB_ENABLE_CLASS */

		break;
	case _DT_ARRAY:
		result = _core_let(s, (void**)&ast);

		break;
	case _DT_INT: /* Fall through */
	case _DT_REAL: /* Fall through */
	case _DT_STRING:
		_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);

		break;
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);

		break;
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		ast = ast->prev;
		result = _core_call(s, (void**)&ast);
		_mark_dangling_intermediate_value(s, running);

		break;
#ifdef MB_ENABLE_SOURCE_TRACE
	case _DT_PREV_IMPORT:
		s->source_file = obj->data.import_info->source_file;

		break;
	case _DT_POST_IMPORT:
		s->source_file = obj->data.import_info->source_file;

		break;
#endif /* MB_ENABLE_SOURCE_TRACE */
	default: /* Do nothing */
		break;
	}

	if(s->schedule_suspend_tag) {
		if(s->schedule_suspend_tag == MB_FUNC_SUSPEND)
			mb_suspend(s, (void**)&ast);
		result = s->schedule_suspend_tag;
		s->schedule_suspend_tag = 0;
	}

	if(result != MB_FUNC_OK && result != MB_FUNC_SUSPEND && result != MB_SUB_RETURN)
		goto _exit;

	if(ast) {
		obj = DON(ast);
		if(!obj) {
			/* Do nothing */
		} else if(_IS_EOS(obj)) {
			if(force_next || result != MB_SUB_RETURN)
				ast = ast->next;
		} else if(_IS_SEP(obj, ':')) {
			skip_to_eoi = false;
			ast = ast->next;
		} else if(obj && obj->type == _DT_VAR) {
#ifdef MB_ENABLE_CLASS
			_ls_node_t* fn = 0;
			if(_is_valid_class_accessor_following_routine(s, obj->data.variable, ast, &fn)) {
				if(fn) {
					if(s->calling)
						result = _core_let(s, (void**)&ast);
				} else {
					_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
				}
			} else {
				_handle_error_on_obj(s, SE_RN_COLON_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
			}
#else /* MB_ENABLE_CLASS */
			_handle_error_on_obj(s, SE_RN_COLON_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_CLASS */
		} else if(_IS_FUNC(obj, _core_enddef) && result != MB_SUB_RETURN) {
			ast = (_ls_node_t*)_ls_popback(sub_stack);
#ifdef MB_ENABLE_LAMBDA
		} else if(obj && _IS_FUNC(obj, _core_close_bracket) && s->last_routine && s->last_routine->type == MB_RT_LAMBDA) {
			/* Do nothing */
#endif /* MB_ENABLE_LAMBDA */
		} else if(obj && obj->type == _DT_FUNC && (_is_operator(obj->data.func->pointer) || _is_flow(obj->data.func->pointer))) {
			ast = ast->next;
		} else if(obj && obj->type == _DT_FUNC) {
			/* Do nothing */
		} else if(obj && obj->type != _DT_FUNC) {
			ast = ast->next;
		} else {
			_handle_error_on_obj(s, SE_RN_COLON_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
	}

	if(skip_to_eoi && s->skip_to_eoi && s->skip_to_eoi == _ls_back(s->sub_stack)) {
		s->skip_to_eoi = 0;
		obj = (_object_t*)ast->data;
		if(obj->type != _DT_EOS) {
			result = _skip_to(s, &ast, 0, _DT_EOS);
			if(result != MB_FUNC_OK)
				goto _exit;
		}
	}

_exit:
	_destroy_lazy_objects(s);

	*l = ast;

	if(!ast) {
		ast = _ls_back(s->ast);
		end_of_ast = true;
	}


	if(s->yieldfunc)
	{
		s->yieldcounter -=1;
		if(s->yieldcounter<1)
		{
			s->yieldfunc(s);
			s->yieldcounter = yield_interval;
		}
	}
	int ret = _stepped(s, ast);
	if(result == MB_FUNC_OK)
		result = ret;

	if(end_of_ast && ast && ast->next) /* May be changed when stepping */
		*l = ast->next;


	return result;
}

/* Common function to end current looping */
static int _common_end_looping(mb_interpreter_t* s, _ls_node_t** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	result = _skip_struct(s, l, _core_for, 0, _core_next);
	if(result == MB_FUNC_OK)
		result = _skip_to(s, l, 0, _DT_EOS);

	return result;
}

/* Common function to keep current looping */
static int _common_keep_looping(mb_interpreter_t* s, _ls_node_t** l, _var_t* var_loop) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_running_context_t* running = 0;

	mb_assert(s && l);

	running = s->running_context;
	ast = *l;

	obj = (_object_t*)ast->data;
	while(!_IS_FUNC(obj, _core_next)) {
		result = _execute_statement(s, &ast, false);
		if(result == MB_LOOP_CONTINUE) { /* NEXT */
			if(!running->next_loop_var || running->next_loop_var == var_loop) { /* This loop */
				running->next_loop_var = 0;
				result = MB_FUNC_OK;

				break;
			} else { /* Not this loop */
				if(_skip_struct(s, &ast, _core_for, 0, _core_next) != MB_FUNC_OK)
					goto _exit;
				_skip_to(s, &ast, 0, _DT_EOS);

				goto _exit;
			}
		} else if(result == MB_LOOP_BREAK) { /* EXIT */
			if(_skip_struct(s, &ast, _core_for, 0, _core_next) != MB_FUNC_OK)
				goto _exit;
			_skip_to(s, &ast, 0, _DT_EOS);

			goto _exit;
		} else if(result == MB_SUB_RETURN) { /* RETURN */
			goto _exit;
		} else if(result != MB_FUNC_OK) { /* Normally */
			goto _exit;
		}

		if(!ast) {
			_handle_error_on_obj(s, SE_RN_NEXT_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		obj = (_object_t*)ast->data;
	}

_exit:
	*l = ast;

	return result;
}

/* Execute normal FOR-TO-STEP-NEXT-routine */
static int _execute_normal_for_loop(mb_interpreter_t* s, _ls_node_t** l, _var_t* var_loop) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* to_node = 0;
	_object_t* obj = 0;
	_object_t to_val;
	_object_t step_val;
	_object_t* to_val_ptr = 0;
	_object_t* step_val_ptr = 0;
	_tuple3_t ass_tuple3;
	_tuple3_t* ass_tuple3_ptr = 0;

	mb_assert(s && l && var_loop);

	ast = *l;

	to_val_ptr = &to_val;
	_MAKE_NIL(to_val_ptr);
	step_val_ptr = &step_val;
	_MAKE_NIL(step_val_ptr);
	ass_tuple3_ptr = &ass_tuple3;

	/* Get begin value */
	result = _execute_statement(s, &ast, true);
	if(result != MB_FUNC_OK)
		goto _exit;
	ast = ast->prev;

	obj = (_object_t*)ast->data;
	if(!_IS_FUNC(obj, _core_to)) {
		_handle_error_on_obj(s, SE_RN_TO_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	ast = ast->next;
	if(!ast) {
		_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	to_node = ast;

_to:
	ast = to_node;

	/* Get end value */
	result = _calc_expression(s, &ast, &to_val_ptr);
	if(result != MB_FUNC_OK)
		goto _exit;

	obj = (_object_t*)ast->data;
	if(!_IS_FUNC(obj, _core_step)) {
		step_val = _OBJ_INT_UNIT;
	} else {
		ast = ast->next;
		if(!ast) {
			_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}

		/* Get step value */
		result = _calc_expression(s, &ast, &step_val_ptr);
		if(result != MB_FUNC_OK)
			goto _exit;
	}

	if((_compare_numbers(step_val_ptr, &_OBJ_INT_ZERO) > 0 && _compare_numbers(var_loop->data, to_val_ptr) > 0) ||
		(_compare_numbers(step_val_ptr, &_OBJ_INT_ZERO) < 0 && _compare_numbers(var_loop->data, to_val_ptr) < 0)) {
		/* End looping */
		result = _common_end_looping(s, &ast);

		goto _exit;
	} else {
		/* Keep looping */
		result = _common_keep_looping(s, &ast, var_loop);
		if(result == MB_LOOP_BREAK) {
			result = MB_FUNC_OK;

			goto _exit;
		} else if(result != MB_FUNC_OK || result == MB_SUB_RETURN) {
			goto _exit;
		}

		ass_tuple3.e1 = var_loop->data;
		ass_tuple3.e2 = step_val_ptr;
		ass_tuple3.e3 = var_loop->data;
		_instruct_num_op_num(+, &ass_tuple3_ptr);

		goto _to;
	}

_exit:
	*l = ast;

	return result;
}

#ifdef MB_ENABLE_COLLECTION_LIB
/* Execute ranged FOR-IN-NEXT-routine */
static int _execute_ranged_for_loop(mb_interpreter_t* s, _ls_node_t** l, _var_t* var_loop) {
	int result = MB_FUNC_ERR;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;
	_var_t* pathed_var = 0;
	_object_t* old_val = 0;
	_ref_t* old_val_gc = 0;
	_ls_node_t* to_node = 0;
	_object_t range;
	_object_t* range_ptr = 0;
	_list_it_t* lit = 0;
	_dict_it_t* dit = 0;
	_list_it_t* tlit = 0;
	_dict_it_t* tdit = 0;
	mb_value_t ref_val;
	mb_value_t ref_it;
#if defined MB_ENABLE_USERTYPE_REF || defined MB_ENABLE_CLASS
	mb_meta_status_e os = MB_MS_NONE;
#endif /* MB_ENABLE_USERTYPE_REF || MB_ENABLE_CLASS */

	mb_assert(s && l && var_loop);

	running = s->running_context;
#ifdef MB_ENABLE_CLASS
	if(var_loop->pathing)
		pathed_var = _search_var_in_scope_chain(s, var_loop, 0);
	if(pathed_var) {
		_UNREF(pathed_var->data)
		_MAKE_NIL(pathed_var->data);
	}
#endif /* MB_ENABLE_CLASS */
	if(!pathed_var)
		pathed_var = var_loop;
	old_val = pathed_var->data;
	if(_is_ref(old_val)) {
		old_val_gc = (_ref_t*)old_val->data.pointer;
		if(!_gc_remove(old_val_gc, (void*)old_val_gc, &old_val_gc->s->gc))
			old_val_gc = 0;
	}
	range_ptr = &range;
	_MAKE_NIL(range_ptr);
	mb_make_nil(ref_val);
	mb_make_nil(ref_it);

	ast = *l;
	ast = ast->next;
	ast = ast->next;
	if(!ast) {
		_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	/* Get collection */
	result = _calc_expression(s, &ast, &range_ptr);
	if(result != MB_FUNC_OK)
		goto _exit;

	/* Create iterator */
	switch(range_ptr->type) {
#ifdef MB_ENABLE_USERTYPE_REF
	case _DT_USERTYPE_REF:
		_internal_object_to_public_value(range_ptr, &ref_val);
		os = _try_overridden(s, (void**)&ast, &ref_val, _COLL_ID_ITERATOR, MB_MF_COLL);
		if((os & MB_MS_DONE) != MB_MS_NONE && (os & MB_MS_RETURNED) != MB_MS_NONE)
			_swap_public_value(&ref_it, &running->intermediate_value);

		break;
#endif /* MB_ENABLE_USERTYPE_REF */
	case _DT_LIST:
		tlit = lit = _create_list_it(range_ptr->data.list, true);

		break;
	case _DT_DICT:
		tdit = dit = _create_dict_it(range_ptr->data.dict, true);

		break;
#ifdef MB_ENABLE_CLASS
	case _DT_CLASS:
		_internal_object_to_public_value(range_ptr, &ref_val);
		os = _try_overridden(s, (void**)&ast, &ref_val, _COLL_ID_ITERATOR, MB_MF_COLL);
		if((os & MB_MS_DONE) != MB_MS_NONE && (os & MB_MS_RETURNED) != MB_MS_NONE)
			_swap_public_value(&ref_it, &running->intermediate_value);

		break;
#endif /* MB_ENABLE_CLASS */
	default:
		_handle_error_on_obj(s, SE_RN_ITERABLE_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);

		break;
	}
	to_node = ast;
	switch(range_ptr->type) { _REF_COLL(range_ptr) _REF_USERTYPE_REF(range_ptr) default: /* Do nothing */ break; }

_to:
	ast = to_node;

	/* Move next */
#ifdef MB_ENABLE_USERTYPE_REF
	if(ref_it.type != MB_DT_NIL) {
		mb_value_t moved_next;
		mb_value_t curr_val;
		_object_t curr_obj;

		mb_make_nil(moved_next);
		mb_make_nil(curr_val);
		_MAKE_NIL(&curr_obj);

		/* Move next */
		os = _try_overridden(s, (void**)&ast, &ref_it, _COLL_ID_MOVE_NEXT, MB_MF_COLL);
		if((os & MB_MS_DONE) != MB_MS_NONE && (os & MB_MS_RETURNED) != MB_MS_NONE)
			_swap_public_value(&moved_next, &running->intermediate_value);

		if(moved_next.type == MB_DT_INT && moved_next.value.integer) {
			/* Get current value */
			os = _try_overridden(s, (void**)&ast, &ref_it, _STD_ID_GET, MB_MF_FUNC);
			if((os & MB_MS_DONE) != MB_MS_NONE && (os & MB_MS_RETURNED) != MB_MS_NONE)
				_swap_public_value(&curr_val, &running->intermediate_value);

			/* Assign loop variable */
			_public_value_to_internal_object(&curr_val, &curr_obj);
			pathed_var->data = &curr_obj;
			/* Keep looping */
			result = _common_keep_looping(s, &ast, var_loop);
			_UNREF(&curr_obj)
			if(result == MB_LOOP_BREAK) {
				result = MB_FUNC_OK;

				goto _exit;
			} else if(result != MB_FUNC_OK || result == MB_SUB_RETURN) {
				goto _exit;
			}

			goto _to;
		} else {
			/* End looping */
			result = _common_end_looping(s, &ast);

			goto _exit;
		}
	}
#endif /* MB_ENABLE_USERTYPE_REF */
	if(lit) lit = _move_list_it_next(lit);
	else if(dit) dit = _move_dict_it_next(dit);
	if((lit && _invalid_list_it(lit)) || (dit && _invalid_dict_it(dit))) {
		_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	if(!lit && !dit) {
		/* End looping */
		result = _common_end_looping(s, &ast);

		goto _exit;
	} else {
		/* Assign loop variable */
		if(lit && !lit->list->range_begin && lit->curr.node && lit->curr.node->data) {
			pathed_var->data = (_object_t*)lit->curr.node->data;
		} else if(lit && lit->list->range_begin) {
			_dispose_object(pathed_var->data);
			pathed_var->data->type = _DT_INT;
			pathed_var->data->data.integer = lit->curr.ranging;
		} else if(dit && dit->curr_node && dit->curr_node->extra) {
			pathed_var->data = (_object_t*)dit->curr_node->extra;
		}
		/* Keep looping */
		result = _common_keep_looping(s, &ast, var_loop);
		if(result == MB_LOOP_BREAK) {
			result = MB_FUNC_OK;

			goto _exit;
		} else if(result != MB_FUNC_OK || result == MB_SUB_RETURN) {
			goto _exit;
		}

		goto _to;
	}

_exit:
#ifdef MB_ENABLE_USERTYPE_REF
	if(ref_it.type != MB_DT_NIL) {
		_object_t it_obj;
		_MAKE_NIL(&it_obj);

		_public_value_to_internal_object(&ref_it, &it_obj);
		_UNREF(&it_obj)
	}
#endif /* MB_ENABLE_USERTYPE_REF */
	if(tlit) _destroy_list_it(tlit);
	else if(tdit) _destroy_dict_it(tdit);
	switch(range_ptr->type) { _UNREF_COLL(range_ptr) _UNREF_USERTYPE_REF(range_ptr) default: /* Do nothing */ break; }

	*l = ast;

	pathed_var->data = old_val;
	if(old_val_gc)
		_gc_add(old_val_gc, (void*)old_val_gc, &old_val_gc->s->gc);

	return result;
}
#endif /* MB_ENABLE_COLLECTION_LIB */

/* Skip current execution flow to a specific function */
static int _skip_to(mb_interpreter_t* s, _ls_node_t** l, mb_func_t f, _data_e t) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* tmp = 0;
	_object_t* obj = 0;

	mb_assert(s && l);

	ast = *l;
	mb_assert(ast && ast->prev);
	do {
		if(!ast) {
			_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON(tmp), MB_FUNC_ERR, _exit, result);
		}
		tmp = ast;
		obj = (_object_t*)ast->data;
		*l = ast;
		ast = ast->next;
	} while(!(_IS_FUNC(obj, f)) && obj->type != t);

_exit:
	return result;
}

/* Skip single line structure */
static bool_t _skip_single_line_struct(_ls_node_t** ast, mb_func_t func) {
	_ls_node_t* post = *ast;
	while(post && !_IS_EOS(post->data))
		post = post->next;
	if(post && post->prev && !_IS_FUNC(post->prev->data, func)) {
		*ast = post;

		return true;
	}

	return false;
}

/* Skip current IF execution flow to next chunk */
static int _skip_if_chunk(mb_interpreter_t* s, _ls_node_t** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* tmp = 0;
	_object_t* obj = 0;
	int nested = 0;
	unsigned mask = 0;

	mb_assert(s && l);

	ast = *l;
	mb_assert(ast && ast->prev);
	do {
		if(!ast) {
			_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}
		tmp = ast;
		obj = (_object_t*)ast->data;
		*l = ast;
		ast = ast->next;
		if(ast && _IS_FUNC((_object_t*)ast->data, _core_if)) {
			if(_skip_single_line_struct(&ast, _core_then))
				continue;
			if(++nested > sizeof(mask) * 8) {
				_handle_error_on_obj(s, SE_RN_NESTED_TOO_MUCH, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}
		} else if(ast && nested && _IS_FUNC((_object_t*)ast->data, _core_then)) {
			if(!(ast && ast->next && _IS_EOS(ast->next->data)))
				mask |= 1 << (nested - 1);
		} else if(ast && nested &&
			(((mask & (1 << (nested - 1))) && _IS_EOS(ast->data)) ||
			(!(mask & (1 << (nested - 1))) && _IS_FUNC((_object_t*)ast->data, _core_endif)))
		) {
			if(--nested < 0) {
				_handle_error_on_obj(s, SE_RN_INCOMPLETE_STRUCTURE, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}
			ast = ast->next;
		}
	} while(nested || (!_IS_FUNC(obj, _core_elseif) && !_IS_FUNC(obj, _core_else) && !_IS_FUNC(obj, _core_endif)));

_exit:
	return result;
}

/* Skip current structure */
static int _skip_struct(mb_interpreter_t* s, _ls_node_t** l, mb_func_t open_func, mb_func_t post_open_func, mb_func_t close_func) {
	int result = MB_FUNC_OK;
	int count = 0;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_object_t* obj_prev = 0;

	mb_assert(s && l && open_func && close_func);

	ast = *l;

	count = 1;
	do {
		if(!ast || !ast->next) {
			_handle_error_on_obj(s, SE_RN_INCOMPLETE_STRUCTURE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		obj_prev = (_object_t*)ast->data;
		ast = ast->next;
		obj = (_object_t*)ast->data;
		if(_IS_FUNC(obj, open_func)) {
			if(post_open_func) {
				if(_skip_single_line_struct(&ast, post_open_func))
					continue;
			}
			++count;
		} else if(_IS_FUNC(obj, close_func) && _IS_EOS(obj_prev)) {
			--count;
		}
	} while(count);

_exit:
	*l = ast;

	return result;
}

/* Check whether multiline statement is allowed */
static bool_t _multiline_statement(mb_interpreter_t* s) {
#if _MULTILINE_STATEMENT
	if(_ls_empty(s->multiline_enabled))
		return false;

	return (bool_t)(intptr_t)_ls_back(s->multiline_enabled)->data;
#else /* _MULTILINE_STATEMENT */
	mb_unrefvar(s);

	return false;
#endif /* _MULTILINE_STATEMENT */
}

/* Create a running context */
static _running_context_t* _create_running_context(bool_t create_var_dict) {
	_running_context_t* result = 0;

	result = (_running_context_t*)mb_malloc(sizeof(_running_context_t));
	memset(result, 0, sizeof(_running_context_t));
	result->calc_depth = _INFINITY_CALC_DEPTH;
	if(create_var_dict)
		result->var_dict = _ht_create(0, _ht_cmp_string, _ht_hash_string, 0);

	return result;
}

/* Reset the parsing context of a MY-BASIC environment */
static _parsing_context_t* _reset_parsing_context(_parsing_context_t* context) {
	_ls_node_t* imp = 0;

	if(!context)
		context = (_parsing_context_t*)mb_malloc(sizeof(_parsing_context_t));
	else
		imp = context->imported;
	memset(context, 0, sizeof(_parsing_context_t));
	context->parsing_row = 1;
	if(!imp) {
		imp = _ls_create();
	} else {
		_ls_foreach(imp, _destroy_memory);
		_ls_clear(imp);
	}
	context->imported = imp;
	_end_of_file(context);

	return context;
}

/* Destroy the parsing context of a MY-BASIC environment */
static void _destroy_parsing_context(_parsing_context_t* _UNALIGNED_ARG * context) {
	if(!context || !(*context))
		return;

	if(*context) {
		if((*context)->imported) {
			_ls_foreach((*context)->imported, _destroy_memory);
			_ls_destroy((*context)->imported);
		}
		safe_free(*context);
	}
}

/** Interface processors */

#ifdef MB_ENABLE_MODULE
/* Create a module function structure */
static _module_func_t* _create_module_func(mb_interpreter_t* s, mb_func_t f) {
	_module_func_t* result = 0;

	mb_assert(s);

	if(!s->with_module)
		return result;

	result = (_module_func_t*)mb_malloc(sizeof(_module_func_t));
	result->module = mb_strdup(s->with_module, 0);
	result->func = f;

	return result;
}

/* Destroy a module function structure */
static int _ls_destroy_module_func(void* data, void* extra) {
	int result = _OP_RESULT_DEL_NODE;
	_module_func_t* mod = 0;
	mb_unrefvar(extra);

	mb_assert(data);

	mod = (_module_func_t*)data;
	safe_free(mod->module);
	safe_free(mod);

	return result;
}

/* Destroy all module function structures */
static int _ht_destroy_module_func_list(void* data, void* extra) {
	int result = _OP_RESULT_DEL_NODE;
	_ls_node_t* lst = 0;
	char* n = 0;

	mb_assert(data);

	lst = (_ls_node_t*)data;
	n = (char*)extra;
	_ls_foreach(lst, _ls_destroy_module_func);
	_ls_destroy(lst);
	safe_free(n);

	return result;
}
#endif /* MB_ENABLE_MODULE */

/* Generate a function name to be registered according to module information */
static char* _generate_func_name(mb_interpreter_t* s, char* n, bool_t with_mod) {
	char* name = 0;
	size_t _sl = 0;

	mb_assert(s && n);

	_sl = strlen(n);
#ifdef MB_ENABLE_MODULE
	if(with_mod && s->with_module) {
		size_t _ml = strlen(s->with_module);
		name = (char*)mb_malloc(_ml + 1 + _sl + 1);
		memcpy(name, s->with_module, _ml);
		name[_ml] = '.';
		memcpy(name + _ml + 1, n, _sl + 1);
	} else {
		name = (char*)mb_malloc(_sl + 1);
		memcpy(name, n, _sl + 1);
	}
#else /* MB_ENABLE_MODULE */
	mb_unrefvar(with_mod);
	name = (char*)mb_malloc(_sl + 1);
	memcpy(name, n, _sl + 1);
#endif /* MB_ENABLE_MODULE */
	mb_strupr(name);

	return name;
}

/* Register a function to a MY-BASIC environment */
static int _register_func(mb_interpreter_t* s, char* n, mb_func_t f, bool_t local) {
	int result = 0;
	_ht_node_t* scope = 0;
	_ls_node_t* exists = 0;
	char* name = 0;

	mb_assert(s);

	if(!n)
		return result;

	n = mb_strdup(n, strlen(n) + 1);
	mb_strupr(n);

	scope = local ? s->local_func_dict : s->global_func_dict;
#ifdef MB_ENABLE_MODULE
	if(s->with_module)
		name = _generate_func_name(s, n, true);
#endif /* MB_ENABLE_MODULE */
	if(!name)
		name = mb_strdup(n, strlen(n) + 1);
	exists = _ht_find(scope, (void*)name);
	if(!exists) {
		result += _ht_set_or_insert(scope, (void*)name, (void*)(intptr_t)f);
	} else {
		_set_current_error(s, SE_CM_FUNC_EXISTS, 0);
		safe_free(name);
	}

#ifdef MB_ENABLE_MODULE
	if(s->with_module) {
		_ls_node_t* tmp = 0;
		exists = _ht_find(s->module_func_dict, (void*)n);
		if(!exists) {
			name = _generate_func_name(s, n, false);
			result += _ht_set_or_insert(s->module_func_dict, (void*)name, _ls_create());
		}
		exists = _ht_find(s->module_func_dict, (void*)n);
		exists = (_ls_node_t*)exists->data;
		tmp = _ls_find(exists, s, _ls_cmp_module_func, 0);
		if(!tmp)
			_ls_pushback(exists, _create_module_func(s, f));
		else
			_set_current_error(s, SE_CM_FUNC_EXISTS, 0);
	}
#endif /* MB_ENABLE_MODULE */

	safe_free(n);

	return result;
}

/* Remove a function from a MY-BASIC environment */
static int _remove_func(mb_interpreter_t* s, char* n, bool_t local) {
	int result = 0;
	_ht_node_t* scope = 0;
	_ls_node_t* exists = 0;
	char* name = 0;

	mb_assert(s);

	if(!n)
		return result;

	n = mb_strdup(n, strlen(n) + 1);
	mb_strupr(n);

	scope = local ? s->local_func_dict : s->global_func_dict;
#ifdef MB_ENABLE_MODULE
	if(s->with_module)
		name = _generate_func_name(s, n, true);
#endif /* MB_ENABLE_MODULE */
	if(!name)
		name = mb_strdup(n, strlen(n) + 1);
	exists = _ht_find(scope, (void*)name);
	if(exists)
		result += _ht_remove(scope, (void*)name, _ls_cmp_extra_string);
	else
		_set_current_error(s, SE_CM_FUNC_NOT_EXISTS, 0);
	safe_free(name);

#ifdef MB_ENABLE_MODULE
	if(s->with_module) {
		_ls_node_t* tmp = 0;
		exists = _ht_find(s->module_func_dict, (void*)n);
		if(exists) {
			exists = (_ls_node_t*)exists->data;
			tmp = _ls_find(exists, s, _ls_cmp_module_func, 0);
			if(tmp)
				_ls_remove(exists, tmp, _ls_destroy_module_func);
		}
	}
#endif /* MB_ENABLE_MODULE */

	safe_free(n);

	return result;
}

/* Find function interface in the function dictionaries */
static _ls_node_t* _find_func(mb_interpreter_t* s, char* n, bool_t* mod) {
	_ls_node_t* result = 0;
	mb_unrefvar(mod);

	mb_assert(s && n);

	n = mb_strdup(n, strlen(n) + 1);
	mb_strupr(n);

	result = _ht_find(s->local_func_dict, (void*)n);
	if(!result)
		result = _ht_find(s->global_func_dict, (void*)n);

#ifdef MB_ENABLE_MODULE
	if(!result) {
		result = _ht_find(s->module_func_dict, (void*)n);
		if(result && result->data) {
			_module_func_t* mp = 0;
			result = (_ls_node_t*)result->data;
			result = result->next;
			while(result) {
				mp = (_module_func_t*)result->data;
				if(_ls_find(s->using_modules, mp->module, (_ls_compare_t)_ht_cmp_string, 0))
					break;
				result = result->next;
			}
			*mod = true;
		}
	}
#endif /* MB_ENABLE_MODULE */

	safe_free(n);

	return result;
}

/* Open global constants */
static int _open_constant(mb_interpreter_t* s) {
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	unsigned long ul = 0;

	mb_assert(s);

	running = s->running_context;

	ul = _ht_set_or_insert(running->var_dict, (void*)MB_TRUE, _OBJ_BOOL_TRUE);
	mb_assert(ul);
	ul = _ht_set_or_insert(running->var_dict, (void*)MB_FALSE, _OBJ_BOOL_FALSE);
	mb_assert(ul);

	return result;
}

/* Close global constants */
static int _close_constant(mb_interpreter_t* s) {
	int result = MB_FUNC_OK;

	mb_assert(s);

	return result;
}

/* Open the core function library */
static int _open_core_lib(mb_interpreter_t* s) {
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < countof(_core_libs); ++i)
		result += _register_func(s, _core_libs[i].name, _core_libs[i].pointer, true);

	return result;
}

/* Close the core function library */
static int _close_core_lib(mb_interpreter_t* s) {
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < countof(_core_libs); ++i)
		result += _remove_func(s, _core_libs[i].name, true);

	return result;
}

/* Open the standard function library */
static int _open_std_lib(mb_interpreter_t* s) {
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < countof(_std_libs); ++i)
		result += _register_func(s, _std_libs[i].name, _std_libs[i].pointer, true);

	return result;
}

/* Close the standard function library */
static int _close_std_lib(mb_interpreter_t* s) {
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < countof(_std_libs); ++i)
		result += _remove_func(s, _std_libs[i].name, true);

	return result;
}

#ifdef MB_ENABLE_COLLECTION_LIB
/* Open the collection function library */
static int _open_coll_lib(mb_interpreter_t* s) {
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < countof(_coll_libs); ++i)
		result += _register_func(s, _coll_libs[i].name, _coll_libs[i].pointer, true);

	return result;
}

/* Close the collection function library */
static int _close_coll_lib(mb_interpreter_t* s) {
	int result = 0;
	int i = 0;

	mb_assert(s);

	for(i = 0; i < countof(_coll_libs); ++i)
		result += _remove_func(s, _coll_libs[i].name, true);

	return result;
}
#endif /* MB_ENABLE_COLLECTION_LIB */

/* ========================================================} */

/*
** {========================================================
** Public functions definitions
*/

/* Get the version number of this MY-BASIC system */
unsigned long mb_ver(void) {
	return MB_VERSION;
}

/* Get the version text of this MY-BASIC system */
const char* mb_ver_string(void) {
	return MB_VERSION_STRING;
}

/* Initialize the MY-BASIC system */
int mb_init(void) {
	if(_exp_assign)
		return MB_FUNC_ERR;

	_exp_assign = _create_object();
	_exp_assign->type = _DT_FUNC;
	_exp_assign->data.func = (_func_t*)mb_malloc(sizeof(_func_t));
	memset(_exp_assign->data.func, 0, sizeof(_func_t));
	_exp_assign->data.func->name = (char*)mb_malloc(strlen(_DUMMY_ASSIGN_CHAR) + 1);
	memcpy(_exp_assign->data.func->name, _DUMMY_ASSIGN_CHAR, strlen(_DUMMY_ASSIGN_CHAR) + 1);
	_exp_assign->data.func->pointer = _core_dummy_assign;

	mb_assert(!_OBJ_BOOL_TRUE);
	if(!_OBJ_BOOL_TRUE) {
		_var_t* bvar = _create_var(&_OBJ_BOOL_TRUE, MB_TRUE, strlen(MB_TRUE) + 1, true);
		bvar->data->type = _DT_INT;
		bvar->data->data.integer = 1;
	}
	mb_assert(!_OBJ_BOOL_FALSE);
	if(!_OBJ_BOOL_FALSE) {
		_var_t* bvar = _create_var(&_OBJ_BOOL_FALSE, MB_FALSE, strlen(MB_FALSE) + 1, true);
		bvar->data->type = _DT_INT;
		bvar->data->data.integer = 0;
	}

	return MB_FUNC_OK;
}

/* Close the MY-BASIC system */
int mb_dispose(void) {
	if(!_exp_assign)
		return MB_FUNC_ERR;

	safe_free(_exp_assign->data.func->name);
	safe_free(_exp_assign->data.func);
	safe_free(_exp_assign);
	_exp_assign = 0;

	mb_assert(_OBJ_BOOL_TRUE);
	if(_OBJ_BOOL_TRUE) {
		safe_free(_OBJ_BOOL_TRUE->data.variable->data);
		safe_free(_OBJ_BOOL_TRUE->data.variable->name);
		safe_free(_OBJ_BOOL_TRUE->data.variable);
		safe_free(_OBJ_BOOL_TRUE);
		_OBJ_BOOL_TRUE = 0;
	}
	mb_assert(_OBJ_BOOL_FALSE);
	if(_OBJ_BOOL_FALSE) {
		safe_free(_OBJ_BOOL_FALSE->data.variable->data);
		safe_free(_OBJ_BOOL_FALSE->data.variable->name);
		safe_free(_OBJ_BOOL_FALSE->data.variable);
		safe_free(_OBJ_BOOL_FALSE);
		_OBJ_BOOL_FALSE = 0;
	}

	return MB_FUNC_OK;
}



//Calling export(var) will assign the value of var to a variable
//with the same name to the interpreter's parent's global scope
//Or this global scope, if the interpreter doesn't have one.
static int _mb_export(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	void* v0 = 0;
	char *n = 0;
	mb_value_t val0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	//Get the name of the variable
	mb_check(mb_get_var(s, l, &v0, true));
	mb_check(mb_get_var_name( s,v0, n));
	
	//Get the actual value of that var, and stash
	mb_check(mb_get_var_value(s, v0, &val0));
	mb_check(mb_attempt_close_bracket(s, l));

	if(s->parent)
	{
		mb_add_var(s->parent,l, n,val0,true);
	}
	else
	{
		mb_add_var(s,l, n,val0,true);
	}
	return result;
}
//Open an interpreter that shares a global function dict,
//And exists in a child scope of the parent. So you 
int mb_open_child(struct mb_interpreter_t** s, struct mb_interpreter_t** parent) {
	mb_open(s);
	(*s)->global_func_dict = (*parent)->global_func_dict;
	(*s)->printer = (*parent)->printer;
	(*s)->inputer = (*parent)->inputer;
	(*s)->error_handler = (*parent)->error_handler;
	(*s)->import_handler = (*parent)->import_handler;

	(*s)->running_context->prev = (*parent)->running_context;
	(*s)-> parent = *parent;
	mb_register_func(*s, "EXPORT", _mb_export);

}


/* Open a MY-BASIC environment */
int mb_open(struct mb_interpreter_t** s) {
	int result = MB_FUNC_OK;
	_ht_node_t* local_scope = 0;
	_ht_node_t* global_scope = 0;
	_running_context_t* running = 0;

	if(!s)
		return MB_FUNC_ERR;

	*s = (mb_interpreter_t*)mb_malloc(sizeof(mb_interpreter_t));
	memset(*s, 0, sizeof(mb_interpreter_t));
	(*s)->valid = true;

	(*s)-> parent = 0;

	local_scope = _ht_create(_HT_ARRAY_SIZE_GLOBALSCOPES, _ht_cmp_string, _ht_hash_string, _ls_free_extra);
	(*s)->local_func_dict = local_scope;

	global_scope = _ht_create(_HT_ARRAY_SIZE_GLOBALSCOPES, _ht_cmp_string, _ht_hash_string, _ls_free_extra);
	(*s)->global_func_dict = global_scope;

#ifdef MB_ENABLE_MODULE
	global_scope = _ht_create(_HT_ARRAY_SIZE_GLOBALSCOPES, _ht_cmp_string, _ht_hash_string, _ht_destroy_module_func_list);
	(*s)->module_func_dict = global_scope;
	(*s)->using_modules = _ls_create();
#endif /* MB_ENABLE_MODULE */

	(*s)->parsing_context = _reset_parsing_context((*s)->parsing_context);

	(*s)->edge_destroy_objects = _ls_create();
	(*s)->lazy_destroy_objects = _ls_create();

	(*s)->gc.table = _ht_create(_HT_ARRAY_SIZE_GC, _ht_cmp_ref, _ht_hash_ref, _do_nothing_on_object);
	(*s)->gc.recursive_table = _ht_create(_HT_ARRAY_SIZE_GC, _ht_cmp_ref, _ht_hash_ref, _do_nothing_on_object);
	(*s)->gc.collected_table = _ht_create(_HT_ARRAY_SIZE_GC, _ht_cmp_ref, _ht_hash_ref, _do_nothing_on_object);
	(*s)->gc.valid_table = 0;
	(*s)->gc.collecting = 0;

	running = _create_running_context(true);
	running->meta = _SCOPE_META_ROOT;
	(*s)->running_context = running;

	(*s)->sub_stack = _ls_create();

	(*s)->in_neg_expr = _ls_create();

#ifdef MB_ENABLE_STACK_TRACE
	(*s)->stack_frames = _ls_create();
#endif /* MB_ENABLE_STACK_TRACE */
#if _MULTILINE_STATEMENT
	(*s)->multiline_enabled = _ls_create();
#endif /* _MULTILINE_STATEMENT */

	(*s)->ast = _ls_create();

	_open_core_lib(*s);
	_open_std_lib(*s);
#ifdef MB_ENABLE_COLLECTION_LIB
	_open_coll_lib(*s);
#endif /* MB_ENABLE_COLLECTION_LIB */

	result = _open_constant(*s);
	mb_assert(MB_FUNC_OK == result);

	return result;
}

/* Close a MY-BASIC environment */
int mb_close(struct mb_interpreter_t** s) {
	_ht_node_t* local_scope = 0;
	_ht_node_t* global_scope = 0;
	_ls_node_t* ast = 0;

	if(!s || !(*s))
		return MB_FUNC_ERR;

#ifdef MB_ENABLE_FORK
	if((*s)->forked_from)
		return mb_join(s);
#endif /* MB_ENABLE_FORK */

	(*s)->valid = false;

#ifdef MB_ENABLE_COLLECTION_LIB
	_close_coll_lib(*s);
#endif /* MB_ENABLE_COLLECTION_LIB */
	_close_std_lib(*s);
	_close_core_lib(*s);

	ast = (*s)->ast;
	/*
	_ls_node_t* ast_i = ast; 
	_ls_node_t* last = 0; 
	_ls_node_t* before_func = 0; 
	
	if(!(_IS_FUNC((_object_t*)(ast_i->data), _core_def)))
	{
		//Loop over the whole collection of AST nodes,
		//and unlink the ones that are functions.
		//They are just floating, and we depend on garbage
		//collection of the actual function object to clean the, 
		while(ast_i)
		{
			if (_IS_FUNC((_object_t*)(ast_i->data), _core_def))
			{
				before_func= last;
			}
		if (_IS_FUNC((_object_t*)(ast_i->data), _core_enddef))
			{
				if(ast_i->next)
				{
					before_func->next =  ast_i->next; 
				}
				//Nothing after the def to splice
				else
				{
					before_func->next=0;
				}
			}
			last = ast_i;
			ast_i=ast_i->next;
		}
	}
	*/
	_ls_foreach(ast, _destroy_object);
	_ls_destroy(ast);

	_ls_destroy((*s)->sub_stack);

	_ls_destroy((*s)->in_neg_expr);

#ifdef MB_ENABLE_STACK_TRACE
	_ls_destroy((*s)->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */
#if _MULTILINE_STATEMENT
	_ls_destroy((*s)->multiline_enabled);
#endif /* _MULTILINE_STATEMENT */

	_tidy_scope_chain(*s);
	_dispose_scope_chain(*s);

	(*s)->gc.disabled = false;
	_gc_collect_garbage(*s, -1);
	_ht_destroy((*s)->gc.table);
	_ht_destroy((*s)->gc.recursive_table);
	_ht_destroy((*s)->gc.collected_table);
	(*s)->gc.table = 0;
	(*s)->gc.recursive_table = 0;
	(*s)->gc.collected_table = 0;

#ifdef MB_ENABLE_FORK
	if((*s)->all_forked) {
		mb_assert(_ls_count((*s)->all_forked) == 0);
		_ls_destroy((*s)->all_forked);
	}
#endif /* MB_ENABLE_FORK */

	_ls_foreach((*s)->edge_destroy_objects, _destroy_object);
	_ls_destroy((*s)->edge_destroy_objects);
	_ls_foreach((*s)->lazy_destroy_objects, _destroy_object);
	_ls_destroy((*s)->lazy_destroy_objects);

	_destroy_parsing_context(&(*s)->parsing_context);

#ifdef MB_ENABLE_MODULE
	global_scope = (*s)->module_func_dict;
	_ht_foreach(global_scope, _ht_destroy_module_func_list);
	_ht_destroy(global_scope);
	_ls_foreach((*s)->using_modules, _destroy_memory);
	_ls_destroy((*s)->using_modules);
#endif /* MB_ENABLE_MODULE */

	global_scope = (*s)->global_func_dict;
	_ht_foreach(global_scope, _ls_free_extra);
	_ht_destroy(global_scope);

	local_scope = (*s)->local_func_dict;
	_ht_foreach(local_scope, _ls_free_extra);
	_ht_destroy(local_scope);

	_close_constant(*s);

	safe_free(*s);

	return MB_FUNC_OK;
}

/* Reset a MY-BASIC environment */
int _mb_reset(struct mb_interpreter_t** s, bool_t clrf, bool_t clearvars) {
	int result = MB_FUNC_OK;
	_ht_node_t* global_scope = 0;
	_ls_node_t* ast;
	_running_context_t* running = 0;
	if(!s || !(*s))
		return MB_FUNC_ERR;

	(*s)->run_count = 0;
	(*s)->has_run = false;
	(*s)->jump_set = _JMP_NIL;
	(*s)->yieldcounter = yield_interval;
#ifdef MB_ENABLE_CLASS
	(*s)->last_instance = 0;
	(*s)->calling = false;
#endif /* MB_ENABLE_CLASS */
	(*s)->last_routine = 0;
	(*s)->no_eat_comma_mark = 0;
	(*s)->handled_error = false;
	(*s)->last_error = SE_NO_ERR;
	(*s)->last_error_file = 0;

	running = (*s)->running_context;
	(*s)->suspent_point = 0;
	running->next_loop_var = 0;
	memset(&(running->intermediate_value), 0, sizeof(mb_value_t));

	ast = (*s)->ast;
	_ls_foreach(ast, _destroy_object);
	_ls_clear(ast);

	_ls_clear((*s)->sub_stack);

#ifdef MB_ENABLE_STACK_TRACE
	_ls_clear((*s)->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */
#if _MULTILINE_STATEMENT
	_ls_clear((*s)->multiline_enabled);
#endif /* _MULTILINE_STATEMENT */

	if(clearvars)
	{
		_tidy_scope_chain(*s);
		_clear_scope_chain(*s);
	}
#ifdef MB_ENABLE_FORK
	if((*s)->all_forked) {
		mb_assert(_ls_count((*s)->all_forked) == 0);
		_ls_clear((*s)->all_forked);
	}
#endif /* MB_ENABLE_FORK */

	(*s)->parsing_context = _reset_parsing_context((*s)->parsing_context);

	if(clrf) {
#ifdef MB_ENABLE_MODULE
		global_scope = (*s)->module_func_dict;
		_ht_foreach(global_scope, _ht_destroy_module_func_list);
		_ht_clear(global_scope);
		_ls_foreach((*s)->using_modules, _destroy_memory);
		_ls_clear((*s)->using_modules);
#endif /* MB_ENABLE_MODULE */

		global_scope = (*s)->global_func_dict;
		_ht_foreach(global_scope, _ls_free_extra);
		_ht_clear(global_scope);
	}

	result = _open_constant(*s);
	mb_assert(MB_FUNC_OK == result);

	return result;
}


int mb_reset(struct mb_interpreter_t** s, bool_t clrf) {
	_mb_reset(s, clrf, true);
}

int mb_reset_preserve(struct mb_interpreter_t** s, bool_t clrf) {
	_mb_reset(s, clrf, false);
}
/* Join a forked MY-BASIC environment */
int mb_join(struct mb_interpreter_t** s) {
#ifdef MB_ENABLE_FORK
	int result = MB_FUNC_OK;
	mb_interpreter_t* src = 0;

	if(!s || !(*s) || !(*s)->forked_from)
		return MB_FUNC_ERR;

	src = *s;
	while(mb_get_forked_from(src, &src) == MB_FUNC_OK) {
		/* Do nothing */
	}

	(*s)->valid = false;

	_ls_destroy((*s)->sub_stack);

	_ls_destroy((*s)->in_neg_expr);

#ifdef MB_ENABLE_STACK_TRACE
	_ls_destroy((*s)->stack_frames);
#endif /* MB_ENABLE_STACK_TRACE */
#if _MULTILINE_STATEMENT
	_ls_destroy((*s)->multiline_enabled);
#endif /* _MULTILINE_STATEMENT */

	(*s)->forked_context->prev = 0;
	(*s)->running_context = (*s)->forked_context;
	_dispose_scope_chain(*s);

	_ls_foreach((*s)->edge_destroy_objects, _destroy_object);
	_ls_destroy((*s)->edge_destroy_objects);
	_ls_foreach((*s)->lazy_destroy_objects, _destroy_object);
	_ls_destroy((*s)->lazy_destroy_objects);

	if(src->all_forked)
		_ls_try_remove(src->all_forked, *s, _ls_cmp_data, 0);

	safe_free(*s);

	return result;
#else /* MB_ENABLE_FORK */
	mb_unrefvar(s);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_FORK */
}

/* Get the source MY-BASIC environment of a forked one */
int mb_get_forked_from(struct mb_interpreter_t* s, struct mb_interpreter_t** src) {
#ifdef MB_ENABLE_FORK
	int result = MB_FUNC_OK;

	if(!s || !src)
		result = MB_FUNC_ERR;
	else if(s->forked_from == 0)
		result = MB_FUNC_ERR;
	else
		*src = s->forked_from;

	return result;
#else /* MB_ENABLE_FORK */
	mb_unrefvar(s);
	mb_unrefvar(src);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_FORK */
}

/* Register an API function to a MY-BASIC environment */
int mb_register_func(struct mb_interpreter_t* s, const char* n, mb_func_t f) {
	if(!s || !n || !f) return 0;

	return _register_func(s, (char*)n, f, false);
}

/* Remove an API function from a MY-BASIC environment */
int mb_remove_func(struct mb_interpreter_t* s, const char* n) {
	if(!s || !n) return 0;

	return _remove_func(s, (char*)n, false);
}

/* Remove a reserved API from a MY-BASIC environment */
int mb_remove_reserved_func(struct mb_interpreter_t* s, const char* n) {
	if(!s || !n) return 0;

	return _remove_func(s, (char*)n, true);
}

/* Begin a module, all functions registered within a module will put inside it */
int mb_begin_module(struct mb_interpreter_t* s, const char* n) {
	int result = MB_FUNC_OK;

	if(!s || !n) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

#ifdef MB_ENABLE_MODULE
	if(s->with_module) {
		_handle_error_on_obj(s, SE_PS_INVALID_MODULE, s->source_file, (_object_t*)0, MB_FUNC_ERR, _exit, result);
	} else {
		s->with_module = mb_strdup(n, strlen(n) + 1);
	}
#else /* MB_ENABLE_MODULE */
	_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, (_object_t*)0, MB_FUNC_WARNING, _exit, result);
#endif /* MB_ENABLE_MODULE */

_exit:
	return result;
}

/* End a module */
int mb_end_module(struct mb_interpreter_t* s) {
	int result = MB_FUNC_OK;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

#ifdef MB_ENABLE_MODULE
	if(s->with_module) {
		safe_free(s->with_module);
	} else {
		_handle_error_on_obj(s, SE_PS_INVALID_MODULE, s->source_file, (_object_t*)0, MB_FUNC_ERR, _exit, result);
	}

#else /* MB_ENABLE_MODULE */
	_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, (_object_t*)0, MB_FUNC_WARNING, _exit, result);
#endif /* MB_ENABLE_MODULE */

_exit:
	return result;
}

/* Try attempting to begin an API function */
int mb_attempt_func_begin(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

#if _MULTILINE_STATEMENT
	_ls_pushback(s->multiline_enabled, (void*)(intptr_t)false);
#endif /* _MULTILINE_STATEMENT */
	ast = (_ls_node_t*)*l;
	obj = (_object_t*)ast->data;
	if(!(obj->type == _DT_FUNC)) {
#if _MULTILINE_STATEMENT
		_ls_popback(s->multiline_enabled);
#endif /* _MULTILINE_STATEMENT */
		_handle_error_on_obj(s, SE_RN_INCOMPLETE_STRUCTURE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;

	++s->no_eat_comma_mark;

_exit:
	*l = ast;

	return result;
}

/* Try attempting to end an API function */
int mb_attempt_func_end(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	if(!s || !l) {
		result = MB_FUNC_ERR;
	} else {
#if _MULTILINE_STATEMENT
		_ls_popback(s->multiline_enabled);
#endif /* _MULTILINE_STATEMENT */
		--s->no_eat_comma_mark;
	}

	return result;
}

/* Try attempting an open bracket */
int mb_attempt_open_bracket(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	ast = (_ls_node_t*)*l;
#if _MULTILINE_STATEMENT
	_ls_pushback(s->multiline_enabled, (void*)(intptr_t)true);
	do {
		ast = ast->next;
		obj = ast ? (_object_t*)ast->data : 0;
	} while(obj && obj->type == _DT_EOS);
#else /* _MULTILINE_STATEMENT */
	ast = ast->next;
	obj = ast ? (_object_t*)ast->data : 0;
#endif /* _MULTILINE_STATEMENT */
	if(!obj || !_IS_FUNC(obj, _core_open_bracket)) {
#if _MULTILINE_STATEMENT
		_ls_popback(s->multiline_enabled);
#endif /* _MULTILINE_STATEMENT */
		_handle_error_on_obj(s, SE_RN_OPEN_BRACKET_EXPECTED, s->source_file, ast ? DON(ast) : DON2(l), MB_FUNC_ERR, _exit, result);
	}
	if(ast) ast = ast->next;

_exit:
	*l = ast;

	return result;
}

/* Try attempting a close bracket */
int mb_attempt_close_bracket(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	ast = (_ls_node_t*)*l;
	if(!ast) {
		_handle_error_on_obj(s, SE_RN_CLOSE_BRACKET_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
#if _MULTILINE_STATEMENT
	_ls_popback(s->multiline_enabled);
	obj = (_object_t*)ast->data;
	while(obj && obj->type == _DT_EOS) {
		ast = ast->next;
		obj = ast ? (_object_t*)ast->data : 0;
	}
#else /* _MULTILINE_STATEMENT */
	obj = ast ? (_object_t*)ast->data : 0;
#endif /* _MULTILINE_STATEMENT */
	if(!obj || !_IS_FUNC(obj, _core_close_bracket)) {
		_handle_error_on_obj(s, SE_RN_CLOSE_BRACKET_EXPECTED, s->source_file, ast ? DON(ast) : DON2(l), MB_FUNC_ERR, _exit, result);
	}
	if(ast) ast = ast->next;

_exit:
	*l = ast;

	return result;
}

/* Detect if there is any more argument */
int mb_has_arg(struct mb_interpreter_t* s, void** l) {
	int result = 0;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;

	if(!s || !l)
		goto _exit;

	ast = (_ls_node_t*)*l;
	if(ast) {
#if _MULTILINE_STATEMENT
		if(_multiline_statement(s)) {
			obj = (_object_t*)ast->data;
			while(obj && obj->type == _DT_EOS) {
				ast = ast->next;
				obj = ast ? (_object_t*)ast->data : 0;
			}
		} else {
			obj = (_object_t*)ast->data;
		}
#else /* _MULTILINE_STATEMENT */
		obj = ast ? (_object_t*)ast->data : 0;
#endif /* _MULTILINE_STATEMENT */
		if(obj && !_IS_FUNC(obj, _core_close_bracket) && obj->type != _DT_EOS)
			result = obj->type != _DT_SEP && obj->type != _DT_EOS;
	}

_exit:
	return result;
}

/* Pop an integer argument */
int mb_pop_int(struct mb_interpreter_t* s, void** l, int_t* val) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	if(val) *val = 0;

	if(!s || !l || !val) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_nil(arg);

	mb_check(mb_pop_value(s, l, &arg));

	switch(arg.type) {
	case MB_DT_INT:
		*val = arg.value.integer;

		break;
	case MB_DT_REAL:
		*val = (int_t)(arg.value.float_point);

		break;
	default:
		_assign_public_value(s, &arg, 0, false);
#if _SIMPLE_ARG_ERROR
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
#else /* _SIMPLE_ARG_ERROR */
		result = MB_FUNC_ERR;
#endif /* _SIMPLE_ARG_ERROR */

		goto _exit;
	}

_exit:
	return result;
}

/* Pop a float point argument */
int mb_pop_real(struct mb_interpreter_t* s, void** l, real_t* val) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	if(val) *val = 0;

	if(!s || !l || !val) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_nil(arg);

	mb_check(mb_pop_value(s, l, &arg));

	switch(arg.type) {
	case MB_DT_INT:
		*val = (real_t)(arg.value.integer);

		break;
	case MB_DT_REAL:
		*val = arg.value.float_point;

		break;
	default:
		_assign_public_value(s, &arg, 0, false);
#if _SIMPLE_ARG_ERROR
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
#else /* _SIMPLE_ARG_ERROR */
		result = MB_FUNC_ERR;
#endif /* _SIMPLE_ARG_ERROR */

		goto _exit;
	}

_exit:
	return result;
}

/* Pop a string argument */
int mb_pop_string(struct mb_interpreter_t* s, void** l, char** val) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	if(val) *val = 0;

	if(!s || !l || !val) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_nil(arg);

	mb_check(mb_pop_value(s, l, &arg));

	switch(arg.type) {
	case MB_DT_STRING:
		*val = arg.value.string;

		break;
	default:
		_assign_public_value(s, &arg, 0, false);
#if _SIMPLE_ARG_ERROR
		_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
#else /* _SIMPLE_ARG_ERROR */
		result = MB_FUNC_ERR;
#endif /* _SIMPLE_ARG_ERROR */

		goto _exit;
	}

_exit:
	return result;
}

/* Pop a usertype argument */
int mb_pop_usertype(struct mb_interpreter_t* s, void** l, void** val) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	if(val) *val = 0;

	if(!s || !l || !val) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_nil(arg);

	mb_check(mb_pop_value(s, l, &arg));

	switch(arg.type) {
	case MB_DT_USERTYPE:
		*val = arg.value.usertype;

		break;
	default:
		_assign_public_value(s, &arg, 0, false);
		result = MB_FUNC_ERR;

		goto _exit;
	}

_exit:
	return result;
}

/* Pop an argument value */
int mb_pop_value(struct mb_interpreter_t* s, void** l, mb_value_t* val) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t val_obj;
	_object_t* val_ptr = 0;
	_running_context_t* running = 0;
	int* inep = 0;

	if(!s || !l || !val) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	running = s->running_context;

	if(!_ls_empty(s->in_neg_expr))
		inep = (int*)_ls_back(s->in_neg_expr)->data;

	val_ptr = &val_obj;
	_MAKE_NIL(val_ptr);

#ifdef MB_ENABLE_USERTYPE_REF
	if(s->usertype_ref_ahead) {
		ast = (_ls_node_t*)*l;
		memcpy(val_ptr, s->usertype_ref_ahead, sizeof(_object_t));
		s->usertype_ref_ahead = 0;

		goto _got;
	}
#endif /* MB_ENABLE_USERTYPE_REF */
	ast = (_ls_node_t*)*l;
	if(!ast)
		goto _exit;
#if _MULTILINE_STATEMENT
	if(_multiline_statement(s)) {
		_object_t* obj = 0;
		obj = (_object_t*)ast->data;
		while(obj && obj->type == _DT_EOS) {
			ast = ast->next;
			obj = ast ? (_object_t*)ast->data : 0;
		}
	}
#endif /* _MULTILINE_STATEMENT */
	result = _calc_expression(s, &ast, &val_ptr);
	if(result != MB_FUNC_OK)
		goto _exit;

#ifdef MB_ENABLE_USERTYPE_REF
_got:
#endif /* MB_ENABLE_USERTYPE_REF */
	if(val_ptr->type == _DT_STRING && !val_ptr->is_ref) {
		_destroy_edge_objects(s);
		_mark_edge_destroy_string(s, val_ptr->data.string);
	}
	_REF(val_ptr)

	if(s->no_eat_comma_mark < _NO_EAT_COMMA && (!inep || (inep && !(*inep)))) {
		if(ast && _IS_SEP(ast->data, ','))
			ast = ast->next;
	}

	result = _internal_object_to_public_value(val_ptr, val);
	if(result != MB_FUNC_OK)
		goto _exit;

_exit:
	*l = ast;

	return result;
}

/* Push an integer argument */
int mb_push_int(struct mb_interpreter_t* s, void** l, int_t val) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_int(arg, val);
	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

/* Push a float point argument */
int mb_push_real(struct mb_interpreter_t* s, void** l, real_t val) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_real(arg, val);
	mb_convert_to_int_if_posible(arg);
	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

/* Push a string argument */
int mb_push_string(struct mb_interpreter_t* s, void** l, char* val) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_string(arg, val);
	mb_check(mb_push_value(s, l, arg));
	_mark_lazy_destroy_string(s, val);

_exit:
	return result;
}

/* Push a usertype argument */
int mb_push_usertype(struct mb_interpreter_t* s, void** l, void* val) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_usertype(arg, val);
	mb_check(mb_push_value(s, l, arg));

_exit:
	return result;
}

/* Push an argument value */
int mb_push_value(struct mb_interpreter_t* s, void** l, mb_value_t val) {
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_object_t obj;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	running = s->running_context;
	_assign_public_value(s, &running->intermediate_value, &val, false);

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&running->intermediate_value, &obj);
	_REF(&obj)

	_gc_try_trigger(s);

_exit:
	return result;
}

/* Begin a class */
int mb_begin_class(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t** meta, int c, mb_value_t* out) {
#ifdef MB_ENABLE_CLASS
	int result = MB_FUNC_OK;
	_class_t* instance = 0;
	_object_t* obj = 0;
	_running_context_t* running = 0;
	_ls_node_t* tmp = 0;
	_var_t* var = 0;
	int i = 0;
	_object_t mo;
	_class_t* mi = 0;

	if(!s || !l || !n || !out) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	running = s->running_context;

	tmp = (_ls_node_t*)*l;

	_using_jump_set_of_structured(s, tmp, _exit, result);

	tmp = _search_identifier_in_scope_chain(s, 0, n, _PATHING_NONE, 0, 0);
	if(tmp && tmp->data) {
		obj = (_object_t*)tmp->data;
		if(obj->type == _DT_VAR)
			var = obj->data.variable;
	}
	if(s->last_instance || (obj && !var)) {
		_handle_error_on_obj(s, SE_RN_DUPLICATE_CLASS, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

	obj = _create_object();
	obj->type = _DT_CLASS;

	instance = (_class_t*)mb_malloc(sizeof(_class_t));
	_init_class(s, instance, mb_strdup(n, strlen(n) + 1));

	for(i = 0; i < c; i++) {
		if(meta[i]->type != MB_DT_CLASS) {
			_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}
		_MAKE_NIL(&mo);
		_public_value_to_internal_object(meta[i], &mo);
		mi = mo.data.instance;
		_link_meta_class(s, instance, mi);
	}

	_push_scope_by_class(s, instance->scope);
	obj->data.instance = instance;

	if(var) {
		_destroy_object(var->data, 0);
		var->data = obj;
	} else {
		_ht_set_or_insert(running->var_dict, (void*)n, obj);
	}

	s->last_instance = instance;

	if(out) {
		out->type = MB_DT_CLASS;
		out->value.instance = instance;
	}

_exit:
	return result;
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(n);
	mb_unrefvar(meta);
	mb_unrefvar(c);
	mb_unrefvar(out);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_CLASS */
}

/* End a class */
int mb_end_class(struct mb_interpreter_t* s, void** l) {
#ifdef MB_ENABLE_CLASS
	int result = MB_FUNC_OK;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	_pop_scope(s, false);

	s->last_instance = 0;

_exit:
	return result;
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(s);
	mb_unrefvar(l);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_CLASS */
}

/* Get the userdata of a class instance */
int mb_get_class_userdata(struct mb_interpreter_t* s, void** l, void** d) {
#ifdef MB_ENABLE_CLASS
	int result = MB_FUNC_OK;

	if(!s || !d) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(s && s->last_instance) {
		if(d)
			*d = s->last_instance->userdata;
	} else if(s && s->last_routine && s->last_routine->instance) {
		if(d)
			*d = s->last_routine->instance->userdata;
	} else {
		if(d) *d = 0;

		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

_exit:
	return result;
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(d);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_CLASS */
}

/* Set the userdata of a class instance */
int mb_set_class_userdata(struct mb_interpreter_t* s, void** l, void* d) {
#ifdef MB_ENABLE_CLASS
	int result = MB_FUNC_OK;

	if(!s || !d) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(s && s->last_instance) {
		s->last_instance->userdata = d;
	} else {
		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

_exit:
	return result;
#else /* MB_ENABLE_CLASS */
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(d);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_CLASS */
}

/* Get a value by its identifier name */
int mb_get_value_by_name(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t* val) {
	int result = MB_FUNC_OK;
	_ls_node_t* tmp = 0;
	_object_t* obj = 0;
	mb_unrefvar(l);

	if(!s || !n) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_nil(*val);

	tmp = _search_identifier_in_scope_chain(s, 0, n, _PATHING_NORMAL, 0, 0);
	if(tmp && tmp->data) {
		obj = (_object_t*)tmp->data;
		_internal_object_to_public_value(obj, val);
	}

_exit:
	return result;
}

/* Add a variable with a specific name */
int mb_add_var(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t val, bool_t force) {
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_object_t* obj = 0;
	_var_t* var = 0;
	_ls_node_t* tmp = 0;

	if(!s || !n) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	running = s->running_context;

	tmp = _ht_find(running->var_dict, (void*)n);

	if(tmp) {
		if(force) {
			result = mb_set_var_value(s, tmp->data, val);

			goto _exit;
		} else {
			_handle_error_on_obj(s, SE_RN_DUPLICATE_ID, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}
	}

	var = _create_var(&obj, n, strlen(n) + 1, true);
	_public_value_to_internal_object(&val, var->data);

	_ht_set_or_insert(running->var_dict, var->name, obj);

_exit:
	return result;
}

/* Get a token literally, store it in an argument if it's a variable */
int mb_get_var(struct mb_interpreter_t* s, void** l, void** v, bool_t redir) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(v) *v = 0;

	ast = (_ls_node_t*)*l;
	if(ast) {
		obj = (_object_t*)ast->data;
		if(_IS_SEP(obj, ',')) {
			ast = ast->next;
			obj = ast ? (_object_t*)ast->data : 0;
		}
		if(ast) ast = ast->next;
	}

	if(obj && obj->type == _DT_VAR) {
#ifdef MB_ENABLE_CLASS
		if(redir && obj->data.variable->pathing)
			_search_var_in_scope_chain(s, obj->data.variable, &obj);
#else /* MB_ENABLE_CLASS */
		mb_unrefvar(redir);
#endif /* MB_ENABLE_CLASS */
		if(v)
			*v = obj;
	}

	if(ast && _IS_SEP(ast->data, ','))
		ast = ast->next;

	*l = ast;

_exit:
	return result;
}

/* Get the name of a variable */
int mb_get_var_name(struct mb_interpreter_t* s, void* v, char** n) {
	int result = MB_FUNC_OK;
	_object_t* obj = 0;

	if(n) *n = 0;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(!n || !v)
		goto _exit;

	obj = (_object_t*)v;
	if(obj->type != _DT_VAR)
		goto _exit;

	if(n) *n = obj->data.variable->name;

_exit:
	return result;
}

/* Get the value of a variable */
int mb_get_var_value(struct mb_interpreter_t* s, void* v, mb_value_t* val) {
	int result = MB_FUNC_OK;
	_object_t* obj = 0;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(!val || !v)
		goto _exit;

	obj = (_object_t*)v;
	if(obj->type != _DT_VAR)
		goto _exit;

	_internal_object_to_public_value(obj->data.variable->data, val);

_exit:
	return result;
}

/* Set the value of a variable */
int mb_set_var_value(struct mb_interpreter_t* s, void* v, mb_value_t val) {
	int result = MB_FUNC_OK;
	_object_t* obj = 0;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(!v)
		goto _exit;
	obj = (_object_t*)v;
	if(obj->type != _DT_VAR)
		goto _exit;

	_public_value_to_internal_object(&val, obj->data.variable->data);

_exit:
	return result;
}

/* Create an array */
int mb_init_array(struct mb_interpreter_t* s, void** l, mb_data_e t, int* d, int c, void** a) {
	int result = MB_FUNC_OK;
	_array_t* arr = 0;
	_data_e type = _DT_NIL;
	int j = 0;
	int n = 0;

	if(!s || !l || !d || !a) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	*a = 0;
	if(c > MB_MAX_DIMENSION_COUNT) {
		_handle_error_on_obj(s, SE_RN_TOO_MANY_DIMENSIONS, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}
	for(j = 0; j < c; j++) {
		n = d[j];
		if(n <= 0) {
			_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}
	}

#ifdef MB_SIMPLE_ARRAY
	if(t == MB_DT_REAL) {
		type = _DT_REAL;
	} else if(t == MB_DT_STRING) {
		type = _DT_STRING;
	} else {
		_handle_error_on_obj(s, SE_RN_UNEXPECTED_TYPE, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}
#else /* MB_SIMPLE_ARRAY */
	mb_unrefvar(t);
	type = _DT_REAL;
#endif /* MB_SIMPLE_ARRAY */

	arr = _create_array(s, 0, type);
	for(j = 0; j < c; j++) {
		n = d[j];
		arr->dimensions[arr->dimension_count++] = n;
		if(arr->count)
			arr->count *= (unsigned)n;
		else
			arr->count += (unsigned)n;
	}
	_init_array(arr);
	if(!arr->raw) {
		arr->dimension_count = 0;
		arr->dimensions[0] = 0;
		arr->count = 0;
	}
	*a = arr;

_exit:
	return result;
}

/* Get the length of an array */
int mb_get_array_len(struct mb_interpreter_t* s, void** l, void* a, int r, int* i) {
	int result = MB_FUNC_OK;
	_array_t* arr = 0;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	arr = (_array_t*)a;
	if(r < 0 || r >= arr->dimension_count) {
		_handle_error_on_obj(s, SE_RN_RANK_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}
	if(i)
		*i = arr->dimensions[r];

_exit:
	return result;
}

/* Get an element of an array with a specific index */
int mb_get_array_elem(struct mb_interpreter_t* s, void** l, void* a, int* d, int c, mb_value_t* val) {
	int result = MB_FUNC_OK;
	_array_t* arr = 0;
	int index = 0;
	_data_e type = _DT_NIL;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	arr = (_array_t*)a;
	if(c < 0 || c > arr->dimension_count) {
		_handle_error_on_obj(s, SE_RN_TOO_MANY_DIMENSIONS, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}
	if(!val)
		goto _exit;

	index = _get_array_pos(s, arr, d, c);
	if(index < 0) {
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

	_get_array_elem(s, arr, index, &val->value, &type);
	val->type = _internal_type_to_public_type(type);

_exit:
	return result;
}

/* Set an element of an array with a specific index */
int mb_set_array_elem(struct mb_interpreter_t* s, void** l, void* a, int* d, int c, mb_value_t val) {
	int result = MB_FUNC_OK;
	_array_t* arr = 0;
	int index = 0;
	_data_e type = _DT_NIL;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	arr = (_array_t*)a;
	if(c < 0 || c > arr->dimension_count) {
		_handle_error_on_obj(s, SE_RN_TOO_MANY_DIMENSIONS, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

	index = _get_array_pos(s, arr, d, c);
	if(index < 0) {
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

	type = _public_type_to_internal_type(val.type);
	_set_array_elem(s, 0, arr, (unsigned)index, &val.value, &type);

_exit:
	return result;
}

/* Initialize a collection */
int mb_init_coll(struct mb_interpreter_t* s, void** l, mb_value_t* coll) {
	int result = MB_FUNC_OK;

	if(!s || !coll) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll->type) {
	case MB_DT_LIST:
		coll->value.list = _create_list(s);

		break;
	case MB_DT_DICT:
		coll->value.dict = _create_dict(s);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(coll);

	_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	return result;
}

/* Get an element of a collection */
int mb_get_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t idx, mb_value_t* val) {
	int result = MB_FUNC_OK;
	_object_t ocoll;
	int_t i = 0;
	mb_value_t ret;

	mb_make_nil(ret);

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	_MAKE_NIL(&ocoll);
#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll.type) {
	case MB_DT_LIST:
		mb_int_val(idx, i);
		_public_value_to_internal_object(&coll, &ocoll);
		if(!_at_list(ocoll.data.list, i, &ret)) {
			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		if(!_find_dict(ocoll.data.dict, &idx, &ret)) {
			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(idx);
	mb_unrefvar(coll);
	mb_unrefvar(i);

	_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	if(val) *val = ret;

	return result;
}

/* Set an element of a collection */
int mb_set_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t idx, mb_value_t val) {
	int result = MB_FUNC_OK;
	_object_t ocoll;
	int_t i = 0;
	_object_t* oval = 0;
	mb_value_t ret;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_nil(ret);

	_MAKE_NIL(&ocoll);
#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll.type) {
	case MB_DT_LIST:
		mb_int_val(idx, i);
		_public_value_to_internal_object(&coll, &ocoll);
		while((int)ocoll.data.list->count <= i)
			_push_list(ocoll.data.list, &ret, 0);
		if(!_set_list(ocoll.data.list, i, &val, &oval)) {
			if(oval)
				_destroy_object(oval, 0);

			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		_set_dict(ocoll.data.dict, &idx, &val, 0, 0);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(val);
	mb_unrefvar(idx);
	mb_unrefvar(coll);
	mb_unrefvar(oval);
	mb_unrefvar(i);

	_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	return result;
}

/* Remove an element from a collection */
int mb_remove_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t idx) {
	int result = MB_FUNC_OK;
	_object_t ocoll;
	int_t i = 0;
	mb_value_t ret;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_nil(ret);

	_MAKE_NIL(&ocoll);
#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll.type) {
	case MB_DT_LIST:
		mb_int_val(idx, i);
		_public_value_to_internal_object(&coll, &ocoll);
		if(!_remove_at_list(ocoll.data.list, i)) {
			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		if(!_remove_dict(ocoll.data.dict, &idx)) {
			_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(coll);
	mb_unrefvar(idx);
	mb_unrefvar(i);

	_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	return result;
}

/* Tell the element count of a collection */
int mb_count_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, int* c) {
	int result = MB_FUNC_OK;
	_object_t ocoll;
#ifdef MB_ENABLE_COLLECTION_LIB
	_list_t* lst = 0;
	_dict_t* dct = 0;
#endif /* MB_ENABLE_COLLECTION_LIB */
	int ret = 0;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	_MAKE_NIL(&ocoll);
#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll.type) {
	case MB_DT_LIST:
		lst = (_list_t*)coll.value.list;
		ret = (int)lst->count;

		break;
	case MB_DT_DICT:
		dct = (_dict_t*)coll.value.dict;
		ret = (int)_ht_count(dct->dict);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(coll);

	_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	if(c) *c = ret;

	return result;
}

/* Get all keys of a collection */
int mb_keys_of_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t* keys, int c) {
	int result = MB_FUNC_OK;
	_object_t ocoll;
#ifdef MB_ENABLE_COLLECTION_LIB
	_list_t* lst = 0;
	_dict_t* dct = 0;
	int i = 0;
	_keys_helper_t helper;
#endif /* MB_ENABLE_COLLECTION_LIB */

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	_MAKE_NIL(&ocoll);
#ifdef MB_ENABLE_COLLECTION_LIB
	switch(coll.type) {
	case MB_DT_LIST:
		lst = (_list_t*)coll.value.list;
		for(i = 0; i < c && i < (int)lst->count; ++i) {
			mb_make_int(keys[i], i);
		}

		break;
	case MB_DT_DICT:
		dct = (_dict_t*)coll.value.dict;
		helper.keys = keys;
		helper.size = c;
		helper.index = 0;
		_HT_FOREACH(dct->dict, _do_nothing_on_object, _copy_keys_to_value_array, &helper);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(coll);
	mb_unrefvar(keys);
	mb_unrefvar(c);

	_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	return result;
}

/* Create a referenced usertype value */
int mb_make_ref_value(struct mb_interpreter_t* s, void* val, mb_value_t* out, mb_dtor_func_t un, mb_clone_func_t cl, mb_hash_func_t hs, mb_cmp_func_t cp, mb_fmt_func_t ft) {
#ifdef MB_ENABLE_USERTYPE_REF
	int result = MB_FUNC_OK;
	_usertype_ref_t* ref = 0;

	if(!s || !out) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(out) {
		ref = _create_usertype_ref(s, val, un, cl, hs, cp, ft);
		out->type = MB_DT_USERTYPE_REF;
		out->value.usertype_ref = ref;
	}

_exit:
	return result;
#else /* MB_ENABLE_USERTYPE_REF */
	mb_unrefvar(s);
	mb_unrefvar(val);
	mb_unrefvar(out);
	mb_unrefvar(un);
	mb_unrefvar(cl);
	mb_unrefvar(hs);
	mb_unrefvar(cp);
	mb_unrefvar(ft);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_USERTYPE_REF */
}

/* Get the data of a referenced usertype value */
int mb_get_ref_value(struct mb_interpreter_t* s, void** l, mb_value_t val, void** out) {
#ifdef MB_ENABLE_USERTYPE_REF
	int result = MB_FUNC_OK;
	_usertype_ref_t* ref = 0;

	if(!s || !out) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(val.type != MB_DT_USERTYPE_REF) {
		_handle_error_on_obj(s, SE_RN_UNEXPECTED_TYPE, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

	if(out) {
		ref = (_usertype_ref_t*)val.value.usertype_ref;
		*out = ref->usertype;
	}

_exit:
	return result;
#else /* MB_ENABLE_USERTYPE_REF */
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(val);
	mb_unrefvar(out);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_USERTYPE_REF */
}

/* Increase the reference of a value by 1 */
int mb_ref_value(struct mb_interpreter_t* s, void** l, mb_value_t val) {
	int result = MB_FUNC_OK;
	_object_t obj;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

#ifdef MB_ENABLE_COLLECTION_LIB
	if(val.type == MB_DT_LIST_IT || val.type == MB_DT_DICT_IT)
		goto _exit;
#endif /* MB_ENABLE_COLLECTION_LIB */

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&val, &obj);
	if(
#ifdef MB_ENABLE_USERTYPE_REF
		obj.type != _DT_USERTYPE_REF &&
#endif /* MB_ENABLE_USERTYPE_REF */
		obj.type != _DT_ARRAY &&
#ifdef MB_ENABLE_COLLECTION_LIB
		obj.type != _DT_LIST && obj.type != _DT_DICT &&
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		obj.type != _DT_CLASS &&
#endif /* MB_ENABLE_CLASS */
		obj.type != _DT_ROUTINE
	) {
		_handle_error_on_obj(s, SE_RN_REFERENCED_TYPE_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}
	_REF(&obj)

_exit:
	return result;
}

/* Decrease the reference of a value by 1 */
int mb_unref_value(struct mb_interpreter_t* s, void** l, mb_value_t val) {
	int result = MB_FUNC_OK;
	_object_t obj;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

#ifdef MB_ENABLE_COLLECTION_LIB
	if(_try_purge_it(s, &val, 0))
		goto _exit;
#endif /* MB_ENABLE_COLLECTION_LIB */

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&val, &obj);
	if(
#ifdef MB_ENABLE_USERTYPE_REF
		obj.type != _DT_USERTYPE_REF &&
#endif /* MB_ENABLE_USERTYPE_REF */
		obj.type != _DT_ARRAY &&
#ifdef MB_ENABLE_COLLECTION_LIB
		obj.type != _DT_LIST && obj.type != _DT_DICT &&
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		obj.type != _DT_CLASS &&
#endif /* MB_ENABLE_CLASS */
		obj.type != _DT_ROUTINE
	) {
		_handle_error_on_obj(s, SE_RN_REFERENCED_TYPE_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}
	_UNREF(&obj)

_exit:
	return result;
}

/* Set the global alive checker */
int mb_set_alive_checker(struct mb_interpreter_t* s, mb_alive_checker_t f) {
	int result = MB_FUNC_OK;

	if(!s)
		result = MB_FUNC_ERR;
	else
		s->alive_check_handler = f;

	return result;
}

/* Set the alive checker of a value */
int mb_set_alive_checker_of_value(struct mb_interpreter_t* s, void** l, mb_value_t val, mb_alive_value_checker_t f) {
#ifdef MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF
	int result = MB_FUNC_OK;
	_object_t obj;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(val.type != MB_DT_USERTYPE_REF) {
		_handle_error_on_obj(s, SE_RN_REFERENCED_TYPE_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&val, &obj);
	obj.data.usertype_ref->alive_checker = f;

_exit:
	return result;
#else /* MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF */
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(val);
	mb_unrefvar(f);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF */
}

/* Override a meta function of a value */
int mb_override_value(struct mb_interpreter_t* s, void** l, mb_value_t val, mb_meta_func_e m, void* f) {
	int result = MB_FUNC_OK;
	_object_t obj;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

#ifdef MB_ENABLE_USERTYPE_REF
	_MAKE_NIL(&obj);
	if(val.type == MB_DT_USERTYPE_REF) {
		_usertype_ref_t* user = 0;
		_public_value_to_internal_object(&val, &obj);
		user = obj.data.usertype_ref;
		if(m & MB_MF_CALC) {
			if(!user->calc_operators) {
				user->calc_operators = (_calculation_operator_info_t*)mb_malloc(sizeof(_calculation_operator_info_t));
				memset(user->calc_operators, 0, sizeof(_calculation_operator_info_t));
			}
		}
		switch(m) {
		case MB_MF_IS:
			user->calc_operators->is = (mb_meta_operator_t)(intptr_t)f;

			break;
		case MB_MF_ADD:
			user->calc_operators->add = (mb_meta_operator_t)(intptr_t)f;

			break;
		case MB_MF_SUB:
			user->calc_operators->sub = (mb_meta_operator_t)(intptr_t)f;

			break;
		case MB_MF_MUL:
			user->calc_operators->mul = (mb_meta_operator_t)(intptr_t)f;

			break;
		case MB_MF_DIV:
			user->calc_operators->div = (mb_meta_operator_t)(intptr_t)f;

			break;
		case MB_MF_NEG:
			user->calc_operators->neg = (mb_meta_operator_t)(intptr_t)f;

			break;
		case MB_MF_COLL:
			user->coll_func = (mb_meta_func_t)(intptr_t)f;

			break;
		case MB_MF_FUNC:
			user->generic_func = (mb_meta_func_t)(intptr_t)f;

			break;
		default: /* Do nothing */
			break;
		}
	} else {
		result = MB_FUNC_ERR;
	}
#else /* MB_ENABLE_USERTYPE_REF */
	mb_unrefvar(obj);
	mb_unrefvar(val);
	mb_unrefvar(m);
	mb_unrefvar(f);
#endif /* MB_ENABLE_USERTYPE_REF */

_exit:
	return result;
}

/* Dispose a value */
int mb_dispose_value(struct mb_interpreter_t* s, mb_value_t val) {
	int result = MB_FUNC_OK;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(val.type == MB_DT_STRING)
		safe_free(val.value.string);

	_assign_public_value(s, &val, 0, false);

_exit:
	return result;
}

/* Get a sub routine with a specific name */
int mb_get_routine(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t* val) {
	int result = MB_FUNC_OK;
	_object_t* obj = 0;
	_ls_node_t* scp = 0;

	if(!s || !n || !val) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	mb_make_nil(*val);

	scp = _search_identifier_in_scope_chain(s, 0, n, _PATHING_NONE, 0, 0);
	if(scp) {
		obj = (_object_t*)scp->data;
		if(obj) {
			if(obj->type == _DT_ROUTINE) {
				_internal_object_to_public_value(obj, val);
			} else {
				_handle_error_on_obj(s, SE_RN_ROUTINE_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}
		}
	}

_exit:
	return result;
}

/* Set a sub routine with a specific name and native function pointer */
int mb_set_routine(struct mb_interpreter_t* s, void** l, const char* n, mb_routine_func_t f, bool_t force) {
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_object_t* obj = 0;
	_routine_t* routine = 0;
	_ls_node_t* tmp = 0;
	_var_t* var = 0;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	running = s->running_context;

	tmp = _ht_find(running->var_dict, (void*)n);

	if(tmp) {
		if(force) {
			obj = (_object_t*)tmp->data;
			if(obj->type == _DT_VAR)
				var = obj->data.variable;
		} else {
			_handle_error_on_obj(s, SE_RN_DUPLICATE_ROUTINE, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}
	}

	routine = (_routine_t*)mb_malloc(sizeof(_routine_t));
	_init_routine(s, routine, mb_strdup(n, strlen(n) + 1), f);

	obj = _create_object();
	obj->type = _DT_ROUTINE;
	obj->data.routine = routine;
	obj->is_ref = false;

#ifdef MB_ENABLE_CLASS
	routine->instance = s->last_instance;
#endif /* MB_ENABLE_CLASS */

	if(var && force) {
		_destroy_object(var->data, 0);
		var->data = obj;
	} else {
		_ht_set_or_insert(running->var_dict, routine->name, obj);
	}

_exit:
	return result;
}

/* Evaluate a sub routine */
int mb_eval_routine(struct mb_interpreter_t* s, void** l, mb_value_t val, mb_value_t* args, unsigned argc, mb_value_t* ret) {
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_object_t obj;
	_ls_node_t* ast = 0;

	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	running = s->running_context;

	if(val.type != MB_DT_ROUTINE) {
		_handle_error_on_obj(s, SE_RN_ROUTINE_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&val, &obj);
	ast = (_ls_node_t*)(*l);
	result = _eval_routine(s, &ast, args, argc, obj.data.routine, _has_routine_fun_arg, _pop_routine_fun_arg);

	if(ret) {
		_assign_public_value(s, ret, &running->intermediate_value, false);
		_MAKE_NIL(&obj);
		_public_value_to_internal_object(ret, &obj);
		_ADDGC(&obj, &s->gc, false)
	}

_exit:
	return result;
}

/* Get the type of a routine */
int mb_get_routine_type(struct mb_interpreter_t* s, mb_value_t val, mb_routine_type_e* y) {
	int result = MB_FUNC_OK;
	_object_t obj;
	mb_unrefvar(s);

	if(!y) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(val.type != MB_DT_ROUTINE) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	_MAKE_NIL(&obj);
	_public_value_to_internal_object(&val, &obj);
	*y = obj.data.routine->type;

_exit:
	return result;
}



word_def = "def";

/* Load and parse a script string */
int mb_load_string(struct mb_interpreter_t* s, const char* l, bool_t reset) {
	int result = MB_FUNC_OK;
	int status = 0;
	unsigned short _row = 0;
	unsigned short _col = 0;
	char wrapped = _ZERO_CHAR;
	_parsing_context_t* context = 0;



	if(!s || !l) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	s->run_count = 0;

	if(!s->parsing_context)
		s->parsing_context = _reset_parsing_context(s->parsing_context);

	context = s->parsing_context;

	mb_uu_getbom(&l);
	while(*l) {
		int n = 1;
#ifdef MB_ENABLE_UNICODE_ID
		if(context->parsing_state == _PS_NORMAL)
			n = mb_uu_ischar(l);
#endif /* MB_ENABLE_UNICODE_ID */
		do {
			if(n == 1) {
				char ch = *l;
				if((ch == _NEWLINE_CHAR || ch == _RETURN_CHAR) && (!wrapped || wrapped == ch)) {
					unsigned short before = 0;
					wrapped = ch;
					before = context->parsing_row++;
					context->parsing_col = 0;
					if(before > context->parsing_row) {
						context->parsing_col = 1;
						_handle_error_now(s, SE_RN_PROGRAM_TOO_LONG, s->last_error_file, MB_FUNC_ERR);

						goto _exit;
					}

					break;
				}
			}
			wrapped = _ZERO_CHAR;
			++context->parsing_col;
		} while(0);
		status = _parse_char(s, l, n, context->parsing_pos, _row, _col);
		result = status;
		if(status) {
			_set_error_pos(s, context->parsing_pos, _row, _col);
			_handle_error_now(s, s->last_error, s->last_error_file, result);

			goto _exit;
		}
		_row = context->parsing_row;
		_col = context->parsing_col;
		++context->parsing_pos;
		l += n;
	};
	status = _parse_char(s, 0, 1, context->parsing_pos, context->parsing_row, context->parsing_col);

_exit:
	if(reset)
		_end_of_file(context);

	return result;
}

/* Load and parse a script file */
int mb_load_file(struct mb_interpreter_t* s, const char* f) {
	int result = MB_FUNC_OK;
	char* buf = 0;
	_parsing_context_t* context = 0;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	s->parsing_context = context = _reset_parsing_context(s->parsing_context);

	buf = _load_file(s, f, 0, false);
	if(buf) {
		result = mb_load_string(s, buf, true);
		safe_free(buf);

		if(result)
			goto _exit;
	} else {
		_set_current_error(s, SE_PS_OPEN_FILE_FAILED, 0);

		result = MB_FUNC_ERR;
	}

_exit:
	if(context)
		context->parsing_state = _PS_NORMAL;

	return result;
}

/* Run the current AST */
int mb_run(struct mb_interpreter_t* s, bool_t clear_parser) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	++s->run_count;

	if(s->parsing_context) {
		if(s->parsing_context->routine_state) {
			s->parsing_context->routine_state = 0;
			result = MB_FUNC_ERR;
			_handle_error_now(s, SE_RN_INCOMPLETE_STRUCTURE, s->source_file, result);
			_tidy_scope_chain(s);

			goto _exit;
		}
#ifdef MB_ENABLE_CLASS
		if(s->parsing_context->class_state != _CLASS_STATE_NONE) {
			s->parsing_context->class_state = _CLASS_STATE_NONE;
			result = MB_FUNC_ERR;
			_handle_error_now(s, SE_RN_INCOMPLETE_STRUCTURE, s->source_file, result);
			_tidy_scope_chain(s);

			goto _exit;
		}
#endif /* MB_ENABLE_CLASS */
	}

	if(clear_parser)
		_destroy_parsing_context(&s->parsing_context);

	s->handled_error = false;

	if(s->suspent_point) {
		ast = s->suspent_point;
		ast = ast->next;
		s->suspent_point = 0;
	} else {
		s->source_file = 0;
#ifdef MB_ENABLE_CLASS
		s->last_instance = 0;
		s->calling = false;
#endif /* MB_ENABLE_CLASS */
		s->last_routine = 0;

#if _MULTILINE_STATEMENT
		_ls_clear(s->multiline_enabled);
#endif /* _MULTILINE_STATEMENT */

		mb_assert(!s->no_eat_comma_mark);
		while(s->running_context->prev)
			s->running_context = s->running_context->prev;
		ast = s->ast;
		ast = ast->next;
		if(!ast) {
			result = MB_FUNC_ERR;
			_set_error_pos(s, 0, 0, 0);
			_handle_error_now(s, SE_RN_EMPTY_PROGRAM, s->source_file, result);

			goto _exit;
		}
	}

	do {
		result = _execute_statement(s, &ast, true);
		if(result != MB_FUNC_OK && result != MB_SUB_RETURN) {
			if(result != MB_FUNC_SUSPEND) {
				if(result >= MB_EXTENDED_ABORT)
					s->last_error = SE_EA_EXTENDED_ABORT;
				_handle_error_now(s, s->last_error, s->last_error_file, result);
			}

			goto _exit;
		}
	} while(ast);

_exit:
	if(s) {
		if(!s->suspent_point)
			s->source_file = 0;
		if(clear_parser)
			_destroy_parsing_context(&s->parsing_context);

		_destroy_edge_objects(s);

		s->has_run = true;
	}

	return result;
}

/* Suspend current execution and save the context */
int mb_suspend(struct mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;

	if(!s || !l || !(*l)) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	ast = (_ls_node_t*)*l;
	s->suspent_point = ast;

_exit:
	return result;
}

/* Schedule to suspend current execution */
int mb_schedule_suspend(struct mb_interpreter_t* s, int t) {
	int result = MB_FUNC_OK;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(t == MB_FUNC_OK)
		t = MB_FUNC_SUSPEND;
	s->schedule_suspend_tag = t;

_exit:
	return result;
}

/* Get the value of an identifier */
int mb_debug_get(struct mb_interpreter_t* s, const char* n, mb_value_t* val) {
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_ls_node_t* v = 0;
	_object_t* obj = 0;
	mb_value_t tmp;

	if(!s || !n) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	running = s->running_context;

	v = _search_identifier_in_scope_chain(s, 0, n, _PATHING_NONE, 0, 0);
	if(v) {
		obj = (_object_t*)v->data;
		mb_assert(obj->type == _DT_VAR);
		if(val)
			result = _internal_object_to_public_value(obj->data.variable->data, val);
		else
			result = _internal_object_to_public_value(obj->data.variable->data, &tmp);
	} else {
		if(val) {
			mb_make_nil(*val);
		}
		_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, (_object_t*)0, MB_FUNC_ERR, _exit, result);
	}

_exit:
	return result;
}

/* Set the value of an identifier */
int mb_debug_set(struct mb_interpreter_t* s, const char* n, mb_value_t val) {
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_ls_node_t* v = 0;
	_object_t* obj = 0;

	if(!s || !n) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	running = s->running_context;

	v = _search_identifier_in_scope_chain(s, 0, n, _PATHING_NONE, 0, 0);
	if(v) {
		obj = (_object_t*)v->data;
		mb_assert(obj->type == _DT_VAR);
		result = _public_value_to_internal_object(&val, obj->data.variable->data);
	} else {
		_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, (_object_t*)0, MB_FUNC_ERR, _exit, result);
	}

_exit:
	return result;
}

/* Get stack frame names of a MY-BASIC environment */
int mb_debug_get_stack_trace(struct mb_interpreter_t* s, void** l, char** fs, unsigned fc) {
#ifdef MB_ENABLE_STACK_TRACE
	int result = MB_FUNC_OK;
	_ls_node_t* f = 0;
	unsigned i = 0;
	mb_unrefvar(l);

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(fs && fc) {
		if(_ls_count(s->stack_frames) > fc)
			fs[--fc] = "...";
		f = s->stack_frames->prev;
		while(f != s->stack_frames && f && f->data && i < fc) {
			fs[i++] = (char*)f->data;
			f = f->prev;
		}
	}
	while(i < fc)
		fs[i++] = 0;

_exit:
	return result;
#else /* MB_ENABLE_STACK_TRACE */
	int result = MB_FUNC_ERR;
	mb_unrefvar(s);
	mb_unrefvar(l);
	mb_unrefvar(fs);
	mb_unrefvar(fc);

	return result;
#endif /* MB_ENABLE_STACK_TRACE */
}

/* Set a stepped handler to a MY-BASIC environment */
int mb_debug_set_stepped_handler(struct mb_interpreter_t* s, mb_debug_stepped_handler_t h) {
	int result = MB_FUNC_OK;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	s->debug_stepped_handler = h;

_exit:
	return result;
}

/* Get type description text */
const char* mb_get_type_string(mb_data_e t) {
	switch(t) {
	case MB_DT_NIL:
		return "NIL";
	case MB_DT_UNKNOWN:
		return "UNKNOWN";
	case MB_DT_INT:
		return "INTEGER";
	case MB_DT_REAL:
		return "REAL";
	case MB_DT_NUM:
		return "NUMBER";
	case MB_DT_STRING:
		return "STRING";
	case MB_DT_TYPE:
		return "TYPE";
	case MB_DT_USERTYPE:
		return "USERTYPE";
#ifdef MB_ENABLE_USERTYPE_REF
	case MB_DT_USERTYPE_REF:
		return "USERTYPE_REF";
#endif /* MB_ENABLE_USERTYPE_REF */
	case MB_DT_ARRAY:
		return "ARRAY";
#ifdef MB_ENABLE_COLLECTION_LIB
	case MB_DT_LIST:
		return "LIST";
	case MB_DT_LIST_IT:
		return "LIST_ITERATOR";
	case MB_DT_DICT:
		return "DICT";
	case MB_DT_DICT_IT:
		return "DICT_ITERATOR";
	case MB_DT_COLLECTION:
		return "COLLECTION";
	case MB_DT_ITERATOR:
		return "ITERATOR";
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	case MB_DT_CLASS:
		return "CLASS";
#endif /* MB_ENABLE_CLASS */
	case MB_DT_ROUTINE:
		return "ROUTINE";
	default: /* Return a not existing string */
		return "";
	}
}

/* Raise an error */
int mb_raise_error(struct mb_interpreter_t* s, void** l, mb_error_e err, int ret) {
	int result = MB_FUNC_ERR;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	_handle_error_on_obj(s, err, s->source_file, DON2(l), ret, _exit, result);

_exit:
	return result;
}

/* Get the last error information */
mb_error_e mb_get_last_error(struct mb_interpreter_t* s, const char** file, int* pos, unsigned short* row, unsigned short* col) {
	mb_error_e result = SE_NO_ERR;

	if(!s)
		goto _exit;

	result = s->last_error;
	s->last_error = SE_NO_ERR; /* Clear error state */
	if(file) *file = s->last_error_file;
	if(pos) *pos = s->last_error_pos;
	if(row) *row = s->last_error_row;
	if(col) *col = s->last_error_col;
	s->last_error_file = 0;

_exit:
	return result;
}

/* Get the error description text */
const char* mb_get_error_desc(mb_error_e err) {
#ifdef MB_ENABLE_FULL_ERROR
	if(err < countof(_ERR_DESC))
		return _ERR_DESC[err];

	return "Unknown error";
#else /* MB_ENABLE_FULL_ERROR */
	mb_unrefvar(err);

	return "Error occurred";
#endif /* MB_ENABLE_FULL_ERROR */
}

//Gets the parent interpreter if there is one
struct mb_interpreter_t*  mb_get_parent(struct mb_interpreter_t* s)
{
	return s->parent;
	
} 

///SEts a yield func to be called every ~200 instructions
int mb_set_yield(struct mb_interpreter_t * s, void (*f)(struct mb_interpreter_t *))
{
	s->yieldfunc = f;
}

/* Set an error handler to a MY-BASIC environment */
int mb_set_error_handler(struct mb_interpreter_t* s, mb_error_handler_t h) {
	int result = MB_FUNC_OK;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	s->error_handler = h;

_exit:
	return result;
}

/* Set a print functor to a MY-BASIC environment */
int mb_set_printer(struct mb_interpreter_t* s, mb_print_func_t p) {
	int result = MB_FUNC_OK;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	s->printer = p;

_exit:
	return result;
}

/* Set an input functor to a MY-BASIC environment */
int mb_set_inputer(struct mb_interpreter_t* s, mb_input_func_t p) {
	int result = MB_FUNC_OK;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	s->inputer = p;

_exit:
	return result;
}

/* Set an import handler to a MY-BASIC environment */
int mb_set_import_handler(struct mb_interpreter_t* s, mb_import_handler_t h) {
	int result = MB_FUNC_OK;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	s->import_handler = h;

_exit:
	return result;
}

/* Register an allocator and a freer globally */
int mb_set_memory_manager(mb_memory_allocate_func_t a, mb_memory_free_func_t f) {
	_mb_allocate_func = a;
	_mb_free_func = f;

	return MB_FUNC_OK;
}

/* Get whether GC is enabled */
bool_t mb_get_gc_enabled(struct mb_interpreter_t* s) {
	if(!s) return false;

	return !s->gc.disabled;
}


/* Sets whether GC is enabled */
int mb_set_gc_enabled(struct mb_interpreter_t* s, bool_t gc) {
	if(!s) return MB_FUNC_ERR;

	s->gc.disabled = !gc;

	return MB_FUNC_OK;
}

/* Trigger GC */
int mb_gc(struct mb_interpreter_t* s, int_t* collected) {
	int_t diff = 0;

	if(!s)
		return MB_FUNC_ERR;

	diff = (int_t)_mb_allocated;
	_gc_collect_garbage(s, 1);
	diff = (int_t)(_mb_allocated - diff);
	if(collected)
		*collected = diff;

	return MB_FUNC_OK;
}

/* Get the userdata of a MY-BASIC environment */
int mb_get_userdata(struct mb_interpreter_t* s, void** d) {
	int result = MB_FUNC_OK;

	if(!s || !d) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(s && d)
		*d = s->userdata;

_exit:
	return result;
}

/* Get the userdata of a MY-BASIC environment */
int mb_get_suspent(struct mb_interpreter_t* s, void** d) {
	int result = MB_FUNC_OK;

	if(!s || !d) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(s && d)
		*d = s->suspent_point;

_exit:
	return result;
}

/* Set the userdata of a MY-BASIC environment */
int mb_set_userdata(struct mb_interpreter_t* s, void* d) {
	int result = MB_FUNC_OK;

	if(!s) {
		result = MB_FUNC_ERR;

		goto _exit;
	}

	if(s)
		s->userdata = d;

_exit:
	return result;
}

/* Safe stdin reader function */
int mb_gets(const char* pmt, char* buf, int s) {
	int result = 0;
	mb_unrefvar(pmt);

	if(buf && s) {
		if(fgets(buf, s, stdin) == 0) {
			fprintf(stderr, "Error reading.\n");

			exit(1);
		}
		result = (int)strlen(buf);
		if(buf[result - 1] == _NEWLINE_CHAR) {
			buf[result - 1] = _ZERO_CHAR;
			result--;
		}
	}

	return result;
}

/* Duplicate a string for internal use */
char* mb_memdup(const char* val, unsigned size) {
	char* result = 0;

	if(val != 0) {
		result = (char*)mb_malloc(size);
		if(result)
			memcpy(result, val, size);
	}

	return result;
}

/* ========================================================} */

/*
** {========================================================
** Lib definitions
*/

/** Core lib */

/* Operator #, dummy assignment */
static int _core_dummy_assign(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* Operator + */
static int _core_add(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_ONCALC(s, l, add, result, _exit);
	_instruct_obj_meta_obj(s, l, add, result, _exit);
	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_connect_strings(l);
		} else {
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, s->source_file, TON(l), MB_FUNC_ERR, _exit, result);
		}
	} else {
		_instruct_num_op_num(+, l);
	}

_exit:
	return result;
}

/* Operator - (minus) */
static int _core_min(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_ONCALC(s, l, sub, result, _exit);
	_instruct_obj_meta_obj(s, l, sub, result, _exit);
	_instruct_num_op_num(-, l);

	goto _exit; /* Avoid an unreferenced label warning */

_exit:
	return result;
}

/* Operator * */
static int _core_mul(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_ONCALC(s, l, mul, result, _exit);
	_instruct_obj_meta_obj(s, l, mul, result, _exit);
	_instruct_num_op_num(*, l);

	goto _exit; /* Avoid an unreferenced label warning */

_exit:
	return result;
}

/* Operator / */
static int _core_div(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_ONCALC(s, l, div, result, _exit);
	_instruct_obj_meta_obj(s, l, div, result, _exit);
	_proc_div_by_zero(s, l, _exit, result, SE_RN_DIVIDE_BY_ZERO);
	_instruct_num_op_num(/, l);

_exit:
	return result;
}

/* Operator MOD */
static int _core_mod(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_proc_div_by_zero(s, l, _exit, result, SE_RN_DIVIDE_BY_ZERO);
	_instruct_int_op_int(%, l);

_exit:
	return result;
}

/* Operator ^ */
static int _core_pow(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_instruct_fun_num_num(pow, l);

	return result;
}

/* Operator ( */
static int _core_open_bracket(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* Operator ) */
static int _core_close_bracket(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

#ifdef MB_ENABLE_LAMBDA
	if(s->last_routine && s->last_routine->type == MB_RT_LAMBDA) {
		result = _core_return(s, l);

		goto _exit;
	}
#endif /* MB_ENABLE_LAMBDA */

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* Operator - (negative) */
static int _core_neg(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	mb_value_t arg;
	_running_context_t* running = 0;
	int* inep = 0;
	int calc_depth = 0;
#ifdef MB_ENABLE_USERTYPE_REF
	_object_t obj;
#endif /* MB_ENABLE_USERTYPE_REF */

	mb_assert(s && l);

	running = s->running_context;
	ast = (_ls_node_t*)*l;
	if(ast) ast = ast->next;

	if(!_ls_empty(s->in_neg_expr))
		inep = (int*)_ls_back(s->in_neg_expr)->data;

	if(inep)
		(*inep)++;

	calc_depth = running->calc_depth;

	mb_make_nil(arg);
	if(ast && _IS_FUNC((_object_t*)ast->data, _core_open_bracket)) {
		mb_check(mb_attempt_open_bracket(s, l));

		mb_check(mb_pop_value(s, l, &arg));

		mb_check(mb_attempt_close_bracket(s, l));
	} else {
		running->calc_depth = 1;

		mb_check(mb_attempt_func_begin(s, l));

		mb_check(mb_pop_value(s, l, &arg));

		mb_check(mb_attempt_func_end(s, l));
	}

	if(inep)
		(*inep)--;

	_ONNEG(s, l, arg, result, _exit);
	switch(arg.type) {
	case MB_DT_INT:
		arg.value.integer = -(arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.float_point = -(arg.value.float_point);

		break;
#ifdef MB_ENABLE_USERTYPE_REF
	case MB_DT_USERTYPE_REF:
		_MAKE_NIL(&obj);
		_public_value_to_internal_object(&arg, &obj);
		if(obj.data.usertype_ref->calc_operators && obj.data.usertype_ref->calc_operators->neg) {
			mb_meta_operator_t neg = obj.data.usertype_ref->calc_operators->neg;
			mb_check(mb_ref_value(s, l, arg));
			mb_check(mb_unref_value(s, l, arg));
			mb_check(neg(s, l, &arg, 0, &arg));

			break;
		}
		/* Fall through */
#endif /* MB_ENABLE_USERTYPE_REF */
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, s->source_file, TON(l), MB_FUNC_WARNING, _exit, result);

		break;
	}

_exit:
	mb_check(mb_push_value(s, l, arg));

	running->calc_depth = calc_depth;

	return result;
}

/* Operator = (equal) */
static int _core_equal(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(==, l);
		} else if(_is_nil(((_tuple3_t*)*l)->e1) || _is_nil(((_tuple3_t*)*l)->e2)) {
			tpr = (_tuple3_t*)*l;
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = false;
		} else {
			_set_tuple3_result(l, 0);
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, s->source_file, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(==, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(==, l);
	}

_exit:
	return result;
}

/* Operator < */
static int _core_less(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(<, l);
		} else {
			if(_is_string(((_tuple3_t*)*l)->e1)) {
				_set_tuple3_result(l, 0);
			} else {
				_set_tuple3_result(l, 1);
			}
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, s->source_file, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(<, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(<, l);
	}

_exit:
	return result;
}

/* Operator > */
static int _core_greater(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(>, l);
		} else {
			if(_is_string(((_tuple3_t*)*l)->e1)) {
				_set_tuple3_result(l, 1);
			} else {
				_set_tuple3_result(l, 0);
			}
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, s->source_file, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(>, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(>, l);
	}

_exit:
	return result;
}

/* Operator <= */
static int _core_less_equal(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(<=, l);
		} else {
			if(_is_string(((_tuple3_t*)*l)->e1)) {
				_set_tuple3_result(l, 0);
			} else {
				_set_tuple3_result(l, 1);
			}
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, s->source_file, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(<=, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(<=, l);
	}

_exit:
	return result;
}

/* Operator >= */
static int _core_greater_equal(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(>=, l);
		} else {
			if(_is_string(((_tuple3_t*)*l)->e1)) {
				_set_tuple3_result(l, 1);
			} else {
				_set_tuple3_result(l, 0);
			}
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, s->source_file, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(>=, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(>=, l);
	}

_exit:
	return result;
}

/* Operator <> */
static int _core_not_equal(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_tuple3_t* tpr = 0;

	mb_assert(s && l);

	if(_is_string(((_tuple3_t*)*l)->e1) || _is_string(((_tuple3_t*)*l)->e2)) {
		if(_is_string(((_tuple3_t*)*l)->e1) && _is_string(((_tuple3_t*)*l)->e2)) {
			_instruct_compare_strings(!=, l);
		} else if(_is_nil(((_tuple3_t*)*l)->e1) || _is_nil(((_tuple3_t*)*l)->e2)) {
			tpr = (_tuple3_t*)*l;
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = true;
		} else {
			_set_tuple3_result(l, 1);
			_handle_error_on_obj(s, SE_RN_STRING_EXPECTED, s->source_file, TON(l), MB_FUNC_WARNING, _exit, result);
		}
	} else if(_is_number(((_tuple3_t*)*l)->e1) && _is_number(((_tuple3_t*)*l)->e2)) {
		_instruct_num_op_num(!=, l);
		tpr = (_tuple3_t*)*l;
		if(((_object_t*)tpr->e3)->type != _DT_INT) {
			((_object_t*)tpr->e3)->type = _DT_INT;
			((_object_t*)tpr->e3)->data.integer = ((_object_t*)tpr->e3)->data.float_point != 0.0f;
		}
	} else {
		_instruct_obj_op_obj(!=, l);
	}

_exit:
	return result;
}

/* Operator AND */
static int _core_and(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_instruct_bool_op_bool(&&, l);

	return result;
}

/* Operator OR */
static int _core_or(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	_instruct_bool_op_bool(||, l);

	return result;
}

/* Operator NOT */
static int _core_not(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	mb_value_t arg;
	mb_value_t ret;
	_running_context_t* running = 0;
	int calc_depth = 0;

	mb_assert(s && l);

	running = s->running_context;
	ast = (_ls_node_t*)*l;
	if(ast) ast = ast->next;

	calc_depth = running->calc_depth;

	mb_make_nil(arg);
	mb_make_nil(ret);
	if(ast && _IS_FUNC((_object_t*)ast->data, _core_open_bracket)) {
		mb_check(mb_attempt_open_bracket(s, l));

		mb_check(mb_pop_value(s, l, &arg));

		mb_check(mb_attempt_close_bracket(s, l));
	} else {
		running->calc_depth = 1;

		mb_check(mb_attempt_func_begin(s, l));

		mb_check(mb_pop_value(s, l, &arg));

		mb_check(mb_attempt_func_end(s, l));
	}
	_ONCOND(s, 0, &arg);

	switch(arg.type) {
	case MB_DT_NIL:
		mb_make_bool(ret, true);

		break;
	case MB_DT_INT:
		mb_make_bool(ret, !arg.value.integer);

		break;
	case MB_DT_REAL:
		mb_make_bool(ret, arg.value.float_point == (real_t)0);

		break;
	default:
		mb_make_bool(ret, false);

		break;
	}
	_assign_public_value(s, &arg, 0, true);
	mb_check(mb_push_int(s, l, ret.value.integer));

	running->calc_depth = calc_depth;

	return result;
}

/* Operator IS */
static int _core_is(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_object_t* fst = 0;
	_object_t* scd = 0;
	_object_t* val = 0;
	bool_t is_a = 0;

	mb_assert(s && l);

	_instruct_obj_meta_obj(s, l, is, result, _exit);

	fst = (_object_t*)((_tuple3_t*)*l)->e1;
	scd = (_object_t*)((_tuple3_t*)*l)->e2;
	val = (_object_t*)((_tuple3_t*)*l)->e3;

	if(fst && fst->type == _DT_VAR) fst = fst->data.variable->data;
	if(scd && scd->type == _DT_VAR) scd = scd->data.variable->data;
	if(!fst || !scd) {
		_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, TON(l), MB_FUNC_ERR, _exit, result);
	}
	if(scd->type == _DT_TYPE) {
		val->type = _DT_INT;
		val->data.integer = (int_t)(!!(_internal_type_to_public_type(fst->type) & scd->data.type));
	} else {
#ifdef MB_ENABLE_CLASS
		if(!_IS_CLASS(fst) || !_IS_CLASS(scd)) {
			_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, s->source_file, TON(l), MB_FUNC_ERR, _exit, result);
		}
		_traverse_class(fst->data.instance, 0, _is_a_class, _META_LIST_MAX_DEPTH, true, scd->data.instance, &is_a);
		val->type = _DT_INT;
		val->data.integer = (int_t)is_a;
#else /* MB_ENABLE_CLASS */
		mb_unrefvar(is_a);

		_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, TON(l), MB_FUNC_ERR, _exit, result);
#endif /* MB_ENABLE_CLASS */
	}

_exit:
	return result;
}

/* LET statement */
static int _core_let(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_running_context_t* running = 0;
	_var_t* var = 0;
	_var_t* evar = 0;
	int refc = 1;
	_array_t* arr = 0;
	_object_t* arr_obj = 0;
	unsigned arr_idx = 0;
	bool_t literally = false;
	_object_t* val = 0;
#ifdef MB_ENABLE_COLLECTION_LIB
	int_t idx = 0;
	mb_value_t key;
	bool_t is_coll = false;
#endif /* MB_ENABLE_COLLECTION_LIB */

	mb_assert(s && l);

	running = s->running_context;

	ast = (_ls_node_t*)*l;
	obj = (_object_t*)ast->data;
	if(obj->type == _DT_FUNC)
		ast = ast->next;
	if(!ast || !ast->data) {
		_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	obj = (_object_t*)ast->data;
#ifdef MB_ENABLE_CLASS
	if(_IS_VAR(obj)) {
		_ls_node_t* fn = 0;
		if(_is_valid_class_accessor_following_routine(s, obj->data.variable, ast, &fn)) {
			if(fn)
				obj = (_object_t*)fn->data;
		}
	}
#endif /* MB_ENABLE_CLASS */
	if(obj->type == _DT_ARRAY) {
		arr_obj = obj;
		arr = _search_array_in_scope_chain(s, obj->data.array, &arr_obj);
		result = _get_array_index(s, &ast, 0, &arr_idx, &literally);
		if(result != MB_FUNC_OK)
			goto _exit;
	} else if(obj->type == _DT_VAR && obj->data.variable->data->type == _DT_ARRAY) {
		arr_obj = obj->data.variable->data;
		arr = _search_array_in_scope_chain(s, obj->data.variable->data->data.array, &arr_obj);
		result = _get_array_index(s, &ast, 0, &arr_idx, &literally);
		if(result != MB_FUNC_OK)
			goto _exit;
	} else if(obj->type == _DT_VAR) {
		if(_IS_ME(obj->data.variable)) {
			_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		} else {
			evar = obj->data.variable;
			var = _search_var_in_scope_chain(s, obj->data.variable, 0);
			if(var == evar) evar = 0;
#ifdef MB_ENABLE_CLASS
			if(evar && evar->pathing == _PATHING_UPVALUE) evar = 0;
#endif /* MB_ENABLE_CLASS */
			if(var == _OBJ_BOOL_TRUE->data.variable || var == _OBJ_BOOL_FALSE->data.variable) {
				_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
			}
		}
	} else {
		_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	ast = ast->next;
	if(!ast || !ast->data) {
		_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
#ifdef MB_ENABLE_COLLECTION_LIB
	if(var && _IS_COLL(var->data)) {
		obj = (_object_t*)ast->data;
		if(_IS_FUNC(obj, _core_open_bracket)) {
			mb_check(mb_attempt_open_bracket(s, l));

			switch(var->data->type) {
			case _DT_LIST:
				mb_check(mb_pop_int(s, l, &idx));

				break;
			case _DT_DICT:
				mb_make_nil(key);
				mb_check(mb_pop_value(s, l, &key));

				break;
			default: /* Do nothing */
				break;
			}

			mb_check(mb_attempt_close_bracket(s, l));

			ast = (_ls_node_t*)*l;
			is_coll = true;
		}
	}
#endif /* MB_ENABLE_COLLECTION_LIB */
	obj = (_object_t*)ast->data;
	if(!_IS_FUNC(obj, _core_equal)) { /* Is it an assign operator? */
		_handle_error_on_obj(s, SE_RN_ASSIGN_OPERATOR_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	ast = ast->next;
	val = _create_object();
	result = _calc_expression(s, &ast, &val);

	if(var) {
#ifdef MB_ENABLE_COLLECTION_LIB
		if(is_coll) {
			switch(var->data->type) {
			case _DT_LIST:
				if(!_set_list(var->data->data.list, idx, 0, &val)) {
					safe_free(val);
					_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
				}

				break;
			case _DT_DICT:
				if(!_set_dict(var->data->data.dict, &key, 0, 0, val)) {
					safe_free(val);
					_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
				}

				break;
			default: /* Do nothing */
				break;
			}

			goto _exit;
		}
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
_proc_extra_var:
#endif /* MB_ENABLE_CLASS */
		_dispose_object(var->data);
		var->data->type = val->type;
#ifdef MB_ENABLE_COLLECTION_LIB
		if(val->type == _DT_LIST_IT || val->type == _DT_DICT_IT)
			_assign_with_it(var->data, val);
		else
			var->data->data = val->data;
#else /* MB_ENABLE_COLLECTION_LIB */
		var->data->data = val->data;
#endif /* MB_ENABLE_COLLECTION_LIB */
		if(val->type == _DT_ROUTINE) {
#ifdef MB_ENABLE_LAMBDA
			if(val->data.routine->type == MB_RT_LAMBDA)
				var->data->is_ref = val->is_ref;
			else
				var->data->is_ref = true;
#else /* MB_ENABLE_LAMBDA */
			var->data->is_ref = true;
#endif /* MB_ENABLE_LAMBDA */
#ifndef MB_ENABLE_ARRAY_REF
		} else if(val->type == _DT_ARRAY) {
			var->data->is_ref = true;
#endif /* MB_ENABLE_ARRAY_REF */
		} else {
			var->data->is_ref = val->is_ref;
		}
#ifdef MB_ENABLE_CLASS
		if(evar && evar->pathing) {
			if(var->data->type == _DT_STRING) {
				var->data->data.string = mb_strdup(var->data->data.string, strlen(var->data->data.string) + 1);
				var->data->is_ref = false;
			}
			var = evar;
			evar = 0;
			refc++;

			goto _proc_extra_var;
		}
#endif /* MB_ENABLE_CLASS */
	} else if(arr && literally) {
		if(val->type != _DT_UNKNOWN) {
			if(arr->name) {
				_destroy_object(val, 0);
				_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}
#ifdef MB_ENABLE_ARRAY_REF
			_unref(&arr_obj->data.array->ref, arr_obj->data.array);
#endif /* MB_ENABLE_ARRAY_REF */
			arr_obj->type = val->type;
#ifdef MB_ENABLE_COLLECTION_LIB
			if(val->type == _DT_LIST_IT || val->type == _DT_DICT_IT)
				_assign_with_it(arr_obj, val);
			else
				arr_obj->data = val->data;
#else /* MB_ENABLE_COLLECTION_LIB */
			arr_obj->data = val->data;
#endif /* MB_ENABLE_COLLECTION_LIB */
			arr_obj->is_ref = val->is_ref;
		}
	} else if(arr) {
		mb_value_u _val;
		switch(val->type) {
#ifdef MB_SIMPLE_ARRAY
		case _DT_INT:
			if(arr->type == _DT_STRING) goto _default;
			_val.integer = val->data.integer;

			break;
		case _DT_REAL:
			if(arr->type == _DT_STRING) goto _default;
			_val.float_point = val->data.float_point;

			break;
		case _DT_STRING:
			if(arr->type != _DT_STRING) goto _default;
			_val.string = val->data.string;

			break;
_default:
#else /* MB_SIMPLE_ARRAY */
		case _DT_NIL: /* Fall through */
		case _DT_UNKNOWN: /* Fall through */
		case _DT_INT: /* Fall through */
		case _DT_REAL: /* Fall through */
		case _DT_STRING: /* Fall through */
		case _DT_TYPE: /* Fall through */
		case _DT_USERTYPE:
			_copy_bytes(_val.bytes, val->data.bytes);

			break;
#endif /* MB_SIMPLE_ARRAY */
		default:
			_dispose_object(val);
			safe_free(val);
			_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		}
		result = _set_array_elem(s, ast, arr, arr_idx, &_val, &val->type);
		if(result != MB_FUNC_OK)
			goto _exit;
		if(val->type == _DT_STRING && !val->is_ref) {
			safe_free(val->data.string);
		}
	}
	while(refc--) {
		_REF(val)
	}
	safe_free(val);

_exit:
	*l = ast;

	return result;
}

/* DIM statement */
static int _core_dim(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* arr = 0;
	_object_t* len = 0;
	mb_value_u val;
	_array_t dummy;

	mb_assert(s && l);

	/* Array name */
	ast = (_ls_node_t*)*l;
	if(!ast->next || ((_object_t*)ast->next->data)->type != _DT_ARRAY) {
		_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, (ast && ast->next) ? ((_object_t*)ast->next->data) : 0, MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	arr = (_object_t*)ast->data;
	memset(&dummy, 0, sizeof(_array_t));
	dummy.type = arr->data.array->type;
	dummy.name = arr->data.array->name;
	/* ( */
	if(!ast->next || ((_object_t*)ast->next->data)->type != _DT_FUNC || ((_object_t*)ast->next->data)->data.func->pointer != _core_open_bracket) {
		_handle_error_on_obj(s, SE_RN_OPEN_BRACKET_EXPECTED, s->source_file, (ast && ast->next) ? ((_object_t*)ast->next->data) : 0, MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	/* Array subscript */
	if(!ast->next) {
		_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, (ast && ast->next) ? ((_object_t*)ast->next->data) : 0, MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	while(((_object_t*)ast->data)->type != _DT_FUNC || ((_object_t*)ast->data)->data.func->pointer != _core_close_bracket) {
		/* Get an integer value */
		len = (_object_t*)ast->data;
		if(!_try_get_value(len, &val, _DT_INT)) {
			_handle_error_on_obj(s, SE_RN_UNEXPECTED_TYPE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if(val.integer <= 0) {
			_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		if(dummy.dimension_count >= MB_MAX_DIMENSION_COUNT) {
			_handle_error_on_obj(s, SE_RN_TOO_MANY_DIMENSIONS, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		dummy.dimensions[dummy.dimension_count++] = (int)val.integer;
		if(dummy.count)
			dummy.count *= (unsigned)val.integer;
		else
			dummy.count += (unsigned)val.integer;
		ast = ast->next;
		/* Comma? */
		if(_IS_SEP(ast->data, ','))
			ast = ast->next;
	}
	/* Create or modify raw data */
	_clear_array(arr->data.array);
#ifdef MB_ENABLE_ARRAY_REF
	dummy.ref = arr->data.array->ref;
#endif /* MB_ENABLE_ARRAY_REF */
	*(arr->data.array) = dummy;
	_init_array(arr->data.array);
	if(!arr->data.array->raw) {
		arr->data.array->dimension_count = 0;
		arr->data.array->dimensions[0] = 0;
		arr->data.array->count = 0;
		_handle_error_on_obj(s, SE_RN_OUT_OF_MEMORY, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

_exit:
	*l = ast;

	return result;
}

/* IF statement */
static int _core_if(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* val = 0;
	_object_t* obj = 0;
	bool_t multi_line = false;
	bool_t skip = false;
	_running_context_t* running = 0;

	mb_assert(s && l);

	running = s->running_context;

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	val = _create_object();

_elseif:
	_MAKE_NIL(val);
	result = _calc_expression(s, &ast, &val);
	_ONCOND(s, val, 0);
	_REF(val)
	if(result != MB_FUNC_OK)
		goto _exit;

	obj = (_object_t*)ast->data;
	if(val->data.integer) {
		skip = true;

		if(!_IS_FUNC(obj, _core_then)) {
			if(ast->prev && _IS_FUNC(ast->prev->data, _core_then)) {
				ast = ast->prev;
			} else {
				_handle_error_on_obj(s, SE_RN_THEN_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
			}
		}

		if(ast && ast->next && _IS_EOS(ast->next->data)) {
			multi_line = true;
		} else {
			if(!s->jump_set || (s->jump_set & _JMP_INS))
				s->skip_to_eoi = _ls_back(s->sub_stack);
		}
		do {
			ast = ast->next;
			while(ast && _IS_EOS(ast->data))
				ast = ast->next;
			if(ast && _IS_FUNC(ast->data, _core_endif)) {
				ast = ast->prev;

				break;
			}
			if(multi_line && ast && (_IS_FUNC(ast->data, _core_else) || _IS_FUNC(ast->data, _core_elseif)))
				break;
			result = _execute_statement(s, &ast, true);
			if(result != MB_FUNC_OK)
				goto _exit;
			if(ast)
				ast = ast->prev;
		} while(ast && (
				(!multi_line && _IS_SEP(ast->data, ':')) || (
					multi_line && ast->next && (
						!_IS_FUNC(ast->next->data, _core_elseif) &&
						!_IS_FUNC(ast->next->data, _core_else) &&
						!_IS_FUNC(ast->next->data, _core_endif)
					)
				)
			)
		);

		if(!ast)
			goto _exit;

		obj = (_object_t*)ast->data;
		if(obj->type != _DT_EOS) {
			s->skip_to_eoi = 0;
			result = _skip_to(s, &ast, 0, _DT_EOS);
			if(result != MB_FUNC_OK)
				goto _exit;
		}
	} else {
		if(ast && ast->next && _IS_EOS(ast->next->data)) {
			multi_line = true;

			_skip_if_chunk(s, &ast);
		}
		if(multi_line && ast && _IS_FUNC(ast->data, _core_elseif)) {
			ast = ast->next;

			goto _elseif;
		}
		if(multi_line && ast && _IS_FUNC(ast->data, _core_endif))
			goto _exit;

		result = _skip_to(s, &ast, _core_else, _DT_EOS);
		if(result != MB_FUNC_OK)
			goto _exit;

		obj = (_object_t*)ast->data;
		if(obj->type != _DT_EOS) {
			skip = true;

			if(!_IS_FUNC(obj, _core_else)) {
				_handle_error_on_obj(s, SE_RN_ELSE_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
			}

			do {
				_ls_node_t* la = 0;
				la = ast = ast->next;
				while(ast && _IS_EOS(ast->data)) {
					ast = ast->next;
					if(ast) la = ast;
				}
				if(!ast) {
					mb_get_last_error(s, 0, 0, 0, 0);
					_handle_error_on_obj(s, SE_RN_ENDIF_EXPECTED, s->source_file, DON(la), MB_FUNC_ERR, _exit, result);
				}
				if(ast && _IS_FUNC(ast->data, _core_endif)) {
					ast = ast->prev;

					break;
				}
				result = _execute_statement(s, &ast, true);
				if(result != MB_FUNC_OK)
					goto _exit;
				if(ast)
					ast = ast->prev;
			} while(ast && (
					(!multi_line && _IS_SEP(ast->data, ':')) ||
					(multi_line && !_IS_FUNC(ast->next->data, _core_endif))
				)
			);
		}
	}

_exit:
	if(result == MB_SUB_RETURN) {
		if(ast)
			ast = ast->prev;
	} else {
		if(multi_line) {
			int ret = MB_FUNC_OK;
			if(skip)
				ret = _skip_struct(s, &ast, _core_if, _core_then, _core_endif);
			if(result != MB_FUNC_END && result != MB_LOOP_BREAK && result != MB_LOOP_CONTINUE && result != MB_SUB_RETURN) {
				if(ret != MB_FUNC_OK)
					result = ret;
			}
		}
	}

	*l = ast;

	if(val->type != _DT_UNKNOWN)
		_destroy_object(val, 0);

	return result;
}

/* THEN statement */
static int _core_then(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* ELSEIF statement */
static int _core_elseif(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* ELSE statement */
static int _core_else(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* ENDIF statement */
static int _core_endif(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* FOR statement */
static int _core_for(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_var_t* var_loop = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	obj = (_object_t*)ast->data;
	if(obj->type != _DT_VAR) {
		_handle_error_on_obj(s, SE_RN_LOOP_VAR_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	var_loop = obj->data.variable;

#ifdef MB_ENABLE_COLLECTION_LIB
	if(ast && ast->next && _IS_FUNC(ast->next->data, _core_in))
		result = _execute_ranged_for_loop(s, &ast, var_loop);
	else
		result = _execute_normal_for_loop(s, &ast, var_loop);
#else /* MB_ENABLE_COLLECTION_LIB */
	result = _execute_normal_for_loop(s, &ast, var_loop);
#endif /* MB_ENABLE_COLLECTION_LIB */

_exit:
	*l = ast;

	return result;
}

/* IN statement */
static int _core_in(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* TO statement */
static int _core_to(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* STEP statement */
static int _core_step(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* NEXT statement */
static int _core_next(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_running_context_t* running = 0;

	mb_assert(s && l);

	running = s->running_context;
	ast = (_ls_node_t*)*l;

	result = MB_LOOP_CONTINUE;

	ast = ast->next;
	if(ast && ((_object_t*)ast->data)->type == _DT_VAR) {
		obj = (_object_t*)ast->data;
		running->next_loop_var = obj->data.variable;
	}

	*l = ast;

	return result;
}

/* WHILE statement */
static int _core_while(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* loop_begin_node = 0;
	_object_t* obj = 0;
	_object_t loop_cond;
	_object_t* loop_cond_ptr = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	loop_cond_ptr = &loop_cond;
	_MAKE_NIL(loop_cond_ptr);

	loop_begin_node = ast;

_loop_begin:
	ast = loop_begin_node;

	result = _calc_expression(s, &ast, &loop_cond_ptr);
	_ONCOND(s, loop_cond_ptr, 0);
	if(result != MB_FUNC_OK)
		goto _exit;

	if(loop_cond_ptr->data.integer) {
		/* Keep looping */
		obj = (_object_t*)ast->data;
		while(!_IS_FUNC(obj, _core_wend)) {
			result = _execute_statement(s, &ast, true);
			if(result == MB_LOOP_BREAK) { /* EXIT */
				if(_skip_struct(s, &ast, _core_while, 0, _core_wend) != MB_FUNC_OK)
					goto _exit;
				_skip_to(s, &ast, 0, _DT_EOS);
				result = MB_FUNC_OK;

				goto _exit;
			} else if(result == MB_SUB_RETURN && s->last_routine) { /* RETURN */
				if(ast) ast = ast->prev;
				if(ast) ast = ast->prev;

				goto _exit;
			} else if(result != MB_FUNC_OK && result != MB_SUB_RETURN) { /* Normally */
				goto _exit;
			}

			if(!ast) {
				_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
			}
			obj = (_object_t*)ast->data;
		}

		goto _loop_begin;
	} else {
		/* End looping */
		if(_skip_struct(s, &ast, _core_while, 0, _core_wend) != MB_FUNC_OK)
			goto _exit;
		_skip_to(s, &ast, 0, _DT_EOS);

		goto _exit;
	}

_exit:
	*l = ast;

	return result;
}

/* WEND statement */
static int _core_wend(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* DO statement */
static int _core_do(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* loop_begin_node = 0;
	_object_t* obj = 0;
	_object_t loop_cond;
	_object_t* loop_cond_ptr = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	obj = (_object_t*)ast->data;
	if(!_IS_EOS(obj)) {
		_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;

	loop_cond_ptr = &loop_cond;
	_MAKE_NIL(loop_cond_ptr);

	loop_begin_node = ast;

_loop_begin:
	ast = loop_begin_node;

	obj = (_object_t*)ast->data;
	while(obj && !_IS_FUNC(obj, _core_until)) {
		result = _execute_statement(s, &ast, true);
		if(result == MB_LOOP_BREAK) { /* EXIT */
			if(_skip_struct(s, &ast, _core_do, 0, _core_until) != MB_FUNC_OK)
				goto _exit;
			_skip_to(s, &ast, 0, _DT_EOS);
			result = MB_FUNC_OK;

			goto _exit;
		} else if(result == MB_SUB_RETURN && s->last_routine) { /* RETURN */
			if(ast) ast = ast->prev;
			if(ast) ast = ast->prev;

			goto _exit;
		} else if(result != MB_FUNC_OK && result != MB_SUB_RETURN) { /* Normally */
			goto _exit;
		}

		obj = ast ? (_object_t*)ast->data : 0;
	}

	obj = ast ? (_object_t*)ast->data : 0;
	if(!obj || !_IS_FUNC(obj, _core_until)) {
		_handle_error_on_obj(s, SE_RN_UNTIL_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;

	result = _calc_expression(s, &ast, &loop_cond_ptr);
	_ONCOND(s, loop_cond_ptr, 0);
	if(result != MB_FUNC_OK)
		goto _exit;

	if(loop_cond_ptr->data.integer) {
		/* End looping */
		if(ast)
			_skip_to(s, &ast, 0, _DT_EOS);

		goto _exit;
	} else {
		/* Keep looping */
		goto _loop_begin;
	}

_exit:
	*l = ast;

	return result;
}

/* UNTIL statement */
static int _core_until(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* EXIT statement */
static int _core_exit(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	result = MB_LOOP_BREAK;

	return result;
}

/* GOTO statement */
static int _core_goto(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_running_context_t* running = 0;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_label_t* label = 0;
	_ls_node_t* glbsyminscope = 0;

	mb_assert(s && l);

	running = s->running_context;

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	_using_jump_set_of_instructional(s, ast, _exit, result);

	obj = (_object_t*)ast->data;
	if(obj->type != _DT_LABEL) {
		_handle_error_on_obj(s, SE_RN_JUMP_LABEL_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	label = (_label_t*)obj->data.label;
	if(!label->node) {
		glbsyminscope = _ht_find(running->var_dict, label->name);
		if(!(glbsyminscope && ((_object_t*)glbsyminscope->data)->type == _DT_LABEL)) {
			_handle_error_on_obj(s, SE_RN_LABEL_NOT_EXISTS, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		label->node = ((_object_t*)glbsyminscope->data)->data.label->node;
	}

	mb_assert(label->node && label->node->prev);
	ast = label->node->prev;
	if(ast && !ast->data)
		ast = ast->next;

_exit:
	*l = ast;

	return result;
}

/* GOSUB statement */
static int _core_gosub(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;

	mb_assert(s && l);

	running = s->running_context;
	ast = (_ls_node_t*)*l;

	_using_jump_set_of_instructional(s, ast, _exit, result);

	result = _core_goto(s, l);
	if(result == MB_FUNC_OK)
		_ls_pushback(s->sub_stack, ast);

_exit:
	return result;
}

/* RETURN statement */
static int _core_return(mb_interpreter_t* s, void** l) {
	int result = MB_SUB_RETURN;
	_ls_node_t* ast = 0;
	_ls_node_t* sub_stack = 0;
	_running_context_t* running = 0;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	running = s->running_context;
	sub_stack = s->sub_stack;

	if(running->prev) {
		ast = (_ls_node_t*)*l;
		ast = ast->next;
		if(mb_has_arg(s, (void**)&ast)) {
			mb_check(mb_pop_value(s, (void**)&ast, &arg));
			mb_check(mb_push_value(s, (void**)&ast, arg));
		}
	}
	ast = (_ls_node_t*)_ls_popback(sub_stack);
	if(!ast) {
		_handle_error_on_obj(s, SE_RN_NO_RETURN_POINT, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	*l = ast;

_exit:
	return result;
}

/* CALL statement */
static int _core_call(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_routine_t* routine = 0;
	mb_value_t ret;
	_ls_node_t* pathed = 0;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	obj = (_object_t*)ast->data;

_retry:
	switch(obj->type) {
	case _DT_FUNC:
		if(_IS_FUNC(obj, _core_open_bracket)) {
			mb_check(mb_attempt_open_bracket(s, l));

			ast = ast->next;
			obj = (_object_t*)ast->data;
#ifdef MB_ENABLE_CLASS
			if(obj->type == _DT_VAR) {
				pathed = _search_identifier_in_scope_chain(s, 0, obj->data.variable->name, _PN(obj->data.variable->pathing), 0, 0);
				if(pathed && pathed->data)
					obj = (_object_t*)pathed->data;
			}
#endif /* MB_ENABLE_CLASS */
			if(!obj || obj->type != _DT_ROUTINE) {
				_handle_error_on_obj(s, SE_RN_ROUTINE_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
			}
			ret.type = MB_DT_ROUTINE;
			ret.value.routine = obj->data.routine;
			ast = ast->next;
			*l = ast;

			mb_check(mb_attempt_close_bracket(s, l));

			mb_check(mb_push_value(s, l, ret));

			ast = (_ls_node_t*)*l;
		}

		break;
	case _DT_VAR:
#ifdef MB_ENABLE_LAMBDA
		if(obj->data.variable->data->type == _DT_ROUTINE && obj->data.variable->data->data.routine->type == MB_RT_LAMBDA) {
#ifdef MB_ENABLE_CLASS
			int fp = _PN(obj->data.variable->pathing);
#else /* MB_ENABLE_CLASS */
			int fp = _PATHING_NORMAL;
#endif /* MB_ENABLE_CLASS */
			pathed = _search_identifier_in_scope_chain(s, 0, obj->data.variable->name, fp, 0, 0);
			if(pathed && pathed->data)
				obj = (_object_t*)pathed->data;
			if(obj->type != _DT_VAR)
				goto _retry;
		}
#endif /* MB_ENABLE_LAMBDA */
		if(obj->data.variable->data->type == _DT_ROUTINE) {
			obj = obj->data.variable->data;

			goto _retry;
		}
#ifdef MB_ENABLE_CLASS
		pathed = _search_identifier_in_scope_chain(s, 0, obj->data.variable->name, _PN(obj->data.variable->pathing), 0, 0);
		if(pathed && pathed->data)
			obj = (_object_t*)pathed->data;
		/* Fall through */
#else /* MB_ENABLE_CLASS */
		mb_unrefvar(pathed);
#endif /* MB_ENABLE_CLASS */
	case _DT_ROUTINE:
		obj = _GET_ROUTINE(obj);
		routine = obj->data.routine;
#ifdef MB_ENABLE_CLASS
#ifdef MB_ENABLE_LAMBDA
		if(routine->type != MB_RT_LAMBDA) {
#else /* MB_ENABLE_LAMBDA */
		{
#endif /* MB_ENABLE_LAMBDA */
			bool_t is_a0 = false;
			bool_t is_a1 = false;
			if(s->last_instance && routine->instance) {
				_traverse_class(s->last_instance->created_from, 0, _is_a_class, _META_LIST_MAX_DEPTH, true, routine->instance->created_from, &is_a0);
				_traverse_class(routine->instance->created_from, 0, _is_a_class, _META_LIST_MAX_DEPTH, true, s->last_instance->created_from, &is_a1);
			}
			if(routine->instance && (
				!s->last_instance || (
					s->last_instance &&
						!is_a0 && !is_a1 &&
						s->last_instance->created_from != routine->instance &&
						routine->instance->created_from != s->last_instance
					)
				)
			) {
				pathed = _search_identifier_in_class(s, routine->instance, routine->name, 0, 0);
			} else {
				pathed = _search_identifier_in_scope_chain(s, 0, routine->name, _PATHING_NONE, 0, 0);
			}
			if(pathed && pathed->data) {
				obj = (_object_t*)pathed->data;
				obj = _GET_ROUTINE(obj);
				routine = obj->data.routine;
			}
		}
#endif /* MB_ENABLE_CLASS */
#ifdef MB_ENABLE_CLASS
		s->calling = true;
#endif /* MB_ENABLE_CLASS */
		result = _eval_routine(s, &ast, 0, 0, routine, _has_routine_lex_arg, _pop_routine_lex_arg);
#ifdef MB_ENABLE_CLASS
		s->calling = false;
#endif /* MB_ENABLE_CLASS */
		if(ast)
			ast = ast->prev;

		break;
	default: /* Do nothing */
		break;
	}

	*l = ast;

_exit:
	return result;
}

/* DEF statement */
static int _core_def(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;
	_object_t* obj = 0;
	_var_t* var = 0;
	_ls_node_t* rnode = 0;
	_routine_t* routine = 0;

	mb_assert(s && l);

	running = s->running_context;

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	_using_jump_set_of_structured(s, ast, _exit, result);

	if(s->has_run)
		goto _skip;

	obj = (_object_t*)ast->data;
	if(!_IS_ROUTINE(obj)) {
		_handle_error_on_obj(s, SE_RN_ROUTINE_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	if(obj->data.routine->func.basic.entry) {
		_handle_error_on_obj(s, SE_RN_DUPLICATE_ROUTINE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	routine = (_routine_t*)((_object_t*)ast->data)->data.routine;
	ast = ast->next;
	obj = (_object_t*)ast->data;
	if(!_IS_FUNC(obj, _core_open_bracket)) {
		_handle_error_on_obj(s, SE_RN_OPEN_BRACKET_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	ast = ast->next;
	obj = (_object_t*)ast->data;
	while(!_IS_FUNC(obj, _core_close_bracket)) {
		if(obj->type == _DT_VAR) {
			var = obj->data.variable;
			rnode = _search_identifier_in_scope_chain(s, routine->func.basic.scope, var->name, _PATHING_NONE, 0, 0);
			if(rnode)
				var = ((_object_t*)rnode->data)->data.variable;
			if(!routine->func.basic.parameters)
				routine->func.basic.parameters = _ls_create();
			_ls_pushback(routine->func.basic.parameters, var);
		} else if(_IS_FUNC(obj, _core_args)) {
			if(!routine->func.basic.parameters)
				routine->func.basic.parameters = _ls_create();
			_ls_pushback(routine->func.basic.parameters, (void*)&_VAR_ARGS);
			ast = ast->next;
			obj = (_object_t*)ast->data;

			break;
		}

		ast = ast->next;
		obj = (_object_t*)ast->data;
	}
	ast = ast->next;
	routine->func.basic.entry = ast;

_skip:
	_skip_to(s, &ast, _core_enddef, _DT_INVALID);

	ast = ast->next;

_exit:
	*l = ast;

	return result;
}

/* ENDDEF statement */
static int _core_enddef(mb_interpreter_t* s, void** l) {
	int result = MB_SUB_RETURN;
	_ls_node_t* ast = 0;
	_ls_node_t* sub_stack = 0;

	mb_assert(s && l);

	sub_stack = s->sub_stack;

	ast = (_ls_node_t*)_ls_popback(sub_stack);
	if(!ast) {
		_handle_error_on_obj(s, SE_RN_NO_RETURN_POINT, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	*l = ast;

_exit:
	return result;
}

/* ... (variable argument list) statement */
static int _core_args(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_ls_node_t* var_args = 0;
	bool_t pushed = false;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;
	if(ast) ast = ast->next;
	*l = ast;

	var_args = s->var_args;
	if(var_args) {
		_object_t* obj = (_object_t*)_ls_popfront(var_args);
		if(obj) {
			mb_value_t arg;
			mb_make_nil(arg);
			_internal_object_to_public_value(obj, &arg);
			mb_check(mb_push_value(s, l, arg));
			_UNREF(obj)
			pushed = true;
			_destroy_object_capsule_only(obj, 0);
		}
	}

	if(!pushed) {
		mb_value_t arg;
		mb_make_nil(arg);
		arg.type = MB_DT_UNKNOWN;
		mb_check(mb_push_value(s, l, arg));
	}

	return result;
}

#ifdef MB_ENABLE_CLASS
/* CLASS statement */
static int _core_class(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_running_context_t* running = 0;
	_object_t* obj = 0;
	_class_t* instance = 0;
	_class_t* inherit = 0;
	_class_t* last_inst = 0;

	mb_assert(s && l);

	running = s->running_context;

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	_using_jump_set_of_structured(s, ast, _exit, result);

	if(s->has_run) {
		if(ast) {
			_skip_to(s, &ast, _core_endclass, _DT_NIL);

			ast = ast->next;
		}

		*l = ast;

		return result;
	}

	obj = (_object_t*)ast->data;
	obj = _GET_CLASS(obj);
	if(!_IS_CLASS(obj)) {
		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	instance = obj->data.instance;
	ast = ast->next;
	obj = (_object_t*)ast->data;

	last_inst = s->last_instance;
	s->last_instance = instance;

	/* Process meta prototype list */
	if(_IS_FUNC(obj, _core_open_bracket)) {
		do {
			ast = ast->next;
			obj = (_object_t*)ast->data;
			if(obj && obj->type == _DT_VAR) {
				_ls_node_t* tmp =_search_identifier_in_scope_chain(s, _OUTTER_SCOPE(running), obj->data.variable->name, _PATHING_NONE, 0, 0);
				if(tmp && tmp->data)
					obj = (_object_t*)tmp->data;
			}
			obj = _GET_CLASS(obj);
			if(!_IS_CLASS(obj)) {
				_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
			}
			inherit = obj->data.instance;
			_link_meta_class(s, instance, inherit);
			ast = ast->next;
			obj = (_object_t*)ast->data;
		} while(_IS_CLASS(obj) || _IS_SEP(obj, ','));
		if(_IS_FUNC(obj, _core_close_bracket)) {
			ast = ast->next;
		} else {
			_handle_error_on_obj(s, SE_RN_CLOSE_BRACKET_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
	}

	*l = ast;

	/* Execute class body */
	running = _push_scope_by_class(s, instance->scope);
	do {
		result = _execute_statement(s, (_ls_node_t**)l, true);
		if(result != MB_FUNC_OK) {
			if(result >= MB_EXTENDED_ABORT)
				s->last_error = SE_EA_EXTENDED_ABORT;
			_handle_error_now(s, s->last_error, s->last_error_file, result);

			goto _pop_exit;
		}
		ast = (_ls_node_t*)*l;
		if(!ast)
			break;
		obj = (_object_t*)ast->data;
	} while(ast && !_IS_FUNC(obj, _core_endclass));
	_pop_scope(s, false);

	/* Search for meta functions */
	if(_search_class_hash_and_compare_functions(s, instance) != MB_FUNC_OK) {
		_handle_error_on_obj(s, SE_RN_HASH_AND_COMPARE_MUST_BE_PROVIDED_TOGETHER, s->source_file, DON(ast), MB_FUNC_WARNING, _exit, result);
	}

	/* Finished */
	if(ast) {
		_skip_to(s, &ast, _core_endclass, _DT_NIL);

		ast = ast->next;
	}

_pop_exit:
	if(result != MB_FUNC_OK)
		_pop_scope(s, false);

_exit:
	*l = ast;

	s->last_instance = last_inst;

	return result;
}

/* ENDCLASS statement */
static int _core_endclass(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	_do_nothing(s, l, _exit, result);

_exit:
	return result;
}

/* NEW statement */
static int _core_new(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;
	_object_t obj;
	_object_t tgt;
	mb_value_t ret;
	_class_t* instance = 0;

	mb_assert(s && l);

	mb_make_nil(ret);

	mb_check(mb_attempt_func_begin(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_func_end(s, l));

	_MAKE_NIL(&obj);
	_MAKE_NIL(&tgt);
	switch(arg.type) {
	case MB_DT_STRING:
		arg.value.string = mb_strupr(arg.value.string);
		if((instance = _reflect_string_to_class(s, arg.value.string, &arg)) == 0)
			goto _default;
		_ref(&instance->ref, instance);
		/* Fall through */
	case MB_DT_CLASS:
		_public_value_to_internal_object(&arg, &obj);
		_clone_object(s, &obj, &tgt, false, true);
		ret.type = MB_DT_CLASS;
		ret.value.instance = tgt.data.instance;

		break;
	default:
_default:
		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	mb_check(mb_push_value(s, l, ret));
	_assign_public_value(s, &ret, 0, false);

_exit:
	_assign_public_value(s, &arg, 0, true);

	return result;
}

/* VAR statement */
static int _core_var(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_IGNORE;
	_ls_node_t* ast = 0;
	mb_unrefvar(s);

	ast = (_ls_node_t*)*l;
	ast = ast->next;

	if(!s->last_instance) {
		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, s->source_file, DON2(l), MB_FUNC_WARNING, _exit, result);
	}

_exit:
	*l = ast;

	return result;
}

/* REFLECT statement */
static int _core_reflect(mb_interpreter_t* s, void** l) {
#ifdef MB_ENABLE_COLLECTION_LIB
	int result = MB_FUNC_OK;
	mb_value_t arg;
	_object_t obj;
	mb_value_t ret;
	_class_t* instance = 0;
	_dict_t* coll = 0;

	mb_assert(s && l);

	mb_make_nil(ret);

	mb_check(mb_attempt_func_begin(s, l));

	mb_check(mb_pop_value(s, l, &arg));

	mb_check(mb_attempt_func_end(s, l));

	_MAKE_NIL(&obj);
	switch(arg.type) {
	case MB_DT_STRING:
		arg.value.string = mb_strupr(arg.value.string);
		if((instance = _reflect_string_to_class(s, arg.value.string, &arg)) == 0)
			goto _default;
		_ref(&instance->ref, instance);
		/* Fall through */
	case MB_DT_CLASS:
		_public_value_to_internal_object(&arg, &obj);
		coll = _create_dict(s);
		_traverse_class(obj.data.instance, _reflect_class_field, 0, _META_LIST_MAX_DEPTH, false, coll, 0);
		ret.type = MB_DT_DICT;
		ret.value.dict = coll;

		break;
	default:
_default:
		_handle_error_on_obj(s, SE_RN_CLASS_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	mb_check(mb_push_value(s, l, ret));

_exit:
	_assign_public_value(s, &arg, 0, true);

	return result;
#else /* MB_ENABLE_COLLECTION_LIB */
	mb_unrefvar(s);
	mb_unrefvar(l);

	return MB_FUNC_ERR;
#endif /* MB_ENABLE_COLLECTION_LIB */
}
#endif /* MB_ENABLE_CLASS */

#ifdef MB_ENABLE_LAMBDA
/* LAMBDA statement */
static int _core_lambda(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t ret;
	_running_context_t* running = 0;
	_routine_t* routine = 0;
	_ls_node_t* ast = 0;
	int brackets = 0;
	_var_t* var = 0;
	_object_t* obj = 0;
	unsigned ul = 0;
	bool_t popped = false;

	mb_assert(s && l);

	/* Create lambda struct */
	routine = (_routine_t*)mb_malloc(sizeof(_routine_t));
	running = _init_lambda(s, routine);

	/* Parameter list */
	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _error);

	while(mb_has_arg(s, l)) {
#ifdef MB_ENABLE_CLASS
		unsigned char fp = _PATHING_NONE;
#endif /* MB_ENABLE_CLASS */
		void* v = 0;

		if(!routine->func.lambda.parameters)
			routine->func.lambda.parameters = _ls_create();

		ast = (_ls_node_t*)*l;
		if(ast && _IS_FUNC(ast->data, _core_args)) {
			_ls_pushback(routine->func.lambda.parameters, (void*)&_VAR_ARGS);
			ast = ast->next;
			*l = ast;

			break;
		}

		_mb_check_mark_exit(mb_get_var(s, l, &v, true), result, _error);

		if(!v || ((_object_t*)v)->type != _DT_VAR) {
			_handle_error_on_obj(s, SE_RN_INVALID_LAMBDA, s->source_file, DON2(l), MB_FUNC_ERR, _error, result);
		}
		var = ((_object_t*)v)->data.variable;
#ifdef MB_ENABLE_CLASS
		fp = var->pathing;
#endif /* MB_ENABLE_CLASS */

		/* Add lambda parameters */
		obj = 0;
		var = _create_var(&obj, var->name, 0, true);
#ifdef MB_ENABLE_CLASS
		var->pathing = fp;
#endif /* MB_ENABLE_CLASS */
		ul = _ht_set_or_insert(routine->func.lambda.scope->var_dict, var->name, obj);
		mb_assert(ul);
		_ls_pushback(routine->func.lambda.parameters, var);

		ast = (_ls_node_t*)*l;
		if(_IS_FUNC(ast->data, _core_close_bracket))
			break;
		*l = ast;
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _error);

	/* Lambda body */
	ast = (_ls_node_t*)*l;
	if(ast) ast = ast->prev;
	while(ast && _IS_EOS(ast->next->data))
		ast = ast->next;
	*l = ast;

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _error);

	ast = (_ls_node_t*)*l;
	routine->func.lambda.entry = ast;
	while(ast && (brackets || !_IS_FUNC(ast->data, _core_close_bracket))) {
		if(_IS_FUNC(ast->data, _core_open_bracket))
			brackets++;
		else if(_IS_FUNC(ast->data, _core_close_bracket))
			brackets--;

		if(ast && !_is_valid_lambda_body_node(s, &routine->func.lambda, (_object_t*)ast->data)) {
			_handle_error_on_obj(s, SE_RN_INVALID_LAMBDA, s->source_file, DON2(l), MB_FUNC_ERR, _error, result);
		}

		/* Mark upvalues */
		if(ast)
			_try_mark_upvalue(s, routine, (_object_t*)ast->data);

		ast = ast->next;
	}
	*l = ast;
	routine->func.lambda.end = ast;

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _error);

	_pop_scope(s, false);
	popped = true;

	/* Push the return value */
	ret.type = MB_DT_ROUTINE;
	ret.value.routine = routine;

	_mb_check_mark_exit(mb_push_value(s, l, ret), result, _error);

	/* Error processing */
	while(0) {
_error:
		if(!popped)
			_pop_scope(s, false);
		if(routine)
			_destroy_routine(s, routine);
	}

	return result;
}
#endif /* MB_ENABLE_LAMBDA */

#ifdef MB_ENABLE_ALLOC_STAT
/* MEM statement */
static int _core_mem(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	mb_check(mb_attempt_func_begin(s, l));

	mb_check(mb_attempt_func_end(s, l));

	mb_check(mb_push_int(s, l, (int_t)_mb_allocated));

	return result;
}
#endif /* MB_ENABLE_ALLOC_STAT */

/* TYPE statement */
static int _core_type(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;
	int i = 0;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(arg);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &arg));
	if(arg.type == MB_DT_STRING) {
		mb_data_e types[] = {
			MB_DT_NIL,
			MB_DT_UNKNOWN,
			MB_DT_INT,
			MB_DT_REAL,
			MB_DT_NUM,
			MB_DT_STRING,
			MB_DT_TYPE,
			MB_DT_USERTYPE,
#ifdef MB_ENABLE_USERTYPE_REF
			MB_DT_USERTYPE_REF,
#endif /* MB_ENABLE_USERTYPE_REF */
			MB_DT_ARRAY,
#ifdef MB_ENABLE_COLLECTION_LIB
			MB_DT_LIST,
			MB_DT_LIST_IT,
			MB_DT_DICT,
			MB_DT_DICT_IT,
			MB_DT_COLLECTION,
			MB_DT_ITERATOR,
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
			MB_DT_CLASS,
#endif /* MB_ENABLE_CLASS */
			MB_DT_ROUTINE
		};
		for(i = 0; i < countof(types); i++) {
			unsigned e = types[i];
			if(!mb_stricmp(mb_get_type_string((mb_data_e)e), arg.value.string)) {
				arg.value.type = (mb_data_e)e;
				arg.type = MB_DT_TYPE;

				goto _found;
			}
		}
	}
	os = _try_overridden(s, l, &arg, _CORE_ID_TYPE, MB_MF_FUNC);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		arg.value.type = arg.type;
		arg.type = MB_DT_TYPE;
	}

_found:
	mb_check(mb_attempt_close_bracket(s, l));

	if((os & MB_MS_RETURNED) == MB_MS_NONE) {
		mb_check(mb_push_value(s, l, arg));
	}

	return result;
}

/* IMPORT statement */
static int _core_import(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	mb_check(mb_attempt_func_begin(s, l));

	mb_check(mb_attempt_func_end(s, l));

	return result;
}

/* END statement */
static int _core_end(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;

	mb_assert(s && l);

	result = MB_FUNC_END;

	return result;
}

/** Standard lib */

/* Get the absolute value of a number */
static int _std_abs(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	switch(arg.type) {
	case MB_DT_INT:
		arg.value.integer = (int_t)abs(arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.float_point = (real_t)fabs(arg.value.float_point);

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, s->source_file, DON2(l), MB_FUNC_WARNING, _exit, result);

		break;
	}

_exit:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

/* Get the sign of a number */
static int _std_sgn(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	switch(arg.type) {
	case MB_DT_INT:
		arg.value.integer = sgn(arg.value.integer);

		break;
	case MB_DT_REAL:
		arg.value.integer = sgn(arg.value.float_point);
		arg.type = MB_DT_INT;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, s->source_file, DON2(l), MB_FUNC_WARNING, _exit, result);

		break;
	}
	_mb_check_mark_exit(mb_push_int(s, l, arg.value.integer), result, _exit);

_exit:
	_assign_public_value(s, &arg, 0, true);

	return result;
}

/* Get the square root of a number */
static int _std_sqr(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_math_calculate_fun_real(s, l, arg, sqrt, _exit, result);

_exit:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

/* Get the greatest integer not greater than a number */
static int _std_floor(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	switch(arg.type) {
	case MB_DT_INT: /* Do nothing */
		break;
	case MB_DT_REAL:
		arg.value.integer = (int_t)floor(arg.value.float_point);
		arg.type = MB_DT_INT;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, s->source_file, DON2(l), MB_FUNC_WARNING, _exit, result);

		break;
	}
	_mb_check_mark_exit(mb_push_int(s, l, arg.value.integer), result, _exit);

_exit:
	_assign_public_value(s, &arg, 0, true);

	return result;
}

/* Get the least integer not less than a number */
static int _std_ceil(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	switch(arg.type) {
	case MB_DT_INT: /* Do nothing */
		break;
	case MB_DT_REAL:
		arg.value.integer = (int_t)ceil(arg.value.float_point);
		arg.type = MB_DT_INT;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, s->source_file, DON2(l), MB_FUNC_WARNING, _exit, result);

		break;
	}
	_mb_check_mark_exit(mb_push_int(s, l, arg.value.integer), result, _exit);

_exit:
	_assign_public_value(s, &arg, 0, true);

	return result;
}

/* Get the integer format of a number */
static int _std_fix(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	switch(arg.type) {
	case MB_DT_INT: /* Do nothing */
		break;
	case MB_DT_REAL:
		arg.value.integer = (int_t)(arg.value.float_point);
		arg.type = MB_DT_INT;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, s->source_file, DON2(l), MB_FUNC_WARNING, _exit, result);

		break;
	}
	_mb_check_mark_exit(mb_push_int(s, l, arg.value.integer), result, _exit);

_exit:
	_assign_public_value(s, &arg, 0, true);

	return result;
}

/* Get the rounded integer of a number */
static int _std_round(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	switch(arg.type) {
	case MB_DT_INT: /* Do nothing */
		break;
	case MB_DT_REAL:
		arg.value.integer = (int_t)(arg.value.float_point + (real_t)0.5f);
		arg.type = MB_DT_INT;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_NUMBER_EXPECTED, s->source_file, DON2(l), MB_FUNC_WARNING, _exit, result);

		break;
	}
	_mb_check_mark_exit(mb_push_int(s, l, arg.value.integer), result, _exit);

_exit:
	_assign_public_value(s, &arg, 0, true);

	return result;
}

/* Set a random seed */
static int _std_srnd(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	int_t seed = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_int(s, l, &seed));

	mb_check(mb_attempt_close_bracket(s, l));

	srand((unsigned)seed);

	return result;
}

/* Get a random value among 0 ~ 1 or among given bounds */
static int _std_rnd(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	real_t rnd = (real_t)0.0f;

	mb_assert(s && l);

	ast = (_ls_node_t*)*l;

	if(ast && ast->next && _IS_FUNC(ast->next->data, _core_open_bracket)) {
		int_t lw = 0;
		int_t hg = 0;

		mb_check(mb_attempt_open_bracket(s, l));

		if(mb_has_arg(s, l)) {
			mb_check(mb_pop_int(s, l, &hg));
		}
		if(mb_has_arg(s, l)) {
			lw = hg;
			mb_check(mb_pop_int(s, l, &hg));
		}

		mb_check(mb_attempt_close_bracket(s, l));

		if(lw >= hg) {
			_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}

		rnd = (real_t)rand() / RAND_MAX * (hg - lw + (real_t)0.99999f) + lw; /* [LOW, HIGH] */

		mb_check(mb_push_int(s, l, (int_t)rnd));
	} else {
		mb_check(mb_attempt_func_begin(s, l));

		mb_check(mb_attempt_func_end(s, l));

		rnd = (real_t)(((real_t)(rand() % 101)) / 100.0f); /* [0.0, 1.0] */

		mb_check(mb_push_real(s, l, rnd));
	}

_exit:
	return result;
}

/* Get the sin value of a number */
static int _std_sin(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_math_calculate_fun_real(s, l, arg, sin, _exit, result);

_exit:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

/* Get the cos value of a number */
static int _std_cos(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_math_calculate_fun_real(s, l, arg, cos, _exit, result);

_exit:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

/* Get the tan value of a number */
static int _std_tan(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_math_calculate_fun_real(s, l, arg, tan, _exit, result);

_exit:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

/* Get the asin value of a number */
static int _std_asin(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_math_calculate_fun_real(s, l, arg, asin, _exit, result);

_exit:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

/* Get the acos value of a number */
static int _std_acos(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_math_calculate_fun_real(s, l, arg, acos, _exit, result);

_exit:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

/* Get the atan value of a number */
static int _std_atan(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_math_calculate_fun_real(s, l, arg, atan, _exit, result);

_exit:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

/* Get the exp value of a number */
static int _std_exp(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_math_calculate_fun_real(s, l, arg, exp, _exit, result);

_exit:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

/* Get the log value of a number */
static int _std_log(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_math_calculate_fun_real(s, l, arg, log, _exit, result);

_exit:
	mb_check(mb_push_value(s, l, arg));

	return result;
}

/* Get the ASCII code of a character */
static int _std_asc(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	char* arg = 0;
	int_t val = 0;
#ifdef MB_ENABLE_UNICODE
	size_t sz = 0;
#endif /* MB_ENABLE_UNICODE */

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_string(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	if(arg[0] == _ZERO_CHAR) {
		result = MB_FUNC_ERR;

		goto _exit;
	}
#ifdef MB_ENABLE_UNICODE
	sz = (size_t)mb_uu_ischar(arg);
	if(sizeof(int_t) < sz) {
		sz = sizeof(int_t);
		_handle_error_on_obj(s, SE_RN_OVERFLOW, s->source_file, DON2(l), MB_FUNC_WARNING, _exit, result);
	}
	memcpy(&val, arg, sz);
#else /* MB_ENABLE_UNICODE */
	val = (int_t)arg[0];
#endif /* MB_ENABLE_UNICODE */
	mb_check(mb_push_int(s, l, val));

_exit:
	return result;
}

/* Get the character of an ASCII code */
static int _std_chr(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	int_t arg = 0;
	char* chr = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_int(s, l, &arg));

	mb_check(mb_attempt_close_bracket(s, l));

	chr = (char*)mb_malloc(sizeof(arg) + 1);
	memset(chr, 0, sizeof(arg) + 1);
	memcpy(chr, &arg, sizeof(arg));
	mb_check(mb_push_string(s, l, chr));

	return result;
}

/* Get a number of characters from the left of a string */
static int _std_left(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	char* arg = 0;
	int_t count = 0;
	char* sub = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_string(s, l, &arg));
	mb_check(mb_pop_int(s, l, &count));

	mb_check(mb_attempt_close_bracket(s, l));

	if(count <= 0) {
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

#ifdef MB_ENABLE_UNICODE
	switch(mb_uu_substr(arg, 0, (int)count, &sub)) {
	case 0:
		_handle_error_on_obj(s, SE_RN_INVALID_STRING, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	case -1:
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_UNICODE */
	sub = (char*)mb_malloc(count + 1);
	memcpy(sub, arg, count);
	sub[count] = _ZERO_CHAR;
#endif /* MB_ENABLE_UNICODE */
	mb_check(mb_push_string(s, l, sub));

_exit:
	return result;
}

/* Get a number of characters from a specific position of a string */
static int _std_mid(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	char* arg = 0;
	int_t start = 0;
	int_t count = 0;
	char* sub = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_string(s, l, &arg));
	mb_check(mb_pop_int(s, l, &start));
	mb_check(mb_pop_int(s, l, &count));

	mb_check(mb_attempt_close_bracket(s, l));

	if(count <= 0 || start < 0 || start >= (int_t)strlen(arg)) {
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

#ifdef MB_ENABLE_UNICODE
	switch(mb_uu_substr(arg, start, (int)count, &sub)) {
	case 0:
		_handle_error_on_obj(s, SE_RN_INVALID_STRING, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	case -1:
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_UNICODE */
	sub = (char*)mb_malloc(count + 1);
	memcpy(sub, arg + start, count);
	sub[count] = _ZERO_CHAR;
#endif /* MB_ENABLE_UNICODE */
	mb_check(mb_push_string(s, l, sub));

_exit:
	return result;
}

/* Get a number of characters from the right of a string */
static int _std_right(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	char* arg = 0;
	int_t count = 0;
	char* sub = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_string(s, l, &arg));
	mb_check(mb_pop_int(s, l, &count));

	mb_check(mb_attempt_close_bracket(s, l));

	if(count <= 0) {
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}

#ifdef MB_ENABLE_UNICODE
	switch(mb_uu_substr(arg, (int)(mb_uu_strlen(arg) - count), (int)count, &sub)) {
	case 0:
		_handle_error_on_obj(s, SE_RN_INVALID_STRING, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	case -1:
		_handle_error_on_obj(s, SE_RN_INDEX_OUT_OF_BOUND, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}
#else /* MB_ENABLE_UNICODE */
	sub = (char*)mb_malloc(count + 1);
	memcpy(sub, arg + (strlen(arg) - count), count);
	sub[count] = _ZERO_CHAR;
#endif /* MB_ENABLE_UNICODE */
	mb_check(mb_push_string(s, l, sub));

_exit:
	return result;
}

/* Get the string format of a number */
static int _std_str(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;
	_dynamic_buffer_t buf;
	size_t lbuf = 32;

	mb_assert(s && l);

	mb_make_nil(arg);

	_INIT_BUF(buf);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	switch(arg.type) {
	case MB_DT_INT:
		lbuf = 32; /* Enough for even 64bit integer */
		_RESIZE_CHAR_BUF(buf, lbuf);
		if((size_t)sprintf(_CHAR_BUF_PTR(buf), MB_INT_FMT, arg.value.integer) >= lbuf) {
			mb_assert(0 && "Buffer overflow.");
		}

		break;
	case MB_DT_REAL:
		lbuf = 1 /* - */ + (DBL_MAX_10_EXP + 1) /* 308 + 1 digits */ + 1 /* . */ + 6 /* precision */ + 1 /* \0 */;
		_RESIZE_CHAR_BUF(buf, lbuf);
#ifdef MB_MANUAL_REAL_FORMATTING
		_real_to_str(arg.value.float_point, _CHAR_BUF_PTR(buf), lbuf, 5);
#else /* MB_MANUAL_REAL_FORMATTING */
		if((size_t)sprintf(_CHAR_BUF_PTR(buf), MB_REAL_FMT, arg.value.float_point) >= lbuf) {
			mb_assert(0 && "Buffer overflow.");
		}
#endif /* MB_MANUAL_REAL_FORMATTING */

		break;
	case MB_DT_TYPE: {
			const char* sp = mb_get_type_string(arg.value.type);
			char* ret = mb_strdup(sp, strlen(sp) + 1);
			mb_check(mb_push_string(s, l, ret));

			goto _exit;
		}
#ifdef MB_ENABLE_CLASS
	case MB_DT_CLASS: {
			bool_t got_tostr = false;
			_class_t* instance = (_class_t*)arg.value.instance;
			_object_t val_obj;
			_MAKE_NIL(&val_obj);
			if((result = _format_class_to_string(s, l, instance, &val_obj, &got_tostr)) == MB_FUNC_OK) {
				if(got_tostr) {
					mb_check(mb_push_string(s, l, val_obj.data.string));
				} else {
					const char* sp = mb_get_type_string(_internal_type_to_public_type(_DT_CLASS));
					char* ret = mb_strdup(sp, strlen(sp) + 1);
					mb_check(mb_push_string(s, l, ret));
				}

				goto _exit;
			} else {
				goto _exit;
			}
		}
#endif /* MB_ENABLE_CLASS */
	default:
		result = MB_FUNC_ERR;

		goto _exit;
	}
	mb_check(mb_push_string(s, l, _HEAP_CHAR_BUF(buf)));

_exit:
	_assign_public_value(s, &arg, 0, true);

	return result;
}

/* Get the number format of a string, or get the value of a dictionary iterator */
static int _std_val(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	char* conv_suc = 0;
	mb_value_t arg;
#ifdef MB_ENABLE_COLLECTION_LIB
	_object_t ocoi;
#endif /* MB_ENABLE_COLLECTION_LIB */
	mb_value_t ret;
	mb_meta_status_e os = MB_MS_NONE;
#ifdef MB_ENABLE_COLLECTION_LIB
	_ls_node_t* ast = 0;
#endif /* MB_ENABLE_COLLECTION_LIB */

	mb_assert(s && l);

	mb_make_nil(arg);
	mb_make_nil(ret);

	mb_check(mb_attempt_open_bracket(s, l));

#ifdef MB_ENABLE_COLLECTION_LIB
	ast = (_ls_node_t*)*l;
	if(ast && _IS_FUNC(ast->data, _coll_iterator)) {
		_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}
#endif /* MB_ENABLE_COLLECTION_LIB */
	mb_check(mb_pop_value(s, l, &arg));
	os = _try_overridden(s, l, &arg, _STD_ID_VAL, MB_MF_FUNC);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		switch(arg.type) {
		case MB_DT_STRING:
			ret.value.integer = (int_t)mb_strtol(arg.value.string, &conv_suc, 0);
			if(*conv_suc == _ZERO_CHAR) {
				ret.type = MB_DT_INT;
				mb_check(mb_push_value(s, l, ret));

				goto _exit;
			}
			ret.value.float_point = (real_t)mb_strtod(arg.value.string, &conv_suc);
			if(*conv_suc == _ZERO_CHAR) {
				ret.type = MB_DT_REAL;
				mb_check(mb_push_value(s, l, ret));

				goto _exit;
			}
			result = MB_FUNC_ERR;

			break;
#ifdef MB_ENABLE_COLLECTION_LIB
		case MB_DT_LIST_IT:
			_MAKE_NIL(&ocoi);
			_public_value_to_internal_object(&arg, &ocoi);
			_handle_error_on_obj(s, SE_RN_UNEXPECTED_TYPE, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		case MB_DT_DICT_IT:
			_MAKE_NIL(&ocoi);
			_public_value_to_internal_object(&arg, &ocoi);
			if(ocoi.data.dict_it && ocoi.data.dict_it->curr_node && ocoi.data.dict_it->curr_node != _INVALID_DICT_IT && ocoi.data.dict_it->curr_node->data) {
				_internal_object_to_public_value((_object_t*)ocoi.data.dict_it->curr_node->data, &ret);
				mb_check(mb_push_value(s, l, ret));
			} else {
				_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}

			break;
#endif /* MB_ENABLE_COLLECTION_LIB */
		default:
			_assign_public_value(s, &arg, 0, true);
			_handle_error_on_obj(s, SE_RN_UNEXPECTED_TYPE, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		}
	} else {
		if((os & MB_MS_RETURNED) == MB_MS_NONE) {
			mb_check(mb_push_value(s, l, ret));
		}
	}

_exit:
	mb_check(mb_attempt_close_bracket(s, l));

	return result;
}

/* Get the length of a string or an array, or element count of a collection or a variable argument list */
static int _std_len(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	mb_value_t arg;
	_array_t* arr = 0;
#ifdef MB_ENABLE_COLLECTION_LIB
	_list_t* lst = 0;
	_dict_t* dct = 0;
#endif /* MB_ENABLE_COLLECTION_LIB */
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	ast = (_ls_node_t*)*l;
	if(ast) obj = (_object_t*)ast->data;
	if(obj && _IS_FUNC(obj, _core_args)) {
		ast = ast->next;
		*l = ast;
		_mb_check_mark_exit(mb_push_int(s, l, s->var_args ? (int_t)_ls_count(s->var_args) : 0), result, _exit);

		_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

		goto _exit;
	}
	_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);
	os = _try_overridden(s, l, &arg, _STD_ID_LEN, MB_MF_FUNC);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		switch(arg.type) {
		case MB_DT_STRING:
#ifdef MB_ENABLE_UNICODE
			_mb_check_mark_exit(mb_push_int(s, l, (int_t)mb_uu_strlen(arg.value.string)), result, _exit);
#else /* MB_ENABLE_UNICODE */
			_mb_check_mark_exit(mb_push_int(s, l, (int_t)strlen(arg.value.string)), result, _exit);
#endif /* MB_ENABLE_UNICODE */

			break;
		case MB_DT_ARRAY:
			arr = (_array_t*)arg.value.array;
			_mb_check_mark_exit(mb_push_int(s, l, (int_t)arr->count), result, _exit);

			break;
#ifdef MB_ENABLE_COLLECTION_LIB
		case MB_DT_LIST:
			lst = (_list_t*)arg.value.list;
			_mb_check_mark_exit(mb_push_int(s, l, (int_t)lst->count), result, _exit);
			_assign_public_value(s, &arg, 0, true);

			break;
		case MB_DT_DICT:
			dct = (_dict_t*)arg.value.dict;
			_mb_check_mark_exit(mb_push_int(s, l, (int_t)_ht_count(dct->dict)), result, _exit);
			_assign_public_value(s, &arg, 0, true);

			break;
#endif /* MB_ENABLE_COLLECTION_LIB */
		default:
			_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);

			break;
		}
	} else {
		if((os & MB_MS_RETURNED) == MB_MS_NONE) {
			_mb_check_mark_exit(mb_push_int(s, l, 0), result, _exit);
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

_exit:
	_assign_public_value(s, &arg, 0, true);

	return result;
}

/* GET statement */
static int _std_get(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t ov;
	mb_value_t arg;
	_object_t obj;
#ifdef MB_ENABLE_COLLECTION_LIB
	int_t index = 0;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	char* field = 0;
	_ls_node_t* fnode = 0;
	_object_t* fobj = 0;
#endif /* MB_ENABLE_CLASS */
	mb_value_t ret;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(ov);
	mb_make_nil(arg);
	mb_make_nil(ret);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &ov), result, _exit);
	os = _try_overridden(s, l, &ov, _STD_ID_GET, MB_MF_FUNC);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		_MAKE_NIL(&obj);
		switch(ov.type) {
#ifdef MB_ENABLE_COLLECTION_LIB
		case MB_DT_LIST:
			_public_value_to_internal_object(&ov, &obj);
			_mb_check_mark_exit(mb_pop_int(s, l, &index), result, _exit);
			if(!_at_list(obj.data.list, index, &ret)) {
				_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}

			break;
		case MB_DT_DICT:
			_public_value_to_internal_object(&ov, &obj);
			_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);
			if(!_find_dict(obj.data.dict, &arg, &ret)) {
				_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}

			break;
		case MB_DT_LIST_IT:
			_public_value_to_internal_object(&ov, &obj);
			if(obj.data.list_it && !obj.data.list_it->list->range_begin && obj.data.list_it->curr.node && obj.data.list_it->curr.node->data) {
				_internal_object_to_public_value((_object_t*)obj.data.list_it->curr.node->data, &ret);
			} else if(obj.data.list_it && obj.data.list_it->list->range_begin) {
				mb_make_int(ret, obj.data.list_it->curr.ranging);
			} else {
				_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}

			break;
		case MB_DT_DICT_IT:
			_public_value_to_internal_object(&ov, &obj);
			if(obj.data.dict_it && obj.data.dict_it->curr_node && obj.data.dict_it->curr_node != _INVALID_DICT_IT && obj.data.dict_it->curr_node->extra) {
				_internal_object_to_public_value((_object_t*)obj.data.dict_it->curr_node->extra, &ret);
			} else {
				_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}

			break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		case MB_DT_CLASS:
			_public_value_to_internal_object(&ov, &obj);
			_mb_check_mark_exit(mb_pop_string(s, l, &field), result, _exit);
			field = mb_strupr(field);
			fnode = _search_identifier_in_class(s, obj.data.instance, field, 0, 0);
			if(fnode && fnode->data) {
				fobj = (_object_t*)fnode->data;
				_internal_object_to_public_value(fobj, &ret);
			}

			break;
#endif /* MB_ENABLE_CLASS */
		default:
			_handle_error_on_obj(s, SE_RN_COLLECTION_OR_ITERATOR_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	if((os & MB_MS_RETURNED) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_push_value(s, l, ret), result, _exit);
	}

_exit:
	_assign_public_value(s, &ov, 0, true);

	return result;
}

/* SET statement */
static int _std_set(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t ov;
	mb_value_t key;
	mb_value_t val;
	_object_t obj;
#ifdef MB_ENABLE_COLLECTION_LIB
	_object_t* oval = 0;
	int_t idx = 0;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
	char* field = 0;
	_ls_node_t* fnode = 0;
	_object_t* fobj = 0;
	mb_value_t nv;
#endif /* MB_ENABLE_CLASS */
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(ov);
	mb_make_nil(key);
	mb_make_nil(val);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &ov), result, _exit);
	os = _try_overridden(s, l, &ov, _STD_ID_SET, MB_MF_FUNC);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		_MAKE_NIL(&obj);
		switch(ov.type) {
#ifdef MB_ENABLE_COLLECTION_LIB
		case MB_DT_LIST:
			_public_value_to_internal_object(&ov, &obj);
			while(mb_has_arg(s, l)) {
				mb_make_nil(val);
				_mb_check_mark_exit(mb_pop_int(s, l, &idx), result, _exit);
				_mb_check_mark_exit(mb_pop_value(s, l, &val), result, _exit);
				if(!_set_list(obj.data.list, idx, &val, &oval)) {
					if(oval)
						_destroy_object(oval, 0);

					_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
				}
			}

			break;
		case MB_DT_DICT:
			_public_value_to_internal_object(&ov, &obj);
			while(mb_has_arg(s, l)) {
				mb_make_nil(key);
				mb_make_nil(val);
				_mb_check_mark_exit(mb_pop_value(s, l, &key), result, _exit);
				_mb_check_mark_exit(mb_pop_value(s, l, &val), result, _exit);
				if(!_set_dict(obj.data.dict, &key, &val, 0, 0)) {
					_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
				}
			}

			break;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
		case MB_DT_CLASS:
			mb_make_nil(nv);

			_public_value_to_internal_object(&ov, &obj);
			_mb_check_mark_exit(mb_pop_string(s, l, &field), result, _exit);
			_mb_check_mark_exit(mb_pop_value(s, l, &nv), result, _exit);
			field = mb_strupr(field);
			fnode = _search_identifier_in_class(s, obj.data.instance, field, 0, 0);
			if(fnode && _IS_VAR(fnode->data)) {
				_object_t* nobj = 0;
				fobj = (_object_t*)fnode->data;
				_destroy_object(fobj->data.variable->data, 0);
				_create_internal_object_from_public_value(&nv, &nobj);
				fobj->data.variable->data = nobj;
			} else {
				_handle_error_on_obj(s, SE_RN_VAR_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}

			break;
#endif /* MB_ENABLE_CLASS */
		default:
			_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	if((os & MB_MS_RETURNED) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_push_value(s, l, ov), result, _exit);
	}

_exit:
	_assign_public_value(s, &ov, 0, true);

	return result;
}

/* PRINT statement */
static int _std_print(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	_object_t val_obj;
	_object_t* val_ptr = 0;
	bool_t pathed_str = false;

	mb_assert(s && l);

	++s->no_eat_comma_mark;
	ast = (_ls_node_t*)*l;
	ast = ast->next;
	if(!ast || !ast->data) {
		_handle_error_on_obj(s, SE_RN_SYNTAX_ERROR, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

	obj = (_object_t*)ast->data;
	do {
		pathed_str = false;
		val_ptr = &val_obj;
		_MAKE_NIL(val_ptr);
		switch(obj->type) {
		case _DT_VAR:
			if(obj->data.variable->data->type == _DT_ROUTINE) {
				obj = obj->data.variable->data;
				val_ptr = _eval_var_in_print(s, &val_ptr, &ast, obj);

				goto _print;
			}
#ifdef MB_ENABLE_CLASS
			if(obj->data.variable->pathing) {
				_ls_node_t* pathed = _search_identifier_in_scope_chain(s, 0, obj->data.variable->name, _PU(obj->data.variable->pathing), 0, 0);
				if(pathed && pathed->data) {
					if(obj != (_object_t*)pathed->data) {
						obj = (_object_t*)pathed->data;
						val_ptr = _eval_var_in_print(s, &val_ptr, &ast, obj);
						if(val_ptr->type == _DT_STRING)
							pathed_str = true;
					}
				}

				goto _print;
			}
#endif /* MB_ENABLE_CLASS */
			/* Fall through */
		case _DT_ARRAY: /* Fall through */
		case _DT_NIL: /* Fall through */
		case _DT_INT: /* Fall through */
		case _DT_REAL: /* Fall through */
		case _DT_STRING: /* Fall through */
		case _DT_TYPE: /* Fall through */
#ifdef MB_ENABLE_CLASS
		case _DT_CLASS: /* Fall through */
#endif /* MB_ENABLE_CLASS */
		case _DT_FUNC: /* Fall through */
		case _DT_ROUTINE:
			result = _calc_expression(s, &ast, &val_ptr);
			if(val_ptr->type == _DT_ROUTINE) {
#ifdef MB_ENABLE_LAMBDA
				if(val_ptr->data.routine->type != MB_RT_LAMBDA)
					val_ptr->is_ref = true;
#else /* MB_ENABLE_LAMBDA */
				val_ptr->is_ref = true;
#endif /* MB_ENABLE_LAMBDA */
			}
			_REF(val_ptr)
			_UNREF(val_ptr)
_print:
			if(val_ptr->type == _DT_NIL) {
				_get_printer(s)(MB_NIL);
			} else if(val_ptr->type == _DT_INT) {
				_get_printer(s)(MB_INT_FMT, val_ptr->data.integer);
			} else if(val_ptr->type == _DT_REAL) {
#ifdef MB_MANUAL_REAL_FORMATTING
				_dynamic_buffer_t buf;
				size_t lbuf = 32;
				_INIT_BUF(buf);
				_RESIZE_CHAR_BUF(buf, lbuf);
				_real_to_str(val_ptr->data.float_point, _CHAR_BUF_PTR(buf), lbuf, 5);
				_get_printer(s)("%s", _CHAR_BUF_PTR(buf));
				_DISPOSE_BUF(buf);
#else /* MB_MANUAL_REAL_FORMATTING */
				_get_printer(s)(MB_REAL_FMT, val_ptr->data.float_point);
#endif /* MB_MANUAL_REAL_FORMATTING */
			} else if(val_ptr->type == _DT_STRING) {
				_print_string(s, val_ptr);
				if(!val_ptr->is_ref && val_ptr->data.string && !pathed_str) {
					safe_free(val_ptr->data.string);
				}
#ifdef MB_ENABLE_USERTYPE_REF
			} else if(val_ptr->type == _DT_USERTYPE_REF) {
				if(val_ptr->data.usertype_ref->fmt) {
					_dynamic_buffer_t buf;
					size_t lbuf = 0;
					_INIT_BUF(buf);
					while((lbuf = (size_t)val_ptr->data.usertype_ref->fmt(s, val_ptr->data.usertype_ref->usertype, _CHAR_BUF_PTR(buf), (unsigned)_CHARS_OF_BUF(buf))) > _CHARS_OF_BUF(buf)) {
						_RESIZE_CHAR_BUF(buf, lbuf);
					}
					_get_printer(s)("%s", _CHAR_BUF_PTR(buf));
					_DISPOSE_BUF(buf);
				} else {
					_get_printer(s)(mb_get_type_string(_internal_type_to_public_type(val_ptr->type)));
				}
#endif /* MB_ENABLE_USERTYPE_REF */
			} else if(val_ptr->type == _DT_TYPE) {
				_get_printer(s)(mb_get_type_string(val_ptr->data.type));
#ifdef MB_ENABLE_CLASS
			} else if(val_ptr->type == _DT_CLASS) {
				bool_t got_tostr = false;
				_class_t* instance = val_ptr->data.instance;
				val_ptr = &val_obj;
				_MAKE_NIL(val_ptr);
				if((result = _format_class_to_string(s, (void**)&ast, instance, val_ptr, &got_tostr)) == MB_FUNC_OK) {
					if(got_tostr) {
						obj = val_ptr;

						goto _print;
					} else {
						_get_printer(s)(mb_get_type_string(_internal_type_to_public_type(_DT_CLASS)));
					}
				} else {
					goto _exit;
				}
#endif /* MB_ENABLE_CLASS */
			} else {
				_get_printer(s)(mb_get_type_string(_internal_type_to_public_type(val_ptr->type)));
			}
			if(result != MB_FUNC_OK)
				goto _exit;
			/* Fall through */
		case _DT_SEP:
			if(!ast)
				break;
			obj = (_object_t*)ast->data;
#if _COMMA_AS_NEWLINE
			if(obj->data.separator == ',') {
#else /* _COMMA_AS_NEWLINE */
			if(obj->data.separator == ';') {
#endif /* _COMMA_AS_NEWLINE */
				_get_printer(s)("\n");
			}

			break;
		default:
			_handle_error_on_obj(s, SE_CM_NOT_SUPPORTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);

			break;
		}

		if(!ast)
			break;
		obj = (_object_t*)ast->data;
		if(_is_print_terminal(s, obj))
			break;
		if(_IS_SEP(obj, ',') || _IS_SEP(obj, ';')) {
			ast = ast->next;
			obj = (_object_t*)ast->data;
		} else {
			_handle_error_on_obj(s, SE_RN_COMMA_OR_SEMICOLON_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
	} while(ast && !_IS_SEP(obj, ':') && !_IS_FUNC(obj, _core_close_bracket) && (obj->type == _DT_SEP || !_is_expression_terminal(s, obj)));

_exit:
	--s->no_eat_comma_mark;

	*l = ast;
	if(result != MB_FUNC_OK)
		_get_printer(s)("\n");

	return result;
}

/* INPUT statement */
static int _std_input(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	_ls_node_t* ast = 0;
	_object_t* obj = 0;
	char line[_INPUT_MAX_LENGTH];
	char* conv_suc = 0;
	const char* pmt = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_func_begin(s, l));

	mb_check(mb_attempt_func_end(s, l));

	ast = (_ls_node_t*)*l;
	obj = ast ? (_object_t*)ast->data : 0;

	if(!obj || obj->type == _DT_EOS) {
#ifdef MB_CP_VC
		getch();
#else /* MB_CP_VC */
		_get_inputer(s)(pmt, line, sizeof(line));
#endif /* MB_CP_VC */

		goto _exit;
	}
	if(obj->type == _DT_STRING) {
		pmt = obj->data.string;
		_print_string(s, obj);
		ast = ast->next;
		obj = (_object_t*)ast->data;
		if(!_IS_SEP(obj, ',')) {
			_handle_error_on_obj(s, SE_RN_COMMA_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
		}
		ast = ast->next;
		obj = (_object_t*)ast->data;
	}
	if(obj->type != _DT_VAR) {
		_handle_error_on_obj(s, SE_RN_VAR_EXPECTED, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}
	if(obj->data.variable->data->type == _DT_INT || obj->data.variable->data->type == _DT_REAL) {
		_get_inputer(s)(pmt, line, sizeof(line));
		obj->data.variable->data->type = _DT_INT;
		obj->data.variable->data->data.integer = (int_t)mb_strtol(line, &conv_suc, 0);
		if(*conv_suc != _ZERO_CHAR) {
			obj->data.variable->data->type = _DT_REAL;
			obj->data.variable->data->data.float_point = (real_t)mb_strtod(line, &conv_suc);
			if(*conv_suc != _ZERO_CHAR) {
				_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
			}
		}
		ast = ast->next;
	} else if(obj->data.variable->data->type == _DT_STRING) {
		size_t len = 0;
		if(obj->data.variable->data->data.string && !obj->data.variable->data->is_ref) {
			safe_free(obj->data.variable->data->data.string);
		}
		len = (size_t)_get_inputer(s)(pmt, line, sizeof(line));
#if defined MB_CP_VC && defined MB_ENABLE_UNICODE
		do {
			_dynamic_buffer_t buf;
			_dynamic_buffer_t wbuf;
			_INIT_BUF(buf);
			_INIT_BUF(wbuf);
			while((len = (size_t)mb_bytes_to_wchar_ansi(line, &_WCHAR_BUF_PTR(wbuf), _WCHARS_OF_BUF(wbuf))) > _WCHARS_OF_BUF(wbuf)) {
				_RESIZE_WCHAR_BUF(wbuf, len);
			}
			while((len = mb_wchar_to_bytes(_WCHAR_BUF_PTR(wbuf), &_CHAR_BUF_PTR(buf), _CHARS_OF_BUF(buf))) > _CHARS_OF_BUF(buf)) {
				_RESIZE_CHAR_BUF(buf, len);
			}
			_DISPOSE_BUF(wbuf);
			obj->data.variable->data->data.string = _HEAP_CHAR_BUF(buf);
			obj->data.variable->data->is_ref = false;
		} while(0);
#else /* MB_CP_VC && MB_ENABLE_UNICODE */
		obj->data.variable->data->data.string = mb_memdup(line, (unsigned)(len + 1));
#endif /* MB_CP_VC && MB_ENABLE_UNICODE */
		ast = ast->next;
	} else {
		_handle_error_on_obj(s, SE_RN_INVALID_ID_USAGE, s->source_file, DON(ast), MB_FUNC_ERR, _exit, result);
	}

_exit:
	*l = ast;

	return result;
}

/** Collection lib */

#ifdef MB_ENABLE_COLLECTION_LIB
/* LIST statement */
static int _coll_list(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;
	_list_t* coll = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	coll = _create_list(s);

	if(mb_has_arg(s, l)) {
		_ls_node_t* ast = 0;
		_object_t* obj = 0;
		mb_make_nil(arg);
		_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _error);
		ast = (_ls_node_t*)*l;
		if(ast) obj = (_object_t*)ast->data;
		if(arg.type == MB_DT_INT && obj && _IS_FUNC(obj, _core_to)) {
			/* Push a range of integer */
			int_t begin = arg.value.integer;
			int_t end = 0;
			int_t step = 0;
			ast = ast->next;
			_mb_check_mark_exit(mb_pop_int(s, (void**)&ast, &end), result, _error);
			step = sgn(end - begin);
			coll->range_begin = (int_t*)mb_malloc(sizeof(int_t));
			*coll->range_begin = begin;
			coll->count = end - begin + step;
			if(!coll->count) coll->count = 1;
			*l = ast;
		} else {
			/* Push arguments */
			if(!_push_list(coll, &arg, 0)) {
				_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, s->source_file, DON2(l), MB_FUNC_ERR, _error, result);
			}
			while(mb_has_arg(s, l)) {
				mb_make_nil(arg);
				_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _error);
				if(!_push_list(coll, &arg, 0)) {
					_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, s->source_file, DON2(l), MB_FUNC_ERR, _error, result);
				}
			}
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _error);

	arg.type = MB_DT_LIST;
	arg.value.list = coll;
	_mb_check_mark_exit(mb_push_value(s, l, arg), result, _error);

	while(0) {
_error:
		mb_make_nil(arg);
		mb_push_value(s, l, arg);
		_destroy_list(coll);
	}

	return result;
}

/* DICT statement */
static int _coll_dict(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t arg;
	mb_value_t val;
	_dict_t* coll = 0;

	mb_assert(s && l);

	mb_check(mb_attempt_open_bracket(s, l));

	coll = _create_dict(s);

	while(mb_has_arg(s, l)) {
		mb_make_nil(arg);
		mb_make_nil(val);
		_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _error);
		_mb_check_mark_exit(mb_pop_value(s, l, &val), result, _error);
		if(!_set_dict(coll, &arg, &val, 0, 0)) {
			_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, s->source_file, DON2(l), MB_FUNC_ERR, _error, result);
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _error);

	arg.type = MB_DT_DICT;
	arg.value.dict = coll;
	_mb_check_mark_exit(mb_push_value(s, l, arg), result, _error);

	while(0) {
_error:
		mb_make_nil(arg);
		mb_push_value(s, l, arg);
		_destroy_dict(coll);
	}

	return result;
}

/* PUSH statement */
static int _coll_push(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t coll;
	mb_value_t arg;
	_object_t olst;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &coll), result, _exit);
	os = _try_overridden(s, l, &coll, _COLL_ID_PUSH, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		if(coll.type != MB_DT_LIST) {
			_handle_error_on_obj(s, SE_RN_LIST_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}
		_MAKE_NIL(&olst);
		_public_value_to_internal_object(&coll, &olst);

		while(mb_has_arg(s, l)) {
			mb_make_nil(arg);
			_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);
			if(!_push_list(olst.data.list, &arg, 0)) {
				_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	if((os & MB_MS_RETURNED) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_push_value(s, l, coll), result, _exit);
	}

_exit:
	_assign_public_value(s, &coll, 0, true);

	return result;
}

/* POP statement */
static int _coll_pop(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t coll;
	mb_value_t val;
	_object_t olst;
	_object_t ocoll;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(val);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &coll));
	os = _try_overridden(s, l, &coll, _COLL_ID_POP, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		if(coll.type != MB_DT_LIST) {
			_assign_public_value(s, &coll, 0, true);
			_handle_error_on_obj(s, SE_RN_LIST_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}

		_MAKE_NIL(&olst);
		_public_value_to_internal_object(&coll, &olst);
		if(_pop_list(olst.data.list, &val, s)) {
			mb_check(mb_push_value(s, l, val));
			_MAKE_NIL(&ocoll);
			_public_value_to_internal_object(&val, &ocoll);
			_UNREF(&ocoll)

			_assign_public_value(s, &coll, 0, true);
		} else {
			mb_check(mb_push_value(s, l, val));

			_assign_public_value(s, &coll, 0, true);

			_handle_error_on_obj(s, SE_RN_EMPTY_COLLECTION, s->source_file, DON2(l), MB_FUNC_WARNING, _exit, result);
		}
	} else {
		if((os & MB_MS_RETURNED) == MB_MS_NONE) {
			mb_check(mb_push_value(s, l, val));
		}
	}

	mb_check(mb_attempt_close_bracket(s, l));

_exit:
	return result;
}

/* BACK statement */
static int _coll_back(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t coll;
	mb_value_t val;
	_object_t olst;
	_object_t* oval = 0;
	_ls_node_t* node = 0;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(val);

	mb_check(mb_attempt_open_bracket(s, l));

	mb_check(mb_pop_value(s, l, &coll));
	os = _try_overridden(s, l, &coll, _COLL_ID_BACK, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		if(coll.type != MB_DT_LIST) {
			_assign_public_value(s, &coll, 0, true);
			_handle_error_on_obj(s, SE_RN_LIST_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}

		_MAKE_NIL(&olst);
		_public_value_to_internal_object(&coll, &olst);
		node = _ls_back(olst.data.list->list);
		oval = node ? (_object_t*)node->data : 0;
		if(oval) {
			_internal_object_to_public_value(oval, &val);

			mb_check(mb_push_value(s, l, val));

			_assign_public_value(s, &coll, 0, true);
		} else {
			mb_check(mb_push_value(s, l, val));

			_assign_public_value(s, &coll, 0, true);

			_handle_error_on_obj(s, SE_RN_EMPTY_COLLECTION, s->source_file, DON2(l), MB_FUNC_WARNING, _exit, result);
		}
	} else {
		if((os & MB_MS_RETURNED) == MB_MS_NONE) {
			mb_check(mb_push_value(s, l, val));
		}
	}

	mb_check(mb_attempt_close_bracket(s, l));

_exit:
	return result;
}

/* INSERT statement */
static int _coll_insert(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t coll;
	int_t idx = 0;
	mb_value_t arg;
	_object_t olst;
	_object_t* oval = 0;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(arg);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &coll), result, _exit);
	os = _try_overridden(s, l, &coll, _COLL_ID_INSERT, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_pop_int(s, l, &idx), result, _exit);
		_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

		if(coll.type != MB_DT_LIST) {
			_handle_error_on_obj(s, SE_RN_LIST_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}
		_MAKE_NIL(&olst);
		_public_value_to_internal_object(&coll, &olst);

		if(!_insert_list(olst.data.list, idx, &arg, &oval)) {
			if(oval)
				_destroy_object(oval, 0);

			_handle_error_on_obj(s, SE_RN_INVALID_EXPRESSION, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}

		_mb_check_mark_exit(mb_push_value(s, l, coll), result, _exit);
	} else {
		if((os & MB_MS_RETURNED) == MB_MS_NONE) {
			_mb_check_mark_exit(mb_push_value(s, l, coll), result, _exit);
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

_exit:
	_assign_public_value(s, &coll, 0, true);

	return result;
}

/* SORT statement */
static int _coll_sort(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t coll;
	_object_t olst;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(coll);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &coll), result, _exit);
	os = _try_overridden(s, l, &coll, _COLL_ID_SORT, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		if(coll.type != MB_DT_LIST) {
			_handle_error_on_obj(s, SE_RN_LIST_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
		}
		_MAKE_NIL(&olst);
		_public_value_to_internal_object(&coll, &olst);

		_sort_list(olst.data.list);
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	if((os & MB_MS_RETURNED) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_push_value(s, l, coll), result, _exit);
	}

_exit:
	_assign_public_value(s, &coll, 0, true);

	return result;
}

/* EXISTS statement */
static int _coll_exists(mb_interpreter_t* s, void** l){
	int result = MB_FUNC_OK;
	mb_value_t coll;
	mb_value_t arg;
	_object_t ocoll;
	mb_value_t ret;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(arg);
	mb_make_nil(ret);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &coll), result, _exit);
	os = _try_overridden(s, l, &coll, _COLL_ID_EXISTS, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_pop_value(s, l, &arg), result, _exit);

		_MAKE_NIL(&ocoll);
		switch(coll.type) {
		case MB_DT_LIST:
			_public_value_to_internal_object(&coll, &ocoll);
			mb_make_bool(ret, _find_list(ocoll.data.list, &arg, 0));

			break;
		case MB_DT_DICT:
			_public_value_to_internal_object(&coll, &ocoll);
			mb_make_bool(ret, _find_dict(ocoll.data.dict, &arg, 0));

			break;
		default:
			_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		}
		_mb_check_mark_exit(mb_push_value(s, l, ret), result, _exit);
	} else {
		if((os & MB_MS_RETURNED) == MB_MS_NONE) {
			_mb_check_mark_exit(mb_push_value(s, l, coll), result, _exit);
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

_exit:
	_assign_public_value(s, &coll, 0, true);

	return result;
}

/* INDEX_OF statement */
static int _coll_index_of(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	int idx = 0;
	mb_value_t coll;
	_object_t ocoll;
	mb_value_t val;
	mb_value_t ret;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(val);
	mb_make_nil(ret);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &coll), result, _exit);
	os = _try_overridden(s, l, &coll, _COLL_ID_INDEX_OF, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		ret.type = MB_DT_UNKNOWN;
		_mb_check_mark_exit(mb_pop_value(s, l, &val), result, _exit);
		_MAKE_NIL(&ocoll);
		switch(coll.type) {
		case MB_DT_LIST:
			_public_value_to_internal_object(&coll, &ocoll);
			if(_find_list(ocoll.data.list, &val, &idx)) {
				mb_make_int(ret, (int_t)idx);
			}

			break;
		default:
			_handle_error_on_obj(s, SE_RN_LIST_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	if((os & MB_MS_RETURNED) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_push_value(s, l, ret), result, _exit);
	}

_exit:
	_assign_public_value(s, &coll, 0, true);

	return result;
}

/* REMOVE statement */
static int _coll_remove(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t coll;
	int_t idx = 0;
	mb_value_t key;
	_object_t ocoll;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(key);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &coll), result, _exit);
	os = _try_overridden(s, l, &coll, _COLL_ID_REMOVE, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		_MAKE_NIL(&ocoll);
		switch(coll.type) {
		case MB_DT_LIST:
			_public_value_to_internal_object(&coll, &ocoll);
			while(mb_has_arg(s, l)) {
				_mb_check_mark_exit(mb_pop_int(s, l, &idx), result, _exit);

				if(!_remove_at_list(ocoll.data.list, idx)) {
					_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
				}
			}

			break;
		case MB_DT_DICT:
			_public_value_to_internal_object(&coll, &ocoll);
			while(mb_has_arg(s, l)) {
				_mb_check_mark_exit(mb_pop_value(s, l, &key), result, _exit);

				if(!_remove_dict(ocoll.data.dict, &key)) {
					_handle_error_on_obj(s, SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
				}
			}

			break;
		default:
			_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	if((os & MB_MS_RETURNED) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_push_value(s, l, coll), result, _exit);
	}

_exit:
	_assign_public_value(s, &coll, 0, true);

	return result;
}

/* CLEAR statement */
static int _coll_clear(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t coll;
	_object_t ocoll;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(coll);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &coll), result, _exit);
	os = _try_overridden(s, l, &coll, _COLL_ID_CLEAR, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		_MAKE_NIL(&ocoll);
		switch(coll.type) {
		case MB_DT_LIST:
			_public_value_to_internal_object(&coll, &ocoll);
			_clear_list(ocoll.data.list);

			break;
		case MB_DT_DICT:
			_public_value_to_internal_object(&coll, &ocoll);
			_clear_dict(ocoll.data.dict);

			break;
		default:
			_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	if((os & MB_MS_RETURNED) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_push_value(s, l, coll), result, _exit);
	}

_exit:
	_assign_public_value(s, &coll, 0, true);

	return result;
}

/* CLONE statement */
static int _coll_clone(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t coll;
	_object_t ocoll;
	_object_t otgt;
	mb_value_t ret;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(ret);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &coll), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_MAKE_NIL(&ocoll);
	_MAKE_NIL(&otgt);
	switch(coll.type) {
#ifdef MB_ENABLE_USERTYPE_REF
	case MB_DT_USERTYPE_REF:
		_public_value_to_internal_object(&coll, &ocoll);
		_clone_usertype_ref(ocoll.data.usertype_ref, &otgt);
		_internal_object_to_public_value(&otgt, &ret);

		break;
#endif /* MB_ENABLE_USERTYPE_REF */
	case MB_DT_LIST:
		_public_value_to_internal_object(&coll, &ocoll);
		_clone_object(s, &ocoll, &otgt, false, true);
		ret.type = MB_DT_LIST;
		ret.value.list = otgt.data.list;

		break;
	case MB_DT_DICT:
		_public_value_to_internal_object(&coll, &ocoll);
		_clone_object(s, &ocoll, &otgt, false, true);
		ret.type = MB_DT_DICT;
		ret.value.dict = otgt.data.dict;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	_mb_check_mark_exit(mb_push_value(s, l, ret), result, _exit);

_exit:
	_assign_public_value(s, &coll, 0, true);

	return result;
}

/* TO_ARRAY statement */
static int _coll_to_array(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t coll;
	_object_t ocoll;
	_array_t* array = 0;
	_array_helper_t helper;
	mb_value_t ret;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(ret);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &coll), result, _exit);

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	_MAKE_NIL(&ocoll);
	switch(coll.type) {
	case MB_DT_LIST:
		_public_value_to_internal_object(&coll, &ocoll);
		array = _create_array(s, mb_strdup("", 1), _DT_REAL);
		array->count = ocoll.data.list->count;
		array->dimension_count = 1;
		array->dimensions[0] = ocoll.data.list->count;
		_init_array(array);
		helper.s = s;
		helper.array = array;
		helper.index = 0;
		_LS_FOREACH(ocoll.data.list->list, _do_nothing_on_object, _copy_list_to_array, &helper);
		ret.type = MB_DT_ARRAY;
		ret.value.array = array;

		break;
	default:
		_handle_error_on_obj(s, SE_RN_LIST_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

		break;
	}

	_mb_check_mark_exit(mb_push_value(s, l, ret), result, _exit);

_exit:
	_assign_public_value(s, &coll, 0, true);

	return result;
}

/* ITERATOR statement */
static int _coll_iterator(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t coll;
	_object_t ocoll;
	_list_it_t* lit = 0;
	_dict_it_t* dit = 0;
	mb_value_t ret;
	mb_meta_status_e os = MB_MS_NONE;

	mb_assert(s && l);

	mb_make_nil(coll);
	mb_make_nil(ret);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	_mb_check_mark_exit(mb_pop_value(s, l, &coll), result, _exit);
	os = _try_overridden(s, l, &coll, _COLL_ID_ITERATOR, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		_MAKE_NIL(&ocoll);
		switch(coll.type) {
		case MB_DT_LIST:
			_public_value_to_internal_object(&coll, &ocoll);
			lit = _create_list_it(ocoll.data.list, false);
			ret.type = MB_DT_LIST_IT;
			ret.value.list_it = lit;

			break;
		case MB_DT_DICT:
			_public_value_to_internal_object(&coll, &ocoll);
			dit = _create_dict_it(ocoll.data.dict, false);
			ret.type = MB_DT_DICT_IT;
			ret.value.dict_it = dit;

			break;
		default:
			_handle_error_on_obj(s, SE_RN_COLLECTION_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	if((os & MB_MS_RETURNED) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_push_value(s, l, ret), result, _exit);
	}

_exit:
	_assign_public_value(s, &coll, 0, true);

	return result;
}

/* MOVE_NEXT statement */
static int _coll_move_next(mb_interpreter_t* s, void** l) {
	int result = MB_FUNC_OK;
	mb_value_t it;
	_object_t oit;
	mb_value_t ret;
	mb_meta_status_e os = MB_MS_NONE;
	_ls_node_t* ast = 0;

	mb_assert(s && l);

	mb_make_nil(it);
	mb_make_nil(ret);

	_mb_check_mark_exit(mb_attempt_open_bracket(s, l), result, _exit);

	ast = (_ls_node_t*)*l;
	if(ast && _IS_FUNC(ast->data, _coll_iterator)) {
		_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
	}
	_mb_check_mark_exit(mb_pop_value(s, l, &it), result, _exit);
	os = _try_overridden(s, l, &it, _COLL_ID_MOVE_NEXT, MB_MF_COLL);
	if((os & MB_MS_DONE) == MB_MS_NONE) {
		_MAKE_NIL(&oit);
		switch(it.type) {
		case MB_DT_LIST_IT:
			_public_value_to_internal_object(&it, &oit);
			oit.data.list_it = _move_list_it_next(oit.data.list_it);
			if(_invalid_list_it(oit.data.list_it)) {
				_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			} else if(oit.data.list_it) {
				mb_make_bool(ret, true);
			} else {
				mb_make_nil(ret);
			}

			break;
		case MB_DT_DICT_IT:
			_public_value_to_internal_object(&it, &oit);
			if(oit.data.dict_it && oit.data.dict_it->curr_node == _INVALID_DICT_IT && _invalid_dict_it(oit.data.dict_it)) {
				_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			}
			oit.data.dict_it = _move_dict_it_next(oit.data.dict_it);
			if(_invalid_dict_it(oit.data.dict_it)) {
				_handle_error_on_obj(s, SE_RN_INVALID_ITERATOR, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);
			} else if(oit.data.dict_it) {
				mb_make_bool(ret, true);
			} else {
				mb_make_nil(ret);
			}

			break;
		default:
			_assign_public_value(s, &it, 0, true);
			_handle_error_on_obj(s, SE_RN_ITERABLE_EXPECTED, s->source_file, DON2(l), MB_FUNC_ERR, _exit, result);

			break;
		}
	}

	_mb_check_mark_exit(mb_attempt_close_bracket(s, l), result, _exit);

	if((os & MB_MS_RETURNED) == MB_MS_NONE) {
		_mb_check_mark_exit(mb_push_value(s, l, ret), result, _exit);
	}

_exit:
	return result;
}
#endif /* MB_ENABLE_COLLECTION_LIB */

/* ========================================================} */

#ifdef MB_COMPACT_MODE
# pragma pack()
#endif /* MB_COMPACT_MODE */

#ifdef MB_CP_BORLANDC
# pragma warn .8004
# pragma warn .8008
# pragma warn .8012
#endif /* MB_CP_BORLANDC */

#ifdef MB_CP_CLANG
# pragma clang diagnostic pop
#endif /* MB_CP_CLANG */

#ifdef MB_CP_VC
# pragma warning(pop)
#endif /* MB_CP_VC */

#ifdef __cplusplus
}
#endif /* __cplusplus */
