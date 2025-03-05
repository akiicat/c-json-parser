#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

enum json_type_t {
    T_MISSING,

    T_EMPTY,

    T_STRING,
    T_NUMBER,
    T_BOOL,
    T_NULL,

    T_INT8,
    T_INT16,
    T_INT32,
    T_INT64,

    T_UINT8,
    T_UINT16,
    T_UINT32,
    T_UINT64,

    T_FLOAT,
    T_DOUBLE,

    VALUE,
    OBJECT,
    ARRAY,

    TOKEN_SIZE
};

// __json_type_t
// struct json_tok_t;
// struct json_obj_t;
// struct json_arr_t;
// struct json_pair_t;
// union json_value_t;

struct json_tok_t {
    enum json_type_t type;
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
    enum json_type_t type;
    struct json_pairs_t *pairs;
};

struct json_arr_t {
    enum json_type_t type;
    struct json_values_t *values;
};

union json_value_t {
    enum json_type_t type;
    struct json_tok_t tok;
    struct json_obj_t obj;
    struct json_arr_t arr;
};

struct json_values_t {
    size_t length;
    size_t capacity;
    union json_value_t list[];
};

struct json_pair_t {
    char *key;
    union json_value_t value;
};

struct json_pairs_t {
    size_t length;
    size_t capacity;
    struct json_pair_t list[];
};


#endif