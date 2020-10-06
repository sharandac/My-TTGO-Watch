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

#ifndef __MY_BASIC_H__
#define __MY_BASIC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#define enum enum  __attribute__((__packed__))


#if defined __EMSCRIPTEN__
#  define MB_CP_EMSCRIPTEN
#elif defined ARDUINO
# define MB_CP_ARDUINO
#elif defined __BORLANDC__
# define MB_CP_BORLANDC
#elif defined __clang__
# define MB_CP_CLANG
#elif defined __CYGWIN__
# define MB_CP_CYGWIN
#elif defined __GNUC__ || defined __GNUG__
# define MB_CP_GCC
#elif defined __HP_cc || defined __HP_aCC
# define MB_CP_HPC
#elif defined __IBMC__ || defined __IBMCPP__
# define MB_CP_IBMC
#elif defined __ICC || defined __INTEL_COMPILER
# define MB_CP_ICC
#elif defined __MINGW32__
# define MB_CP_MINGW32
#elif defined __POCC__
# define MB_CP_PELLESC
#elif defined __PGI
# define MB_CP_PGCC
#elif defined __SUNPRO_C || defined __SUNPRO_CC
# define MB_CP_SOLARISC
#elif defined __TINYC__
# define MB_CP_TCC
#elif defined _MSC_VER
# define MB_CP_VC _MSC_VER
#else
# define MB_CP_UNKNOWN
#endif /* Compiler dependent macro */

#if defined __EMSCRIPTEN__
# define MB_OS_HTML
#elif defined _WIN64
# define MB_OS_WIN
# define MB_OS_WIN64
#elif defined _WIN32
# define MB_OS_WIN
# define MB_OS_WIN32
#elif defined __APPLE__
# include <TargetConditionals.h>
# define MB_OS_APPLE
# if defined TARGET_OS_IPHONE && TARGET_OS_IPHONE == 1
#   define MB_OS_IOS
# elif defined TARGET_IPHONE_SIMULATOR && TARGET_IPHONE_SIMULATOR == 1
#   define MB_OS_IOS_SIM
# elif defined TARGET_OS_MAC && TARGET_OS_MAC == 1
#   define MB_OS_MAC
# endif
#elif defined __ANDROID__
# define MB_OS_ANDROID
#elif defined __linux__
# define MB_OS_LINUX
#elif defined __unix__
# define MB_OS_UNIX
#else
# define MB_OS_UNKNOWN
#endif /* OS dependent macro */

#ifndef MBAPI
# define MBAPI
#endif /* MBAPI */

#ifndef MBIMPL
# define MBIMPL
#endif /* MBIMPL */

#ifndef MBCONST
# define MBCONST
#endif /* MBCONST */

#ifndef MB_SIMPLE_ARRAY
# define MB_SIMPLE_ARRAY
#endif /* MB_SIMPLE_ARRAY */

#ifndef MB_ENABLE_ARRAY_REF
# define MB_ENABLE_ARRAY_REF
#endif /* MB_ENABLE_ARRAY_REF */

#ifndef MB_MAX_DIMENSION_COUNT
# define MB_MAX_DIMENSION_COUNT 4
#endif /* MB_MAX_DIMENSION_COUNT */

#ifndef MB_ENABLE_COLLECTION_LIB
# define MB_ENABLE_COLLECTION_LIB
#endif /* MB_ENABLE_COLLECTION_LIB */

#ifndef MB_ENABLE_USERTYPE_REF
# define MB_ENABLE_USERTYPE_REF
#endif /* MB_ENABLE_USERTYPE_REF */

#ifdef MB_ENABLE_USERTYPE_REF
# ifndef MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF
#   define MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF
# endif /* MB_ENABLE_ALIVE_CHECKING_ON_USERTYPE_REF */
#endif /* MB_ENABLE_USERTYPE_REF */

#ifndef MB_ENABLE_CLASS
# define MB_ENABLE_CLASS
#endif /* MB_ENABLE_CLASS */

#ifndef MB_ENABLE_LAMBDA
# define MB_ENABLE_LAMBDA
#endif /* MB_ENABLE_LAMBDA */

#ifndef MB_ENABLE_MODULE
# define MB_ENABLE_MODULE
#endif /* MB_ENABLE_MODULE */

#ifndef MB_ENABLE_UNICODE
# define MB_ENABLE_UNICODE
#endif /* MB_ENABLE_UNICODE */

#ifndef MB_ENABLE_UNICODE_ID
# define MB_ENABLE_UNICODE_ID
# if defined MB_ENABLE_UNICODE_ID && !defined MB_ENABLE_UNICODE
#   error "Requires MB_ENABLE_UNICODE enabled."
# endif
#endif /* MB_ENABLE_UNICODE_ID */

#ifndef MB_ENABLE_FORK
# define MB_ENABLE_FORK
#endif /* MB_ENABLE_FORK */

#ifndef MB_GC_GARBAGE_THRESHOLD
# define MB_GC_GARBAGE_THRESHOLD 16
#endif /* MB_GC_GARBAGE_THRESHOLD */

#ifndef MB_ENABLE_ALLOC_STAT
# define MB_ENABLE_ALLOC_STAT
#endif /* MB_ENABLE_ALLOC_STAT */

#ifndef MB_ENABLE_SOURCE_TRACE
//# define MB_ENABLE_SOURCE_TRACE
#endif /* MB_ENABLE_SOURCE_TRACE */

#ifndef MB_ENABLE_STACK_TRACE
//# define MB_ENABLE_STACK_TRACE
#endif /* MB_ENABLE_STACK_TRACE */

#ifndef MB_ENABLE_FULL_ERROR
# define MB_ENABLE_FULL_ERROR
#endif /* MB_ENABLE_FULL_ERROR */

#ifndef MB_CONVERT_TO_INT_LEVEL_NONE
# define MB_CONVERT_TO_INT_LEVEL_NONE 0
#endif /* MB_CONVERT_TO_INT_LEVEL_NONE */
#ifndef MB_CONVERT_TO_INT_LEVEL_ALL
# define MB_CONVERT_TO_INT_LEVEL_ALL 1
#endif /* MB_CONVERT_TO_INT_LEVEL_ALL */

#ifndef MB_CONVERT_TO_INT_LEVEL
# define MB_CONVERT_TO_INT_LEVEL MB_CONVERT_TO_INT_LEVEL_ALL
#endif /* MB_CONVERT_TO_INT_LEVEL */

#ifndef MB_PREFER_SPEED
# define MB_PREFER_SPEED
#endif /* MB_PREFER_SPEED */

#ifndef MB_COMPACT_MODE
# define MB_COMPACT_MODE
#endif /* MB_COMPACT_MODE */

#ifdef MB_COMPACT_MODE
# pragma pack(1)
#endif /* MB_COMPACT_MODE */

#ifndef __cplusplus
# ifndef true
#   define true (1)
# endif /* true */
# ifndef false
#   define false (0)
# endif /* false */
#endif /* __cplusplus */

#ifndef bool_t
# define bool_t unsigned char
#endif /* bool_t */
#ifndef int_t
# define int_t int64_t
#endif /* int_t */
#ifndef real_t
# ifdef MB_DOUBLE_FLOAT
#   define real_t double
# else /* MB_DOUBLE_FLOAT */
#   define real_t float
# endif /* MB_DOUBLE_FLOAT */
#endif /* real_t */

#ifndef mb_strtol
# define mb_strtol(__s, __e, __r) strtol((__s), (__e), (__r))
#endif /* mb_strtol */
#ifndef mb_strtod
# define mb_strtod(__s, __e) strtod((__s), (__e))
#endif /* mb_strtod */

#ifndef MB_INT_FMT
# define MB_INT_FMT "%d"
#endif /* MB_INT_FMT */
#ifndef MB_REAL_FMT
# define MB_REAL_FMT "%g"
#endif /* MB_REAL_FMT */

#ifndef MB_FNAN
# ifdef MB_DOUBLE_FLOAT
#   define MB_FNAN 0x7FF8000000000000
# else /* MB_DOUBLE_FLOAT */
#   define MB_FNAN 0x7FC00000
# endif /* MB_DOUBLE_FLOAT */
#endif /* MB_FNAN */
#ifndef MB_FINF
# ifdef MB_DOUBLE_FLOAT
#   define MB_FINF 0x7FF0000000000000
# else /* MB_DOUBLE_FLOAT */
#   define MB_FINF 0x7F800000
# endif /* MB_DOUBLE_FLOAT */
#endif /* MB_FINF */

#ifndef MB_EOS
# define MB_EOS '\n'
#endif /* MB_EOS */
#ifndef MB_NIL
# define MB_NIL "NIL"
#endif /* MB_NIL */
#ifndef MB_TRUE
# define MB_TRUE "TRUE"
#endif /* MB_TRUE */
#ifndef MB_FALSE
# define MB_FALSE "FALSE"
#endif /* MB_FALSE */
#ifndef MB_NULL_STRING
# define MB_NULL_STRING "(EMPTY)"
#endif /* MB_NULL_STRING */

#ifndef mb_min
# define mb_min(a, b) (((a) < (b)) ? (a) : (b))
#endif /* mb_min */
#ifndef mb_max
# define mb_max(a, b) (((a) > (b)) ? (a) : (b))
#endif /* mb_max */

#ifndef mb_stricmp
# ifdef MB_CP_VC
#   define mb_stricmp _strcmpi
# else /* MB_CP_VC */
#   ifdef MB_CP_BORLANDC
#     define mb_stricmp stricmp
#   elif defined MB_CP_PELLESC
#     define mb_stricmp _stricmp
#   else
#     define mb_stricmp strcasecmp
#   endif
# endif /* MB_CP_VC */
#endif /* mb_stricmp */

#ifndef mb_assert
# define mb_assert(__a) do { ((void)(__a)); assert(__a); } while(0)
#endif /* mb_assert */

#ifndef mb_static_assert
# define _static_assert_impl(cond, msg) typedef char static_assertion_##msg[(!!(cond)) * 2 - 1]
# define _compile_time_assert3(x, l) _static_assert_impl(x, static_assertion_at_line_##l)
# define _compile_time_assert2(x, l) _compile_time_assert3(x, l)
# define mb_static_assert(x) _compile_time_assert2(x, __LINE__)
#endif /* mb_static_assert */

#ifndef mb_unrefvar
# define mb_unrefvar(__v) ((void)(__v))
#endif /* mb_unrefvar */

#ifndef mb_mem_tag_t
typedef unsigned short mb_mem_tag_t;
#endif /* mb_mem_tag_t */

#ifndef mb_bytes_size
# define mb_bytes_size (mb_max(mb_max(mb_max(sizeof(void*), sizeof(unsigned long)), sizeof(int_t)), sizeof(real_t)))
#endif /* mb_bytes_size */

#ifndef mb_make_nil
# define mb_make_nil(__v) do { memset(&(__v).value.bytes, 0, sizeof(mb_val_bytes_t)); (__v).type = MB_DT_NIL; } while(0)
#endif /* mb_make_nil */
#ifndef mb_make_type
# define mb_make_type(__v, __d) do { (__v).value.type = (__d); (__v).type = MB_DT_TYPE; } while(0)
#endif /* mb_make_type */
#ifndef mb_make_bool
# define mb_make_bool(__v, __d) do { (__v).value.integer = (!!(__d) ? true : false); (__v).type = MB_DT_INT; } while(0)
#endif /* mb_make_bool */
#ifndef mb_make_int
# define mb_make_int(__v, __d) do { (__v).value.integer = (__d); (__v).type = MB_DT_INT; } while(0)
#endif /* mb_make_int */
#ifndef mb_make_real
# define mb_make_real(__v, __d) do { (__v).value.float_point = (__d); (__v).type = MB_DT_REAL; } while(0)
#endif /* mb_make_real */
#ifndef mb_make_string
# define mb_make_string(__v, __d) do { (__v).value.string = (__d); (__v).type = MB_DT_STRING; } while(0)
#endif /* mb_make_string */
#ifndef mb_make_usertype
# define mb_make_usertype(__v, __d) do { memset(&(__v).value.bytes, 0, sizeof(mb_val_bytes_t)); (__v).value.usertype = (__d); (__v).type = MB_DT_USERTYPE; } while(0)
#endif /* mb_make_usertype */
#ifndef mb_make_usertype_bytes
# define mb_make_usertype_bytes(__v, __d) do { mb_static_assert(sizeof(mb_val_bytes_t) >= sizeof(*__d)); memcpy(&(__v).value.bytes, (__d), mb_min(sizeof(mb_val_bytes_t), sizeof(*__d))); (__v).type = MB_DT_USERTYPE; } while(0)
#endif /* mb_make_usertype_bytes */
#ifndef mb_make_array
# define mb_make_array(__v, __d) do { (__v).value.array = (__d); (__v).type = MB_DT_ARRAY; } while(0)
#endif /* mb_make_array */
#ifdef MB_ENABLE_COLLECTION_LIB
# ifndef mb_make_list
#   define mb_make_list(__v, __d) do { (__v).value.list = (__d); (__v).type = MB_DT_LIST; } while(0)
# endif /* mb_make_list */
# ifndef mb_make_dict
#   define mb_make_dict(__v, __d) do { (__v).value.dict = (__d); (__v).type = MB_DT_DICT; } while(0)
# endif /* mb_make_dict */
#endif /* MB_ENABLE_COLLECTION_LIB */

#ifndef mb_int_val
# define mb_int_val(__v, __d) do { if((__v).type == MB_DT_INT) (__d) = (__v).value.integer; else if((__v).type == MB_DT_REAL) (__d) = (int_t)((__v).value.float_point); else (__d) = ~((int_t)0); } while(0)
#endif /* mb_int_val */

#ifndef MB_CODES
# define MB_CODES
# define MB_FUNC_OK 0
# define MB_FUNC_IGNORE 1
# define MB_FUNC_WARNING 2
# define MB_FUNC_ERR 3
# define MB_FUNC_BYE 4
# define MB_FUNC_SUSPEND 5
# define MB_FUNC_END 6
# define MB_LOOP_BREAK 101
# define MB_LOOP_CONTINUE 102
# define MB_SUB_RETURN 103
# define MB_EXTENDED_ABORT 201
#endif /* MB_CODES */

#ifndef mb_check
# define mb_check(__expr) do { int __hr = (__expr); if((__hr) != MB_FUNC_OK) { return (__hr); } } while(0)
#endif /* mb_check */

#ifndef mb_reg_fun
# define mb_reg_fun(__s, __f) mb_register_func(__s, #__f, __f)
#endif /* mb_reg_fun */
#ifndef mb_rem_fun
# define mb_rem_fun(__s, __f) mb_remove_func(__s, #__f)
#endif /* mb_rem_fun */
#ifndef mb_rem_res_fun
# define mb_rem_res_fun(__s, __f) mb_remove_reserved_func(__s, #__f)
#endif /* mb_rem_res_fun */

#ifndef mb_convert_to_int_if_posible
# if MB_CONVERT_TO_INT_LEVEL == MB_CONVERT_TO_INT_LEVEL_NONE
#   define mb_convert_to_int_if_posible(__v) ((void)(__v))
# else /* MB_CONVERT_TO_INT_LEVEL == MB_CONVERT_TO_INT_LEVEL_NONE */
#   define mb_convert_to_int_if_posible(__v) \
      do { \
        if((__v).type == MB_DT_REAL && (real_t)(int_t)(__v).value.float_point == (__v).value.float_point) { \
          (__v).type = MB_DT_INT; \
          (__v).value.integer = (int_t)(__v).value.float_point; \
        } \
      } while(0)
# endif /* MB_CONVERT_TO_INT_LEVEL == MB_CONVERT_TO_INT_LEVEL_NONE */
#endif /* mb_convert_to_int_if_posible */

struct mb_interpreter_t;

typedef enum mb_error_e {
  SE_NO_ERR = 0,
  /** Common */
  SE_CM_FUNC_EXISTS,
  SE_CM_FUNC_NOT_EXISTS,
  SE_CM_NOT_SUPPORTED,
  /** Parsing */
  SE_PS_OPEN_FILE_FAILED,
  SE_PS_SYMBOL_TOO_LONG,
  SE_PS_INVALID_CHAR,
  SE_PS_INVALID_MODULE,
  /** Running */
  SE_RN_EMPTY_PROGRAM,
  SE_RN_PROGRAM_TOO_LONG,
  SE_RN_SYNTAX_ERROR,
  SE_RN_OUT_OF_MEMORY,
  SE_RN_OVERFLOW,
  SE_RN_UNEXPECTED_TYPE,
  SE_RN_INVALID_STRING,
  SE_RN_INTEGER_EXPECTED,
  SE_RN_NUMBER_EXPECTED,
  SE_RN_STRING_EXPECTED,
  SE_RN_VAR_EXPECTED,
  SE_RN_INDEX_OUT_OF_BOUND,
  SE_RN_CANNOT_FIND_WITH_GIVEN_INDEX,
  SE_RN_TOO_MANY_DIMENSIONS,
  SE_RN_RANK_OUT_OF_BOUND,
  SE_RN_INVALID_ID_USAGE,
  SE_RN_DUPLICATE_ID,
  SE_RN_INCOMPLETE_STRUCTURE,
  SE_RN_LABEL_NOT_EXISTS,
  SE_RN_NO_RETURN_POINT,
  SE_RN_COLON_EXPECTED,
  SE_RN_COMMA_EXPECTED,
  SE_RN_COMMA_OR_SEMICOLON_EXPECTED,
  SE_RN_OPEN_BRACKET_EXPECTED,
  SE_RN_CLOSE_BRACKET_EXPECTED,
  SE_RN_NESTED_TOO_MUCH,
  SE_RN_OPERATION_FAILED,
  SE_RN_OPERATOR_EXPECTED,
  SE_RN_ASSIGN_OPERATOR_EXPECTED,
  SE_RN_THEN_EXPECTED,
  SE_RN_ELSE_EXPECTED,
  SE_RN_ENDIF_EXPECTED,
  SE_RN_TO_EXPECTED,
  SE_RN_NEXT_EXPECTED,
  SE_RN_UNTIL_EXPECTED,
  SE_RN_LOOP_VAR_EXPECTED,
  SE_RN_JUMP_LABEL_EXPECTED,
  SE_RN_CALCULATION_ERROR,
  SE_RN_INVALID_EXPRESSION,
  SE_RN_DIVIDE_BY_ZERO,
  SE_RN_WRONG_FUNCTION_REACHED,
  SE_RN_CANNOT_SUSPEND_HERE,
  SE_RN_CANNOT_MIX_INSTRUCTIONAL_AND_STRUCTURED,
  SE_RN_INVALID_ROUTINE,
  SE_RN_ROUTINE_EXPECTED,
  SE_RN_DUPLICATE_ROUTINE,
  SE_RN_INVALID_CLASS,
  SE_RN_CLASS_EXPECTED,
  SE_RN_DUPLICATE_CLASS,
  SE_RN_HASH_AND_COMPARE_MUST_BE_PROVIDED_TOGETHER,
  SE_RN_INVALID_LAMBDA,
  SE_RN_EMPTY_COLLECTION,
  SE_RN_LIST_EXPECTED,
  SE_RN_INVALID_ITERATOR,
  SE_RN_ITERABLE_EXPECTED,
  SE_RN_COLLECTION_EXPECTED,
  SE_RN_COLLECTION_OR_ITERATOR_EXPECTED,
  SE_RN_REFERENCED_TYPE_EXPECTED,
  /** Extended abort */
  SE_EA_EXTENDED_ABORT,
  /** Extra */
  SE_COUNT
} mb_error_e;

typedef enum mb_data_e {
  MB_DT_NIL = 0,
  MB_DT_UNKNOWN = 1 << 0,
  MB_DT_INT = 1 << 1,
  MB_DT_REAL = 1 << 2,
  MB_DT_NUM = MB_DT_INT | MB_DT_REAL,
  MB_DT_STRING = 1 << 3,
  MB_DT_TYPE = 1 << 4,
  MB_DT_USERTYPE = 1 << 5,
#ifdef MB_ENABLE_USERTYPE_REF
  MB_DT_USERTYPE_REF = 1 << 6,
#endif /* MB_ENABLE_USERTYPE_REF */
  MB_DT_ARRAY = 1 << 7,
#ifdef MB_ENABLE_COLLECTION_LIB
  MB_DT_LIST = 1 << 8,
  MB_DT_LIST_IT = 1 << 9,
  MB_DT_DICT = 1 << 10,
  MB_DT_DICT_IT = 1 << 11,
  MB_DT_COLLECTION = MB_DT_LIST | MB_DT_DICT,
  MB_DT_ITERATOR = MB_DT_LIST_IT | MB_DT_DICT_IT,
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
  MB_DT_CLASS = 1 << 12,
#endif /* MB_ENABLE_CLASS */
  MB_DT_ROUTINE = 1 << 13
} mb_data_e;

typedef enum mb_meta_func_e {
  MB_MF_IS = 1 << 0,
  MB_MF_ADD = 1 << 1,
  MB_MF_SUB = 1 << 2,
  MB_MF_MUL = 1 << 3,
  MB_MF_DIV = 1 << 4,
  MB_MF_NEG = 1 << 5,
  MB_MF_CALC = MB_MF_IS | MB_MF_ADD | MB_MF_SUB | MB_MF_MUL | MB_MF_DIV | MB_MF_NEG,
  MB_MF_COLL = 1 << 6,
  MB_MF_FUNC = 1 << 7
} mb_meta_func_e;

typedef enum mb_meta_status_e {
  MB_MS_NONE = 0,
  MB_MS_DONE = 1 << 0,
  MB_MS_RETURNED = 1 << 1
} mb_meta_status_e;

typedef enum mb_routine_type_e {
  MB_RT_NONE,
  MB_RT_SCRIPT,
  MB_RT_LAMBDA,
  MB_RT_NATIVE
} mb_routine_type_e;

typedef unsigned char mb_val_bytes_t[mb_bytes_size];

typedef union mb_value_u {
  int_t integer;
  real_t float_point;
  char* string;
  mb_data_e type;
  void* usertype;
#ifdef MB_ENABLE_USERTYPE_REF
  void* usertype_ref;
#endif /* MB_ENABLE_USERTYPE_REF */
  void* array;
#ifdef MB_ENABLE_COLLECTION_LIB
  void* list;
  void* list_it;
  void* dict;
  void* dict_it;
#endif /* MB_ENABLE_COLLECTION_LIB */
#ifdef MB_ENABLE_CLASS
  void* instance;
#endif /* MB_ENABLE_CLASS */
  void* routine;
  mb_val_bytes_t bytes;
} mb_value_u;

mb_static_assert(sizeof(mb_val_bytes_t) >= sizeof(mb_value_u));

typedef struct mb_value_t {
  mb_data_e type;
  mb_value_u value;
} mb_value_t;

typedef int (* mb_func_t)(struct mb_interpreter_t*, void**);
typedef int (* mb_has_routine_arg_func_t)(struct mb_interpreter_t*, void**, mb_value_t*, unsigned, unsigned*, void*);
typedef int (* mb_pop_routine_arg_func_t)(struct mb_interpreter_t*, void**, mb_value_t*, unsigned, unsigned*, void*, mb_value_t*);
typedef int (* mb_routine_func_t)(struct mb_interpreter_t*, void**, mb_value_t*, unsigned, void*, mb_has_routine_arg_func_t, mb_pop_routine_arg_func_t);
typedef int (* mb_debug_stepped_handler_t)(struct mb_interpreter_t*, void**, const char*, int, unsigned short, unsigned short);
typedef void (* mb_error_handler_t)(struct mb_interpreter_t*, mb_error_e, const char*, const char*, int, unsigned short, unsigned short, int);
typedef int (* mb_print_func_t)(const char*, ...);
typedef int (* mb_input_func_t)(const char*, char*, int);
typedef int (* mb_import_handler_t)(struct mb_interpreter_t*, const char*);
typedef void (* mb_dtor_func_t)(struct mb_interpreter_t*, void*);
typedef void* (* mb_clone_func_t)(struct mb_interpreter_t*, void*);
typedef unsigned (* mb_hash_func_t)(struct mb_interpreter_t*, void*);
typedef int (* mb_cmp_func_t)(struct mb_interpreter_t*, void*, void*);
typedef int (* mb_fmt_func_t)(struct mb_interpreter_t*, void*, char*, unsigned);
typedef void (* mb_alive_marker_t)(struct mb_interpreter_t*, void*, mb_value_t);
typedef void (* mb_alive_checker_t)(struct mb_interpreter_t*, void*, mb_alive_marker_t);
typedef void (* mb_alive_value_checker_t)(struct mb_interpreter_t*, void*, mb_value_t, mb_alive_marker_t);
typedef int (* mb_meta_operator_t)(struct mb_interpreter_t*, void**, mb_value_t*, mb_value_t*, mb_value_t*);
typedef mb_meta_status_e (* mb_meta_func_t)(struct mb_interpreter_t*, void**, mb_value_t*, const char*);
typedef char* (* mb_memory_allocate_func_t)(unsigned);
typedef void (* mb_memory_free_func_t)(char*);

MBAPI unsigned long mb_ver(void);
MBAPI const char* mb_ver_string(void);

MBAPI int mb_init(void);
MBAPI int mb_dispose(void);
MBAPI int mb_open(struct mb_interpreter_t** s);
MBAPI int mb_open_child(struct mb_interpreter_t** s, struct mb_interpreter_t** parent);
MBAPI int mb_close(struct mb_interpreter_t** s);
MBAPI int mb_reset(struct mb_interpreter_t** s, bool_t clrf/* = false*/);
MBAPI int mb_reset_preserve(struct mb_interpreter_t** s, bool_t clrf/* = false*/);


MBAPI int mb_fork(struct mb_interpreter_t** s, struct mb_interpreter_t* r, bool_t clfk/* = true*/);
MBAPI int mb_join(struct mb_interpreter_t** s);
MBAPI int mb_get_forked_from(struct mb_interpreter_t* s, struct mb_interpreter_t** src);

MBAPI int mb_register_func(struct mb_interpreter_t* s, const char* n, mb_func_t f);
MBAPI int mb_remove_func(struct mb_interpreter_t* s, const char* n);
MBAPI int mb_remove_reserved_func(struct mb_interpreter_t* s, const char* n);
MBAPI int mb_begin_module(struct mb_interpreter_t* s, const char* n);
MBAPI int mb_end_module(struct mb_interpreter_t* s);

MBAPI int mb_attempt_func_begin(struct mb_interpreter_t* s, void** l);
MBAPI int mb_attempt_func_end(struct mb_interpreter_t* s, void** l);
MBAPI int mb_attempt_open_bracket(struct mb_interpreter_t* s, void** l);
MBAPI int mb_attempt_close_bracket(struct mb_interpreter_t* s, void** l);
MBAPI int mb_has_arg(struct mb_interpreter_t* s, void** l);
MBAPI int mb_pop_int(struct mb_interpreter_t* s, void** l, int_t* val);
MBAPI int mb_pop_real(struct mb_interpreter_t* s, void** l, real_t* val);
MBAPI int mb_pop_string(struct mb_interpreter_t* s, void** l, char** val);
MBAPI int mb_pop_usertype(struct mb_interpreter_t* s, void** l, void** val);
MBAPI int mb_pop_value(struct mb_interpreter_t* s, void** l, mb_value_t* val);
MBAPI int mb_push_int(struct mb_interpreter_t* s, void** l, int_t val);
MBAPI int mb_push_real(struct mb_interpreter_t* s, void** l, real_t val);
MBAPI int mb_push_string(struct mb_interpreter_t* s, void** l, char* val);
MBAPI int mb_push_usertype(struct mb_interpreter_t* s, void** l, void* val);
MBAPI int mb_push_value(struct mb_interpreter_t* s, void** l, mb_value_t val);

MBAPI int mb_begin_class(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t** meta, int c, mb_value_t* out);
MBAPI int mb_end_class(struct mb_interpreter_t* s, void** l);
MBAPI int mb_get_class_userdata(struct mb_interpreter_t* s, void** l, void** d);
MBAPI int mb_set_class_userdata(struct mb_interpreter_t* s, void** l, void* d);

MBAPI int mb_get_value_by_name(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t* val);
MBAPI int mb_add_var(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t val, bool_t force);
MBAPI int mb_get_var(struct mb_interpreter_t* s, void** l, void** v, bool_t redir);
MBAPI int mb_get_var_name(struct mb_interpreter_t* s, void* v, char** n);
MBAPI int mb_get_var_value(struct mb_interpreter_t* s, void* v, mb_value_t* val);
MBAPI int mb_set_var_value(struct mb_interpreter_t* s, void* v, mb_value_t val);
MBAPI int mb_init_array(struct mb_interpreter_t* s, void** l, mb_data_e t, int* d, int c, void** a);
MBAPI int mb_get_array_len(struct mb_interpreter_t* s, void** l, void* a, int r, int* i);
MBAPI int mb_get_array_elem(struct mb_interpreter_t* s, void** l, void* a, int* d, int c, mb_value_t* val);
MBAPI int mb_set_array_elem(struct mb_interpreter_t* s, void** l, void* a, int* d, int c, mb_value_t val);
MBAPI int mb_init_coll(struct mb_interpreter_t* s, void** l, mb_value_t* coll);
MBAPI int mb_get_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t idx, mb_value_t* val);
MBAPI int mb_set_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t idx, mb_value_t val);
MBAPI int mb_remove_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t idx);
MBAPI int mb_count_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, int* c);
MBAPI int mb_keys_of_coll(struct mb_interpreter_t* s, void** l, mb_value_t coll, mb_value_t* keys, int c);
MBAPI int mb_make_ref_value(struct mb_interpreter_t* s, void* val, mb_value_t* out, mb_dtor_func_t un, mb_clone_func_t cl, mb_hash_func_t hs/* = NULL*/, mb_cmp_func_t cp/* = NULL*/, mb_fmt_func_t ft/* = NULL*/);
MBAPI int mb_get_ref_value(struct mb_interpreter_t* s, void** l, mb_value_t val, void** out);
MBAPI int mb_ref_value(struct mb_interpreter_t* s, void** l, mb_value_t val);
MBAPI int mb_unref_value(struct mb_interpreter_t* s, void** l, mb_value_t val);
MBAPI int mb_set_alive_checker(struct mb_interpreter_t* s, mb_alive_checker_t f);
MBAPI int mb_set_alive_checker_of_value(struct mb_interpreter_t* s, void** l, mb_value_t val, mb_alive_value_checker_t f);
MBAPI int mb_override_value(struct mb_interpreter_t* s, void** l, mb_value_t val, mb_meta_func_e m, void* f);
MBAPI int mb_dispose_value(struct mb_interpreter_t* s, mb_value_t val);

MBAPI int mb_get_routine(struct mb_interpreter_t* s, void** l, const char* n, mb_value_t* val);
MBAPI int mb_set_routine(struct mb_interpreter_t* s, void** l, const char* n, mb_routine_func_t f, bool_t force);
MBAPI int mb_eval_routine(struct mb_interpreter_t* s, void** l, mb_value_t val, mb_value_t* args, unsigned argc, mb_value_t* ret/* = NULL*/);
MBAPI int mb_get_routine_type(struct mb_interpreter_t* s, mb_value_t val, mb_routine_type_e* y);

MBAPI int mb_load_string(struct mb_interpreter_t* s, const char* l, bool_t reset/* = true*/);
MBAPI int mb_load_file(struct mb_interpreter_t* s, const char* f);
MBAPI int mb_run(struct mb_interpreter_t* s, bool_t clear_parser/* = true*/);
MBAPI int mb_suspend(struct mb_interpreter_t* s, void** l);
MBAPI int mb_schedule_suspend(struct mb_interpreter_t* s, int t);

MBAPI int mb_debug_get(struct mb_interpreter_t* s, const char* n, mb_value_t* val);
MBAPI int mb_debug_set(struct mb_interpreter_t* s, const char* n, mb_value_t val);
MBAPI int mb_debug_get_stack_trace(struct mb_interpreter_t* s, void** l, char** fs, unsigned fc);
MBAPI int mb_debug_set_stepped_handler(struct mb_interpreter_t* s, mb_debug_stepped_handler_t h);

MBAPI const char* mb_get_type_string(mb_data_e t);

MBAPI int mb_raise_error(struct mb_interpreter_t* s, void** l, mb_error_e err, int ret);
MBAPI mb_error_e mb_get_last_error(struct mb_interpreter_t* s, const char** file, int* pos, unsigned short* row, unsigned short* col);
MBAPI const char* mb_get_error_desc(mb_error_e err);
MBAPI int mb_set_error_handler(struct mb_interpreter_t* s, mb_error_handler_t h);

MBAPI int mb_set_printer(struct mb_interpreter_t* s, mb_print_func_t p);
MBAPI int mb_set_inputer(struct mb_interpreter_t* s, mb_input_func_t p);


MBAPI struct mb_interpreter_t* mb_get_parent(struct mb_interpreter_t* s);
MBAPI int mb_set_yield(struct mb_interpreter_t * s, void (*f)(struct mb_interpreter_t *));

MBAPI int mb_set_import_handler(struct mb_interpreter_t* s, mb_import_handler_t h);
MBAPI int mb_set_memory_manager(mb_memory_allocate_func_t a, mb_memory_free_func_t f);
MBAPI bool_t mb_get_gc_enabled(struct mb_interpreter_t* s);
MBAPI int mb_set_gc_enabled(struct mb_interpreter_t* s, bool_t gc);
MBAPI int mb_gc(struct mb_interpreter_t* s, int_t* collected/* = NULL*/);
MBAPI int mb_get_userdata(struct mb_interpreter_t* s, void** d);
MBAPI int mb_get_userdata(struct mb_interpreter_t* s, void** d);
MBAPI int mb_get_suspent(struct mb_interpreter_t* s, void** d);
MBAPI int mb_set_userdata(struct mb_interpreter_t* s, void* d);
MBAPI int mb_gets(const char* pmt, char* buf, int s);
MBAPI char* mb_memdup(const char* val, unsigned size);

#ifdef MB_COMPACT_MODE
# pragma pack()
#endif /* MB_COMPACT_MODE */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MY_BASIC_H__ */
