#ifndef __JSON_H__
#define __JSON_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
void print_trace();
// --------------------------------------------------
//                  JSON TOKEN
// --------------------------------------------------
enum json_token_type_t {
    JT_MISSING,
    JT_EMPTY,
    JT_STRING,
    JT_NUMBER,
    JT_BOOL,
    JT_NULL,
    JT_INT,
    JT_UINT,
    JT_FLOAT,
    JT_DOUBLE,
    JT_VALUE,
    JT_OBJECT,
    JT_ARRAY,
    JT_TOKEN_SIZE,
};

struct json_tok_t {
    enum json_token_type_t type;
    char *text;
    union {
        bool boolean;
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        float f32;
        double f64;
    };
};

struct json_obj_t {
    enum json_token_type_t type;
    void *pairs;
};

struct json_arr_t {
    enum json_token_type_t type;
    void *values;
};

union json_t {
    enum json_token_type_t type;
    struct json_tok_t tok;
    struct json_obj_t obj;
    struct json_arr_t arr;
};

struct json_pair_t {
    char *key;
    union json_t value;
};

#define JSON(x) ((union json_t)(x))
#define JSON_TOK(x) ((union json_t){.tok = (x)})
#define JSON_OBJ(x) ((union json_t){.obj = (x)})
#define JSON_ARR(x) ((union json_t){.arr = (x)})
#define JSON_ARRAY ((union json_t){.arr = {.type = JT_ARRAY}})
#define JSON_OBJECT ((union json_t){.obj = {.type = JT_OBJECT}})
#define JSON_NULL ((union json_t){.tok = {.type = JT_NULL}})
#define JSON_EMPTY ((union json_t){.tok = {.type = JT_EMPTY}})
#define JSON_TRUE ((union json_t){.tok = {.type = JT_BOOL, .boolean = true}})
#define JSON_FALSE ((union json_t){.tok = {.type = JT_BOOL, .boolean = false}})
#define JSON_STR(x) ((union json_t){.tok = {.type = JT_STRING, .text = (x)}})
#define JSON_NUM(x) ((union json_t){.tok = {.type = JT_NUMBER, .text = (x)}})
#define JSON_BOOL(x) ((union json_t){.tok = {.type = JT_BOOL, .boolean = (x)}})
#define JSON_INT(x) ((union json_t){.tok = {.type = JT_INT, .i64 = (int64_t)(x)}})
#define JSON_UINT(x) ((union json_t){.tok = {.type = JT_UINT, .u64 = (uint64_t)(x)}})
#define JSON_INT8(x) ((union json_t){.tok = {.type = JT_INT8, .i8 = (x)}})
#define JSON_INT16(x) ((union json_t){.tok = {.type = JT_INT16, .i16 = (x)}})
#define JSON_INT32(x) ((union json_t){.tok = {.type = JT_INT32, .i32 = (x)}})
#define JSON_INT64(x) ((union json_t){.tok = {.type = JT_INT64, .i64 = (x)}})
#define JSON_UINT8(x) ((union json_t){.tok = {.type = JT_UINT8, .u8 = (x)}})
#define JSON_UINT16(x) ((union json_t){.tok = {.type = JT_UINT16, .u16 = (x)}})
#define JSON_UINT32(x) ((union json_t){.tok = {.type = JT_UINT32, .u32 = (x)}})
#define JSON_UINT64(x) ((union json_t){.tok = {.type = JT_UINT64, .u64 = (x)}})
#define JSON_FLOAT(x) ((union json_t){.tok = {.type = JT_FLOAT, .f32 = (x)}})
#define JSON_DOUBLE(x) ((union json_t){.tok = {.type = JT_DOUBLE, .f64 = (x)}})

const char *json_type2str(enum json_token_type_t type);

// --------------------------------------------------
//     JSON UNDER LAYER DATA STRUCTURE EXTENSION
// --------------------------------------------------

typedef void (*json_obj_iter_cb)(struct json_pair_t *pair, void *args);
typedef void (*json_arr_iter_cb)(size_t index, union json_t *value, void *args);

void jsonext_obj_new(union json_t *j, size_t capacity);
void jsonext_arr_new(union json_t *j, size_t capacity);

void jsonext_obj_insert(union json_t *j, struct json_pair_t *pair);
void jsonext_arr_append(union json_t *j, union json_t *value);

struct json_pair_t *jsonext_obj_get(union json_t *j, const char *key);
union json_t *jsonext_arr_get(union json_t *j, size_t index);

struct json_pair_t *jsonext_obj_delete(union json_t *j, const char *key);
union json_t *jsonext_arr_delete(union json_t *j, size_t index);

void jsonext_obj_clean(union json_t *j);
void jsonext_arr_clean(union json_t *j);

size_t jsonext_obj_length(union json_t *j);
size_t jsonext_arr_length(union json_t *j);

size_t jsonext_obj_capacity(union json_t *j);
size_t jsonext_arr_capacity(union json_t *j);

void jsonext_obj_iter(union json_t *j, json_obj_iter_cb f, void *fargs);
struct json_pair_t *jsonext_obj_iter_first(union json_t *j);
struct json_pair_t *jsonext_obj_iter_next(union json_t *j, struct json_pair_t *pair);

// --------------------------------------------------
//                JSON COMMON FUNCTION
// --------------------------------------------------

union json_t json_dup(union json_t t);

#define json_get(j, key) _Generic((key), char *: __json_get_from_obj, default: __json_get_from_arr)((j), (key))

#define json_del(j, key) _Generic((key), char *: __json_delete_from_obj, default: __json_delete_from_arr)((j), (key))

size_t json_length(union json_t j);
size_t json_capacity(union json_t j);

// TODO json_assign
// TODO void json_dump(char *s, size_t n);
void json_print(union json_t j);
void json_clean(union json_t *j);

// --------------------------------------------------
//                JSON OBJECT FUNCTION
// --------------------------------------------------
union json_t json_create_obj(size_t capacity);

union json_t *__json_get_from_obj(union json_t j, const char *key);
void __json_delete_from_obj(union json_t *j, const char *key);

/*
 * Booling type is compatibale i32, it will not goto bool option
*/
#define json_set(j, key, value)                                                                                        \
    _Generic((value),                                                                                                  \
        char *: __json_set_str,                                                                                        \
        bool: __json_set_bool,                                                                                         \
        int8_t: __json_set_i8,                                                                                         \
        int16_t: __json_set_i16,                                                                                       \
        int32_t: __json_set_i32,                                                                                       \
        int64_t: __json_set_i64,                                                                                       \
        uint8_t: __json_set_u8,                                                                                        \
        uint16_t: __json_set_u16,                                                                                      \
        uint32_t: __json_set_u32,                                                                                      \
        uint64_t: __json_set_u64,                                                                                      \
        float: __json_set_f32,                                                                                         \
        double: __json_set_f64,                                                                                        \
        struct json_obj_t: __json_set_obj,                                                                             \
        struct json_obj_t *: __json_set_obj_p,                                                                         \
        struct json_arr_t: __json_set_arr,                                                                             \
        struct json_arr_t *: __json_set_arr_p,                                                                         \
        struct json_tok_t: __json_set_token,                                                                           \
        struct json_tok_t *: __json_set_token_p,                                                                       \
        union json_t: __json_set_value,                                                                                \
        union json_t *: __json_set_value_p,                                                                            \
        default: __json_set_value)((j), (key), (value))

int __json_set_str(union json_t *j, const char *key, const char *value);
int __json_set_bool(union json_t *j, const char *key, bool value);
int __json_set_null(union json_t *j, const char *key, void *value);
int __json_set_i8(union json_t *j, const char *key, int8_t value);
int __json_set_i16(union json_t *j, const char *key, int16_t value);
int __json_set_i32(union json_t *j, const char *key, int32_t value);
int __json_set_i64(union json_t *j, const char *key, int64_t value);
int __json_set_u8(union json_t *j, const char *key, uint8_t value);
int __json_set_u16(union json_t *j, const char *key, uint16_t value);
int __json_set_u32(union json_t *j, const char *key, uint32_t value);
int __json_set_u64(union json_t *j, const char *key, uint64_t value);
int __json_set_f32(union json_t *j, const char *key, float value);
int __json_set_f64(union json_t *j, const char *key, double value);
int __json_set_obj(union json_t *j, const char *key, struct json_obj_t value);
int __json_set_obj_p(union json_t *j, const char *key, struct json_obj_t *value);
int __json_set_arr(union json_t *j, const char *key, struct json_arr_t value);
int __json_set_arr_p(union json_t *j, const char *key, struct json_arr_t *value);
int __json_set_token(union json_t *j, const char *key, struct json_tok_t value);
int __json_set_token_p(union json_t *j, const char *key, struct json_tok_t *value);
int __json_set_value(union json_t *j, const char *key, union json_t value);
int __json_set_value_p(union json_t *j, const char *key, union json_t *value);

// --------------------------------------------------
//                JSON ARRAY FUNCTION
// --------------------------------------------------
union json_t json_create_arr(size_t capacity);

union json_t *__json_get_from_arr(union json_t j, size_t i);
void __json_delete_from_arr(union json_t *j, size_t i);

#define json_append(j, value)                                                                                          \
    _Generic((value),                                                                                                  \
        char *: __json_append_str,                                                                                     \
        bool: __json_append_bool,                                                                                      \
        int8_t: __json_append_i8,                                                                                      \
        int16_t: __json_append_i16,                                                                                    \
        int32_t: __json_append_i32,                                                                                    \
        int64_t: __json_append_i64,                                                                                    \
        uint8_t: __json_append_u8,                                                                                     \
        uint16_t: __json_append_u16,                                                                                   \
        uint32_t: __json_append_u32,                                                                                   \
        uint64_t: __json_append_u64,                                                                                   \
        float: __json_append_f32,                                                                                      \
        double: __json_append_f64,                                                                                     \
        struct json_obj_t: __json_append_obj,                                                                          \
        struct json_obj_t *: __json_append_obj_p,                                                                      \
        struct json_arr_t: __json_append_arr,                                                                          \
        struct json_arr_t *: __json_append_arr_p,                                                                      \
        struct json_tok_t: __json_append_token,                                                                        \
        struct json_tok_t *: __json_append_token_p,                                                                    \
        union json_t: __json_append_value,                                                                             \
        union json_t *: __json_append_value_p,                                                                         \
        default: __json_append_value)((j), (value))

int __json_append_str(union json_t *j, const char *value);
int __json_append_bool(union json_t *j, bool value);
int __json_append_null(union json_t *j, void *value);
int __json_append_i8(union json_t *j, int8_t value);
int __json_append_i16(union json_t *j, int16_t value);
int __json_append_i32(union json_t *j, int32_t value);
int __json_append_i64(union json_t *j, int64_t value);
int __json_append_u8(union json_t *j, uint8_t value);
int __json_append_u16(union json_t *j, uint16_t value);
int __json_append_u32(union json_t *j, uint32_t value);
int __json_append_u64(union json_t *j, uint64_t value);
int __json_append_f32(union json_t *j, float value);
int __json_append_f64(union json_t *j, double value);
int __json_append_obj(union json_t *j, struct json_obj_t value);
int __json_append_obj_p(union json_t *j, struct json_obj_t *value);
int __json_append_arr(union json_t *j, struct json_arr_t value);
int __json_append_arr_p(union json_t *j, struct json_arr_t *value);
int __json_append_token(union json_t *j, struct json_tok_t value);
int __json_append_token_p(union json_t *j, struct json_tok_t *value);
int __json_append_value(union json_t *j, union json_t value);
int __json_append_value_p(union json_t *j, union json_t *value);

// --------------------------------------------------
//                    JSON Lexer
// --------------------------------------------------
enum json_lexer_token_type_t {
    JLT_MISSING,
    JLT_STRING,
    JLT_NUMBER,
    JLT_COMMA,
    JLT_COLON,
    JLT_LPAIR,
    JLT_RPAIR,
    JLT_LARRAY,
    JLT_RARRAY,
    JLT_TRUE,
    JLT_FALSE,
    JLT_NULL,
    JLT_LEXER_TOKEN_SIZE
};

struct json_lexer_token_t {
    enum json_lexer_token_type_t type;
    char *text;
    unsigned int index;
    unsigned int start;
    unsigned int end;
    unsigned int column;
    unsigned int row;
};

struct json_lexer_container_t {
    unsigned int length;
    unsigned int capacity;
    struct json_lexer_token_t *list;
};

struct json_lexer_context_t {
    struct json_lexer_container_t tokens;
    int currentChar;
    unsigned int offset;
    unsigned int column;
    unsigned int row;
    const char *text;
    size_t length;
    FILE *stream;
};

struct json_lexer_context_t *json_create_lexer(FILE *stream);
void json_free_lexer(struct json_lexer_context_t *ctx);
void json_execute_lexer(struct json_lexer_context_t *ctx);
void json_print_lexer(struct json_lexer_context_t *ctx);
// --------------------------------------------------
//                END JSON Lexer
// --------------------------------------------------

// --------------------------------------------------
//                  JSON Parser
// --------------------------------------------------
struct ParserContext {
    unsigned int tokenIndex;
    struct LexerContext *lexer;
    union valueToken *root;
};

union valueToken json_string(const char *input_text);
union valueToken json_file(const char *file_path);
union valueToken json_FILE(FILE *f);

struct ParserContext *json_create_parser(struct LexerContext *lexer_ctx);
void json_free_parser(struct ParserContext *parser_ctx);
void json_execute_lexer(struct json_lexer_context_t *ctx);
void json_print_parser(struct ParserContext *parser_ctx);
// --------------------------------------------------
//                  END JSON Parser
// --------------------------------------------------
#endif /* __JSON_H__ */
